#include "http/request/RequestInfo.hpp"

#include <sstream>

static std::size_t hexstr_to_size(const std::string &str) {
  std::size_t       x;
  std::stringstream ss;
  ss << std::hex << str;
  ss >> x;
  return x;
}

void RequestInfo::set_next_chunk_size(std::string &chunk_size_line) {
  next_chunk_size_ = hexstr_to_size(chunk_size_line);
  next_chunk_      = CHUNK_DATA;
}

void RequestInfo::store_unchunked_body(std::string &chunk_line) {
  unchunked_body_.append(chunk_line);
  next_chunk_ = CHUNK_SIZE;
}