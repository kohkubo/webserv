#include "http/response/ResponseGenerator.hpp"

#include <unistd.h>

#include <iostream>

#include "config/Config.hpp"
#include "http/const/const_header_field_values.hpp"
#include "http/const/const_status_phrase.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/file_io_utils.hpp"

HttpStatusCode
ResponseGenerator::__delete_target_file(const RequestInfo request_info,
                                        const std::string file_path) {
  if (request_info.content_length_ != 0) {
    std::cerr << "DELETE with body is unsupported" << std::endl;
    return BAD_REQUEST_400;
  }
  if (!is_file_exists(file_path)) {
    std::cerr << "target file is not found" << std::endl;
    return NOT_FOUND_404;
  }
  if (!is_accessible(file_path, W_OK)) {
    std::cerr << "process can not delete target file" << std::endl;
    return FORBIDDEN_403;
  }
  if (!remove_file(file_path)) {
    std::cerr << "unknown error while deleting file" << std::endl;
    return INTERNAL_SERVER_ERROR_500;
  }
  std::cerr << "deleted file successfully" << std::endl;
  return NO_CONTENT_204;
}
