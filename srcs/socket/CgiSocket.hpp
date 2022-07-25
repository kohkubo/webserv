#ifndef SRCS_SOCKET_CGISOCKET_HPP
#define SRCS_SOCKET_CGISOCKET_HPP

#include <string>

#include "event/Response.hpp"
#include "http/response/CgiInfo.hpp"
#include "http/response/ResponseGenerator.hpp"
#include "socket/ClientSocket.hpp"
#include "socket/LocalIOSocket.hpp"
#include "socket/Timeout.hpp"

namespace ns_socket {

typedef response_generator::ResponseGenerator ResponseGenerator;

class CgiSocket : public LocalIOSocket {
private:
  std::string              _buffer_;
  CgiInfo                  _cgi_info_;
  bool                     _is_sending_;
  static const std::time_t TIMEOUT_SECONDS_ = 5;
  static const size_t      CGI_BUFFER_SIZE_ = 2048;

private:
  bool _handle_receive_event(SocketMapActions &socket_map_actions);
  void _handle_send_event(SocketMapActions &socket_map_actions);
  void _parse_buffer(SocketMapActions &socket_map_actions);
  void _kill_cgi_process() const;
  void _create_cgi_response(SocketMapActions &socket_map_actions);
  void _redirect_local(SocketMapActions &socket_map_actions);

public:
  CgiSocket(Response &response, ResponseGenerator response_generator,
            ClientSocket *parent_socket);
  virtual ~CgiSocket();
  virtual struct pollfd    pollfd();
  virtual SocketMapActions handle_event(short int revents);
  virtual bool             is_timed_out();
  virtual SocketMapActions destroy_timedout_socket();
};

} // namespace ns_socket

#endif /* SRCS_SOCKET_CGISOCKET_HPP */
