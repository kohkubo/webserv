#include "http/response/CgiParser.hpp"

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
parse_content_type(const CgiParser::HeaderMap &header_map) {
  CgiParser::HeaderMap::const_iterator it = header_map.find("content-type");
  if (it == header_map.end()) {
    return Error<std::string>();
  }
  return Ok<std::string>(parse_content_type_sub(it->second));
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
parse_content_length(const CgiParser::HeaderMap &header_map) {
  CgiParser::HeaderMap::const_iterator it = header_map.find("content-length");
  if (it == header_map.end()) {
    return Error<std::size_t>();
  }
  return parse_content_length_sub(it->second);
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
parse_cgi_location(const CgiParser::HeaderMap &header_map) {
  CgiParser::HeaderMap::const_iterator it = header_map.find("location");
  if (it == header_map.end()) {
    LOG("cgi location not found");
    return Error<CgiParser::CgiLocation>();
  }
  return Ok<CgiParser::CgiLocation>(parse_cgi_location_sub(it->second));
}

static CgiParser::ResponseType
get_cgi_response_type(const CgiParser::HeaderMap &header_map) {
  CgiParser::HeaderMap::const_iterator it = header_map.find("location");
  if (it == header_map.end()) {
    return CgiParser::DOCUMENT;
  }
  std::string location = it->second;
  if (is_client_location(location)) {
    it = header_map.find("status");
    if (it == header_map.end()) {
      return CgiParser::CLIENT_REDIRDOC;
    }
    return CgiParser::CLIENT_REDIR;
  }
  return CgiParser::LOCAL_REDIR;
}

static bool has_cgi_body(const CgiParser::HeaderMap &header_map) {
  CgiParser::HeaderMap::const_iterator it = header_map.find("content-length");
  return it != header_map.end();
}

static bool insert_header_map(CgiParser::HeaderMap &header_map,
                              const std::string &line, std::size_t pos) {
  std::string key   = line.substr(0, pos);
  std::string value = trim(line.substr(pos + 1), " ");
  std::pair<CgiParser::HeaderMap::const_iterator, bool> insert_result =
      header_map.insert(std::make_pair(key, value));
  return insert_result.second;
}

CgiParser::CgiState CgiParser::parse_header(std::string &buffer) {
  for (;;) {
    Result<std::string> result = getline_cgi(buffer, "\n");
    if (result.is_err_) {
      return HEADER;
    }
    std::string line = result.object_;
    if (line.empty()) {
      LOG("cgi header end ============");
      return HEADER_END;
    }
    std::size_t pos = line.find(":");
    if (pos == std::string::npos) {
      ERROR_LOG("parse_hander: invalid header line");
      return ERROR;
    }
    if (!insert_header_map(header_map_, tolower(line), pos)) {
      ERROR_LOG("parse_hander: duplicate header line");
      return ERROR;
    }
  }
}

CgiParser::CgiState CgiParser::parse_body(std::string &buffer) {
  Result<std::size_t> result = parse_content_length(header_map_);
  if (result.is_err_) {
    LOG("parse_body: invalid content-length");
    return ERROR;
  }
  content_length_ = result.object_;
  if (buffer.size() >= content_length_) {
    LOG("parse_body");
    content_ = buffer.substr(0, content_length_);
    buffer.erase(0, content_length_);
    return END;
  }
  return BODY;
}

CgiParser::CgiState CgiParser::parse_header_end() {
  Result<std::string> result = parse_content_type(header_map_);
  if (result.is_err_) {
    ERROR_LOG("parse_content_type error");
    return ERROR;
  }
  content_type_  = result.object_;
  response_type_ = get_cgi_response_type(header_map_);
  Result<CgiParser::CgiLocation> result_cgi_location =
      parse_cgi_location(header_map_);
  if (!result_cgi_location.is_err_) {
    cgi_location_ = result_cgi_location.object_;
  }
  if (has_cgi_body(header_map_)) {
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
