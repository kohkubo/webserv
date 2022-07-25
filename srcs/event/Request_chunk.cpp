#include "event/Request.hpp"

#include "http/const/const_delimiter.hpp"

static bool get_next_chunk_line(Request::NextChunkType chunk_type,
                                std::string &request_buffer, std::string &line,
                                size_t next_chunk_size) {
  if (chunk_type == Request::CHUNK_SIZE) {
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

Request::RequestState Request::_chunk_loop(std::string &request_buffer) {
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