#include "event.hpp"

#include <map>

/*
** ServerConfigのリストからソケットを作成、ソケットのfdをkeyにしたSocketクラスのmapを作る。
** n^2になりそう。
*/
std::map<socket_fd, std::vector<Socket>>
create_socket_map(const std::vector<ServerConfig> &server_list) {
  std::map<socket_fd, std::vector<Socket>>  res;

  // tmpにlistenディレクティブが共通するServerConfigのリストを作る。
  std::vector<std::vector<ServerConfig>>    tmp;
  std::vector<ServerConfig>::const_iterator it = server_list.begin();
  for (; it != server_list.end(); it++) {
  }

  // tmpからsocketを開いて、fdをkeyにしたmapにする。

  return res;
}