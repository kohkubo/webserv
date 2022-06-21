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

static std::string create_file_path(const std::string &request_target,
                                    const Location    &location) {
  std::string file_path = location.root_ + request_target;
  LOG("create_file_path: " << file_path);
  if (has_suffix(file_path, "/") &&
      is_file_exists(file_path + location.index_)) {
    file_path += location.index_;
  }
  return file_path;
}

void Request::_tmp(const confGroup &conf_group) {
  // TODO: 例外の処理を整理したあと、1関数に切り出し予定です。 kohkubo
  _request_info_.config_ =
      *select_proper_config(conf_group, _request_info_.host_);
  _request_info_.location_ = select_proper_location(
      _request_info_.request_target_, _request_info_.config_.locations_);
  _request_info_.file_path_ = create_file_path(_request_info_.request_target_,
                                               _request_info_.location_);
}