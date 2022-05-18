#ifndef SRCS_EVENT_CONNECTION_HANDLER_HPP
#define SRCS_EVENT_CONNECTION_HANDLER_HPP

#include "config/ServerConfig.hpp"
#include "event/Connection.hpp"

void connection_receive_handler(int                            accfd,
                                std::map<conn_fd, Connection> &conn_fd_map);
void connection_send_handler(int                            accfd,
                             std::map<conn_fd, Connection> &conn_fd_map);

#endif /* SRCS_EVENT_CONNECTION_HANDLER_HPP */
