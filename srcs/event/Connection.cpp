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
    Transaction &transaction = __transaction_queue_.back();
    transaction.handle_request(__buffer_, __conf_group_);
    std::cout << __buffer_ << std::endl;
    if (transaction.state() != SENDING) {
      break;
    }
    __transaction_queue_.push_back(Transaction());
  }
  __last_event_time_ = __time_now();
}

bool Connection::append_receive_buffer() {
  const int buf_size = 2048;
  char      buf[buf_size] = {0};
  // std::vector<char> buf(buf_size);
  ssize_t   rc = recv(__conn_fd_, buf, buf_size, MSG_DONTWAIT);
  if (rc == -1) {
    std::cerr << "recv() failed." << std::endl;
    exit(EXIT_FAILURE);
  }
  // fin from client
  if (rc == 0) {
    return true;
  }
  std::cout << "append_receive_buffer()" << std::endl;
  std::cout << "rc: " << rc << std::endl;
  write(0, buf, rc);
  std::cout << std::endl;
  // std::string recv_data = std::string(buf.begin(), buf.begin() + rc);
  __buffer_.append(buf, rc);
  return false;
}

struct pollfd Connection::create_pollfd() const {
  struct pollfd pfd = {__conn_fd_, POLLIN, 0};
  if (__transaction_queue_.front().state() == SENDING) {
    pfd.events = POLLIN | POLLOUT;
  }
  return pfd;
}

void Connection::send_response() {
  Transaction &transaction = __transaction_queue_.front();
  transaction.send_response(__conn_fd_);
  if (transaction.request_info().connection_close_) {
    shutdown(__conn_fd_, SHUT_WR);
    __transaction_queue_.front().set_state(CLOSING);
    return;
  }
  if (transaction.is_send_all()) {
    __transaction_queue_.pop_front();
  }
  __last_event_time_ = __time_now();
}
