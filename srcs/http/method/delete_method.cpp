#include "http/method/delete_method.hpp"
#include "http/const/const_response_key_map.hpp"
#include "http/const/const_status_phrase.hpp"
#include <iostream>

static bool has_body(const HttpMessage &request_message) {
  // TODO: chunked
  return request_message.content_length_ != 0;
}

static http_message_map
response_to_bad_request(const HttpMessage &request_message) {
  http_message_map response_message;
  response_message[STATUS_PHRASE] = STATUS_400_PHRASE;
  return response_message;
}

http_message_map delete_method_handler(const ServerConfig &server_config,
                                       HttpMessage        &request_message) {
  http_message_map response_message;
  if (has_body(request_message)) {
    std::cerr << "DELETE with body is unsupported" << std::endl;
    response_message = response_to_bad_request(request_message);
  }
  (void)server_config;
  (void)request_message;
  return response_message;
}