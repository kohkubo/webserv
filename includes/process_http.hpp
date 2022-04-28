#ifndef INCLUDES_PROCESS_HTTP_HPP
#define INCLUDES_PROCESS_HTTP_HPP

#include "Http.hpp"
#include "ServerConfig.hpp"
#include <map>
#include <string>

void             process_http(int accfd);
HttpMessage      receive_request(int accfd);
std::string      create_response(const ServerConfig &server_config,
                                 HttpMessage        &request_message);

http_message_map method_get(const ServerConfig &server_config,
                            HttpMessage        &request_message);

#endif /* INCLUDES_PROCESS_HTTP_HPP */
