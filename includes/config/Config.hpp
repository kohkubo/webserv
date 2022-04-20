#ifndef INCLUDES_CONFIG_CONFIG_HPP
#define INCLUDES_CONFIG_CONFIG_HPP

#include <stdexcept>
#include <string>
#include <vector>

#include "Tokens.hpp"
#include "config/ServerConfig.hpp"

class Config {
public:
  ServerConfig server_config_;
  // デフォルトエラーページ、ポート必要？

public:
  class ParseError : public std::logic_error {
  public:
    ParseError(const std::string &msg = "encountered parsing error.");
  };

public:
  Config(Tokens &config_tokens);
  ~Config() {}

private:
  Config() {}
  Config &operator=(const Config &other);
  Config(const Config &other);
  void __parse(Tokens &config_tokens);
};

#endif /* INCLUDES_CONFIG_CONFIG_HPP */
