#ifndef SRCS_EVENT_SOCKETMAP_HPP
#define SRCS_EVENT_SOCKETMAP_HPP

#include <poll.h>

#include "config/Config.hpp"
#include "socket/SocketBase.hpp"
#include "socket/SocketMapAction.hpp"

class SocketMap {
private:
  serverList                  _server_list_;
  std::map<int, SocketBase *> _socket_map_;

private:
  SocketMap(const SocketMap &other);
  SocketMap                  &operator=(const SocketMap &other);
  void                        _read_config(const char *config_file_path);
  std::map<int, SocketBase *> _generate();

public:
  SocketMap() {}
  SocketMap(const char *config_file_path);
  ~SocketMap() {
    // TODO: map socket delete
  }
  void                       close_timedout_socket();
  std::vector<struct pollfd> create_pollfds();
  SocketMapAction handle_socket_event(int socket_fd, short int revents);
  void            do_socket_map_action(const SocketMapAction &socket_map_op);
};

#endif /* SRCS_EVENT_SOCKETMAP_HPP */
