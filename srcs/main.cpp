#include "Webserv.hpp"
#include <iostream>


#include "ServerConfig.hpp"
#include "util.hpp"
#include <iostream>
#include <map>

#define DELIMITER "\v\r\f\t\n {};"
#define SKIP      "\v\r\f\t\n "

ServerConfig generate_server_config(const char *config_file_path) {
  std::string config_content =
      read_file_tostring(config_file_path); // TODO: エラー受け取る
  std::vector<std::string> l = tokenize(config_content, DELIMITER, SKIP);
  ServerConfig server_config;
  try {
    server_config.parse(l.begin(), l.end());
    return server_config;
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    exit(EXIT_FAILURE);// 一旦
  }
}

#define DEFAULT_CONFIG_FILE_PATH "conf/webserv.conf"

int main(int argc, char **argv) {
  std::string config_file_path;
  switch (argc) {
  case 1:
    config_file_path = DEFAULT_CONFIG_FILE_PATH;
    break;
  case 2:
    config_file_path = argv[1];
    break;
  default:
    std::cerr << "Usage: ./webserv [config file]" << std::endl;
    return (1);
  }
  ServerConfig server_config = generate_server_config(config_file_path.c_str());
  server_io_multiplexing(server_config);
  return (0);
}
