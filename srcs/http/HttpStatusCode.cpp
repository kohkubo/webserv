#include "http/HttpStatusCode.hpp"

#include <map>
#include <string>

#include "utils/utils.hpp"

HttpStatusCode::HttpStatusCode(StatusCode status_code)
    : status_code_(status_code) {}

HttpStatusCode &HttpStatusCode::operator=(StatusCode status_code) {
  status_code_ = status_code;
  return *this;
}

HttpStatusCode &HttpStatusCode::operator=(const HttpStatusCode &other) {
  if (this == &other) {
    return *this;
  }
  status_code_ = other.status_code_;
  return *this;
}

HttpStatusCode &HttpStatusCode::operator=(const int num) {
  status_code_ = static_cast<StatusCode>(num);
  return *this;
}

std::map<int, const char *> init_response_status_phrase_map() {
  std::map<int, const char *> res;
  res[200] = "200 OK";
  res[201] = "201 Created";
  res[202] = "202 Accepted";
  res[204] = "204 No Content";
  res[206] = "206 Partial Content";
  res[301] = "301 Moved Permanently";
  res[302] = "302 Found";
  res[303] = "303 See Other";
  res[304] = "304 Not Modified";
  res[307] = "307 Temporary Redirect";
  res[308] = "308 Permanent Redirect";
  res[400] = "400 Bad Request";
  res[401] = "401 Unauthorized";
  res[403] = "403 Forbidden";
  res[404] = "404 Not Found";
  res[405] = "405 Method Not Allowed";
  res[406] = "406 Not Acceptable";
  res[408] = "408 Request Timeout";
  res[409] = "409 Conflict";
  res[410] = "410 Gone";
  res[411] = "411 Length Required";
  res[412] = "412 Precondition Failed";
  res[413] = "413 Content Too Large";
  res[414] = "414 URI Too Long";
  res[415] = "415 Unsupported Media Type";
  res[416] = "416 Range Not Satisfiable";
  res[421] = "421 Misdirected Request";
  res[500] = "500 Internal Server Error";
  res[501] = "501 Not Implemented";
  res[502] = "502 Bad Gateway";
  res[503] = "503 Service Temporarily Unavailable";
  res[504] = "504 Gateway Timeout";
  res[505] = "505 HTTP Version Not Supported";
  return res;
}

std::map<int, const char *> g_response_status_phrase_map =
    init_response_status_phrase_map();

bool HttpStatusCode::is_error_status_code() const {
  return 399 < status_code_ && status_code_ < 600;
}
bool HttpStatusCode::has_default_content() const {
  return 299 < status_code_ && status_code_ < 600;
}

std::string HttpStatusCode::status_phrase() const {
  return std::string(g_response_status_phrase_map[status_code_]);
}

std::string HttpStatusCode::str() const { return to_string(status_code_); }

std::string HttpStatusCode::create_default_content_str() const {
  LOG("create_default_content_str");
  return "<!DOCTYPE html>\n"
         "<html>\n"
         "    <head>\n"
         "        <title>" +
         str() +
         "</title>\n"
         "    </head>\n"
         "    <body>\n"
         "<h2>" +
         status_phrase() +
         "</h2>\n"
         "provided by webserv\n"
         "    </body>\n"
         "</html>";
}

bool operator==(const HttpStatusCode &lhs, const HttpStatusCode &rhs) {
  return lhs.status_code_ == rhs.status_code_;
}

HttpStatusCode::operator StatusCode() const { return status_code_; }
