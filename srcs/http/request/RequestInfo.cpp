#include "http/request/RequestInfo.hpp"

#include "http/HttpStatusCode.hpp"
#include "utils/utils.hpp"

RequestInfo::BadRequestException::BadRequestException(HttpStatusCode     status,
                                                      const std::string &msg)
    : logic_error(msg)
    , _status_(status) {}

HttpStatusCode RequestInfo::BadRequestException::status() const {
  return _status_;
}

bool RequestInfo::is_valid_request_header() const {
  if (content_info_.has_content_length() && is_chunked()) {
    ERROR_LOG("request contains both of Content-Length and Chunked-Encoding.");
    return false;
  }
  if (transfer_encoding_.size() != 0 &&
      transfer_encoding_.back() != "chunked") {
    ERROR_LOG("Transfer-Encoding's last element is not chunked.");
    return false;
  }
  if ((request_line_.method_ == "GET" || request_line_.method_ == "DELETE") &&
      has_body()) {
    ERROR_LOG("GET or DELETE request has body.");
    return false;
  }
  if (request_line_.method_ == "POST" && !has_body()) {
    ERROR_LOG("POST request does not have body.");
    return false;
  }
  return true;
}
