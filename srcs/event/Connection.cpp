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
    __transaction_.handle_request(__buffer_, __conf_group_);
    if (__transaction_.state() != COMPLETE) {
      break;
    }
    __response_queue_.push_back(__transaction_.create_response());
    __transaction_ = Request();
  }
  __last_event_time_ = __time_now();
}

bool Connection::append_receive_buffer() {
  const int         buf_size = 2048;
  std::vector<char> buf(buf_size);
  ssize_t           rc = recv(__conn_fd_, &buf[0], buf_size, MSG_DONTWAIT);
  if (rc == -1) {
    std::cerr << "recv() failed." << std::endl;
    exit(EXIT_FAILURE);
  }
  // fin from client
  if (rc == 0) {
    return true;
  }
  std::string recv_data = std::string(buf.begin(), buf.begin() + rc);
  __buffer_.append(recv_data);
  return false;
}

struct pollfd Connection::create_pollfd() const {
  struct pollfd pfd = {__conn_fd_, POLLIN, 0};
  if (!__response_queue_.empty() &&
      __response_queue_.front().state() == SENDING) {
    pfd.events = POLLIN | POLLOUT;
  }
  return pfd;
}

void Connection::send_response() {
  Response &response = __response_queue_.front();
  response.send_response(__conn_fd_);
  if (response.is_send_all() && response.is_last_response()) {
    shutdown(__conn_fd_, SHUT_WR);
    response.set_state(CLOSING);
    return;
  }
  if (response.is_send_all()) {
    __response_queue_.pop_front();
  }
  __last_event_time_ = __time_now();
}
