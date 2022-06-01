#include "event/Connection.hpp"

#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>

#include "config/Config.hpp"
#include "http/const/const_delimiter.hpp"
#include "http/response/Response.hpp"

void Connection::create_sequential_transaction() {
  while (true) {
    Transaction &transaction = __transaction_queue_.back();
    try {
      transaction.handle_request(__buffer_);
      __check_buffer_length_exception(__buffer_, buffer_max_length_);
      if (transaction.get_transaction_state() != SENDING) { // noexcept
        return;
      }
      const Config *config = transaction.get_proper_config(
          __conf_group_, transaction.get_request_info().host_); // noexcept
      Response response(*config, transaction.get_request_info());
      __response_ = response.get_response_string();
    } catch (const RequestInfo::BadRequestException &e) {
      // 400エラー処理
      // TODO: nginxのerror_pageディレクティブで400指定できるか確認。
      // 指定できるとき、nginxはどうやってserverを決定しているか。
      // serverが決定できる不正なリクエストと決定できないリクエストを実際に送信して確認？
      // 現状は暫定的に、定型文を送信。
      __response_ = "HTTP/1.1 400 Bad Request\r\nconnection: close\r\n\r\n";
      transaction.set_transaction_state(SENDING);
      transaction.get_request_info().set_is_close(true);
    }
    __transaction_queue_.push_back(Transaction());
  }
}

// 通信がクライアントから閉じられた時trueを返す。
// TODO: ↑読み取りきったらじゃない? kohkubo
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
  if (__transaction_queue_.front().get_transaction_state() == SENDING) {
    pfd.events = POLLIN | POLLOUT;
  }
  return pfd;
}

void Connection::send_response(connFd conn_fd) {
  Transaction &transaction = __transaction_queue_.front();
  __response_string_size_ +=
      transaction.send_response(conn_fd, __response_, __response_string_size_);
  if (transaction.get_request_info().is_close_) {
    shutdown(conn_fd, SHUT_WR);
    __transaction_queue_.front().set_transaction_state(CLOSING);
    return;
  }
  if (__is_send_all(__response_string_size_, __response_)) {
    __transaction_queue_.pop_front();
  }
}

void Connection::__check_buffer_length_exception(
    std::string &request_buffer, std::size_t buffer_max_length) {
  if (request_buffer.size() >= buffer_max_length) {
    request_buffer.clear();
    throw RequestInfo::BadRequestException();
  }
}
