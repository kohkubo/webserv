#include "config/Config.hpp"

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstring>
#include <iostream>
#include <map>

#include "config/directive_parser.hpp"

namespace config {

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

} // namespace config
