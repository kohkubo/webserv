#ifndef SRCS_EVENT_REQUEST_HPP
#define SRCS_EVENT_REQUEST_HPP

#include "config/ServerConfig.hpp"
#include "http/HttpMessage.hpp"
#include <string>
#include <sys/types.h>

enum RequestState {
  NO_REQUEST,       // Connectionはリクエストを持たない。
  RECEIVING_HEADER, // リクエストはheaderを読み取り中。
  RECEIVING_BODY,   // リクエストはbodyを読み取り中。
  PENDING, // リクエストの読み取りは完了。レスポンスの生成待ち。
};

// TODO: string -> vector<char>

struct Request {
private:
  RequestState __state_;
  HttpMessage  __info_;

public:
  Request()
      : __state_(RECEIVING_HEADER) {}

  RequestState get_state() const { return __state_; }
  void         set_state(RequestState state) { __state_ = state; }
  bool         is_close() { return __info_.is_close_; }
  size_t       get_body_size() { return __info_.content_length_; }

  void         parse_header(const std::string &header);
  void         parse_body(const std::string &body);
  std::string  create_response(const ServerConfig &conf);
};

#endif /* SRCS_EVENT_REQUEST_HPP */
