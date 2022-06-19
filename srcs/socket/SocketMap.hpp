#ifndef SRCS_SOCKET_SOCKETMAP_HPP
#define SRCS_SOCKET_SOCKETMAP_HPP

#include <poll.h>

#include "config/Config.hpp"
#include "socket/SocketBase.hpp"
#include "socket/SocketMapOp.hpp"

class SocketMap {
private:
  serverList                  _server_list_;
  std::map<int, SocketBase *> _socket_map_;
  std::vector<struct pollfd>  _pollfds_;

private:
  SocketMap();
  SocketMap(const SocketMap &other);
  SocketMap                  &operator=(const SocketMap &other);
  void                        _read_config(const char *config_file_path);
  std::map<int, SocketBase *> _generate();

public:
  SocketMap(const char *config_file_path);
  ~SocketMap(){};
  std::size_t    pollfd_size() { return _pollfds_.size(); }
  struct pollfd *pollfds();
  void           do_map_operation(const SocketMapOp &socket_map_op);
};

#endif /* SRCS_SOCKET_SOCKETMAP_HPP */
