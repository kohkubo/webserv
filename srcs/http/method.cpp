#include <fstream>
#include <string>
#include <unistd.h>

#include "ServerConfig.hpp"
#include "http.hpp"
#include "util.hpp"

static int set_url(const ServerConfig &server_config,
                   http_message_map   &request_message) {
  std::string path;
  if (request_message[URL] == "/") {
    path = server_config.root_ + server_config.index_;
  } else {
    path = server_config.root_ + path;
  }
  if (is_file_exists(path.c_str())) {
    if (access(path.c_str(), R_OK) == 0) {
      request_message[URL] = path;
      return 200;
    } else {
      request_message[URL] = NOT_FOUND_PAGE;
      return 403;
    }
  }
  request_message[URL] = NOT_FOUND_PAGE;
  return 404;
}

/*
 * mapへの挿入時keyが被っている時の処理は現状考慮してない.
 */
http_message_map method_get(const ServerConfig &server_config,
                            http_message_map   &request_message) {
  std::string      target_url;
  http_message_map response_message;
  switch (set_url(server_config, request_message)) {
  case 200:
    std::cout << response_message[URL] << std::endl;
    response_message[STATUS] = STATUS_OK;
    response_message[PHRASE] = PHRASE_STATUS_OK;
    break;
  case 404:
    response_message[STATUS] = STATUS_NOTFOUND;
    response_message[PHRASE] = PHRASE_STATUS_NOTFOUND;
    break;
  default:
    // TODO: error
    break;
  }
  std::string content    = read_file_tostring(request_message[URL].c_str());
  response_message[BODY] = content;
  response_message[CONTENT_LEN]  = to_string(content.size());
  response_message[CONTENT_TYPE] = TEXT_HTML;
  return response_message;
}
