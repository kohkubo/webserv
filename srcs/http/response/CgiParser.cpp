#include "http/response/CgiParser.hpp"

static std::string detect_newline_delimiter(const std::string &cgi_response) {
  std::size_t crlf_pos = cgi_response.find("\r\n");
  if (crlf_pos != std::string::npos) {
    return "\r\n";
  }
  return "\n";
}

CgiParser::CgiParser(const std::string &cgi_response) {
  _new_line_delim_ = detect_newline_delimiter(cgi_response);
}

CgiParser::~CgiParser() {}