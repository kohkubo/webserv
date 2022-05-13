#ifndef SRCS_EVENT_CONNECTION
#define SRCS_EVENT_CONNECTION

#include "http/HttpMessage.hpp"
#include <deque>
#include <map>
#include <string>
#include <sys/types.h>
#include <vector>

typedef class ServerConfig ServerConfig;

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
  NO_REQUEST,       // Connectionはリクエストを持たない。
  RECEIVING_HEADER, // リクエストはheaderを読み取り中。
  RECEIVING_BODY,   // リクエストはbodyを読み取り中。
  PENDING, // リクエストの読み取りは完了。レスポンスの生成待ち。
  SENDING, // レスポンスの送信中。
};

// TODO: string -> vector<char>
struct Request {
  RequestState state_;
  std::string  header_;
  std::string  body_;
  HttpMessage  info_;
  std::string  response_;
  ssize_t      send_count_;

  Request()
      : state_(RECEIVING_HEADER)
      , send_count_(0) {}
};

class Connection {
private:
  std::deque<Request> __request_queue_;
  std::string         __buffer_;

public:
  int socket_fd_;

  Connection()
      : socket_fd_(-1) {}
  Connection(int fd)
      : socket_fd_(fd) {}
  ~Connection() {}

  Request &get_last_request() {
    if (__request_queue_.empty())
      __request_queue_.push_back(Request());
    return __request_queue_.back();
  }

  Request &get_front_request() {
    if (__request_queue_.empty())
      __request_queue_.push_back(Request());
    return __request_queue_.front();
  }

  void         erase_front_req() { __request_queue_.pop_front(); }

  RequestState get_last_state() {
    if (__request_queue_.empty())
      return NO_REQUEST;
    return __request_queue_.back().state_;
  }

  bool is_waiting_send() const {
    if (__request_queue_.empty())
      return false;
    RequestState state = __request_queue_.front().state_;
    return state == SENDING;
  }

  void        parse_buffer(const std::string &data);
  std::string cut_buffer(std::size_t len);
  void make_response(const std::vector<const ServerConfig *> &server_list);
};

typedef std::map<int, Connection> connection_list_type;

#endif /* SRCS_EVENT_CONNECTION */
