#include "config/config.hpp"
#include "event/Connection.hpp"
#include "event/connection_handler.hpp"
#include "event/event.hpp"
#include "utils/utils.hpp"
#include <cstdlib>
#include <poll.h>
#include <sys/socket.h>

static void
add_listenfd_to_pollfds(std::vector<struct pollfd>            &pollfds,
                        const std::map<listen_fd, conf_group> &listen_fd_map) {
  std::map<listen_fd, conf_group>::const_iterator it = listen_fd_map.begin();
  for (; it != listen_fd_map.end(); it++) {
    struct pollfd new_pfd;
    new_pfd.fd     = it->first;
    new_pfd.events = POLLIN;
    pollfds.push_back(new_pfd);
  }
}

static void
add_connfd_to_pollfds(std::vector<struct pollfd>          &pollfds,
                      const std::map<conn_fd, Connection> &conn_fd_map) {
  std::map<conn_fd, Connection>::const_iterator it = conn_fd_map.begin();
  for (; it != conn_fd_map.end(); it++) {
    struct pollfd new_pfd;
    new_pfd.fd = it->first;
    if (it->second.is_sending()) {
      new_pfd.events = POLLIN | POLLOUT;
    } else {
      new_pfd.events = POLLIN;
    }
    pollfds.push_back(new_pfd);
  }
}

static void reset_pollfds(std::vector<struct pollfd>            &pollfds,
                          const std::map<listen_fd, conf_group> &listen_fd_map,
                          const std::map<conn_fd, Connection>   &conn_fd_map) {
  pollfds.clear();
  add_listenfd_to_pollfds(pollfds, listen_fd_map);
  add_connfd_to_pollfds(pollfds, conn_fd_map);
}

static void debug_put_events_info(int fd, short revents) {
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

// pollfds - listen_fd_map->first , conn_fd_map->first

// listen_fd_map:     listen_fdとserver_groupの関係を管理
// conn_fd_map: connection_fdとlisten_fdの関係を管理
void listen_event(std::map<listen_fd, conf_group> &listen_fd_map) {
  std::map<conn_fd, Connection> conn_fd_map;
  std::vector<struct pollfd>    pollfds;

  while (1) {
    reset_pollfds(pollfds, listen_fd_map, conn_fd_map);
    // NOTE: nreadyはpollfdsでreventにフラグが立ってる要素数
    int nready = xpoll(&pollfds[0], pollfds.size(), 0);
    std::vector<struct pollfd>::iterator it = pollfds.begin();
    for (; it != pollfds.end() && 0 < nready; it++) {
      if (!it->revents) {
        continue;
      }
      debug_put_events_info(it->fd, it->revents);
      int listen_flg = listen_fd_map.count(it->fd);
      if (listen_flg) {
        int connection_fd = xaccept(it->fd);
        conn_fd_map.insert(
            std::make_pair(connection_fd, Connection(&listen_fd_map[it->fd])));
        continue;
      }
      // conn_fd
      if (it->revents & POLLIN) {
        connection_receive_handler(it->fd, conn_fd_map);
      } else if (it->revents & POLLOUT) {
        connection_send_handler(it->fd, conn_fd_map);
      }
      // TODO: 他reventsに対する処理
      nready--;
    }
  }
}
