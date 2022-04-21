#include "Webserv.hpp"
#include "config/generate_config.hpp"
#include <iostream>

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
  Config *c = generate_config(config_file_path.c_str());
  if (c == NULL)
    return (1);
  server_io_multiplexing(*c);
  delete c;
  return (0);
}
