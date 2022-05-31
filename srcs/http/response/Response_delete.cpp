#include "http/response/Response.hpp"

#include <unistd.h>

#include <iostream>

#include "config/Config.hpp"
#include "http/const/const_header_field_values.hpp"
#include "http/const/const_status_phrase.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/file_io_utils.hpp"

void Response::__delete_target_file() {
  if (__status_code_ != NONE) {
    return;
  }
  if (__request_info_.content_length_ != 0) {
    std::cerr << "DELETE with body is unsupported" << std::endl;
    __status_code_ = BAD_REQUEST_400;
    return;
  }
  if (!is_file_exists(__file_path_)) {
    std::cerr << "target file is not found" << std::endl;
    __status_code_ = NOT_FOUND_404;
    return;
  }
  if (!is_accessible(__file_path_, W_OK)) {
    std::cerr << "process can not delete target file" << std::endl;
    __status_code_ = FORBIDDEN_403;
    return;
  }
  if (!remove_file(__file_path_)) {
    std::cerr << "unknown error while deleting file" << std::endl;
    __status_code_ = INTERNAL_SERVER_ERROR_500;
    return;
  }
  std::cerr << "deleted file successfully" << std::endl;
  __status_code_ = NO_CONTENT_204;
}
