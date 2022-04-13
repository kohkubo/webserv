#include "Response.hpp"
#include "Lexer.hpp"
#include "util.hpp"
#include <iostream>

#define STATUS_OK            "200"
#define TEXT_STATUS_OK       "OK"
#define STATUS_NOTFOUND      "404"
#define TEXT_STATUS_NOTFOUND "Not Found"
#define VERSION              "HTTP/1.1"
#define HELLO_WORLD_PAGE     "./html/index.html"
#define ERR_PAGE             "./html/not_found.html"
#define CR                   "\r"
#define LF                   "\n"
#define CRLF                 "\r\n"
#define SP                   " "

/*
 * コンストラクタ
 * リクエストメッセージをパース
 * 現時点: レキサーを使ってターゲットurlのみ確認してます.
 *        "/"ならindex.htmlを, それ以外ならエラー用のファイルを指定します.
 */
Response::Response(const std::string &request_message) {
  Lexer                 l(request_message, SP);
  Lexer::token_iterator it = l.begin();

  // token: "GET", " ", "/", " ", "HTTP/1.1"...
  __request_line_.push_back(*it);
  it++;
  it++;
  if (*it == "/")
    __request_line_.push_back(HELLO_WORLD_PAGE);
  else
    __request_line_.push_back(ERR_PAGE);
  __request_line_.push_back(VERSION);

  __request_field_.push_back("Host: example.com");
  __request_field_.push_back("User-Agent: curl/7.79.1");
  __request_field_.push_back("Accept: */*");
}

/*
 * サーバー処理
 * レスポンスに必要な属性を埋める.
 */
void Response::process() {
  __status_line_.push_back(VERSION);

  // 対象ファイルから内容読み込み
  std::string target_path = __request_line_[1];
  if (is_file_exists(target_path.c_str())) {
    __status_line_.push_back(STATUS_OK);
    __status_line_.push_back(TEXT_STATUS_OK);
  } else {
    target_path = ERR_PAGE;
    __status_line_.push_back(STATUS_NOTFOUND);
    __status_line_.push_back(TEXT_STATUS_NOTFOUND);
  }
  std::string content = read_file_tostring(target_path.c_str());
  __response_body_    = content;
  __response_field_.push_back("Content-Length: " + tostring(content.size()));

  __response_field_.push_back("Content-Type: text/html");
  __response_field_.push_back("Connection: close");
}

std::string Response::message() {
  return __status_line_[0] + SP + __status_line_[1] + SP + __status_line_[2] +
         CRLF + __response_field_[0] + CRLF + __response_field_[1] + CRLF +
         __response_field_[2] + CRLF + CRLF + __response_body_;
}
