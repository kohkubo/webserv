#ifndef SRCS_SOCKET_CLIENTSOCKET_HPP
#define SRCS_SOCKET_CLIENTSOCKET_HPP

#include <ctime>
#include <deque>
#include <string>
#include <vector>

#include "config/ConfigGroup.hpp"
#include "event/Request.hpp"
#include "event/SendResponse.hpp"
#include "socket/SocketBase.hpp"
#include "socket/Timeout.hpp"

namespace socket_base {

class ClientSocket : public SocketBase {
private:
  const ConfigGroup       &_config_group_;
  Request                  _request_;
  std::deque<SendResponse> _response_queue_;
  std::string              _buffer_;
  Timeout                  _timeout_;
  static const std::time_t TIMEOUT_SECONDS_ = 60;
  // #define NGX_HTTP_LINGERING_BUFFER_SIZE     4096
  // TODO: lingering の意味調べる
  // TODO: 4096の根拠を調べる
  static const size_t HTTP_BUFFER_SIZE_     = 4096;

public:
  ClientSocket(int client_fd, const ConfigGroup &config_group);
  virtual ~ClientSocket() {}
  virtual struct pollfd    pollfd();
  virtual SocketMapActions handle_event(short int revents);
  virtual bool             is_timed_out();

  bool handle_receive_event(SocketMapActions &socket_map_actions);
  void handle_send_event();

  void parse_buffer(SocketMapActions &socket_map_actions);
  bool append_receive_buffer();
};

} // namespace socket_base

#endif /* SRCS_SOCKET_CLIENTSOCKET_HPP */
