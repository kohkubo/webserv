#ifndef SRCS_EVENT_EVENT_HPP
#define SRCS_EVENT_EVENT_HPP

#include "config/ServerConfig.hpp"
#include <map>
#include <poll.h>
#include <unistd.h>

void                               listen_event(socket_list_type &socket_list);

typedef std::vector<struct pollfd> pollfds_type;

#endif /* SRCS_EVENT_EVENT_HPP */
