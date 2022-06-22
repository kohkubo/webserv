#ifndef SRCS_SOCKET_LISTENSOCKET_HPP
#define SRCS_SOCKET_LISTENSOCKET_HPP

#include <map>

#include <poll.h>

#include "config/Config.hpp"
#include "config/ConfigGroup.hpp"
#include "socket/SocketBase.hpp"

class ListenSocket : public SocketBase {
public:
  const ConfigGroup &config_group_;

private:
  static listenFd _create_socket(struct addrinfo *info);
  static void     _bind_address(listenFd listen_fd, struct addrinfo *info);
  static void     _start_listen(listenFd listen_fd);
  static listenFd _open_new_socket(struct addrinfo *info);

public:
  ListenSocket(const ConfigGroup &config_group);
  virtual ~ListenSocket() {}
  virtual struct pollfd   pollfd();
  virtual SocketMapAction handle_event(short int revents);
  virtual bool            is_timed_out();
};

#endif /* SRCS_SOCKET_LISTENSOCKET_HPP */