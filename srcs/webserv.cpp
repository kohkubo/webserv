#include <cstdlib>
#include <iostream>
#include <vector>

#include "config/Config.hpp"
#include "config/create_listen_fd_map.hpp"
#include "event/Server.hpp"
#include "event/event.hpp"

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
  const char *config_file_path = resolve_config_file(argc, argv);
  server_list server_list      = read_config(config_file_path);
  std::map<listen_fd, conf_group> listen_fd_map =
      create_socket_map(server_list);
  Server server(listen_fd_map);
  server.run_loop();
  return (0);
}
