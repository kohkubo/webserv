#include "event.hpp"

#include <map>

/*
  list(serverconfigのvector)の中にconfと同様のlistenを持つserverconfigがあるか確認する。
  あったら、そのリストのiteratorを返し、なかったらendを返す。
*/

static std::vector<std::vector<ServerConfig> >::iterator
find_same_socket(const ServerConfig                      &conf,
                 std::vector<std::vector<ServerConfig> > &server_group) {
  std::vector<std::vector<ServerConfig> >::iterator it = server_group.begin();
  for (; it != server_group.end(); it++) {
    // 先頭の要素と一致すれば良い
    if (is_same_socket(conf, (*it)[0]))
      break;
  }
  return it;
}

/*
** ServerConfigのリストからソケットを作成、ソケットのfdをkeyにしたSocketクラスのmapを作る。
*/

socket_list_type
create_socket_map(const std::vector<ServerConfig> &server_list) {
  socket_list_type                          res;

  // tmpにlistenディレクティブが共通するServerConfigのリストを作る
  std::vector<std::vector<ServerConfig> >   server_group;
  std::vector<ServerConfig>::const_iterator sl_it = server_list.begin();
  for (; sl_it != server_list.end(); sl_it++) {
    std::vector<std::vector<ServerConfig> >::iterator it =
        find_same_socket(*sl_it, server_group);
    if (it != server_group.end()) {
      // TODO: server_nameが同じ場合追加しない。
      (*it).push_back(*sl_it);
    } else {
      server_group.push_back(std::vector<ServerConfig>());
      server_group.back().push_back(*sl_it);
    }
  }

  // server_groupからsocketを開いて、fdをkeyにしたmapにする。
  std::vector<std::vector<ServerConfig> >::iterator it = server_group.begin();
  for (; it != server_group.end(); it++) {
    // ここでSocketがスコープ抜けるとlitenfdがcloseされる
    // Socketをコピーしてもコピー元がcloseする
    // => 別関数で最後にclose or
    // resにSocket格納（関数の返り値のコピーでcloseされるかも）
    Socket sock((*it)[0]);
    res.insert(std::make_pair(sock.get_listenfd(), *it));
  }
  return res;
}

void close_all_socket(const socket_list_type &socket_list) {
  socket_list_type::const_iterator it = socket_list.begin();
  for (; it != socket_list.end(); it++) {
    close(it->first);
  }
}
