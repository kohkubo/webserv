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
bool is_request_error(HttpMessage &request_message) {
  if (request_message.host_ == "") {
    return true;
  }
  return false;
}

static HttpStatusCode check_url(const std::string &target_filepath) {
  if (!is_file_exists(target_filepath)) {
    return NOT_FOUND_404;
  }
  if (!check_access(target_filepath, R_OK)) {
    return FORBIDDEN_403; // TODO: Permission error が 403なのか確かめてない
  }
  return OK_200;
}

/*
 * mapへの挿入時keyが被っている時の処理は現状考慮してない.
 */
http_message_map method_get(const ServerConfig &server_config,
                            HttpMessage        &request_message) {
  http_message_map response_message;
  // リクエストのエラー 判定はここがよさそう。
  // メソッドごとのエラー判定と共通のエラーで分けてもいいかも
  // hostは共通かな -> hostのエラーチェックはloopの段階でvalidateされる
  if (is_request_error(request_message)) {
    std::cout << "request error." << std::endl;
    response_message[STATUS_PHRASE] = STATUS_400_PHRASE;
    return response_message;
  }

  std::string target_filepath =
      resolve_url(server_config, request_message.url_);
  switch (check_url(target_filepath)) {
  case OK_200:
    response_message[STATUS_PHRASE] = STATUS_200_PHRASE;
    break;
  case FORBIDDEN_403:
    response_message[STATUS_PHRASE] = STATUS_403_PHRASE;
    target_filepath                 = FORBIDDEN_PAGE;
    break;
  case NOT_FOUND_404:
    response_message[STATUS_PHRASE] = STATUS_404_PHRASE;
    target_filepath                 = NOT_FOUND_PAGE;
    break;
  default:
    response_message[STATUS_PHRASE] = STATUS_520_PHRASE;
    target_filepath                 = UNKNOWN_ERROR_PAGE;
    break;
  }
  set_response_body(response_message, target_filepath);
  return response_message;
}
