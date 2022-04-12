#include "Socket.hpp"
#include <cstdlib>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

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
void Socket::set_listenfd() {
  // TODO: getaddrinfoに書き換え　CSAPP
  // ベストプラクティスはgetaddrinfo()を使うことで引数をプロトコル非依存にする。
  __listenfd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (__listenfd_ == -1) {
    error_log_with_errno("socket() failed.");
    exit(1);
  }
  if (fcntl(__listenfd_, F_SETFL, O_NONBLOCK) == -1) {
    error_log_with_errno("fcntl() failed.");
    exit(EXIT_FAILURE);
  }
}

/*
 * sockaddr_in構造体の設定
 *　INADDR_ANY　一般には0.0.0.0が定義され、全てのローカルインターフェイスにバインドされうることを意味する。
 */
void Socket::set_sockaddr_in() {
  memset(&__serv_addr_, 0, sizeof(__serv_addr_));
  __serv_addr_.sin_family = AF_INET;
  inet_pton(AF_INET, __server_config_.listen_ip_.c_str(),
            &__serv_addr_.sin_addr);
  __serv_addr_.sin_port = htons(__server_config_.listen_port_);
}

/*
 * ソケットを関連付ける
 * int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
 * sockfd 　ソケットディスクリプタ
 * addr    ソケットアドレス
 * addrlen ソケットアドレス長
 */
void Socket::set_bind() {
  if (bind(__listenfd_, (struct sockaddr *)&__serv_addr_,
           sizeof(__serv_addr_)) == -1) {
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
void Socket::set_listen() {
  if (listen(__listenfd_, SOMAXCONN) == -1) {
    error_log_with_errno("listen() failed.");
    close(__listenfd_);
    exit(1);
  }
}