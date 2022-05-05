#include "config/ServerConfig.hpp"
#include "http/HttpMessage.hpp"
#include "http/const/const_delimiter.hpp"
#include "http/const/const_response_key_map.hpp"
#include "http/method/delete_method.hpp"
#include "http/method/method.hpp"
#include <iostream>
#include <string>
#include <sys/socket.h>

/*
 * ステータスラインの要素は必須だが, 存在しなかった時のバリデートは現状してない
 */
// clang-format off
static std::string reponse_startline(http_message_map &response_message) {
  return \
    response_message[VERSION] + SP + \
    response_message[STATUS_PHRASE] + \
    CRLF;
}
// clang-format on

static std::string response_header(http_message_map &response_message,
                                   const std::string key) {
  if (response_message.find(key) == response_message.end())
    return "";
  return key + ": " + response_message[key] + CRLF;
}

// clang-format off
static std::string response_headers(http_message_map &response_message) {
  return \
    response_header(response_message, "Host") + \
    response_header(response_message, "Content-Length") + \
    response_header(response_message, "Content-Type") + \
    response_header(response_message, "Connection");
}
// clang-format on

static std::string empty_line() { return CRLF; }

static std::string response_body(http_message_map &response_message) {
  return response_message[BODY];
}

/*
 * 引数responseの型は const http_message_map& で受け取りたかったが
 * map[key]でのアクセス方法はconstに用意されていないみたいなので現状このまま
 */
// clang-format off
static std::string response_message_to_string(http_message_map &response_message) {
  return \
    reponse_startline(response_message) + \
    response_headers(response_message) + \
    empty_line() + \
    response_body(response_message);
}
// clang-format on

std::string create_response(const ServerConfig &server_config,
                            HttpMessage        &request_message) {
  http_message_map response_message;
  switch (request_message.method_) {
  case GET:
    response_message = method_get(server_config, request_message);
    break;
  case DELETE:
    response_message = delete_method_handler(server_config, request_message);
    break;
  // case POST:
  //   break;
  default:
    break;
  }
  response_message[VERSION]    = VERSION_HTTP;     // TODO: 別関数に実装
  response_message[CONNECTION] = CONNECTION_CLOSE; // TODO: 別関数に実装

  // responseに設定された、file_path, status_codeと
  // request_methodからbodyを設定する。
  // set_response_body(response_message, target_filepath);
  return response_message_to_string(response_message);
}
