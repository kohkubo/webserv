#include "config/Config.hpp"

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstring>
#include <iostream>
#include <map>

#include "config/Location.hpp"
#include "config/directive_parser.hpp"

namespace config {

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
    pos = parse_string_directive("server_name", server_name_, pos, end);
    pos = parse_size_directive("client_max_body_size", client_max_body_size_, pos, end);
    pos = parse_map_directive("error_page", error_pages_, pos, end);
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

static struct sockaddr_in create_sockaddr_in(const std::string &listen_address,
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

tokenIterator Config::_parse_available_methods(Location     &location,
                                               tokenIterator pos,
                                               tokenIterator end) {
  if (*pos != "available_methods")
    return pos;
  pos++;
  if (pos == end)
    ERROR_EXIT("could not detect directive value.");
  location.has_available_methods_ = true;
  for (; pos != end && *pos != ";"; pos++) {
    if (*pos == "GET") {
      location.available_methods_.get_ = true;
    } else if (*pos == "POST") {
      location.available_methods_.post_ = true;
    } else if (*pos == "DELETE") {
      location.available_methods_.delete_ = true;
    }
  }
  if (pos == end)
    ERROR_EXIT("available methods directive value is invalid.");
  return pos + 1;
}

tokenIterator Config::_parse_location(tokenIterator pos, tokenIterator end) {
  if (*pos != "location")
    return pos;
  Location location;

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
    pos = parse_path_directive("root", location.root_, pos, end);
    pos = parse_path_directive("index", location.index_, pos, end);
    pos = parse_bool_directive("autoindex", location.autoindex_, pos, end);
    pos = parse_map_directive("return", location.return_map_, pos, end);
    pos = _parse_available_methods(location, pos, end);
    pos = parse_bool_directive("cgi_extension", location.cgi_extension_, pos, end);
    pos = parse_bool_directive("upload_file", location.upload_file_, pos, end);
    // clang-format on
    if (pos == head) {
      ERROR_EXIT("parse location directive failed.");
    }
  }
  if (pos == end)
    ERROR_EXIT("could not detect context end.");
  if (location.index_.has_suffix("/"))
    ERROR_EXIT("index directive failed. don't add \"/\" to file path.");
  if (!location.root_.has_suffix("/"))
    ERROR_EXIT("root directive failed. please add \"/\" to end of root dir");
  if (location.location_path_.is_minus_depth() ||
      location.root_.is_minus_depth() || location.index_.is_minus_depth())
    ERROR_EXIT("minus depth path failed.");

  locations_.add_or_exit(location);
  return ++pos;
}

} // namespace config
