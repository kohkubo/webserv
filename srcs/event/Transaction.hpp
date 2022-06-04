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

enum NextChunkType { CHUNK_SIZE, CHUNK_DATA };

// TODO: string -> vector<char>

struct Transaction {
private:
  const Config                      *__config_;
  TransactionState                   __transaction_state_;
  ssize_t                            __send_count_;
  std::string                        __response_;
  RequestInfo                        __request_info_;
  NextChunkType                      __next_chunk_;
  std::size_t                        __next_chunk_size_;
  std::string                        __request_body_;
  std::map<std::string, std::string> __field_map_;
  static const std::size_t           buffer_max_length_ = 8192;

private:
  static bool      __getline(std::string &request_buffer, std::string &line);
  static void      __set_request_body(std::string &request_buffer,
                                      std::string &request_body,
                                      size_t       content_length);
  static bool      __get_next_chunk_line(NextChunkType chunk_type,
                                         std::string  &request_buffer,
                                         std::string &chunk, size_t next_chunk_size);
  TransactionState __chunk_loop(std::string &request_buffer);
  static void
  __check_max_client_body_size_exception(std::size_t actual_body_size,
                                         std::size_t max_body_size);
  static void __check_buffer_length_exception(std::string &request_buffer,
                                              std::size_t  buffer_limit_length);
  static const Config *__get_proper_config(const confGroup   &conf_group,
                                           const std::string &host_name);

public:
  Transaction()
      : __config_(NULL)
      , __transaction_state_(RECEIVING_STARTLINE)
      , __send_count_(0)
      , __next_chunk_(CHUNK_SIZE)
      , __next_chunk_size_(-1) {}

  void               set_response_for_bad_request();
  void               create_response();
  const RequestInfo &get_request_info() const { return __request_info_; }
  TransactionState   get_transaction_state() const {
    return __transaction_state_;
  }
  void set_transaction_state(TransactionState transaction_state) {
    __transaction_state_ = transaction_state;
  }
  void handle_request(std::string &request_buffer, const confGroup &conf_group);
  void send_response(connFd conn_fd);
  bool is_send_all() const {
    return __send_count_ == static_cast<ssize_t>(__response_.size());
  }
};

#endif /* SRCS_EVENT_TRANSACTION_HPP */
