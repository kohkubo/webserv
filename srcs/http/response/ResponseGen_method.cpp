#include "http/response/ResponseGenerator.hpp"

#include <unistd.h>

#include <iostream>

#include "config/Config.hpp"
#include "http/const/const_status_phrase.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/file_io_utils.hpp"

// TODO: リンクやその他のファイルシステムの時どうするか
static HttpStatusCode check_filepath_status(const RequestInfo &request_info) {
  if (has_suffix(request_info.file_path_, "/")) {
    if (is_dir_exists(request_info.file_path_)) {
      if (!request_info.location_->autoindex_) {
        return FORBIDDEN_403; // nginxに合わせた
      }
      return OK_200;
    }
    return NOT_FOUND_404;
  }
  if (!is_file_exists(request_info.file_path_)) {
    return NOT_FOUND_404;
  }
  if (!is_accessible(request_info.file_path_, R_OK)) {
    // TODO: Permission error が 403なのか確かめてない
    return FORBIDDEN_403;
  }
  return OK_200;
}

static HttpStatusCode handle_post_method(const RequestInfo &request_info) {
  // TODO: ここらへんの処理、未定なので雑に書いています。
  if (std::find(request_info.location_->limit_except_.begin(),
                request_info.location_->limit_except_.end(),
                "POST") == request_info.location_->limit_except_.end()) {
    return NOT_ALLOWED_405;
  }
  return check_filepath_status(request_info);
}

static HttpStatusCode handle_get_method(const RequestInfo &request_info) {
  // TODO: ここらへんの処理、未定なので雑に書いています。
  if (std::find(request_info.location_->limit_except_.begin(),
                request_info.location_->limit_except_.end(),
                "GET") == request_info.location_->limit_except_.end()) {
    return NOT_ALLOWED_405;
  }
  return check_filepath_status(request_info);
}

static HttpStatusCode delete_target_file(const RequestInfo &request_info) {
  if (request_info.content_length_ != 0) {
    ERROR_LOG("DELETE with body is unsupported");
    return BAD_REQUEST_400;
  }
  if (!is_file_exists(request_info.file_path_)) {
    ERROR_LOG("target file is not found");
    return NOT_FOUND_404;
  }
  if (!is_accessible(request_info.file_path_, W_OK)) {
    ERROR_LOG("process can not delete target file");
    return FORBIDDEN_403;
  }
  if (!remove_file(request_info.file_path_)) {
    ERROR_LOG("unknown error while deleting file");
    return INTERNAL_SERVER_ERROR_500;
  }
  ERROR_LOG("deleted file successfully");
  return NO_CONTENT_204;
}

static HttpStatusCode handle_delete_method(const RequestInfo &request_info) {
  // TODO: ここらへんの処理、未定なので雑に書いています。
  if (std::find(request_info.location_->limit_except_.begin(),
                request_info.location_->limit_except_.end(),
                "DELETE") == request_info.location_->limit_except_.end()) {
    return NOT_ALLOWED_405;
  }
  return delete_target_file(request_info);
}

HttpStatusCode
ResponseGenerator::_handle_method(const RequestInfo &request_info) {
  HttpStatusCode status_code = NONE;
  if ("GET" == request_info.method_) {
    status_code = handle_get_method(request_info);
  } else if ("POST" == request_info.method_) {
    status_code = handle_post_method(request_info);
  } else if ("DELETE" == request_info.method_) {
    status_code = handle_delete_method(request_info);
  } else {
    LOG("unknown method: " << request_info.method_);
    status_code = NOT_IMPLEMENTED_501;
  }
  return status_code;
}