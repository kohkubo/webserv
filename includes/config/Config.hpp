#ifndef INCLUDES_CONFIG_HPP
#define INCLUDES_CONFIG_HPP

#include <string>
#include <vector>

#include "ServerConfig.hpp"

class Config {
public:
  ServerConfig server_config_;
  // デフォルトエラーページ、ポート必要？
public:
  Config();
  ~Config();
};

Config::Config() {}

Config::~Config() {}

#endif /* INCLUDES_CONFIG_HPP */
