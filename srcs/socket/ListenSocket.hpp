#ifndef SRCS_SOCKET_LISTENSOCKET_HPP
#define SRCS_SOCKET_LISTENSOCKET_HPP

#include <map>

#include <poll.h>

#include "config/Config.hpp"
#include "config/ConfigGroup.hpp"
#include "socket/SocketBase.hpp"
#include "socket/SocketMapActions.hpp"

namespace ns_socket {

typedef int listenFd;

class ListenSocket : public SocketBase {
public:
  const config::ConfigGroup &config_group_;

private:
  static listenFd _create_socket();
  static void     _bind_address(listenFd listen_fd, struct sockaddr_in info);
  static void     _start_listen(listenFd listen_fd);
  static listenFd _open_new_socket(struct sockaddr_in info);

public:
  ListenSocket(const config::ConfigGroup &config_group);
  virtual ~ListenSocket() {}
  virtual struct pollfd    pollfd();
  virtual void             handle_event(short int         revents,
                                        SocketMapActions &socket_map_actions);
  virtual bool             is_timed_out();
  virtual SocketMapActions destroy_timedout_socket() {
    return SocketMapActions();
  }
};

} // namespace ns_socket

#endif /* SRCS_SOCKET_LISTENSOCKET_HPP */
