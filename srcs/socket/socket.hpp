#ifndef SRCS_SOCKET_SOCKET_HPP
#define SRCS_SOCKET_SOCKET_HPP

#include <string>

#include "utils/Result.hpp"

namespace socket_base {

Result<std::string> receive(int socket_fd, std::size_t size);

}
#endif /* SRCS_SOCKET_SOCKET_HPP */
