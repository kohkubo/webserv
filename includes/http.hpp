#ifndef INCLUDES_HTTP_HPP
#define INCLUDES_HTTP_HPP

#include <map>
#include <string>

#include "ServerConfig.hpp"

typedef std::map<std::string, std::string> http_message_map;

/* key */
// start line
#define METHOD                 "METHOD"
#define URL                    "URL"
#define VERSION                "VERSION"
#define STATUS                 "STATUS"
#define PHRASE                 "PHRASE"
// field
#define HOST                   "Host"
#define USERAGENT              "User-Agent"
#define ACCEPT                 "Accept"
#define CONTENT_LEN            "Content-Length"
#define CONTENT_TYPE           "Content-Type"
#define CONNECTION             "Connection"
// body
#define BODY                   "BODY"
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
#define HELLO_WORLD_PAGE             "./html/index.html"
#define FORBIDDEN_PAGE               "./html/forbidden.html"
#define NOT_FOUND_PAGE               "./html/not_found.html"
#define BAD_REQUEST_PAGE             "./html/bad_request.html"
#define NOT_IMPLEMENTED_PAGE         "./html/not_implemented.html"
#define INTERNAL_SERVER_ERROR_PAGE   "./html/internal_server_error.html"
#define UNKNOWN_ERROR_PAGE           "./html/unknown_error.html"
#define ROOT                         "/"
#define TEXT_HTML                    "text/html"
#define CONNECTION_CLOSE             "close"

/* delimiter */
#define CR                         "\r"
#define LF                         "\n"
#define CRLF                       "\r\n"
#define SP                         " "

void             http(int accfd);
http_message_map receive_request(int accfd);
std::string      create_response(const ServerConfig &server_config,
                                 http_message_map   &request_message);

http_message_map method_get(const ServerConfig &server_config,
                            http_message_map   &request_message);

#endif /* INCLUDES_HTTP_HPP */
