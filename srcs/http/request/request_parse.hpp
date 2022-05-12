#ifndef SRCS_HTTP_REQUEST_REQUEST_HPP
#define SRCS_HTTP_REQUEST_REQUEST_HPP

#include "config/ServerConfig.hpp"
#include "http/HttpMessage.hpp"

HttpMessage parse_request_header(const std::string &request_string);

#endif /* SRCS_HTTP_REQUEST_REQUEST_HPP */
