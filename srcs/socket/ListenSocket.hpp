#ifndef SRCS_SOCKET_LISTENSOCKET_HPP
#define SRCS_SOCKET_LISTENSOCKET_HPP

#include <map>

#include <poll.h>

#include "config/Config.hpp"
#include "socket/SocketBase.hpp"

class ListenSocket : public SocketBase {
private:
  confGroup                          _conf_group_;
  const std::map<int, SocketBase *> &_parent_;

public:
  ListenSocket(int passive_socket, const std::map<int, SocketBase *> &parent);
  virtual ~ListenSocket();
  virtual struct pollfd pollfd();
  virtual void          handle_event(short int revents);
};

#endif /* SRCS_SOCKET_LISTENSOCKET_HPP */
