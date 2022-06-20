#ifndef SRCS_SOCKET_SOCKETMAP_HPP
#define SRCS_SOCKET_SOCKETMAP_HPP

#include <poll.h>

#include "config/Config.hpp"
#include "config/ConfigList.hpp"
#include "socket/SocketBase.hpp"
#include "socket/SocketMapAction.hpp"

class SocketMap {
private:
  std::map<int, SocketBase *> _socket_map_;

private:
  SocketMap(const SocketMap &other);
  SocketMap                         &operator=(const SocketMap &other);
  static std::map<int, SocketBase *> _generate(const ConfigList &config_list);

public:
  SocketMap() {}
  SocketMap(const ConfigList &config_list);
  ~SocketMap();
  void                       close_timedout_socket();
  std::vector<struct pollfd> create_pollfds();
  SocketMapAction handle_socket_event(int socket_fd, short int revents);
  void            do_socket_map_action(const SocketMapAction &socket_map_op);
};

#endif /* SRCS_SOCKET_SOCKETMAP_HPP */
