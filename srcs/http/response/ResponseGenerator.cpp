#include "http/response/ResponseGenerator.hpp"

#include <cstdlib>
#include <unistd.h>

#include <iostream>
#include <map>
#include <stdexcept>

#include "config/Config.hpp"
#include "http/const/const_error_contents.hpp"
#include "http/const/const_header_field_values.hpp"
#include "http/const/const_http_enums.hpp"
#include "http/const/const_status_phrase.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/tokenize.hpp"
#include "utils/utils.hpp"

std::map<int, std::string> g_error_page_contents_map = init_page_contents_map();

std::map<int, std::string> init_page_contents_map() {
  std::map<int, std::string> res;
  res[400] = content_400;
  res[403] = content_403;
  res[404] = content_404;
  res[405] = content_405;
  res[500] = content_500;
  res[501] = content_501;
  res[520] = content_520;
  return res;
}

bool ResponseGenerator::__is_error_status_code(HttpStatusCode status_code) {
  // TODO: エラーのステータスコードの扱いを決まったら再実装
  return status_code > 299 && status_code < 600;
}

std::string
ResponseGenerator::generate_response(const Config      &config,
                                     const RequestInfo &request_info) {
  HttpStatusCode  status_code = NONE;
  std::string     body;
  // TODO: 例外処理をここに挟むかも 2022/05/22 16:21 kohkubo nakamoto 話し合い
  // エラーがあった場合、それ以降の処理が不要なので、例外処理でその都度投げる??
  // TODO:locationを決定する処理をResponseの前に挟むと、
  // Responseクラスがconst参照としてLocationを持つことができるがどうだろう。kohkubo
  const Location *location =
      __select_proper_location(request_info.uri_, config.locations_);
  if (location == NULL) {
    // TODO: ここ処理どうするかまとまってないのでとりあえずの処理
    status_code = NOT_FOUND_404;
    body        = __error_page_body(Location(), config, status_code);
    return __response_message(status_code, body);
  }
  if (is_minus_depth(request_info.uri_)) {
    status_code = FORBIDDEN_403;
    body        = __error_page_body(*location, config, status_code);
    return __response_message(status_code, body);
  }
  std::string file_path = __file_path(request_info.uri_, *location);
  if ("GET" == request_info.method_) {
    status_code = __handle_get_method(*location, file_path);
  } else if ("POST" == request_info.method_) {
    status_code = __handle_post_method(*location, file_path);
  } else if ("DELETE" == request_info.method_) {
    status_code = __handle_delete_method(*location, request_info, file_path);
  } else {
    LOG("unknown method: " << request_info.method_);
    status_code = NOT_IMPLEMENTED_501;
  }
  if (__is_error_status_code(status_code)) {
    // TODO: locationの渡し方は全体の処理の流れが決まるまで保留 kohkubo
    body = __error_page_body(*location, config, status_code);
  } else {
    body = __body(file_path, request_info);
  }
  return __response_message(status_code, body);
}

// 最長マッチ
const Location *ResponseGenerator::__select_proper_location(
    const std::string &request_uri, const std::vector<Location> &locations) {
  // clang-format off
  std::string     path;
  const Location *ret_location = NULL;
  // clang-format on
  std::vector<Location>::const_iterator it = locations.begin();
  for (; it != locations.end(); ++it) {
    if (request_uri.find(it->location_path_) == 0) {
      if (path.size() < it->location_path_.size()) {
        path         = it->location_path_;
        ret_location = &(*it);
      }
    }
  }
  return ret_location;
}

std::string ResponseGenerator::__file_path(const std::string &request_uri,
                                           const Location    &location) {
  std::string file_path;
  file_path = location.root_ + request_uri;
  if (has_suffix(file_path, "/") &&
      is_file_exists(file_path + location.index_)) {
    file_path += location.index_;
  }
  return file_path;
}

// TODO: リンクやその他のファイルシステムの時どうするか
HttpStatusCode
ResponseGenerator::__check_filepath_status(const Location    &location,
                                           const std::string &file_path) {
  if (has_suffix(file_path, "/")) {
    if (is_dir_exists(file_path)) {
      if (!location.autoindex_) {
        return FORBIDDEN_403; // nginxに合わせた
      }
      return OK_200;
    }
    return NOT_FOUND_404;
  }
  if (!is_file_exists(file_path)) {
    return NOT_FOUND_404;
  }
  if (!is_accessible(file_path, R_OK)) {
    // TODO: Permission error が 403なのか確かめてない
    return FORBIDDEN_403;
  }
  return OK_200;
}

// TODO: config.error_page validate
std::string
ResponseGenerator::__error_page_body(const Location      &location,
                                     const Config        &config,
                                     const HttpStatusCode status_code) {
  std::map<int, std::string>::const_iterator it =
      config.error_pages_.find(status_code);
  if (it != config.error_pages_.end()) {
    std::string file_path = location.root_ + it->second;
    return read_file_tostring(file_path);
  }
  return g_error_page_contents_map[status_code];
}

std::string ResponseGenerator::__body(const std::string &file_path,
                                      const RequestInfo  request_info) {
  if (has_suffix(file_path, ".sh")) {
    return __read_file_tostring_cgi(file_path, request_info.env_values_);
  }
  if (has_suffix(file_path, "/")) {
    return __create_autoindex_body(file_path, request_info);
  }
  return read_file_tostring(file_path);
}