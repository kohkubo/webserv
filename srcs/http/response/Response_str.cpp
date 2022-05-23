#include "http/response/Response.hpp"

std::map<int, std::string> g_response_status_phrase_map =
    init_response_status_phrase_map();

std::map<int, std::string> init_response_status_phrase_map() {
  std::map<int, std::string> res;
  res[200] = STATUS_200_PHRASE;
  res[204] = STATUS_204_PHRASE;
  res[304] = STATUS_304_PHRASE;
  res[400] = STATUS_400_PHRASE;
  res[403] = STATUS_403_PHRASE;
  res[404] = STATUS_404_PHRASE;
  res[500] = STATUS_500_PHRASE;
  res[501] = STATUS_501_PHRASE;
  res[520] = STATUS_520_PHRASE;
  return res;
}

std::string Response::get_response_string() {
  __set_status_phrase();
  __set_general_header();
  bool is_bodiless =
      __status_code_ == NO_CONTENT_204 || __status_code_ == NOT_MODIFIED_304;
  if (is_bodiless) {
    __make_bodiless_message_string();
  } else {
    __set_entity_header();
    __make_message_string();
  }
  return __response_string_;
}

void Response::__set_status_phrase() {
  __status_phrase_ = g_response_status_phrase_map[__status_code_];
}

void Response::__set_general_header() {
  __connection_ = CONNECTION_CLOSE; // TODO: 別関数に実装
}

// bodyをセットするところに移動したほうがよさそう
void Response::__set_entity_header() {
  __set_content_len();
  __set_content_type();
}

void Response::__set_content_len() {
  __content_len_ = to_string(__body_.size());
};

void Response::__set_content_type() { __content_type_ = TEXT_HTML; };

void Response::__make_bodiless_message_string() {
  // start line
  __response_string_ = VERSION_HTTP + SP + __status_phrase_ + CRLF;
  // header
  __response_string_ += "Connection: " + __connection_ + CRLF;
  // empty line
  __response_string_ += CRLF;
};

void Response::__make_message_string() {
  // start line
  __response_string_ = VERSION_HTTP + SP + __status_phrase_ + CRLF;
  // header
  __response_string_ += "Content-Length: " + __content_len_ + CRLF;
  __response_string_ += "Content-Type: " + __content_type_ + CRLF;
  __response_string_ += "Connection: " + __connection_ + CRLF;
  // empty line
  __response_string_ += CRLF;
  // body
  __response_string_ += __body_;
};
