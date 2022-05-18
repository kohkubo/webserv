#include "config/Config.hpp"
#include "http/HttpMessage.hpp"
#include "http/const/const_html_filename.hpp"
#include "http/const/const_response_key_map.hpp"
#include "http/const/const_status_phrase.hpp"
#include "http/http_parser_utils.hpp"
#include "utils/file_io_utils.hpp"
#include <iostream>
#include <unistd.h>

static bool has_body(const HttpMessage &request_info) {
  // TODO: chunked
  return request_info.content_length_ != 0;
}

static HttpStatusCode delete_target_file(const HttpMessage &request_info,
                                         const std::string &target_filepath) {
  if (has_body(request_info)) {
    std::cerr << "DELETE with body is unsupported" << std::endl;
    return BAD_REQUEST_400;
  }
  if (!is_file_exists(target_filepath.c_str())) {
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
                                       HttpMessage  &request_info) {
  http_message_map response_info;
  if (is_minus_depth(request_info.url_)) {
    set_status_and_path(response_info, server_config, NOT_FOUND_404);
    return response_info;
  }
  std::string target_filepath = resolve_url(server_config, request_info.url_);

  HttpStatusCode code = delete_target_file(request_info, target_filepath);
  set_status_and_path(response_info, server_config, code);
  return response_info;
}