#include "message.hpp"
#include "Lexer.hpp"
#include <iostream>

/*
 * 超安易パース
 * mapへの挿入時keyが被っている時の処理は現状考慮してない.
 */
void parse_request(Lexer &message_lexer) {
  Lexer::token_iterator it = message_lexer.begin();
  request_[METHOD]       = *it; // *it = "GET"
  it++;
  it++;
  std::string target_path = *it; // *it = "/"
  if (target_path == "/")
    target_path = HELLO_WORLD_PAGE;
  request_[URL] = target_path;
  it++;
  it++;
  request_[VERSION]   = *it; // *it = "HTTP/1.1"

  request_[HOST]      = "example.com";
  request_[USERAGENT] = "curl/7.79.1";
  request_[ACCEPT]    = "*/*";
}
