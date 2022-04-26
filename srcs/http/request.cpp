#include <algorithm>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "ServerConfig.hpp"
#include "http.hpp"
#include "util.hpp"

#define BUF_SIZE 1024

// TODO: Requestのエラーについて、未調査です。Getにおいては、Hostだけで良さそう
bool is_request_error(std::vector<std::string> &request_tokens) {
  if (request_tokens.size() < 5) {
    return true;
  }
  if (std::count(request_tokens.begin(), request_tokens.end(), "Host:") != 1) {
    return true;
  }
  return false;
}

// example.com -> example.com
// localhost:8080 -> localhost
// Host - HTTP | MDN
// https://developer.mozilla.org/ja/docs/Web/HTTP/Headers/Host
// Host: <host>:<port>
// tokenize関数を使う予定でしたが、findが使えることに気づいたので、そちらを利用しています。
static std::string parse_request_host(const std::string &request_line_host) {
  std::string::size_type pos = request_line_host.find(':');
  if (pos == std::string::npos) {
    return request_line_host;
  }
  return request_line_host.substr(0, pos);
}

static HttpMethod parse_request_method(const std::string &method) {
  if (method == "GET") {
    return GET;
  }
  if (method == "POST") {
    return POST;
  }
  if (method == "DELETE") {
    return DELETE;
  }
  return UNKNOWN;
}

/*
 * 超安易パース
 * 必須情報のみを取得しています。
 */
static HttpMessage
parse_request_message(std::vector<std::string> &request_tokens) {
  std::vector<std::string>::iterator it = request_tokens.begin();
  HttpMessage                        request_message;
  request_message.method_  = parse_request_method(*it); // "GET"
  request_message.url_     = *(++it); // "example.com/test/test"
  request_message.version_ = *(++it); // "HTTP/1.1"
  request_message.host_    = parse_request_host(*(++it)); // "example.com"
  return request_message;
}

static std::string read_connected_fd(int accfd) {
  char        buf[BUF_SIZE] = {};
  std::string recv_str      = "";
  ssize_t     read_size     = 0;

  std::cout << "start recieving request....." << std::endl;
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
      tokenize(read_connected_fd(accfd), SEPARATOR, SEPARATOR);
  if (is_request_error(request_tokens)) {
    std::cout << "request error." << std::endl;
    HttpMessage request_message;
    request_message.status_code_ = BAD_REQUEST;
    return request_message;
  }
  return parse_request_message(request_tokens);
}
