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
  while (getline(request_buffer, line)) {
    _request_info_.parse_request_line(line); // noexcept
    return RECEIVING_HEADER;
  }
  return _state_;
}

Request::RequestState
Request::_handle_request_header(std::string &request_buffer) {
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
      if (!_request_info_.is_valid_request_header()) {
        throw RequestInfo::BadRequestException();
      }
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

Request::RequestState
Request::_handle_request_body(std::string &request_buffer) {
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
    _request_info_.body_ = _request_body_;
  }
  return _state_;
}

// 一つのリクエストのパースを行う、bufferに一つ以上のリクエストが含まれるときtrueを返す。
Request::RequestState
Request::handle_request(std::string               &request_buffer,
                        const config::ConfigGroup &config_group) {
  if (_state_ == Request::RECEIVING_STARTLINE) {
    _state_ = _handle_request_line(request_buffer);
    // TODO: この例外チェックの適切な場所を見直す kohkubo
    _check_buffer_length_exception(request_buffer, BUFFER_MAX_LENGTH_);
  }
  if (_state_ == Request::RECEIVING_HEADER) {
    _state_ = _handle_request_header(request_buffer);
    // TODO: この例外チェックの適切な場所を見直す kohkubo
    _check_buffer_length_exception(request_buffer, BUFFER_MAX_LENGTH_);
    _tmp(config_group);
  }
  if (_state_ == Request::RECEIVING_BODY) {
    /*
    TODO: bodyはconfigで指定がない場合、無限に長くて大丈夫。 kohkubo
    RFC 9110
    8.6. Content-Length
    Any Content-Length field value greater than or equal to zero is valid.
    ゼロ以上のContent-Lengthフィールド値はすべて有効です。

    Since there is no predefined limit to the length of content, a recipient
    MUST anticipate potentially large decimal numerals and prevent parsing
    errors due to integer conversion overflows or precision loss due to integer
    conversion (Section 17.5).
    コンテンツの長さに事前定義された制限がないため、受信者は潜在的に大きな10進数を予測し、
    整数変換のオーバーフローによる解析エラーや整数変換による精度の低下を防止する必要があります（セクション17.5）。

    TODO: クライアントのbodyが想定より短い場合は、以下の理由でエラーにして返す
    kohkubo
    RFC 9110
    8.6. Content-Length
    As a result, a sender MUST NOT forward a message with a Content- Length
    header field value that is known to be incorrect.
    その結果、送信者は、正しくないことがわかっているContent-Lengthヘッダーフィールド値を持つメッセージを転送してはなりません（MUSTNOT）。
    */
    _state_ = _handle_request_body(request_buffer);
  }
  return _state_;
}
