#ifndef SRCS_EVENT_SERVER_HPP
#define SRCS_EVENT_SERVER_HPP

#include "config/Config.hpp"
#include "event/Connection.hpp"
#include <poll.h>

typedef int connFd;

enum HandlerState {
  SEND,
  RECV,
  INSERT,
  NO_REVENTS,
  ERROR,
};

class Server {
private:
  std::map<listenFd, confGroup> __listen_fd_map_;
  std::map<int, Connection>     __conn_fd_map_;
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
  void         __add_listenfd_to_pollfds();
  void         __add_connfd_to_pollfds();
  void         __connection_receive_handler(connFd conn_fd);
  void         __connection_send_handler(connFd conn_fd);
  void         __insert_connection_map(connFd conn_fd);
  HandlerState __state(std::vector<struct pollfd>::iterator it);

public:
  Server(std::map<listenFd, confGroup> &listen_fd_map)
      : __listen_fd_map_(listen_fd_map) {}
  ~Server() {}
  void run_loop();
};

#endif /* SRCS_EVENT_SERVER_HPP */
