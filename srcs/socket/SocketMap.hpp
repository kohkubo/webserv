#ifndef SRCS_SOCKET_SOCKETMAP_HPP
#define SRCS_SOCKET_SOCKETMAP_HPP

#include "config/Config.hpp"
#include "socket/SocketBase.hpp"

class SocketMapGenerator {
private:
  serverList _server_list_;

public:
  SocketMapGenerator(const char *config_file_path);
  ~SocketMapGenerator(){};
  std::map<int, SocketBase *> generate();

private:
  SocketMapGenerator();
  SocketMapGenerator(const SocketMapGenerator &other);
  SocketMapGenerator &operator=(const SocketMapGenerator &other);
  void                _read_config(const char *config_file_path);
};

#endif /* SRCS_SOCKET_SOCKETMAP_HPP */
