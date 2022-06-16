#ifndef SRCS_EVENT_SERVER_HPP
#define SRCS_EVENT_SERVER_HPP

#include <poll.h>

#include "config/Config.hpp"
#include "event/Connection.hpp"

typedef int connFd;

class Server {
private:
  std::map<listenFd, confGroup> _conf_group_map_;
  std::map<connFd, Connection>  _connection_map_;
  std::vector<struct pollfd>    _pollfds_;

private:
  static void
  _close_timedout_connection(std::map<connFd, Connection> &connection_map);

  Server();
  Server(Server const &other);
  Server &operator=(Server const &other);
  void    _reset_pollfds() {
    _pollfds_.clear();
    _add_listenfd_to_pollfds(_conf_group_map_);
    _add_connfd_to_pollfds(_connection_map_);
  }
  void
  _add_listenfd_to_pollfds(const std::map<listenFd, confGroup> &conf_group_map);
  void
  _add_connfd_to_pollfds(const std::map<connFd, Connection> &connection_map);
  void _connection_receive_handler(Connection &connection);
  void _connection_send_handler(connFd conn_fd);
  void _insert_connection_map(connFd conn_fd);

public:
  Server(std::map<listenFd, confGroup> &conf_group_map)
      : _conf_group_map_(conf_group_map) {}
  ~Server() {}
  void run_loop();
};

#endif /* SRCS_EVENT_SERVER_HPP */
