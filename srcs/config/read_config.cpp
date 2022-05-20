#include <cstdlib>
#include <iostream>
#include <vector>

#include "config/Config.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/tokenize.hpp"

serverList read_config(const char *config_file_path) {
  std::string contents      = read_file_tostring(config_file_path);
  tokenVector config_tokens = tokenize(contents, CONFIG_DELIMITER, CONFIG_SKIP);
  serverList  server_list;
  try {
    tokenIterator it = config_tokens.begin();
    while (it != config_tokens.end()) {
      if (*it == "server") {
        Config config(it, config_tokens.end());
        it = config.get_moved_it();
        server_list.push_back(config);
      } else {
        std::cerr << "unexpected token: " << *it << std::endl;
        exit(EXIT_FAILURE);
      }
    }
    return server_list;
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
}