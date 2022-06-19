#ifndef SRCS_EVENT_SERVER_HPP
#define SRCS_EVENT_SERVER_HPP

#include <poll.h>

#include "config/Config.hpp"
#include "event/Connection.hpp"
#include "socket/SocketBase.hpp"
#include "socket/SocketMap.hpp"

typedef int connFd;

class Server {
private:
  SocketMap                    _socket_map_;
  std::map<connFd, Connection> _connection_map_;

private:
  static void
  _close_timedout_connection(std::map<connFd, Connection> &connection_map);

  Server();
  Server(Server const &other);
  Server &operator=(Server const &other);
  void    _connection_receive_handler(Connection &connection);
  void    _connection_send_handler(connFd conn_fd);
  void    _insert_connection_map(connFd conn_fd);

public:
  Server(const char *config_file_path)
      : _socket_map_(config_file_path) {}
  ~Server() {}
  void run_loop();
};

#endif /* SRCS_EVENT_SERVER_HPP */
