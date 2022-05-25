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
  SENDING,          // レスポンスの送信中。
  CLOSING,
};

// TODO: string -> vector<char>

struct Transaction {
private:
  TransactionState __transaction_state_;
  ssize_t          __send_count_;
  std::string      __response_;
  RequestInfo      __request_info_;
  const Config    *__conf_;

private:
  std::string __cut_buffer(std::string &request_buffer, std::size_t len);
  bool        __is_send_all() const {
    return __send_count_ == static_cast<ssize_t>(__response_.size());
  }

public:
  Transaction()
      : __transaction_state_(RECEIVING_HEADER)
      , __send_count_(0)
      , __conf_(NULL) {}

  bool   is_close() const { return __request_info_.is_close_; }
  bool   is_sending() const { return __transaction_state_ == SENDING; }
  size_t get_body_size() const { return __request_info_.content_length_; }
  // test用
  const Config *get_conf() { return __conf_; }

  bool          handle_transaction_state(std::string     &request_buffer,
                                         const confGroup &conf_group);
  void parse_header(const std::string &header, const confGroup &conf_group);
  void detect_config(const confGroup &conf_group);
  void parse_body(const std::string &body);
  void create_response();
  bool send_response(int socket_fd);
};

#endif /* SRCS_EVENT_TRANSACTION_HPP */
