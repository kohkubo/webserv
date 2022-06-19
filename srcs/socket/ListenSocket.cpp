#include "socket/ListenSocket.hpp"

#include "socket/ClientSocket.hpp"
#include "socket/SocketMapOp.hpp"
#include "utils/syscall_wrapper.hpp"
#include "utils/utils.hpp"

ListenSocket::ListenSocket(int passive_socket)
    : SocketBase(passive_socket) {}

struct pollfd ListenSocket::pollfd() {
  return (struct pollfd){_socket_fd_, POLLIN, 0};
}

SocketMapOp ListenSocket::handle_event(short int revents) {
  if ((revents & POLLIN) == 0)
    return SocketMapOp();
  connFd conn_fd = xaccept(_socket_fd_);
  LOG("insert " << conn_fd << " to connection");
  // insert new connection to socket map
  SocketBase *new_client = new ClientSocket(conn_fd, _conf_group_);
  return SocketMapOp(INSERT, new_client);
}
