#include "config/Location.hpp"
#include "http/response/ResponseGenerator.hpp"

std::map<int, std::string> g_response_status_phrase_map =
    init_response_status_phrase_map();

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

std::string ResponseGenerator::__response_message(HttpStatusCode status_code,
                                                  const std::string &body,
                                                  const Location    &location) {
  std::string response;
  bool        has_body =
      status_code != NO_CONTENT_204 || status_code == NOT_MODIFIED_304;

  // start line
  response = "HTTP/1.1" + SP + g_response_status_phrase_map[status_code] + CRLF;

  if (has_body) {
    // entity_header
    response += "Content-Length: " + to_string(body.size()) + CRLF;
    response += "Content-Type: text/html" + CRLF;
  }

  // general_header
  response += "Connection: close" + CRLF;
  if (MOVED_PERMANENTLY_301 == status_code) {
    std::map<int, std::string>::const_iterator it =
        location.return_.find(status_code);
    response += "Location: " + it->second + CRLF;
  }

  // empty line
  response += CRLF;

  if (has_body) {
    // body
    response += body;
  }
  return response;
};
