#include <cstdlib>
#include <fstream>
#include <string>
#include <unistd.h>

#include "ServerConfig.hpp"
#include "http.hpp"
#include "util.hpp"

static HttpStatusCode set_url(const ServerConfig &server_config,
                              HttpMessage        &request_message) {
  if (request_message.status_code_ == BAD_REQUEST) {
    request_message.path_ = NOT_FOUND_PAGE;
    return BAD_REQUEST;
  }
  std::string path;
  if (request_message.url_ == "/") {
    path = server_config.root_ + server_config.index_;
  } else {
    path = server_config.root_ + path;
  }
  if (is_file_exists(path.c_str())) {
    if (access(path.c_str(), R_OK) == 0) {
      request_message.path_ = path;
      return OK;
    } else {
      request_message.path_ = NOT_FOUND_PAGE;
      return FORBIDDEN; // TODO: Permission error が 403なのか確かめてない
    }
  }
  request_message.path_ = NOT_FOUND_PAGE;
  return NOT_FOUND;
}

static void set_response_body(HttpMessage      &request_message,
                              http_message_map &response_message) {
  std::string content    = read_file_tostring(request_message.path_.c_str());
  response_message[BODY] = content;
  response_message[CONTENT_LEN]  = to_string(content.size());
  response_message[CONTENT_TYPE] = TEXT_HTML;
}

/*
 * mapへの挿入時keyが被っている時の処理は現状考慮してない.
 */
http_message_map method_get(const ServerConfig &server_config,
                            HttpMessage        &request_message) {
  http_message_map response_message;
  switch (set_url(server_config, request_message)) {
  case OK:
    response_message[STATUS] = STATUS_OK;
    response_message[PHRASE] = PHRASE_STATUS_OK;
    break;
  case FORBIDDEN:
    response_message[STATUS] = STATUS_FORBIDDEN;
    response_message[PHRASE] = PHRASE_STATUS_FORBIDDEN;
    break;
  case NOT_FOUND:
    response_message[STATUS] = STATUS_NOTFOUND;
    response_message[PHRASE] = PHRASE_STATUS_NOTFOUND;
    break;
  default:
    response_message[STATUS] = STATUS_UNKNOWNERROR;
    response_message[PHRASE] = PHRASE_STATUS_UNKNOWNERROR;
    break;
  }
  set_response_body(request_message, response_message);
  return response_message;
}
