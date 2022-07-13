#include "socket/CgiSocket.hpp"

#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "http/response/CgiEnviron.hpp"
#include "socket/FileReadSocket.hpp"
#include "socket/SocketMapActions.hpp"

namespace ns_socket {

CgiSocket::CgiSocket(Response &response, ResponseGenerator response_generator)
    : LocalIOSocket(response, response_generator)
    , _timeout_(TIMEOUT_SECONDS_)
    , _response_(response)
    , _response_generator_(response_generator)
    , _is_sending_(response_generator.response_info_.content_.size() != 0)
    , _send_count_(0) {
  if (!_is_sending_) {
    shutdown(_socket_fd_, SHUT_WR);
  }
}

CgiSocket::~CgiSocket() {
  pid_t cgi_process = _response_generator_.response_info_.cgi_pid_;
  int   status      = 0;
  kill(cgi_process, SIGTERM);
  if (waitpid(cgi_process, &status, WNOHANG) == -1) {
    ERROR_LOG("error: waitpid in read_file_to_str_cgi");
  }
  if (WIFEXITED(status)) {
    LOG("child is dead");
    return;
  }
}

struct pollfd CgiSocket::pollfd() {
  struct pollfd pfd = {_socket_fd_, POLLIN, 0};
  if (_is_sending_) {
    pfd.events = POLLIN | POLLOUT;
  }
  return pfd;
}

bool CgiSocket::is_timed_out() { return _timeout_.is_timed_out(); }

SocketMapActions CgiSocket::handle_event(short int revents) {
  LOG("cgi socket handle event");
  SocketMapActions socket_map_actions;
  _timeout_.update_last_event();

  // TODO: POLLHUP 拾えるはず
  if ((revents & POLLIN) != 0) {
    LOG("got POLLIN  event of cgi " << _socket_fd_);
    bool is_close = _handle_receive_event(socket_map_actions);
    if (is_close) {
      // TODO: parse_cgi_response
      std::string response_message =
          _response_generator_.create_response_message(_buffer_.str());
      _response_.set_response_message_and_sending(response_message);
      socket_map_actions.add_action(SocketMapAction::DELETE, _socket_fd_, this);
      return socket_map_actions;
    }
  }
  if ((revents & POLLOUT) != 0) {
    LOG("got POLLOUT event of cgi " << _socket_fd_);
    _handle_send_event(socket_map_actions);
  }
  return socket_map_actions;
}

bool CgiSocket::_handle_receive_event(SocketMapActions &socket_map_actions) {
  ReceiveResult receive_result = receive(_socket_fd_, CGI_BUFFER_SIZE_);
  if (receive_result.rc_ == -1) {
    LOG("write error");
    socket_map_actions.add_action(SocketMapAction::DELETE, _socket_fd_, this);
    overwrite_error_response(socket_map_actions,
                             HttpStatusCode::S_500_INTERNAL_SERVER_ERROR);
    return false;
  }
  if (receive_result.rc_ == 0) {
    // LOG("got FIN from connection");
    return true;
  }
  _buffer_ << receive_result.str_;
  return false;
}

void CgiSocket::_handle_send_event(SocketMapActions &socket_map_actions) {
  std::string &sending_content = _response_generator_.response_info_.content_;
  const char  *rest_str        = sending_content.c_str() + _send_count_;
  size_t       rest_count      = sending_content.size() - _send_count_;
  ssize_t      wc              = write(_socket_fd_, rest_str, rest_count);
  if (wc == -1) {
    LOG("write error");
    socket_map_actions.add_action(SocketMapAction::DELETE, _socket_fd_, this);
    overwrite_error_response(socket_map_actions,
                             HttpStatusCode::S_500_INTERNAL_SERVER_ERROR);
    return;
  }
  _send_count_ += wc;
  if (_send_count_ == static_cast<ssize_t>(sending_content.size())) {
    _is_sending_ = false;
    shutdown(_socket_fd_, SHUT_WR);
  }
}

} // namespace ns_socket
