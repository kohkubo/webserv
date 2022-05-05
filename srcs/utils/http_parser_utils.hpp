#ifndef SRCS_UTILS_HTTP_PARSER_UTILS_HPP
#define SRCS_UTILS_HTTP_PARSER_UTILS_HPP

#include "config/ServerConfig.hpp"

std::string resolve_url(const ServerConfig &server_config,
                        const std::string   request_url);

#endif /* SRCS_UTILS_HTTP_PARSER_UTILS_HPP */
