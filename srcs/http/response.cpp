#include "http.hpp"

/*
 * ステータスラインの要素は必須だが, 存在しなかった時のバリデートは現状してない
 */
// clang-format off
static std::string http_reponse_startline(http_message_map &response_message) {
  return \
    response_message[VERSION] + SP + \
    response_message[STATUS] + SP + \
    response_message[PHRASE] + \
    CRLF;
}
// clang-format on

static std::string http_response_header(http_message_map &response_message,
                                        const std::string key) {
  if (response_message.find(key) == response_message.end())
    return "";
  return key + ": " + response_message[key] + CRLF;
}

// clang-format off
static std::string http_response_headers(http_message_map &response_message) {
  return \
    http_response_header(response_message, HOST) + \
    http_response_header(response_message, CONTENT_LEN) + \
    http_response_header(response_message, CONTENT_TYPE) + \
    http_response_header(response_message, CONNECTION);
}
// clang-format on

static std::string http_empty_line() { return CRLF; }

static std::string http_response_body(http_message_map &response_message) {
  return response_message[BODY];
}

/*
 * 引数responseの型は const http_message_map& で受け取りたかったが
 * map[key]でのアクセス方法はconstに用意されていないみたいなので現状このまま
 */
// clang-format off
std::string http_response_message_get(http_message_map &response_message) {
  return \
    http_reponse_startline(response_message) + \
    http_response_headers(response_message) + \
    http_empty_line() + \
    http_response_body(response_message);
}
// clang-format on
