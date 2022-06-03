#include "http/request/RequestInfo.hpp"

#include "http/const/const_http_enums.hpp"

RequestInfo::BadRequestException::BadRequestException(HttpStatusCode     status,
                                                      const std::string &msg)
    : logic_error(msg)
    , __status_(status) {}

HttpStatusCode RequestInfo::BadRequestException::status() const {
  return __status_;
}

// TODO: フィールド追加
bool RequestInfo::__is_comma_sparated(std::string &field_name) {
  (void)field_name;
  // カンマ区切りが定義されたフィールドか判定する。
  // tmp
  return false;
}
