#include "config/config.hpp"
#include "event/event.hpp"
#include "http/http.hpp"
#include "utils/utils.hpp"
#include <cstdlib>
#include <poll.h>
#include <sys/socket.h>

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

static void create_pollfds(pollfds_type               &pollfds,
                           const socket_list_type     &socket_list,
                           const connection_list_type &connection_list) {
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

// TODO: exitすべきか調査
static int xaccept(int listen_fd) {
  int connection_fd = accept(listen_fd, (struct sockaddr *)NULL, NULL);
  if (connection_fd == -1) {
    error_log_with_errno("accept()) failed.");
    exit(EXIT_FAILURE);
  }
  std::cout << "listen fd: " << listen_fd << std::endl;
  std::cout << "connection fd: " << connection_fd << std::endl;
  return connection_fd;
}

// TODO: exitすべきか調査
static int xpoll(struct pollfd *fds, nfds_t nfds, int timeout) {
  int nready = poll(fds, nfds, timeout);
  if (nready == -1) {
    error_log_with_errno("poll() failed");
    exit(EXIT_FAILURE);
  }
  return nready;
}

// socket_list:     listen_fdとserver_groupの関係を管理
// connection_list: connection_fdとlisten_fdの関係を管理
void listen_event(const server_group_type &server_group) {
  socket_list_type     socket_list = create_socket_map(server_group);
  connection_list_type connection_list;
  pollfds_type         pollfds;

  while (1) {
    create_pollfds(pollfds, socket_list, connection_list);
    // NOTE: nreadyはpollfdsでreventにフラグが立ってる要素数
    int                   nready = xpoll(&pollfds[0], pollfds.size(), 0);
    pollfds_type_iterator it     = pollfds.begin();
    for (; it != pollfds.end() && 0 < nready; it++) {
      if (it->revents) {
        put_events_info(it->fd, it->revents);
        if (it->revents & POLLIN) {
          // TMP: socket_listの要素かどうかでfdを区別
          int listen_flg = socket_list.count(it->fd);
          if (listen_flg) {
            int connection_fd = xaccept(it->fd);
            connection_list.insert(std::make_pair(connection_fd, it->fd));
          } else {
            http(it->fd);
            close(it->fd);
            connection_list.erase(it->fd);
          }
        }
        // TODO: POLLIN以外の処理
        nready--;
      }
    }
  }
  // TMP
  close_all_socket(socket_list);
  close_all_socket(connection_list);
}
