#include "ServerConfig.hpp"
#include "event.hpp"
#include "http.hpp"

/*
socket -> listenディレクティブで決まる（と仮定する。）　ip+portでいい？
一意なip+portの識別子（socketfdだと楽な気がする）の集合をkeyにしたServer_configのmapがほしい。

実際のリクエストはaccfdから受け取る。->accfdからどのsocketの通信か判別する必要あり。
  std::map connection_list<accfd,socketfd>>（もしくは類似の構造）
  connection_listのvalueとしてsocketfd、受信したstringをもった構造が欲しい


pollingするときに二つのmapのkeyをreadfdsに加える。
*/

void listen_event(const std::vector<ServerConfig> &server_list) {
  // socket open
  std::map<socket_fd, std::vector<ServerConfig> > socket_list =
      create_socket_map(server_list);

  timeval timeout = {.tv_sec = 0, .tv_usec = 0};
  while (1) {
    // TODO: pair以外のデータ構造に変える。
    std::pair<fd_set, int> res     = create_readfds(socket_list);
    fd_set                 readfds = res.first;
    int                    ret, nfds = res.second;

    ret = select(nfds, &readfds, NULL, NULL, &timeout);
    if (ret == -1) {
      error_log_with_errno("select() failed. readfds.");
      continue;
    }
    // TODO: retの数処理を行ったら打ち切り
    if (ret) {
      std::map<socket_fd, std::vector<ServerConfig> >::iterator sit =
          socket_list.begin();
      for (; sit != socket_list.end(); sit++) {
        if (FD_ISSET(sit->first, &readfds)) {
          int accfd = accept(sit->first, (struct sockaddr *)NULL, NULL);
          if (accfd == -1) {
            continue;
          }
          // FIXME: don't use sleep
          usleep(2000);
          http(accfd);
        }
      }
    }
  }
  return;
}
