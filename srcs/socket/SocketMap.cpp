#include "socket/SocketMap.hpp"

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "config/ConfigGroup.hpp"
#include "socket/ListenSocket.hpp"
#include "socket/SocketMapActions.hpp"

SocketMap::SocketMap(const std::vector<ConfigGroup> &config_groups) {
  std::vector<ConfigGroup>::const_iterator it = config_groups.begin();
  for (; it != config_groups.end(); it++) {
    SocketBase *listen_socket = new ListenSocket(*it);
    _socket_map_.insert(
        std::make_pair(listen_socket->socket_fd(), listen_socket));
  }
}

SocketMap::~SocketMap() {
  std::map<int, SocketBase *>::iterator it = _socket_map_.begin();
  for (; it != _socket_map_.end(); it++) {
    delete it->second;
  }
}

std::vector<struct pollfd> SocketMap::create_pollfds() {
  std::vector<struct pollfd> pollfds;
  pollfds.clear();
  std::map<int, SocketBase *>::const_iterator it = _socket_map_.begin();
  for (; it != _socket_map_.end(); it++) {
    pollfds.push_back(it->second->pollfd());
  }
  return pollfds;
}

SocketMapActions SocketMap::handle_socket_event(int       socket_fd,
                                                short int revents) {
  return _socket_map_[socket_fd]->handle_event(revents);
}

void SocketMap::do_socket_map_action(const SocketMapAction &socket_map_action) {
  switch (socket_map_action.type_) {
  case SocketMapAction::INSERT:
    _socket_map_.insert(std::make_pair(socket_map_action.socket_fd_,
                                       socket_map_action.target_));
    break;
  case SocketMapAction::DELETE:
    delete socket_map_action.target_;
    _socket_map_.erase(socket_map_action.socket_fd_);
    break;
  default:
    break;
  }
}

void SocketMap::close_timedout_socket() {
  std::map<int, SocketBase *>::const_iterator it = _socket_map_.begin();
  while (it != _socket_map_.end()) {
    if (it->second->is_timed_out()) {
      int         socket_fd = it->first;
      SocketBase *socket    = it->second;
      it++;
      do_socket_map_action(
          SocketMapAction(SocketMapAction::DELETE, socket_fd, socket));
    } else {
      it++;
    }
  }
}
