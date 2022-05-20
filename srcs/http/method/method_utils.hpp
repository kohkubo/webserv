#ifndef SRCS_HTTP_HTTP_PARSER_UTILS_HPP
#define SRCS_HTTP_HTTP_PARSER_UTILS_HPP

#include "config/Config.hpp"
#include "http/const/const_response_key_map.hpp"
#include "http/request/RequestInfo.hpp"

std::string resolve_url(const Config      &server_config,
                        const std::string &request_url);

void        set_status_and_path(http_message_map &response_info,
                                const Config &server_config, HttpStatusCode code);

bool        is_minus_depth(std::string url);

#endif /* SRCS_HTTP_HTTP_PARSER_UTILS_HPP */
