#include <cstdlib>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "ServerConfig.hpp"
#include "util.hpp"

#define SPACES "\v\r\f\t\n "

ServerConfig::UnexpectedTokenException::UnexpectedTokenException(
    const std::string &msg)
    : logic_error(msg) {}

ServerConfig::ServerConfig()
    : listen_address_("0.0.0.0"), listen_port_("80"), client_max_body_size_(0) {
  root_  = "./html/";
  index_ = "index.html";
}

ServerConfig::~ServerConfig() { freeaddrinfo(info_); }

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
    throw UnexpectedTokenException("could not detect directive value.");

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
  __set_getaddrinfo();
  return pos + 2;
}

std::vector<std::string>::iterator
ServerConfig::__parse_root(std::vector<std::string>::iterator pos,
                           std::vector<std::string>::iterator end) {
  pos++;
  if (pos == end || pos + 1 == end || *(pos + 1) != ";")
    throw UnexpectedTokenException("could not detect directive value.");
  root_ = *pos;
  return pos + 2;
}

void ServerConfig::__set_getaddrinfo() {
  struct addrinfo hints;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family   = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  int error = getaddrinfo(listen_address_.c_str(), listen_port_.c_str(), &hints,
                          &info_);
  if (error) {
    std::cerr << "getaddrinfo: " << gai_strerror(error) << std::endl;
    exit(EXIT_FAILURE);
  }
}
