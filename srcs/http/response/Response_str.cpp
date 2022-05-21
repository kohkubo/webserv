#include "http/response/Response.hpp"

std::string Response::get_response_string() {
  if (__status_code_ == NO_CONTENT_204 || __status_code_ == NOT_MODIFIED_304) {
    __make_bodyless_message_string();
    return __response_string_;
  }
  __set_body_header();
  __make_message_string();
  return __response_string_;
}

void Response::__set_body_header() {
  __set_content_len();
  __set_content_type();
}

void Response::__set_content_len() {
  __content_len_ = to_string(__body_.size());
};

void Response::__set_content_type() { __content_type_ = TEXT_HTML; };

void Response::__make_message_string() {
  // start line
  __response_string_ = __version_ + SP + __status_phrase_ + CRLF;
  // header
  __response_string_ += "Content-Length: " + __content_len_ + CRLF;
  __response_string_ += "Content-Type: " + __content_type_ + CRLF;
  __response_string_ += "Connection: " + __connection_ + CRLF;
  // empty line
  __response_string_ += CRLF;
  // body
  __response_string_ += __body_;
};

void Response::__make_bodyless_message_string() {
  // start line
  __response_string_ = __version_ + SP + __status_phrase_ + CRLF;
  // header
  __response_string_ += "Connection: " + __connection_ + CRLF;
  // empty line
  __response_string_ += CRLF;
};