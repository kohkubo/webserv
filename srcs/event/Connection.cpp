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
  if (__transaction_queue_.empty()) {
    // TODO: この分岐 必要?? kohkubo
    // TODO: この中で生成のは__get_last_transactionという名前からずれている。
    __transaction_queue_.push_back(Transaction());
  }
  return __transaction_queue_.back();
}

// TODO: 関数名を適切に変更したほうがよい?
// transactionを作る以上のことをしている。
void Connection::__create_transaction() {
  while (1) {
    Transaction &transaction = __get_last_transaction();
    // ここ何しているの??
    // リクエストのパース
    bool is_continue = transaction.handle_request(__buffer_, __conf_group_);
    if (is_continue) {
      // TODO: continueのときに新たにTransactionを生成するのはなぜか? kohkubo
      __transaction_queue_.push_back(Transaction());
      continue;
    } else {
      return;
    }
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
  __buffer_.append(recv_data);
  // TODO: conn_fdを__create_transactionに渡して
  // transactionが内部でconn_fdを持ったほうが良さそう? kohkubo
  __create_transaction();
  return false;
}

struct pollfd Connection::create_pollfd(connFd conn_fd) const {
  struct pollfd pfd = {conn_fd, POLLIN, 0};
  if (__transaction_queue_.empty()) {
    return pfd;
  }
  if (get_front_transaction().get_transaction_state() == SENDING &&
      get_front_transaction().get_request_info().is_close_) {
    pfd.events = POLLOUT;
  } else if (get_front_transaction().get_transaction_state() == SENDING) {
    pfd.events = POLLIN | POLLOUT;
  }
  return pfd;
}
