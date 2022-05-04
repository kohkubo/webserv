#include "HttpMessage.hpp"
#include "config/ServerConfig.hpp"
#include "const_html_filename.hpp"
#include "const_response_key_map.hpp"
#include "const_status_phrase.hpp"
#include "utils/util.hpp"
#include <string>
#include <unistd.h>

// TODO: Requestのエラーについて、未調査です。Getにおいては、Hostだけで良さそう
bool is_request_error(HttpMessage &request_message) {
  if (request_message.host_ == "") {
    return true;
  }
  return false;
}

static HttpStatusCode check_url(const char *target_filepath) {
  if (is_file_exists(target_filepath)) {
    if (access(target_filepath, R_OK) == 0) {
      return OK_200;
    } else {
      return FORBIDDEN_403; // TODO: Permission error が 403なのか確かめてない
    }
  }
  return NOT_FOUND_404;
}

static void set_response_body(http_message_map &response_message,
                              const char       *target_filepath) {
  std::string content            = read_file_tostring(target_filepath);
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
  http_message_map response_message;
  // リクエストのエラー 判定はここがよさそう。
  // メソッドごとのエラー判定と共通のエラーで分けてもいいかも
  // hostは共通かな
  if (is_request_error(request_message)) {
    std::cout << "request error." << std::endl;
    response_message[PATH] = BAD_REQUEST_PAGE;
    return response_message;
  }
  std::string target_filepath =
      resolve_url(server_config, request_message.url_);
  switch (check_url(target_filepath.c_str())) {
  case OK_200:
    response_message[STATUS_PHRASE] = STATUS_200_PHRASE;
    break;
  case FORBIDDEN_403:
    response_message[STATUS_PHRASE] = STATUS_403_PHRASE;
    target_filepath                 = FORBIDDEN_PAGE;
    break;
  case NOT_FOUND_404:
    response_message[STATUS_PHRASE] = STATUS_404_PHRASE;
    target_filepath                 = NOT_FOUND_PAGE;
    break;
  default:
    response_message[STATUS_PHRASE] = STATUS_520_PHRASE;
    target_filepath                 = UNKNOWN_ERROR_PAGE;
    break;
  }
  set_response_body(response_message, target_filepath.c_str());
  return response_message;
}
