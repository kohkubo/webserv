#ifndef SRCS_EVENT_EVENT_HPP
#define SRCS_EVENT_EVENT_HPP

#include "config/ServerConfig.hpp"
#include <map>
#include <poll.h>
#include <unistd.h>

int  open_new_socket(const ServerConfig &config);
void listen_event(const server_group_type &server_group);

typedef std::vector<struct pollfd> pollfds_type;

#endif /* SRCS_EVENT_EVENT_HPP */
