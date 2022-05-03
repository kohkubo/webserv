#include "event/listen_event.hpp"
#include "http/http.hpp"
#include "socket.hpp"
#include "util/util.hpp"
#include "Connection.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

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
static fd_set create_readfds(const socket_list_type &socket_list, const connection_list_type &connection_list, int &nfds) {
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

static void close_all_socket(const socket_list_type &socket_list) {
  socket_list_type::const_iterator it = socket_list.begin();
  for (; it != socket_list.end(); it++) {
    close(it->first);
  }
}

#include <vector>

void listen_event(const server_group_type &server_group) {
  socket_list_type socket_list = create_socket_map(server_group);
  connection_list_type connection_list;

  timeval          timeout     = {.tv_sec = 0, .tv_usec = 0};
  while (1) {
    int    nfds;
    fd_set readfds = create_readfds(socket_list, connection_list, nfds);

    int    ret     = select(nfds + 1, &readfds, NULL, NULL, &timeout);
    if (ret == -1) {
      error_log_with_errno("select() failed. readfds.");
      exit(EXIT_FAILURE);
    }
    // TODO: retの数処理を行ったら打ち切り
    if (ret) {
      socket_list_type::iterator it = socket_list.begin();
      for (; it != socket_list.end(); it++) {
        int listen_fd = it->first;
        if (FD_ISSET(listen_fd, &readfds)) {
          std::cout << "FD_ISSET listen_fd:" << listen_fd << std::endl;
          int accfd = accept(listen_fd, (struct sockaddr *)NULL, NULL);
          std::cout << "accepted fd:" << accfd << std::endl;
          if (accfd == -1) {
            error_log_with_errno("accept()) failed.");
            exit(EXIT_FAILURE);
          }
          connection_list.insert(std::make_pair(accfd, Connection(listen_fd)));
        }
      }
      // TODO: connection内にstringを受け取る
      connection_list_type::iterator cit = connection_list.begin();
      typedef std::vector<connection_list_type::iterator> erase_vector_type;
      erase_vector_type to_erase;
      for (; cit != connection_list.end() ; cit++) {
        int accfd = cit->first;
        if (FD_ISSET(accfd, &readfds)) {
          std::cout << "FD_ISSET accfd:" << accfd << std::endl;
          http(accfd);
          to_erase.push_back(cit);
        }
      }
      erase_vector_type::iterator eit = to_erase.begin();
      for (; eit != to_erase.end(); eit++) {
        int accfd = (*eit)->first;
        FD_CLR(accfd, &readfds);
        close(accfd); // tmp
        connection_list.erase(*eit);
      }
    }
  }
  // tmp
  close_all_socket(socket_list);
}
