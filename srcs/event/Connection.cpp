#include "event/Connection.hpp"

#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>

#include "config/Config.hpp"
#include "http/const/const_delimiter.hpp"



// TODO:
// 関数名を適切に変更したほうがよい。create_transaction以外のことを行っている。
void Connection::create_transaction() {
  while (1) {
    Transaction &transaction = __transaction_queue_.back();
    // ここ何しているの??
    // リクエストのパース
    // TODO: is_continue以外の名前が良さそう
    bool is_continue = transaction.handle_request(__buffer_, __conf_group_);
    if (is_continue == false) {
      return;
    }
    // TODO: continueのときに新たにTransactionを生成するのはなぜか? kohkubo
    __transaction_queue_.push_back(Transaction(__conn_fd_));
  }
}

// 通信がクライアントから閉じられた時trueを返す。
bool Connection::receive_request() {
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
