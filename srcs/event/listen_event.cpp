#include "config/config.hpp"
#include "event/event.hpp"
#include "http/http.hpp"
#include "utils/utils.hpp"
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>

// TODO: selectがなくなったことによるinclude変更あり？

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
    int new_socket = open_new_socket(*((*it)[0]));
    res.insert(std::make_pair(new_socket, *it));
  }
  return res;
}

// socket_fd + connection_fdをreadfdsに加える。
static fd_set create_readfds(const socket_list_type     &socket_list,
                             const connection_list_type &connection_list,
                             int                        &nfds) {
  int    res;
  fd_set readfds;

  nfds = -1;
  FD_ZERO(&readfds);
  res  = set_fd_list(&readfds, socket_list);
  nfds = std::max(nfds, res);
  res  = set_fd_list(&readfds, connection_list);
  nfds = std::max(nfds, res);
  return readfds;
}

// socket_fd + connection_fdをreadfdsに加える。
static struct pollfd create_pollfds(const struct pollfd *old_pfds,
                             const socket_list_type     &socket_list,
                             const connection_list_type &connection_list,
                             const int                        &nfds) {
  struct pollfd *new_pfds = realloc(old_pfds, sizeof(struct pollfd) * nfds);
  memset(new_pfds, 0, sizeof(new_pfds));
  set_fd_list(new_pfds, 0, socket_list);
  set_fd_list(new_pfds, socket_list.size(), connection_list);
  return new_pfds;
}

static void close_all_socket(const socket_list_type &socket_list) {
  socket_list_type::const_iterator it = socket_list.begin();
  for (; it != socket_list.end(); it++) {
    close(it->first);
  }
}

void listen_event(const server_group_type &server_group) {
  socket_list_type     socket_list = create_socket_map(server_group);
  connection_list_type connection_list; // accfdとsocketとの対応関係持つ

  timeval              timeout = {.tv_sec = 0, .tv_usec = 0};
  struct pollfd *pfds;
  while (1) {
    int    nfds = socket_list.size() + connection_list.size();// overflow可能性確認
    pfds = create_pollfds(pfds, socket_list, connection_list, nfds);

    int    ret     = select(nfds + 1, &readfds, NULL, NULL, &timeout);
    if (ret == -1) {
      error_log_with_errno("select() failed. readfds.");
      exit(EXIT_FAILURE);
    }
    // TODO: retの数処理を行ったら打ち切り
    if (ret) {
      /* 接続要求のあったsocket探す -> accept -> connection_listに追加 */
      socket_list_type::iterator it = socket_list.begin();
      for (; it != socket_list.end(); it++) {
        int listen_fd = it->first;
        if (FD_ISSET(listen_fd, &readfds)) {
          int accfd = accept(listen_fd, (struct sockaddr *)NULL, NULL);
          if (accfd == -1) {
            error_log_with_errno("accept()) failed.");
            exit(EXIT_FAILURE);
          }
          connection_list.insert(std::make_pair(accfd, listen_fd));
        }
      }
      /* 読み込み可能なaccfd探す -> http -> connection_listから削除 */
      // TODO: httpでのread処理もこのloopで行う
      connection_list_iterator cit = connection_list.begin();
      while (cit != connection_list.end()) {
        int accfd = cit->first;
        if (FD_ISSET(accfd, &readfds)) {
          http(accfd);
          close(accfd); // tmp
          connection_list.erase(cit++);
        } else {
          cit++;
        }
      }
    }
  }
  // tmp
  close_all_socket(socket_list);
  // TODO: close_all_socket(connection_list);
}
