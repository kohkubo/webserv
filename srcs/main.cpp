#include <iostream>
#include <stdlib.h>

#include "ServerConfig.hpp"
#include "Webserv.hpp"

#define DEFAULT_CONFIG_FILE_PATH "conf/webserv.conf"

static const char *get_config_file_path(int argc, char **argv) {
  switch (argc) {
  case 1:
    return (DEFAULT_CONFIG_FILE_PATH);
  case 2:
    return (argv[1]);
  default:
    std::cerr << "Usage: ./webserv [config file]" << std::endl;
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char **argv) {
  const char  *config_file_path = get_config_file_path(argc, argv);
  ServerConfig server_config    = read_config_file(config_file_path);
  server_io_multiplexing(server_config);
  return (0);
}
