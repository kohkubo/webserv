#include <iostream>
#include <stdlib.h>

#include "ServerConfig.hpp"
#include "util.hpp"

#include <map>
#include <vector>
typedef std::vector<ServerConfig> server_config_vector;
typedef std::map<int, server_config_vector> server_config_map;

#define DELIMITER "\v\r\f\t\n {};"
#define SKIP      "\v\r\f\t\n "

ServerConfig read_config_file(const char *config_file_path) {
  std::string config_content =
      read_file_tostring(config_file_path); // TODO: エラー受け取る
  std::vector<std::string> l = tokenize(config_content, DELIMITER, SKIP);// 変数名しっかり
  server_config_vector  server_config;// 変数名しっかり
  ServerConfig          server_config_tmp;// 変数名しっかり
  try {
    std::vector<std::string>::iterator it = l.begin();
    while (it != l.end()) {
      if (*it == "server") {
        it = server_config_tmp.parse(it, l.end());
        server_config.push_back(server_config_tmp);
      }
    }
    return server_config[0];// TODO: server_configを返す
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    exit(EXIT_FAILURE); // 一旦
  }
}
