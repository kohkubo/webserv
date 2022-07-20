#ifndef SRCS_SOCKET_SOCKETMAPACTIONS_HPP
#define SRCS_SOCKET_SOCKETMAPACTIONS_HPP

#include "socket/SocketMap.hpp"
#include "socket/SocketMapAction.hpp"

#include <vector>

namespace ns_socket {

class SocketMapActions {
private:
  std::vector<SocketMapAction> _socket_map_action_vector_;

public:
  SocketMapActions() {}
  ~SocketMapActions() {}
  void add_action(SocketMapAction::ActionType type, int socket_fd,
                  SocketBase *target) {
    SocketMapAction socket_map_action(type, socket_fd, target);
    _socket_map_action_vector_.push_back(socket_map_action);
  }
  void do_action(SocketMap &socket_map) {
    std::vector<SocketMapAction>::iterator it =
        _socket_map_action_vector_.begin();
    for (; it != _socket_map_action_vector_.end(); it++) {
      socket_map.do_socket_map_action(*it);
    }
  }
};

} // namespace ns_socket

#endif /* SRCS_SOCKET_SOCKETMAPACTIONS_HPP */
