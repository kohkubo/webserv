#include <iostream>
#include <stdlib.h>

#include "config/config.hpp"
#include "util.hpp"

#define DELIMITER "\v\r\f\t\n {};"
#define SKIP      "\v\r\f\t\n "

ServerConfig generate_server_config(const char *config_file_path) {
  std::string config_content =
      read_file_tostring(config_file_path); // TODO: エラー受け取る
  std::vector<std::string> l = tokenize(config_content, DELIMITER, SKIP);
  ServerConfig             server_config;
  try {
    server_config.parse(l.begin(), l.end());
    return server_config;
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    exit(EXIT_FAILURE); // 一旦
  }
}
