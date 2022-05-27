#include "http/response/Response.hpp"

#include <cstdlib>
#include <unistd.h>

#include <iostream>

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
  __resolve_uri();
  switch (__request_info_.method_) {
  // TODO:
  // methodの前処理をどこまで共通化するのか。一旦個別に実装して、最後リファクタで考えるのがよい。
  // 2018/05/22 16:21 kohkubo nakamoto 話し合い
  // 現状はmethod handlerに
  case GET:
    __get_method_handler();
    break;
  case POST:
    __post_method_handler();
    break;
  case DELETE:
    __delete_method_handler();
    break;
  default:
    std::cout << "unsupported method" << std::endl;
    __status_code_ = NOT_IMPLEMENTED_501;
    break;
  }
  if (__is_error_status_code()) {
    __set_error_page_body();
  } else {
    __set_body();
  }
}

void Response::__resolve_uri() {
  // TODO: rootの末尾に/入ってるとき
  if (__request_info_.uri_ == "/") {
    __file_path_ =
        __config_.locations_[0].root_ + "/" + __config_.locations_[0].index_;
  } else {
    __file_path_ = __config_.locations_[0].root_ + __request_info_.uri_;
  }
}

bool Response::__is_minus_depth() {
  tokenVector   tokens = tokenize(__request_info_.uri_, "/", "/");
  tokenIterator it     = tokens.begin();

  for (long depth = 0; it != tokens.end(); it++) {
    if (*it == "..") {
      depth--;
    } else if (*it != ".") {
      depth++;
    }
    if (depth < 0) {
      return true;
    }
  }
  return false;
}

void Response::__check_filepath_status() {
  if (__status_code_ != NONE) {
    return;
  }
  if (__is_minus_depth()) {
    __status_code_ = NOT_FOUND_404;
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

void Response::__set_error_page_body() {
  // TODO: locationの扱いどうする?
  std::map<int, std::string>::const_iterator it =
      __config_.error_pages_.find(__status_code_);
  if (it != __config_.error_pages_.end()) {
    __file_path_ = __config_.locations_[0].root_ + "/" + it->second;
    __body_      = read_file_tostring(__file_path_);
  } else {
    __body_ = g_error_page_contents_map[__status_code_];
  }
}

void Response::__set_body() {
  if (has_suffix(__file_path_, ".sh")) {
    __body_ = __read_file_tostring_cgi(__file_path_, __request_info_.values_);
  } else {
    __body_ = read_file_tostring(__file_path_);
  }
}