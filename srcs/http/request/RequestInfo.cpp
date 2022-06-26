#include "http/request/RequestInfo.hpp"

#include "http/const/const_http_enums.hpp"

RequestInfo::BadRequestException::BadRequestException(
    HttpStatusCode::StatusCode status, const std::string &msg)
    : logic_error(msg)
    , _status_(status) {}

HttpStatusCode::StatusCode RequestInfo::BadRequestException::status() const {
  return _status_;
}

bool RequestInfo::is_valid_request_header() const {
  if (has_content_length_ && is_chunked_) {
    LOG("request contains both of Content-Length and Chunked-Encoding.");
    return false;
  }
  // TODO: has Transfer-Encoding but last encoding is not chunked. -> 400
  // そもそもchunked以外のTransfer-Encoding実装する？
  if ((method_ == "GET" || method_ == "DELETE") && has_body()) {
    LOG("GET or DELETE request has body.");
    return false;
  }
  if (method_ == "POST" && !has_body()) {
    LOG("POST request does not have body.");
    return false;
  }

  return true;
}