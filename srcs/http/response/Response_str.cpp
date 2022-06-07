#include "http/response/Response.hpp"

std::map<int, std::string> g_response_status_phrase_map =
    init_response_status_phrase_map();

std::map<int, std::string> init_response_status_phrase_map() {
  std::map<int, std::string> res;
  res[200] = STATUS_200_PHRASE;
  res[204] = STATUS_204_PHRASE;
  res[301] = STATUS_301_PHRASE;
  res[304] = STATUS_304_PHRASE;
  res[400] = STATUS_400_PHRASE;
  res[403] = STATUS_403_PHRASE;
  res[404] = STATUS_404_PHRASE;
  res[405] = STATUS_405_PHRASE;
  res[500] = STATUS_500_PHRASE;
  res[501] = STATUS_501_PHRASE;
  res[520] = STATUS_520_PHRASE;
  return res;
}

std::string Response::get_response_string() {
  // TODO: この分岐、モブプロ用のとき説明しながら直します。今はとりあえずの処理 kohkubo
  if (__status_code_ == MOVED_PERMANENTLY_301) {
    std::string response = "HTTP/1.1 " + std::to_string(__status_code_) + " " +
                           g_response_status_phrase_map[__status_code_] + CRLF +
                           "Location: " + "https://localhost:5001/" + CRLF +
                           CRLF + __body_;
    return response;
  }
  __status_phrase_ = __get_status_phrase();
  __set_general_header();
  bool is_bodiless =
      __status_code_ == NO_CONTENT_204 || __status_code_ == NOT_MODIFIED_304;
  if (is_bodiless) {
    return __make_bodiless_message_string();
  }
  __set_entity_header();
  return __make_message_string();
}

std::string Response::__get_status_phrase() {
  return g_response_status_phrase_map[__status_code_];
}

void Response::__set_general_header() {
  // Date
  // TODO: 別関数に実装
  __connection_ = CONNECTION_CLOSE;
}

void Response::__set_entity_header() {
  __content_len_  = to_string(__body_.size());
  __content_type_ = __get_content_type();
}

const std::string &Response::__get_content_type() { return TEXT_HTML; };

std::string        Response::__make_bodiless_message_string() {
  std::string response;
  // start line
  response = VERSION_HTTP + SP + __status_phrase_ + CRLF;
  // header
  response += "Connection: " + __connection_ + CRLF;
  // empty line
  response += CRLF;
  return response;
};

std::string Response::__make_message_string() {
  std::string response;
  // start line
  response = VERSION_HTTP + SP + __status_phrase_ + CRLF;
  // header
  response += "Content-Length: " + __content_len_ + CRLF;
  response += "Content-Type: " + __content_type_ + CRLF;
  response += "Connection: " + __connection_ + CRLF;
  // empty line
  response += CRLF;
  // body
  response += __body_;
  return response;
};
