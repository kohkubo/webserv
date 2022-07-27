#include <signal.h>

#include <cstdlib>
#include <iostream>
#include <vector>

#include "Server.hpp"
#include "config/ConfGroupGen.hpp"

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

void init_signal() { signal(SIGPIPE, SIG_IGN); }

int main(int argc, char **argv) {
  const char *config_file_path = resolve_config_file(argc, argv);
  std::vector<config::ConfigGroup> config_groups =
      config::generate_config_group(config_file_path);

  init_signal();
  Server server(config_groups);
  server.run_loop();
  return (0);
}
