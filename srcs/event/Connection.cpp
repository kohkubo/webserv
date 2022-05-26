#include "event/Connection.hpp"

#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>

#include "config/Config.hpp"
#include "http/const/const_delimiter.hpp"

Transaction &Connection::__get_last_transaction() {
  if (__transaction_queue_.empty())
    __transaction_queue_.push_back(Transaction());
  return __transaction_queue_.back();
}

void Connection::__create_transaction(const std::string &data) {
  __buffer_.append(data);
  while (1) {
    Transaction &transaction = __get_last_transaction();
    bool         is_continue =
        transaction.handle_transaction_state(__buffer_, __conf_group_);
    if (is_continue) {
      __transaction_queue_.push_back(Transaction());
      continue;
    } else {
      return;
    }
  }
}

void Connection::send_response(connFd conn_fd) {
  Transaction &transaction = __front_transaction();
  if (transaction.send_response(conn_fd)) {
    __erase_front_transaction();
  }
}

// 通信がクライアントから閉じられた時trueを返す。
bool Connection::receive_request(connFd conn_fd) {
  const int         buf_size = 2048;
  std::vector<char> buf(buf_size);
  ssize_t           rc = recv(conn_fd, &buf[0], buf_size, MSG_DONTWAIT);
  if (rc == -1) {
    std::cerr << "recv() failed." << std::endl;
    exit(EXIT_FAILURE);
  }
  if (rc == 0) {
    return true;
  }
  std::string recv_data = std::string(buf.begin(), buf.begin() + rc);
  __create_transaction(recv_data);
  return false;
}

struct pollfd Connection::create_pollfd(connFd conn_fd) const {
  struct pollfd pfd = {conn_fd, POLLIN, 0};
  if (__transaction_queue_.empty()) {
    return pfd;
  }
  if (__front_transaction().is_sending() && __front_transaction().is_close()) {
    pfd.events = POLLOUT;
  } else if (__front_transaction().is_sending()) {
    pfd.events = POLLIN | POLLOUT;
  }
  return pfd;
}
