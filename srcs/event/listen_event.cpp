#include "config/config.hpp"
#include "event/event.hpp"
#include "http/http.hpp"
#include "utils/utils.hpp"
#include <cstdlib>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>

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

// pollに渡すpollfd構造体の配列作成
static struct pollfd *create_pollfds(const struct pollfd *old_pfds,
                             const socket_list_type     &socket_list,
                             const connection_list_type &connection_list,
                             const int                        &nfds) {
  struct pollfd *new_pfds = (struct pollfd *)realloc((void *)old_pfds, sizeof(struct pollfd) * nfds);
  if (new_pfds == NULL) {
    error_log_with_errno("realloc");
    exit(EXIT_FAILURE);
  }
  memset(new_pfds, 0, sizeof(struct pollfd) * nfds);
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
  socket_list_type     socket_list = create_socket_map(server_group); // listen_fdとserver_groupの関係を管理
  connection_list_type connection_list;                               // connection_fdとliste_fdの関係を管理

  struct pollfd *pfds = NULL;
  while (1) {
    int    nfds_listen = socket_list.size();
    int    nfds_connect = connection_list.size();
    int    nfds = nfds_listen + nfds_connect;// TODO: 合計の最大値が幾つになるか確認
    pfds = create_pollfds(pfds, socket_list, connection_list, nfds);

    int    ret     = poll(pfds, nfds, 0);
    if (ret == -1) {
      error_log_with_errno("poll() failed");
      exit(EXIT_FAILURE);
    }
    for (int i = 0; i < nfds && 0 < ret; i++) {
      if (pfds[i].revents & POLLIN) {
        std::cout << "POLLIN fd: " << pfds[i].fd << std::endl;
        if (i < nfds_listen) {
          /* listen_fdがPOLLIN -> accept -> connection_listに追加 */
          int accfd = accept(pfds[i].fd, (struct sockaddr *)NULL, NULL);
          if (accfd == -1) {
            error_log_with_errno("accept()) failed.");
            exit(EXIT_FAILURE);
          }
          std::cout << "listen fd: " << pfds[i].fd << " connection fd: " << accfd << std::endl;
          connection_list.insert(std::make_pair(accfd, pfds[i].fd));
        } else {
          /* connection_fdがPOLLIN -> http -> connection_listから削除 */
          std::cout << "read from fd: " << pfds[i].fd << std::endl;
          http(pfds[i].fd);
          close(pfds[i].fd); // tmp
          connection_list.erase(pfds[i].fd);
        }
        ret--;
      } else if (pfds[i].revents & (POLLERR | POLLIN)) {
        std::cout << ((pfds[i].revents & POLLERR) ? "POLLERR" : "")
                  << ((pfds[i].revents & POLLIN) ? "POLLIN" : "")
                  << " fd: " <<  pfds[i].fd << std::endl;
        exit(EXIT_FAILURE); // tmp
      }
    }
  }
  // tmp
  close_all_socket(socket_list);
  // TODO: close_all_socket(connection_list);
}
