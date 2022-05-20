#include "config/Config.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "config/config_parser_utils.hpp"

Config::UnexpectedTokenException::UnexpectedTokenException(
    const std::string &msg)
    : logic_error(msg) {}

Config::Config()
    : listen_address_("0.0.0.0")
    , listen_port_("80")
    , client_max_body_size_(0)
    , server_name_("")
    , root_("./html/")
    , index_("index.html")
    , error_pages_()
    , return_()
    , autoindex_(true)
    , limit_except_()
    , addrinfo_(NULL) {}

Config::Config(const Config &other) { *this = other; }

Config &Config::operator=(const Config &other) {
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
  return_               = other.return_;
  autoindex_            = other.autoindex_;
  limit_except_         = other.limit_except_;
  __set_getaddrinfo();
  return *this;
}

Config::~Config() { freeaddrinfo(addrinfo_); }

tokenIterator Config::parse(tokenIterator pos, tokenIterator end) {
  pos++;
  if (*pos++ != "{")
    throw UnexpectedTokenException("server directive does not have context.");
  while (pos != end && *pos != "}") {
    tokenIterator head = pos;
    // clang-format off
    pos = __parse_listen(pos, end);
    pos = __parse_string_directive("index", index_, pos, end);
    pos = __parse_string_directive("root", root_, pos, end);
    pos = __parse_string_directive("server_name", server_name_, pos, end);
    pos = __parse_vector_directive("limit_except", limit_except_, pos, end);
    pos = __parse_sizet_directive("client_max_body_size", client_max_body_size_, pos, end);
    pos = __parse_bool_directive("autoindex", autoindex_, pos, end);
    pos = __parse_map_directive("error_page", error_pages_, pos, end);
    pos = __parse_map_directive("return", return_, pos, end);
    // clang-format on
    if (pos == head) {
      throw UnexpectedTokenException("parse server directive failed.");
    }
  }
  if (pos == end)
    throw UnexpectedTokenException("could not detect context end.");
  return ++pos;
}

tokenIterator Config::__parse_listen(tokenIterator pos, tokenIterator end) {
  if (*pos != "listen")
    return pos;
  pos++;
  if (pos == end || pos + 1 == end || *(pos + 1) != ";")
    throw UnexpectedTokenException("could not detect directive value.");
  tokenVector   l  = tokenize(*pos, ": ", " ");
  tokenIterator it = l.begin();
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

tokenIterator Config::__parse_map_directive(std::string                 key,
                                            std::map<int, std::string> &value,
                                            tokenIterator               pos,
                                            tokenIterator               end) {
  if (*pos != key)
    return pos;
  pos++;
  if (pos == end || pos + 2 == end || *(pos + 2) != ";")
    throw UnexpectedTokenException("could not detect directive value.");
  value.insert(std::make_pair(std::atoi((*pos).c_str()), *(pos + 1)));
  return pos + 3;
}

tokenIterator Config::__parse_string_directive(std::string   key,
                                               std::string  &value,
                                               tokenIterator pos,
                                               tokenIterator end) {
  if (*pos != key)
    return pos;
  pos++;
  if (pos == end || pos + 1 == end || *(pos + 1) != ";")
    throw UnexpectedTokenException("could not detect directive value.");
  value = *pos;
  return pos + 2;
}

tokenIterator Config::__parse_sizet_directive(std::string key, size_t &value,
                                              tokenIterator pos,
                                              tokenIterator end) {
  if (*pos != key)
    return pos;
  pos++;
  if (pos == end || pos + 1 == end || *(pos + 1) != ";")
    throw UnexpectedTokenException("could not detect directive value.");
  // TODO: size_tに変換できるやり方ちゃんと調査
  // TODO: エラー処理
  value = std::atol((*pos).c_str());
  return pos + 2;
}

tokenIterator Config::__parse_bool_directive(std::string key, bool &value,
                                             tokenIterator pos,
                                             tokenIterator end) {
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

tokenIterator Config::__parse_vector_directive(std::string               key,
                                               std::vector<std::string> &value,
                                               tokenIterator             pos,
                                               tokenIterator             end) {
  if (*pos != key)
    return pos;
  pos++;
  if (pos == end)
    throw UnexpectedTokenException("could not detect directive value.");
  for (; pos != end && *pos != ";"; pos++) {
    value.push_back(*pos);
  }
  if (pos == end)
    throw UnexpectedTokenException("vector directive value is invalid.");
  return pos + 1;
}

void Config::__set_getaddrinfo() {
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