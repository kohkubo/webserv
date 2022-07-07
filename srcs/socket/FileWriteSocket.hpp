#ifndef SRCS_SOCKET_FILEWRITESOCKET_HPP
#define SRCS_SOCKET_FILEWRITESOCKET_HPP

#include <string>

#include "event/Response.hpp"
#include "http/HttpStatusCode.hpp"
#include "http/response/ResponseGenerator.hpp"
#include "socket/SocketBase.hpp"
#include "socket/Timeout.hpp"

namespace ns_socket {

typedef response_generator::ResponseGenerator ResponseGenerator;

class FileWriteSocket : public SocketBase {
private:
  Timeout                  _timeout_;
  Response                &_response_;
  ResponseGenerator        _response_generator_;
  ssize_t                  _send_count_;
  static const std::time_t TIMEOUT_SECONDS_ = 5;

public:
  FileWriteSocket(Response &response, ResponseGenerator response_generator)
      : SocketBase(response_generator.content_.fd_)
      , _timeout_(TIMEOUT_SECONDS_)
      , _response_(response)
      , _response_generator_(response_generator)
      , _send_count_(0) {}
  ~FileWriteSocket() {}
  virtual struct pollfd    pollfd();
  virtual SocketMapActions handle_event(short int revents);
  virtual bool             is_timed_out() { return _timeout_.is_timed_out(); }
};

} // namespace ns_socket

#endif /* SRCS_SOCKET_FILEWRITESOCKET_HPP */
