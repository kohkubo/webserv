#include "../ServerConfig.hpp"
#include "../tokenize.hpp"
#include "../util.hpp"
#include "request_parse.hpp"
#include <algorithm>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 1024

// TODO: Requestのエラーについて、未調査です。Getにおいては、Hostだけで良さそう
static bool is_request_error(std::vector<std::string> &request_tokens) {
  if (request_tokens.size() < 5) {
    return true;
  }
  if (std::count(request_tokens.begin(), request_tokens.end(), "Host:") != 1) {
    return true;
  }
  return false;
}

/*
 * 超安易パース
 * 必須情報のみを取得しています。
 */
static HttpMessage
parse_request_message(std::vector<std::string> &request_tokens) {
  HttpMessage request_message;
  parse_request_method_line(request_message, request_tokens);
  parse_request_host(request_message, request_tokens);
  parse_request_content_length(request_message, request_tokens);
  parse_request_body(request_message, request_tokens);
  return request_message;
}

static std::string read_connected_fd(int accfd) {
  char        buf[BUF_SIZE] = {};
  std::string recv_str      = "";
  ssize_t     read_size     = 0;

  std::cout << "start recieving request....." << std::endl;
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(accfd, &readfds);
  do {
    int ret = select(accfd + 1, &readfds, NULL, NULL, NULL);
    std::cout << "threwed select" << std::endl;
    if (ret == -1) {
      error_log_with_errno("select() failed. readfds.");
      read_size = 1;
    }
    if (ret && FD_ISSET(accfd, &readfds)) {
      std::cout << "you can read" << std::endl;
      read_size = recv(accfd, buf, sizeof(buf) - 1, 0);
      if (read_size == -1) {
        error_log_with_errno("read() failed.");
        exit(1);
      }
      if (read_size > 0) {
        recv_str.append(buf);
        break;
      }
    }
  } while (read_size != 0);
  std::cout << "receive_request()" << std::endl;
  std::cout << recv_str << std::endl;
  return recv_str;
}

#define SEPARATOR " \r\n"
/*
 * メッセージ読み込み
 */
HttpMessage receive_request(int accfd) {
  std::vector<std::string> request_tokens =
      tokenize(read_connected_fd(accfd), SEPARATOR, " ");
  if (is_request_error(request_tokens)) {
    std::cout << "request error." << std::endl;
    HttpMessage request_message;
    request_message.status_code_ = BAD_REQUEST_400;
    return request_message;
  }
  return parse_request_message(request_tokens);
}
