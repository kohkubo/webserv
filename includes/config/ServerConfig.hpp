#ifndef INCLUDES_CONFIG_SERVERCONFIG_HPP
#define INCLUDES_CONFIG_SERVERCONFIG_HPP

#include <stdexcept>
#include <string>
#include <vector>

#include "Lexer.hpp"
#include "Location.hpp"

class ServerConfig {
public:
  std::string              listen_ip_;
  int                      listen_port_;
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
  Lexer::token_iterator parse(Lexer::token_iterator pos,
                              Lexer::token_iterator end);
};

#endif /* INCLUDES_CONFIG_SERVERCONFIG_HPP */
