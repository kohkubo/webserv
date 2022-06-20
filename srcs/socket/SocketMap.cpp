#include "socket/SocketMap.hpp"

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include "config/Config.hpp"
#include "socket/ClientSocket.hpp"
#include "socket/ListenSocket.hpp"
#include "utils/file_io_utils.hpp"

#include "utils/tokenize.hpp"
#include "utils/utils.hpp"

SocketMap::SocketMap(const std::vector<ConfigGroup> &config_group_list)
    : _socket_map_(_generate(config_group_list)) {}

SocketMap::~SocketMap() {
  std::map<int, SocketBase *>::iterator it = _socket_map_.begin();
  for (; it != _socket_map_.end(); it++) {
    delete it->second;
  }
}

std::map<int, SocketBase *>
SocketMap::_generate(const std::vector<ConfigGroup> &config_group_list) {
  std::map<int, SocketBase *>              socket_map;
  std::vector<ConfigGroup>::const_iterator it = config_group_list.begin();
  for (; it != config_group_list.end(); it++) {
    SocketBase *listen_socket = new ListenSocket(*it);
    socket_map.insert(
        std::make_pair(listen_socket->socket_fd(), listen_socket));
  }
  return socket_map;
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

SocketMapAction SocketMap::handle_socket_event(int       socket_fd,
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
