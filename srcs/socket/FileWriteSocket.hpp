#ifndef SRCS_SOCKET_FILEWRITESOCKET_HPP
#define SRCS_SOCKET_FILEWRITESOCKET_HPP

#include <string>

#include "event/Response.hpp"
#include "http/HttpStatusCode.hpp"
#include "http/response/ResponseGenerator.hpp"
#include "socket/LocalIOSocket.hpp"
#include "socket/Timeout.hpp"

namespace ns_socket {

typedef response_generator::ResponseGenerator ResponseGenerator;

class FileWriteSocket : public LocalIOSocket {
private:
  enum IOResult {
    ERROR = -1,
    CONTINUE,
    SUCCESS,
  };

  Timeout                  _timeout_;
  Response                &_response_;
  ResponseGenerator        _response_generator_;
  ssize_t                  _write_count_;
  static const std::time_t TIMEOUT_SECONDS_ = 5;

public:
  FileWriteSocket(Response &response, ResponseGenerator response_generator)
      : LocalIOSocket(response, response_generator)
      , _timeout_(TIMEOUT_SECONDS_)
      , _response_(response)
      , _response_generator_(response_generator)
      , _write_count_(0) {}
  virtual ~FileWriteSocket(){};
  virtual struct pollfd    pollfd();
  virtual SocketMapActions handle_event(short int revents);
  virtual bool             is_timed_out() { return _timeout_.is_timed_out(); }
  IOResult                 send_content();
};

} // namespace ns_socket

#endif /* SRCS_SOCKET_FILEWRITESOCKET_HPP */
