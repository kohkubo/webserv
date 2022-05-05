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

void set_response_body(http_message_map &response_message) {
  if (response_message[STATUS_PHRASE] == STATUS_204_PHRASE ||
      response_message[STATUS_PHRASE] == STATUS_304_PHRASE) {
    return;
  }
  std::string content            = read_file_tostring(response_message[PATH]);
  response_message[BODY]         = content;
  response_message[CONTENT_LEN]  = to_string(content.size());
  response_message[CONTENT_TYPE] = TEXT_HTML;
}
