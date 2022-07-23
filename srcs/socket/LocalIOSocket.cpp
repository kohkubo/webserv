#include "socket/LocalIOSocket.hpp"

namespace ns_socket {

LocalIOSocket::LocalIOSocket(Response         &response,
                             ResponseGenerator response_generator,
                             ClientSocket     *parent_socket)
    : SocketBase(response_generator.response_info_.fd_)
    , _response_(response)
    , _response_generator_(response_generator)
    , _parent_socket_(parent_socket)
    , _send_count_(0) {}

SocketMapActions LocalIOSocket::destroy_timedout_socket() {
  SocketMapActions  socket_map_actions;
  ResponseGenerator new_response_gen =
      _response_generator_.new_status_response_generator(500);

  _response_ = new_response_gen.generate_response();
  if (new_response_gen.need_socket()) {
    SocketBase *file_socket =
        new_response_gen.create_socket(_response_, _parent_socket_);
    _parent_socket_->store_new_child_socket(file_socket);
    socket_map_actions.add_action(SocketMapAction::INSERT,
                                  file_socket->socket_fd(), file_socket);
  }
  return socket_map_actions;
}

void LocalIOSocket::overwrite_error_response(
    SocketMapActions &socket_map_actions, HttpStatusCode status_code) {
  ResponseGenerator new_response_gen =
      _response_generator_.new_status_response_generator(status_code);

  _response_ = new_response_gen.generate_response();
  if (new_response_gen.need_socket()) {
    SocketBase *file_socket =
        new_response_gen.create_socket(_response_, _parent_socket_);
    _parent_socket_->store_new_child_socket(file_socket);
    socket_map_actions.add_action(SocketMapAction::INSERT,
                                  file_socket->socket_fd(), file_socket);
  }
}

LocalIOSocket::IOResult LocalIOSocket::send_content() {
  std::string &sending_content = _response_generator_.response_info_.content_;
  const char  *rest_str        = sending_content.c_str() + _send_count_;
  size_t       rest_count      = sending_content.size() - _send_count_;
  ssize_t      write_size      = write(_socket_fd_, rest_str, rest_count);

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
