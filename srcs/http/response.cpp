#include <iostream>
#include <string>
#include <sys/socket.h>

#include "ServerConfig.hpp"
#include "http.hpp"

/*
 * ステータスラインの要素は必須だが, 存在しなかった時のバリデートは現状してない
 */
// clang-format off
static std::string reponse_startline(http_message_map &response_message) {
  return \
    response_message[VERSION] + SP + \
    response_message[STATUS] + SP + \
    response_message[PHRASE] + \
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
    response_header(response_message, HOST) + \
    response_header(response_message, CONTENT_LEN) + \
    response_header(response_message, CONTENT_TYPE) + \
    response_header(response_message, CONNECTION);
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
// TODO: 命名候補
// std::string generate_response_message(http_message_map &response_message)
// std::string get_response_message(http_message_map &response_message)
// std::string make_response_message(http_message_map &response_message)
// std::string response_message_to_string(http_message_map &response_message)
// 名前空間のことも含めて相談
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
  // case POST:
  //   break;
  // case DELETE:
  //   break;
  default:
    break;
  }
  response_message[VERSION]    = VERSION_HTTP;     // TODO: 別関数に実装
  response_message[CONNECTION] = CONNECTION_CLOSE; // TODO: 別関数に実装
  return response_message_to_string(response_message);
}
