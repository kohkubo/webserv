#include "Webserv.hpp"
#include <iostream>


#include "ServerConfig.hpp"
#include "util.hpp"
#include <iostream>
#include <map>

#define DELIMITER "\v\r\f\t\n {};"
#define SKIP      "\v\r\f\t\n "

std::map<std::string, ServerConfig> generate_config(const char *config_file_path) {
  std::string config_content =
      read_file_tostring(config_file_path); // TODO: エラー受け取る
  std::vector<std::string> l = tokenize(config_content, DELIMITER, SKIP);
  try {
    std::map<std::string, ServerConfig> server_config_map;
    server_config_map["tmp"] = ServerConfig();
    server_config_map["tmp"].listen_port_ = "5001"; //一時的
    return server_config_map;
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    exit(EXIT_FAILURE);// 例外投げる？
  }
}

#define DEFAULT_CONFIG_FILE_PATH "conf/webserv.conf"

int main(int argc, char **argv) {
  std::string config_file_path;
  switch (argc) {
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
  std::map<std::string, ServerConfig> server_config_map = generate_config(config_file_path.c_str());
  server_io_multiplexing(server_config_map);
  return (0);
}
