#include "event/Transaction.hpp"

#include <sys/socket.h>

#include "http/const/const_delimiter.hpp"
#include "http/response/Response.hpp"

std::string Transaction::__cut_buffer(std::string &request_buffer,
                                      std::size_t  len) {
  std::string res = request_buffer.substr(0, len);
  request_buffer  = request_buffer.substr(len);
  return res;
}

bool Transaction::handle_state(std::string     &request_buffer,
                               const confGroup &conf_group) {
  std::size_t pos;
  switch (__transaction_state_) {
  case RECEIVING_HEADER:
    pos = request_buffer.find(HEADER_SP);
    if (pos == std::string::npos) {
      return false;
    }
    parse_header(__cut_buffer(request_buffer, pos + HEADER_SP.size()),
                 conf_group);
    break;
  case RECEIVING_BODY:
    // TODO: chunkedのサイズ判定
    if (request_buffer.size() < get_body_size()) {
      return false;
    }
    parse_body(__cut_buffer(request_buffer, get_body_size()));
    break;
  default:
    break;
  }
  if (!is_sending())
    return false;
  if (is_close())
    return false;
  return true;
}

// ヘッダーがパース出来たとき、configが決定できる。
void Transaction::parse_header(const std::string &header,
                               const confGroup   &conf_group) {
  // requestのエラーは例外が送出されるのでここでキャッチする。
  // エラーの時のレスポンスの生成方法は要検討
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
    // TODO: nginxのerror_pageディレクティブで400指定できるか確認。
    // 指定できるとき、nginxはどうやってserverを決定しているか。
    // serverが決定できる不正なリクエストと決定できないリクエストを実際に送信して確認？
    // 現状は暫定的に、定型文を送信。
    __response_ = "HTTP/1.1 400 Bad Request\r\nconnection: close\r\n\r\n";
    __transaction_state_      = SENDING;
    __request_info_.is_close_ = true;
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
