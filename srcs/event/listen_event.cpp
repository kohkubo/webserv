#include "config/config.hpp"
#include "event/Connection.hpp"
#include "event/connection_handler.hpp"
#include "event/event.hpp"
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

static void set_connection_fd(struct pollfd *pfds, int idx,
                              const connection_list_type &connection_list) {
  connection_list_type::const_iterator it = connection_list.begin();
  for (; it != connection_list.end(); it++) {
    pfds[idx].fd = it->first;
    if (it->second.is_sending()) {
      pfds[idx].events = POLLIN | POLLOUT;
    } else {
      pfds[idx].events = POLLIN;
    }
    idx++;
  }
}

// pollに渡すpollfd構造体の配列作成
// TODO: vectorでやる, 最初の要素のアドレス渡せば良い
static struct pollfd *
create_pollfds(const struct pollfd        *old_pfds,
               const socket_list_type     &socket_list,
               const connection_list_type &connection_list, const int &nfds) {
  struct pollfd *new_pfds =
      (struct pollfd *)realloc((void *)old_pfds, sizeof(struct pollfd) * nfds);
  if (new_pfds == NULL) {
    error_log_with_errno("realloc");
    exit(EXIT_FAILURE);
  }
  memset(new_pfds, 0, sizeof(struct pollfd) * nfds);
  set_fd_list(new_pfds, 0, socket_list);
  set_connection_fd(new_pfds, socket_list.size(), connection_list);
  return new_pfds;
}

static void connect_fd(int listen_fd, connection_list_type &connection_list) {
  int accfd = accept(listen_fd, (struct sockaddr *)NULL, NULL);
  if (accfd == -1) {
    error_log_with_errno("accept()) failed.");
    exit(EXIT_FAILURE);
  }
  std::cout << "listen fd: " << listen_fd << " connection fd: " << accfd
            << std::endl;
  connection_list.insert(std::make_pair(accfd, Connection(listen_fd)));
}

void listen_event(const server_group_type &server_group) {
  socket_list_type socket_list =
      create_socket_map(server_group); // listen_fdとserver_groupの関係を管理
  connection_list_type connection_list;

  struct pollfd       *pfds = NULL;
  while (1) {
    int nfds_listen     = socket_list.size();
    int nfds_connection = connection_list.size();
    int nfds            = nfds_listen + nfds_connection;
    pfds       = create_pollfds(pfds, socket_list, connection_list, nfds);

    int nready = poll(pfds, nfds, 0);
    if (nready == -1) {
      error_log_with_errno("poll() failed");
      exit(EXIT_FAILURE);
    }
    for (int i = 0; i < nfds && 0 < nready; i++) {
      if (pfds[i].revents != 0) {
        std::cout << ((pfds[i].revents & POLLIN) ? "POLLIN " : "")
                  << ((pfds[i].revents & POLLPRI) ? "POLLPRI " : "")
                  << ((pfds[i].revents & POLLOUT) ? "POLLOUT " : "")
                  << ((pfds[i].revents & POLLERR) ? "POLLERR " : "")
                  << ((pfds[i].revents & POLLHUP) ? "POLLHUP " : "")
                  << ((pfds[i].revents & POLLNVAL) ? "POLLNVAL " : "")
                  << "fd: " << pfds[i].fd << std::endl;
        if (pfds[i].revents & POLLIN) {
          // TMP: 処理するfdの種類はindex番号の範囲で判別している
          if (i < nfds_listen) {
            connect_fd(pfds[i].fd, connection_list);
          } else {
            connection_receive_handler(pfds[i].fd, connection_list,
                                       socket_list);
          }
        }
        if (pfds[i].revents & POLLOUT) {
          connection_send_handler(pfds[i].fd, connection_list);
        }
        nready--;
      }
    }
  }
}
