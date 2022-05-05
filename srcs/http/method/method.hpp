#ifndef SRCS_HTTP_METHOD_METHOD_HPP
#define SRCS_HTTP_METHOD_METHOD_HPP

#include "config/ServerConfig.hpp"
#include "http/HttpMessage.hpp"
#include "http/const/const_response_key_map.hpp"

http_message_map method_get(const ServerConfig &server_config,
                            HttpMessage        &request_message);

#endif /* SRCS_HTTP_METHOD_METHOD_HPP */
