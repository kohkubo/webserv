#ifndef SRCS_SOCKET_LOCALIOSOCKET_HPP
#define SRCS_SOCKET_LOCALIOSOCKET_HPP

#include "SocketMapActions.hpp"
#include "event/ResponseSender.hpp"
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

  ResponseSender         &_response_sender_;
  const ResponseGenerator _response_generator_;
  ClientSocket           *_parent_socket_;
  ssize_t                 _send_count_;

protected:
  void     overwrite_error_response(SocketMapActions &socket_map_actions,
                                    HttpStatusCode    status_code);
  IOResult send_content();

public:
  LocalIOSocket(ResponseSender &response, ResponseGenerator response_generator,
                ClientSocket *parent_socket, const std::time_t timeout_seconds)
      : SocketBase(response_generator.response_info_.fd_, timeout_seconds)
      , _response_sender_(response)
      , _response_generator_(response_generator)
      , _parent_socket_(parent_socket)
      , _send_count_(0) {}
  virtual ~LocalIOSocket() { _parent_socket_->notify_accomplished(this); }
  virtual SocketMapActions destroy_timedout_socket();
};

} // namespace ns_socket

#endif /* SRCS_SOCKET_LOCALIOSOCKET_HPP */
