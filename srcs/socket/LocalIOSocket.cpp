#include "socket/LocalIOSocket.hpp"

namespace ns_socket {

LocalIOSocket::LocalIOSocket(Response         &response,
                             ResponseGenerator response_generator,
                             ClientSocket     *parent_socket)
    : SocketBase(response_generator.response_info_.fd_)
    , _response_(response)
    , _response_generator_(response_generator)
    , _parent_socket_(parent_socket) {}

SocketMapActions LocalIOSocket::handle_timed_out() {
  SocketMapActions socket_map_actions;
  SocketBase      *file_socket = NULL;
  _response_                   = _response_generator_.new_status_response(
                        HttpStatusCode::S_500_INTERNAL_SERVER_ERROR);
  if (_response_generator_.need_socket()) {
    file_socket =
        _response_generator_.create_socket(_response_, _parent_socket_);
    _parent_socket_->store_new_child_socket(file_socket);
    socket_map_actions.add_action(SocketMapAction::INSERT,
                                  file_socket->socket_fd(), file_socket);
  }
  return socket_map_actions;
}

void LocalIOSocket::overwrite_error_response(
    SocketMapActions &socket_map_actions, HttpStatusCode status_code) {
  _response_ = _response_generator_.new_status_response(status_code);
  SocketBase *file_socket =
      _response_generator_.create_socket(_response_, _parent_socket_);
  if (file_socket != NULL) {
    _parent_socket_->store_new_child_socket(file_socket);
    socket_map_actions.add_action(SocketMapAction::INSERT,
                                  file_socket->socket_fd(), file_socket);
  }
}

} // namespace ns_socket
