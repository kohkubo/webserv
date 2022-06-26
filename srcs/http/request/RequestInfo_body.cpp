#include "http/request/RequestInfo.hpp"

#include <cstdlib>
#include <iostream>
#include <vector>

#include "event/Request.hpp"
#include "http/const/const_delimiter.hpp"
#include "utils/utils.hpp"

void RequestInfo::parse_request_body(std::string       &request_body,
                                     const ContentInfo &content_type) {
  if (content_type.type_ == "application/x-www-form-urlencoded" ||
      content_type.type_ == "text/plain" ||
      content_type.type_ == "multipart/form-data") {
    body_ = request_body;
    return;
  }
  ERROR_LOG("unknown content-type:" + content_type.type_);
  throw RequestInfo::BadRequestException(
      HttpStatusCode::NOT_IMPLEMENTED_501); // tmp
}
