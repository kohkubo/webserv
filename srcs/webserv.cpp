#include <cstdlib>
#include <iostream>
#include <vector>

#include "config/ConfGroupMapGenerator.hpp"
#include "config/Config.hpp"
#include "event/Server.hpp"

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
  const char           *config_file_path = resolve_config_file(argc, argv);
  ConfGroupMapGenerator config_map_generator(config_file_path);
  std::map<listenFd, confGroup> __confgroup_map_ =
      config_map_generator.generate();
  Server server(__confgroup_map_);
  server.run_loop();
  return (0);
}
