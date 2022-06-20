#include "http/request/RequestInfo.hpp"

#include "http/const/const_http_enums.hpp"

RequestInfo::BadRequestException::BadRequestException(
    HttpStatusCode::StatusCode status, const std::string &msg)
    : logic_error(msg)
    , _status_(status) {}

HttpStatusCode::StatusCode RequestInfo::BadRequestException::status() const {
  return _status_;
}
