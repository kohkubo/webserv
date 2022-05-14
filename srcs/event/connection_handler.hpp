#ifndef SRCS_EVENT_CONNECTION_HANDLER_HPP
#define SRCS_EVENT_CONNECTION_HANDLER_HPP

#include "config/ServerConfig.hpp"
#include "event/Connection.hpp"

void connection_receive_handler(int                   accfd,
                                connection_list_type &connection_list,
                                socket_list_type     &socket_list);
void connection_send_handler(int accfd, connection_list_type &connection_list);

#endif /* SRCS_EVENT_CONNECTION_HANDLER_HPP */
