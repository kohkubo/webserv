#include <iostream>
#include <stdlib.h>
#include <vector>

#include "ServerConfig.hpp"
#include "util.hpp"

#define DELIMITER "\v\r\f\t\n {};"
#define SKIP      "\v\r\f\t\n "

std::vector<ServerConfig> read_server_configuration(const char *config_file_path) {
  std::string config_content =
      read_file_tostring(config_file_path);
  std::vector<std::string> config_tokens = tokenize(config_content, DELIMITER, SKIP);
  std::vector<ServerConfig> server_list;
  try {
    std::vector<std::string>::iterator it = config_tokens.begin();
    while (it != config_tokens.end()) {
      if (*it == "server") {
        ServerConfig          new_server;
        it = new_server.parse(it, config_tokens.end());
        server_list.push_back(new_server);
      }
    }
    return server_list;
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    exit(EXIT_FAILURE); // 一旦
  }
}
