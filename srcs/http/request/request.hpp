#ifndef SRCS_HTTP_REQUEST_HPP
#define SRCS_HTTP_REQUEST_HPP

#include "http/HttpMessage.hpp"
#include <vector>

std::vector<std::string> receive_request(int accfd);

#endif /* SRCS_HTTP_REQUEST_HPP */
