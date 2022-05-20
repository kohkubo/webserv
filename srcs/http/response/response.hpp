#ifndef SRCS_HTTP_RESPONSE_RESPONSE_HPP
#define SRCS_HTTP_RESPONSE_RESPONSE_HPP

#include "config/Config.hpp"
#include "http/const/const_response_key_map.hpp"
#include "http/request/RequestInfo.hpp"

std::string create_response_string(const Config &config,
                                      RequestInfo  &request_info);
std::string      make_message_string(http_message_map &response_info);
std::string      read_file_tostring_cgi(const std::string &file_path);
// TODO:
// テストのためにグローバルになっているが、テストのやり方を変更して、staticにする必要がありそう
void             set_response_body(http_message_map &response_info);

#endif /* SRCS_HTTP_RESPONSE_RESPONSE_HPP */
