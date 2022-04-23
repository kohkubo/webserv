#include <iostream>
#include <stdlib.h>

#include "ServerConfig.hpp"
#include "Webserv.hpp"

#define DEFAULT_CONFIG_FILE_PATH "conf/webserv.conf"

static const char *resolve_conf_file(int argc, char **argv) {
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
  const char  *conf_file_path   = resolve_conf_file(argc, argv);
  ServerConfig server_config    = read_server_configuration(conf_file_path);
  server_io_multiplexing(server_config);
  return (0);
}
