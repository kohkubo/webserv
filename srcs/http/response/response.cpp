#include "http/response/response.hpp"

#include <sys/socket.h>

#include <iostream>
#include <string>

#include "http/const/const_delimiter.hpp"
#include "http/const/const_html_filename.hpp"
#include "http/const/const_response_key_map.hpp"
#include "http/const/const_status_phrase.hpp"
#include "http/method/method.hpp"
#include "http/method/method_utils.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/utils.hpp"

/*
 * ステータスラインの要素は必須だが, 存在しなかった時のバリデートは現状してない
 */
static std::string reponse_startline(http_message_map &response_info) {
  return response_info[VERSION] + SP + response_info[STATUS_PHRASE] + CRLF;
}

static std::string response_header(http_message_map &response_info,
                                   const std::string key) {
  if (response_info.find(key) == response_info.end())
    return "";
  return key + ": " + response_info[key] + CRLF;
}

// clang-format off
static std::string response_headers(http_message_map &response_info) {
  return \
    response_header(response_info, "Host") + \
    response_header(response_info, "Content-Length") + \
    response_header(response_info, "Content-Type") + \
    response_header(response_info, "Connection");
}
// clang-format on

static std::string empty_line() { return CRLF; }

static std::string response_body(http_message_map &response_info) {
  return response_info[BODY];
}

/*
 * 引数responseの型は const http_message_map& で受け取りたかったが
 * map[key]でのアクセス方法はconstに用意されていないみたいなので現状このまま
 */
// clang-format off
std::string make_message_string(http_message_map &response_info) {
  return \
    reponse_startline(response_info) + \
    response_headers(response_info) + \
    empty_line() + \
    response_body(response_info);
}
// clang-format on

static std::string response_body_content(const std::string &path) {
  if (is_match_suffix_string(path, ".sh")) {
    return read_file_tostring_cgi(path);
  }
  return read_file_tostring(path);
}

void set_response_body(http_message_map &response_info) {
  if (response_info[STATUS_PHRASE] == STATUS_204_PHRASE ||
      response_info[STATUS_PHRASE] == STATUS_304_PHRASE) {
    return;
  }
  response_info[BODY]         = response_body_content(response_info[PATH]);
  response_info[CONTENT_LEN]  = to_string(response_info[BODY].size());
  response_info[CONTENT_TYPE] = TEXT_HTML;
}

std::string create_response_string(const Config &config,
                                      RequestInfo  &request_info) {
  http_message_map response_info;
  switch (request_info.method_) {
  case GET:
    response_info = method_get(config, request_info);
    break;
  case DELETE:
    response_info = delete_method_handler(config, request_info);
    break;
  // case POST:
  //   break;
  default:
    break;
  }
  response_info[VERSION]    = VERSION_HTTP;     // TODO: 別関数に実装
  response_info[CONNECTION] = CONNECTION_CLOSE; // TODO: 別関数に実装

  set_response_body(response_info);

  return make_message_string(response_info);
}
