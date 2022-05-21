#ifndef SRCS_EVENT_TRANSACTION_HPP
#define SRCS_EVENT_TRANSACTION_HPP

#include <sys/types.h>

#include <string>

#include "config/Config.hpp"
#include "http/request/RequestInfo.hpp"

enum TransactionState {
  NO_REQUEST,       // Connectionはリクエストを持たない。
  RECEIVING_HEADER, // リクエストはheaderを読み取り中。
  RECEIVING_BODY,   // リクエストはbodyを読み取り中。
  PENDING, // リクエストの読み取りは完了。レスポンスの生成待ち。
  SENDING, // レスポンスの送信中。
  CLOSING,
};

// TODO: string -> vector<char>

struct Transaction {
private:
  TransactionState __transction_state_;
  ssize_t          __send_count_;
  std::string      __response_;
  RequestInfo      __requst_info_;
  const Config    *__conf_;

public:
  Transaction()
      : __transction_state_(RECEIVING_HEADER)
      , __send_count_(0)
      , __conf_(NULL) {}

  TransactionState get_tranction_state() const { return __transction_state_; }
  void             set_tranction_state(TransactionState state) {
    __transction_state_ = state;
  }
  bool   is_close() { return __requst_info_.is_close_; }
  size_t get_body_size() { return __requst_info_.content_length_; }
  bool   is_send_completed() {
    return __send_count_ == static_cast<ssize_t>(__response_.size());
  }

  void parse_header(const std::string &header);
  void detect_config(const confGroup &conf_group);
  void parse_body(const std::string &body);
  void create_response();
  void send_response(int socket_fd);
};

#endif /* SRCS_EVENT_TRANSACTION_HPP */
