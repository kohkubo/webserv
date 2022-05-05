#ifndef SRCS_HTTP_REQUEST_PARSE_HPP
#define SRCS_HTTP_REQUEST_PARSE_HPP

#include "config/ServerConfig.hpp"
#include "http/HttpMessage.hpp"
#include "utils/tokenize.hpp"
#include <vector>

HttpMessage parse_request_message(token_vector &request_tokens);

#endif /* SRCS_HTTP_REQUEST_PARSE_HPP */
