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
  TransactionState                   __transaction_state_;
  RequestInfo                        __request_info_;
  NextChunkType                      __next_chunk_;
  std::size_t                        __next_chunk_size_;
  std::string                        __unchunked_body_;
  std::map<std::string, std::string> __field_map_;

private:
  static bool __getline(std::string &request_buffer, std::string &line);
  static void __set_request_body(std::string &request_buffer, std::string &body,
                                 size_t content_length);
  static bool __get_next_chunk_line(NextChunkType chunk_type,
                                    std::string  &request_buffer,
                                    std::string &chunk, size_t next_chunk_size);
  TransactionState __chunk_loop(std::string     &request_buffer,
                                std::string     &request_body,
                                TransactionState transaction_state);

public:
  Transaction()
      : __transaction_state_(RECEIVING_STARTLINE)
      , __next_chunk_(CHUNK_SIZE)
      , __next_chunk_size_(-1) {}

  static const Config *get_proper_config(const confGroup   &conf_group,
                                         const std::string &host);
  // TODO: getterがconstじゃないの変だがとりあえず暫定的にこうしている。
  RequestInfo         &get_request_info() { return __request_info_; }
  TransactionState     get_transaction_state() const {
    return __transaction_state_;
  }
  void set_transaction_state(TransactionState transaction_state) {
    __transaction_state_ = transaction_state;
  }
  void handle_request(std::string &request_buffer);
};

#endif /* SRCS_EVENT_TRANSACTION_HPP */
