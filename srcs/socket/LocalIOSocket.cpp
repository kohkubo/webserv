#include "socket/LocalIOSocket.hpp"

namespace ns_socket {

SocketMapActions LocalIOSocket::destroy_timedout_socket() {
  SocketMapActions  socket_map_actions;
  ResponseGenerator new_response_generator(
      _response_generator_.request_info_, _response_generator_.peer_name_,
      HttpStatusCode::S_500_INTERNAL_SERVER_ERROR);

  _response_ = new_response_generator.generate_response();
  if (new_response_generator.need_socket()) {
    SocketBase *file_socket =
        new_response_generator.create_socket(_response_, _parent_socket_);
    _parent_socket_->store_child_socket(file_socket);
    socket_map_actions.add_action(SocketMapAction::INSERT,
                                  file_socket->socket_fd(), file_socket);
  }
  return socket_map_actions;
}

void LocalIOSocket::overwrite_error_response(
    SocketMapActions &socket_map_actions, HttpStatusCode status_code) {
  ResponseGenerator new_response_generator(_response_generator_.request_info_,
                                           _response_generator_.peer_name_,
                                           status_code);

  _response_ = new_response_generator.generate_response();
  if (new_response_generator.need_socket()) {
    SocketBase *file_socket =
        new_response_generator.create_socket(_response_, _parent_socket_);
    _parent_socket_->store_child_socket(file_socket);
    socket_map_actions.add_action(SocketMapAction::INSERT,
                                  file_socket->socket_fd(), file_socket);
  }
}

LocalIOSocket::IOResult LocalIOSocket::send_content() {
  const std::string &sending_content =
      _response_generator_.response_info_.content_;
  const char *rest_str   = sending_content.c_str() + _send_count_;
  size_t      rest_count = sending_content.size() - _send_count_;
  ssize_t     write_size = write(_socket_fd_, rest_str, rest_count);

  if (write_size == -1) {
    LOG("write error");
    return ERROR;
  }
  _send_count_ += write_size;
  if (_send_count_ == static_cast<ssize_t>(sending_content.size())) {
    return SUCCESS;
  }
  return CONTINUE;
}

} // namespace ns_socket
