#include "http/response/ResponseGenerator.hpp"

#include <unistd.h>

#include <iostream>

#include "config/Config.hpp"
#include "http/const/const_status_phrase.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/file_io_utils.hpp"

HttpStatusCode
ResponseGenerator::__delete_target_file(const RequestInfo request_info,
                                        const std::string file_path) {
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
