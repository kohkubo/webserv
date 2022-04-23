#include "event.hpp"
#include "http.hpp"

/*
** socketfdをkeyにしたServer_configvectorのmapをsocket_listとして保持
** 実際のリクエストはaccfdから受け取る。->accfdからどのsocketの通信か判別する必要あり。
** std::map connection_list<accfd,socketfd>>（もしくは類似の構造）
** connection_listのvalueとしてsocketfd、受信したstringをもった構造が欲しい
** pollingするときに二つのmapのkeyをreadfdsに加える。
*/

void listen_event(const std::vector<ServerConfig> &server_list) {
  // socket open
  socket_list_type socket_list = create_socket_map(server_list);

  timeval          timeout     = {.tv_sec = 0, .tv_usec = 0};
  while (1) {
    int    nfds;
    fd_set readfds = create_readfds(socket_list, nfds);

    int    ret     = select(nfds + 1, &readfds, NULL, NULL, &timeout);
    if (ret == -1) {
      error_log_with_errno("select() failed. readfds.");
      continue;
    }
    // TODO: retの数処理を行ったら打ち切り
    if (ret) {
      socket_list_type::iterator it = socket_list.begin();
      for (; it != socket_list.end(); it++) {
        if (FD_ISSET(it->first, &readfds)) {
          int accfd = accept(it->first, (struct sockaddr *)NULL, NULL);
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
  // tmp
  close_all_socket(socket_list);
}
