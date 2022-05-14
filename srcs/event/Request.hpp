#ifndef SRCS_EVENT_REQUEST_HPP
#define SRCS_EVENT_REQUEST_HPP

#include "http/HttpMessage.hpp"
#include "http/const/const_response_key_map.hpp"
#include "http/request/request_parse.hpp"
#include "http/response/response.hpp"
#include <string>
#include <sys/socket.h>
#include <sys/types.h>

enum RequestState {
  NO_REQUEST,       // Connectionはリクエストを持たない。
  RECEIVING_HEADER, // リクエストはheaderを読み取り中。
  RECEIVING_BODY,   // リクエストはbodyを読み取り中。
  PENDING, // リクエストの読み取りは完了。レスポンスの生成待ち。
  SENDING, // レスポンスの送信中。
};

// TODO: string -> vector<char>

struct Request {
private:
  RequestState __state_;
  ssize_t      __send_count_;
  std::string  __response_;

public:
  std::string body_;
  HttpMessage info_;

public:
  Request()
      : __state_(RECEIVING_HEADER)
      , __send_count_(0) {}

  RequestState get_state() const { return __state_; }

  void         parse_header(const std::string &header) {
    parse_request_header(info_, header);
    if (info_.is_expected_body()) {
      __state_ = RECEIVING_BODY;
    } else {
      __state_ = PENDING;
    }
  }

  void parse_body(const std::string &body) {
    parse_request_body(info_, body);
    __state_ = PENDING;
  }

  void create_response(const ServerConfig &conf) {
    if (get_state() != PENDING) {
      return;
    }
    http_message_map response_info = create_response_info(conf, info_);
    __response_                    = make_message_string(response_info);
    __state_                       = SENDING;
  }

  void send_response(int socket_fd) {
    const char *rest_str   = __response_.c_str() + __send_count_;
    size_t      rest_count = __response_.size() - __send_count_;
    ssize_t     sc = send(socket_fd, rest_str, rest_count, MSG_DONTWAIT);
    __send_count_ += sc;
  }

  bool is_send_completed() {
    return __send_count_ == static_cast<ssize_t>(__response_.size());
  }
};

#endif /* SRCS_EVENT_REQUEST_HPP */
