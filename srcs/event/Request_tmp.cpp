#include "event/Request.hpp"

#include "utils/file_io_utils.hpp"

void Request::_tmp(const ConfigGroup &config_group) {
  // TODO: 例外の処理を整理したあと、1関数に切り出し予定です。 kohkubo
  _request_info_.config_   = config_group.select_config(_request_info_.host_);
  const Location *location = _request_info_.config_.locations_.select_location(
      _request_info_.request_line_.absolute_path_);
  _request_info_.location_ = location;
}
