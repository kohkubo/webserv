#include <cstdlib>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "config/Config.hpp"
#include "utils/utils.hpp"

/*
 * リスニングソケットを作成する
 * int socket(int domain, int type, int protocol);
 *
 * macで扱っているプロトコルとプロトコル番号が知りたい場合は
 * `cat /etc/protocols` で確認できる
 *
 * socket(AF_INET, SOCK_STREAM, 0);
 * AF_INET      IPv4 インターネットドメイン
 * SOCK_STREAM
 * シーケンス、信頼性の高い、双方向接続ベースのバイトストリームを提供します。
 * 詳解UNIXでは「引数「protocol」は普通ゼロで、指定したドメインとソケット種別に
 * たいするデフォルトのプロトコルを選択します。」と書いてある。なぜ0になるのかわからない
 */
static listenFd get_listen_fd(struct addrinfo *info) {
  listenFd listen_fd = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
  if (listen_fd == -1) {
    error_log_with_errno("socket() failed.");
    exit(EXIT_FAILURE);
  }
  if (fcntl(listen_fd, F_SETFL, O_NONBLOCK) == -1) {
    error_log_with_errno("fcntl() failed.");
    exit(EXIT_FAILURE);
  }
  int on = 1;
  if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&on,
                 sizeof(on)) == -1) {
    error_log_with_errno("setsockopt() failed.");
    close(listen_fd);
    exit(EXIT_FAILURE);
  }
  return listen_fd;
}

/*
 * ソケットを関連付ける
 * int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
 * sockfd 　ソケットディスクリプタ
 * addr    ソケットアドレス
 * addrlen ソケットアドレス長
 */
static void bind_socket(listenFd listen_fd, struct addrinfo *info) {
  if (bind(listen_fd, info->ai_addr, info->ai_addrlen) == -1) {
    error_log_with_errno("bind() failed.");
    close(listen_fd);
    exit(EXIT_FAILURE);
  }
}

/*
 * 接続を待つ
 * int listen(int sockfd, int backlog);
 * sockfd 　ソケットディスクリプタ
 * backlog 　待ち受けキューの最大数
 */
static void listen_passive_socket(listenFd listen_fd) {
  if (listen(listen_fd, SOMAXCONN) == -1) {
    error_log_with_errno("listen() failed.");
    close(listen_fd);
    exit(EXIT_FAILURE);
  }
}

int open_new_socket(const Config &config) {
  listenFd listen_fd = get_listen_fd(config.addrinfo_);
  bind_socket(listen_fd, config.addrinfo_);
  listen_passive_socket(listen_fd);
  return listen_fd;
}
