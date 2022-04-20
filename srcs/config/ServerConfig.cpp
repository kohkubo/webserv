#include "config/ServerConfig.hpp"
#include "util.hpp"

#include <algorithm>

#define SPACES "\v\r\f\t\n "

ServerConfig::UnexpectedTokenException::UnexpectedTokenException(
    const std::string &msg)
    : logic_error(msg) {}

ServerConfig::ServerConfig() : listen_ip_("0.0.0.0"), listen_port_("5001") {}

Tokens::token_iterator ServerConfig::parse(Tokens::token_iterator pos,
                                           Tokens::token_iterator end) {
  pos++;
  if (*pos != "{")
    throw UnexpectedTokenException("server directive does not have context.");
  pos++;
  while (pos != end) {
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

Tokens::token_iterator
ServerConfig::__parse_listen(Tokens::token_iterator pos,
                             Tokens::token_iterator end) {
  pos++;
  if (pos == end || pos + 1 == end || *(pos + 1) != ";")
    throw UnexpectedTokenException("could not detect directive value.");

  Tokens                 l(*pos, ": ", " ");
  Tokens::token_iterator it = l.begin();
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

Tokens::token_iterator ServerConfig::__parse_root(Tokens::token_iterator pos,
                                                  Tokens::token_iterator end) {
  pos++;
  if (pos == end || pos + 1 == end || *(pos + 1) != ";")
    throw UnexpectedTokenException("could not detect directive value.");
  root_ = *pos;
  return pos + 2;
}
