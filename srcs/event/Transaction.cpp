#include "event/Transaction.hpp"

#include <sys/socket.h>

#include "http/response/response.hpp"

void Transaction::parse_header(const std::string &header) {
  // requestのエラーは例外が送出されるのでここでキャッチする。
  // エラーの時のレスポンスの生成方法は要検討
  __request_info_.parse_request_header(header);
  if (__request_info_.is_expected_body()) {
    __transaction_state_ = RECEIVING_BODY;
  } else {
    __transaction_state_ = PENDING;
  }
}

void Transaction::parse_body(const std::string &body) {
  __request_info_.parse_request_body(body);
  __transaction_state_ = PENDING;
}

void Transaction::create_response(const Config &conf) {
  if (get_transaction_state() != PENDING) {
    return;
  }
  http_message_map response_info = create_response_info(conf, __request_info_);
  __response_                    = make_message_string(response_info);
  __transaction_state_           = SENDING;
}

void Transaction::send_response(int socket_fd) {
  const char *rest_str   = __response_.c_str() + __send_count_;
  size_t      rest_count = __response_.size() - __send_count_;
  ssize_t     sc         = send(socket_fd, rest_str, rest_count, MSG_DONTWAIT);
  __send_count_ += sc;
}
