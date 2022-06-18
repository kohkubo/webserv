#include "event/Request.hpp"

#include <sys/socket.h>

#include "http/const/const_delimiter.hpp"
#include "http/request/RequestInfo.hpp"
#include "http/response/ResponseGenerator.hpp"
#include "utils/file_io_utils.hpp"
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

static std::string cutout_request_body(std::string &request_buffer,
                                       size_t       content_length) {
  std::string request_body = request_buffer.substr(0, content_length);
  request_buffer.erase(0, content_length);
  return request_body;
}

// 最長マッチ
// TODO: pairでの実装の方がいいのか、意見聞きたいです。 kohkubo
// TODO: マッチしないパターンがどうなるのか、検証必要 kohkubo
static Location select_proper_location(const std::string           &request_uri,
                                       const std::vector<Location> &locations) {
  // clang-format off
  std::string path;
  // clang-format on
  std::vector<Location>::const_iterator it = locations.begin();
  for (; it != locations.end(); ++it) {
    if (request_uri.find(it->location_path_) == 0) {
      if (path.size() < it->location_path_.size()) {
        path = it->location_path_;
        return *it;
      }
    }
  }
  LOG("########################");
  LOG("location is null");
  LOG("########################");
  throw RequestInfo::BadRequestException(NOT_FOUND_404);
}

static std::string create_file_path(const std::string &request_target,
                                    const Location    &location) {
  std::string file_path = location.root_ + request_target;
  LOG("create_file_path: " << file_path);
  if (has_suffix(file_path, "/") &&
      is_file_exists(file_path + location.index_)) {
    file_path += location.index_;
  }
  return file_path;
}

// 一つのリクエストのパースを行う、bufferに一つ以上のリクエストが含まれるときtrueを返す。
RequestState Request::handle_request(std::string     &request_buffer,
                                     const confGroup &conf_group) {
  try {
    // TODO: そもそもstartlineは最初の一行なので、ループ処理する必要がない
    // kohkubo
    if (_state_ == RECEIVING_STARTLINE || _state_ == RECEIVING_HEADER) {
      std::string line;
      while (getline(request_buffer, line)) { // noexcept
        if (_state_ == RECEIVING_STARTLINE) {
          _request_info_.check_first_multi_blank_line(line);
          // throws BadRequestException
          if (_request_info_.is_blank_first_line_) {
            continue;
          }
          RequestInfo::check_bad_parse_request_start_line(line);
          // throws BadRequestException
          _request_info_.parse_request_start_line(line); // noexcept
          _state_ = RECEIVING_HEADER;
        } else if (_state_ == RECEIVING_HEADER) {
          if (line != "") {
            RequestInfo::store_request_header_field_map(line, _field_map_);
            // throws BadRequestException
            continue;
          }
          _request_info_.parse_request_header(_field_map_);
          // throws BadRequestException
          // TODO: 1関数に切り出し予定です。 kohkubo
          _request_info_.config_ =
              *select_proper_config(conf_group, _request_info_.host_);
          _request_info_.location_ =
              select_proper_location(_request_info_.request_target_,
                                     _request_info_.config_.locations_);
          _request_info_.file_path_ = create_file_path(
              _request_info_.request_target_, _request_info_.location_);
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
    }
    if (_state_ == RECEIVING_BODY) {
      if (_request_info_.is_chunked_) {
        _state_ = _chunk_loop(request_buffer);
        // throws BadRequestException
        _check_max_client_body_size_exception(
            _request_body_.size(),
            _request_info_.config_.client_max_body_size_);
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
    }
    if (_state_ == SUCCESS) {
      _response_ = ResponseGenerator::generate_response(_request_info_);
    } else if (_state_ == RECEIVING_STARTLINE || _state_ == RECEIVING_HEADER) {
      _check_buffer_length_exception(request_buffer, BUFFER_MAX_LENGTH_);
    }
  } catch (const RequestInfo::BadRequestException &e) {
    _response_ =
        ResponseGenerator::generate_error_response(_request_info_, e.status());
    _request_info_.connection_close_ = true;
    _state_                          = SUCCESS;
  }
  return _state_;
}

static bool get_next_chunk_line(NextChunkType chunk_type,
                                std::string &request_buffer, std::string &line,
                                size_t next_chunk_size) {
  if (chunk_type == CHUNK_SIZE) {
    return getline(request_buffer, line);
  }
  if (request_buffer.size() < next_chunk_size + CRLF.size()) {
    return false;
  }
  line = request_buffer.substr(0, next_chunk_size);
  request_buffer.erase(0, next_chunk_size);
  if (!has_prefix(request_buffer, CRLF)) {
    throw RequestInfo::BadRequestException();
  }
  request_buffer.erase(0, CRLF.size());
  return true;
}

RequestState Request::_chunk_loop(std::string &request_buffer) {
  std::string line;
  while (get_next_chunk_line(_next_chunk_, request_buffer, line,
                             _next_chunk_size_)) {
    if (_next_chunk_ == CHUNK_SIZE) {
      // TODO: validate line
      _next_chunk_size_ = hexstr_to_size(line);
      _next_chunk_      = CHUNK_DATA;
    } else {
      bool is_last_chunk = _next_chunk_size_ == 0 && line == "";
      if (is_last_chunk) {
        return SUCCESS;
      }
      _request_body_.append(line);
      _next_chunk_ = CHUNK_SIZE;
    }
  }
  if (_next_chunk_ == CHUNK_SIZE) {
    _check_buffer_length_exception(request_buffer, BUFFER_MAX_LENGTH_);
  }
  return RECEIVING_BODY;
}

void Request::_check_max_client_body_size_exception(
    std::size_t actual_body_size, std::size_t max_body_size) {
  if (actual_body_size > max_body_size) {
    LOG("########################");
    LOG("max_client_body_size exceeded");
    LOG("########################");
    throw RequestInfo::BadRequestException(ENTITY_TOO_LARGE_413);
  }
}

void Request::_check_buffer_length_exception(std::string &request_buffer,
                                             std::size_t  buffer_max_length) {
  if (request_buffer.size() >= buffer_max_length) {
    request_buffer.clear();
    LOG("########################");
    LOG("buffer_max_length exceeded");
    LOG("########################");
    throw RequestInfo::BadRequestException();
  }
}