#include "http/response/CgiParser.hpp"

#include "utils/tokenize.hpp"
#include "utils/utils.hpp"

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
      result = parse_content_type(header_map_);
      if (result.is_err_) {
        ERROR_LOG("parse_content_type error");
        return ERROR;
      }
      content_type_ = result.object_;
      return HEADER_END;
    }
    std::size_t pos = line.find(":");
    if (pos == std::string::npos) {
      return ERROR;
    }
    std::string key   = tolower(line.substr(0, pos));
    std::string value = tolower(trim(line.substr(pos + 1), " "));
    std::pair<HeaderMap::const_iterator, bool> insert_result =
        header_map_.insert(std::make_pair(key, value));
    if (!insert_result.second) {
      return ERROR;
    }
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

bool is_client_location(const std::string &location) {
  return location.find("http://") == 0 || location.find("https://") == 0;
}

CgiParser::ResponseType
get_cgi_response_type(const CgiParser::HeaderMap &header_map) {
  CgiParser::HeaderMap::const_iterator it = header_map.find("location");
  if (it == header_map.end()) {
    return CgiParser::DOCUMENT;
  }
  std::string location = it->second;
  if (is_client_location(location)) {
    CgiParser::HeaderMap::const_iterator it = header_map.find("status");
    if (it == header_map.end()) {
      return CgiParser::CLIENT_REDIRDOC;
    }
    return CgiParser::CLIENT_REDIR;
  }
  return CgiParser::LOCAL_REDIR;
}

CgiParser::CgiState CgiParser::handle_cgi(std::string &buffer) {
  if (state_ == HEADER) {
    state_ = parse_header(buffer);
  }
  if (state_ == HEADER_END) {
    response_type_ = get_cgi_response_type(header_map_);
    CgiParser::HeaderMap::const_iterator it =
        header_map_.find("content-length");
    if (it == header_map_.end()) {
      state_ = END;
    } else {
      state_ = BODY;
    }
  }
  if (state_ == BODY) {
    state_ = parse_body(buffer);
  }
  return state_;
}
