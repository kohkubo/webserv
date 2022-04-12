#ifndef INCLUDES_CONFIG_HPP
#define INCLUDES_CONFIG_HPP

#include <stdexcept>
#include <string>
#include <vector>

#include "Lexer.hpp"
#include "ServerConfig.hpp"

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
  Config(const Config &other) {}
  Config &              operator=(const Config &other) { return *this; }
  Lexer::token_iterator __skip_token(const std::string &skip);
};

#endif /* INCLUDES_CONFIG_HPP */
