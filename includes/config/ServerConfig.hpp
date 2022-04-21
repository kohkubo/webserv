#ifndef INCLUDES_CONFIG_SERVERCONFIG_HPP
#define INCLUDES_CONFIG_SERVERCONFIG_HPP

#include <stdexcept>
#include <string>
#include <vector>

#include "config/Location.hpp"

class ServerConfig {
public:
  std::string              listen_address_;
  std::string              listen_port_;
  int                      client_max_body_size_;
  std::vector<std::string> server_name_;
  std::string              root_;
  std::vector<std::string> index_;
  std::vector<Location>    location_;

  // error_page;

public:
  class UnexpectedTokenException : public std::logic_error {
  public:
    UnexpectedTokenException(const std::string &msg = "unexpected token.");
  };

public:
  ServerConfig();
  ~ServerConfig(){};
  std::vector<std::string>::iterator
  parse(std::vector<std::string>::iterator pos,
        std::vector<std::string>::iterator end);

private:
  std::vector<std::string>::iterator
  __parse_listen(std::vector<std::string>::iterator pos,
                 std::vector<std::string>::iterator end);
  std::vector<std::string>::iterator
  __parse_root(std::vector<std::string>::iterator pos,
               std::vector<std::string>::iterator end);
};

#endif  /* INCLUDES_CONFIG_SERVERCONFIG_HPP */
