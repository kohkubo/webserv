#ifndef SRCS_HTTP_RESPONSE_HPP
#define SRCS_HTTP_RESPONSE_HPP

#include "config/ServerConfig.hpp"
#include "http/HttpMessage.hpp"
#include "http/const/const_response_key_map.hpp"

http_message_map create_response_info(const ServerConfig &server_config,
                                      HttpMessage        &request_info);

std::string      make_message_string(http_message_map &response_info);

#endif /* SRCS_HTTP_RESPONSE_HPP */
