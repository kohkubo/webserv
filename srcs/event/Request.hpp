#ifndef SRCS_EVENT_REQUEST_HPP
#define SRCS_EVENT_REQUEST_HPP

#include "http/HttpMessage.hpp"
#include "http/const/const_response_key_map.hpp"
#include <string>
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
  RequestState state_;
  std::string  header_;
  std::string  body_;
  HttpMessage  info_;
  std::string  response_;
  ssize_t      send_count_;

  Request()
      : state_(RECEIVING_HEADER)
      , send_count_(0) {}
};

#endif /* SRCS_EVENT_REQUEST_HPP */
