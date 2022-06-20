#include "event/SocketMap.hpp"

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

SocketMap::SocketMap(const ConfigList &config_list)
    : _socket_map_(_generate(config_list)) {}

static bool is_same_socket(const Config &serv_x, const Config &serv_y) {
  struct sockaddr_in *x = NULL;
  struct sockaddr_in *y = NULL;
  x = reinterpret_cast<sockaddr_in *>(serv_x.addrinfo_->ai_addr);
  y = reinterpret_cast<sockaddr_in *>(serv_y.addrinfo_->ai_addr);
  return (x->sin_addr.s_addr == y->sin_addr.s_addr) &&
         (x->sin_port == y->sin_port);
}

static std::map<int, SocketBase *>::iterator
find_same_socket(const Config &conf, std::map<int, SocketBase *> &socket_map) {
  std::map<int, SocketBase *>::iterator it = socket_map.begin();
  for (; it != socket_map.end(); it++) {
    if (is_same_socket(
            conf, *(dynamic_cast<ListenSocket *>(it->second))->conf_group_[0]))
      break;
  }
  return it;
}

static bool is_include_same_server_name(const Config &conf,
                                        confGroup     conf_group) {
  confGroup::iterator it = conf_group.begin();
  for (; it != conf_group.end(); it++) {
    if ((*it)->server_name_ == conf.server_name_)
      return true;
  }
  return false;
}

std::map<int, SocketBase *>
SocketMap::_generate(const ConfigList &config_list) {
  std::map<int, SocketBase *> socket_map;
  ConfigList::const_iterator  cl_it = config_list.begin();
  for (; cl_it != config_list.end(); cl_it++) {
    std::map<int, SocketBase *>::iterator it =
        find_same_socket(*cl_it, socket_map);
    if (it != socket_map.end()) {
      if (is_include_same_server_name(
              *cl_it,
              (dynamic_cast<ListenSocket *>(it->second))->conf_group_)) {
        LOG("server_name conflicts.");
        continue;
      }
      (dynamic_cast<ListenSocket *>(it->second))
          ->conf_group_.push_back(&(*cl_it));
    } else {
      SocketBase *listen_socket = new ListenSocket(&(*cl_it));
      socket_map.insert(
          std::make_pair(listen_socket->socket_fd(), listen_socket));
    }
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
      // 現状、clientSocketしかこの条件に入らないためdynamic_castでclose
      // CGI追加時に仮想関数に
      dynamic_cast<ClientSocket *>(socket)->close();
      it++;
      do_socket_map_action(
          SocketMapAction(SocketMapAction::DELETE, socket_fd, socket));
    } else {
      it++;
    }
  }
}
