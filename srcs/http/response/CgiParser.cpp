#include "http/response/CgiParser.hpp"

#include "utils/utils.hpp"

// cgiのパースエラー -> 500？

static std::string detect_newline_delimiter(const std::string &cgi_response) {
  std::size_t pos = cgi_response.find("\r\n");
  if (pos != std::string::npos) {
    return "\r\n";
  }
  pos = cgi_response.find("\n");
  if (pos == std::string::npos) {
    // error
  }
  return "\n";
}

bool CgiParser::_parse_content_type(std::string &cgi_response) {
  if (!has_prefix(cgi_response, "Content-Type:")) {
    return false;
  }
  std::string line;
  if (getline(cgi_response, line, _new_line_delim_)) {
    std::size_t pos = line.find(':');
    _content_type_  = trim(line.substr(pos + 1), "\t ");
    return true;
  }
  // error
  return false;
}

CgiParser::CgiParser(std::string &cgi_response) {
  _new_line_delim_ = detect_newline_delimiter(cgi_response);
  if (_parse_content_type(cgi_response)) {
    _response_type_ = document;
    // _parse_document_response();
  }
  // } else if (_parse_location()) {
  // }
}

CgiParser::~CgiParser() {}