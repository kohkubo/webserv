#include "config/ServerConfig.hpp"

ServerConfig::UnexpectedTokenException::UnexpectedTokenException(
    const std::string &msg)
    : logic_error(msg) {}

ServerConfig::ServerConfig() : listen_ip_("0.0.0.0"), listen_port_(5001) {}

Lexer::token_iterator ServerConfig::parse(Lexer::token_iterator pos,
                                          Lexer::token_iterator end) {
  while (pos != end) {
    pos = Lexer::skip_delimiter(pos, end, "\v\r\f\t\n ");
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

Lexer::token_iterator ServerConfig::__parse_listen(Lexer::token_iterator pos,
                                                   Lexer::token_iterator end) {
  pos = Lexer::skip_delimiter(++pos, end, "\v\r\f\t\n ");
  if (pos == end)
    throw UnexpectedTokenException("could not detect directice value.");

  // parse listen dirctive

  pos++;
  if (*pos != ";")
    throw UnexpectedTokenException(
        "could not detect directive end token(\";\").");
  pos++;
  return pos;
}
Lexer::token_iterator ServerConfig::__parse_root(Lexer::token_iterator pos,
                                                 Lexer::token_iterator end) {
  pos = Lexer::skip_delimiter(++pos, end, "\v\r\f\t\n ");
  if (pos == end)
    throw UnexpectedTokenException("could not detect directice value.");
  root_ = *pos;
  pos++;
  if (*pos != ";")
    throw UnexpectedTokenException(
        "could not detect directive end token(\";\").");
  pos++;
  return pos;
}