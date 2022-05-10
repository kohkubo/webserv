#ifndef SRCS_EVENT_CONNECTION
#define SRCS_EVENT_CONNECTION

#include <deque>
#include <string>
#include <sys/types.h>

/*
  確立されたtcpコネクションからは接続が閉じられない限り、
  複数のリクエストが連続して送信されてくる可能性がある。
  先着したリクエストを処理している間、後続のリクエストを保存するキュー構造が必要。

  deque.backのRequestStateを確認、
  receivingのときはリクエストの受信が終わっていない、received_data_へ追記。
  それ以外の時は、新しいRequestを追加。
  受信したデータが一つ以上のリクエストを含むときの処理も必要
*/

enum RequestState {
  RECEIVING,
  PENDING,
  PROCESSING,
  SENDING,
};

struct Request {
  RequestState state_;
  std::string  received_data_;
  ssize_t      send_count_;
  std::string  response_;
};

struct Connection {
  int                 socket_fd_;
  std::deque<Request> request_queue_;

  Connection(int fd)
      : socket_fd_(fd) {}
  ~Connection();
  is_new_request();
};

#endif /* SRCS_EVENT_CONNECTION */
