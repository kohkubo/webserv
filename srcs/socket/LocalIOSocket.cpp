#include "socket/LocalIOSocket.hpp"

namespace ns_socket {

LocalIOSocket::LocalIOSocket(Response         &response,
                             ResponseGenerator response_generator)
    : SocketBase(response_generator.response_info_.fd_)
    , _response_(response)
    , _response_generator_(response_generator) {}

SocketBase *LocalIOSocket::handle_timed_out() {
  SocketBase *file_socket = NULL;
  _response_              = _response_generator_.update_new_status(
                   HttpStatusCode::S_500_INTERNAL_SERVER_ERROR);
  if (_response_generator_.need_socket()) {
    file_socket = _response_generator_.create_socket(_response_);
  }
  return file_socket;
}

void LocalIOSocket::overwrite_error_response(
    SocketMapActions &socket_map_actions, HttpStatusCode status_code) {
  _response_              = _response_generator_.update_new_status(status_code);
  SocketBase *file_socket = _response_generator_.create_socket(_response_);
  if (file_socket != NULL) {
    socket_map_actions.add_action(SocketMapAction::INSERT,
                                  file_socket->socket_fd(), file_socket);
  }
}

} // namespace ns_socket
