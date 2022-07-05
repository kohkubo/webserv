#include "http/response/CgiParser.hpp"

#include "utils/tokenize.hpp"
#include "utils/utils.hpp"

// cgiのパースエラー -> 500？

std::string detect_newline_delimiter(const std::string &cgi_response) {
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

static Result<std::string> getline_cgi(std::string       &source,
                                       const std::string &delim) {
  std::size_t pos = source.find(delim);
  if (pos == std::string::npos)
    return Error<std::string>();
  std::string line = source.substr(0, pos);
  source.erase(0, pos + delim.size());
  return Ok<std::string>(line);
}

// Content-type: text/plain; charset=iso-8859-1
Result<std::string> parse_content_type(const CgiParser::HeaderMap &header_map) {
  CgiParser::HeaderMap::const_iterator it = header_map.find("content-type");
  if (it == header_map.end()) {
    return Error<std::string>();
  }
  std::string content_type = it->second;
  tokenVector token_vector = tokenize(content_type, ";", ";");
  return Ok<std::string>(token_vector[0]);
}

CgiParser::CgiState CgiParser::parse_header(std::string &buffer) {
  for (;;) {
    Result<std::string> result = getline_cgi(buffer, new_line_delimiter_);
    if (result.is_err_) {
      return HEADER;
    }
    std::string line = result.object_;
    if (line.empty()) {
      Result<std::string> result = parse_content_type(header_map_);
      if (result.is_err_) {
        ERROR_LOG("parse_content_type error");
        return ERROR;
      }
      content_type_ = result.object_;
      return BODY;
    }
    std::size_t pos = line.find(":");
    if (pos == std::string::npos) {
      return ERROR;
    }
    std::string key   = tolower(line.substr(0, pos));
    std::string value = tolower(trim(line.substr(pos + 1), " "));
    header_map_[key]  = value;
  }
}

Result<std::size_t>
parse_content_length(const CgiParser::HeaderMap &header_map) {
  CgiParser::HeaderMap::const_iterator it = header_map.find("content-length");
  if (it == header_map.end()) {
    return Error<std::size_t>();
  }
  Result<std::size_t> result = string_to_size(it->second);
  if (result.is_err_) {
    return Error<std::size_t>();
  }
  return Ok<std::size_t>(result.object_);
}

CgiParser::CgiState CgiParser::parse_body(std::string &buffer) {
  Result<std::size_t> result = parse_content_length(header_map_);
  content_length_            = result.object_;
  if (buffer.size() >= content_length_) {
    LOG("parse_body");
    content_ = buffer.substr(0, content_length_);
    buffer.erase(0, content_length_);
    return END;
  }
  return BODY;
}

CgiParser::CgiState CgiParser::handle_cgi(std::string &buffer) {
  (void)response_type_;
  if (state_ == HEADER) {
    state_ = parse_header(buffer);
  }
  if (state_ == BODY) {
    state_ = parse_body(buffer);
  }
  if (state_ == END) {
    return END;
  }
  return state_;
}
