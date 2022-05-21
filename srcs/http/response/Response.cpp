#include "http/response/Response.hpp"

#include <cstdlib>
#include <unistd.h>

#include <iostream>

#include "config/Config.hpp"
#include "http/const/const_error_contents.hpp"
#include "http/const/const_html_filename.hpp"
#include "http/const/const_response_key_map.hpp"
#include "http/const/const_status_phrase.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/tokenize.hpp"
#include "utils/utils.hpp"

std::map<int, std::string> g_response_status_phrase_map =
    init_response_status_phrase_map();
std::map<int, std::string> g_error_page_contents_map = init_page_contents_map();

std::map<int, std::string> init_response_status_phrase_map() {
  std::map<int, std::string> res;
  res[200] = STATUS_200_PHRASE;
  res[204] = STATUS_204_PHRASE;
  res[400] = STATUS_400_PHRASE;
  res[403] = STATUS_403_PHRASE;
  res[404] = STATUS_404_PHRASE;
  res[500] = STATUS_500_PHRASE;
  res[501] = STATUS_501_PHRASE;
  res[520] = STATUS_520_PHRASE;
  return res;
}

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

Response::Response(const Config &config, const RequestInfo &request_info)
    : __config_(config)
    , __request_info_(request_info)
    , __status_code_(0) {
  __version_    = VERSION_HTTP;
  __connection_ = CONNECTION_CLOSE; // TODO: 別関数に実装
  __resolve_uri();
  __check_filepath_status();
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
  __status_phrase_ = g_response_status_phrase_map.at(__status_code_);
  if (__status_code_ != 200) {
    __set_error_page_body();
    return;
  }
  __set_body();
}

void Response::__resolve_uri() {
  if (__is_minus_depth()) {
    __status_code_ = NOT_FOUND_404;
  }
  bool error_code_already_set = __status_code_ != 0;
  if (error_code_already_set) {
    __file_path_ =
        __config_.root_ + "/" + __config_.error_pages_.at(__status_code_);
    return;
  }
  // TODO: rootの末尾に/入ってるとき
  if (__request_info_.uri_ == "/") {
    __file_path_ = __config_.root_ + "/" + __config_.index_;
  } else {
    __file_path_ = __config_.root_ + "/" + __request_info_.uri_;
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

void Response::__set_error_page_body() {
  switch (__config_.error_pages_.count(__status_code_)) {
  case 0:
    __body_ = g_error_page_contents_map[__status_code_];
    break;
  case 1:
    __resolve_uri();
    __set_body();
    break;
  default:
    std::cout << "Error: status code is duplicated." << std::endl;
    exit(1);
  }
}

void Response::__set_body() {
  if (is_match_suffix_string(__file_path_, ".sh")) {
    __body_ = __read_file_tostring_cgi(__file_path_);
  }
  __body_ = read_file_tostring(__file_path_);
}