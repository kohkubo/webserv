#include "event.hpp"

#include <map>

/*
** ServerConfigのリストからソケットを作成、ソケットのfdをkeyにしたSocketクラスのmapを作る。
** n^2になりそう。
*/
std::map<socket_fd, std::vector<ServerConfig> >
create_socket_map(const std::vector<ServerConfig> &server_list) {
  std::map<socket_fd, std::vector<ServerConfig> > res;

  // tmpにlistenディレクティブが共通するServerConfigのリストを作る。
  // TODO: addressを名前解決した状態で比較する必要があるか確認。
  std::vector<std::vector<ServerConfig> >         tmp;
  std::vector<ServerConfig>::const_iterator       sl_it = server_list.begin();
  for (; sl_it != server_list.end(); sl_it++) {
  }

  // tmpからsocketを開いて、fdをkeyにしたmapにする。
  std::vector<std::vector<ServerConfig> >::iterator it = tmp.begin();
  for (; it != tmp.end(); it++) {
    // ここでSocketがスコープ抜けるとlitenfdがcloseされそう
    Socket sock((*it)[0]);
    res.insert(std::make_pair(sock.get_listenfd(), *it));
  }

  return res;
}