#include "event/Request.hpp"
#include "http/request/request_parse.hpp"
#include "http/response/response.hpp"
#include <sys/socket.h>

void Request::parse_header(const std::string &header) {
  parse_request_header(__info_, header);
  if (__info_.is_expected_body()) {
    __state_ = RECEIVING_BODY;
  } else {
    __state_ = PENDING;
  }
}

void Request::parse_body(const std::string &body) {
  parse_request_body(__info_, body);
  __state_ = PENDING;
}

std::string Request::create_response(const ServerConfig &conf) {
  http_message_map response_info = create_response_info(conf, __info_);
  return make_message_string(response_info);
}
