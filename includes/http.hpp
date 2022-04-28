#ifndef INCLUDES_HTTP_HPP
#define INCLUDES_HTTP_HPP

#include "ServerConfig.hpp"
#include <map>
#include <string>

typedef std::map<std::string, std::string> http_message_map;

// method
enum HttpMethod { GET, POST, DELETE, UNKNOWN };
enum HttpStatusCode {
  OK                    = 200,
  BAD_REQUEST           = 400,
  FORBIDDEN             = 403,
  NOT_FOUND             = 404,
  INTERNAL_SERVER_ERROR = 500,
  NOT_IMPLEMENTED       = 501,
  UNKNOWN_ERROR         = 520,
  NONE                  = 0
};

struct HttpMessage {
  HttpMethod     method_;
  std::string    url_;
  std::string    path_;
  std::string    version_;
  std::string    host_;
  std::string    status_;
  std::string    phrase_;
  std::string    body_;
  HttpStatusCode status_code_;

  HttpMessage() {
    method_      = UNKNOWN;
    status_code_ = NONE;
    path_        = "";
  }
};

/* key */
// start line
#define METHOD                       "METHOD"
#define URL                          "URL"
#define VERSION                      "VERSION"
#define STATUS                       "STATUS"
#define PHRASE                       "PHRASE"
#define PATH                         "PATH"
// field
#define HOST                         "Host"
#define USERAGENT                    "User-Agent"
#define ACCEPT                       "Accept"
#define CONTENT_LEN                  "Content-Length"
#define CONTENT_TYPE                 "Content-Type"
#define CONNECTION                   "Connection"
// status code
#define STATUS_CODE                  "status_code"
// body
#define BODY                         "BODY"
/* value */
#define VERSION_HTTP                 "HTTP/1.1"
#define STATUS_OK                    "200"
#define STATUS_FORBIDDEN             "403"
#define STATUS_NOTFOUND              "404"
#define STATUS_BADREQUEST            "400"
#define STATUS_INTERNALSERVER        "500"
#define STATUS_NOTIMPLEMENTED        "501"
#define STATUS_UNKNOWNERROR          "520"
#define PHRASE_STATUS_OK             "OK"
#define PHRASE_STATUS_FORBIDDEN      "Forbidden"
#define PHRASE_STATUS_NOTFOUND       "Not Found"
#define PHRASE_STATUS_BADREQUEST     "Bad Request"
#define PHRASE_STATUS_INTERNALSERVER "Internal Server Error"
#define PHRASE_STATUS_NOTIMPLEMENTED "Not Implemented"
#define PHRASE_STATUS_UNKNOWNERROR   "Unknown Error"
#define CONNECTION_CLOSE             "close"

/* delimiter */
#define CR                           "\r"
#define LF                           "\n"
#define CRLF                         "\r\n"
#define SP                           " "

void             http(int accfd);
HttpMessage      receive_request(int accfd);
std::string      create_response(const ServerConfig &server_config,
                                 HttpMessage        &request_message);

http_message_map method_get(const ServerConfig &server_config,
                            HttpMessage        &request_message);

#endif /* INCLUDES_HTTP_HPP */
