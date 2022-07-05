#ifndef SRCS_SOCKET_CGISOCKET_HPP
#define SRCS_SOCKET_CGISOCKET_HPP

#include <string>

#include "event/Response.hpp"
#include "http/response/ResponseGenerator.hpp"
#include "socket/SocketBase.hpp"
#include "socket/Timeout.hpp"
#include "socket/socket.hpp"

namespace ns_socket {

typedef response_generator::ResponseGenerator ResponseGenerator;

class CgiSocket : public SocketBase {
private:
  Timeout                  _timeout_;
  Response                &_response_;
  std::string              _buffer_;
  ResponseGenerator        _response_generator_;
  ssize_t                  _send_count_;
  static const std::time_t TIMEOUT_SECONDS_ = 5;
  static const size_t      CGI_BUFFER_SIZE_ = 2048;

private:
  bool _handle_receive_event();
  void _handle_send_event();

public:
  CgiSocket(Response &response, ResponseGenerator response_generator);
  virtual ~CgiSocket() {}
  virtual struct pollfd    pollfd();
  virtual SocketMapActions handle_event(short int revents);
  virtual bool             is_timed_out();
};

} // namespace ns_socket

#endif /* SRCS_SOCKET_CGISOCKET_HPP */
