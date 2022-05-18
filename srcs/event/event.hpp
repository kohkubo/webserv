#ifndef SRCS_EVENT_EVENT_HPP
#define SRCS_EVENT_EVENT_HPP

#include "config/ServerConfig.hpp"
#include <map>
#include <poll.h>
#include <unistd.h>

void listen_event(std::map<listen_fd, conf_group> &listen_fds);

typedef std::vector<struct pollfd> pollfds_type;

#endif /* SRCS_EVENT_EVENT_HPP */
