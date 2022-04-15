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
void method_get(std::map<std::string, std::string> request, std::map<std::string, std::string> response) {
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
static std::string status_line_message() {
  return response_[VERSION] + SP + response_[STATUS] + SP +
         response_[PHRASE] + CRLF;
}

static std::string field_message() {
  return field_line(HOST) + field_line(CONTENT_LEN) +
         field_line(CONTENT_TYPE) + field_line(CONNECTION);
}

static std::string field_line(static std::string key) {
  if (request.find(key) == request.end())
    return "";
  else
    return key + ": " + request[key] + CRLF;
}

static std::string body_message() { return response_[BODY]; }

std::string response_message() {
  return status_line_message() + field_message() + CRLF + body_message();
}
