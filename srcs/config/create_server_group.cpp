#include "ServerConfig.hpp"
#include "event/event.hpp"
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

static bool is_same_socket(const ServerConfig &serv_x,
                           const ServerConfig &serv_y) {
  struct sockaddr_in *x, *y;
  x = reinterpret_cast<sockaddr_in *>(serv_x.addrinfo_->ai_addr);
  y = reinterpret_cast<sockaddr_in *>(serv_y.addrinfo_->ai_addr);
  return (x->sin_addr.s_addr == y->sin_addr.s_addr) &&
         (x->sin_port == y->sin_port);
}

static socket_list_type::iterator
find_same_socket(const ServerConfig &conf, socket_list_type &socket_list) {
  socket_list_type::iterator it = socket_list.begin();
  for (; it != socket_list.end(); it++) {
    if (is_same_socket(conf, *(it->second[0])))
      break;
  }
  return it;
}

static bool
is_include_same_server_name(const ServerConfig               &conf,
                            std::vector<const ServerConfig *> group) {
  std::vector<const ServerConfig *>::iterator it = group.begin();
  for (; it != group.end(); it++) {
    if ((*it)->server_name_ == conf.server_name_)
      return true;
  }
  return false;
}

socket_list_type create_socket_map(const server_list_type &server_list) {
  socket_list_type                 socket_list;
  server_list_type::const_iterator sl_it = server_list.begin();
  for (; sl_it != server_list.end(); sl_it++) {
    socket_list_type::iterator it = find_same_socket(*sl_it, socket_list);
    if (it != socket_list.end()) {
      if (is_include_same_server_name(*sl_it, it->second)) {
        std::cout << "server_name conflicts." << std::endl;
        continue;
      }
      it->second.push_back(&(*sl_it));
    } else {
      int new_socket = open_new_socket(*sl_it);
      socket_list.insert(
          std::make_pair(new_socket, std::vector<const ServerConfig *>()));
      socket_list[new_socket].push_back(&(*sl_it));
    }
  }
  return socket_list;
}
