#include "http/request/RequestInfo.hpp"

#include <sstream>

// TODO: validate
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
