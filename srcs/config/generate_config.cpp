#include "config/Config.hpp"
#include "Tokens.hpp"
#include "util.hpp"
#include <iostream>

#define DELIMITER "\v\r\f\t\n {};"
#define SKIP "\v\r\f\t\n "

Config *generate_config(const char *config_file_path) {
  std::string config_content = read_file_tostring(config_file_path);// TODO: エラー受け取る
  Tokens t(config_content, DELIMITER, SKIP);
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
