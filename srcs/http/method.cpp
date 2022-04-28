#include "Resource.hpp"
#include "ServerConfig.hpp"
#include "http.hpp"
#include "util.hpp"
#include <cstdlib>
#include <fstream>
#include <string>
#include <unistd.h>

static HttpStatusCode check_url(const char *target_url) {
  if (is_file_exists(target_url)) {
    if (access(target_url, R_OK) == 0) {
      return OK;
    } else {
      return FORBIDDEN; // TODO: Permission error が 403なのか確かめてない
    }
  }
  return NOT_FOUND;
}

static void set_response_body(http_message_map &response_message,
                              const char       *target_url) {
  std::string content            = read_file_tostring(target_url);
  response_message[BODY]         = content;
  response_message[CONTENT_LEN]  = to_string(content.size());
  response_message[CONTENT_TYPE] = Resource::text_html_;
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
  case OK:
    response_message[STATUS] = STATUS_OK;
    response_message[PHRASE] = PHRASE_STATUS_OK;
    break;
  case FORBIDDEN:
    response_message[STATUS] = STATUS_FORBIDDEN;
    response_message[PHRASE] = PHRASE_STATUS_FORBIDDEN;
    target_url               = Resource::forbidden_page_;
    break;
  case NOT_FOUND:
    response_message[STATUS] = STATUS_NOTFOUND;
    response_message[PHRASE] = PHRASE_STATUS_NOTFOUND;
    target_url               = Resource::not_found_page_;
    break;
  default:
    response_message[STATUS] = STATUS_UNKNOWNERROR;
    response_message[PHRASE] = PHRASE_STATUS_UNKNOWNERROR;
    target_url               = Resource::unknown_error_page_;
    break;
  }
  set_response_body(response_message, target_url.c_str());
  return response_message;
}
