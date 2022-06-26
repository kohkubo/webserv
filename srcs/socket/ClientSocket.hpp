#ifndef SRCS_SOCKET_CLIENTSOCKET_HPP
#define SRCS_SOCKET_CLIENTSOCKET_HPP

#include <ctime>
#include <deque>
#include <string>
#include <vector>

#include "config/ConfigGroup.hpp"
#include "event/Request.hpp"
#include "event/Response.hpp"
#include "socket/SocketBase.hpp"
#include "socket/Timeout.hpp"

class ClientSocket : public SocketBase {
private:
  const ConfigGroup       &_config_group_;
  Request                  _request_;
  std::deque<Response>     _response_queue_;
  std::string              _buffer_;
  Timeout                  _timeout_;
  static const std::time_t TIMEOUT_SECONDS_ = 60;
  // #define NGX_HTTP_LINGERING_BUFFER_SIZE     4096
  static const size_t HTTP_LINGERING_BUFFER_SIZE_ = 4096;

public:
  ClientSocket(int client_fd, const ConfigGroup &config_group);
  virtual ~ClientSocket() {}
  virtual struct pollfd   pollfd();
  virtual SocketMapAction handle_event(short int revents);
  virtual bool            is_timed_out();

  SocketMapAction handle_receive_event();
  void            handle_send_event();

  void parse_buffer();
  bool append_receive_buffer();
};

#endif /* SRCS_SOCKET_CLIENTSOCKET_HPP */
