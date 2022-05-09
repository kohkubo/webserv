#include "config/ServerConfig.hpp"
#include "http/HttpMessage.hpp"
#include "http/const/const_html_filename.hpp"
#include "http/const/const_response_key_map.hpp"
#include "http/const/const_status_phrase.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/http_parser_utils.hpp"
#include "utils/utils.hpp"
#include <string>
#include <unistd.h>

// TODO: Requestのエラーについて、未調査です。Getにおいては、Hostだけで良さそう
bool is_request_error(HttpMessage &request_info) {
  if (request_info.host_ == "") {
    return true;
  }
  return false;
}

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
http_message_map method_get(const ServerConfig &server_config,
                            HttpMessage        &request_info) {
  http_message_map response_info;
  std::string target_filepath = resolve_url(server_config, request_info.url_);

  // リクエストのエラー 判定はここがよさそう。
  // メソッドごとのエラー判定と共通のエラーで分けてもいいかも
  // hostは共通かな -> hostのエラーチェックはloopの段階でvalidateされる
  if (is_request_error(request_info)) {
    std::cout << "request error." << std::endl;
    response_info[STATUS_PHRASE] = STATUS_400_PHRASE;
    response_info[PATH]          = BAD_REQUEST_PAGE;
    return response_info;
  }
  if (is_minus_depth(request_info.url_)) {
    set_status_and_path(response_info, server_config, NOT_FOUND_404);
    return response_info;
  }

  HttpStatusCode code = check_url(target_filepath);
  set_status_and_path(response_info, server_config, code);
  if (code == OK_200)
    response_info[PATH] = target_filepath;
  return response_info;
}
