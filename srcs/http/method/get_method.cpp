#include "http/method/method.hpp"

#include <unistd.h>

#include <string>

#include "config/Config.hpp"
#include "http/const/const_html_filename.hpp"
#include "http/const/const_response_key_map.hpp"
#include "http/const/const_status_phrase.hpp"
#include "http/http_parser_utils.hpp"
#include "http/request/RequestInfo.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/utils.hpp"

static HttpStatusCode check_url(const std::string &target_filepath) {
  if (!is_file_exists(target_filepath)) {
    return NOT_FOUND_404;
  }
  if (!is_accessible(target_filepath, R_OK)) {
    return FORBIDDEN_403; // TODO: Permission error が 403なのか確かめてない
  }
  return OK_200;
}

/*
 * mapへの挿入時keyが被っている時の処理は現状考慮してない.
 */
http_message_map method_get(const Config &config, RequestInfo &request_info) {
  http_message_map response_info;
  std::string      target_filepath = resolve_url(config, request_info.target_);

  if (is_minus_depth(request_info.target_)) {
    set_status_and_path(response_info, config, NOT_FOUND_404);
    return response_info;
  }

  HttpStatusCode code = check_url(target_filepath);
  set_status_and_path(response_info, config, code);
  if (code == OK_200)
    response_info[PATH] = target_filepath;
  return response_info;
}
