#include "http/response/ResponseGenerator.hpp"

#include <unistd.h>

#include <iostream>

#include "config/Config.hpp"
#include "http/const/const_status_phrase.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/file_io_utils.hpp"

// TODO: リンクやその他のファイルシステムの時どうするか
static HttpStatusCode::StatusCode
check_filepath_status(const RequestInfo &request_info) {
  if (has_suffix(request_info.target_path_, "/")) {
    if (is_dir_exists(request_info.target_path_)) {
      if (!request_info.location_->autoindex_) {
        return HttpStatusCode::FORBIDDEN_403;
      }
      return HttpStatusCode::OK_200;
    }
    return HttpStatusCode::NOT_FOUND_404;
  }
  if (!is_file_exists(request_info.target_path_)) {
    ERROR_LOG("check_filepath_status: file not found");
    return HttpStatusCode::NOT_FOUND_404;
  }
  if (!is_accessible(request_info.target_path_, R_OK)) {
    // TODO: Permission error が 403なのか確かめてない
    return HttpStatusCode::FORBIDDEN_403;
  }
  return HttpStatusCode::OK_200;
}

static HttpStatusCode::StatusCode
delete_target_file(const RequestInfo &request_info) {
  if (request_info.content_length_ != 0) {
    ERROR_LOG("DELETE with body is unsupported");
    return HttpStatusCode::BAD_REQUEST_400;
  }
  if (!is_file_exists(request_info.target_path_)) {
    ERROR_LOG("target file is not found");
    return HttpStatusCode::NOT_FOUND_404;
  }
  if (!is_accessible(request_info.target_path_, W_OK)) {
    ERROR_LOG("process can not delete target file");
    return HttpStatusCode::FORBIDDEN_403;
  }
  if (!remove_file(request_info.target_path_)) {
    ERROR_LOG("unknown error while deleting file");
    return HttpStatusCode::INTERNAL_SERVER_ERROR_500;
  }
  ERROR_LOG("deleted file successfully");
  return HttpStatusCode::NO_CONTENT_204;
}

static bool is_available_methods(const RequestInfo &request_info) {
  return std::find(request_info.location_->available_methods_.begin(),
                   request_info.location_->available_methods_.end(),
                   request_info.method_) ==
         request_info.location_->available_methods_.end();
}

HttpStatusCode::StatusCode
ResponseGenerator::_handle_method(const RequestInfo &request_info) {
  HttpStatusCode::StatusCode status_code = HttpStatusCode::NONE;
  if (is_available_methods(request_info)) {
    return HttpStatusCode::NOT_ALLOWED_405;
  }
  if ("GET" == request_info.method_) {
    status_code = check_filepath_status(request_info);
  } else if ("POST" == request_info.method_) {
    status_code = check_filepath_status(request_info);
  } else if ("DELETE" == request_info.method_) {
    status_code = delete_target_file(request_info);
  } else {
    ERROR_LOG("unknown method: " << request_info.method_);
    status_code = HttpStatusCode::NOT_IMPLEMENTED_501;
  }
  return status_code;
}