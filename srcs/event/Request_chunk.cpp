#include "event/Request.hpp"

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