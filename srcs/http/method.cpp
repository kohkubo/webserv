#include <fstream>
#include <string>

#include "http.hpp"
#include "util.hpp"
#include "config/ServerConfig.hpp"

static int response_status(ServerConfig      &server_config,
                           const std::string &path) {
  if (server_config.status_code_ == 404) {
    return 404;
  }
  std::ifstream file(path);
  if (file.fail()) {
    // TODO: エラーによってステータス変える?
    std::cout << "internal error?" << std::endl;
    return 500;
  }
  return 200;
}

/*
 * mapへの挿入時keyが被っている時の処理は現状考慮してない.
 */
http_message_map method_get(ServerConfig     &server_config,
                            http_message_map &request_message) {
  std::string      target_url = request_message[URL];
  http_message_map response_message;
  switch (response_status(server_config, target_url)) {
  case 200:
    response_message[STATUS] = STATUS_OK;
    response_message[PHRASE] = PHRASE_STATUS_OK;
    break;
  case 404:
    target_url               = NOT_FOUND_PAGE;
    response_message[STATUS] = STATUS_NOTFOUND;
    response_message[PHRASE] = PHRASE_STATUS_NOTFOUND;
    break;
  default:
    // TODO: error
    break;
  }
  std::string content            = read_file_tostring(target_url.c_str());
  response_message[BODY]         = content;
  response_message[CONTENT_LEN]  = to_string(content.size());
  response_message[CONTENT_TYPE] = TEXT_HTML;
  return response_message;
}
