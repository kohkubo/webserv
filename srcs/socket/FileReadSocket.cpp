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
  // LOG("got POLLIN  event of fd " << _socket_fd_);
  (void)revents;
  SocketMapActions socket_map_actions;
  _timeout_.update_last_event();

  ReceiveResult receive_result = receive_content(READ_BUFFER_SIZE_);
  if (receive_result.status_ == ReceiveResult::ERROR) {
    LOG("read error");
    socket_map_actions.add_action(SocketMapAction::DELETE, _socket_fd_, this);
    overwrite_error_response(socket_map_actions,
                             HttpStatusCode::S_500_INTERNAL_SERVER_ERROR);
    return socket_map_actions;
  }
  if (receive_result.status_ == ReceiveResult::END) {
    std::string response_message = response_generator::create_response_message(
        _response_generator_.request_info_, _response_generator_.response_info_,
        _buffer_.str());
    // LOG("response message: " << response_message);
    _response_.set_response_message_and_sending(response_message);
    socket_map_actions.add_action(SocketMapAction::DELETE, _socket_fd_, this);
    return socket_map_actions;
  }
  _buffer_ << receive_result.str_;
  // LOG("read_size: " << read_size);
  return socket_map_actions;
}

} // namespace ns_socket