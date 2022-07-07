#include "socket/FileWriteSocket.hpp"

#include <poll.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "http/response/ResponseGenerator.hpp"
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
  // TODO: 一定数を書き込むようにする 3人
  ssize_t write_size =
      write(_socket_fd_, _response_generator_.content_.content_.c_str(),
            _response_generator_.content_.content_.size());
  if (write_size == -1) {
    LOG("write error");
    socket_map_actions.add_action(SocketMapAction::DELETE, _socket_fd_, this);
    return socket_map_actions;
  }
  if (write_size != 0) {
    std::string response_message = _response_generator_.create_response_message(
        HttpStatusCode(HttpStatusCode::S_201_CREATED)
            .create_default_content_str());
    LOG("response message: " << response_message);
    _response_.set_response_message_and_sending(response_message);
    socket_map_actions.add_action(SocketMapAction::DELETE, _socket_fd_, this);
    return socket_map_actions;
  }
  LOG("write_size: " << write_size);
  return socket_map_actions;
}

} // namespace ns_socket