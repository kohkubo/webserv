#include <iostream>
#include <netdb.h>
#include <poll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "Socket.hpp"
#include "Webserv.hpp"
#include "util.hpp"

#define HTML_FILE     "index.html"
#define HTTP1_PORT    5000
#define BUF_SIZE      1024
#define NGINX_TIMEOUT 60000

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

void send_response(int accfd) {
  std::string response_message_hello_world = "\
HTTP/1.1 200 OK\r\n\
Content-Type: text/html\r\n\
Content-Length: 62\r\n\
Connection: close\r\n\
\r\n\
<html>\n\
    <body>\n\
        Hello, world!\n\
    </body>\n\
</html>\n";
  send(accfd, response_message_hello_world.c_str(),
       response_message_hello_world.size(), 0);
  std::cout << "send_response()" << std::endl;
  std::cout << response_message_hello_world << std::endl;
}

void server() {
  const std::string __executive_file = HTML_FILE;
  Socket           *__socket         = new Socket(HTTP1_PORT);
  while (1) {
    int __accfd =
        accept(__socket->get_listenfd(), (struct sockaddr *)NULL, NULL);
    if (__accfd == -1) {
      error_log_with_errno("accept() failed.");
      continue;
    }
    read_request(__accfd);
  }
  close(__socket->get_listenfd());
  return;
}

void server_io_multiplexing_with_select() {
  const std::string __executive_file = HTML_FILE;
  Socket           *__socket         = new Socket(HTTP1_PORT);
  while (1) {
    fd_set __read_fds;
    FD_ZERO(&__read_fds);
    FD_SET(__socket->get_listenfd(), &__read_fds);
    int __ret =
        select(__socket->get_listenfd() + 1, &__read_fds, NULL, NULL, NULL);
    if (__ret == -1) {
      error_log_with_errno("select() failed.");
      continue;
    }
    if (FD_ISSET(__socket->get_listenfd(), &__read_fds)) {
      int __accfd =
          accept(__socket->get_listenfd(), (struct sockaddr *)NULL, NULL);
      if (__accfd == -1) {
        continue;
      }
      read_request(__accfd);
      send_response(__accfd);
    }
  }
  close(__socket->get_listenfd());
  return;
}

// /*
//  * struct pollfd {
//  *   int     fd;       // descriptor to check
//  *   short   events;   // events of interest on fd
//  *   short   revents;  // events that occurred on fd
//  * };
//  */

// #define FD_SETSIZE 1

// void server_io_multiplexing_with_poll() {
//   const std::string __executive_file = HTML_FILE;
//   Socket           *__socket         = new Socket(HTTP1_PORT);
//   while (1) {
//     struct pollfd __fds[1]; // pollfd array
//     __fds[0].fd     = __socket->get_listenfd();
//     __fds[0].events = POLLIN; //
//     優先度の高いデータ以外のポーラインデータは、ブロックせずに読み取ることができます。
//     int __ret = poll(__fds, 1, NGINX_TIMEOUT); //
//     第三引数は、ブロックする時間を指定するためのパラメータです。-1でタイムアウトの定義
//     if (__ret == -1) {
//       error_log_with_errno("poll() failed.");
//       continue;
//     }
//     if (__fds[0].revents & POLLIN) {
//       int __accfd =
//           accept(__socket->get_listenfd(), (struct sockaddr *)NULL, NULL);
//       if (__accfd == -1) {
//         error_log_with_errno("accept() failed.");
//         continue;
//       }
//       read_request(__accfd);
//     }
//   }
//   close(__socket->get_listenfd());
//   return;
// }