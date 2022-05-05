#include "http/method/delete_method.hpp"
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
  (void)request_message;
  return response_message;
}

http_message_map delete_method_handler(const ServerConfig &server_config,
                                       HttpMessage        &request_message) {
  if (has_body(request_message)) {
    std::cerr << "DELETE with body is unsupported" << std::endl;
    return response_to_bad_request(request_message);
  }
  http_message_map response_message;
  std::string      target_filepath =
      resolve_url(server_config, request_message.url_);
  // TODO: 相対パス等バリデーション
  if (!is_file_exists(target_filepath.c_str())) {
    std::cerr << "target file is not found" << std::endl;
    response_message[STATUS_PHRASE] = STATUS_404_PHRASE;
    // return 404
  } else if (access(target_filepath.c_str(), W_OK) == 0) {
    response_message[STATUS_PHRASE] = STATUS_403_PHRASE;
  } else if (remove_file(target_filepath)) {
    response_message[STATUS_PHRASE] = STATUS_204_PHRASE;
  } else {
    response_message[STATUS_PHRASE] = STATUS_500_PHRASE;
  }
  return response_message;
}