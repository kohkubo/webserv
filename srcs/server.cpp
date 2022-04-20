#include <iostream>
#include <netdb.h>
#include <poll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "Socket.hpp"
#include "Webserv.hpp"
#include "config/Config.hpp"
#include "http.hpp"
#include "util.hpp"



void server_io_multiplexing() {
  std::string config_file = read_file_tostring("conf/webserv.conf");
  Lexer l(config_file, "\v\r\f\t\n " "{};");
  try
  {
    Config config(l);
    //std::cout << "---->" << config.server_config_.listen_host_ << std::endl;
    //std::cout << "---->" << config.server_config_.listen_port_ << std::endl;
    Socket      *socket = new Socket(config.server_config_);
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
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return;
  }
  return;
}
