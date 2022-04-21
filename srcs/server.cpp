#include <iostream>
#include <netdb.h>
#include <poll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "Socket.hpp"
#include "Webserv.hpp"
#include "config/ServerConfig.hpp"
#include "http.hpp"
#include "util.hpp"

void server_io_multiplexing(const ServerConfig &server_config) {
  Socket *socket = new Socket(server_config);
  while (1) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(socket->get_listenfd(), &readfds);

    timeval timeout;
    timeout.tv_sec  = 0;
    timeout.tv_usec = 0;

    int ret =
        select(socket->get_listenfd() + 1, &readfds, NULL, NULL, &timeout);
    if (ret == -1) {
      error_log_with_errno("select() failed. readfds.");
      continue;
    }
    if (ret && FD_ISSET(socket->get_listenfd(), &readfds)) {
      int accfd = accept(socket->get_listenfd(), (struct sockaddr *)NULL, NULL);
      if (accfd == -1) {
        continue;
      }
      usleep(1000);
      http(accfd);
    }
  }
  return;
}
