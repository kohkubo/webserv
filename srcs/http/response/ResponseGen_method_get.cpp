#include "http/response/ResponseGenerator.hpp"

#include "http/request/RequestInfo.hpp"
#include "utils/file_io_utils.hpp"

// TODO: リンクやその他のファイルシステムの時どうするか
HttpStatusCode
ResponseGenerator::_method_get_dir(const RequestInfo &request_info,
                                   const std::string &target_path) {
  if (!is_dir_exists(target_path)) {
    return HttpStatusCode::S_404_NOT_FOUND;
  }
  if (!request_info.location_->autoindex_) {
    return HttpStatusCode::S_403_FORBIDDEN;
  }
  _body_.content_     = _create_autoindex_body(request_info, target_path);
  _body_.has_content_ = true;
  return HttpStatusCode::S_200_OK;
}

// TODO: リンクやその他のファイルシステムの時どうするか
HttpStatusCode
ResponseGenerator::_method_get_file(const RequestInfo &request_info,
                                    const std::string &target_path) {
  if (!is_file_exists(target_path)) {
    ERROR_LOG("_method_get_file: file not found");
    return HttpStatusCode::S_404_NOT_FOUND;
  }
  if (has_suffix(target_path, ".py")) {
    Result<std::string> result =
        _read_file_to_str_cgi(request_info, target_path);
    if (result.is_err_) {
      return HttpStatusCode::S_500_INTERNAL_SERVER_ERROR;
    }
    _body_.content_     = result.object_;
    _body_.has_content_ = true;
    return HttpStatusCode::S_200_OK;
  }
  Result<int> result = open_read_file(target_path);
  if (result.is_err_) {
    return HttpStatusCode::S_500_INTERNAL_SERVER_ERROR;
  }
  _body_.action_ = Body::READ;
  _body_.fd_     = result.object_;
  return HttpStatusCode::S_200_OK;
}