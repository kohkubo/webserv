#include "ServerConfig.hpp"
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

static server_group_type::iterator
find_same_socket(const ServerConfig &conf, server_group_type &server_group) {
  server_group_type::iterator it = server_group.begin();
  for (; it != server_group.end(); it++) {
    // 先頭の要素と一致すれば良い
    if (is_same_socket(conf, *((*it)[0])))
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

// tmpにlistenディレクティブが共通するServerConfigのリストを作る
server_group_type create_server_group(const server_list_type &server_list) {
  server_group_type                server_group;
  server_list_type::const_iterator sl_it = server_list.begin();
  for (; sl_it != server_list.end(); sl_it++) {
    server_group_type::iterator it = find_same_socket(*sl_it, server_group);
    if (it != server_group.end()) {
      if (is_include_same_server_name(*sl_it, *it)) {
        std::cout << "server_name conflicts." << std::endl;
        continue;
      }
      (*it).push_back(&(*sl_it));
    } else {
      server_group.push_back(std::vector<const ServerConfig *>());
      server_group.back().push_back(&(*sl_it));
    }
  }
  return server_group;
}