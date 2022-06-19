#ifndef SRCS_CONFIG_CONFGROUPMAPGENERATOR_HPP
#define SRCS_CONFIG_CONFGROUPMAPGENERATOR_HPP

#include "config/Config.hpp"
#include "socket/SocketBase.hpp"

class ConfGroupMapGenerator {
private:
  serverList _server_list_;

public:
  ConfGroupMapGenerator(const char *config_file_path);
  ~ConfGroupMapGenerator(){};
  std::map<int, SocketBase *> generate();

private:
  ConfGroupMapGenerator();
  ConfGroupMapGenerator(const ConfGroupMapGenerator &other);
  ConfGroupMapGenerator &operator=(const ConfGroupMapGenerator &other);
  void                   _read_config(const char *config_file_path);
};

#endif /* SRCS_CONFIG_CONFGROUPMAPGENERATOR_HPP */
