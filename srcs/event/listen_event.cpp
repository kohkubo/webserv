#include "config/config.hpp"
#include "event/event.hpp"
#include "http/http.hpp"
#include "utils/utils.hpp"
#include <cstdlib>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

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

static void create_pollfds(pollfds_type               &pollfds,
                           const socket_list_type     &socket_list,
                           const connection_list_type &connection_list) {
  // vectorをクリア（以前に確保された領域はfreeされない。->sizeは0,だけどcapacityは0じゃない
  pollfds.clear();
  set_pollfds(pollfds, socket_list);
  set_pollfds(pollfds, connection_list);
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
static void do_connection(int                   listen_fd,
                          connection_list_type &connection_list) {
  int connection_fd = accept(listen_fd, (struct sockaddr *)NULL, NULL);
  if (connection_fd == -1) {
    error_log_with_errno("accept()) failed.");
    exit(EXIT_FAILURE);
  }
  std::cout << "listen fd: " << listen_fd << std::endl;
  std::cout << "connection fd: " << connection_fd << std::endl;

  connection_list.insert(std::make_pair(connection_fd, listen_fd));
  // create_pollfdsで追加するのでここではpollfdsに追加しなくてok
  // struct pollfd new_pfd;
  // new_pfd.fd     = connection_fd;
  // new_pfd.events = POLLIN;
  // pollfds.push_back(new_pfd);
}

// tmp
// connection_fdをhttpに渡す, 各データ構造から不要データを削除
static void do_http(pollfds_type_iterator it,
                    connection_list_type &connection_list) {
  std::cout << "read from fd: " << it->fd << std::endl;
  http(it->fd);
  close(it->fd); // tmp

  connection_list.erase(it->fd);
  // do_connection関数と同様に、create_pollfdsでconnection_listをもとに
  // pollfdsにfdをセットするのでここでは削除しなくてok
  // return pollfds.erase(it);
}

void listen_event(const server_group_type &server_group) {
  socket_list_type socket_list =
      create_socket_map(server_group); // listen_fdとserver_groupの関係を管理
  connection_list_type connection_list; // connection_fdとliste_fdの関係を管理
  // listen_eventのスコープで宣言したvectorを使いまわす。（もしかしたら最適化があっていらない作業かも
  pollfds_type         pollfds;

  while (1) {
    // pollのたびにチェックする値をセット
    create_pollfds(pollfds, socket_list, connection_list);
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
            do_connection(it->fd, connection_list);
          } else {
            do_http(it, connection_list);
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
