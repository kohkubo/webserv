#include "config/Config.hpp"
#include <iostream>

Config::Config(Lexer &config_lexer) {
  Lexer::token_iterator it = config_lexer.begin();
  while (it != config_lexer.end()) {
    it = __skip_token("\v\r\f\t\n ");
    if (*it == "server") {
      try {
        it = server_config_.parse(it, config_lexer.end());
      } catch (const ServerConfig::UnexpectedTokenException &e) {
        throw ParseError();
      } catch (const std::exception &e) {
        throw e;
      }
    }
  }
}
