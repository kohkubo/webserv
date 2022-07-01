#include "http/response/ResponseGenerator.hpp"

#include "utils/file_io_utils.hpp"

namespace response_generator {

ResponseGenerator::Body
ResponseGenerator::_create_status_code_body(const RequestInfo &request_info) {
  ResponseGenerator::Body      body;
  errorPageMap::const_iterator it =
      request_info.config_.error_pages_.find(_status_code_);
  if (it != request_info.config_.error_pages_.end()) {
    std::string file_path = request_info.location_->root_ + it->second;
    if (!is_file_exists(file_path)) {
      if (_status_code_ == HttpStatusCode::S_404_NOT_FOUND) {
        return body;
      }
      _status_code_ = HttpStatusCode::S_404_NOT_FOUND;
      return _create_status_code_body(request_info);
    }
    Result<int> result = open_read_file(file_path);
    if (result.is_err_) {
      if (_status_code_ == HttpStatusCode::S_500_INTERNAL_SERVER_ERROR) {
        return body;
      }
      _status_code_ = HttpStatusCode::S_500_INTERNAL_SERVER_ERROR;
      return _create_status_code_body(request_info);
    }
    body.action_ = Body::READ;
    body.fd_     = result.object_;
  }
  return body;
}

} // namespace response_generator