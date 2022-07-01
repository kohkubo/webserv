#include "http/response/ResponseGenerator.hpp"

namespace response_generator {

ResponseGenerator::Content ResponseGenerator::_create_status_code_content(
    const RequestInfo &request_info) {
  ResponseGenerator::Content   content;
  errorPageMap::const_iterator it =
      request_info.config_.error_pages_.find(_status_code_);
  if (it != request_info.config_.error_pages_.end()) {
    Path file_path = request_info.location_->root_ + it->second;
    if (!file_path.is_file_exists()) {
      if (_status_code_ == HttpStatusCode::S_404_NOT_FOUND) {
        return content;
      }
      _status_code_ = HttpStatusCode::S_404_NOT_FOUND;
      return _create_status_code_content(request_info);
    }
    Result<int> result = file_path.open_read_file();
    if (result.is_err_) {
      if (_status_code_ == HttpStatusCode::S_500_INTERNAL_SERVER_ERROR) {
        return content;
      }
      _status_code_ = HttpStatusCode::S_500_INTERNAL_SERVER_ERROR;
      return _create_status_code_content(request_info);
    }
    content.state_ = Content::READ;
    content.fd_    = result.object_;
  }
  return content;
}

} // namespace response_generator