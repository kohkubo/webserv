#include "event/Transaction.hpp"

#include <sys/socket.h>

#include "http/const/const_delimiter.hpp"
#include "http/request/RequestInfo.hpp"
#include "http/response/Response.hpp"

// TODO: ステータスコードに合わせたレスポンスを生成
void Transaction::__set_response_for_bad_request() {
  // 400エラー処理
  // TODO: nginxのerror_pageディレクティブで400指定できるか確認。
  // 指定できるとき、nginxはどうやってserverを決定しているか。
  // serverが決定できる不正なリクエストと決定できないリクエストを実際に送信して確認？
  // 現状は暫定的に、定型文を送信。
  __response_ = "HTTP/1.1 400 Bad Request\r\nconnection: close\r\n\r\n";
  __state_    = COMPLETE;
  __request_info_.connection_close_ = true;
}

// 一つのリクエストのパースを行う、bufferに一つ以上のリクエストが含まれるときtrueを返す。
void Transaction::handle_request(std::string     &request_buffer,
                                 const confGroup &conf_group) {
  try {
    if (__state_ == RECEIVING_STARTLINE || __state_ == RECEIVING_HEADER) {
      std::string line;
      while (__getline(request_buffer, line)) { // noexcept
        if (__state_ == RECEIVING_STARTLINE) {
          __request_info_.check_first_multi_blank_line(line);
          // throws BadRequestException
          if (__request_info_.is_blank_first_line_) {
            continue;
          }
          RequestInfo::check_bad_parse_request_start_line(line);
          // throws BadRequestException
          __request_info_.parse_request_start_line(line); // noexcept
          __state_ = RECEIVING_HEADER;
        } else if (__state_ == RECEIVING_HEADER) {
          if (line != "") {
            RequestInfo::store_request_header_field_map(line, __field_map_);
            // throws BadRequestException
            continue;
          }
          __request_info_.parse_request_header(__field_map_);
          // throws BadRequestException
          __config_ = __select_proper_config(conf_group, __request_info_.host_);
          // TODO: validate request_header
          // delete with body
          // has transfer-encoding but last elm is not chunked
          // content-length and transfer-encoding -> delete content-length
          __check_max_client_body_size_exception(
              __request_info_.content_length_,
              __config_->client_max_body_size_);
          if (__request_info_.content_length_ != 0 ||
              __request_info_.is_chunked_) {
            __state_ = RECEIVING_BODY;
            break;
          }
          __state_ = COMPLETE;
          break;
        }
      }
    }
    if (__state_ == RECEIVING_BODY) {
      if (__request_info_.is_chunked_) {
        __state_ = __chunk_loop(request_buffer);
        // throws BadRequestException
        __check_max_client_body_size_exception(
            __request_body_.size(), __config_->client_max_body_size_);
        // throws BadRequestException
      } else if (request_buffer.size() >= __request_info_.content_length_) {
        __request_body_ = __cutout_request_body(
            request_buffer, __request_info_.content_length_);
        __state_ = COMPLETE;
      }
      if (__state_ == COMPLETE) {
        __request_info_.parse_request_body(__request_body_,
                                           __request_info_.content_type_);
      }
    }
    if (__state_ == COMPLETE) {
      __response_ = Response::generate_response(*__config_, __request_info_);
    } else if (__state_ == RECEIVING_STARTLINE ||
               __state_ == RECEIVING_HEADER) {
      __check_buffer_length_exception(request_buffer, buffer_max_length_);
    }
  } catch (const RequestInfo::BadRequestException &e) {
    __set_response_for_bad_request();
  }
}

bool Transaction::__getline(std::string &request_buffer, std::string &line) {
  std::size_t pos = request_buffer.find(CRLF);
  if (pos == std::string::npos)
    return false;
  line = request_buffer.substr(0, pos);
  request_buffer.erase(0, pos + 2);
  return true;
}

std::string Transaction::__cutout_request_body(std::string &request_buffer,
                                               size_t       content_length) {
  std::string request_body = request_buffer.substr(0, content_length);
  request_buffer.erase(0, content_length);
  return request_body;
}

bool Transaction::__get_next_chunk_line(NextChunkType chunk_type,
                                        std::string  &request_buffer,
                                        std::string  &chunk,
                                        size_t        next_chunk_size) {
  if (chunk_type == CHUNK_SIZE) {
    return __getline(request_buffer, chunk);
  }
  if (request_buffer.size() < next_chunk_size + CRLF.size()) {
    return false;
  }
  chunk = request_buffer.substr(0, next_chunk_size);
  request_buffer.erase(0, next_chunk_size);
  if (!has_prefix(request_buffer, CRLF)) {
    throw RequestInfo::BadRequestException();
  }
  request_buffer.erase(0, CRLF.size());
  return true;
}

const Config *
Transaction::__select_proper_config(const confGroup   &conf_group,
                                    const std::string &host_name) {
  confGroup::const_iterator it = conf_group.begin();
  for (; it != conf_group.end(); it++) {
    if ((*it)->server_name_ == host_name) {
      return *it;
    }
  }
  return conf_group[0];
}

TransactionState Transaction::__chunk_loop(std::string &request_buffer) {
  std::string chunk_line;
  while (__get_next_chunk_line(__next_chunk_, request_buffer, chunk_line,
                               __next_chunk_size_)) {
    if (__next_chunk_ == CHUNK_SIZE) {
      // TODO: validate chunk_line
      __next_chunk_size_ = hexstr_to_size(chunk_line);
      __next_chunk_      = CHUNK_DATA;
    } else {
      bool is_last_chunk = __next_chunk_size_ == 0 && chunk_line == "";
      if (is_last_chunk) {
        return COMPLETE;
      }
      __request_body_.append(chunk_line);
      __next_chunk_ = CHUNK_SIZE;
    }
  }
  if (__next_chunk_ == CHUNK_SIZE)
    __check_buffer_length_exception(request_buffer, buffer_max_length_);
  return RECEIVING_BODY;
}

void Transaction::__check_max_client_body_size_exception(
    std::size_t actual_body_size, std::size_t max_body_size) {
  if (actual_body_size > max_body_size) {
    throw RequestInfo::BadRequestException(ENTITY_TOO_LARGE_413);
  }
}

void Transaction::__check_buffer_length_exception(
    std::string &request_buffer, std::size_t buffer_max_length) {
  if (request_buffer.size() >= buffer_max_length) {
    request_buffer.clear();
    throw RequestInfo::BadRequestException();
  }
}