#include <iostream>
#include <netdb.h>
#include <poll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "Socket.hpp"
#include "Webserv.hpp"
#include "config/ServerConfig.hpp"
#include "util.hpp"

#define HTML_FILE  "index.html"
#define HTTP1_PORT 5001
#define BUF_SIZE   1024

void read_request(int accfd) {
  char        buf[BUF_SIZE] = {};
  std::string recv_str      = "";
  ssize_t     read_size     = 0;
  do {
    read_size = recv(accfd, buf, sizeof(buf) - 1, 0);
    if (read_size == -1) {
      error_log_with_errno("read() failed.");
      close(accfd);
      // __accfd = -1;
      break;
    }
    if (read_size > 0) {
      recv_str.append(buf);
    }
    if (is_match_suffix_string(recv_str, "\r\n\r\n")) {
      break;
    }
  } while (read_size > 0);
  std::cout << recv_str << std::endl;
}

void server() {
  const std::string executive_file = HTML_FILE;
  ServerConfig      server_config;
  Socket           *socket = new Socket(server_config);
  while (1) {
    int accfd = accept(socket->get_listenfd(), (struct sockaddr *)NULL, NULL);
    if (accfd == -1) {
      continue;
    }
    read_request(accfd);
  }
  close(socket->get_listenfd());
  return;
}
