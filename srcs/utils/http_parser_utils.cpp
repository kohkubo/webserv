#include "utils/http_parser_utils.hpp"
#include "http/const/const_html_filename.hpp"
#include "http/const/const_status_phrase.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/utils.hpp"

// TODO: location
std::string resolve_url(const ServerConfig &server_config,
                        const std::string  &request_url) {
  if (request_url == "/") {
    return server_config.root_ + "/" + server_config.index_;
  } else {
    return server_config.root_ + "/" + request_url;
  }
}

void set_status_and_path(http_message_map   &response_info,
                         const ServerConfig &server_config,
                         HttpStatusCode      code) {
  const std::string error_response_status[] = {
      STATUS_200_PHRASE, STATUS_204_PHRASE, STATUS_400_PHRASE,
      STATUS_403_PHRASE, STATUS_404_PHRASE, STATUS_500_PHRASE,
      STATUS_501_PHRASE, STATUS_520_PHRASE,
  };
  const std::string default_error_page_path[] = {
      "",                         // 200
      "",                         // 204
      BAD_REQUEST_PAGE,           // 400
      FORBIDDEN_PAGE,             // 403
      NOT_FOUND_PAGE,             // 404
      INTERNAL_SERVER_ERROR_PAGE, // 500
      NOT_IMPLEMENTED_PAGE,       // 501
      UNKNOWN_ERROR_PAGE,         // 520
  };

  // TODO: serverConfigのerror_pageみる
  (void)server_config;

  response_info[STATUS_PHRASE] = error_response_status[code];
  if (!(code == OK_200 || code == NO_CONTENT_204))
    response_info[PATH] = default_error_page_path[code];
}

void set_response_body(http_message_map &response_info) {
  if (response_info[STATUS_PHRASE] == STATUS_204_PHRASE ||
      response_info[STATUS_PHRASE] == STATUS_304_PHRASE) {
    return;
  }
  std::string content         = read_file_tostring(response_info[PATH]);
  response_info[BODY]         = content;
  response_info[CONTENT_LEN]  = to_string(content.size());
  response_info[CONTENT_TYPE] = TEXT_HTML;
}
