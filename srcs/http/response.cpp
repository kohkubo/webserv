#include "http/response.hpp"

/*
 * ステータスラインの要素は必須だが, 存在しなかった時のバリデートは現状してない
 */
static std::string status_line_message(message_type &response) {
  return response[VERSION] + SP + response[STATUS] + SP + response[PHRASE] +
         CRLF;
}

static std::string field_line(message_type &response, const std::string key) {
  if (response.find(key) == response.end())
    return "";
  else
    return key + ": " + response[key] + CRLF;
}

static std::string field_message(message_type &response) {
  return field_line(response, HOST) + field_line(response, CONTENT_LEN) +
         field_line(response, CONTENT_TYPE) + field_line(response, CONNECTION);
}

static std::string body_message(message_type &response) {
  return response[BODY];
}

/*
 * 引数responseの型は const message_type& で受け取りたかったが
 * map[key]でのアクセス方法はconstに用意されていないみたいなので現状このまま
 */
std::string response_message(message_type &response) {
  return status_line_message(response) + field_message(response) + CRLF +
         body_message(response);
}
