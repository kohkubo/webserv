#ifndef SRCS_SOCKET_FILEWRITESOCKET_HPP
#define SRCS_SOCKET_FILEWRITESOCKET_HPP

#include <string>

#include "event/ResponseSender.hpp"
#include "http/HttpStatusCode.hpp"
#include "http/response/ResponseGenerator.hpp"
#include "socket/ClientSocket.hpp"
#include "socket/LocalIOSocket.hpp"
#include "socket/Timeout.hpp"

namespace ns_socket {

typedef response_generator::ResponseGenerator ResponseGenerator;

class FileWriteSocket : public LocalIOSocket {
private:
  static const std::time_t TIMEOUT_SECONDS_ = 5;

public:
  FileWriteSocket(ResponseSender   &response_sender,
                  ResponseGenerator response_generator,
                  ClientSocket     *parent_socket)
      : LocalIOSocket(response_sender, response_generator, parent_socket,
                      TIMEOUT_SECONDS_) {}

  virtual ~FileWriteSocket(){};
  virtual struct pollfd    pollfd();
  virtual SocketMapActions handle_event(short int revents);
  virtual bool             is_timed_out() { return _timeout_.is_timed_out(); }
};

} // namespace ns_socket

#endif /* SRCS_SOCKET_FILEWRITESOCKET_HPP */
