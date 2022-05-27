#include "event/Transaction.hpp"

#include <sys/socket.h>

#include "http/const/const_delimiter.hpp"
#include "http/response/Response.hpp"

void Transaction::set_response_for_bad_request() {
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
void Transaction::handle_request(std::string &request_buffer) {
  try {
    if (__transaction_state_ == RECEIVING_STARTLINE ||
        __transaction_state_ == RECEIVING_HEADER) {
      std::string line;
      while (__getline(request_buffer, line)) {
        if (__transaction_state_ == RECEIVING_STARTLINE) {
          __request_info_.check_first_mulit_blank_line(line);
          if (__request_info_.is_blank_first_line_ == true) {
            continue;
          }
          __request_info_.check_bad_parse_request_start_line(line);
          __request_info_.parse_request_start_line(line);
          __transaction_state_ = RECEIVING_HEADER;
        } else if (__transaction_state_ == RECEIVING_HEADER) {
          if (line != "") {
            __request_info_.store_request_header_field_map(line);
            continue;
          }
          __request_info_.parse_request_header();
          // TODO: チャンク or content_length_ != 0
          if (__request_info_.content_length_ != 0) {
            __transaction_state_ = RECEIVING_BODY;
          } else {
            __transaction_state_ = SENDING;
            return;
          }
        }
      }
    }
    if (__transaction_state_ == RECEIVING_BODY &&
        __request_info_.has_request_body(request_buffer)) {
      std::string request_body =
          __request_info_.cut_request_body(request_buffer);
      __request_info_.parse_request_body(request_body);
      __transaction_state_ = SENDING;
    }
  } catch (const RequestInfo::BadRequestException &e) {
    set_response_for_bad_request();
  }
}

bool Transaction::__getline(std::string &request_buffer, std::string &line) {
  std::size_t pos = request_buffer.find(CRLF);
  if (pos == std::string::npos)
    return false;
  line = request_buffer.substr(0, pos);
  request_buffer.erase(0, pos + 2);
  return true;
}

const Config *Transaction::get_proper_config(const confGroup &conf_group) {
  confGroup::const_iterator it = conf_group.begin();
  for (; it != conf_group.end(); it++) {
    if ((*it)->server_name_ == __request_info_.host_) {
      return *it;
    }
  }
  return conf_group[0];
}

void Transaction::create_response(const Config *config) {
  Response response(*config, __request_info_);
  __response_ = response.get_response_string();
}

// 送信が完了かつtransactionを保持する必要がないときtrueを返す。
void Transaction::send_response() {
  const char *rest_str   = __response_.c_str() + __send_count_;
  size_t      rest_count = __response_.size() - __send_count_;
  ssize_t     sc         = send(__conn_fd_, rest_str, rest_count, MSG_DONTWAIT);
  __send_count_ += sc;
}
