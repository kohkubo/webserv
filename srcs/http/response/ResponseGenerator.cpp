#include "http/response/ResponseGenerator.hpp"

#include <cstdlib>
#include <unistd.h>

#include <iostream>
#include <map>
#include <stdexcept>

#include "config/Config.hpp"
#include "config/Location.hpp"
#include "event/Response.hpp"
#include "http/const/const_http_enums.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/utils.hpp"

static std::pair<bool, HttpStatusCode::StatusCode>
check_location(const Location *location) {
  if (location == NULL) {
    return std::make_pair(true, HttpStatusCode::NOT_FOUND_404);
  }
  if (location->return_map_.size() != 0) {
    return std::make_pair(true, static_cast<HttpStatusCode::StatusCode>(
                                    location->return_map_.begin()->first));
  }
  return std::make_pair(false, HttpStatusCode::OK_200);
}

Response ResponseGenerator::generate_response() {
  Body body;
  body.status_code_ = _response_info_.status_code_;
  if (!_is_error_status_code(body.status_code_)) {
    std::pair<bool, HttpStatusCode::StatusCode> pair =
        check_location(_response_info_.request_info_.location_);
    if (pair.first) {
      body.status_code_ = pair.second;
    }
  }
  if (!_is_error_status_code(body.status_code_) &&
      body.status_code_ != HttpStatusCode::MOVED_PERMANENTLY_301) {
    body = _handle_method(_response_info_.request_info_);
  }
  if (body.has_fd() && body.action_ == Body::WRITE) {
    if (write(body.fd_, body.content_.c_str(), sizeof(body.content_.c_str())) ==
        -1)
      body.status_code_ = HttpStatusCode::INTERNAL_SERVER_ERROR_500;
  }
  if (body.has_fd() && body.action_ == Body::READ) {
    Result<std::string> result = read_fd(body.fd_);
    if (result.is_err_) {
      body.status_code_ = HttpStatusCode::INTERNAL_SERVER_ERROR_500;
    } else {
      body.content_     = result.object_;
      body.has_content_ = true;
    }
  }
  if (_is_error_status_code(body.status_code_) ||
      body.status_code_ == HttpStatusCode::MOVED_PERMANENTLY_301) {
    body = _create_status_code_body(_response_info_.request_info_,
                                    body.status_code_);
  }
  if (!body.has_content_ && body.has_fd()) {
    Result<std::string> result = read_fd(body.fd_);
    if (!result.is_err_) {
      body.content_     = result.object_;
      body.has_content_ = true;
    }
  }

  if (!body.has_content_) {
    body.content_ = _create_default_body_content(body.status_code_);
  }
  // LOG("status_code: " << body.status_code_);
  std::string response = _create_response_message(
      _response_info_.request_info_, _response_info_.is_connection_close_,
      body.status_code_, body.content_);
  return Response(response, _response_info_.is_connection_close_);
}
