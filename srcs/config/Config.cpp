#include "config/Config.hpp"

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>

#include "utils/utils.hpp"

Config::Config()
    : client_max_body_size_(1024)
    , server_name_("") {}

Config::Config(const Config &other) { *this = other; }

Config &Config::operator=(const Config &other) {
  if (this == &other) {
    return *this;
  }
  client_max_body_size_ = other.client_max_body_size_;
  server_name_          = other.server_name_;
  error_pages_          = other.error_pages_;
  locations_            = other.locations_;
  sockaddr_in_          = other.sockaddr_in_;
  return *this;
}

tokenIterator Config::_parse(tokenIterator pos, tokenIterator end) {
  pos++;
  if (*pos++ != "{")
    ERROR_EXIT("server directive does not have context.");
  while (pos != end && *pos != "}") {
    tokenIterator head = pos;
    // clang-format off
    pos = _parse_location(pos, end);
    pos = _parse_listen(pos, end);
    pos = _parse_string_directive("server_name", server_name_, pos, end);
    pos = _parse_size_directive("client_max_body_size", client_max_body_size_, pos, end);
    pos = _parse_map_directive("error_page", error_pages_, pos, end);
    // clang-format on
    if (pos == head) {
      ERROR_EXIT("parse server directive failed.");
    }
  }
  if (pos == end)
    ERROR_EXIT("could not detect context end.");
  if (locations_.empty())
    ERROR_EXIT("could not detect location directive.");
  if (!has_listen_) {
    ERROR_EXIT("could not detect listen directive.");
  }
  return ++pos;
}

struct sockaddr_in create_sockaddr_in(const std::string &listen_address,
                                      const std::string &listen_port) {
  struct addrinfo  hints    = {};
  struct addrinfo *addrinfo = NULL;
  hints.ai_family           = AF_INET;
  hints.ai_socktype         = SOCK_STREAM;
  int error = getaddrinfo(listen_address.c_str(), listen_port.c_str(), &hints,
                          &addrinfo);
  if (error != 0) {
    ERROR_EXIT("getaddrinfo: " << gai_strerror(error));
  }
  struct sockaddr_in *tmp = reinterpret_cast<sockaddr_in *>(addrinfo->ai_addr);
  struct sockaddr_in  sockaddr_in;
  sockaddr_in.sin_family = hints.ai_family;
  sockaddr_in.sin_addr   = tmp->sin_addr;
  sockaddr_in.sin_port   = tmp->sin_port;
  freeaddrinfo(addrinfo);
  return sockaddr_in;
}

tokenIterator Config::_parse_listen(tokenIterator pos, tokenIterator end) {
  if (*pos != "listen")
    return pos;
  pos++;
  if (pos == end || pos + 1 == end || *(pos + 1) != ";")
    ERROR_EXIT("could not detect directive value.");
  // LOG("listen: " << *pos);
  tokenVector   token_vector = tokenize(*pos, ":", "");
  tokenIterator it           = token_vector.begin();
  if (it == token_vector.end()) {
    ERROR_EXIT("could not detect listen address.");
  }
  std::string listen_address = *it++;
  // LOG("listen address: " << listen_address);
  if (it == token_vector.end()) {
    ERROR_EXIT("could not detect listen port.");
  }
  ++it;
  std::string listen_port = *it;
  sockaddr_in_            = create_sockaddr_in(listen_address, listen_port);
  has_listen_             = true;
  return pos + 2;
}

tokenIterator Config::_parse_location(tokenIterator pos, tokenIterator end) {
  if (*pos != "location")
    return pos;
  Location location;
  location.available_methods_
      .clear(); // TODO: 雑だけど他に思いつかない。案ほしい。
  pos++;
  if (pos == end)
    ERROR_EXIT("could not detect directive value.");
  location.location_path_ = *pos++;
  if (pos == end || *pos != "{")
    ERROR_EXIT("could not detect context.");
  pos++;
  while (pos != end && *pos != "}") {
    tokenIterator head = pos;
    // clang-format off
    pos = _parse_string_directive("root", location.root_, pos, end);
    pos = _parse_string_directive("index", location.index_, pos, end);
    pos = _parse_bool_directive("autoindex", location.autoindex_, pos, end);
    pos = _parse_map_directive("return", location.return_map_, pos, end);
    pos = _parse_vector_directive("available_methods", location.available_methods_, pos, end);
    pos = _parse_bool_directive("cgi_extension", location.cgi_extension_, pos, end);
    pos = _parse_bool_directive("upload_file", location.upload_file_, pos, end);
    // clang-format on
    if (pos == head) {
      ERROR_EXIT("parse location directive failed.");
    }
  }
  if (pos == end)
    ERROR_EXIT("could not detect context end.");
  // TODO: 雑だけど他に思いつかない。案ほしい。
  if (location.available_methods_.size() == 0) {
    location.available_methods_.push_back("GET");
    location.available_methods_.push_back("POST");
    location.available_methods_.push_back("DELETE");
  }
  if (has_suffix(location.index_, "/"))
    ERROR_EXIT("index directive failed. don't add \"/\" to file path.");
  if (!has_suffix(location.root_, "/"))
    ERROR_EXIT("root directive failed. please add \"/\" to end of root dir");
  if (location.location_path_.is_minus_depth() ||
      is_minus_depth(location.root_) || is_minus_depth(location.index_))
    ERROR_EXIT("minus depth path failed.");
  locations_.add_or_exit(location);
  return ++pos;
}

tokenIterator Config::_parse_map_directive(std::string                 key,
                                           std::map<int, std::string> &value,
                                           tokenIterator               pos,
                                           tokenIterator               end) {
  if (*pos != key)
    return pos;
  pos++;
  if (pos == end || pos + 2 == end || *(pos + 2) != ";")
    ERROR_EXIT("could not detect directive value.");
  Result<std::size_t> result = string_to_size(*pos);
  if (result.is_err_) {
    ERROR_EXIT("could not detect directive value.");
  }
  value.insert(std::make_pair(result.object_, *(pos + 1)));
  return pos + 3;
}

tokenIterator Config::_parse_string_directive(std::string   key,
                                              std::string  &value,
                                              tokenIterator pos,
                                              tokenIterator end) {
  if (*pos != key)
    return pos;
  pos++;
  if (pos == end || pos + 1 == end || *(pos + 1) != ";")
    ERROR_EXIT("could not detect directive value.");
  value = *pos;
  return pos + 2;
}

tokenIterator Config::_parse_size_directive(std::string key, size_t &value,
                                            tokenIterator pos,
                                            tokenIterator end) {
  if (*pos != key)
    return pos;
  pos++;
  if (pos == end || pos + 1 == end || *(pos + 1) != ";")
    ERROR_EXIT("could not detect directive value.");
  Result<std::size_t> result = string_to_size(*pos);
  if (result.is_err_)
    ERROR_EXIT("could not parse size string.");
  value = result.object_;
  return pos + 2;
}

tokenIterator Config::_parse_bool_directive(std::string key, bool &value,
                                            tokenIterator pos,
                                            tokenIterator end) {
  if (*pos != key)
    return pos;
  pos++;
  if (pos == end || pos + 1 == end || *(pos + 1) != ";")
    ERROR_EXIT("could not detect directive value.");
  if (*pos == "on")
    value = true;
  else if (*pos == "off")
    value = false;
  else
    ERROR_EXIT("bool directive value is invalid.");
  return pos + 2;
}

tokenIterator Config::_parse_vector_directive(std::string               key,
                                              std::vector<std::string> &value,
                                              tokenIterator             pos,
                                              tokenIterator             end) {
  if (*pos != key)
    return pos;
  pos++;
  if (pos == end)
    ERROR_EXIT("could not detect directive value.");
  for (; pos != end && *pos != ";"; pos++) {
    value.push_back(*pos);
  }
  if (pos == end)
    ERROR_EXIT("vector directive value is invalid.");
  return pos + 1;
}
