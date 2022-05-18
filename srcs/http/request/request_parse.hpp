#ifndef SRCS_HTTP_REQUEST_REQUEST_HPP
#define SRCS_HTTP_REQUEST_REQUEST_HPP

#include "config/Config.hpp"
#include "http/HttpMessage.hpp"

void parse_request_header(HttpMessage       &request_info,
                          const std::string &request_string);
void parse_request_body(HttpMessage       &request_info,
                        const std::string &request_string);
#endif /* SRCS_HTTP_REQUEST_REQUEST_HPP */
