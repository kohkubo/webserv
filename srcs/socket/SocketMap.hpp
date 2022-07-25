#ifndef SRCS_SOCKET_SOCKETMAP_HPP
#define SRCS_SOCKET_SOCKETMAP_HPP

#include <poll.h>

#include "config/ConfigGroup.hpp"
#include "socket/SocketBase.hpp"
#include "socket/SocketMapAction.hpp"

namespace ns_socket {

class SocketMap {
private:
  typedef int                              socketFd;
  typedef std::map<socketFd, SocketBase *> socketMapType;
  socketMapType                            _socket_map_;

private:
  SocketMap(const SocketMap &other);
  SocketMap &operator=(const SocketMap &other);

public:
  SocketMap() {}
  SocketMap(const std::vector<config::ConfigGroup> &config_groups);
  ~SocketMap();
  void                       close_timedout_socket();
  std::vector<struct pollfd> create_pollfds();
  void handle_socket_event(int socket_fd, short int revents,
                           SocketMapActions &socket_map_actions);
  void do_socket_map_action(const SocketMapAction &socket_map_op);
};

} // namespace ns_socket

#endif /* SRCS_SOCKET_SOCKETMAP_HPP */
