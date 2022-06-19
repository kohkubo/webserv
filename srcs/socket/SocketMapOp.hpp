#ifndef SRCS_SOCKET_SOCKETMAPOP_HPP
#define SRCS_SOCKET_SOCKETMAPOP_HPP

#include <cstddef>

#include "socket/SocketBase.hpp"

struct SocketMapOp {
  enum OperationType {
    NONE,
    INSERT,
    DELETE,
  };

  OperationType type_;
  SocketBase   *target_;

  SocketMapOp()
      : type_(NONE)
      , target_(NULL) {}

  SocketMapOp(OperationType type, SocketBase *target)
      : type_(type)
      , target_(target) {}
};

#endif /* SRCS_SOCKET_SOCKETMAPOP_HPP */
