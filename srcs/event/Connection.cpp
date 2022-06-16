#include "event/Connection.hpp"

#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>

#include "config/Config.hpp"
#include "http/const/const_delimiter.hpp"

void Connection::create_sequential_transaction() {
  for (;;) {
    RequestState request_state =
        _request_.handle_request(_buffer_, _conf_group_);
    if (request_state != SUCCESS) {
      break;
    }
    _response_queue_.push_back(_request_.create_response());
    _request_ = Request();
  }
  _last_event_time_ = _time_now();
}

bool Connection::append_receive_buffer() {
  const int buf_size      = 2048;
  char      buf[buf_size] = {0};
  ssize_t   rc            = recv(_conn_fd_, buf, buf_size, MSG_DONTWAIT);
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

struct pollfd Connection::create_pollfd() const {
  struct pollfd pfd = {_conn_fd_, POLLIN, 0};
  if (!_response_queue_.empty() && _response_queue_.front().is_sending()) {
    pfd.events = POLLIN | POLLOUT;
  }
  return pfd;
}

void Connection::send_front_response() {
  ResponseState response_state = _response_queue_.front().send(_conn_fd_);
  if (response_state == COMPLETE) {
    _response_queue_.pop_front();
  }
  _last_event_time_ = _time_now();
}
