#include "config/Config.hpp"
#include <iostream>

Config::Config(const Config &other) { (void)other; }

Config &Config::operator=(const Config &other) {
  (void)other;
  return *this;
}

Config::Config(Lexer &config_lexer) {
  try {
    __parse(config_lexer);
  } catch (const ServerConfig::UnexpectedTokenException &e) {
    throw ParseError();
  } catch (const std::exception &e) {
    throw e;
  }
}

void Config::__parse(Lexer &config_lexer) {
  Lexer::token_iterator it = config_lexer.begin();
  while (it != config_lexer.end()) {
    it = __skip_token(it, config_lexer.end(), "\v\r\f\t\n ");
    if (*it == "server") {
      it = server_config_.parse(it, config_lexer.end());
    }
  }
}

Lexer::token_iterator Config::__skip_token(Lexer::token_iterator pos,
                                           Lexer::token_iterator end,
                                           const std::string &   skip) {
  while (pos != end) {
    if (skip.find(pos[0]) == std::string::npos)
      break;
    pos++;
  }
  return pos;
}
