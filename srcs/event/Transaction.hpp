#ifndef SRCS_EVENT_REQUEST_HPP
#define SRCS_EVENT_REQUEST_HPP

#include "config/Config.hpp"
#include "http/request/RequestInfo.hpp"
#include <string>
#include <sys/types.h>

enum RequestState {
  NO_REQUEST,       // Connectionはリクエストを持たない。
  RECEIVING_HEADER, // リクエストはheaderを読み取り中。
  RECEIVING_BODY,   // リクエストはbodyを読み取り中。
  PENDING, // リクエストの読み取りは完了。レスポンスの生成待ち。
  SENDING, // レスポンスの送信中。
  CLOSING,
};

// TODO: string -> vector<char>

struct Transaction {
private:
  RequestState __state_;
  ssize_t      __send_count_;
  std::string  __response_;
  RequestInfo  __info_;

public:
  Transaction()
      : __state_(RECEIVING_HEADER)
      , __send_count_(0) {}

  RequestState get_state() const { return __state_; }
  void         set_state(RequestState state) { __state_ = state; }
  bool         is_close() { return __info_.is_close_; }
  size_t       get_body_size() { return __info_.content_length_; }
  bool         is_send_completed() {
    return __send_count_ == static_cast<ssize_t>(__response_.size());
  }

  void parse_header(const std::string &header);
  void parse_body(const std::string &body);
  void create_response(const Config &conf);
  void send_response(int socket_fd);
};

#endif /* SRCS_EVENT_REQUEST_HPP */