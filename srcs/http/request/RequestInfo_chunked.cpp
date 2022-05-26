#include "http/request/RequestInfo.hpp"

/*
  length := 0
  read chunk-size, chunk-ext (if any), and CRLF
  while (chunk-size > 0) {
    read chunk-data and CRLF
    append chunk-data to decoded-body
    length := length + chunk-size
    read chunk-size, chunk-ext (if any), and CRLF
  }
  read trailer field
  while (trailer field is not empty) {
    if (trailer field is allowed to be sent in a trailer) {
        append trailer field to existing header fields
    }
    read trailer-field
  }
  Content-Length := length
  Remove "chunked" from Transfer-Encoding
  Remove Trailer from existing header fields
 */

bool RequestInfo::parse_request_chunked_body(std::string &body_line) {
  switch (__next_line_) {
  case CHUNK_SIZE:
    // hexdigit string to size_t
    // assign to next_chunk_size
    break;
  case CHUNK_DATA:
    // check equal size and data
    // append data to body
    // if size == 0 and data == "", return true
  default:
    break;
  }
  return false;
}
