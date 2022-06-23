#include "socket/ClientSocket.hpp"

#include <poll.h>

ClientSocket::ClientSocket(int client_fd, const ConfigGroup &config_group)
    : SocketBase(client_fd)
    , _config_group_(config_group)
    , _timeout_(TIMEOUT_SECONDS_) {}

struct pollfd ClientSocket::pollfd() {
  struct pollfd pfd = {_socket_fd_, POLLIN, 0};
  if (!_response_queue_.empty() && _response_queue_.front().is_sending()) {
    pfd.events = POLLIN | POLLOUT;
  }
  return pfd;
}

bool ClientSocket::is_timed_out() { return _timeout_.is_timed_out(); }

SocketMapAction ClientSocket::handle_event(short int revents) {
  _timeout_.update_last_event();
  if ((revents & POLLIN) != 0) {
    LOG("got POLLIN  event of fd " << _socket_fd_);
    SocketMapAction socket_map_action = handle_receive_event();
    if (socket_map_action.type_ == SocketMapAction::DELETE)
      return socket_map_action;
  }
  if ((revents & POLLOUT) != 0) {
    LOG("got POLLOUT event of fd " << _socket_fd_);
    handle_send_event();
  }
  return SocketMapAction();
}

SocketMapAction ClientSocket::handle_receive_event() {
  bool is_socket_closed_from_client = append_receive_buffer();
  if (is_socket_closed_from_client) {
    LOG("got FIN from connection");
    return SocketMapAction(SocketMapAction::DELETE, _socket_fd_, this);
  }
  parse_buffer();
  return SocketMapAction();
}

void ClientSocket::handle_send_event() {
  Response::ResponseState response_state =
      _response_queue_.front().send(_socket_fd_);
  if (response_state == Response::COMPLETE) {
    _response_queue_.pop_front();
  }
}

void ClientSocket::parse_buffer() {
  try {
    for (;;) {
      Request::RequestState request_state =
          _request_.handle_request(_buffer_, _config_group_);
      if (request_state != Request::SUCCESS) {
        break;
      }
      _response_queue_.push_back(_request_.create_response());
      _request_ = Request();
    }
  } catch (const RequestInfo::BadRequestException &e) {
    _response_queue_.push_back(_request_.create_response(e.status()));
    _request_ = Request();
  }
}

bool ClientSocket::append_receive_buffer() {
  const int buf_size      = 2048;
  char      buf[buf_size] = {0};
  ssize_t   rc            = recv(_socket_fd_, buf, buf_size, MSG_DONTWAIT);
  if (rc == -1) {
    ERROR_EXIT("recv() failed.");
  }
  // fin from client
  if (rc == 0) {
    return true;
  }
  _buffer_.append(buf, rc);
  return false;
}
