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

static http_message_map
response_to_bad_request(const HttpMessage &request_message) {
  http_message_map response_message;
  response_message[STATUS_PHRASE] = STATUS_400_PHRASE;
  response_message[PATH]          = BAD_REQUEST_PAGE;
  (void)request_message;
  return response_message;
}

http_message_map delete_method_handler(const ServerConfig &server_config,
                                       HttpMessage        &request_message) {
  http_message_map response_message;
  // TODO: 相対パス等バリデーション
  std::string      target_filepath =
      resolve_url(server_config, request_message.url_);
  if (has_body(request_message)) {
    std::cerr << "DELETE with body is unsupported" << std::endl;
    return response_to_bad_request(request_message);
  }
  if (!is_file_exists(target_filepath.c_str())) {
    std::cerr << "target file is not found" << std::endl;
    response_message[STATUS_PHRASE] = STATUS_404_PHRASE;
    response_message[PATH]          = NOT_FOUND_PAGE;
    return response_message;
  }
  if (!check_access(target_filepath, W_OK)) {
    std::cerr << "process can not delete target file" << std::endl;
    response_message[STATUS_PHRASE] = STATUS_403_PHRASE;
    response_message[PATH]          = FORBIDDEN_PAGE;
    return response_message;
  }
  if (remove_file(target_filepath)) {
    response_message[STATUS_PHRASE] = STATUS_204_PHRASE;
    return response_message;
  }
  response_message[STATUS_PHRASE] = STATUS_500_PHRASE;
  response_message[PATH]          = INTERNAL_SERVER_ERROR_PAGE;
  return response_message;
}