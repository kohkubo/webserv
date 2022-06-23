#ifndef SRCS_EVENT_REQUEST_HPP
#define SRCS_EVENT_REQUEST_HPP

#include <sys/types.h>

#include <string>

#include "config/ConfigGroup.hpp"
#include "event/Response.hpp"
#include "http/request/RequestInfo.hpp"
#include "http/response/ResponseGenerator.hpp"

typedef int connFd;

struct Request {
  enum RequestState {
    RECEIVING_STARTLINE, // リクエストはrequest_lineを読み取り中。
    RECEIVING_HEADER,    // リクエストはheaderを読み取り中。
    RECEIVING_BODY,      // リクエストはbodyを読み取り中。
    SUCCESS,             // リクエストのパース完了。
  };
  enum NextChunkType { CHUNK_SIZE, CHUNK_DATA };

private:
  RequestState                       _state_;
  RequestInfo                        _request_info_;
  NextChunkType                      _next_chunk_;
  std::size_t                        _next_chunk_size_;
  std::string                        _request_body_;
  std::map<std::string, std::string> _field_map_;
  static const std::size_t           BUFFER_MAX_LENGTH_ = 8192;

private:
  RequestState _chunk_loop(std::string &request_buffer);
  static void
  _check_max_client_body_size_exception(std::size_t actual_body_size,
                                        std::size_t max_body_size);
  static void  _check_buffer_length_exception(std::string &request_buffer,
                                              std::size_t  buffer_limit_length);
  void         _tmp(const ConfigGroup &config_group);
  RequestState _handle_request_startline(std::string &request_buffer);
  RequestState _handle_request_header(std::string &request_buffer);
  RequestState _handle_request_body(std::string &request_buffer);

public:
  Request()
      : _state_(RECEIVING_STARTLINE)
      , _next_chunk_(CHUNK_SIZE)
      , _next_chunk_size_(-1) {}

  // TODO: テストでの使用のみなのでテストを変更し、消去する 2022/06/10 22:15 3人
  const RequestInfo &request_info() const { return _request_info_; }
  RequestState       handle_request(std::string       &request_buffer,
                                    const ConfigGroup &config_group);

  Response create_response() {
    std::string response = ResponseGenerator::generate_response(_request_info_);
    return Response(response, _request_info_.connection_close_);
  }
  Response create_response(HttpStatusCode::StatusCode status_code) {
    _request_info_.connection_close_ = true;
    std::string response =
        ResponseGenerator::generate_response(_request_info_, status_code);
    return Response(response, _request_info_.connection_close_);
  }
};

#endif /* SRCS_EVENT_REQUEST_HPP */
