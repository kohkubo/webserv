#ifndef SRCS_HTTP_METHOD_HPP
#define SRCS_HTTP_METHOD_HPP

#include "HttpMessage.hpp"
#include "config/ServerConfig.hpp"
#include "const_response_key_map.hpp"

http_message_map method_get(const ServerConfig &server_config,
                            HttpMessage        &request_message);

#endif /* SRCS_HTTP_METHOD_HPP */
