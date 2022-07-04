#ifndef SRCS_SOCKET_SOCKETMAP_HPP
#define SRCS_SOCKET_SOCKETMAP_HPP

#include <poll.h>

#include "config/ConfigGroup.hpp"
#include "socket/SocketBase.hpp"
#include "socket/SocketMapAction.hpp"

namespace ns_socket {

class SocketMap {
private:
  std::map<int, SocketBase *> _socket_map_;

private:
  SocketMap(const SocketMap &other);
  SocketMap &operator=(const SocketMap &other);

public:
  SocketMap() {}
  SocketMap(const std::vector<ConfigGroup> &config_groups);
  ~SocketMap();
  void                       close_timedout_socket();
  std::vector<struct pollfd> create_pollfds();
  SocketMapActions handle_socket_event(int socket_fd, short int revents);
  void             do_socket_map_action(const SocketMapAction &socket_map_op);
};

} // namespace ns_socket

#endif /* SRCS_SOCKET_SOCKETMAP_HPP */
