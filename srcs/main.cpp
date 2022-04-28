#include <iostream>
#include <stdlib.h>
#include <vector>

#include "ServerConfig.hpp"
#include "Webserv.hpp"

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
  std::vector<ServerConfig> server_list      = read_config(config_file_path);
  server_group_type         server_group     = create_server_group(server_list);
  server_io_multiplexing(server_list[0]); // TODO: server_group本体を渡す
  return (0);
}
