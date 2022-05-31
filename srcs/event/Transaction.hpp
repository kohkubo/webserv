#ifndef SRCS_EVENT_TRANSACTION_HPP
#define SRCS_EVENT_TRANSACTION_HPP

#include <sys/types.h>

#include <string>

#include "config/Config.hpp"
#include "http/request/RequestInfo.hpp"

typedef int connFd;

enum TransactionState {
  NO_REQUEST, // Connectionはリクエストを持たない。
  RECEIVING_STARTLINE,
  RECEIVING_HEADER, // リクエストはheaderを読み取り中。
  RECEIVING_BODY,   // リクエストはbodyを読み取り中。
  SENDING,          // レスポンスの送信中。
  CLOSING,
};

// TODO: string -> vector<char>

struct Transaction {
private:
  connFd           __conn_fd_;
  TransactionState __transaction_state_;
  ssize_t          __send_count_;
  std::string      __response_;
  RequestInfo      __request_info_;

private:
  static bool __getline(std::string &request_buffer, std::string &line);
  bool __get_request_body(std::string &request_buffer, std::string &body) const;

public:
  Transaction(connFd conn_fd)
      : __conn_fd_(conn_fd)
      , __transaction_state_(RECEIVING_STARTLINE)
      , __send_count_(0) {}
  void               set_response_for_bad_request();
  const Config      *get_proper_config(const confGroup &conf_group) const;
  void               create_response(const Config *config);
  const RequestInfo &get_request_info() const { return __request_info_; }
  TransactionState   get_transaction_state() const {
    return __transaction_state_;
  }
  void set_transaction_state(TransactionState transaction_state) {
    __transaction_state_ = transaction_state;
  }
  void handle_request(std::string &request_buffer);
  void send_response();
  bool is_send_all() const {
    return __send_count_ == static_cast<ssize_t>(__response_.size());
  }
};

#endif /* SRCS_EVENT_TRANSACTION_HPP */
