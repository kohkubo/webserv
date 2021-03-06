#include "http/response/ResponseGenerator.hpp"

#include "utils/Path.hpp"

namespace response_generator {

ResponseInfo ResponseGenerator::_create_status_code_content(
    const HttpStatusCode &status_code) {
  config::errorPageMap::const_iterator it =
      request_info_.config_->error_pages_.find(status_code);
  if (it != request_info_.config_->error_pages_.end()) {
    Path file_path = request_info_.location_->root_ + it->second;
    if (!file_path.is_file_exists()) {
      if (status_code == HttpStatusCode::S_404_NOT_FOUND) {
        return ResponseInfo(ResponseInfo::CREATED,
                            status_code.create_default_content_str(),
                            status_code);
      }
      return _create_status_code_content(HttpStatusCode::S_404_NOT_FOUND);
    }
    Result<int> result = file_path.open_read_file();
    if (result.is_err_) {
      if (status_code == HttpStatusCode::S_500_INTERNAL_SERVER_ERROR) {
        return ResponseInfo(ResponseInfo::CREATED,
                            status_code.create_default_content_str(),
                            status_code);
      }
      return _create_status_code_content(
          HttpStatusCode::S_500_INTERNAL_SERVER_ERROR);
    }
    return ResponseInfo(ResponseInfo::READ, std::string(), status_code,
                        result.object_);
  }
  return ResponseInfo(ResponseInfo::CREATED,
                      status_code.create_default_content_str(), status_code);
}

} // namespace response_generator