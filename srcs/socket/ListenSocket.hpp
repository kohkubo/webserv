#ifndef SRCS_SOCKET_LISTENSOCKET_HPP
#define SRCS_SOCKET_LISTENSOCKET_HPP

#include <map>

#include <poll.h>

#include "config/Config.hpp"
#include "socket/SocketBase.hpp"

class ListenSocket : public SocketBase {
private:
  confGroup                    _conf_group_;
  std::map<int, SocketBase *> &_socket_map_;

public:
  ListenSocket(int passive_socket, std::map<int, SocketBase *> &socket_map);
  virtual ~ListenSocket();
  virtual struct pollfd pollfd();
  virtual void          handle_event(short int revents);
};

#endif /* SRCS_SOCKET_LISTENSOCKET_HPP */
