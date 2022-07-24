#ifndef SRCS_EVENT_REQUEST_HPP
#define SRCS_EVENT_REQUEST_HPP

#include <sys/types.h>

#include <string>

#include "config/ConfigGroup.hpp"
#include "http/request/RequestInfo.hpp"

typedef int connFd;

struct Request {
  enum RequestState {
    RECEIVING_STARTLINE, // リクエストはrequest_lineを読み取り中。
    RECEIVING_HEADER,    // リクエストはheaderを読み取り中。
    RECEIVING_BODY,      // リクエストはbodyを読み取り中。
    SUCCESS,             // リクエストのパース完了。
  };
  enum NextChunkType { CHUNK_SIZE, CHUNK_DATA };
  struct Chunk {
    NextChunkType next_chunk_type_;
    std::size_t   next_chunk_size_;
    Chunk()
        : next_chunk_type_(CHUNK_SIZE)
        , next_chunk_size_(-1) {}
  };

private:
  RequestState   _state_;
  RequestInfo    _request_info_;
  Chunk          _chunk_;
  HeaderFieldMap _header_field_map_;
  // RFC 9110
  // 4.1.URI References
  // It is RECOMMENDED that all senders and recipients support, at a minimum,
  // URIs with lengths of 8000 octets in protocol elements.
  static const std::size_t BUFFER_MAX_LENGTH_ = 8192;

private:
  RequestState _chunk_loop(std::string &request_buffer);
  static void  _check_max_client_body_size_exception(ssize_t actual_body_size,
                                                     std::size_t max_body_size);
  static void  _check_buffer_length_exception(std::string &request_buffer);
  RequestState _handle_request_line(std::string &request_buffer);
  RequestState _handle_request_header(std::string &request_buffer);
  RequestState _handle_request_body(std::string &request_buffer);

public:
  Request()
      : _state_(RECEIVING_STARTLINE) {}

  const RequestInfo &request_info() const { return _request_info_; }
  RequestState       handle_request(std::string               &request_buffer,
                                    const config::ConfigGroup &config_group);
};

#endif /* SRCS_EVENT_REQUEST_HPP */
