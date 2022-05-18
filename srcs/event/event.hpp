#ifndef SRCS_EVENT_EVENT_HPP
#define SRCS_EVENT_EVENT_HPP

#include "config/Config.hpp"
#include <map>
#include <poll.h>
#include <unistd.h>

void listen_event(std::map<listen_fd, conf_group> &listen_fd_map);

#endif /* SRCS_EVENT_EVENT_HPP */
