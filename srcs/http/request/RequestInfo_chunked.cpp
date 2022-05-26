#include "http/request/RequestInfo.hpp"

/*
  length := 0
  read chunk-size, chunk-ext (if any), and CRLF
  while (chunk-size > 0) {
    read chunk-data and CRLF
    append chunk-data to decoded-body
    length := length + chunk-size
    read chunk-size, chunk-ext (if any), and CRLF
  }
  read trailer field
  while (trailer field is not empty) {
    if (trailer field is allowed to be sent in a trailer) {
        append trailer field to existing header fields
    }
    read trailer-field
  }
  Content-Length := length
  Remove "chunked" from Transfer-Encoding
  Remove Trailer from existing header fields
 */

#include <sstream>

static std::size_t hexstr_to_size(const std::string &str) {
  std::size_t       x;
  std::stringstream ss;
  ss << std::hex << str;
  ss >> x;
  return x;
}

// chunkedの受信が完了したときtrueを返す。
bool RequestInfo::parse_request_chunked_body(std::string &body_line) {
  switch (__next_line_) {
  case CHUNK_SIZE:
    __next_chunk_size_ = hexstr_to_size(body_line);
    __next_line_       = CHUNK_DATA;
    break;
  case CHUNK_DATA:
    if (__next_chunk_size_ == 0 && body_line == "") {
      parse_request_body(__unchunked_body_);
      return true;
    }
    if (__next_chunk_size_ == body_line.size()) {
      __unchunked_body_.append(body_line);
      __next_line_ = CHUNK_SIZE;
    }
    break;
  default:
    break;
  }
  return false;
}
