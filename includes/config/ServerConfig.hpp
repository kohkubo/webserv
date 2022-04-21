#ifndef INCLUDES_CONFIG_SERVERCONFIG_HPP
#define INCLUDES_CONFIG_SERVERCONFIG_HPP

#include <stdexcept>
#include <string>
#include <vector>

#include "Tokens.hpp"
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
  Tokens::token_iterator parse(Tokens::token_iterator pos,
                               Tokens::token_iterator end);

private:
  Tokens::token_iterator __parse_listen(Tokens::token_iterator pos,
                                        Tokens::token_iterator end);
  Tokens::token_iterator __parse_root(Tokens::token_iterator pos,
                                      Tokens::token_iterator end);
};

#endif  /* INCLUDES_CONFIG_SERVERCONFIG_HPP */
