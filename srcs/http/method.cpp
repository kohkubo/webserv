#include "../ServerConfig.hpp"
#include "../util.hpp"
#include "HttpMessage.hpp"
#include "const_html_filename.hpp"
#include "const_response_key_map.hpp"
#include "const_status_phrase.hpp"
#include <cstdlib>
#include <fstream>
#include <string>
#include <unistd.h>

static HttpStatusCode check_url(const char *target_url) {
  if (is_file_exists(target_url)) {
    if (access(target_url, R_OK) == 0) {
      return OK_200;
    } else {
      return FORBIDDEN_403; // TODO: Permission error が 403なのか確かめてない
    }
  }
  return NOT_FOUND_404;
}

static void set_response_body(http_message_map &response_message,
                              const char       *target_url) {
  std::string content            = read_file_tostring(target_url);
  response_message[BODY]         = content;
  response_message[CONTENT_LEN]  = to_string(content.size());
  response_message[CONTENT_TYPE] = TEXT_HTML;
}

static std::string resolve_url(const ServerConfig &server_config,
                               const std::string   request_url) {
  if (request_url == "/") {
    return server_config.root_ + "/" + server_config.index_;
  } else {
    return server_config.root_ + "/" + request_url;
  }
}

/*
 * mapへの挿入時keyが被っている時の処理は現状考慮してない.
 */
http_message_map method_get(const ServerConfig &server_config,
                            HttpMessage        &request_message) {
  std::string target_url = resolve_url(server_config, request_message.url_);
  http_message_map response_message;
  switch (check_url(target_url.c_str())) {
  case OK_200:
    response_message[STATUS_PHRASE] = STATUS_200_PHRASE;
    break;
  case FORBIDDEN_403:
    response_message[STATUS_PHRASE] = STATUS_403_PHRASE;
    target_url                      = FORBIDDEN_PAGE;
    break;
  case NOT_FOUND_404:
    response_message[STATUS_PHRASE] = STATUS_404_PHRASE;
    target_url                      = NOT_FOUND_PAGE;
    break;
  default:
    response_message[STATUS_PHRASE] = STATUS_520_PHRASE;
    target_url                      = UNKNOWN_ERROR_PAGE;
    break;
  }
  set_response_body(response_message, target_url.c_str());
  return response_message;
}
