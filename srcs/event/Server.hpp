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

private:
  Server();
  Server(Server const &other);
  Server &operator=(Server const &other);
  void    __reset_pollfds() {
       __pollfds_.clear();
       __add_listenfd_to_pollfds();
       __add_connfd_to_pollfds();
  }
  void __add_listenfd_to_pollfds();
  void __add_connfd_to_pollfds();
  void __connection_receive_handler(connFd conn_fd);
  void __connection_send_handler(connFd conn_fd);
  void __insert_connection_map(connFd conn_fd);

public:
  Server(std::map<listenFd, confGroup> &__confgroup_map_)
      : __conf_group_map_(__confgroup_map_) {}
  ~Server() {}
  void run_loop();
};

#endif /* SRCS_EVENT_SERVER_HPP */
