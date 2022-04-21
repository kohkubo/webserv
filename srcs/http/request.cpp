#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "ServerConfig.hpp"
#include "http.hpp"
#include "util.hpp"

#define BUF_SIZE 1024

static std::string index_path(ServerConfig                       &server_config,
                              std::vector<std::string>::iterator &it) {
  std::string path;
  if (*it == "/") {
    for (std::vector<std::string>::iterator it_index =
             server_config.index_.begin();
         it_index != server_config.index_.end(); ++it_index) {
      path = server_config.root_ + *it_index;
      if (is_file_exists(path.c_str())) {
        return path;
      }
    }
  } else {
    path = server_config.root_ + *it;
    if (is_file_exists(path.c_str())) {
      return path;
    }
  }
  server_config.status_code_ = 404;
  return "";
}

/*
 * 超安易パース
 * リクエストメッセージのターゲットURLが"/"の時index.htmlに差し替えることだけしています.
 * mapへの挿入時keyが被っている時の処理は現状考慮してない.
 */
static http_message_map
parse_request_message(ServerConfig             &server_config,
                      std::vector<std::string> &request_tokens) {
  std::vector<std::string>::iterator it = request_tokens.begin();
  http_message_map                   request_message;
  request_message[METHOD] = *it; // *it = "http_method_read_get"
  it++;
  it++;
  request_message[URL] = index_path(server_config, it); // /index.html
  it++;
  it++;
  request_message[VERSION]   = *it; // *it = "HTTP/1.1"
  request_message[HOST]      = "example.com";
  request_message[USERAGENT] = "curl/7.79.1";
  request_message[ACCEPT]    = "*/*";
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

/*
 * メッセージ読み込み
 */
http_message_map receive_request(ServerConfig &server_config, int accfd) {
  std::vector<std::string> request_tokens =
      tokenize(read_connected_fd(accfd), SP, "");
  http_message_map request_message =
      parse_request_message(server_config, request_tokens);
  return request_message;
}
