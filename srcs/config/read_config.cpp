#include "ServerConfig.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/tokenize.hpp"
#include <cstdlib>
#include <iostream>
#include <vector>

server_list_type read_config(const char *config_file_path) {
  std::string  contents = read_file_tostring(config_file_path);
  token_vector config_tokens =
      tokenize(contents, CONFIG_DELIMITER, CONFIG_SKIP);
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