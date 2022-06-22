#include "event/Request.hpp"

static const Config *select_proper_config(const confGroup   &conf_group,
                                          const std::string &host_name) {
  confGroup::const_iterator it = conf_group.begin();
  for (; it != conf_group.end(); it++) {
    if ((*it)->server_name_ == host_name) {
      return *it;
    }
  }
  return conf_group[0];
}

// 最長マッチ
// TODO: pairでの実装の方がいいのか、意見聞きたいです。 kohkubo
// TODO: マッチしないパターンがどうなるのか、検証必要 kohkubo
static Location select_proper_location(const std::string           &request_uri,
                                       const std::vector<Location> &locations) {
  const Location *location = NULL;
  std::string     path;

  std::vector<Location>::const_iterator it = locations.begin();
  for (; it != locations.end(); ++it) {
    if (request_uri.find(it->location_path_) == 0) {
      if (path.size() < it->location_path_.size()) {
        path     = it->location_path_;
        location = &(*it);
      }
    }
  }
  if (location == NULL) {
    LOG("no match found with locations.");
    throw RequestInfo::BadRequestException(HttpStatusCode::NOT_FOUND_404);
  }
  return *location;
}

// GETの条件分岐をhandle_method()のGETの処理で行うなら,
// request_infoの外にtarget_path_変数を出して編集可能にすべきと考える rakiyama
static std::string create_target_path(const std::string &request_target,
                                      const std::string &request_method,
                                      const Location    &location) {
  std::string target_path = location.root_ + request_target;
  if (request_method == "GET") {
    if (has_suffix(target_path, "/") &&
        is_file_exists(target_path + location.index_)) {
      target_path += location.index_;
    }
  }
  LOG("create_target_path: " << target_path);
  return target_path;
}

void Request::_tmp(const ConfigGroup &config_group) {
  // TODO: 例外の処理を整理したあと、1関数に切り出し予定です。 kohkubo
  _request_info_.config_   = config_group.select_config(_request_info_.host_);
  const Location *location = _request_info_.config_.locations_.select_location(
      _request_info_.request_target_);
  if (location == NULL) {
    throw RequestInfo::BadRequestException(HttpStatusCode::NOT_FOUND_404);
  }
  _request_info_.location_ = *location;
  _request_info_.target_path_ =
      create_target_path(_request_info_.request_target_, _request_info_.method_,
                         _request_info_.location_);
}