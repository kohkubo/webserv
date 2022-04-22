#include "Socket.hpp"
#include <cstdlib>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

Socket::Socket(const ServerConfig &config) : __server_config_(config) {
  __set_addrinfo();
  __set_listenfd();
  __set_bind();
  __set_listen();
}

Socket::~Socket() {
  freeaddrinfo(__addrinfo_);
  // close(__listenfd_);
}

void Socket::__set_addrinfo() {
  // TODO: 不要ならhintsはNULLにするように変更する
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  int error =
      getaddrinfo(__server_config_.listen_address_.c_str(),
                  __server_config_.listen_port_.c_str(), &hints, &__addrinfo_);
  if (error) {
    std::cerr << "getaddrinfo: " << gai_strerror(error) << std::endl;
  }
}

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
 * 0　　　　　　　　　　　　　　　　　　　　　　　　詳解UNIXでは「引数「protocol」は普通ゼロで、指定したドメインとソケット種別に
 *              たいするデフォルトのプロトコルを選択します。」と書いてある。なぜ0になるのかわからない
 */
void Socket::__set_listenfd() {
  __listenfd_ = socket(__addrinfo_->ai_family, __addrinfo_->ai_socktype,
                       __addrinfo_->ai_protocol);
  if (__listenfd_ == -1) {
    error_log_with_errno("socket() failed.");
    exit(1);
  }
  if (fcntl(__listenfd_, F_SETFL, O_NONBLOCK) == -1) {
    error_log_with_errno("fcntl() failed.");
    exit(EXIT_FAILURE);
  }
  int on = 1;
  if (setsockopt(__listenfd_, SOL_SOCKET, SO_REUSEADDR, (const void *)&on,
                 sizeof(on)) == -1) {
    error_log_with_errno("setsockopt() failed.");
    close(__listenfd_);
    exit(EXIT_FAILURE);
  }
}

/*
 * ソケットを関連付ける
 * int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
 * sockfd 　ソケットディスクリプタ
 * addr    ソケットアドレス
 * addrlen ソケットアドレス長
 */
void Socket::__set_bind() {
  if (bind(__listenfd_, __addrinfo_->ai_addr, __addrinfo_->ai_addrlen) == -1) {
    error_log_with_errno("bind() failed.");
    close(__listenfd_);
    exit(1);
  }
}

/*
 * 接続を待つ
 * int listen(int sockfd, int backlog);
 * sockfd 　ソケットディスクリプタ
 * backlog 　待ち受けキューの最大数
 */
void Socket::__set_listen() {
  if (listen(__listenfd_, SOMAXCONN) == -1) {
    error_log_with_errno("listen() failed.");
    close(__listenfd_);
    exit(1);
  }
}