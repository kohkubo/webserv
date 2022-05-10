#ifndef SRCS_EVENT_CONNECTION
#define SRCS_EVENT_CONNECTION

#include <deque>
#include <string>
#include <sys/types.h>
#include <vector>

/*
  確立されたtcpコネクションからは接続が閉じられない限り、
  複数のリクエストが連続して送信されてくる可能性がある。
  先着したリクエストを処理している間、後続のリクエストを保存するキュー構造が必要。

  deque.backのRequestStateを確認、
  receivingのときはリクエストの受信が終わっていない、対応するvectorへデータ格納。
  それ以外の時は、新しいRequestを追加。
  受信したデータが一つ以上のリクエストを含むときの処理も必要

  レスポンスを返す順番はリクエストを受けた順番。
*/

enum RequestState {
  RECEIVING_HEADER,
  RECEIVING_BODY,
  PENDING,
  PROCESSING,
  SENDING,
};

struct Request {
  RequestState      state_;
  std::vector<char> request_header_;
  std::vector<char> request_body_;
  ssize_t           send_count_;
  std::vector<char> response_;
};

struct Connection {
  int                 socket_fd_;
  std::deque<Request> request_queue_;

  Connection(int fd)
      : socket_fd_(fd) {}
  ~Connection();
  bool is_new_request() const {
    if (request_queue_.empty())
      return true;
    return !(request_queue_.back().state_ == RECEIVING_HEADER ||
             request_queue_.back().state_ == RECEIVING_BODY);
  }
};

#endif /* SRCS_EVENT_CONNECTION */
