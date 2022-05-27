#include "event/Transaction.hpp"

#include <sys/socket.h>

#include "http/const/const_delimiter.hpp"
#include "http/response/Response.hpp"

void Transaction::__set_response_for_bad_request() {
  // 400エラー処理
  // TODO: nginxのerror_pageディレクティブで400指定できるか確認。
  // 指定できるとき、nginxはどうやってserverを決定しているか。
  // serverが決定できる不正なリクエストと決定できないリクエストを実際に送信して確認？
  // 現状は暫定的に、定型文を送信。
  __response_ = "HTTP/1.1 400 Bad Request\r\nconnection: close\r\n\r\n";
  __transaction_state_      = SENDING;
  __request_info_.is_close_ = true;
}

// 一つのリクエストのパースを行う、bufferに一つ以上のリクエストが含まれるときtrueを返す。
bool Transaction::parse_single_request(std::string     &request_buffer,
                                       const confGroup &conf_group) {
  if (__transaction_state_ == RECEIVING_STARTLINE ||
      __transaction_state_ == RECEIVING_HEADER) {
    __parse_single_line(request_buffer, conf_group);
  }
  if (__transaction_state_ == RECEIVING_BODY &&
      __request_info_.has_request_body(request_buffer)) {
    parse_body(request_buffer);
  }
  if (__transaction_state_ != SENDING)
    return false;
  if (__request_info_.is_close_)
    return false;
  return true;
}

void Transaction::__parse_single_line(std::string     &request_buffer,
                                      const confGroup &conf_group) {
  while (__check_line(request_buffer)) {
    std::string line = __getline_from_buffer(request_buffer);
    switch (__transaction_state_) {
    case RECEIVING_STARTLINE:
      parse_start_line(line);
      break;
    case RECEIVING_HEADER:
      parse_header(line, conf_group);
      break;
    default:
      break;
    }
  }
}

bool Transaction::__check_line(const std::string &request_buffer) {
  std::size_t pos = request_buffer.find(CRLF);
  return pos != std::string::npos;
}

std::string Transaction::__getline_from_buffer(std::string &buf) {
  std::size_t pos = buf.find(CRLF);
  std::string res = buf.substr(0, pos);
  buf             = buf.substr(pos + CRLF.size());
  return res;
}

void Transaction::parse_start_line(std::string &request_line) {
  try {
    if (__request_info_.parse_request_start_line(request_line))
      __transaction_state_ = RECEIVING_HEADER;
  } catch (const RequestInfo::BadRequestException &e) {
    __set_response_for_bad_request();
  }
}

void Transaction::parse_header(std::string     &header_line,
                               const confGroup &conf_group) {
  try {
    if (__request_info_.parse_request_header(header_line)) {
      detect_config(conf_group);
      if (__request_info_.is_expected_body()) {
        __transaction_state_ = RECEIVING_BODY;
      } else {
        create_response();
      }
    }
  } catch (const RequestInfo::BadRequestException &e) {
    __set_response_for_bad_request();
  }
}

void Transaction::parse_body(std::string &request_buffer) {
  std::string request_body = __request_info_.cut_request_body(request_buffer);
  try {
    __request_info_.parse_request_body(request_body);
    create_response();
  } catch (const RequestInfo::BadRequestException &e) {
    __set_response_for_bad_request();
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

void Transaction::create_response() {
  Response response(*__conf_, __request_info_);
  __response_          = response.get_response_string();
  __transaction_state_ = SENDING;
}

// 送信が完了かつtransactionを保持する必要がないときtrueを返す。
bool Transaction::send_response(int socket_fd) {
  const char *rest_str   = __response_.c_str() + __send_count_;
  size_t      rest_count = __response_.size() - __send_count_;
  ssize_t     sc         = send(socket_fd, rest_str, rest_count, MSG_DONTWAIT);
  __send_count_ += sc;
  if (!__is_send_all()) {
    return false;
  }
  if (__request_info_.is_close_) {
    shutdown(socket_fd, SHUT_WR);
    __transaction_state_ = CLOSING;
    return false;
  }
  return true;
}
