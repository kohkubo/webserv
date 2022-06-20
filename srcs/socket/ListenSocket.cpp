#include "socket/ListenSocket.hpp"

#include "socket/ClientSocket.hpp"
#include "socket/SocketMapAction.hpp"
#include "utils/syscall_wrapper.hpp"
#include "utils/utils.hpp"

ListenSocket::ListenSocket(int passive_socket, const Config *config)
    : SocketBase(passive_socket) {
  conf_group_.push_back(config);
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
