#include "ServerConfig.hpp"
#include "util/tokenize.hpp"
#include "util/util.hpp"

#include <cstdlib>
#include <map>
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
    , error_pages_()
    , autoindex_(true)
    , limit_except_()
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
  error_pages_          = other.error_pages_;
  autoindex_            = other.autoindex_;
  limit_except_         = other.limit_except_;
  __set_getaddrinfo();
  return *this;
}

ServerConfig::~ServerConfig() { freeaddrinfo(addrinfo_); }

token_iterator ServerConfig::parse(token_iterator pos, token_iterator end) {
  pos++;
  if (*pos++ != "{")
    throw UnexpectedTokenException("server directive does not have context.");
  while (pos != end && *pos != "}") {
    token_iterator head = pos;
    // clang-format off
    pos = __parse_listen(pos, end);
    pos = __parse_error_page(pos, end);
    pos = __parse_string_directive("index", index_, pos, end);
    pos = __parse_string_directive("root", root_, pos, end);
    pos = __parse_string_directive("server_name", server_name_, pos, end);
    pos = __parse_vector_directive("limit_except", limit_except_, pos, end);
    pos = __parse_sizet_directive("client_max_body_size", client_max_body_size_, pos, end);
    pos = __parse_bool_directive("autoindex", autoindex_, pos, end);
    // clang-format on
    if (pos == head) {
      throw UnexpectedTokenException();
    }
  }
  if (pos == end)
    throw UnexpectedTokenException("could not detect context end.");
  return ++pos;
}

token_iterator ServerConfig::__parse_listen(token_iterator pos,
                                            token_iterator end) {
  if (*pos != "listen")
    return pos;
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

// TODO: エラー処理
token_iterator ServerConfig::__parse_error_page(token_iterator pos,
                                                token_iterator end) {
  if (*pos != "error_page")
    return pos;
  pos++;
  if (pos == end || pos + 2 == end || *(pos + 2) != ";")
    throw UnexpectedTokenException("could not detect directive value.");
  error_pages_.insert(std::make_pair(std::atoi((*pos).c_str()), *(pos + 1)));
  return pos + 3;
}

token_iterator ServerConfig::__parse_string_directive(std::string    key,
                                                      std::string   &value,
                                                      token_iterator pos,
                                                      token_iterator end) {
  if (*pos != key)
    return pos;
  pos++;
  if (pos == end || pos + 1 == end || *(pos + 1) != ";")
    throw UnexpectedTokenException("could not detect directive value.");
  value = *pos;
  return pos + 2;
}

token_iterator ServerConfig::__parse_sizet_directive(std::string    key,
                                                     size_t        &value,
                                                     token_iterator pos,
                                                     token_iterator end) {
  if (*pos != key)
    return pos;
  pos++;
  if (pos == end || pos + 1 == end || *(pos + 1) != ";")
    throw UnexpectedTokenException("could not detect directive value.");
  // TODO: size_tに変換できるやり方ちゃんと調査
  value = std::atol((*pos).c_str());
  return pos + 2;
}

token_iterator ServerConfig::__parse_bool_directive(std::string    key,
                                                    bool          &value,
                                                    token_iterator pos,
                                                    token_iterator end) {
  if (*pos != key)
    return pos;
  pos++;
  if (pos == end || pos + 1 == end || *(pos + 1) != ";")
    throw UnexpectedTokenException("could not detect directive value.");
  if (*pos == "on")
    value = true;
  else if (*pos == "off")
    value = false;
  else
    throw UnexpectedTokenException("bool directive value is invalid.");
  return pos + 2;
}

token_iterator
ServerConfig::__parse_vector_directive(std::string               key,
                                       std::vector<std::string> &value,
                                       token_iterator pos, token_iterator end) {
  if (*pos != key)
    return pos;
  pos++;
  if (pos == end)
    throw UnexpectedTokenException("could not detect directive value.");
  size_t i = 0;
  for (; pos != end && *pos != ";"; pos++) {
    value.push_back(*pos);
    i++;
  }
  return pos + i + 1;
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