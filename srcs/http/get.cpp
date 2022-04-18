#include "http/method.hpp"
#include "util.hpp"

/*
 * mapへの挿入時keyが被っている時の処理は現状考慮してない.
 */
void get(message_type &request, message_type &response) {
  std::string target_url = request[URL];
  if (is_file_exists(target_url.c_str())) {
    response[STATUS] = STATUS_OK;
    response[PHRASE] = PHRASE_STATUS_OK;
  } else {
    target_url       = NOT_FOUND_PAGE;
    response[STATUS] = STATUS_NOTFOUND;
    response[PHRASE] = PHRASE_STATUS_NOTFOUND;
  }
  std::string content    = read_file_tostring(target_url.c_str());
  response[BODY]         = content;
  response[CONTENT_LEN]  = to_string(content.size());
  response[CONTENT_TYPE] = TEXT_HTML;
}
