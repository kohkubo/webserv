#ifndef INCLUDES_REQUEST_PARSE_HPP
#define INCLUDES_REQUEST_PARSE_HPP

#include <map>
#include <vector>

#include "ServerConfig.hpp"
#include "http.hpp"

void parse_request_method_line(HttpMessage              &request_message,
                               std::vector<std::string> &request_tokens);
void parse_request_host(HttpMessage              &request_message,
                        std::vector<std::string> &request_tokens);
void parse_request_content_length(HttpMessage              &request_message,
                                  std::vector<std::string> &request_tokens);
void parse_request_body(HttpMessage              &request_message,
                        std::vector<std::string> &request_tokens);

#endif /* INCLUDES_REQUEST_PARSE_HPP */
