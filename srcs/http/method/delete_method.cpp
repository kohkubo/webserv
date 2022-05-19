#include "config/Config.hpp"
#include "http/const/const_html_filename.hpp"
#include "http/const/const_response_key_map.hpp"
#include "http/const/const_status_phrase.hpp"
#include "http/http_parser_utils.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/file_io_utils.hpp"
#include <iostream>
#include <unistd.h>

static HttpStatusCode delete_target_file(const RequestInfo &request_info,
                                         const std::string &target_filepath) {
  if (request_info.is_expected_body()) {
    std::cerr << "DELETE with body is unsupported" << std::endl;
    return BAD_REQUEST_400;
  }
  if (!is_file_exists(target_filepath)) {
    std::cerr << "target file is not found" << std::endl;
    return NOT_FOUND_404;
  }
  if (!is_accessible(target_filepath, W_OK)) {
    std::cerr << "process can not delete target file" << std::endl;
    return FORBIDDEN_403;
  }
  if (!remove_file(target_filepath)) {
    std::cerr << "unknown error while deleting file" << std::endl;
    return INTERNAL_SERVER_ERROR_500;
  }
  std::cerr << "deleted file successfully" << std::endl;
  return NO_CONTENT_204;
}

http_message_map delete_method_handler(const Config &server_config,
                                       RequestInfo  &request_info) {
  http_message_map response_info;
  if (is_minus_depth(request_info.target_)) {
    set_status_and_path(response_info, server_config, NOT_FOUND_404);
    return response_info;
  }
  std::string target_filepath =
      resolve_url(server_config, request_info.target_);

  HttpStatusCode code = delete_target_file(request_info, target_filepath);
  set_status_and_path(response_info, server_config, code);
  return response_info;
}