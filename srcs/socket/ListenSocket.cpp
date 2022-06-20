#include "socket/ListenSocket.hpp"

#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdlib>

#include "config/Config.hpp"
#include "socket/ClientSocket.hpp"
#include "socket/SocketMapAction.hpp"
#include "utils/syscall_wrapper.hpp"
#include "utils/utils.hpp"

ListenSocket::ListenSocket(const Config *config) {
  _socket_fd_ = _open_new_socket(config->addrinfo_);
  conf_group_.push_back(config);
}

listenFd ListenSocket::_open_new_socket(struct addrinfo *info) {
  listenFd listen_fd = _create_socket(info);
  _bind_address(listen_fd, info);
  _start_listen(listen_fd);
  return listen_fd;
}

listenFd ListenSocket::_create_socket(struct addrinfo *info) {
  listenFd listen_fd =
      socket(info->ai_family, info->ai_socktype, info->ai_protocol);
  if (listen_fd == -1) {
    ERROR_EXIT_WITH_ERRNO("socket() failed.");
  }
  if (fcntl(listen_fd, F_SETFL, O_NONBLOCK) == -1) {
    ERROR_EXIT_WITH_ERRNO("fcntl() failed.");
  }
  int on = 1;
  if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&on,
                 sizeof(on)) == -1) {
    close(listen_fd);
    ERROR_EXIT_WITH_ERRNO("setsockopt() failed.");
  }
  return listen_fd;
}

void ListenSocket::_bind_address(listenFd listen_fd, struct addrinfo *info) {
  if (bind(listen_fd, info->ai_addr, info->ai_addrlen) == -1) {
    close(listen_fd);
    ERROR_EXIT_WITH_ERRNO("bind() failed.");
  }
}

void ListenSocket::_start_listen(listenFd listen_fd) {
  if (listen(listen_fd, SOMAXCONN) == -1) {
    close(listen_fd);
    ERROR_EXIT_WITH_ERRNO("listen() failed.");
  }
}

struct pollfd ListenSocket::pollfd() {
  return (struct pollfd){_socket_fd_, POLLIN, 0};
}

SocketMapAction ListenSocket::handle_event(short int revents) {
  if ((revents & POLLIN) != 0) {
    connFd conn_fd = xaccept(_socket_fd_);
    LOG("add new connection fd: " << conn_fd);
    SocketBase *client_socket = new ClientSocket(conn_fd, conf_group_);
    return SocketMapAction(SocketMapAction::INSERT, conn_fd, client_socket);
  }
  return SocketMapAction();
}

bool ListenSocket::is_timed_out() { return false; }
