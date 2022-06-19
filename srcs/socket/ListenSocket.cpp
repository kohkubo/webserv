#include "socket/ListenSocket.hpp"

#include "utils/syscall_wrapper.hpp"
#include "utils/utils.hpp"

ListenSocket::ListenSocket(const std::map<int, SocketBase *> &parent)
    : _parent_(parent) {}

struct pollfd ListenSocket::pollfd() {
  return (struct pollfd){_socket_fd_, POLLIN, 0};
}

void ListenSocket::handle_event(short int revents) {
  if ((revents & POLLIN) == 0)
    return;
  connFd conn_fd = xaccept(_socket_fd_);
  LOG("insert " << conn_fd << " to connection");
  // insert new connection to socket map
  // _parent_.insert(std::make_pair(
  //     conn_fd, Connection(conn_fd, _conf_group_map_[listen_fd])));
}