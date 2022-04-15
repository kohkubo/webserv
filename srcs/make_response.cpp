#include "message.hpp"
#include "util.hpp"
#include <iostream>

/*
 * サーバー処理
 * GETのみ対応.
 * 挿入時keyが被っている時の処理は現状考慮してない.
 * レスポンスに必要な要素を埋める.
 */
void process() {
  /* 対象ファイルから内容読み込み */
  std::string target_url = request_[URL];
  if (is_file_exists(target_url.c_str())) {
    response_[STATUS] = STATUS_OK;
    response_[PHRASE] = PHRASE_STATUS_OK;
  } else {
    target_url          = NOT_FOUND_PAGE;
    response_[STATUS] = STATUS_NOTFOUND;
    response_[PHRASE] = PHRASE_STATUS_NOTFOUND;
  }
  std::string content       = read_file_tostring(target_url.c_str());
  response_[BODY]         = content;
  response_[CONTENT_LEN]  = to_string(content.size());

  /* その他必要なものをレスポンスに追加 */
  response_[VERSION]      = VERSION_HTTP;
  response_[CONTENT_TYPE] = TEXT_HTML;
  response_[CONNECTION]   = CONNECTION_CLOSE;
}

std::string message() {
  return status_line_message() + field_message() + CRLF + body_message();
}

/* ステータスラインの要素は必須だが, 存在しなかった時のバリデートは現状してない
 */
std::string status_line_message() {
  return response_[VERSION] + SP + response_[STATUS] + SP +
         response_[PHRASE] + CRLF;
}

std::string field_message() {
  return field_line(HOST) + field_line(CONTENT_LEN) +
         field_line(CONTENT_TYPE) + field_line(CONNECTION);
}

std::string field_line(std::string key) {
  if (request_.find(key) == request_.end())
    return "";
  else
    return key + ": " + request_[key] + CRLF;
}

std::string body_message() { return response_[BODY]; }
