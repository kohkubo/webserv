#include "http/response/CgiInfo.hpp"

#include "http/const/const_delimiter.hpp"
#include "utils/tokenize.hpp"
#include "utils/utils.hpp"

static std::string getline_cgi_sub(std::string       &source,
                                   const std::string &delim, std::size_t pos) {
  std::string line = source.substr(0, pos);
  source.erase(0, pos + delim.size());
  return line;
}

static Result<std::string> getline_cgi(std::string       &source,
                                       const std::string &delim) {
  std::size_t pos = source.find(delim);
  if (pos == std::string::npos) {
    // LOG("getline_cgi: delim not found");
    return Error<std::string>();
  }
  return Ok<std::string>(getline_cgi_sub(source, delim, pos));
}

static bool is_client_location(const std::string &location) {
  return location.find("http://") == 0 || location.find("https://") == 0;
}

static CgiInfo::CgiLocation
parse_cgi_location_sub(const std::string &location_line) {
  CgiInfo::CgiLocation cgi_location;
  tokenVector          token_vector = tokenize(location_line, "?", "?");
  cgi_location.path_                = token_vector[0];
  if (token_vector.size() > 1) {
    cgi_location.query_ = token_vector[1];
  }
  return cgi_location;
}

static Result<CgiInfo::CgiLocation>
parse_cgi_location(const HeaderFieldMap &header_field_map) {
  if (!header_field_map.has_field("location")) {
    // LOG("cgi location not found");
    return Error<CgiInfo::CgiLocation>();
  }
  const std::string &value = header_field_map.value("location");
  return Ok<CgiInfo::CgiLocation>(parse_cgi_location_sub(value));
}

CgiInfo::CgiState CgiInfo::parse_header(std::string &buffer) {
  for (;;) {
    Result<std::string> result = getline_cgi(buffer, "\n");
    if (result.is_err_) {
      return HEADER;
    }
    std::string line = result.object_;
    if (line.size() >= BUFFER_MAX_LENGTH_) {
      return ERROR;
    }
    if (line.empty()) {
      // LOG("cgi header end ============");
      return HEADER_END;
    }
    if (!header_field_map_.store_new_field(line)) {
      return ERROR;
    }
  }
}

static CgiInfo::ResponseType
get_cgi_response_type(const HeaderFieldMap &header_field_map) {
  if (!header_field_map.has_field("location")) {
    return CgiInfo::DOCUMENT;
  }
  const std::string &value = header_field_map.value("location");
  if (is_client_location(value)) {
    if (header_field_map.has_field("status")) {
      return CgiInfo::CLIENT_REDIRDOC;
    }
    return CgiInfo::CLIENT_REDIR;
  }
  return CgiInfo::LOCAL_REDIR;
}

bool CgiInfo::_is_valid_header() const {
  if ((response_type_ == DOCUMENT || response_type_ == CLIENT_REDIRDOC) &&
      !header_field_map_.has_field("content-type")) {
    return false;
  }
  if (response_type_ == LOCAL_REDIR) {
    if (header_field_map_.size() != 1) {
      return false;
    }
  }
  return true;
}

bool CgiInfo::_parse_status_header() {
  const std::string &status = header_field_map_.value("status");
  if (status.size() <= 3) {
    return false;
  }
  Result<std::size_t> result = string_to_size(status.substr(0, 3));
  if (result.is_err_ || status[3] != ' ') {
    return false;
  }
  http_status_phrase_ = status;
  status_digit_       = result.object_;
  header_field_map_.erase("status");
  return true;
}

CgiInfo::CgiState CgiInfo::parse_header_end() {
  response_type_ = get_cgi_response_type(header_field_map_);
  if (!_is_valid_header()) {
    return ERROR;
  }
  Result<CgiInfo::CgiLocation> result_cgi_location =
      parse_cgi_location(header_field_map_);
  if (!result_cgi_location.is_err_) {
    cgi_location_ = result_cgi_location.object_;
  }

  if (header_field_map_.has_field("status")) {
    if (!_parse_status_header()) {
      return ERROR;
    }
  }
  if (header_field_map_.has_field("content-length")) {
    const std::string  &value  = header_field_map_.value("content-length");
    Result<std::size_t> result = string_to_size(value);
    if (result.is_err_) {
      return ERROR;
    }
    content_info_.content_length_ = result.object_;
    header_field_map_.erase("content-length");
  }
  if (response_type_ == LOCAL_REDIR) {
    return END;
  }
  return BODY;
}

CgiInfo::CgiState CgiInfo::parse_body(std::string &buffer) {
  if (content_info_.has_content_length() &&
      content_.size() + buffer.size() >=
          static_cast<std::size_t>(content_info_.content_length_)) {
    content_ +=
        buffer.substr(0, content_info_.content_length_ - content_.size());
    return END;
  }

  content_ += buffer;
  buffer.clear();
  if (content_.size() >= CONTENT_MAX_LENGTH_) {
    ERROR_LOG("cgi: parse_body: too long content");
    return ERROR;
  }
  return BODY;
}

CgiInfo::CgiState CgiInfo::handle_cgi(std::string &buffer) {
  if (state_ == HEADER) {
    state_ = parse_header(buffer);
  }
  if (state_ == HEADER_END) {
    state_ = parse_header_end();
  }
  if (state_ == BODY) {
    state_ = parse_body(buffer);
  }
  return state_;
}

std::string CgiInfo::http_start_line() const {
  if (response_type_ == CLIENT_REDIR) {
    return "HTTP/1.1" + SP + HttpStatusCode(302).status_phrase() + CRLF;
  }
  if (!http_status_phrase_.empty()) {
    return "HTTP/1.1" + SP + http_status_phrase_ + CRLF;
  }
  return "HTTP/1.1" + SP + HttpStatusCode().status_phrase() + CRLF;
}
