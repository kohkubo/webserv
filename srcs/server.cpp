#include <iostream>
#include <netdb.h>
#include <poll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "ServerConfig.hpp"
#include "Webserv.hpp"
#include "http.hpp"
#include "socket.hpp"
#include "util.hpp"

void server_io_multiplexing(const ServerConfig &server_config) {
  int socket = open_new_socket(server_config);
  while (1) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(socket, &readfds);

    timeval timeout;
    timeout.tv_sec  = 0;
    timeout.tv_usec = 0;

    int ret         = select(socket + 1, &readfds, NULL, NULL, &timeout);
    if (ret == -1) {
      error_log_with_errno("select() failed. readfds.");
      continue;
    }
    if (ret && FD_ISSET(socket, &readfds)) {
      int accfd = accept(socket, (struct sockaddr *)NULL, NULL);
      if (accfd == -1) {
        continue;
      }
      usleep(1000);
      http(accfd);
    }
  }
  return;
}
