#ifndef SRCS_SOCKET_FILEREADSOCKET_HPP
#define SRCS_SOCKET_FILEREADSOCKET_HPP

#include <string>

#include "event/ResponseSender.hpp"
#include "http/HttpStatusCode.hpp"
#include "http/response/ResponseGenerator.hpp"
#include "socket/ClientSocket.hpp"
#include "socket/LocalIOSocket.hpp"

namespace ns_socket {

typedef response_generator::ResponseGenerator ResponseGenerator;

class FileReadSocket : public LocalIOSocket {
private:
  std::stringstream        _buffer_;
  static const std::time_t TIMEOUT_SECONDS_  = 5;
  static const std::size_t READ_BUFFER_SIZE_ = 1024;

public:
  FileReadSocket(ResponseSender &response, ResponseGenerator response_generator,
                 ClientSocket *parent_socket)
      : LocalIOSocket(response, response_generator, parent_socket,
                      TIMEOUT_SECONDS_) {}

  virtual ~FileReadSocket() {}
  virtual struct pollfd    pollfd();
  virtual SocketMapActions handle_event(short int revents);
  virtual bool             is_timed_out() { return _timeout_.is_timed_out(); }
};

} // namespace ns_socket

#endif /* SRCS_SOCKET_FILEREADSOCKET_HPP */
