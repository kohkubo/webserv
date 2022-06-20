#ifndef SRCS_EVENT_SERVER_HPP
#define SRCS_EVENT_SERVER_HPP

#include "config/ConfigGroup.hpp"
#include "socket/SocketMap.hpp"

class Server {
private:
  SocketMap _socket_map_;

private:
  Server() {}

public:
  Server(const std::vector<ConfigGroup> &config_group_list)
      : _socket_map_(config_group_list) {}
  ~Server() {}
  void run_loop();
};

#endif /* SRCS_EVENT_SERVER_HPP */
