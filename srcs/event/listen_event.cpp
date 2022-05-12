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

static void print_fd_revents(int fd, short revents) {
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

static int accept_wrapper(int listen_fd) {
  int accfd = accept(listen_fd, (struct sockaddr *)NULL, NULL);
  if (accfd == -1) {
    error_log_with_errno("accept()) failed.");
    exit(EXIT_FAILURE);
  }
  std::cout << "listen fd: " << listen_fd << std::endl;
  std::cout << "connection fd: " << accfd << std::endl;
  return accfd;
}

static void http_wrapper(int connection_fd) {
  std::cout << "read from fd: " << connection_fd << std::endl;
  http(connection_fd);
  close(connection_fd); // tmp
}

void listen_event(const server_group_type &server_group) {
  socket_list_type socket_list =
      create_socket_map(server_group); // listen_fdとserver_groupの関係を管理
  connection_list_type connection_list; // connection_fdとliste_fdの関係を管理

  pollfds_type         pollfds;
  set_fd_list(pollfds, socket_list);
  set_fd_list(pollfds, connection_list);
  // int cnt = 0;
  while (1) {
    // for (pollfds_type_iterator it = pollfds.begin(); it != pollfds.end();
    //      it++) {
    //   std::cout << "~~~>fd: " << it->fd << std::endl;
    //   std::cout << "~~~>events: " << it->events << std::endl;
    //   std::cout << "~~~>reventsf: " << it->revents << std::endl;
    // }
    // if (cnt == 2) {
    //   exit(0);
    // }
    int nready = poll(&pollfds[0], pollfds.size(), 0);
    if (nready == -1) {
      error_log_with_errno("poll() failed");
      exit(EXIT_FAILURE);
    }
    pollfds_type_iterator it = pollfds.begin();
    for (; it != pollfds.end() && 0 < nready;) {
      if (it->revents != 0) {
        print_fd_revents(it->fd, it->revents);
        if (it->revents & POLLIN) {
          int is_listen_fd = socket_list.count(it->fd);
          if (is_listen_fd) {
            int           connection_fd = accept_wrapper(it->fd);
            struct pollfd p;
            p.fd      = connection_fd;
            p.events  = POLLIN;
            p.revents = 0;
            pollfds.push_back(p);
            connection_list.insert(std::make_pair(connection_fd, it->fd));
          } else {
            http_wrapper(it->fd);
            connection_list.erase(it->fd);
            it = pollfds.erase(it);
            continue; // it++を避けるため
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
