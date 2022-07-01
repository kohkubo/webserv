#include "socket/FileReadSocket.hpp"

#include <poll.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "http/response/ResponseGenerator.hpp"
#include "socket/SocketMapActions.hpp"

namespace ns_socket {

typedef response_generator::ResponseGenerator ResponseGenerator;

struct pollfd FileReadSocket::pollfd() {
  struct pollfd pfd = {_socket_fd_, POLLIN, 0};
  return pfd;
}

SocketMapActions FileReadSocket::handle_event(short int revents) {
  LOG("got POLLIN  event of fd " << _socket_fd_);
  (void)revents;
  SocketMapActions socket_map_actions;
  _timeout_.update_last_event();
  char    buf[1024];
  ssize_t read_size = read(_socket_fd_, buf, 1024);
  if (read_size == -1) {
    LOG("read error");
    // TODO: 500をセットしてFileReadSocketを作成する
    socket_map_actions.add_socket_map_action(
        SocketMapAction(SocketMapAction::DELETE, _socket_fd_, this));
    return socket_map_actions;
  }
  if (read_size == 0) {
    std::string response_message =
        _response_generator_.create_response_message(_buffer_.str());
    // LOG("response message: " << response_message);
    _response_.set_response_message_and_sending(response_message);
    socket_map_actions.add_socket_map_action(
        SocketMapAction(SocketMapAction::DELETE, _socket_fd_, this));
    return socket_map_actions;
  }
  _buffer_ << std::string(buf, read_size);
  // LOG("read_size: " << read_size);
  return socket_map_actions;
}

} // namespace ns_socket