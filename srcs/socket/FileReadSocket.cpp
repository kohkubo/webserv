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

SocketBase *FileReadSocket::handle_timed_out() {
  SocketBase *file_socket = NULL;
  _response_generator_.update_response(
      HttpStatusCode::S_500_INTERNAL_SERVER_ERROR);
  if (_response_generator_.need_socket()) {
    file_socket = new FileReadSocket(_response_, _response_generator_);
  }
  _response_ = _response_generator_.generate_response();
  return file_socket;
}

SocketMapActions FileReadSocket::handle_event(short int revents) {
  // LOG("got POLLIN  event of fd " << _socket_fd_);
  (void)revents;
  SocketMapActions socket_map_actions;
  _timeout_.update_last_event();
  char    buf[1024];
  ssize_t read_size = read(_socket_fd_, buf, 1024);
  if (read_size == -1) {
    LOG("read error");
    socket_map_actions.add_action(SocketMapAction::DELETE, _socket_fd_, this);
    _set_error_content(socket_map_actions,
                       HttpStatusCode::S_500_INTERNAL_SERVER_ERROR);
    return socket_map_actions;
  }
  if (read_size == 0) {
    std::string response_message =
        _response_generator_.create_response_message(_buffer_.str());
    // LOG("response message: " << response_message);
    _response_.set_response_message_and_sending(response_message);
    socket_map_actions.add_action(SocketMapAction::DELETE, _socket_fd_, this);
    return socket_map_actions;
  }
  _buffer_ << std::string(buf, read_size);
  // LOG("read_size: " << read_size);
  return socket_map_actions;
}

void FileReadSocket::_set_error_content(SocketMapActions &socket_map_actions,
                                        HttpStatusCode    status_code) {
  _response_generator_.update_response(status_code);
  if (_response_generator_.need_socket()) {
    SocketBase *file_socket =
        new FileReadSocket(_response_, _response_generator_);
    socket_map_actions.add_action(SocketMapAction::INSERT,
                                  file_socket->socket_fd(), file_socket);
  }
  _response_ = _response_generator_.generate_response();
}

} // namespace ns_socket