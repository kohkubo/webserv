#include "event/Request.hpp"

#include <sys/socket.h>

#include "http/request/RequestInfo.hpp"
#include "utils/utils.hpp"

static std::string cutout_request_body(std::string &request_buffer,
                                       size_t       content_length) {
  std::string request_body = request_buffer.substr(0, content_length);
  request_buffer.erase(0, content_length);
  return request_body;
}

Request::RequestState
Request::_handle_request_line(std::string &request_buffer) {
  std::string line;
  if (getline(request_buffer, line)) {
    _request_info_.parse_request_line(line);
    return RECEIVING_HEADER;
  }
  return _state_;
}

Request::RequestState
Request::_handle_request_header(std::string &request_buffer) {
  std::string line;
  while (getline(request_buffer, line)) {
    if (_state_ == RECEIVING_HEADER) {
      if (line != "") {
        bool result = _header_field_map_.store_new_field(line);
        if (!result) {
          ERROR_LOG("invalid header field");
          throw RequestInfo::BadRequestException();
        }
        continue;
      }
      _request_info_.parse_request_header(_header_field_map_);
      if (!_request_info_.is_valid_request_header()) {
        ERROR_LOG("invalid request header");
        throw RequestInfo::BadRequestException();
      }
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

Request::RequestState
Request::_handle_request_body(std::string &request_buffer) {
  if (_request_info_.is_chunked()) {
    _state_ = _chunk_loop(request_buffer);
    _check_max_client_body_size_exception(
        _request_info_.content_info_.content_.size(),
        _request_info_.config_.client_max_body_size_);
  } else if (request_buffer.size() >=
             static_cast<std::size_t>(
                 _request_info_.content_info_.content_length_)) {
    _request_info_.content_info_.content_ = cutout_request_body(
        request_buffer, _request_info_.content_info_.content_length_);
    _state_ = SUCCESS;
  }
  return _state_;
}

Request::RequestState
Request::handle_request(std::string               &request_buffer,
                        const config::ConfigGroup &config_group) {
  if (_state_ == Request::RECEIVING_STARTLINE) {
    _state_ = _handle_request_line(request_buffer);
  }
  if (_state_ == Request::RECEIVING_HEADER) {
    _state_ = _handle_request_header(request_buffer);
    if (_state_ == RECEIVING_BODY || _state_ == SUCCESS) {
      _request_info_.config_ = config_group.select_config(_request_info_.host_);
      if (_request_info_.content_info_.has_content_length()) {
        _check_max_client_body_size_exception(
            _request_info_.content_info_.content_length_,
            _request_info_.config_.client_max_body_size_);
      }
    }
  }
  if (_state_ == Request::RECEIVING_BODY) {
    _state_ = _handle_request_body(request_buffer);
  }
  _check_buffer_length_exception(request_buffer);
  return _state_;
}
