#ifndef SRCS_SOCKET_CLIENTSOCKET_HPP
#define SRCS_SOCKET_CLIENTSOCKET_HPP

#include "config/Config.hpp"
#include "socket/SocketBase.hpp"

class ClientSocket : public SocketBase {
private:
  const confGroup &_conf_group_;

public:
  ClientSocket(int client_fd, const confGroup &conf_group);
  ~ClientSocket();
  virtual struct pollfd pollfd();
  virtual void          handle_event(short int revents);
};

#endif /* SRCS_SOCKET_CLIENTSOCKET_HPP */
