#include "message.hpp"
#include <iostream>
#include <map>

/* ステータスラインの要素は必須だが, 存在しなかった時のバリデートは現状してない */
static std::string status_line_message(std::map<std::string, std::string> &response) {
  return response[VERSION] + SP + response[STATUS] + SP +
         response[PHRASE] + CRLF;
}

static std::string field_line(std::map<std::string, std::string> &response, const std::string key) {
  if (response.find(key) == response.end())
    return "";
  else
    return key + ": " + response[key] + CRLF;
}

static std::string field_message(std::map<std::string, std::string> &response) {
  return field_line(response, HOST) + field_line(response, CONTENT_LEN) +
         field_line(response, CONTENT_TYPE) + field_line(response, CONNECTION);
}

static std::string body_message(std::map<std::string, std::string> &response) { return response[BODY]; }

std::string response_message(std::map<std::string, std::string> &response) {
  return status_line_message(response) + field_message(response) + CRLF + body_message(response);
}
