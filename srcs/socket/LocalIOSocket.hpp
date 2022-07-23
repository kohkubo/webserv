#ifndef SRCS_SOCKET_LOCALIOSOCKET_HPP
#define SRCS_SOCKET_LOCALIOSOCKET_HPP

#include "SocketMapActions.hpp"
#include "event/Response.hpp"
#include "http/response/ResponseGenerator.hpp"
#include "socket/SocketBase.hpp"

namespace ns_socket {

typedef response_generator::ResponseGenerator ResponseGenerator;

class LocalIOSocket : public SocketBase {
protected:
  enum IOResult {
    ERROR = -1,
    CONTINUE,
    SUCCESS,
  };

  Response         &_response_;
  ResponseGenerator _response_generator_;
  ssize_t           _send_count_;

protected:
  void     overwrite_error_response(SocketMapActions &socket_map_actions,
                                    HttpStatusCode    status_code);
  IOResult send_content();

public:
  LocalIOSocket(Response &response, ResponseGenerator response_generator);
  virtual ~LocalIOSocket() {}
  virtual SocketBase *handle_timed_out();
};

} // namespace ns_socket

#endif /* SRCS_SOCKET_LOCALIOSOCKET_HPP */
