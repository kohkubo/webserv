#include "event/Request.hpp"

#include <sys/socket.h>

#include "http/const/const_delimiter.hpp"
#include "http/request/RequestInfo.hpp"
#include "http/response/ResponseGenerator.hpp"
#include "utils/utils.hpp"

static const Config *select_proper_config(const confGroup   &conf_group,
                                          const std::string &host_name) {
  confGroup::const_iterator it = conf_group.begin();
  for (; it != conf_group.end(); it++) {
    if ((*it)->server_name_ == host_name) {
      return *it;
    }
  }
  return conf_group[0];
}

// 一つのリクエストのパースを行う、bufferに一つ以上のリクエストが含まれるときtrueを返す。
RequestState Request::handle_request(std::string     &request_buffer,
                                     const confGroup &conf_group) {
  try {
    if (__state_ == RECEIVING_STARTLINE || __state_ == RECEIVING_HEADER) {
      std::string line;
      while (getline(request_buffer, line)) { // noexcept
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
          __config_ = select_proper_config(conf_group, __request_info_.host_);
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
          __state_ = SUCCESS;
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
        __state_ = SUCCESS;
      }
      if (__state_ == SUCCESS) {
        __request_info_.parse_request_body(__request_body_,
                                           __request_info_.content_type_);
      }
    }
    if (__state_ == SUCCESS) {
      __response_ =
          ResponseGenerator::generate_response(*__config_, __request_info_);
    } else if (__state_ == RECEIVING_STARTLINE ||
               __state_ == RECEIVING_HEADER) {
      __check_buffer_length_exception(request_buffer, BUFFER_MAX_LENGTH_);
    }
  } catch (const RequestInfo::BadRequestException &e) {
    __response_ = ResponseGenerator::generate_error_response(
        Location(), Config(), e.status());
    __request_info_.connection_close_ = true;
    __state_                          = SUCCESS;
  }
  return __state_;
}

std::string Request::__cutout_request_body(std::string &request_buffer,
                                           size_t       content_length) {
  std::string request_body = request_buffer.substr(0, content_length);
  request_buffer.erase(0, content_length);
  return request_body;
}

bool Request::__get_next_chunk_line(NextChunkType chunk_type,
                                    std::string  &request_buffer,
                                    std::string  &chunk,
                                    size_t        next_chunk_size) {
  if (chunk_type == CHUNK_SIZE) {
    return getline(request_buffer, chunk);
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

RequestState Request::__chunk_loop(std::string &request_buffer) {
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
        return SUCCESS;
      }
      __request_body_.append(chunk_line);
      __next_chunk_ = CHUNK_SIZE;
    }
  }
  if (__next_chunk_ == CHUNK_SIZE)
    __check_buffer_length_exception(request_buffer, BUFFER_MAX_LENGTH_);
  return RECEIVING_BODY;
}

void Request::__check_max_client_body_size_exception(
    std::size_t actual_body_size, std::size_t max_body_size) {
  if (actual_body_size > max_body_size) {
    throw RequestInfo::BadRequestException(ENTITY_TOO_LARGE_413);
  }
}

void Request::__check_buffer_length_exception(std::string &request_buffer,
                                              std::size_t  buffer_max_length) {
  if (request_buffer.size() >= buffer_max_length) {
    request_buffer.clear();
    throw RequestInfo::BadRequestException();
  }
}