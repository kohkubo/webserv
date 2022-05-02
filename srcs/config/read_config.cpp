#include "config/ServerConfig.hpp"
#include "util/tokenize.hpp"
#include "util/util.hpp"
#include <cstdlib>
#include <iostream>
#include <vector>

#define DELIMITER "\v\r\f\t\n {};"
#define SKIP      "\v\r\f\t\n "

std::vector<const ServerConfig *> read_config(const char *config_file_path) {
  std::string              contents      = read_file_tostring(config_file_path);
  std::vector<std::string> config_tokens = tokenize(contents, DELIMITER, SKIP);
  std::vector<const ServerConfig *> server_list;
  try {
    std::vector<std::string>::iterator it = config_tokens.begin();
    while (it != config_tokens.end()) {
      if (*it == "server") {
        ServerConfig *new_server = new ServerConfig();
        it                       = new_server->parse(it, config_tokens.end());
        server_list.push_back(new_server);
      } else {
        it++;
      }
    }
    return server_list;
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
}