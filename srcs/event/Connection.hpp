#ifndef SRCS_EVENT_CONNECTION
#define SRCS_EVENT_CONNECTION

#include "http/HttpMessage.hpp"
#include <deque>
#include <map>
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
  NO_REQUEST,
  RECEIVING_HEADER,
  RECEIVING_BODY,
  PENDING,
  WAITING,
  SENDING,
};

// TODO: string -> vector<char>
struct Request {
  RequestState state_;
  std::string  request_header_;
  HttpMessage  request_info_;
  std::string  request_body_;
  ssize_t      send_count_;
  std::string  response_;

  Request()
      : state_(RECEIVING_HEADER) {}
};

struct Connection {
  int                 socket_fd_;
  std::deque<Request> request_queue_;
  std::string         buffer_;

  Connection(int fd)
      : socket_fd_(fd) {}
  ~Connection();

  RequestState get_last_state() {
    if (request_queue_.empty())
      return NO_REQUEST;
    return request_queue_.back().state_;
  }
  void parse_buffer(const std::string &data);
};

typedef std::map<int, Connection> connection_list_type;

#endif /* SRCS_EVENT_CONNECTION */
