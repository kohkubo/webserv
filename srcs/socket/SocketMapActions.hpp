#ifndef SRCS_SOCKET_SOCKETMAPACTIONS_HPP
#define SRCS_SOCKET_SOCKETMAPACTIONS_HPP

#include "socket/SocketMap.hpp"
#include "socket/SocketMapAction.hpp"

#include <vector>

class SocketMapActions {
private:
  std::vector<SocketMapAction> _socket_map_action_vector_;

public:
  typedef std::vector<SocketMapAction>::iterator       iterator;
  typedef std::vector<SocketMapAction>::const_iterator const_iterator;

public:
  SocketMapActions() {}
  ~SocketMapActions() {}
  void add_socket_map_action(SocketMapAction socket_map_action) {
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

#endif /* SRCS_SOCKET_SOCKETMAPACTIONS_HPP */
