#ifndef SRCS_HTTP_RESPONSE_RESPONSE_HPP
#define SRCS_HTTP_RESPONSE_RESPONSE_HPP

#include "config/ServerConfig.hpp"
#include "http/HttpMessage.hpp"
#include "http/const/const_response_key_map.hpp"

http_message_map create_response_info(const ServerConfig &server_config,
                                      HttpMessage        &request_info);
std::string      make_message_string(http_message_map &response_info);
std::string      read_file_tostring_cgi(const std::string &file_path);

#endif /* SRCS_HTTP_RESPONSE_RESPONSE_HPP */
