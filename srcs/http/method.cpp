#include "http.hpp"
#include "util.hpp"

/*
 * mapへの挿入時keyが被っている時の処理は現状考慮してない.
 */
http_message_map http_read_method_get(http_message_map &request_message) {
  std::string      target_url = request_message[URL];
  http_message_map response_message;

  if (is_file_exists(target_url.c_str())) {
    response_message[STATUS] = STATUS_OK;
    response_message[PHRASE] = PHRASE_STATUS_OK;
  } else {
    target_url               = NOT_FOUND_PAGE;
    response_message[STATUS] = STATUS_NOTFOUND;
    response_message[PHRASE] = PHRASE_STATUS_NOTFOUND;
  }
  std::string content            = read_file_tostring(target_url.c_str());
  response_message[BODY]         = content;
  response_message[CONTENT_LEN]  = to_string(content.size());
  response_message[CONTENT_TYPE] = TEXT_HTML;
  return response_message;
}
