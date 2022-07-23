#include "socket/LocalIOSocket.hpp"

namespace ns_socket {

LocalIOSocket::LocalIOSocket(Response         &response,
                             ResponseGenerator response_generator)
    : SocketBase(response_generator.response_info_.fd_)
    , _response_(response)
    , _response_generator_(response_generator)
    , _send_count_(0) {}

SocketBase *LocalIOSocket::handle_timed_out() {
  SocketBase *file_socket = NULL;
  _response_              = _response_generator_.new_status_response(
                   HttpStatusCode::S_500_INTERNAL_SERVER_ERROR);
  if (_response_generator_.need_socket()) {
    file_socket = _response_generator_.create_socket(_response_);
  }
  return file_socket;
}

void LocalIOSocket::overwrite_error_response(
    SocketMapActions &socket_map_actions, HttpStatusCode status_code) {
  _response_ = _response_generator_.new_status_response(status_code);
  SocketBase *file_socket = _response_generator_.create_socket(_response_);
  if (file_socket != NULL) {
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
