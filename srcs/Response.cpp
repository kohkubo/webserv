#include "Response.hpp"
#include "util.hpp"
#include <iostream>

/*
 * 現状:超安易パース
 */
void Response::__parse(Lexer &message_lexer) {
  Lexer::token_iterator it = message_lexer.begin();
  __request_.insert({METHOD, *it}); // *it = "GET"
  it++;
  it++;
  __request_.insert({URL, *it});    // *it = "/"
  it++;
  it++;
  __request_.insert({VERSION, *it});   // *it = "HTTP/1.1"

  __request_.insert({HOST, "example.com"});
  __request_.insert(USERAGENT, "curl/7.79.1");
  __request_.insert(ACCEPT, "*/*");
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

//std::string Response::message() {
//  return __status_line_[0] + SP + __status_line_[1] + SP + __status_line_[2] +
//         CRLF + __response_field_[0] + CRLF + __response_field_[1] + CRLF +
//         __response_field_[2] + CRLF + CRLF + __response_body_;
//}
