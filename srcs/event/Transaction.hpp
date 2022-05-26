#ifndef SRCS_EVENT_TRANSACTION_HPP
#define SRCS_EVENT_TRANSACTION_HPP

#include <sys/types.h>

#include <string>

#include "config/Config.hpp"
#include "http/request/RequestInfo.hpp"

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
  TransactionState __transaction_state_;
  ssize_t          __send_count_;
  std::string      __response_;
  RequestInfo      __request_info_;
  const Config    *__conf_;

private:
  std::string __getline_from_buffer(std::string &buf);
  bool        __check_line(const std::string &request_buffer);
  void        __parse_single_line(std::string     &request_buffer,
                                  const confGroup &conf_group);
  void        __set_response_for_bad_request();
  bool        __is_send_all() const {
    return __send_count_ == static_cast<ssize_t>(__response_.size());
  }

public:
  Transaction()
      : __transaction_state_(RECEIVING_STARTLINE)
      , __send_count_(0)
      , __conf_(NULL) {}

  // for test
  RequestInfo &get_request_info() { return __request_info_; }

  bool         is_close() const { return __request_info_.is_close_; }
  bool         is_sending() const { return __transaction_state_ == SENDING; }
  size_t       get_body_size() const { return __request_info_.content_length_; }
  // test用
  const Config *get_conf() { return __conf_; }

  bool          parse_single_request(std::string     &request_buffer,
                                     const confGroup &conf_group);
  void          parse_start_line(std::string &buf);
  void          parse_header(std::string &buf, const confGroup &conf_group);
  void          parse_body(std::string &buf);
  void          parse_chunked_body(std::string &request_buffer);
  void          detect_config(const confGroup &conf_group);
  void          create_response();
  bool          send_response(int socket_fd);
};

#endif /* SRCS_EVENT_TRANSACTION_HPP */
