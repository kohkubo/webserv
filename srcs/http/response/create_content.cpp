#include "http/response/ResponseGenerator.hpp"

#include "utils/Path.hpp"

namespace response_generator {

ResponseInfo create_status_code_content(const RequestInfo    &request_info,
                                        const HttpStatusCode &status_code) {
  config::errorPageMap::const_iterator it =
      request_info.config_.error_pages_.find(status_code);
  if (it != request_info.config_.error_pages_.end()) {
    Path file_path = request_info.location_->root_ + it->second;
    if (!file_path.is_file_exists()) {
      if (status_code == HttpStatusCode::S_404_NOT_FOUND) {
        return ResponseInfo(status_code.create_default_content_str(),
                            status_code);
      }
      return create_status_code_content(request_info,
                                        HttpStatusCode::S_404_NOT_FOUND);
    }
    Result<int> result = file_path.open_read_file();
    if (result.is_err_) {
      if (status_code == HttpStatusCode::S_500_INTERNAL_SERVER_ERROR) {
        return ResponseInfo(status_code.create_default_content_str(),
                            status_code);
      }
      return create_status_code_content(
          request_info, HttpStatusCode::S_500_INTERNAL_SERVER_ERROR);
    }
    return ReadContent(result.object_, status_code);
  }
  return ResponseInfo(status_code.create_default_content_str(), status_code);
}

} // namespace response_generator