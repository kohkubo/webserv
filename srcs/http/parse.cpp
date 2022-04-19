#include "Lexer.hpp"
#include "http.hpp"

/*
 * 超安易パース
 * リクエストメッセージのターゲットURLが"/"の時index.htmlに差し替えることだけしています.
 * mapへの挿入時keyが被っている時の処理は現状考慮してない.
 */
http_message http_parse_request_message(Lexer &request_lexer) {
  Lexer::token_iterator it = request_lexer.begin();
  http_message          http_request_message;
  http_request_message[METHOD] = *it; // *it = "http_method_read_get"
  it++;
  it++;
  std::string target_path = *it; // *it = "/"
  if (target_path == "/")
    target_path = HELLO_WORLD_PAGE;
  http_request_message[URL] = target_path;
  it++;
  it++;
  http_request_message[VERSION]   = *it; // *it = "HTTP/1.1"
  http_request_message[HOST]      = "example.com";
  http_request_message[USERAGENT] = "curl/7.79.1";
  http_request_message[ACCEPT]    = "*/*";
  return http_request_message;
}
