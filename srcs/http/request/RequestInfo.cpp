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
  if (has_content_length_ && is_chunked_) {
    // LOG("request contains both of Content-Length and Chunked-Encoding.");
    return false;
  }
  if (header_field_map_.has_field("transfer-encoding")) {
    const std::string &value = header_field_map_.value("transfer-encoding");
    if (!has_suffix(value, "chunked")) {
      return false;
    }
  }
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
