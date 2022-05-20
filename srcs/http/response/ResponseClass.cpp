#include "http/response/ResponseClass.hpp"

#include <unistd.h>

#include <iostream>

#include "config/Config.hpp"
#include "http/const/const_html_filename.hpp"
#include "http/const/const_response_key_map.hpp"
#include "http/const/const_status_phrase.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/tokenize.hpp"
#include "utils/utils.hpp"

// mapを宣言時に定義できるのはC++11以降のため、初期化関数を実装しています。
std::map<int, std::string> g_error_page_map;
std::map<int, std::string> g_response_status_phrase_map;

// clang-format off
static void init_response_status_maps() {
  g_error_page_map[200]             = "";
  g_response_status_phrase_map[200] = STATUS_200_PHRASE;
  g_error_page_map[204]             = "";
  g_response_status_phrase_map[204] = STATUS_204_PHRASE;
  g_error_page_map[400]             = BAD_REQUEST_PAGE;
  g_response_status_phrase_map[400] = STATUS_400_PHRASE;
  g_error_page_map[403]             = FORBIDDEN_PAGE;
  g_response_status_phrase_map[403] = STATUS_403_PHRASE;
  g_error_page_map[404]             = NOT_FOUND_PAGE;
  g_response_status_phrase_map[404] = STATUS_404_PHRASE;
  g_error_page_map[500]             = INTERNAL_SERVER_ERROR_PAGE;
  g_response_status_phrase_map[500] = STATUS_500_PHRASE;
  g_error_page_map[501]             = NOT_IMPLEMENTED_PAGE;
  g_response_status_phrase_map[501] = STATUS_501_PHRASE;
  g_error_page_map[520]             = UNKNOWN_ERROR_PAGE;
  g_response_status_phrase_map[520] = STATUS_520_PHRASE;
}
// clang-format on

ResponseClass::ResponseClass(const Config      &config,
                             const RequestInfo &request_info)
    : __config_(config)
    , __request_info_(request_info)
    , __status_code_(0) {
  version_    = VERSION_HTTP;
  connection_ = CONNECTION_CLOSE; // TODO: 別関数に実装
  init_response_status_maps();
  __resolve_url();
  __check_status();
  switch (__request_info_.method_) {
  case GET:
    __get_method_handler();
    break;
  case DELETE:
    __delete_method_handler();
    break;
  default:
    std::cout << "unsupported method" << std::endl;
    __status_code_ = NOT_IMPLEMENTED_501;
    break;
  }
  if (__status_code_ != 200) {
    __file_path_ = g_error_page_map.at(__status_code_);
  }
  status_phrase_ = g_response_status_phrase_map.at(__status_code_);
  __set_body();
}

void ResponseClass::__resolve_url() {
  if (__is_minus_depth()) {
    __status_code_ = NOT_FOUND_404;
    return;
  }
  // TODO: rootの末尾に/入ってるとき
  if (__request_info_.target_ == "/") {
    __file_path_ = __config_.root_ + "/" + __config_.index_;
  } else {
    __file_path_ = __config_.root_ + "/" + __request_info_.target_;
  }
}

bool ResponseClass::__is_minus_depth() {
  tokenVector   tokens = tokenize(__request_info_.target_, "/", "/");
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

void ResponseClass::__check_status() {
  if (__status_code_) {
    return;
  }
  if (!is_file_exists(__file_path_)) {
    __status_code_ = NOT_FOUND_404;
    return;
  }
  if (!is_accessible(__file_path_, R_OK)) {
    __status_code_ =
        FORBIDDEN_403; // TODO: Permission error が 403なのか確かめてない
    return;
  }
  __status_code_ = OK_200;
}

void ResponseClass::__set_error_page_file_path() {
  switch (__config_.error_pages_.count(__status_code_)) {
  case 0:
    __file_path_ = g_error_page_map[__status_code_];
    break;
  case 1:
    // TODO:現状エラーページがフルパスでしか対応していない
    __file_path_ = __config_.error_pages_.at(__status_code_);
    break;
  default:
    std::cout << "Error: status code is duplicated." << std::endl;
    exit(1);
  }
}

void ResponseClass::__set_body() {
  if (is_match_suffix_string(__file_path_, ".sh")) {
    body_ = __read_file_tostring_cgi(__file_path_);
  }
  body_ = read_file_tostring(__file_path_);
}