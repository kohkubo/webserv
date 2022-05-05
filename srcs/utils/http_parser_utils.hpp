#ifndef SRCS_UTILS_HTTP_PARSER_UTILS
#define SRCS_UTILS_HTTP_PARSER_UTILS

#include "config/ServerConfig.hpp"
#include "http/HttpMessage.hpp"
#include "http/const/const_response_key_map.hpp"

std::string resolve_url(const ServerConfig &server_config,
                        const std::string  &request_url);

#endif /* SRCS_UTILS_HTTP_PARSER_UTILS */
