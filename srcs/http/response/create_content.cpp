#include "http/response/ResponseGenerator.hpp"

#include "utils/file_io_utils.hpp"

namespace response_generator {

ResponseGenerator::Content
create_status_code_content(const RequestInfo    &request_info,
                           const HttpStatusCode &status_code) {
  ResponseGenerator::Content   content(status_code);
  errorPageMap::const_iterator it =
      request_info.config_.error_pages_.find(status_code);
  if (it != request_info.config_.error_pages_.end()) {
    std::string file_path = request_info.location_->root_ + it->second;
    if (!is_file_exists(file_path)) {
      if (status_code == HttpStatusCode::S_404_NOT_FOUND) {
        content.str_   = create_default_body_content(status_code);
        content.state_ = ResponseGenerator::Content::CREATED;
        return content;
      }
      return create_status_code_content(request_info,
                                        HttpStatusCode::S_404_NOT_FOUND);
    }
    Result<int> result = open_read_file(file_path);
    if (result.is_err_) {
      if (status_code == HttpStatusCode::S_500_INTERNAL_SERVER_ERROR) {
        content.str_   = create_default_body_content(status_code);
        content.state_ = ResponseGenerator::Content::CREATED;
        return content;
      }
      return create_status_code_content(
          request_info, HttpStatusCode::S_500_INTERNAL_SERVER_ERROR);
    }
    content.state_ = ResponseGenerator::Content::READ;
    content.fd_    = result.object_;
    return content;
  }
  content.str_   = create_default_body_content(status_code);
  content.state_ = ResponseGenerator::Content::CREATED;
  return content;
}

} // namespace response_generator