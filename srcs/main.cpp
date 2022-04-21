#include <iostream>

#include "Webserv.hpp"
#include "config/ServerConfig.hpp"
#include "config/config.hpp"

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
