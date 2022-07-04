#include "http/response/ResponseGenerator.hpp"

#include "utils/Path.hpp"

namespace response_generator {

std::string create_default_body_content(const HttpStatusCode &status_code) {
  LOG("create_default_body_content");
  return "<!DOCTYPE html>\n"
         "<html>\n"
         "    <head>\n"
         "        <title>" +
         status_code.str() +
         "</title>\n"
         "    </head>\n"
         "    <body>\n"
         "<h2>" +
         status_code.status_phrase() +
         "</h2>\n"
         "provided by webserv\n"
         "    </body>\n"
         "</html>";
}

ResponseGenerator::Content
create_status_code_content(const RequestInfo    &request_info,
                           const HttpStatusCode &status_code) {
  ResponseGenerator::Content   content(status_code);
  errorPageMap::const_iterator it =
      request_info.config_.error_pages_.find(status_code);
  if (it != request_info.config_.error_pages_.end()) {
    Path file_path = request_info.location_->root_ + it->second;
    if (!file_path.is_file_exists()) {
      if (status_code == HttpStatusCode::S_404_NOT_FOUND) {
        content.str_    = create_default_body_content(status_code);
        content.action_ = ResponseGenerator::Content::CREATED;
        return content;
      }
      return create_status_code_content(request_info,
                                        HttpStatusCode::S_404_NOT_FOUND);
    }
    Result<int> result = file_path.open_read_file();
    if (result.is_err_) {
      if (status_code == HttpStatusCode::S_500_INTERNAL_SERVER_ERROR) {
        content.str_    = create_default_body_content(status_code);
        content.action_ = ResponseGenerator::Content::CREATED;
        return content;
      }
      return create_status_code_content(
          request_info, HttpStatusCode::S_500_INTERNAL_SERVER_ERROR);
    }
    content.action_ = ResponseGenerator::Content::READ;
    content.fd_     = result.object_;
    return content;
  }
  content.str_    = create_default_body_content(status_code);
  content.action_ = ResponseGenerator::Content::CREATED;
  return content;
}

} // namespace response_generator