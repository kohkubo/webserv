#include "utils/http_parser_utils.hpp"
#include "http/const/const_html_filename.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/utils.hpp"

// TODO: location
std::string resolve_url(const ServerConfig &server_config,
                        const std::string   request_url) {
  if (request_url == "/") {
    return server_config.root_ + "/" + server_config.index_;
  } else {
    return server_config.root_ + "/" + request_url;
  }
}

void set_response_body(http_message_map  &response_message,
                       const std::string &target_filepath) {
  std::string content            = read_file_tostring(target_filepath);
  response_message[BODY]         = content;
  response_message[CONTENT_LEN]  = to_string(content.size());
  response_message[CONTENT_TYPE] = TEXT_HTML;
}

// 引数に渡したステータスコードのenumによってステータスフレーズとbody（必要に応じて）をセットする関数？
// static void set_body_with_phrase(http_message_map  &response_message,
//                                  const std::string &file_path) {
//   if (response_message[STATUS_PHRASE] == STATUS_200_PHRASE) {

//   }
// }
