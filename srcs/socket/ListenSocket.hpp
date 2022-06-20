#ifndef SRCS_SOCKET_LISTENSOCKET_HPP
#define SRCS_SOCKET_LISTENSOCKET_HPP

#include <map>

#include <poll.h>

#include "config/Config.hpp"
#include "socket/SocketBase.hpp"

class ListenSocket : public SocketBase {
public:
  confGroup conf_group_;

private:
  static listenFd _create_socket(struct addrinfo *info);
  static void     _bind_address(listenFd listen_fd, struct addrinfo *info);
  static void     _start_listen(listenFd listen_fd);
  static listenFd _open_new_socket(struct addrinfo *info);

public:
  ListenSocket(const Config *config);
  virtual ~ListenSocket() {}
  virtual struct pollfd   pollfd();
  virtual SocketMapAction handle_event(short int revents);
  virtual bool            is_timed_out();
};

#endif /* SRCS_SOCKET_LISTENSOCKET_HPP */
