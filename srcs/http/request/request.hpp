#ifndef SRCS_HTTP_REQUEST_HPP
#define SRCS_HTTP_REQUEST_HPP

#include "config/ServerConfig.hpp"
#include "http/HttpMessage.hpp"

HttpMessage parse_request_message(const std::string &request_string);
std::string receive_request(int accfd);

#endif /* SRCS_HTTP_REQUEST_HPP */