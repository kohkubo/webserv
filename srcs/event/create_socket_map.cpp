#include "event.hpp"

#include <map>

/*
  list(serverconfigのvector)の中にconfと同様のlistenを持つserverconfigがあるか確認する。
  あったら、そのリストのiteratorを返し、なかったらendを返す。
*/

static std::vector<std::vector<ServerConfig> >::iterator
find_same_socket(const ServerConfig                      &conf,
                 std::vector<std::vector<ServerConfig> > &double_arr) {
  std::vector<std::vector<ServerConfig> >::iterator it = double_arr.begin();
  for (; it != double_arr.end(); it++) {
    // 先頭の要素と一致すれば良い
    if (is_same_socket(conf, (*it)[0]))
      break;
  }
  return it;
}

/*
** ServerConfigのリストからソケットを作成、ソケットのfdをkeyにしたSocketクラスのmapを作る。
*/

std::map<socket_fd, std::vector<ServerConfig> >
create_socket_map(const std::vector<ServerConfig> &server_list) {
  std::map<socket_fd, std::vector<ServerConfig> > res;

  // tmpにlistenディレクティブが共通するServerConfigのリストを作る
  std::vector<std::vector<ServerConfig> >         double_arr;
  std::vector<ServerConfig>::const_iterator       sl_it = server_list.begin();
  for (; sl_it != server_list.end(); sl_it++) {
    std::vector<std::vector<ServerConfig> >::iterator it =
        find_same_socket(*sl_it, double_arr);
    if (it != double_arr.end()) {
      // TODO: server_name 確認
      (*it).push_back(*sl_it);
    } else {
      double_arr.push_back(std::vector<ServerConfig>());
      double_arr.back().push_back(*sl_it);
    }
  }

  // double_arrからsocketを開いて、fdをkeyにしたmapにする。
  std::vector<std::vector<ServerConfig> >::iterator it = double_arr.begin();
  for (; it != double_arr.end(); it++) {
    // ここでSocketがスコープ抜けるとlitenfdがcloseされそう
    Socket sock((*it)[0]);
    res.insert(std::make_pair(sock.get_listenfd(), *it));
  }
  return res;
}