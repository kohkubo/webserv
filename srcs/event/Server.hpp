#ifndef SRCS_EVENT_SERVER_HPP
#define SRCS_EVENT_SERVER_HPP

#include "config/ConfigList.hpp"
#include "socket/SocketMap.hpp"

class Server {
private:
  ConfigList _config_list_;
  SocketMap  _socket_map_;

private:
  Server() {}

public:
  Server(const char *config_file_path)
      : _config_list_(config_file_path)
      , _socket_map_(_config_list_) {}
  ~Server() {}
  void run_loop();
};

#endif /* SRCS_EVENT_SERVER_HPP */
