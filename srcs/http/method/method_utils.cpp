#include "http/method/method_utils.hpp"

#include "config/Config.hpp"
#include "http/const/const_html_filename.hpp"
#include "http/const/const_response_key_map.hpp"
#include "http/const/const_status_phrase.hpp"
#include "http/request/RequestInfo.hpp"
#include "http/response/response.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/utils.hpp"

// TODO: location
std::string resolve_url(const Config &config, const std::string &request_url) {
  // TODO: rootの末尾に/入ってるとき
  if (request_url == "/") {
    return config.root_ + "/" + config.index_;
  } else {
    return config.root_ + "/" + request_url;
  }
}

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

void set_response_status_and_file_path(http_message_map &response_info,
                                       const Config     &config,
                                       HttpStatusCode    status_code) {
  init_response_status_maps();
  response_info[STATUS_PHRASE] = g_response_status_phrase_map[status_code];
  if (status_code == OK_200 || status_code == NO_CONTENT_204) {
    return;
  }
  switch (config.error_pages_.count(status_code)) {
  case 0:
    response_info[PATH] = g_error_page_map[status_code];
    break;
  case 1:
    // TODO:現状エラーページがフルパスでしか対応していない
    response_info[PATH] = config.error_pages_.at(status_code);
    break;
  default:
    std::cout << "Error: status code is duplicated." << std::endl;
    exit(1);
  }
  return;
}

bool is_minus_depth(std::string url) {
  tokenVector   tokens = tokenize(url, "/", "/");
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
