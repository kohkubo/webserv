#ifndef SRCS_EVENT_SERVER_HPP
#define SRCS_EVENT_SERVER_HPP

#include "event/SocketMap.hpp"

class Server {
private:
  SocketMap _socket_map_;

private:
  Server() {}

public:
  Server(const char *config_file_path)
      : _socket_map_(config_file_path) {}
  ~Server() {}
  void run_loop();
};

#endif /* SRCS_EVENT_SERVER_HPP */
