#include "config/Config.hpp"
#include "Tokens.hpp"
#include <iostream>

Config::ParseError::ParseError(const std::string &msg) : logic_error(msg) {}

Config::Config(Tokens &config_tokens) {
  try {
    __parse(config_tokens);
  } catch (const ServerConfig::UnexpectedTokenException &e) {
    throw e;
  } catch (const std::exception &e) {
    throw e;
  }
}

void Config::__parse(Tokens &config_tokens) {
  Tokens::token_iterator it = config_tokens.begin();
  while (it != config_tokens.end()) {
    if (*it == "server") {
      it = server_config_.parse(it, config_tokens.end());
    }
  }
}
