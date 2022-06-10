#ifndef SRCS_EVENT_TRANSACTION_HPP
#define SRCS_EVENT_TRANSACTION_HPP

#include <sys/types.h>

#include <string>

#include "config/Config.hpp"
#include "event/Response.hpp"
#include "http/request/RequestInfo.hpp"
#include "http/response/ResponseGenerator.hpp"

typedef int connFd;

enum RequestState {
  RECEIVING_STARTLINE, // リクエストはrequest_lineを読み取り中。
  RECEIVING_HEADER,    // リクエストはheaderを読み取り中。
  RECEIVING_BODY,      // リクエストはbodyを読み取り中。
  SUCCESS,             // リクエストのパース完了。
};

enum NextChunkType { CHUNK_SIZE, CHUNK_DATA };

// TODO: string -> vector<char>

struct Request {
private:
  const Config                      *__config_;
  RequestState                       __state_;
  RequestInfo                        __request_info_;
  std::string                        __response_;
  NextChunkType                      __next_chunk_;
  std::size_t                        __next_chunk_size_;
  std::string                        __request_body_;
  std::map<std::string, std::string> __field_map_;
  static const std::size_t           buffer_max_length_ = 8192;

private:
  static bool        __getline(std::string &request_buffer, std::string &line);
  static std::string __cutout_request_body(std::string &request_buffer,
                                           size_t       content_length);
  static bool        __get_next_chunk_line(NextChunkType chunk_type,
                                           std::string  &request_buffer,
                                           std::string &chunk, size_t next_chunk_size);
  RequestState       __chunk_loop(std::string &request_buffer);
  static void
  __check_max_client_body_size_exception(std::size_t actual_body_size,
                                         std::size_t max_body_size);
  static void __check_buffer_length_exception(std::string &request_buffer,
                                              std::size_t  buffer_limit_length);
  static const Config *__select_proper_config(const confGroup   &conf_group,
                                              const std::string &host_name);
  // TODO: 命名変える
  void                 __set_response_for_bad_request();

public:
  Request()
      : __config_(NULL)
      , __state_(RECEIVING_STARTLINE)
      , __next_chunk_(CHUNK_SIZE)
      , __next_chunk_size_(-1) {}

  const RequestInfo &request_info() const { return __request_info_; }
  RequestState       state() const { return __state_; }
  void               set_state(RequestState transaction_state) {
    __state_ = transaction_state;
  }
  void handle_request(std::string &request_buffer, const confGroup &conf_group);
  Response create_response() {
    // TODO: is_close判定
    // エラー || Connection: close -> true
    return Response(__response_, __request_info_.connection_close_);
  }
};

#endif /* SRCS_EVENT_TRANSACTION_HPP */
