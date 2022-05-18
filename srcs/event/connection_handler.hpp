#ifndef SRCS_EVENT_CONNECTION_HANDLER_HPP
#define SRCS_EVENT_CONNECTION_HANDLER_HPP

#include "config/Config.hpp"
#include "event/Connection.hpp"

void connection_receive_handler(int                        conn_fd,
                                std::map<int, Connection> &conn_fd_map);
void connection_send_handler(int                        conn_fd,
                             std::map<int, Connection> &conn_fd_map);

#endif /* SRCS_EVENT_CONNECTION_HANDLER_HPP */
