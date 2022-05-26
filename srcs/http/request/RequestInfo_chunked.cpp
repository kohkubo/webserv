#include "http/request/RequestInfo.hpp"

bool RequestInfo::parse_request_chunked_body(std::string &body_line) {
  (void)body_line;
  return true;
}