#include <iostream>
#include <stdlib.h>

#include "ServerConfig.hpp"
#include "Webserv.hpp"
#include "event.hpp"

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
  const char *config_file_path = get_config_file_path(argc, argv);
  // ServerConfig server_config    = read_config_file(config_file_path);
  // server_io_multiplexing(server_config);
  (void)config_file_path;
  std::vector<ServerConfig> server_list;
  server_list.push_back(ServerConfig());
  server_list.push_back(ServerConfig());
  server_list.push_back(ServerConfig());
  server_list[0].listen_port_ = "5500";
  server_list[1].listen_port_ = "5500";
  server_list[2].listen_port_ = "5001";
  listen_event(server_list);
  return (0);
}
