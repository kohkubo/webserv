#include "Socket.hpp"

#include <map>

/*
** ServerConfigのリストからソケットを作成、ソケットのfdをkeyにしたSocketクラスのmapを作る。
** n^2になりそう。
*/
std::map<int, Socket>
create_socket_map(const std::vector<ServerConfig> &server_list) {
  std::map<int, Socket> res;

  return res;
}