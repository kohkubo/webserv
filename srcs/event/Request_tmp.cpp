#include "event/Request.hpp"

#include "utils/file_io_utils.hpp"

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
  // LOG("create_target_path: " << target_path);
  return target_path;
}

void Request::_tmp(const ConfigGroup &config_group) {
  // TODO: 例外の処理を整理したあと、1関数に切り出し予定です。 kohkubo
  _request_info_.config_   = config_group.select_config(_request_info_.host_);
  const Location *location = _request_info_.config_.locations_.select_location(
      _request_info_.request_line_.absolute_path_);
  _request_info_.location_ = location;
}
