#include "http.hpp"
#include "util.hpp"

/*
 * mapへの挿入時keyが被っている時の処理は現状考慮してない.
 */
http_message http_read_method_get(http_message &http_request_message) {
  std::string  target_url = http_request_message[URL];
  http_message http_response_message;

  if (is_file_exists(target_url.c_str())) {
    http_response_message[STATUS] = STATUS_OK;
    http_response_message[PHRASE] = PHRASE_STATUS_OK;
  } else {
    target_url                    = NOT_FOUND_PAGE;
    http_response_message[STATUS] = STATUS_NOTFOUND;
    http_response_message[PHRASE] = PHRASE_STATUS_NOTFOUND;
  }
  std::string content                 = read_file_tostring(target_url.c_str());
  http_response_message[BODY]         = content;
  http_response_message[CONTENT_LEN]  = to_string(content.size());
  http_response_message[CONTENT_TYPE] = TEXT_HTML;
  return http_response_message;
}
