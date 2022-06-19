#include "socket/ListenSocket.hpp"

#include "socket/ClientSocket.hpp"
#include "utils/syscall_wrapper.hpp"
#include "utils/utils.hpp"

ListenSocket::ListenSocket(int                          passive_socket,
                           std::map<int, SocketBase *> &socket_map)
    : SocketBase(passive_socket)
    , _socket_map_(socket_map) {}

struct pollfd ListenSocket::pollfd() {
  return (struct pollfd){_socket_fd_, POLLIN, 0};
}

void ListenSocket::handle_event(short int revents) {
  if ((revents & POLLIN) == 0)
    return;
  connFd conn_fd = xaccept(_socket_fd_);
  LOG("insert " << conn_fd << " to connection");
  // insert new connection to socket map
  SocketBase *new_client =
      new ClientSocket(conn_fd, _conf_group_, _socket_map_);
  _socket_map_.insert(std::make_pair(conn_fd, new_client));
}
