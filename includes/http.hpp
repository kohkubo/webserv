#ifndef INCLUDES_HTTP_PROCESS_HPP
#define INCLUDES_HTTP_PROCESS_HPP

#include "Lexer.hpp"
#include <map>
#include <string>

typedef std::map<std::string, std::string> http_message_map;

/* key */
// start line
#define METHOD       "METHOD"
#define URL          "URL"
#define VERSION      "VERSION"
#define STATUS       "STATUS"
#define PHRASE       "PHRASE"
// field
#define HOST         "Host"
#define USERAGENT    "User-Agent"
#define ACCEPT       "Accept"
#define CONTENT_LEN  "Content-Length"
#define CONTENT_TYPE "Content-Type"
#define CONNECTION   "Connection"
// body
#define BODY         "BODY"

// method
enum HttpMethod { GET, POST, DELETE, UNKNOWN };

/* value */
#define VERSION_HTTP           "HTTP/1.1"
#define STATUS_OK              "200"
#define STATUS_NOTFOUND        "404"
#define PHRASE_STATUS_OK       "OK"
#define PHRASE_STATUS_NOTFOUND "Not Found"
#define HELLO_WORLD_PAGE       "./html/index.html"
#define NOT_FOUND_PAGE         "./html/not_found.html"
#define ROOT                   "/"
#define TEXT_HTML              "text/html"
#define CONNECTION_CLOSE       "close"

/* delimiter */
#define CR                     "\r"
#define LF                     "\n"
#define CRLF                   "\r\n"
#define SP                     " "

void             http(int accfd);
http_message_map http_parse_request_message(Lexer &request_lexer);
std::string      response_message_to_string(http_message_map &response_message);
http_message_map http_read_method_get(http_message_map &request_message);

#endif /* INCLUDES_HTTP_PROCESS_HPP */
