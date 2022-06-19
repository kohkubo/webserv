#ifndef SRCS_SOCKET_LISTENSOCKET_HPP
#define SRCS_SOCKET_LISTENSOCKET_HPP

#include <map>

#include <poll.h>

#include "config/Config.hpp"
#include "socket/SocketBase.hpp"

class ListenSocket : public SocketBase {
public:
  confGroup conf_group_;

public:
  ListenSocket(int passive_socket, const Config *config);
  virtual ~ListenSocket() {}
  virtual struct pollfd pollfd();
  virtual SocketMapOp   handle_event(short int revents);
};

#endif /* SRCS_SOCKET_LISTENSOCKET_HPP */
