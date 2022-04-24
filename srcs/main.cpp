#include <iostream>
#include <stdlib.h>
#include <vector>

#include "ServerConfig.hpp"
#include "Webserv.hpp"
#include "event.hpp"

#define DEFAULT_CONFIG_FILE_PATH "conf/webserv.conf"

static const char *resolve_config_file(int argc, char **argv) {
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
  const char               *config_file_path = resolve_config_file(argc, argv);
  std::vector<ServerConfig> server_list = read_config_file(config_file_path);
  listen_event(server_list);
  return (0);
}
