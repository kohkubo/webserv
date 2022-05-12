#ifndef SRCS_HTTP_REQUEST_REQUEST_HPP
#define SRCS_HTTP_REQUEST_REQUEST_HPP

#include "config/ServerConfig.hpp"
#include "http/HttpMessage.hpp"
#include "utils/tokenize.hpp"
#include <vector>

HttpMessage              parse_request_message(token_vector &request_tokens);
std::vector<std::string> receive_request(int accfd);

#endif /* SRCS_HTTP_REQUEST_REQUEST_HPP */
