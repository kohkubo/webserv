#include "http/request/RequestInfo.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

#include "http/const/const_delimiter.hpp"
#include "utils/tokenize.hpp"
#include "utils/utils.hpp"

const std::string RequestInfo::OWS_ = " \t";

static std::string parse_request_host(const std::string &host_line) {
  std::size_t pos = host_line.find(':');
  if (pos == std::string::npos) {
    return host_line;
  }
  return host_line.substr(0, pos);
}

static size_t parse_request_content_length(const std::string &content_length) {
  Result<std::size_t> result = string_to_size(content_length);
  if (result.is_err_) {
    throw RequestInfo::BadRequestException();
  }
  return result.object_;
}

static RequestInfo::transferEncodingVector
parse_request_transfer_encoding(const std::string &transfer_encoding) {
  return tokenize(transfer_encoding, " ,", " ,");
}

void RequestInfo::parse_request_header(const HeaderFieldMap &header_field_map) {
  if (header_field_map.has_field("host")) {
    const std::string &value = header_field_map.value("host");
    host_                    = parse_request_host(value);
  } else {
    throw BadRequestException(HttpStatusCode::S_400_BAD_REQUEST,
                              "Host field is not found.");
  }
  if (header_field_map.has_field("connection")) {
    connection_ = tolower(header_field_map.value("connection"));
  }
  if (header_field_map.has_field("content-length")) {
    const std::string &value      = header_field_map.value("content-length");
    content_info_.content_length_ = parse_request_content_length(value);
  }
  if (header_field_map.has_field("transfer-encoding")) {
    const std::string &value = header_field_map.value("transfer-encoding");
    transfer_encoding_       = parse_request_transfer_encoding(value);
  }
  if (header_field_map.has_field("content-type")) {
    content_info_.content_type_ = header_field_map.value("content-type");
  }
}
