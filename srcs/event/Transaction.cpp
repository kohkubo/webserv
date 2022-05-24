#include "event/Transaction.hpp"

#include <sys/socket.h>

#include "http/response/Response.hpp"

// ヘッダーがパース出来たとき、configが決定できる。
void Transaction::parse_header(const std::string &header,
                               const confGroup   &conf_group) {
  // requestのエラーは例外が送出されるのでここでキャッチする。
  // エラーの時のレスポンスの生成方法は要検討
  std::cout << "=======" << std::endl;
  std::cout << header << std::endl;
  std::cout << "=======" << std::endl;
  try {
    __request_info_.parse_request_header(header);
    detect_config(conf_group);
    if (__request_info_.is_expected_body()) {
      __transaction_state_ = RECEIVING_BODY;
    } else {
      create_response();
    }
  } catch (const std::exception &e) {
    // 400エラー処理
    // 仕様読まないとconfigで400エラーが指定できるのか、する必要があるのか不明。
    // TODO: 本当にこれでよいの?? 2022/05/22 17:19 kohkubo nakamoto
    // 現状、Requestが400だったときは、Responseクラスを呼び出さなくてもよい??
    __response_ = "HTTP/1.1 400 Bad Request\r\nconnection: close\r\n\r\n";
    __transaction_state_ = SENDING;
  }
}

void Transaction::detect_config(const confGroup &conf_group) {
  confGroup::const_iterator it = conf_group.begin();
  for (; it != conf_group.end(); it++) {
    if ((*it)->server_name_ == __request_info_.host_) {
      __conf_ = *it;
      return;
    }
  }
  __conf_ = conf_group[0];
}

void Transaction::parse_body(const std::string &body) {
  std::cout << "~~~~~~~" << std::endl;
  std::cout << body << std::endl;
  std::cout << "size: " << body.size() << std::endl;
  std::cout << "~~~~~~~" << std::endl;
  __request_info_.parse_request_body(body);
  create_response();
}

void Transaction::create_response() {
  Response response(*__conf_, __request_info_);
  __response_          = response.get_response_string();
  __transaction_state_ = SENDING;
}

void Transaction::send_response(int socket_fd) {
  const char *rest_str   = __response_.c_str() + __send_count_;
  size_t      rest_count = __response_.size() - __send_count_;
  ssize_t     sc         = send(socket_fd, rest_str, rest_count, MSG_DONTWAIT);
  __send_count_ += sc;
}
