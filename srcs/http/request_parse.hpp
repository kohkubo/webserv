#ifndef SRCS_HTTP_REQUEST_PARSE_HPP
#define SRCS_HTTP_REQUEST_PARSE_HPP

#include "HttpMessage.hpp"
#include "config/ServerConfig.hpp"
#include "util/tokenize.hpp"
#include <vector>

void parse_request_method_line(HttpMessage  &request_message,
                               token_vector &request_tokens);
void parse_request_host(HttpMessage  &request_message,
                        token_vector &request_tokens);
void parse_request_content_length(HttpMessage  &request_message,
                                  token_vector &request_tokens);
void parse_request_body(HttpMessage  &request_message,
                        token_vector &request_tokens);

#endif /* SRCS_HTTP_REQUEST_PARSE_HPP */
