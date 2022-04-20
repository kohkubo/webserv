#include "Webserv.hpp"
#include <iostream>

#include "config/Config.hpp"
#include "Tokens.hpp"
#include "util.hpp"
#include <iostream>

Config *set_config(const char *config_file_path) {
  std::string config_content = read_file_tostring(config_file_path);
  Tokens t(config_content, "\v\r\f\t\n " "{};", "\v\r\f\t\n ");
  try
  {
    Config *c = new Config(t);
    return c;
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return NULL;
  }
}

#define DEFAULT_CONFIG_FILE_PATH "conf/webserv.conf"

int main(int argc, char **argv) {
  std::string config_file_path;
  switch (argc)
  {
    case 1:
      config_file_path = DEFAULT_CONFIG_FILE_PATH;
      break;
    case 2:
      config_file_path = argv[1];
      break;
    default:
      std::cerr << "Usage: ./webserv [config file]" << std::endl;
      return (1);
  }
  Config *c = set_config(config_file_path.c_str());
  if (c == NULL)
    return (1);
  server_io_multiplexing(*c);
  delete c;
  return (0);
}
