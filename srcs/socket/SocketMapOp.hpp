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
  SocketBase   *target_;

  SocketMapOp()
      : type_(NO_OP)
      , target_(NULL) {}

  SocketMapOp(OperationType type, SocketBase *target)
      : type_(type)
      , target_(target) {}
};

#endif /* SRCS_SOCKET_SOCKETMAPOP_HPP */
