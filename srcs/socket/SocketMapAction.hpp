#ifndef SRCS_SOCKET_SOCKETMAPACTION_HPP
#define SRCS_SOCKET_SOCKETMAPACTION_HPP

#include <cstddef>

class SocketBase;

struct SocketMapAction {
  enum ActionType {
    NONE,
    INSERT,
    DELETE,
  };

  ActionType  type_;
  int         socket_fd_;
  SocketBase *target_;

  SocketMapAction()
      : type_(NONE)
      , socket_fd_(-1)
      , target_(NULL) {}

  SocketMapAction(ActionType type, int socket_fd, SocketBase *target)
      : type_(type)
      , socket_fd_(socket_fd)
      , target_(target) {}
};

#endif /* SRCS_SOCKET_SOCKETMAPACTION_HPP */
