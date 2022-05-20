#ifndef SRCS_CONFIG_CONFIGMAPGENERATOR_HPP
#define SRCS_CONFIG_CONFIGMAPGENERATOR_HPP

#include "config/Config.hpp"

class ConfigMapGenerator {
private:
  serverList  __server_list_;

public:
  ConfigMapGenerator(const char *config_file_path);
  ~ConfigMapGenerator();
  std::map<listenFd, confGroup> generate();

private:
  ConfigMapGenerator();
  ConfigMapGenerator(const ConfigMapGenerator &other);
  ConfigMapGenerator &operator=(const ConfigMapGenerator &other);

  void __read_config(const char *config_file_path);
  void
};

#endif /* SRCS_CONFIG_CONFIGMAPGENERATOR_HPP */
