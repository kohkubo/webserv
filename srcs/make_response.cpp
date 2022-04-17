#include "message.hpp"
#include "util.hpp"
#include <iostream>
#include <map>

/*
 * サーバー処理
 * メソッドはGETであることが前提.
 * mapへの挿入時keyが被っている時の処理は現状考慮してない.
 * レスポンスに必要な要素を埋める.
 */
void method_get(std::map<std::string, std::string> &request, std::map<std::string, std::string> &response) {
  /* 対象ファイルから内容読み込み */
  std::string target_url = request[URL];
  if (is_file_exists(target_url.c_str())) {
    response[STATUS] = STATUS_OK;
    response[PHRASE] = PHRASE_STATUS_OK;
  } else {
    target_url       = NOT_FOUND_PAGE;
    response[STATUS] = STATUS_NOTFOUND;
    response[PHRASE] = PHRASE_STATUS_NOTFOUND;
  }
  std::string content       = read_file_tostring(target_url.c_str());
  response[BODY]         = content;
  response[CONTENT_LEN]  = to_string(content.size());

  /* その他必要なものをレスポンスに追加 */
  response[VERSION]      = VERSION_HTTP;
  response[CONTENT_TYPE] = TEXT_HTML;
  response[CONNECTION]   = CONNECTION_CLOSE;
}

/* ステータスラインの要素は必須だが, 存在しなかった時のバリデートは現状してない
 */
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
