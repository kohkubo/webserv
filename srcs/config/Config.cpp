#include "config/Config.hpp"

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstring>
#include <iostream>
#include <map>

#include "config/directive_parser.hpp"

namespace config {

Config::directiveParserMap Config::directive_parser_map_ =
    _init_directive_parser_map();

Config::directiveParserMap Config::_init_directive_parser_map() {
  directiveParserMap directive_parser_map;
  // clang-format off
  directive_parser_map["client_max_body_size"] = &Config::parse_client_max_body_size_directive;
  directive_parser_map["listen"] = &Config::parse_listen_directive;
  directive_parser_map["error_page"] = &Config::parse_error_page_directive;
  directive_parser_map["server_name"] = &Config::parse_server_name_directive;
  directive_parser_map["location"] = &Config::parse_location_directive;
  // clang-format on
  return directive_parser_map;
}

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

static void check_config_error(const Config &config) {
  if (config.locations_.empty()) {
    ERROR_EXIT("could not detect location directive.");
  }
  if (!config.has_listen_) {
    ERROR_EXIT("could not detect listen directive.");
  }
}

tokenIterator Config::_parse(tokenIterator pos, tokenIterator end) {
  pos++;
  if (*pos++ != "{")
    ERROR_EXIT("server directive does not have context.");
  while (pos != end && *pos != "}") {
    directiveParserMap::iterator it = directive_parser_map_.find(*pos);
    if (it == directive_parser_map_.end()) {
      ERROR_EXIT("unknown directive: " + *pos);
    }
    pos = (this->*it->second)(++pos, end);
  }
  if (pos == end) {
    ERROR_EXIT("could not detect context end.");
  }
  check_config_error(*this);
  return ++pos;
}

tokenIterator Config::parse_location_directive(tokenIterator pos,
                                               tokenIterator end) {
  Location      location;
  tokenIterator result = location.parse_location(pos, end);
  locations_.add_or_exit(location);
  return result;
}

tokenIterator Config::parse_server_name_directive(tokenIterator pos,
                                                  tokenIterator end) {
  return parse_string_directive(server_name_, pos, end);
}

tokenIterator Config::parse_client_max_body_size_directive(tokenIterator pos,
                                                           tokenIterator end) {
  tokenIterator token_iterator =
      parse_size_directive(client_max_body_size_, pos, end);
  if (client_max_body_size_ > MAX_CLIENT_MAX_BODY_SIZE_) {
    ERROR_EXIT("client_max_body_size is too large.");
  }
  return token_iterator;
}

tokenIterator Config::parse_error_page_directive(tokenIterator pos,
                                                 tokenIterator end) {
  return parse_map_directive(error_pages_, pos, end);
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

tokenIterator Config::parse_listen_directive(tokenIterator pos,
                                             tokenIterator end) {
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

} // namespace config
