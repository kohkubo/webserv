#include "config/ServerConfig.hpp"
#include "config/config.hpp"
#include "event/event.hpp"
#include <cstdlib>
#include <iostream>
#include <vector>

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
  const char      *config_file_path = resolve_config_file(argc, argv);
  server_list_type server_list      = read_config(config_file_path);
  socket_list_type socket_list      = create_socket_map(server_list);
  listen_event(socket_list);
  return (0);
}
