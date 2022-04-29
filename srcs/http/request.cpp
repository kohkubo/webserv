#include "ServerConfig.hpp"
#include "http.hpp"
#include "request_parse.hpp"
#include "util.hpp"
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

  do {
    read_size = recv(accfd, buf, sizeof(buf) - 1, 0);
    if (read_size == -1) {
      error_log_with_errno("read() failed.");
      close(accfd);
      // accfd = -1;
      break;
    }
    if (read_size > 0) {
      recv_str.append(buf);
    }
    if (is_match_suffix_string(recv_str, "\r\n\r\n")) {
      break;
    }
  } while (read_size > 0);
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
    request_message.status_code_ = BAD_REQUEST;
    return request_message;
  }
  return parse_request_message(request_tokens);
}
