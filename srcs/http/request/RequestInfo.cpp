#include "http/request/RequestInfo.hpp"

#include "http/HttpStatusCode.hpp"

RequestInfo::BadRequestException::BadRequestException(HttpStatusCode     status,
                                                      const std::string &msg)
    : logic_error(msg)
    , _status_(status) {}

HttpStatusCode RequestInfo::BadRequestException::status() const {
  return _status_;
}

bool RequestInfo::is_valid_request_header() const {
  if (has_content_length_ && is_chunked()) {
    // LOG("request contains both of Content-Length and Chunked-Encoding.");
    return false;
  }
  // TODO: has Transfer-Encoding but last encoding is not chunked. -> 400
  // そもそもchunked以外のTransfer-Encoding実装する？
  if ((request_line_.method_ == "GET" || request_line_.method_ == "DELETE") &&
      has_body()) {
    // LOG("GET or DELETE request has body.");
    return false;
  }
  if (request_line_.method_ == "POST" && !has_body()) {
    // LOG("POST request does not have body.");
    return false;
  }

  return true;
}
