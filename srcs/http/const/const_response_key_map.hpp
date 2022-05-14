#ifndef SRCS_HTTP_CONST_CONST_RESPONSE_KEY_MAP_HPP
#define SRCS_HTTP_CONST_CONST_RESPONSE_KEY_MAP_HPP

#include <map>
#include <string>

typedef std::map<std::string, std::string> http_message_map;

// response map key
const std::string                          PATH             = "PATH";
const std::string                          STATUS_PHRASE    = "STATUS_PHRASE";
const std::string                          VERSION          = "VERSION";
const std::string                          CONTENT_LEN      = "Content-Length";
const std::string                          CONTENT_TYPE     = "Content-Type";
const std::string                          CONNECTION       = "Connection";
const std::string                          BODY             = "BODY";
// response map value
const std::string                          VERSION_HTTP     = "HTTP/1.1";
const std::string                          CONNECTION_CLOSE = "close";

#endif /* SRCS_HTTP_CONST_CONST_RESPONSE_KEY_MAP_HPP */
