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
  char        __buf[BUF_SIZE] = {};
  std::string __recv_str      = "";
  ssize_t     __read_size     = 0;
  do {
    __read_size = recv(accfd, __buf, sizeof(__buf) - 1, 0);
    if (__read_size == -1) {
      error_log_with_errno("read() failed.");
      close(accfd);
      // __accfd = -1;
      break;
    }
    if (__read_size > 0) {
      __recv_str.append(__buf);
    }
    if (is_match_suffix_string(__recv_str, "\r\n\r\n")) {
      break;
    }
  } while (__read_size > 0);
  std::cout << __recv_str << std::endl;
}

void server() {
  const std::string __executive_file = HTML_FILE;
  ServerConfig      __server_config;
  Socket           *__socket = new Socket(__server_config);
  while (1) {
    int __accfd =
        accept(__socket->get_listenfd(), (struct sockaddr *)NULL, NULL);
    if (__accfd == -1) {
      continue;
    }
    read_request(__accfd);
  }
  close(__socket->get_listenfd());
  return;
}

void server_io_multiplexing() {
  const std::string __executive_file = HTML_FILE;
  ServerConfig      __server_config;
  Socket           *__socket = new Socket(__server_config);
  while (1) {
    fd_set __readfds;
    FD_ZERO(&__readfds);
    FD_SET(__socket->get_listenfd(), &__readfds);

    timeval __timeout;
    // ここをゼロにすると動かない
    __timeout.tv_sec  = 0;
    __timeout.tv_usec = 0;

    int __ret = select(__socket->get_listenfd() + 1, &__readfds, NULL, NULL,
                       &__timeout);
    if (__ret == -1) {
      error_log_with_errno("select() failed. readfds.");
      continue;
    }
    if (__ret && FD_ISSET(__socket->get_listenfd(), &__readfds)) {
      int __accfd =
          accept(__socket->get_listenfd(), (struct sockaddr *)NULL, NULL);
      if (__accfd == -1) {
        continue;
      }
      usleep(1000);
      read_request(__accfd);
    }
  }
  close(__socket->get_listenfd());
  return;
}