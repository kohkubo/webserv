#include "ServerConfig.hpp"
#include "util.hpp"

#include <algorithm>

#define SPACES "\v\r\f\t\n "

ServerConfig::UnexpectedTokenException::UnexpectedTokenException(
    const std::string &msg)
    : logic_error(msg) {}

ServerConfig::ServerConfig() : listen_address_("0.0.0.0"), listen_port_("80") {}

std::vector<std::string>::iterator
ServerConfig::parse(std::vector<std::string>::iterator pos,
                    std::vector<std::string>::iterator end) {
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

std::vector<std::string>::iterator
ServerConfig::__parse_listen(std::vector<std::string>::iterator pos,
                             std::vector<std::string>::iterator end) {
  pos++;
  if (pos == end || pos + 1 == end || *(pos + 1) != ";")
    throw UnexpectedTokenException("could not detect directice value.");

  std::vector<std::string>           l  = tokenize(*pos, ": ", " ");
  std::vector<std::string>::iterator it = l.begin();
  while (it != l.end()) {
    if (is_digits(*it)) {
      listen_port_ = *it;
    } else if (is_ip(*it) || *it != ":") {
      listen_address_ = *it;
    }
    it++;
  }

  return pos + 2;
}

std::vector<std::string>::iterator
ServerConfig::__parse_root(std::vector<std::string>::iterator pos,
                           std::vector<std::string>::iterator end) {
  pos++;
  if (pos == end || pos + 1 == end || *(pos + 1) != ";")
    throw UnexpectedTokenException("could not detect directice value.");
  root_ = *pos;
  return pos + 2;
}
