#include "http/response/CgiParser.hpp"

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
    LOG("getline_cgi: delim not found");
    return Error<std::string>();
  }
  return Ok<std::string>(getline_cgi_sub(source, delim, pos));
}

static std::string
parse_content_type_sub(const std::string &content_type_line) {
  tokenVector token_vector = tokenize(content_type_line, ";", ";");
  return token_vector[0];
}

static Result<std::string>
parse_content_type(const HeaderFieldMap &header_field_map) {
  if (!header_field_map.has_field("content-type")) {
    return Error<std::string>();
  }
  const std::string &value = header_field_map.value("content-type");
  return Ok<std::string>(parse_content_type_sub(value));
}

static Result<std::size_t>
parse_content_length_sub(const std::string &content_length) {
  Result<std::size_t> result = string_to_size(content_length);
  if (result.is_err_) {
    return Error<std::size_t>();
  }
  return Ok<std::size_t>(result.object_);
}

static Result<std::size_t>
parse_content_length(const HeaderFieldMap &header_field_map) {
  if (!header_field_map.has_field("content-length")) {
    return Error<std::size_t>();
  }
  const std::string &value = header_field_map.value("content-length");
  return parse_content_length_sub(value);
}

static bool is_client_location(const std::string &location) {
  return location.find("http://") == 0 || location.find("https://") == 0;
}

static CgiParser::CgiLocation
parse_cgi_location_sub(const std::string &location_line) {
  CgiParser::CgiLocation cgi_location;
  tokenVector            token_vector = tokenize(location_line, "?", "?");
  cgi_location.path_                  = token_vector[0];
  if (token_vector.size() > 1) {
    cgi_location.query_ = token_vector[1];
  }
  return cgi_location;
}

static Result<CgiParser::CgiLocation>
parse_cgi_location(const HeaderFieldMap &header_field_map) {
  if (!header_field_map.has_field("location")) {
    LOG("cgi location not found");
    return Error<CgiParser::CgiLocation>();
  }
  const std::string &value = header_field_map.value("location");
  return Ok<CgiParser::CgiLocation>(parse_cgi_location_sub(value));
}

static CgiParser::ResponseType
get_cgi_response_type(const HeaderFieldMap &header_field_map) {
  if (!header_field_map.has_field("location")) {
    return CgiParser::DOCUMENT;
  }
  const std::string &value = header_field_map.value("location");
  if (is_client_location(value)) {
    if (!header_field_map.has_field("status")) {
      return CgiParser::CLIENT_REDIRDOC;
    }
    return CgiParser::CLIENT_REDIR;
  }
  return CgiParser::LOCAL_REDIR;
}

CgiParser::CgiState CgiParser::parse_header(std::string &buffer) {
  for (;;) {
    Result<std::string> result = getline_cgi(buffer, "\n");
    if (result.is_err_) {
      return HEADER;
    }
    std::string line = result.object_;
    if (line.empty() && !has_content_length_) {
      LOG("cgi header end ============");
      return HEADER_END;
    }
    if (!header_field_map_.store_new_field(line)) {
      return ERROR;
    }
  }
}

CgiParser::CgiState CgiParser::parse_body(std::string &buffer) {
  // bodyのパース時に毎回content lengthパース？ yn
  Result<std::size_t> result = parse_content_length(header_field_map_);
  if (result.is_err_) {
    LOG("parse_body: invalid content-length");
    return ERROR;
  }
  content_length_     = result.object_;
  has_content_length_ = true;
  if (buffer.size() >= content_length_) {
    LOG("parse_body");
    content_ = buffer.substr(0, content_length_);
    buffer.erase(0, content_length_);
    return END;
  }
  return BODY;
}

CgiParser::CgiState CgiParser::parse_header_end() {
  Result<std::string> result = parse_content_type(header_field_map_);
  if (result.is_err_) {
    ERROR_LOG("no parse_content_type");
  }
  content_type_  = result.object_;
  response_type_ = get_cgi_response_type(header_field_map_);
  Result<CgiParser::CgiLocation> result_cgi_location =
      parse_cgi_location(header_field_map_);
  if (!result_cgi_location.is_err_) {
    cgi_location_ = result_cgi_location.object_;
  }
  if (header_field_map_.has_field("content-length")) {
    LOG("has_cgi_body");
    return BODY;
  }
  return END;
}

CgiParser::CgiState CgiParser::handle_cgi(std::string &buffer) {
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

std::string CgiParser::http_start_line() const {
  if (response_type_ == CLIENT_REDIR || response_type_ == CLIENT_REDIRDOC) {
    return "HTTP/1.1" + SP + HttpStatusCode(302).status_phrase() + CRLF;
  }
  if (header_field_map_.has_field("status")) {
    return "HTTP/1.1" + SP + header_field_map_.value("status") + CRLF;
  }
  return "HTTP/1.1" + SP + HttpStatusCode().status_phrase() + CRLF;
}
