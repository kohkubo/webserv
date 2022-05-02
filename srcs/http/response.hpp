#ifndef SRCS_HTTP_RESPONSE_HPP
#define SRCS_HTTP_RESPONSE_HPP

#include "../ServerConfig.hpp"
#include "HttpMessage.hpp"

std::string create_response(const ServerConfig &server_config,
                            HttpMessage        &request_message);

#endif /* SRCS_HTTP_RESPONSE_HPP */
