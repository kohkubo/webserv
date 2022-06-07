#include "http/response/Response.hpp"

std::map<int, std::string> g_response_status_phrase_map =
    init_response_status_phrase_map();

std::map<int, std::string> init_response_status_phrase_map() {
  std::map<int, std::string> res;
  res[200] = STATUS_200_PHRASE;
  res[204] = STATUS_204_PHRASE;
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

const std::string &Response::__get_status_phrase(HttpStatusCode status_code) {
  return g_response_status_phrase_map[status_code];
}

std::string Response::__get_response_string(HttpStatusCode     status_code,
                                            const std::string &body) {
  std::string response;
  bool        has_body =
      status_code != NO_CONTENT_204 || status_code == NOT_MODIFIED_304;

  // start line
  response = VERSION_HTTP + SP + __get_status_phrase(status_code) + CRLF;

  if (has_body) {
    // entity_header
    response += "Content-Length: " + to_string(body.size()) + CRLF;
    response += "Content-Type: " + TEXT_HTML + CRLF;
  }

  // general_header
  response += "Connection: " + CONNECTION_CLOSE + CRLF;

  // empty line
  response += CRLF;

  if (has_body) {
    // body
    response += body;
  }
  return response;
};
