#ifndef SRCS_SOCKET_CLIENTSOCKET_HPP
#define SRCS_SOCKET_CLIENTSOCKET_HPP

#include <ctime>
#include <deque>
#include <string>
#include <vector>

#include "config/Config.hpp"
#include "event/Request.hpp"
#include "event/Response.hpp"
#include "socket/SocketBase.hpp"
#include "socket/Timeout.hpp"

class ClientSocket : public SocketBase {
private:
  static const std::time_t TIMEOUT_SECONDS_ = 60;
  const confGroup         &_conf_group_;
  Request                  _request_;
  std::deque<Response>     _response_queue_;
  std::string              _buffer_;
  Timeout                  _timeout_;

public:
  ClientSocket(int client_fd, const confGroup &conf_group);
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
