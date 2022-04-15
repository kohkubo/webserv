#include "message.hpp"
#include "Lexer.hpp"
#include <iostream>
#include <map>

/*
 * 超安易パース
 * リクエストメッセージのターゲットURLが"/"の時index.htmlに差し替えることだけしています.
 * mapへの挿入時keyが被っている時の処理は現状考慮してない.
 */
std::map<std::string, std::string> parse_request(Lexer &request_message) {
  Lexer::token_iterator it = request_message.begin();
  std::map<std::string, std::string>  request;
  request[METHOD]       = *it; // *it = "GET"
  it++;
  it++;
  std::string target_path = *it; // *it = "/"
  if (target_path == "/")
    target_path = HELLO_WORLD_PAGE;
  request[URL] = target_path;
  it++;
  it++;
  request[VERSION]   = *it; // *it = "HTTP/1.1"
  request[HOST]      = "example.com";
  request[USERAGENT] = "curl/7.79.1";
  request[ACCEPT]    = "*/*";
  return request;
}
