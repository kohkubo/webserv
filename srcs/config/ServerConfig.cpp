#include "config/ServerConfig.hpp"

#include <algorithm>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define SPACES "\v\r\f\t\n "

ServerConfig::UnexpectedTokenException::UnexpectedTokenException(
    const std::string &msg)
    : logic_error(msg) {}

ServerConfig::ServerConfig() : listen_ip_("0.0.0.0"), listen_port_(5001) {}

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
  if (pos == end)
    throw UnexpectedTokenException("could not detect directice value.");

  Lexer                 l(*pos, ":");
  Lexer::token_iterator it = l.begin();
  while (it != l.end()) {
    if (__is_ip(*it)) {
      listen_ip_ = *it;
    } else if (__is_digits(*it)) {
      listen_port_ = std::atoi((*it).c_str());
    } else if (*it != ":") {
      listen_host_ = *it;
    }
    it++;
  }

  pos++;
  if (*pos != ";")
    throw UnexpectedTokenException(
        "could not detect directive end token(\";\").");
  pos++;
  return pos;
}

bool ServerConfig::__is_ip(const std::string &token) {
  struct in_addr inp;
  return (inet_aton(token.c_str(), &inp));
}

bool ServerConfig::__is_digits(const std::string &token) {
  for (size_t i = 0; i < token.size(); i++) {
    if (!isdigit(token[i]))
      return false;
  }
  return true;
}

Lexer::token_iterator ServerConfig::__parse_root(Lexer::token_iterator pos,
                                                 Lexer::token_iterator end) {
  pos = Lexer::skip_delimiter(++pos, end, SPACES);
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
