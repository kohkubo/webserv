#include "config/ServerConfig.hpp"

ServerConfig::ServerConfig() : listen_ip_("0.0.0.0"), listen_port_(5001) {}

Lexer::token_iterator ServerConfig::parse(Lexer::token_iterator pos,
                                          Lexer::token_iterator end) {
  while (pos != end) {
    pos = Lexer::skip_token(pos, end, "\v\r\f\t\n ");
    if (*pos == "}") {
      pos++;
      break;
    }
    if (*pos == "listen") {
      pos = __parse_listen(pos, end);
    } else if (*pos == "root") {
      pos = __parse_root(pos, end);
    }
  }
  return pos;
}
