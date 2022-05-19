#include "event/Request.hpp"
#include "http/response/response.hpp"
#include <sys/socket.h>

void Request::parse_header(const std::string &header) {
  __info_.parse_request_header(header);
  if (__info_.is_expected_body()) {
    __state_ = RECEIVING_BODY;
  } else {
    __state_ = PENDING;
  }
}

void Request::parse_body(const std::string &body) {
  __info_.parse_request_body(body);
  __state_ = PENDING;
}

void Request::create_response(const Config &conf) {
  if (get_state() != PENDING) {
    return;
  }
  http_message_map response_info = create_response_info(conf, __info_);
  __response_                    = make_message_string(response_info);
  __state_                       = SENDING;
}

void Request::send_response(int socket_fd) {
  const char *rest_str   = __response_.c_str() + __send_count_;
  size_t      rest_count = __response_.size() - __send_count_;
  ssize_t     sc         = send(socket_fd, rest_str, rest_count, MSG_DONTWAIT);
  __send_count_ += sc;
}
