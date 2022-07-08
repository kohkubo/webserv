#include "socket/FileWriteSocket.hpp"

#include <poll.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "http/response/ResponseGenerator.hpp"
#include "socket/FileReadSocket.hpp"
#include "socket/SocketMapActions.hpp"
#include "utils/Path.hpp"

namespace ns_socket {

typedef response_generator::ResponseGenerator ResponseGenerator;

struct pollfd FileWriteSocket::pollfd() {
  struct pollfd pfd = {_socket_fd_, POLLIN, 0};
  return pfd;
}

SocketMapActions FileWriteSocket::handle_event(short int revents) {
  LOG("FileWriteSocket: handle_event: socket: " << _socket_fd_);
  (void)revents;
  SocketMapActions socket_map_actions;
  _timeout_.update_last_event();
  std::string &sending_content = _response_generator_.response_info_.content_;
  const char  *rest_str        = sending_content.c_str() + _write_count_;
  size_t       rest_count      = sending_content.size() - _write_count_;
  ssize_t      write_size      = write(_socket_fd_, rest_str, rest_count);
  if (write_size == -1) {
    LOG("write error");
    socket_map_actions.add_action(SocketMapAction::DELETE, _socket_fd_, this);
    _set_error_content(socket_map_actions,
                       HttpStatusCode::S_500_INTERNAL_SERVER_ERROR);
    return socket_map_actions;
  }
  _write_count_ += write_size;
  if (_write_count_ ==
      static_cast<ssize_t>(
          _response_generator_.response_info_.content_.size())) {
    std::string response_message = _response_generator_.create_response_message(
        HttpStatusCode(HttpStatusCode::S_201_CREATED)
            .create_default_content_str());
    _response_.set_response_message_and_sending(response_message);
    socket_map_actions.add_action(SocketMapAction::DELETE, _socket_fd_, this);
  }
  return socket_map_actions;
}

// status_codeはエラーコード前提、つまりActionはREAD or CREATED
void FileWriteSocket::_set_error_content(SocketMapActions &socket_map_actions,
                                         HttpStatusCode    status_code) {
  _response_generator_.update_content(status_code);
  if (_response_generator_.action() == response_generator::ResponseInfo::READ) {
    SocketBase *file_socket =
        new FileReadSocket(_response_, _response_generator_);
    socket_map_actions.add_action(SocketMapAction::INSERT,
                                  file_socket->socket_fd(), file_socket);
  }
  _response_ = _response_generator_.generate_response();
}

} // namespace ns_socket