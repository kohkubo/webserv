#include "event/RequestHandler.hpp"

#include <sys/socket.h>

#include "http/const/const_delimiter.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/utils.hpp"

static std::string cutout_request_body(std::string &request_buffer,
                                       size_t       content_length) {
  std::string request_body = request_buffer.substr(0, content_length);
  request_buffer.erase(0, content_length);
  return request_body;
}

RequestHandler::RequestState
RequestHandler::_handle_request_line(std::string &request_buffer) {
  std::string line;
  if (getline(request_buffer, line)) {
    _request_info_.parse_request_line(line);
    return RECEIVING_HEADER;
  }
  return _state_;
}

RequestHandler::RequestState
RequestHandler::_handle_request_header(std::string &request_buffer) {
  std::string line;
  while (getline(request_buffer, line)) {
    if (line != "") {
      bool result = _header_field_map_.store_new_field(line);
      if (!result) {
        throw RequestInfo::BadRequestException();
      }
      continue;
    }
    _request_info_.parse_request_header(_header_field_map_);
    if (!_request_info_.is_valid_request_header()) {
      throw RequestInfo::BadRequestException();
    }
    if (_request_info_.has_body()) {
      return RECEIVING_BODY;
    }
    return SUCCESS;
  }
  return _state_;
}

RequestHandler::RequestState
RequestHandler::_handle_request_body(std::string &request_buffer) {
  if (_request_info_.is_chunked()) {
    _state_ = _chunk_loop(request_buffer);
    _check_max_client_body_size_exception(
        _request_info_.content_info_.content_.size(),
        _request_info_.config_->client_max_body_size_);
    return _state_;
  }
  if (request_buffer.size() >=
      static_cast<std::size_t>(_request_info_.content_info_.content_length_)) {
    _request_info_.content_info_.content_ = cutout_request_body(
        request_buffer, _request_info_.content_info_.content_length_);
    return SUCCESS;
  }
  return _state_;
}

RequestHandler::RequestState
RequestHandler::handle_request(std::string               &request_buffer,
                               const config::ConfigGroup &config_group) {
  if (_state_ == RequestHandler::RECEIVING_STARTLINE) {
    _state_ = _handle_request_line(request_buffer);
    _check_buffer_length_exception(request_buffer);
  }
  if (_state_ == RequestHandler::RECEIVING_HEADER) {
    _state_ = _handle_request_header(request_buffer);
    _check_buffer_length_exception(request_buffer);
    if (_state_ == RECEIVING_BODY || _state_ == SUCCESS) {
      _request_info_.config_ = config_group.select_config(_request_info_.host_);
      _request_info_.location_ =
          _request_info_.config_->locations_.select_location(
              _request_info_.request_line_.absolute_path_);
      if (_request_info_.content_info_.has_content_length()) {
        _check_max_client_body_size_exception(
            _request_info_.content_info_.content_length_,
            _request_info_.config_->client_max_body_size_);
      }
    }
  }
  if (_state_ == RequestHandler::RECEIVING_BODY) {
    _state_ = _handle_request_body(request_buffer);
  }
  return _state_;
}

void RequestHandler::_check_max_client_body_size_exception(
    ssize_t actual_body_size, std::size_t max_body_size) {
  if (static_cast<std::size_t>(actual_body_size) > max_body_size) {
    ERROR_LOG("max_client_body_size exceeded: " << actual_body_size << " > "
                                                << max_body_size);
    throw RequestInfo::BadRequestException(
        HttpStatusCode::S_413_ENTITY_TOO_LARGE);
  }
}

void RequestHandler::_check_buffer_length_exception(
    std::string &request_buffer) {
  if (request_buffer.size() >= BUFFER_MAX_LENGTH_) {
    request_buffer.clear();
    ERROR_LOG("buffer_max_length exceeded");
    throw RequestInfo::BadRequestException();
  }
}

static bool get_next_chunk_line(RequestHandler::NextChunkType chunk_type,
                                std::string &request_buffer, std::string &line,
                                size_t next_chunk_size) {
  if (chunk_type == RequestHandler::CHUNK_SIZE) {
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

RequestHandler::RequestState
RequestHandler::_chunk_loop(std::string &request_buffer) {
  std::string line;
  while (get_next_chunk_line(_chunk_info_.next_chunk_type_, request_buffer,
                             line, _chunk_info_.next_chunk_size_)) {
    if (_chunk_info_.next_chunk_type_ == CHUNK_SIZE) {
      Result<std::size_t> result = hexstr_to_size(line);
      if (result.is_err_) {
        throw RequestInfo::BadRequestException();
      }
      _chunk_info_.next_chunk_size_ = result.object_;
      _chunk_info_.next_chunk_type_ = CHUNK_DATA;
    } else {
      bool is_last_chunk = _chunk_info_.next_chunk_size_ == 0 && line == "";
      if (is_last_chunk) {
        return SUCCESS;
      }
      _request_info_.content_info_.content_.append(line);
      _chunk_info_.next_chunk_type_ = CHUNK_SIZE;
    }
  }
  if (_chunk_info_.next_chunk_type_ == CHUNK_SIZE) {
    _check_buffer_length_exception(request_buffer);
  }
  return RECEIVING_BODY;
}