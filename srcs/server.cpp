#include <iostream>
#include <netdb.h>
#include <poll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "Response.hpp"
#include "Socket.hpp"
#include "Webserv.hpp"
#include "config/ServerConfig.hpp"
#include "util.hpp"

#define BUF_SIZE 1024

std::string read_request(int accfd) {
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
  std::cout << "read_request()" << std::endl;
  std::cout << recv_str << std::endl;
  return recv_str;
}

void send_response(int accfd, const std::string &message) {
  send(accfd, message.c_str(), message.size(), 0);
  std::cout << "send_response()" << std::endl;
  std::cout << message << std::endl;
}

void server() {
  ServerConfig server_config;
  Socket      *socket = new Socket(server_config);
  while (1) {
    int accfd = accept(socket->get_listenfd(), (struct sockaddr *)NULL, NULL);
    if (accfd == -1) {
      continue;
    }
    std::string request_message = read_request(accfd);
    Response    response(request_message);
    response.process();
    send_response(accfd, response.message());
  }
  return;
}

void server_io_multiplexing() {
  ServerConfig server_config;
  Socket      *socket = new Socket(server_config);
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
      std::string request_message = read_request(accfd);
      Response    response(request_message);
      response.process();
      send_response(accfd, response.message());
    }
  }
  return;
}
