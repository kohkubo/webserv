#include "ServerConfig.hpp"
#include "event.hpp"

#include <set>

/*
socket -> listenディレクティブで決まる（と仮定する。）　ip+portでいい？
一意なip+portの識別子（socketfdだと楽な気がする）の集合をkeyにしたServer_configのmapがほしい。

実際のリクエストはaccfdから受け取る。->accfdからどのsocketの通信か判別する必要あり。
  std::map connection_list<accfd,socketfd>>（もしくは類似の構造）
  connection_listのvalueとしてsocketfd、受信したstringをもった構造が欲しい


pollingするときに二つのmapのkeyをreadfdsに加える。
*/

void listen_event(const std::vector<ServerConfig> &server_list) {
  std::map<int, Socket>     socket_list = create_socket_map(server_list);
  std::map<int, Connection> connection_list;

  timeval                   timeout = {.tv_sec = 0, .tv_usec = 0};
  while (1) {
    // TODO: pair以外のデータ構造に変える。
    std::pair<fd_set, int> res = create_readfds(socket_list, connection_list);
    fd_set                 readfds = res.first;
    int                    ret, nfds = res.second;

    ret = select(nfds, &readfds, NULL, NULL, &timeout);
    if (ret == -1) {
      error_log_with_errno("select() failed. readfds.");
      continue;
    }
    // TODO: retの数処理を行ったら打ち切り
    if (ret) {
      std::map<int, Socket>::iterator sit = socket_list.begin();
      for (; sit != socket_list.end(); sit++) {
        if (FD_ISSET(sit->first, &readfds)) {
          add_connection_list(sit->first, connection_list);
        }
      }
      std::map<int, Connection>::iterator cit = connection_list.begin();
      for (; cit != connection_list.end(); cit++) {
        if (FD_ISSET(cit->first, &readfds)) {
          handle_request(cit->first, socket_list, connection_list);
        }
      }
    }
  }
  return;
}
