#ifndef SRCS_CONFIG_CONFIGMAPGENERATOR_HPP
#define SRCS_CONFIG_CONFIGMAPGENERATOR_HPP

#include "config/Config.hpp"

class ConfGroupMapGenerator {
private:
  serverList __server_list_;

public:
  ConfGroupMapGenerator(const char *config_file_path);
  ~ConfGroupMapGenerator(){};
  std::map<listenFd, confGroup> generate();

private:
  ConfGroupMapGenerator();
  ConfGroupMapGenerator(const ConfGroupMapGenerator &other);
  ConfGroupMapGenerator &operator=(const ConfGroupMapGenerator &other);
  void                   __read_config(const char *config_file_path);
};

#endif /* SRCS_CONFIG_CONFIGMAPGENERATOR_HPP */
