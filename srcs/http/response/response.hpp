#ifndef SRCS_HTTP_RESPONSE_RESPONSE_HPP
#define SRCS_HTTP_RESPONSE_RESPONSE_HPP

#include <string>

#include "http/request/RequestInfo.hpp"

std::string create_autoindex_body(const RequestInfo &request_info,
                                  const std::string &target_path);

#endif /* SRCS_HTTP_RESPONSE_RESPONSE_HPP */
