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

class ClientSocket : public SocketBase {
private:
  const ConfigGroup       &_config_group_;
  Request                  _request_;
  std::deque<Response>     _response_queue_;
  std::string              _buffer_;
  std::time_t              _last_event_time_;
  static const std::time_t TIMEOUT_SECONDS_ = 60;

private:
  static std::time_t _time_now() { return std::time(NULL); }

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
