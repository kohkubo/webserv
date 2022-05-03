#include "ServerConfig.hpp"
#include "event/socket.hpp"
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

static bool is_same_socket(const ServerConfig &serv_x,
                           const ServerConfig &serv_y) {
  bool                res = false;
  struct sockaddr_in *x =
      reinterpret_cast<sockaddr_in *>(serv_x.addrinfo_->ai_addr);
  struct sockaddr_in *y =
      reinterpret_cast<sockaddr_in *>(serv_y.addrinfo_->ai_addr);
  res = (x->sin_addr.s_addr == y->sin_addr.s_addr) &&
        (x->sin_port == y->sin_port);
  return res;
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

// tmpにlistenディレクティブが共通するServerConfigのリストを作る
server_group_type create_server_group(const server_list_type &server_list) {
  server_group_type                         server_group;
  std::vector<ServerConfig>::const_iterator sl_it = server_list.begin();
  for (; sl_it != server_list.end(); sl_it++) {
    server_group_type::iterator it = find_same_socket(*sl_it, server_group);
    if (it != server_group.end()) {
      // TODO: server_nameが同じ場合追加しない。
      (*it).push_back(&(*sl_it));
    } else {
      server_group.push_back(std::vector<const ServerConfig *>());
      server_group.back().push_back(&(*sl_it));
    }
  }
  return server_group;
}