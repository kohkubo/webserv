#include "config/ServerConfig.hpp"
#include "util.hpp"

#include <algorithm>

#define SPACES "\v\r\f\t\n "

ServerConfig::UnexpectedTokenException::UnexpectedTokenException(
    const std::string &msg)
    : logic_error(msg) {}

ServerConfig::ServerConfig() : listen_ip_("0.0.0.0"), listen_port_("5001") {}

Lexer::token_iterator ServerConfig::parse(Lexer::token_iterator pos,
                                          Lexer::token_iterator end) {
  pos++;
  pos = Lexer::skip_delimiter(pos, end, SPACES);
  if (*pos != "{")
    throw UnexpectedTokenException("server directive does not have context.");
  pos++;
  while (pos != end) {
    pos = Lexer::skip_delimiter(pos, end, SPACES);
    if (pos == end || *pos == "}")
      break;
    if (*pos == "listen") {
      pos = __parse_listen(pos, end);
    } else if (*pos == "root") {
      pos = __parse_root(pos, end);
    } else {
      throw UnexpectedTokenException();
    }
  }
  if (pos == end)
    throw UnexpectedTokenException("could not detect context end.");
  pos++;
  return pos;
}

Lexer::token_iterator ServerConfig::__parse_listen(Lexer::token_iterator pos,
                                                   Lexer::token_iterator end) {
  pos = Lexer::skip_delimiter(++pos, end, SPACES);
  if (pos == end || pos + 1 == end || *(pos + 1) != ";")
    throw UnexpectedTokenException("could not detect directice value.");

  Lexer                 l(*pos, ":");
  Lexer::token_iterator it = l.begin();
  while (it != l.end()) {
    if (is_digits(*it)) {
      listen_port_ = *it;
    } else if (is_ip(*it)) {
      listen_ip_ = *it;
    } else if (*it != ":") {
      listen_host_ = *it;
    }
    it++;
  }

  return pos + 2;
}

Lexer::token_iterator ServerConfig::__parse_root(Lexer::token_iterator pos,
                                                 Lexer::token_iterator end) {
  pos = Lexer::skip_delimiter(++pos, end, SPACES);
  if (pos == end || pos + 1 == end || *(pos + 1) != ";")
    throw UnexpectedTokenException("could not detect directice value.");
  root_ = *pos;
  return pos + 2;
}
