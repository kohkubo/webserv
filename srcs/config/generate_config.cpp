#include "config/Config.hpp"
#include "Tokens.hpp"
#include "util.hpp"
#include <iostream>

Config *generate_config(const char *config_file_path) {
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
