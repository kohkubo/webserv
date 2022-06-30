#include "http/response/ResponseGenerator.hpp"

#include <cstdlib>
#include <unistd.h>

#include <iostream>
#include <map>
#include <stdexcept>

#include "config/Config.hpp"
#include "config/Location.hpp"
#include "event/Response.hpp"
#include "http/HttpStatusCode.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/utils.hpp"

namespace response_generator {

ResponseGenerator::ResponseGenerator(const RequestInfo &request_info,
                                     HttpStatusCode     status_code)
    : _request_info_(request_info)
    , _status_code_(status_code)
    , _is_connection_close_(
          _request_info_.connection_close_ ||
          _status_code_ == HttpStatusCode::C_400_BAD_REQUEST ||
          _status_code_ == HttpStatusCode::S_413_ENTITY_TOO_LARGE) {
  if (_status_code_.is_error_status_code()) {
    return;
  }
  if (_request_info_.location_ == NULL) {
    _status_code_ = HttpStatusCode::S_404_NOT_FOUND;
    return;
  }
  if (_request_info_.location_->return_map_.size() != 0) {
    _status_code_ = _request_info_.location_->return_map_.begin()->first;
    return;
  }
  _status_code_ = _handle_method(_request_info_);
}

Response ResponseGenerator::generate_response() {
  if (_body_.has_fd() && _body_.action_ == Body::WRITE) {
    if (write(_body_.fd_, _body_.content_.c_str(), _body_.content_.size()) ==
        -1) {
      _status_code_ = HttpStatusCode::S_500_INTERNAL_SERVER_ERROR;
    }
    _body_.has_content_ = false;
  }
  if (_body_.has_fd() && _body_.action_ == Body::READ) {
    Result<std::string> result = read_fd(_body_.fd_);
    if (result.is_err_) {
      _status_code_ = HttpStatusCode::S_500_INTERNAL_SERVER_ERROR;
    } else {
      _body_.content_     = result.object_;
      _body_.has_content_ = true;
    }
  }
  if (_status_code_.has_default_content()) {
    _body_ = _create_status_code_body(_request_info_);
  }
  if (!_body_.has_content_ && _body_.has_fd()) {
    Result<std::string> result = read_fd(_body_.fd_);
    if (!result.is_err_) {
      _body_.content_     = result.object_;
      _body_.has_content_ = true;
    }
  }

  if (!_body_.has_content_) {
    _body_.content_ = create_default_body_content(_status_code_);
  }
  // LOG("status_code: " << _status_code_);
  std::string response = create_response_message(
      _request_info_, _is_connection_close_, _status_code_, _body_.content_);
  return Response(response, _is_connection_close_);
}

} // namespace response_generator