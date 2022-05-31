#include "http/response/Response.hpp"

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

bool Response::__is_error_status_code() {
  HttpStatusCode code = __status_code_;
  // TODO: エラーのステータスコードの扱いを決まったら再実装
  if (code > 299 && code < 600) {
    return true;
  }
  return false;
}

Response::Response(const Config &config, const RequestInfo &request_info)
    : __config_(config)
    , __request_info_(request_info)
    , __status_code_(NONE) {
  // TODO: 例外処理をここに挟むかも 2022/05/22 16:21 kohkubo nakamoto 話し合い
  // エラーがあった場合、それ以降の処理が不要なので、例外処理でその都度投げる??
  // TODO:locationを決定する処理をResponseの前に挟むと、
  // Responseクラスがconst参照としてLocationを持つことができるがどうだろう。kohkubo
  const Location *location =
      __get_proper_location(__request_info_.uri_, __config_.locations_);
  if (location == NULL) {
    // TODO: ここ処理どうするかまとまってないのでとりあえずの処理
    __status_code_ = NOT_FOUND_404;
    __set_error_page_body(Location());
    return;
  }
  __set_file_path(__request_info_.uri_, *location);
  switch (__request_info_.method_) {
  // TODO:
  // methodの前処理をどこまで共通化するのか。一旦個別に実装して、最後リファクタで考えるのがよい。
  // 2018/05/22 16:21 kohkubo nakamoto 話し合い
  // 現状はmethod handlerに
  case GET:
    __get_method_handler(*location);
    break;
  case POST:
    __post_method_handler(*location);
    break;
  case DELETE:
    __delete_method_handler(*location);
    break;
  default:
    LOG("unknown method:" << __request_info_.method_);
    __status_code_ = NOT_IMPLEMENTED_501;
    break;
  }
  if (__is_error_status_code()) {
    // TODO: locationの渡し方は全体の処理の流れが決まるまで保留 kohkubo
    __set_error_page_body(*location);
  } else {
    __set_body();
  }
}

// 最長マッチ
const Location *
Response::__get_proper_location(const std::string           &request_uri,
                                const std::vector<Location> &locations) {
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

void Response::__set_file_path(const std::string &request_uri,
                               const Location    &location) {
  if (has_suffix(request_uri, "/")) {
    __file_path_ = location.root_ + location.location_path_ + location.index_;
  } else {
    __file_path_ = location.root_ + request_uri;
  }
  // 末尾が"/"のものをディレクトリとして扱う, 挙動としてはnginxもそうだと思う
  if (has_suffix(__file_path_, "/") &&
      is_file_exists(__file_path_ + location.index_)) {
    // indexを追記したパスが存在すれば採用, autoindexは無視される
    // 存在しなければディレクトリのままで, 後のautoindexの処理に入る
    __file_path_ += location.index_;
  }
}

void Response::__check_filepath_status(const Location &location) {
  if (__status_code_ != NONE) {
    return;
  }
  if (is_minus_depth(__file_path_)) {
    __status_code_ = NOT_FOUND_404;
  }
  // TODO: POSTはディレクトリの時どう処理するのか
  // TODO: 以下は別関数にするか整理する
  if (has_suffix(__file_path_, "/")) {
    if (is_dir_exists(__file_path_)) {
      if (!location.autoindex_) {
        __status_code_ = FORBIDDEN_403; // nginxに合わせた
      } else {
        __status_code_ = OK_200;
      }
    } else {
      __status_code_ = NOT_FOUND_404;
    }
    return;
  }
  if (!is_file_exists(__file_path_)) {
    __status_code_ = NOT_FOUND_404;
    return;
  }
  if (!is_accessible(__file_path_, R_OK)) {
    // TODO: Permission error が 403なのか確かめてない
    __status_code_ = FORBIDDEN_403;
    return;
  }
  __status_code_ = OK_200;
}

void Response::__set_error_page_body(const Location &location) {
  std::map<int, std::string>::const_iterator it =
      __config_.error_pages_.find(__status_code_);
  if (it != __config_.error_pages_.end()) {
    __file_path_ = location.root_ + it->second;
    __body_      = read_file_tostring(__file_path_);
  } else {
    __body_ = g_error_page_contents_map[__status_code_];
  }
}

void Response::__set_body() {
  if (has_suffix(__file_path_, ".sh")) {
    __body_ = __read_file_tostring_cgi(__file_path_, __request_info_.values_);
  } else if (has_suffix(__file_path_, "/")) {
    __body_ = __create_autoindex_body(__file_path_);
  } else {
    __body_ = read_file_tostring(__file_path_);
  }
}