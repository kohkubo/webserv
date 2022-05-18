#include "config/config.hpp"
#include "event/Connection.hpp"
#include "event/connection_handler.hpp"
#include "event/event.hpp"
#include "utils/utils.hpp"
#include <cstdlib>
#include <poll.h>
#include <sys/socket.h>

static void set_listen_fd(pollfds_type                          &pollfds,
                          const std::map<listen_fd, conf_group> &listen_fds) {
  std::map<listen_fd, conf_group>::const_iterator it = listen_fds.begin();
  for (; it != listen_fds.end(); it++) {
    struct pollfd new_pfd;
    new_pfd.fd     = it->first;
    new_pfd.events = POLLIN;
    pollfds.push_back(new_pfd);
  }
}

static void set_connection_fd(pollfds_type               &pollfds,
                              const connection_list_type &connection_list) {
  connection_list_type::const_iterator it = connection_list.begin();
  for (; it != connection_list.end(); it++) {
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

static void create_pollfds(pollfds_type                          &pollfds,
                           const std::map<listen_fd, conf_group> &listen_fds,
                           const connection_list_type &connection_list) {
  pollfds.clear();
  set_listen_fd(pollfds, listen_fds);
  set_connection_fd(pollfds, connection_list);
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

// pollfds - listen_fds->first , connection_list->first

// listen_fds:     listen_fdとserver_groupの関係を管理
// connection_list: connection_fdとlisten_fdの関係を管理
void listen_event(std::map<listen_fd, conf_group> &listen_fds) {
  connection_list_type connection_list;
  pollfds_type         pollfds;

  while (1) {
    create_pollfds(pollfds, listen_fds, connection_list);
    // NOTE: nreadyはpollfdsでreventにフラグが立ってる要素数
    int                    nready = xpoll(&pollfds[0], pollfds.size(), 0);
    pollfds_type::iterator it     = pollfds.begin();
    for (; it != pollfds.end() && 0 < nready; it++) {
      if (it->revents) {
        debug_put_events_info(it->fd, it->revents);
        if (it->revents & POLLIN) {
          // TMP: socket_listの要素かどうかでfdを区別
          int listen_flg = listen_fds.count(it->fd);
          if (listen_flg) {
            int connection_fd = xaccept(it->fd);
            connection_list.insert(
                std::make_pair(connection_fd, Connection(&listen_fds[it->fd])));
          } else {
            connection_receive_handler(it->fd, connection_list);
          }
        }
        if (it->revents & POLLOUT) {
          connection_send_handler(it->fd, connection_list);
        }
        // TODO: 他reventsに対する処理
        nready--;
      }
    }
  }
}
