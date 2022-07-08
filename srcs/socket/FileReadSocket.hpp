#ifndef SRCS_SOCKET_FILESOCKET_HPP
#define SRCS_SOCKET_FILESOCKET_HPP

#include <string>

#include "event/Response.hpp"
#include "http/HttpStatusCode.hpp"
#include "http/response/ResponseGenerator.hpp"
#include "socket/SocketBase.hpp"
#include "socket/Timeout.hpp"

namespace ns_socket {

typedef response_generator::ResponseGenerator ResponseGenerator;

class FileReadSocket : public SocketBase {
private:
  Timeout                  _timeout_;
  Response                &_response_;
  ResponseGenerator        _response_generator_;
  std::stringstream        _buffer_;
  static const std::time_t TIMEOUT_SECONDS_ = 5;

public:
  FileReadSocket(Response &response, ResponseGenerator response_generator)
      : SocketBase(response_generator.response_info_.fd_)
      , _timeout_(TIMEOUT_SECONDS_)
      , _response_(response)
      , _response_generator_(response_generator){};
  virtual ~FileReadSocket(){};
  virtual struct pollfd    pollfd();
  virtual SocketMapActions handle_event(short int revents);
  virtual bool             is_timed_out() { return _timeout_.is_timed_out(); }
  virtual SocketBase      *handle_timed_out();
};

} // namespace ns_socket

#endif /* SRCS_SOCKET_FILESOCKET_HPP */
