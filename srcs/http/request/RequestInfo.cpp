#include "http/request/RequestInfo.hpp"

#include "http/const/const_http_enums.hpp"

RequestInfo::BadRequestException::BadRequestException(HttpStatusCode     status,
                                                      const std::string &msg)
    : logic_error(msg)
    , __status_(status) {}

HttpStatusCode RequestInfo::BadRequestException::status() const {
  return __status_;
}
