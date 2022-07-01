#ifndef SRCS_EVENT_SERVER_HPP
#define SRCS_EVENT_SERVER_HPP

#include "config/ConfigGroup.hpp"
#include "socket/SocketMap.hpp"

class Server {
private:
  ns_socket::SocketMap _socket_map_;

private:
  Server() {}

public:
  Server(const std::vector<ConfigGroup> &config_groups)
      : _socket_map_(config_groups) {}
  ~Server() {}
  void run_loop();
};

#endif /* SRCS_EVENT_SERVER_HPP */
