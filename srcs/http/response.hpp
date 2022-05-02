#ifndef SRCS_HTTP_RESPONSE_HPP
#define SRCS_HTTP_RESPONSE_HPP

#include "HttpMessage.hpp"
#include "config/ServerConfig.hpp"

std::string create_response(const ServerConfig &server_config,
                            HttpMessage        &request_message);

#endif /* SRCS_HTTP_RESPONSE_HPP */
