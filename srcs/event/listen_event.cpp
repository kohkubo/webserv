
#include "Socket.hpp"
#include "config/ServerConfig.hpp"
#include "event.hpp"

void listen_event(std::map<int, std::vector<ServerConfig>> server_list) {
  // 必要なもの
  // serverConfigのリスト
  std::deque<int> socket_fd;
  std::deque<int> connection_fd;

  // server_listのportに対してsocketを開く。
  Socket         *socket = new Socket(server_config);
  while (1) {
    // チェックしたいfdのリスト
    // socket_fd + connection_fd（接続済みのsocket）
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(socket->get_listenfd(), &readfds);

    timeval timeout = {.tv_sec = 0, .tv_usec = 0};

    int     ret =
        select(socket->get_listenfd() + 1, &readfds, NULL, NULL, &timeout);
    if (ret == -1) {
      error_log_with_errno("select() failed. readfds.");
      continue;
    }
    int i = 0;
    while (i < ret) {
      // socket_fd -> acceptしてconnection_fdに加える。
      if (FD_ISSET(socket_fd[0], &readfds)) {
        handle_connection();
      }
      // connection_fd -> リクエストのデータがある。
      if (FD_ISSET(connection_fd[0], &readfds)) {
        handle_request();
      }
    }

    if (ret && FD_ISSET(socket->get_listenfd(), &readfds)) {
      int accfd = accept(socket->get_listenfd(), (struct sockaddr *)NULL, NULL);
      if (accfd == -1) {
        continue;
      }
    }
  }
  return;
}
