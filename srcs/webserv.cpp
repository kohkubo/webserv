#include <cstdlib>
#include <iostream>
#include <vector>

#include "config/Config.hpp"
#include "event/Server.hpp"
#include "socket/SocketMap.hpp"

#define DEFAULT_CONFIG_FILE_PATH "conf/webserv.conf"

static const char *resolve_config_file(int argc, char **argv) {
  switch (argc) {
  case 1:
    return (DEFAULT_CONFIG_FILE_PATH);
  case 2:
    return (argv[1]);
  default:
    ERROR_EXIT("Usage: ./webserv [config file]");
  }
}

int main(int argc, char **argv) {
  const char *config_file_path = resolve_config_file(argc, argv);
  Server      server(config_file_path);
  server.run_loop();
  return (0);
}
