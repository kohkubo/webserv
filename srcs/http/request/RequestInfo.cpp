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

void RequestInfo::select_proper_config(const confGroup   &conf_group,
                                       const std::string &host_name) {
  confGroup::const_iterator it = conf_group.begin();
  for (; it != conf_group.end(); it++) {
    if ((*it)->server_name_ == host_name) {
      config_ = *it;
    }
  }
  config_ = conf_group[0];
}