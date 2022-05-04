#include "ServerConfig.hpp"
#include "util/tokenize.hpp"
#include "util/util.hpp"

#include <cstdlib>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#define SPACES "\v\r\f\t\n "

ServerConfig::UnexpectedTokenException::UnexpectedTokenException(
    const std::string &msg)
    : logic_error(msg) {}

ServerConfig::ServerConfig()
    : listen_address_("0.0.0.0")
    , listen_port_("80")
    , client_max_body_size_(0)
    , server_name_("")
    , root_("./html/")
    , index_("index.html")
    , addrinfo_(NULL) {}

ServerConfig::ServerConfig(const ServerConfig &other) { *this = other; }

ServerConfig &ServerConfig::operator=(const ServerConfig &other) {
  if (this == &other) {
    return *this;
  }
  listen_address_       = other.listen_address_;
  listen_port_          = other.listen_port_;
  client_max_body_size_ = other.client_max_body_size_;
  server_name_          = other.server_name_;
  root_                 = other.root_;
  index_                = other.index_;
  __set_getaddrinfo();
  return *this;
}

ServerConfig::~ServerConfig() { freeaddrinfo(addrinfo_); }

token_iterator ServerConfig::parse(token_iterator pos, token_iterator end) {
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
      pos = __parse_string_directive("root", root_, pos, end);
    } else if (*pos == "server_name") {
      pos = __parse_string_directive("server_name", server_name_, pos, end);
    } else {
      throw UnexpectedTokenException();
    }
  }
  if (pos == end)
    throw UnexpectedTokenException("could not detect context end.");
  pos++;
  return pos;
}

token_iterator ServerConfig::__parse_listen(token_iterator pos,
                                            token_iterator end) {
  pos++;
  if (pos == end || pos + 1 == end || *(pos + 1) != ";")
    throw UnexpectedTokenException("could not detect directive value.");

  token_vector   l  = tokenize(*pos, ": ", " ");
  token_iterator it = l.begin();
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

token_iterator ServerConfig::__parse_string_directive(std::string    key,
                                                      std::string   &value,
                                                      token_iterator pos,
                                                      token_iterator end) {
  pos++;
  if (pos == end || pos + 1 == end || *(pos + 1) != ";")
    throw UnexpectedTokenException("could not detect directive value.");
  value = *pos;
  return pos + 2;
}

void ServerConfig::__set_getaddrinfo() {
  struct addrinfo hints;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family   = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  int error = getaddrinfo(listen_address_.c_str(), listen_port_.c_str(), &hints,
                          &addrinfo_);
  if (error) {
    std::cerr << "getaddrinfo: " << gai_strerror(error) << std::endl;
    exit(EXIT_FAILURE);
  }
}