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
  struct pollfd pfd = {_socket_fd_, POLLOUT, 0};
  return pfd;
}
void FileWriteSocket::handle_event(short int         revents,
                                   SocketMapActions &socket_map_actions) {
  LOG("FileWriteSocket: handle_event: socket: " << _socket_fd_);
  (void)revents;
  _timeout_.update_last_event();

  IOResult io_result = send_content();
  if (io_result == ERROR) {
    socket_map_actions.add_action(SocketMapAction::DELETE, _socket_fd_, this);
    overwrite_error_response(socket_map_actions, 500);
    return;
  }
  if (io_result == SUCCESS) {
    std::string response_message = response_generator::create_response_message(
        _response_generator_.request_info_, _response_generator_.response_info_,
        HttpStatusCode(201).create_default_content_str());
    _response_.set_response_message_and_sending(response_message);
    socket_map_actions.add_action(SocketMapAction::DELETE, _socket_fd_, this);
  }
}

} // namespace ns_socket