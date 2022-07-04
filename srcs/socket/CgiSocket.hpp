#ifndef SRCS_SOCKET_CGISOCKET_HPP
#define SRCS_SOCKET_CGISOCKET_HPP

#include "event/Response.hpp"
#include "http/response/ResponseGenerator.hpp"
#include "socket/SocketBase.hpp"
#include "socket/Timeout.hpp"

namespace ns_socket {

typedef response_generator::ResponseGenerator ResponseGenerator;

class CgiSocket : public SocketBase {
private:
  Timeout                  _timeout_;
  Response                &_response_;
  ResponseGenerator        _response_generator_;
  static const std::time_t TIMEOUT_SECONDS_ = 5;

public:
  CgiSocket(Response &response, ResponseGenerator response_generator);
  virtual ~CgiSocket() {}
  virtual struct pollfd    pollfd();
  virtual SocketMapActions handle_event(short int revents);
  virtual bool             is_timed_out();
};

} // namespace ns_socket

#endif /* SRCS_SOCKET_CGISOCKET_HPP */
