#ifndef SRCS_CONFIG_CONFIG_PARSER_UTILS_HPP
#define SRCS_CONFIG_CONFIG_PARSER_UTILS_HPP

#include <string>

bool is_uint8(const std::string &token);
bool is_ip(const std::string &token);
bool is_digits(const std::string &token);

#endif /* SRCS_CONFIG_CONFIG_PARSER_UTILS_HPP */
