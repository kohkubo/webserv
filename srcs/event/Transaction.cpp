#include "event/Transaction.hpp"

#include <sys/socket.h>

#include "http/response/Response.hpp"

void Transaction::parse_header(const std::string &header) {
  // requestのエラーは例外が送出されるのでここでキャッチする。
  // エラーの時のレスポンスの生成方法は要検討
  try {
    __requst_info_.parse_request_header(header);
    if (__requst_info_.is_expected_body()) {
      __transction_state_ = RECEIVING_BODY;
    } else {
      __transction_state_ = PENDING;
    }
  } catch (const std::exception &e) {
    // 400エラー処理
    // 仕様読まないとconfigで400エラーが指定できるのか、する必要があるのか不明。
    // TODO: 本当にこれでよいの?? 2022/05/22 17:19 kohkubo nakamoto
    // 現状、Requestが400だったときは、Responseクラスを呼び出さなくてもよい??
    __response_ = "HTTP/1.1 400 Bad Request\r\nconnection: close\r\n\r\n";
    __transction_state_ = SENDING;
  }
}

void Transaction::detect_config(const confGroup &conf_group) {
  confGroup::const_iterator it = conf_group.begin();
  for (; it != conf_group.end(); it++) {
    if ((*it)->server_name_ == __requst_info_.host_) {
      __conf_ = *it;
      return;
    }
  }
  __conf_ = conf_group[0];
}

void Transaction::parse_body(const std::string &body) {
  __request_info_.parse_request_body(body);
  __transaction_state_ = PENDING;
}

void Transaction::create_response() {
  if (get_tranction_state() != PENDING) {
    return;
  }
  Response response(*__conf_, __requst_info_);
  __response_         = response.get_response_string();
  __transction_state_ = SENDING;
}

void Transaction::send_response(int socket_fd) {
  const char *rest_str   = __response_.c_str() + __send_count_;
  size_t      rest_count = __response_.size() - __send_count_;
  ssize_t     sc         = send(socket_fd, rest_str, rest_count, MSG_DONTWAIT);
  __send_count_ += sc;
}
