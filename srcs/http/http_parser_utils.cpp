#include "http/http_parser_utils.hpp"

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

void set_status_and_path(http_message_map &response_info, const Config &config,
                         HttpStatusCode code) {
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
  (void)config;

  response_info[STATUS_PHRASE] = error_response_status[code];
  if (!(code == OK_200 || code == NO_CONTENT_204))
    response_info[PATH] = default_error_page_path[code];
}

static std::string response_body_content(const std::string &path) {
  if (is_match_suffix_string(path, ".sh")) {
    return read_file_tostring_cgi(path);
  }
  return read_file_tostring(path);
}

void set_response_body(http_message_map &response_info) {
  if (response_info[STATUS_PHRASE] == STATUS_204_PHRASE ||
      response_info[STATUS_PHRASE] == STATUS_304_PHRASE) {
    return;
  }
  response_info[BODY]         = response_body_content(response_info[PATH]);
  response_info[CONTENT_LEN]  = to_string(response_info[BODY].size());
  response_info[CONTENT_TYPE] = TEXT_HTML;
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
