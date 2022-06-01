#include "http/request/RequestInfo.hpp"

RequestInfo::BadRequestException::BadRequestException(const std::string &msg)
    : logic_error(msg) {}

// TODO: フィールド追加
bool RequestInfo::__is_comma_sparated(std::string &field_name) {
  (void)field_name;
  // カンマ区切りが定義されたフィールドか判定する。
  // tmp
  return false;
}
