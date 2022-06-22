#include "event/Request.hpp"

#include <sys/socket.h>

#include "http/const/const_delimiter.hpp"
#include "http/request/RequestInfo.hpp"
#include "http/response/ResponseGenerator.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/utils.hpp"

static std::string cutout_request_body(std::string &request_buffer,
                                       size_t       content_length) {
  std::string request_body = request_buffer.substr(0, content_length);
  request_buffer.erase(0, content_length);
  return request_body;
}

RequestState Request::_handle_request_startline(std::string &request_buffer) {
  std::string line;
  while (getline(request_buffer, line)) {
    _request_info_.check_first_multi_blank_line(line);
    // throws BadRequestException
    if (_request_info_.is_blank_first_line_) {
      continue;
    }
    RequestInfo::check_bad_parse_request_start_line(line);
    // throws BadRequestException
    _request_info_.parse_request_start_line(line); // noexcept
    return RECEIVING_HEADER;
  }
  return _state_;
}

RequestState Request::_handle_request_header(std::string &request_buffer) {
  std::string line;
  while (getline(request_buffer, line)) { // noexcept
    if (_state_ == RECEIVING_HEADER) {
      if (line != "") {
        RequestInfo::store_request_header_field_map(line, _field_map_);
        // throws BadRequestException
        continue;
      }
      _request_info_.parse_request_header(_field_map_);
      // throws BadRequestException
      // TODO: validate request_header
      // delete with body
      // has transfer-encoding but last elm is not chunked
      // content-length and transfer-encoding -> delete content-length
      _check_max_client_body_size_exception(
          _request_info_.content_length_,
          _request_info_.config_.client_max_body_size_);
      if (_request_info_.has_body()) {
        _state_ = RECEIVING_BODY;
        break;
      }
      _state_ = SUCCESS;
      break;
    }
  }
  return _state_;
}

RequestState Request::_handle_request_body(std::string &request_buffer) {
  if (_request_info_.is_chunked_) {
    _state_ = _chunk_loop(request_buffer);
    // throws BadRequestException
    _check_max_client_body_size_exception(
        _request_body_.size(), _request_info_.config_.client_max_body_size_);
    // throws BadRequestException
  } else if (request_buffer.size() >= _request_info_.content_length_) {
    _request_body_ =
        cutout_request_body(request_buffer, _request_info_.content_length_);
    _state_ = SUCCESS;
  }
  if (_state_ == SUCCESS) {
    _request_info_.parse_request_body(_request_body_,
                                      _request_info_.content_type_);
  }
  return _state_;
}

// 一つのリクエストのパースを行う、bufferに一つ以上のリクエストが含まれるときtrueを返す。
RequestState Request::handle_request(std::string       &request_buffer,
                                     const ConfigGroup &config_group) {
  try {
    if (_state_ == RECEIVING_STARTLINE) {
      _state_ = _handle_request_startline(request_buffer);
      _check_buffer_length_exception(request_buffer, BUFFER_MAX_LENGTH_);
    }
    if (_state_ == RECEIVING_HEADER) {
      _state_ = _handle_request_header(request_buffer);
      _check_buffer_length_exception(request_buffer, BUFFER_MAX_LENGTH_);
      _tmp(config_group);
    }
    if (_state_ == RECEIVING_BODY) {
      _state_ = _handle_request_body(request_buffer);
    }
    if (_state_ == SUCCESS) {
      _response_ = ResponseGenerator::generate_response(_request_info_);
    }
  } catch (const RequestInfo::BadRequestException &e) {
    _response_ =
        ResponseGenerator::generate_error_response(_request_info_, e.status());
    _request_info_.connection_close_ = true;
    _state_                          = SUCCESS;
  }
  return _state_;
}
