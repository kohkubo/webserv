#ifndef SRCS_SOCKET_LOCALIOSOCKET_HPP
#define SRCS_SOCKET_LOCALIOSOCKET_HPP

#include "SocketMapActions.hpp"
#include "event/Response.hpp"
#include "http/response/ResponseGenerator.hpp"
#include "socket/ClientSocket.hpp"
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
  ClientSocket     *_parent_socket_;
  ssize_t           _send_count_;

protected:
  void     overwrite_error_response(SocketMapActions &socket_map_actions,
                                    HttpStatusCode    status_code);
  IOResult send_content();

public:
  LocalIOSocket(Response &response, ResponseGenerator response_generator,
                ClientSocket *parent_socket);
  virtual ~LocalIOSocket() { _parent_socket_->notify_accomplished(this); }
  virtual SocketMapActions destroy_timedout_socket();
};

} // namespace ns_socket

#endif /* SRCS_SOCKET_LOCALIOSOCKET_HPP */
