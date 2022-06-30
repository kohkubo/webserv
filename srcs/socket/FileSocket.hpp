#ifndef SRCS_SOCKET_FILESOCKET_HPP
#define SRCS_SOCKET_FILESOCKET_HPP

#include <string>

#include "http/HttpStatusCode.hpp"
#include "socket/SocketBase.hpp"

namespace ns_socket {

class FileSocket : public SocketBase {
private:
  // 何を持つべきか考える
  HttpStatusCode _status_code_;
  std::string    _response_body_;

public:
  FileSocket();
  ~FileSocket();
};

} // namespace ns_socket

#endif /* SRCS_SOCKET_FILESOCKET_HPP */
