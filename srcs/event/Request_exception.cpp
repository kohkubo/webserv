#include "event/Request.hpp"

#include "http/request/RequestInfo.hpp"
#include "utils/utils.hpp"

void Request::_check_max_client_body_size_exception(ssize_t actual_body_size,
                                                    std::size_t max_body_size) {
  if (static_cast<std::size_t>(actual_body_size) > max_body_size) {
    ERROR_LOG("max_client_body_size exceeded: " << actual_body_size << " > "
                                                << max_body_size);
    throw RequestInfo::BadRequestException(
        HttpStatusCode::S_413_ENTITY_TOO_LARGE);
  }
}

void Request::_check_buffer_length_exception(std::string &request_buffer) {
  if (request_buffer.size() >= BUFFER_MAX_LENGTH_) {
    request_buffer.clear();
    ERROR_LOG("buffer_max_length exceeded");
    throw RequestInfo::BadRequestException();
  }
}