#ifndef SRCS_HTTP_METHOD_METHOD_HPP
#define SRCS_HTTP_METHOD_METHOD_HPP

#include "config/Config.hpp"
#include "http/HttpMessage.hpp"
#include "http/const/const_response_key_map.hpp"

http_message_map method_get(const Config &server_config,
                            HttpMessage  &request_info);

http_message_map delete_method_handler(const Config &server_config,
                                       HttpMessage  &request_info);

#endif /* SRCS_HTTP_METHOD_METHOD_HPP */
