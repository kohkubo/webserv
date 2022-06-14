#include "http/response/ResponseGenerator.hpp"

#include "config/Location.hpp"
#include "http/const/const_error_contents.hpp"
#include "utils/file_io_utils.hpp"

std::map<int, std::string> init_response_status_phrase_map() {
  std::map<int, std::string> res;
  res[200] = STATUS_200_PHRASE;
  res[204] = STATUS_204_PHRASE;
  res[301] = STATUS_301_PHRASE;
  res[304] = STATUS_304_PHRASE;
  res[400] = STATUS_400_PHRASE;
  res[403] = STATUS_403_PHRASE;
  res[404] = STATUS_404_PHRASE;
  res[405] = STATUS_405_PHRASE;
  res[500] = STATUS_500_PHRASE;
  res[501] = STATUS_501_PHRASE;
  res[520] = STATUS_520_PHRASE;
  return res;
}

std::map<int, std::string> g_response_status_phrase_map =
    init_response_status_phrase_map();

std::map<int, std::string> init_page_contents_map() {
  std::map<int, std::string> res;
  res[400] = content_400;
  res[403] = content_403;
  res[404] = content_404;
  res[405] = content_405;
  res[500] = content_500;
  res[501] = content_501;
  res[520] = content_520;
  return res;
}

std::map<int, std::string> g_error_page_contents_map = init_page_contents_map();

// TODO: config.error_page validate
static std::string         error_page_body(const Location      &location,
                                           const Config        &config,
                                           const HttpStatusCode status_code) {
          std::map<int, std::string>::const_iterator it =
              config.error_pages_.find(status_code);
          if (it != config.error_pages_.end()) {
            std::string file_path = location.root_ + it->second;
            return read_file_tostring(file_path);
  }
          return g_error_page_contents_map[status_code];
}

static std::string start_line(const HttpStatusCode status_code) {
  return "HTTP/1.1" + SP + g_response_status_phrase_map[status_code] + CRLF;
}

static std::string general_header() { return "Connection: close" + CRLF; }
static std::string location_header(const Location &location,
                                   HttpStatusCode  status_code) {
  std::map<int, std::string>::const_iterator it =
      location.return_.find(status_code);
  return "Location: " + it->second + CRLF;
}

static std::string entity_header_and_body(const std::string &body) {
  return "Content-Length: " + to_string(body.size()) + CRLF +
         "Content-Type: text/html" + CRLF + CRLF + body;
}

std::string ResponseGenerator::generate_error_response(const Location &location,
                                    const Config   &config,
                                    HttpStatusCode  status_code) {
  std::string response = start_line(status_code);
  response += general_header();
  if (BAD_REQUEST_400 == status_code) {
    return response;
  }
  std::string body = error_page_body(location, config, status_code);
  response += entity_header_and_body(body);
  return response;
}

std::string ResponseGenerator::__response_message(HttpStatusCode status_code,
                                                  const std::string &body,
                                                  const Location    &location) {
  std::string response = start_line(status_code);
  response += general_header();
  if (status_code == NO_CONTENT_204) {
    return response;
  }
  if (MOVED_PERMANENTLY_301 == status_code) {
    response += location_header(location, status_code);
  }
  response += entity_header_and_body(body);
  return response;
};