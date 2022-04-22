#ifndef INCLUDES_CONNECTION
#define INCLUDES_CONNECTION

#include <string>

struct Connection {
  int         socket_fd_;
  std::string data_;

  Connection(int socket_fd) : socket_fd_(socket_fd) {}
};

#endif /* INCLUDES_CONNECTION */
