#include "event/Request.hpp"

void Request::_check_max_client_body_size_exception(
    std::size_t actual_body_size, std::size_t max_body_size) {
  if (actual_body_size > max_body_size) {
    ERROR_LOG("max_client_body_size exceeded");
    throw RequestInfo::BadRequestException(
        HttpStatusCode::ENTITY_TOO_LARGE_413);
  }
}

void Request::_check_buffer_length_exception(std::string &request_buffer,
                                             std::size_t  buffer_max_length) {
  if (request_buffer.size() >= buffer_max_length) {
    request_buffer.clear();
    ERROR_LOG("buffer_max_length exceeded");
    throw RequestInfo::BadRequestException();
  }
}