#include "event.hpp"
#include "http.hpp"
#include "socket.hpp"
#include "util.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/*
** socketfdをkeyにしたServer_configvectorのmapをsocket_listとして保持
** 実際のリクエストはaccfdから受け取る。->accfdからどのsocketの通信か判別する必要あり。
** std::map connection_list<accfd,socketfd>>（もしくは類似の構造）
** connection_listのvalueとしてsocketfd、受信したstringをもった構造が欲しい
** pollingするときに二つのmapのkeyをreadfdsに加える。
*/

static socket_list_type
create_socket_map(const server_group_type &server_group) {
  socket_list_type                  res;

  server_group_type::const_iterator it = server_group.begin();
  for (; it != server_group.end(); it++) {
    int new_socket = open_new_socket((*it)[0]);
    res.insert(std::make_pair(new_socket, *it));
  }
  return res;
}

// socket_fd + connection_fdをreadfdsに加える。
static fd_set create_readfds(const socket_list_type &socket_list, int &nfds) {
  int    res;
  fd_set readfds;

  nfds = -1;
  FD_ZERO(&readfds);
  res  = set_fd_list(&readfds, socket_list);
  nfds = std::max(nfds, res);
  return readfds;
}

static void close_all_socket(const socket_list_type &socket_list) {
  socket_list_type::const_iterator it = socket_list.begin();
  for (; it != socket_list.end(); it++) {
    close(it->first);
  }
}

void listen_event(const server_group_type &server_group) {
  socket_list_type socket_list = create_socket_map(server_group);

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
          process_http(accfd);
        }
      }
    }
  }
  // tmp
  close_all_socket(socket_list);
}
