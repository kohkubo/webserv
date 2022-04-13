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

#define HTML_FILE  "index.html"
#define HTTP1_PORT 5001
#define BUF_SIZE   1024

std::string read_request(int accfd) {
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
  std::cout << "read_request()" << std::endl;
  std::cout << __recv_str << std::endl;
  return __recv_str;
}

void send_response(int accfd, const std::string &message) {
  send(accfd, message.c_str(), message.size(), 0);
  std::cout << "send_response()" << std::endl;
  std::cout << message << std::endl;
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
    std::string request_message = read_request(__accfd);
    Lexer       message_lexer(request_message, SP);
    Response    response(message_lexer);
    send_response(__accfd, response.message());
  }
  close(__socket->get_listenfd());
  return;
}
