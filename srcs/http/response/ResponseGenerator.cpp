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
  if (has_fd() && content_.state_ == Content::WRITE) {
    if (write(content_.fd_, content_.str_.c_str(), content_.str_.size()) ==
        -1) {
      _status_code_ = HttpStatusCode::S_500_INTERNAL_SERVER_ERROR;
    }
    content_.fd_ =
        -1; // writeが成功してもfd=-1にして_create_status_code_body()を使うようにする
  }
  if (content_.state_ == Content::CREATED) {
    // bodyのcontent自体をレスポンスとして使いたい場合
    return Response(create_response_message(content_.str_),
                    _is_connection_close_);
  }
  if (has_fd()) {
    return Response("", _is_connection_close_, Response::READING);
  }
  if (_status_code_.has_default_content()) {
    content_ = _create_status_code_content(_request_info_); // status_code or fd
    if (has_fd()) {
      return Response("", _is_connection_close_, Response::READING);
    }
  }
  std::string content = create_default_body_content(_status_code_);
  return Response(create_response_message(content), _is_connection_close_);
}

} // namespace response_generator