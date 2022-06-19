#include "socket/ClientSocket.hpp"

#include <poll.h>

ClientSocket::ClientSocket(int client_fd, const confGroup &conf_group)
    : SocketBase(client_fd)
    , _conf_group_(conf_group) {}

struct pollfd ClientSocket::pollfd() {
  struct pollfd pfd = {_socket_fd_, POLLIN, 0};
  // if (!_response_queue_.empty() && _response_queue_.front().is_sending()) {
  //   pfd.events = POLLIN | POLLOUT;
  // }
  return pfd;
}

void ClientSocket::handle_event(short int revents) {
  if ((revents & POLLIN) != 0) {
    LOG("got POLLIN  event of fd " << _socket_fd_);
    // recv buffer
    // _connection_receive_handler(_connection_map_.find(_socket_fd_)->second);
  }
  if ((revents & POLLOUT) != 0) {
    LOG("got POLLOUT event of fd " << _socket_fd_);
    // send response
    // _connection_map_.find(it->fd)->second.send_front_response();
  }
}