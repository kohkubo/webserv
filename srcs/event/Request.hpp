#ifndef SRCS_EVENT_REQUEST_HPP
#define SRCS_EVENT_REQUEST_HPP

#include "http/HttpMessage.hpp"
#include "http/const/const_response_key_map.hpp"
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

public:
  RequestState state_;
  std::string  header_;
  std::string  body_;
  HttpMessage  info_;
  std::string  response_;

private:
  ssize_t __send_count_;

public:
  Request()
      : state_(RECEIVING_HEADER)
      , __send_count_(0) {}

  void send_response(int socket_fd) {
    const char *rest_str   = response_.c_str() + __send_count_;
    size_t      rest_count = response_.size() - __send_count_;
    ssize_t     sc = send(socket_fd, rest_str, rest_count, MSG_DONTWAIT);
    __send_count_ += sc;
  }

  bool is_send_completed() {
    return __send_count_ == static_cast<ssize_t>(response_.size());
  }
};

#endif /* SRCS_EVENT_REQUEST_HPP */
