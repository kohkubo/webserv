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

ResponseGenerator::ResponseGenerator(const RequestInfo         &request_info,
                                     HttpStatusCode::StatusCode status_code)
    : _request_info_(request_info)
    , _status_code_(status_code)
    , _is_connection_close_(_request_info_.connection_close_ ||
                            _status_code_ == HttpStatusCode::BAD_REQUEST_400 ||
                            _status_code_ ==
                                HttpStatusCode::ENTITY_TOO_LARGE_413) {}

Response ResponseGenerator::generate_response() {
  Body body;
  if (!_is_error_status_code(_status_code_)) {
    std::pair<bool, HttpStatusCode::StatusCode> pair =
        check_location(_request_info_.location_);
    if (pair.first) {
      _status_code_ = pair.second;
    }
  }
  if (!_is_error_status_code(_status_code_) &&
      _status_code_ != HttpStatusCode::MOVED_PERMANENTLY_301) {
    body = _handle_method(_request_info_);
  }
  if (body.has_fd() && body.action_ == Body::WRITE) {
    if (write(body.fd_, body.content_.c_str(), body.content_.size()) == -1) {
      _status_code_ = HttpStatusCode::INTERNAL_SERVER_ERROR_500;
    }
    body.has_content_ = false;
  }
  if (body.has_fd() && body.action_ == Body::READ) {
    Result<std::string> result = read_fd(body.fd_);
    if (result.is_err_) {
      _status_code_ = HttpStatusCode::INTERNAL_SERVER_ERROR_500;
    } else {
      body.content_     = result.object_;
      body.has_content_ = true;
    }
  }
  if (_is_error_status_code(_status_code_) ||
      _status_code_ == HttpStatusCode::MOVED_PERMANENTLY_301) {
    body = _create_status_code_body(_request_info_);
  }
  if (!body.has_content_ && body.has_fd()) {
    Result<std::string> result = read_fd(body.fd_);
    if (!result.is_err_) {
      body.content_     = result.object_;
      body.has_content_ = true;
    }
  }

  if (!body.has_content_) {
    body.content_ = _create_default_body_content(_status_code_);
  }
  // LOG("status_code: " << _status_code_);
  std::string response = _create_response_message(
      _request_info_, _is_connection_close_, _status_code_, body.content_);
  return Response(response, _is_connection_close_);
}
