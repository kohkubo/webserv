#ifndef SRCS_SOCKET_CLIENTSOCKET_HPP
#define SRCS_SOCKET_CLIENTSOCKET_HPP

#include <ctime>
#include <deque>
#include <set>
#include <string>
#include <vector>

#include "config/ConfigGroup.hpp"
#include "event/RequestHandler.hpp"
#include "event/Response.hpp"
#include "socket/SocketMapActions.hpp"
#include "socket/Timeout.hpp"

namespace ns_socket {

class ClientSocket : public SocketBase {
private:
  const config::ConfigGroup &_config_group_;
  RequestHandler             _request_handler_;
  std::deque<Response>       _response_queue_;
  std::string                _buffer_;
  std::string                _peer_name_;
  static const std::time_t   TIMEOUT_SECONDS_  = 60;
  static const size_t        HTTP_BUFFER_SIZE_ = 4096;

  std::set<SocketBase *> _child_socket_set_;

private:
  void _parse_buffer(SocketMapActions &socket_map_actions);
  void _handle_send_event(SocketMapActions &socket_map_actions);
  bool _handle_receive_event(SocketMapActions &socket_map_actions);
  void _add_delete_child_socket(SocketMapActions &socket_map_actions);

public:
  ClientSocket(int client_fd, const config::ConfigGroup &config_group);
  virtual ~ClientSocket() {}
  virtual struct pollfd    pollfd();
  virtual void             handle_event(short int         revents,
                                        SocketMapActions &socket_map_actions);
  virtual bool             is_timed_out();
  virtual SocketMapActions destroy_timedout_socket();
  void                     store_child_socket(SocketBase *child_socket);
  void                     notify_accomplished(SocketBase *child_socket);
};

} // namespace ns_socket

#endif /* SRCS_SOCKET_CLIENTSOCKET_HPP */
