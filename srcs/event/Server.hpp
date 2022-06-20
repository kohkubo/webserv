#ifndef SRCS_SOCKET_SOCKETMAP_HPP
#define SRCS_SOCKET_SOCKETMAP_HPP

#include <poll.h>

#include "config/Config.hpp"
#include "socket/SocketBase.hpp"
#include "socket/SocketMapOp.hpp"

class Server {
private:
  serverList                  _server_list_;
  std::map<int, SocketBase *> _socket_map_;

private:
  Server();
  Server(const Server &other);
  Server                     &operator=(const Server &other);
  void                        _read_config(const char *config_file_path);
  std::map<int, SocketBase *> _generate();
  std::vector<struct pollfd>  _create_pollfds();
  SocketMapOp _handle_socket_event(int socket_fd, short int revents);
  void        _do_map_operation(const SocketMapOp &socket_map_op);
  void        _close_timedout_socket();

public:
  Server(const char *config_file_path);
  ~Server() {
    // TODO: map socket delete
  }
  void run_loop();
};

#endif /* SRCS_SOCKET_SOCKETMAP_HPP */
