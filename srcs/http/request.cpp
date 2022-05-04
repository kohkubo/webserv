#include "config/ServerConfig.hpp"
#include "http/const_delimiter.hpp"
#include "utils/tokenize.hpp"
#include "utils/util.hpp"
#include <cstdlib>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 1024

static std::string read_connected_fd(int accfd) {
  char        buf[BUF_SIZE] = {};
  std::string recv_str      = "";
  ssize_t     read_size     = 0;

  std::cout << "start recieving request....." << std::endl;
  do {
    read_size = recv(accfd, buf, sizeof(buf) - 1, 0);
    if (read_size == -1) {
      error_log_with_errno("read() failed.");
      exit(1);
    }
    if (read_size > 0) {
      recv_str.append(buf);
      break;
    }
  } while (read_size != 0);
  std::cout << "receive_request()" << std::endl;
  std::cout << recv_str << std::endl;
  return recv_str;
}

/*
 * メッセージ読み込み
 */
// read_connected_fdに変更が入りそうなので一旦放置
std::vector<std::string> receive_request(int accfd) {
  return tokenize(read_connected_fd(accfd), SEPARATOR, " ");
}
