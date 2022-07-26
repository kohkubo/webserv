#ifndef SRCS_CONFIG_CONFIG_HPP
#define SRCS_CONFIG_CONFIG_HPP

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstdlib>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "config/Location.hpp"
#include "config/Locations.hpp"
#include "utils/tokenize.hpp"
#include "utils/utils.hpp"

const std::string CONFIG_DELIMITER = "\v\r\f\t\n {};";
const std::string CONFIG_SKIP      = "\v\r\f\t\n ";

namespace config {

typedef std::map<int, std::string> errorPageMap;

class Config {
public:
  size_t             client_max_body_size_;
  std::string        server_name_;
  bool               has_listen_;
  struct sockaddr_in sockaddr_in_;
  errorPageMap       error_pages_;
  Locations          locations_;

public:
  Config()
      : client_max_body_size_(1024)
      , server_name_("")
      , has_listen_(false) { 
    memset(&sockaddr_in_, 0, sizeof(sockaddr_in_));
  }
  ~Config() {}
  Config(const Config &other);
  Config &operator=(const Config &other);
};

} // namespace config

#endif /* SRCS_CONFIG_CONFIG_HPP */
