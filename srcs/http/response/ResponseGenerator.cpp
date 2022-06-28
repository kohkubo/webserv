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
#include "http/const/const_status_phrase.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/utils.hpp"

static bool is_error_status_code(HttpStatusCode::StatusCode status_code) {
  // TODO: エラーのステータスコードの扱いを決まったら再実装
  return 399 < status_code && status_code < 600;
}

ResponseGenerator::Body
ResponseGenerator::_create_body(const RequestInfo               &request_info,
                                const HttpStatusCode::StatusCode status_code) {
  Body body;
  if (is_error_status_code(status_code) ||
      HttpStatusCode::MOVED_PERMANENTLY_301 == status_code) {
    return _body_of_status_code(request_info, status_code);
  }
  if (has_suffix(request_info.target_path_, ".py")) {
    Result<std::string> result = _read_file_to_str_cgi(request_info);
    if (result.is_err_) {
      return _body_of_status_code(request_info,
                                  HttpStatusCode::INTERNAL_SERVER_ERROR_500);
    }
    body.content_ = result.object_;
    return body;
  }
  if (has_suffix(request_info.target_path_, "/")) {
    body.content_ = _create_autoindex_body(request_info);
    return body;
  }
  Result<std::string> result = read_file_to_str(request_info.target_path_);
  if (result.is_err_) {
    return _body_of_status_code(request_info,
                                HttpStatusCode::INTERNAL_SERVER_ERROR_500);
  }
  body.content_ = result.object_;
  return body;
}

HttpStatusCode::StatusCode
ResponseGenerator::_get_status_code(const RequestInfo         &request_info,
                                    HttpStatusCode::StatusCode status_code) {
  if (request_info.location_ == NULL) {
    status_code = HttpStatusCode::NOT_FOUND_404;
  } else if (is_error_status_code(status_code)) {
    // LOG("status_code: " << status_code);
  } else if (request_info.location_->return_map_.size() != 0) {
    status_code = static_cast<HttpStatusCode::StatusCode>(
        request_info.location_->return_map_.begin()->first);
  } else {
    status_code = _handle_method(request_info);
  }
  return status_code;
}

Response ResponseGenerator::generate_response() {
  _response_info_.status_code_ = _get_status_code(_response_info_.request_info_,
                                                  _response_info_.status_code_);
  Body body =
      _create_body(_response_info_.request_info_, _response_info_.status_code_);
  if (body.has_fd()) {
    Result<std::string> result = read_fd(body.fd_);
    if (result.is_err_) {
      _response_info_.status_code_ = HttpStatusCode::INTERNAL_SERVER_ERROR_500;
      body = _body_of_status_code(_response_info_.request_info_,
                                  _response_info_.status_code_);
    } else {
      body.content_ = result.object_;
    }
  }
  std::string response = _create_response_message(
      _response_info_.request_info_, _response_info_.is_connection_close_,
      _response_info_.status_code_, body.content_);
  return Response(response, _response_info_.is_connection_close_);
}
