#ifndef SRCS_SOCKET_SOCKETMAPOP_HPP
#define SRCS_SOCKET_SOCKETMAPOP_HPP

#include <cstddef>

typedef class SocketBase SocketBase;

enum OperationType {
  NO_OP,
  INSERT,
  DELETE,
};

struct SocketMapOp {

  OperationType type_;
  int           socket_fd_;
  SocketBase   *target_;

  SocketMapOp()
      : type_(NO_OP)
      , socket_fd_(-1)
      , target_(NULL) {}

  SocketMapOp(OperationType type, int socket_fd, SocketBase *target)
      : type_(type)
      , socket_fd_(socket_fd)
      , target_(target) {}
};

#endif /* SRCS_SOCKET_SOCKETMAPOP_HPP */
