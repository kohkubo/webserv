#include "Response.hpp"
#include "util.hpp"
#include <iostream>

/*
 * 超安易パース
 * 挿入時keyが被っている時の処理はいるかわからないので書いてない.
 */
void Response::__parse(Lexer &message_lexer) {
  Lexer::token_iterator it = message_lexer.begin();
  __request_[METHOD] = *it; // *it = "GET"
  it++;
  it++;
  __request_[URL] = *it;    // *it = "/"
  it++;
  it++;
  __request_[VERSION] = *it;   // *it = "HTTP/1.1"

  __request_[HOST] = "example.com";
  __request_[USERAGENT] = "curl/7.79.1";
  __request_[ACCEPT] = "*/*";
}

/*
 * サーバー処理
 * GETのみ対応
 * 対象ファイルから読み込む.
 * レスポンスに必要な要素を埋める.
 */
void Response::__process() {

  /* 対象ファイルから内容読み込み */
  std::string target_path = __request_[URL];
  if (target_path == ROOT)
    target_path = HELLO_WORLD_PAGE;
  if (is_file_exists(target_path.c_str())) {
  __response_.insert({STATUS, STATUS_OK});
  __response_.insert({PHRASE, PHRASE_STATUS_OK});
  } else {
    target_path = NOT_FOUND_PAGE;
  __response_.insert({STATUS, STATUS_NOTFOUND});
  __response_.insert({PHRASE, PHRASE_STATUS_NOTFOUND});
  }
  std::string content = read_file_tostring(target_path.c_str());
  __response_.insert({BODY, content});
  __response_.insert({CONTENT_LEN, tostring(content.size())});

  /* その他必要なものをレスポンスに追加 */
  __response_.insert({VERSION, VERSION_HTTP});
  __response_.insert({CONTENT_TYPE, TEXT_HTML});
  __response_.insert({CONNECTION, CONNECTION_CLOSE});
}

std::string Response::message() const {
  return __start_line_message() + __field_message() + CRLF + __body_message();
}

std::string Response::__start_line_message() const {
  return __response_[VERSION] + SP + __response_[STATUS] + SP + __response_[PHRASE] + CRLF;
}

//static std::string get_field_line(std::map<std::string, std::string> &field, const std::string &key) {
//  return "";
//}

std::string Response::__field_message() const {
  return "";
}

std::string Response::__body_message() const {
  return __response_[BODY];
}
