#include "http/method/delete_method.hpp"
#include "http/const/const_html_filename.hpp"
#include "http/const/const_response_key_map.hpp"
#include "http/const/const_status_phrase.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/http_parser_utils.hpp"
#include <iostream>
#include <unistd.h>

static bool has_body(const HttpMessage &request_message) {
  // TODO: chunked
  return request_message.content_length_ != 0;
}

static HttpStatusCode delete_target_file(const HttpMessage &request_message,
                                         const std::string &target_filepath) {
  if (has_body(request_message)) {
    std::cerr << "DELETE with body is unsupported" << std::endl;
    return BAD_REQUEST_400;
  }
  if (!is_file_exists(target_filepath.c_str())) {
    std::cerr << "target file is not found" << std::endl;
    return NOT_FOUND_404;
  }
  if (!check_access(target_filepath, W_OK)) {
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

http_message_map delete_method_handler(const ServerConfig &server_config,
                                       HttpMessage        &request_message) {
  http_message_map response_message;
  // TODO: 相対パス等バリデーション
  std::string      target_filepath =
      resolve_url(server_config, request_message.url_);

  HttpStatusCode code = delete_target_file(request_message, target_filepath);
  set_status_and_path(response_message, server_config, code);
  return response_message;
}