#include "config/config.hpp"
#include "event/Connection.hpp"
#include "event/ConnectionMap.hpp"
#include "event/event.hpp"
#include "utils/syscall_wrapper.hpp"
#include "utils/utils.hpp"

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

static void set_listen_fd(pollfds_type           &pollfds,
                          const socket_list_type &socket_list) {
  socket_list_type::const_iterator it = socket_list.begin();
  for (; it != socket_list.end(); it++) {
    struct pollfd new_pfd;
    new_pfd.fd     = it->first;
    new_pfd.events = POLLIN;
    pollfds.push_back(new_pfd);
  }
}

static void set_connection_fd(pollfds_type        &pollfds,
                              const ConnectionMap &connection_list) {
  ConnectionMap::const_iterator it = connection_list.begin();
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

static void create_pollfds(pollfds_type           &pollfds,
                           const socket_list_type &socket_list,
                           const ConnectionMap    &connection_list) {
  pollfds.clear();
  set_listen_fd(pollfds, socket_list);
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

// socket_list:     listen_fdとserver_groupの関係を管理
// connection_list: connection_fdとlisten_fdの関係を管理
void listen_event(const server_group_type &server_group) {
  socket_list_type socket_list = create_socket_map(server_group);
  ConnectionMap    connection_list;
  pollfds_type     pollfds;

  while (1) {
    create_pollfds(pollfds, socket_list, connection_list);
    // NOTE: nreadyはpollfdsでreventにフラグが立ってる要素数
    int                    nready = xpoll(&pollfds[0], pollfds.size(), 0);
    pollfds_type::iterator it     = pollfds.begin();
    for (; it != pollfds.end() && 0 < nready; it++) {
      if (!it->revents) {
        continue;
      }
      debug_put_events_info(it->fd, it->revents);
      if (it->revents & POLLIN) {
        // TMP: socket_listの要素かどうかでfdを区別
        int listen_flg = socket_list.count(it->fd);
        if (listen_flg) {
          connection_list.add_new_connection(it->fd, socket_list);
        } else {
          connection_list.receive(it->fd);
        }
      }
      if (it->revents & POLLOUT) {
        connection_list.send(it->fd);
      }
      // TODO: 他reventsに対する処理
      nready--;
    }
  }
}
