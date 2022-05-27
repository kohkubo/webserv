#include "http/response/Response.hpp"

#include <unistd.h>

#include <iostream>

#include "config/Config.hpp"
#include "http/const/const_header_field_values.hpp"
#include "http/const/const_status_phrase.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/file_io_utils.hpp"

void Response::__delete_target_file() {
  if (!remove_file(__file_path_)) {
    std::cerr << "unknown error while deleting file" << std::endl;
    __status_code_ = INTERNAL_SERVER_ERROR_500;
    return;
  }
  std::cerr << "deleted file successfully" << std::endl;
  __status_code_ = NO_CONTENT_204;
  return;
}
