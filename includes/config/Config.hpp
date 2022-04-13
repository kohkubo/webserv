#ifndef INCLUDES_CONFIG_CONFIG_HPP
#define INCLUDES_CONFIG_CONFIG_HPP

#include <stdexcept>
#include <string>
#include <vector>

#include "Lexer.hpp"
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
  Config(Lexer &config_lexer);
  ~Config() {}

private:
  Config() {}
  Config &operator=(const Config &other);
  Config(const Config &other);
  void __parse(Lexer &config_lexer);
};

#endif /* INCLUDES_CONFIG_CONFIG_HPP */
