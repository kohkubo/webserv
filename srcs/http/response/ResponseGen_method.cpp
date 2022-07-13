#include "http/response/ResponseGenerator.hpp"

#include <unistd.h>

#include <algorithm>

#include "http/request/RequestInfo.hpp"
#include "utils/Path.hpp"

namespace response_generator {

ResponseInfo ResponseGenerator::_method_get_dir(const Path &target_path) {
  if (!target_path.is_dir_exists()) {
    return _create_status_code_content(HttpStatusCode::S_404_NOT_FOUND);
  }
  if (!location_->autoindex_) {
    return _create_status_code_content(HttpStatusCode::S_403_FORBIDDEN);
  }
  return ResponseInfo(create_autoindex_body(request_info_, target_path),
                      HttpStatusCode::S_200_OK);
}

ResponseInfo ResponseGenerator::_method_get_file(const Path &target_path) {
  if (!target_path.is_file_exists()) {
    ERROR_LOG("method_get_file: file not found");
    return _create_status_code_content(HttpStatusCode::S_404_NOT_FOUND);
  }
  if (target_path.has_suffix(".py")) {
    Result<ResponseInfo> result =
        create_cgi_content(request_info_, peer_name_, target_path);
    if (result.is_err_) {
      return _create_status_code_content(
          HttpStatusCode::S_500_INTERNAL_SERVER_ERROR);
    }
    return result.object_;
  }
  Result<int> result = target_path.open_read_file();
  if (result.is_err_) {
    ERROR_LOG("method_get_file: open file failed");
    return _create_status_code_content(
        HttpStatusCode::S_500_INTERNAL_SERVER_ERROR);
  }
  return ReadContent(result.object_, HttpStatusCode::S_200_OK);
}

ResponseInfo ResponseGenerator::_method_get(const Path &target_path) {
  if (target_path.has_suffix("/")) {
    Path path_add_index = target_path + location_->index_;
    if (!path_add_index.is_file_exists()) {
      return _method_get_dir(target_path);
    }
    return _method_get_file(path_add_index);
  }
  return _method_get_file(target_path);
}

ResponseInfo ResponseGenerator::_method_post(const Path &target_path) {
  if (!location_->upload_file_ || target_path.has_suffix("/")) {
    return _create_status_code_content(HttpStatusCode::S_403_FORBIDDEN);
  }
  Result<int> result = target_path.open_write_file();
  if (result.is_err_) {
    ERROR_LOG("method_post: open file failed");
    return _create_status_code_content(
        HttpStatusCode::S_500_INTERNAL_SERVER_ERROR);
  }
  return WriteContent(result.object_, request_info_.body_);
}

ResponseInfo ResponseGenerator::_method_delete(const Path &target_path) {
  HttpStatusCode status_code;
  if (!target_path.is_file_exists()) {
    ERROR_LOG("target file is not found");
    status_code = HttpStatusCode::S_404_NOT_FOUND;
  } else if (!target_path.is_accessible(W_OK)) {
    ERROR_LOG("process can not delete target file");
    status_code = HttpStatusCode::S_403_FORBIDDEN;
  } else if (!target_path.remove_file()) {
    ERROR_LOG("unknown error while deleting file");
    status_code = HttpStatusCode::S_500_INTERNAL_SERVER_ERROR;
  } else {
    ERROR_LOG("deleted file successfully");
    status_code = HttpStatusCode::S_204_NO_CONTENT;
  }
  return _create_status_code_content(status_code);
}

// RFC 9110
// 9.1. Overview
// An origin server that receives a request method that is recognized and
// implemented, but not allowed for the target resource, SHOULD respond with the
// 405 (Method Not Allowed) status code.

ResponseInfo ResponseGenerator::_handle_method(const Path &target_path) {
  if (location_->is_unavailable_method(request_info_.request_line_.method_)) {
    return _create_status_code_content(HttpStatusCode::S_405_NOT_ALLOWED);
  }
  if ("GET" == request_info_.request_line_.method_) {
    return _method_get(target_path);
  }
  if ("POST" == request_info_.request_line_.method_) {
    return _method_post(target_path);
  }
  if ("DELETE" == request_info_.request_line_.method_) {
    return _method_delete(target_path);
  }
  ERROR_LOG("unknown method: " << request_info_.request_line_.method_);
  return _create_status_code_content(HttpStatusCode::S_501_NOT_IMPLEMENTED);
}

} // namespace response_generator