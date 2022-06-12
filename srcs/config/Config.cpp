#include "config/Config.hpp"

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>

#include "utils/utils.hpp"

Config::UnexpectedTokenException::UnexpectedTokenException(
    const std::string &msg)
    : logic_error(msg) {}

Config::Config()
    : listen_address_("0.0.0.0")
    , listen_port_("80")
    , client_max_body_size_(1024)
    , server_name_("")
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
  error_pages_          = other.error_pages_;
  locations_            = other.locations_;
  __set_getaddrinfo(listen_address_, listen_port_, &addrinfo_);
  return *this;
}

Config::~Config() { freeaddrinfo(addrinfo_); }

tokenIterator Config::__parse(tokenIterator pos, tokenIterator end) {
  pos++;
  if (*pos++ != "{")
    throw UnexpectedTokenException("server directive does not have context.");
  while (pos != end && *pos != "}") {
    tokenIterator head = pos;
    // clang-format off
    pos = __parse_location(pos, end);
    pos = __parse_listen(pos, end);
    pos = __parse_string_directive("server_name", server_name_, pos, end);
    pos = __parse_sizet_directive("client_max_body_size", client_max_body_size_, pos, end);
    pos = __parse_map_directive("error_page", error_pages_, pos, end);
    // clang-format on
    if (pos == head) {
      throw UnexpectedTokenException("parse server directive failed.");
    }
  }
  if (pos == end)
    throw UnexpectedTokenException("could not detect context end.");
  if (locations_.size() == 0)
    throw UnexpectedTokenException("could not detect location directive.");
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
  __set_getaddrinfo(listen_address_, listen_port_, &addrinfo_);
  return pos + 2;
}

static bool
is_location_path_duplication(const std::string           &location_path,
                             const std::vector<Location> &locations) {
  std::vector<Location>::const_iterator it = locations.begin();
  for (; it != locations.end(); it++) {
    if (it->location_path_ == location_path) {
      return true;
    }
  }
  return false;
}

tokenIterator Config::__parse_location(tokenIterator pos, tokenIterator end) {
  if (*pos != "location")
    return pos;
  Location location;
  location.limit_except_.clear(); // TODO: 雑だけど他に思いつかない。案ほしい。
  pos++;
  if (pos == end)
    throw UnexpectedTokenException("could not detect directive value.");
  location.location_path_ = *pos++;
  if (pos == end || *pos != "{")
    throw UnexpectedTokenException("could not detect context.");
  pos++;
  while (pos != end && *pos != "}") {
    tokenIterator head = pos;
    // clang-format off
    pos = __parse_string_directive("root", location.root_, pos, end);
    pos = __parse_string_directive("index", location.index_, pos, end);
    pos = __parse_bool_directive("autoindex", location.autoindex_, pos, end);
    pos = __parse_map_directive("return", location.return_, pos, end);
    pos = __parse_vector_directive("limit_except", location.limit_except_, pos, end);
    pos = __parse_bool_directive("cgi_extension", location.cgi_extension_, pos, end);
    pos = __parse_bool_directive("upload_file", location.upload_file_, pos, end);
    // clang-format on
    if (pos == head) {
      throw UnexpectedTokenException("parse location directive failed.");
    }
  }
  if (pos == end)
    throw UnexpectedTokenException("could not detect context end.");
  // TODO: 雑だけど他に思いつかない。案ほしい。
  if (location.limit_except_.size() == 0) {
    location.limit_except_.push_back("GET");
    location.limit_except_.push_back("POST");
    location.limit_except_.push_back("DELETE");
  }
  if (has_suffix(location.index_, "/"))
    throw UnexpectedTokenException("index directive failed.");
  if (!has_suffix(location.root_, "/"))
    throw UnexpectedTokenException("root directive failed.");
  if (is_location_path_duplication(location.location_path_, locations_))
    throw UnexpectedTokenException("location path duplication.");
  if (is_minus_depth(location.location_path_) ||
      is_minus_depth(location.root_) || is_minus_depth(location.index_))
    throw UnexpectedTokenException("minus depth path failed.");
  locations_.push_back(location);
  return ++pos;
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

void Config::__set_getaddrinfo(const std::string &host, const std::string &port,
                               struct addrinfo **addrinfo) {
  struct addrinfo hints = {};

  hints.ai_family       = AF_INET;
  hints.ai_socktype     = SOCK_STREAM;
  int error = getaddrinfo(host.c_str(), port.c_str(), &hints, addrinfo);
  if (error != 0) {
    ERROR_LOG("getaddrinfo: " << gai_strerror(error));
    exit(EXIT_FAILURE);
  }
}
