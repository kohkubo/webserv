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

std::string RequestInfo::__trim_optional_whitespace(std::string str) {
  str.erase(0, str.find_first_not_of(" \t"));
  std::size_t pos = str.find_last_not_of(" \t");
  if (pos != std::string::npos) {
    str.erase(pos + 1);
  }
  return str;
}
