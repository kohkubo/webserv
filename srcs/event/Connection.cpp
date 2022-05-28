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
  while (1) {
    Transaction &transaction = __transaction_queue_.back();
    try {
      transaction.handle_request(__buffer_);
      if (transaction.get_transaction_state() != SENDING) {
        return;
      }
      const Config *config = transaction.get_proper_config(__conf_group_);
      transaction.create_response(config);
    } catch (const RequestInfo::BadRequestException &e) {
      transaction.set_response_for_bad_request();
    }
    if (transaction.get_request_info().is_close_ == true) {
      return;
    }
    __transaction_queue_.push_back(Transaction(__conn_fd_));
  }
}

// 通信がクライアントから閉じられた時trueを返す。
bool Connection::append_receive_buffer() {
  const int         buf_size = 2048;
  std::vector<char> buf(buf_size);
  ssize_t           rc = recv(__conn_fd_, &buf[0], buf_size, MSG_DONTWAIT);
  if (rc == -1) {
    std::cerr << "recv() failed." << std::endl;
    exit(EXIT_FAILURE);
  }
  if (rc == 0) {
    return true;
  }
  std::string recv_data = std::string(buf.begin(), buf.begin() + rc);
  __buffer_.append(recv_data);
  return false;
}

struct pollfd Connection::create_pollfd() const {
  struct pollfd pfd = {__conn_fd_, POLLIN, 0};
  if (get_front_transaction().get_transaction_state() == CLOSING) {
    pfd.events = POLLOUT;
  } else if (get_front_transaction().get_transaction_state() == SENDING) {
    pfd.events = POLLIN | POLLOUT;
  }
  return pfd;
}
