#ifndef SRCS_EVENT_SERVER_HPP
#define SRCS_EVENT_SERVER_HPP

#include <poll.h>

#include "config/Config.hpp"
#include "event/Connection.hpp"

typedef int connFd;

class Server {
private:
  std::map<listenFd, confGroup> __conf_group_map_;
  std::map<connFd, Connection>  __connection_map_;
  std::vector<struct pollfd>    __pollfds_;

public:
  Server(std::map<listenFd, confGroup> &conf_group_map)
      : __conf_group_map_(conf_group_map) {}
  ~Server() {}
  void run_loop();

private:
  Server();
  Server(Server const &other);
  Server &operator=(Server const &other);
  void    __reset_pollfds() {
    __pollfds_.clear();
    __add_listenfd_to_pollfds(__conf_group_map_);
    __add_connfd_to_pollfds(__connection_map_);
  }
  void __add_listenfd_to_pollfds(
      const std::map<listenFd, confGroup> &conf_group_map);
  void
  __add_connfd_to_pollfds(const std::map<connFd, Connection> &connection_map);
  void __connection_receive_handler(connFd conn_fd);
  void __connection_send_handler(connFd conn_fd);
  void __insert_connection_map(connFd conn_fd);
};

#endif /* SRCS_EVENT_SERVER_HPP */
