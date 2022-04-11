#ifndef INCLUDES_CONFIG_SERVERCONFIG_HPP
#define INCLUDES_CONFIG_SERVERCONFIG_HPP

#include <string>
#include <vector>

#include "Location.hpp"

class ServerConfig {
public:
  unsigned int             listen_ip_; // long?
  int                      listen_port_;
  int                      client_max_body_size_;
  std::vector<std::string> server_name_;
  std::string              root_;
  std::vector<std::string> index_;
  std::vector<Location>    location_;

  // error_page;

public:
  ServerConfig();
  ~ServerConfig();
};

ServerConfig::ServerConfig() {}

ServerConfig::~ServerConfig() {}

#endif /* INCLUDES_CONFIG_SERVERCONFIG_HPP */
