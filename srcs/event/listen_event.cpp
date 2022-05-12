#include "config/config.hpp"
#include "event/event.hpp"
#include "http/http.hpp"
#include "utils/utils.hpp"
#include <cstdlib>
#include <poll.h>
#include <sys/socket.h>
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
    int new_socket = open_new_socket(*((*it)[0]));
    res.insert(std::make_pair(new_socket, *it));
  }
  return res;
}

static void close_all_socket(const socket_list_type &socket_list) {
  socket_list_type::const_iterator it = socket_list.begin();
  for (; it != socket_list.end(); it++) {
    close(it->first);
  }
}

static pollfds_type
create_pollfds(const socket_list_type     &socket_list,
               const connection_list_type &connection_list) {
  pollfds_type pollfds;
  set_pollfds(pollfds, socket_list);
  set_pollfds(pollfds, connection_list);
  return pollfds;
}

static void put_events_info(int fd, short revents) {
  // clang-format off
  std::cout << ((revents & POLLIN) ? "POLLIN " : "")
            << ((revents & POLLPRI) ? "POLLPRI " : "")
            << ((revents & POLLOUT) ? "POLLOUT " : "")
            << ((revents & POLLERR) ? "POLLERR " : "")
            << ((revents & POLLHUP) ? "POLLHUP " : "")
            << ((revents & POLLNVAL) ? "POLLNVAL " : "")
            << "fd: " << fd
            << std::endl;
  // clang-format on
}

// tmp
// listen_fdのaccept(), 各データ構造に新規データを追加
static void do_connection(int listen_fd, pollfds_type &pollfds,
                          connection_list_type &connection_list) {
  int connection_fd = accept(listen_fd, (struct sockaddr *)NULL, NULL);
  if (connection_fd == -1) {
    error_log_with_errno("accept()) failed.");
    exit(EXIT_FAILURE);
  }
  std::cout << "listen fd: " << listen_fd << std::endl;
  std::cout << "connection fd: " << connection_fd << std::endl;

  connection_list.insert(std::make_pair(connection_fd, listen_fd));
  struct pollfd new_pfd;
  new_pfd.fd     = connection_fd;
  new_pfd.events = POLLIN;
  pollfds.push_back(new_pfd);
}

// tmp
// connection_fdをhttpに渡す, 各データ構造から不要データを削除
static pollfds_type_iterator do_http(pollfds_type_iterator it,
                                     pollfds_type         &pollfds,
                                     connection_list_type &connection_list) {
  std::cout << "read from fd: " << it->fd << std::endl;
  http(it->fd);
  close(it->fd); // tmp

  connection_list.erase(it->fd);
  return pollfds.erase(it);
}

void listen_event(const server_group_type &server_group) {
  socket_list_type socket_list =
      create_socket_map(server_group); // listen_fdとserver_groupの関係を管理
  connection_list_type connection_list; // connection_fdとliste_fdの関係を管理

  pollfds_type         pollfds = create_pollfds(socket_list, connection_list);
  while (1) {
    int nready = poll(&pollfds[0], pollfds.size(), 0);
    if (nready == -1) {
      error_log_with_errno("poll() failed");
      exit(EXIT_FAILURE);
    }
    pollfds_type_iterator it = pollfds.begin();
    for (; it != pollfds.end() && 0 < nready;) {
      if (it->revents) {
        put_events_info(it->fd, it->revents);
        if (it->revents & POLLIN) {
          int is_listen_fd = socket_list.count(it->fd);
          if (is_listen_fd) {
            do_connection(it->fd, pollfds, connection_list);
          } else {
            it = do_http(it, pollfds, connection_list);
            continue; // 既にitは次のイテレータなので
          }
          nready--;
        }
        // TODO: POLLIN以外の処理
      }
      it++;
    }
  }
  // tmp
  close_all_socket(socket_list);
  // TODO: close_all_socket(connection_list);
}
