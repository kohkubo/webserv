#ifndef SRCS_HTTP_HTTP_PARSER_UTILS_HPP
#define SRCS_HTTP_HTTP_PARSER_UTILS_HPP

#include "config/Config.hpp"
#include "http/HttpMessage.hpp"
#include "http/const/const_response_key_map.hpp"

std::string resolve_url(const ServerConfig &server_config,
                        const std::string  &request_url);

void        set_status_and_path(http_message_map   &response_info,
                                const ServerConfig &server_config,
                                HttpStatusCode      code);

void        set_response_body(http_message_map &response_info);

bool        is_minus_depth(std::string url);

#endif /* SRCS_HTTP_HTTP_PARSER_UTILS_HPP */
