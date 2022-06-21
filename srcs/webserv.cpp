#include <cstdlib>
#include <iostream>
#include <vector>

#include "config/ConfGroupGen.hpp"
#include "event/Server.hpp"

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
  const char              *config_file_path = resolve_config_file(argc, argv);
  std::vector<ConfigGroup> config_group_list =
      generate_config_group(config_file_path);

  Server server(config_group_list);
  server.run_loop();
  return (0);
}
