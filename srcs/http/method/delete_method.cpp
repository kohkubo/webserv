#include "http/method/delete_method.hpp"

http_message_map method_delete(const ServerConfig &server_config,
                               HttpMessage        &request_message) {
  http_message_map response_message;
  (void)server_config;
  (void)request_message;
  return response_message;
}