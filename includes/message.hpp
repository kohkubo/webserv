#ifndef INCLUDES_MESSAGE_HPP
#define INCLUDES_MESSAGE_HPP

#include "Lexer.hpp"
#include <iostream>
#include <map>

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

typedef std::map<std::string, std::string> message_type;

std::string response_message(std::map<std::string, std::string> &response);
std::map<std::string, std::string> parse_request(Lexer &request_lexer);
void get(message_type &request, message_type &response);

#endif /* INCLUDES_MESSAGE_HPP */
