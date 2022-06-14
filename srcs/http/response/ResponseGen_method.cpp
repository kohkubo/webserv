#include "http/response/ResponseGenerator.hpp"

#include <unistd.h>

#include <iostream>

#include "config/Config.hpp"
#include "http/const/const_status_phrase.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/file_io_utils.hpp"

// TODO: リンクやその他のファイルシステムの時どうするか
static HttpStatusCode check_filepath_status(const Location    &location,
                                            const std::string &file_path) {
  if (has_suffix(file_path, "/")) {
    if (is_dir_exists(file_path)) {
      if (!location.autoindex_) {
        return FORBIDDEN_403; // nginxに合わせた
      }
      return OK_200;
    }
    return NOT_FOUND_404;
  }
  if (!is_file_exists(file_path)) {
    return NOT_FOUND_404;
  }
  if (!is_accessible(file_path, R_OK)) {
    // TODO: Permission error が 403なのか確かめてない
    return FORBIDDEN_403;
  }
  return OK_200;
}

static HttpStatusCode handle_post_method(const Location    &location,
                                         const std::string &file_path) {
  // TODO: ここらへんの処理、未定なので雑に書いています。
  if (std::find(location.limit_except_.begin(), location.limit_except_.end(),
                "POST") == location.limit_except_.end()) {
    return NOT_ALLOWED_405;
  }
  return check_filepath_status(location, file_path);
}

static HttpStatusCode handle_get_method(const Location    &location,
                                        const std::string &file_path) {
  // TODO: ここらへんの処理、未定なので雑に書いています。
  if (std::find(location.limit_except_.begin(), location.limit_except_.end(),
                "GET") == location.limit_except_.end()) {
    return NOT_ALLOWED_405;
  }
  return check_filepath_status(location, file_path);
}

static HttpStatusCode delete_target_file(const RequestInfo &request_info,
                                         const std::string &file_path) {
  if (request_info.content_length_ != 0) {
    ERROR_LOG("DELETE with body is unsupported");
    return BAD_REQUEST_400;
  }
  if (!is_file_exists(file_path)) {
    ERROR_LOG("target file is not found");
    return NOT_FOUND_404;
  }
  if (!is_accessible(file_path, W_OK)) {
    ERROR_LOG("process can not delete target file");
    return FORBIDDEN_403;
  }
  if (!remove_file(file_path)) {
    ERROR_LOG("unknown error while deleting file");
    return INTERNAL_SERVER_ERROR_500;
  }
  ERROR_LOG("deleted file successfully");
  return NO_CONTENT_204;
}

static HttpStatusCode handle_delete_method(const Location    &location,
                                           const RequestInfo &request_info,
                                           const std::string &file_path) {
  // TODO: ここらへんの処理、未定なので雑に書いています。
  if (std::find(location.limit_except_.begin(), location.limit_except_.end(),
                "DELETE") == location.limit_except_.end()) {
    return NOT_ALLOWED_405;
  }
  return delete_target_file(request_info, file_path);
}

HttpStatusCode
ResponseGenerator::__handle_method(const Location    &location,
                                   const RequestInfo &request_info,
                                   const std::string &file_path) {
  HttpStatusCode status_code = NONE;
  if ("GET" == request_info.method_) {
    status_code = handle_get_method(location, file_path);
  } else if ("POST" == request_info.method_) {
    status_code = handle_post_method(location, file_path);
  } else if ("DELETE" == request_info.method_) {
    status_code = handle_delete_method(location, request_info, file_path);
  } else {
    LOG("unknown method: " << request_info.method_);
    status_code = NOT_IMPLEMENTED_501;
  }
  return status_code;
}