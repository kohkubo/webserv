#include "socket/CgiSocket.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "http/response/CgiEnviron.hpp"
#include "socket/SocketMapActions.hpp"

namespace ns_socket {

CgiSocket::CgiSocket(Response &response, ResponseGenerator response_generator)
    : SocketBase(response_generator.content_.fd_)
    , _timeout_(TIMEOUT_SECONDS_)
    , _response_(response)
    , _response_generator_(response_generator)
    , _send_count_(0) {}

struct pollfd CgiSocket::pollfd() {
  struct pollfd pfd = {_socket_fd_, POLLIN, 0};
  // bodyがあるときかつ、送信済みでないときpollout
  return pfd;
}

bool CgiSocket::is_timed_out() { return _timeout_.is_timed_out(); }

SocketMapActions CgiSocket::handle_event(short int revents) {
  SocketMapActions socket_map_actions;
  _timeout_.update_last_event();

  if ((revents & POLLIN) != 0) {
    // LOG("got POLLIN  event of fd " << _socket_fd_);
    bool is_close = _handle_receive_event();
    if (is_close) {
      // TODO: parse_cgi_response
      _response_.set_response_message_and_sending(_buffer_);
      socket_map_actions.add_socket_map_action(
          SocketMapAction(SocketMapAction::DELETE, _socket_fd_, this));
      if (waitpid(_response_generator_.content_.cgi_pid_, NULL, 0) == -1) {
        ERROR_LOG("error: waitpid in read_file_to_str_cgi");
      }
      return socket_map_actions;
    }
  }
  if ((revents & POLLOUT) != 0) {
    // LOG("got POLLOUT event of fd " << _socket_fd_);
    _handle_send_event();
  }
  return socket_map_actions;
}

bool CgiSocket::_handle_receive_event() {
  ReceiveResult receive_result = receive(_socket_fd_, CGI_BUFFER_SIZE_);
  if (receive_result.rc_ == 0) {
    // LOG("got FIN from connection");
    return true;
  }
  _buffer_ += receive_result.str_;
  return false;
}

// リクエストにbodyがあるとき、bodyを入力。書き込み終了したらshutdown。
void CgiSocket::_handle_send_event() {
  // asita
}

} // namespace ns_socket
