#include "event/Transaction.hpp"

#include <sys/socket.h>

#include "http/const/const_delimiter.hpp"
#include "http/request/RequestInfo.hpp"
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
  if (__transaction_state_ == RECEIVING_STARTLINE ||
      __transaction_state_ == RECEIVING_HEADER) {
    std::string line;
    while (__getline(request_buffer, line)) { // noexcept
      if (__transaction_state_ == RECEIVING_STARTLINE) {
        __request_info_.check_first_multi_blank_line(
            line); // throws BadRequestException
        if (__request_info_.is_blank_first_line_) {
          continue;
        }
        RequestInfo::check_bad_parse_request_start_line(
            line); // throws BadRequestException
        __request_info_.parse_request_start_line(line); // noexcept
        __transaction_state_ = RECEIVING_HEADER;
      } else if (__transaction_state_ == RECEIVING_HEADER) {
        if (line != "") {
          RequestInfo::store_request_header_field_map(
              line, __field_map_); // throws BadRequestException
          continue;
        }
        __request_info_.parse_request_header(
            __field_map_); // throws BadRequestException
        // TODO: validate request_header
        if (__request_info_.content_length_ != 0 ||
            __request_info_.is_chunked_) {
          __transaction_state_ = RECEIVING_BODY;
          break;
        }
        __transaction_state_ = SENDING;
        return;
      }
    }
  }
  if (__transaction_state_ == RECEIVING_BODY) {
    std::string request_body;
    if (__request_info_.is_chunked_) {
      __transaction_state_ =
          __chunk_loop(request_buffer, request_body,
                       __transaction_state_); // throws BadRequestException
      if (__transaction_state_ == SENDING) {
        __request_info_.parse_request_body(__unchunked_body_,
                                           __request_info_.content_type_);
        return;
      }
    } else {
      //__set_request_body == trueだったらparse_request_bodyを呼べる
      // TODO: request_buffer.size() ==
      // __request_info_.content_length_が同じとき、bodyのパースに入れる??
      // kohkubo
      if (request_buffer.size() < __request_info_.content_length_) {
        return;
      }
      __set_request_body(request_buffer, request_body,
                         __request_info_.content_length_);
      __transaction_state_ = SENDING;
    }
    if (__transaction_state_ == SENDING) {
      __request_info_.parse_request_body(request_body,
                                         __request_info_.content_type_);
    }
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

void Transaction::__set_request_body(std::string &request_buffer,
                                     std::string &body, size_t content_length) {
  body = request_buffer.substr(0, content_length);
  request_buffer.erase(0, content_length);
}

bool Transaction::__get_next_chunk_line(NextChunkType chunk_type,
                                        std::string  &request_buffer,
                                        std::string  &chunk,
                                        size_t        next_chunk_size) {
  if (chunk_type == CHUNK_SIZE) {
    return __getline(request_buffer, chunk);
  }
  if (request_buffer.size() < next_chunk_size + CRLF.size()) {
    return false;
  }
  chunk = request_buffer.substr(0, next_chunk_size);
  request_buffer.erase(0, next_chunk_size);
  if (!has_prefix(request_buffer, CRLF)) {
    throw RequestInfo::BadRequestException();
  }
  request_buffer.erase(0, CRLF.size());
  return true;
}

const Config *
Transaction::get_proper_config(const confGroup &conf_group) const {
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

void Transaction::send_response() {
  const char *rest_str   = __response_.c_str() + __send_count_;
  size_t      rest_count = __response_.size() - __send_count_;
  ssize_t     sc         = send(__conn_fd_, rest_str, rest_count, MSG_DONTWAIT);
  __send_count_ += sc;
}

// 最終的にこのループは外部に切り出せるようにtransaction_stateを引数に持っておく
TransactionState Transaction::__chunk_loop(std::string     &request_buffer,
                                           std::string     &request_body,
                                           TransactionState transaction_state) {
  while (__get_next_chunk_line(__next_chunk_, request_buffer, request_body,
                               __next_chunk_size_)) {
    if (__next_chunk_ == CHUNK_SIZE) {
      __next_chunk_size_ = hexstr_to_size(request_body);
      __next_chunk_      = CHUNK_DATA;
    } else {
      if (__next_chunk_size_ == 0 && request_body == "") {
        return SENDING;
      }
      __unchunked_body_.append(request_body);
      __next_chunk_ = CHUNK_SIZE;
    }
  }
  return transaction_state;
}
