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
  while (true) {
    std::string response;
    try {
      RequestState request_state =
          __receive_request_.handle_request(__buffer_, __conf_group_);
      if (request_state != SUCCESS) {
        break;
      }
      response = ResponseGenerator::generate_response(
          __receive_request_.request_info());
    } catch (const RequestInfo::BadRequestException &e) {
      response = ResponseGenerator::generate_bad_response();
    }
    __response_queue_.push_back(__receive_request_.create_response());
    __receive_request_ = ReceiveRequest();
  }
  __last_event_time_ = __time_now();
}

bool Connection::append_receive_buffer() {
  const int buf_size      = 2048;
  char      buf[buf_size] = {0};
  ssize_t   rc            = recv(__conn_fd_, buf, buf_size, MSG_DONTWAIT);
  if (rc == -1) {
    ERROR_EXIT("recv() failed.");
  }
  // fin from client
  if (rc == 0) {
    return true;
  }
  __buffer_.append(buf, rc);
  return false;
}

struct pollfd Connection::create_pollfd() const {
  struct pollfd pfd = {__conn_fd_, POLLIN, 0};
  if (!__response_queue_.empty() && __response_queue_.front().is_sending()) {
    pfd.events = POLLIN | POLLOUT;
  }
  return pfd;
}

void Connection::send_front_response() {
  ResponseState response_state = __response_queue_.front().send(__conn_fd_);
  if (response_state == COMPLETE) {
    __response_queue_.pop_front();
  }
  __last_event_time_ = __time_now();
}
