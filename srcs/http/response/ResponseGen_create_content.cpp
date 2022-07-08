#include "http/response/ResponseGenerator.hpp"

#include "utils/Path.hpp"

namespace response_generator {

ResponseGenerator::Content ResponseGenerator::_create_status_code_content(
    const HttpStatusCode &status_code) {
  ResponseGenerator::Content           content(status_code);
  config::errorPageMap::const_iterator it =
      request_info_.config_.error_pages_.find(status_code);
  if (it != request_info_.config_.error_pages_.end()) {
    Path file_path = location_->root_ + it->second;
    if (!file_path.is_file_exists()) {
      if (status_code == HttpStatusCode::S_404_NOT_FOUND) {
        content.str_    = status_code.create_default_content_str();
        content.action_ = ResponseGenerator::Content::CREATED;
        return content;
      }
      return _create_status_code_content(HttpStatusCode::S_404_NOT_FOUND);
    }
    Result<int> result = file_path.open_read_file();
    if (result.is_err_) {
      if (status_code == HttpStatusCode::S_500_INTERNAL_SERVER_ERROR) {
        content.str_    = status_code.create_default_content_str();
        content.action_ = ResponseGenerator::Content::CREATED;
        return content;
      }
      return _create_status_code_content(
          HttpStatusCode::S_500_INTERNAL_SERVER_ERROR);
    }
    content.action_ = ResponseGenerator::Content::READ;
    content.fd_     = result.object_;
    return content;
  }
  content.str_    = status_code.create_default_content_str();
  content.action_ = ResponseGenerator::Content::CREATED;
  return content;
}

void ResponseGenerator::update_content(const HttpStatusCode &status_code) {
  content_ = _create_status_code_content(status_code);
}

} // namespace response_generator