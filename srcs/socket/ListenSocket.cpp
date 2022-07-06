#include "socket/ListenSocket.hpp"

#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdlib>

#include "config/Config.hpp"
#include "socket/ClientSocket.hpp"
#include "socket/SocketMapActions.hpp"
#include "utils/syscall_wrapper.hpp"
#include "utils/utils.hpp"

namespace ns_socket {

ListenSocket::ListenSocket(const config::ConfigGroup &config_group)
    : config_group_(config_group) {
  _socket_fd_ = _open_new_socket(config_group.sockaddr_in());
}

listenFd ListenSocket::_open_new_socket(struct sockaddr_in info) {
  listenFd listen_fd = _create_socket();
  _bind_address(listen_fd, info);
  _start_listen(listen_fd);
  return listen_fd;
}

listenFd ListenSocket::_create_socket() {
  listenFd listen_fd = socket(AF_INET, SOCK_STREAM, 6);
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

void ListenSocket::_bind_address(listenFd listen_fd, struct sockaddr_in info) {
  if (bind(listen_fd, (const struct sockaddr *)&info, sizeof(info)) == -1) {
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

SocketMapActions ListenSocket::handle_event(short int revents) {
  SocketMapActions socket_map_actions;
  if ((revents & POLLIN) != 0) {
    connFd conn_fd            = xaccept(_socket_fd_);
    // LOG("add new connection fd: " << conn_fd);
    SocketBase *client_socket = new ClientSocket(conn_fd, config_group_);

    socket_map_actions.add_action(SocketMapAction::INSERT, conn_fd,
                                  client_socket);
  }
  return socket_map_actions;
}

bool ListenSocket::is_timed_out() { return false; }

} // namespace ns_socket