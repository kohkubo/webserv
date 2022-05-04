#include "ServerConfig.hpp"
#include "utils/tokenize.hpp"
#include "utils/util.hpp"
#include <cstdlib>
#include <iostream>
#include <vector>

#define DELIMITER "\v\r\f\t\n {};"
#define SKIP      "\v\r\f\t\n "

server_list_type read_config(const char *config_file_path) {
  std::string      contents      = read_file_tostring(config_file_path);
  token_vector     config_tokens = tokenize(contents, DELIMITER, SKIP);
  server_list_type server_list;
  try {
    token_iterator it = config_tokens.begin();
    while (it != config_tokens.end()) {
      if (*it == "server") {
        ServerConfig new_server = ServerConfig();
        it                      = new_server.parse(it, config_tokens.end());
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