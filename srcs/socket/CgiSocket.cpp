#include "socket/CgiSocket.hpp"

#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "http/response/CgiInfo.hpp"
#include "socket/FileReadSocket.hpp"
#include "socket/SocketMapActions.hpp"

namespace ns_socket {

CgiSocket::CgiSocket(Response &response, ResponseGenerator response_generator,
                     ClientSocket *parent_socket)
    : LocalIOSocket(response, response_generator, parent_socket,
                    TIMEOUT_SECONDS_)
    , _is_sending_(response_generator.response_info_.content_.size() != 0) {
  if (!_is_sending_) {
    if (shutdown(_socket_fd_, SHUT_WR) == -1) {
      ERROR_LOG_WITH_ERRNO("shutdown error");
    }
  }
}

CgiSocket::~CgiSocket() { _kill_cgi_process(); }

struct pollfd CgiSocket::pollfd() {
  struct pollfd pfd = {_socket_fd_, POLLIN, 0};
  if (_is_sending_) {
    pfd.events = POLLIN | POLLOUT;
  }
  return pfd;
}

bool CgiSocket::is_timed_out() { return _timeout_.is_timed_out(); }

SocketMapActions CgiSocket::destroy_timedout_socket() {
  SocketMapActions  socket_map_actions;
  ResponseGenerator new_response_generator =
      _response_generator_.create_response_generator(504);

  _response_ = new_response_generator.generate_response(504);
  if (new_response_generator.need_socket()) {
    SocketBase *file_socket =
        new_response_generator.create_socket(_response_, _parent_socket_);
    _parent_socket_->store_child_socket(file_socket);
    socket_map_actions.add_action(SocketMapAction::INSERT,
                                  file_socket->socket_fd(), file_socket);
  }
  return socket_map_actions;
}

SocketMapActions CgiSocket::handle_event(short int revents) {
  SocketMapActions socket_map_actions;
  _timeout_.update_last_event();

  if ((revents & POLLIN) != 0) {
    LOG("got POLLIN  event of cgi " << _socket_fd_);
    bool is_close = _handle_receive_event(socket_map_actions);
    if (is_close) {
      _create_cgi_response(socket_map_actions);
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
  ReceiveResult receive_result = receive_content(CGI_BUFFER_SIZE_);
  if (receive_result.status_ == ReceiveResult::ERROR) {
    LOG("receive error");
    socket_map_actions.add_action(SocketMapAction::DELETE, _socket_fd_, this);
    overwrite_error_response(socket_map_actions, 500);
    return false;
  }
  if (receive_result.status_ == ReceiveResult::END) {
    LOG("got FIN from connection");
    return true;
  }
  _buffer_ += receive_result.str_;
  _parse_buffer(socket_map_actions);
  return false;
}

void CgiSocket::_handle_send_event(SocketMapActions &socket_map_actions) {
  IOResult io_result = send_content();
  if (io_result == ERROR) {
    socket_map_actions.add_action(SocketMapAction::DELETE, _socket_fd_, this);
    overwrite_error_response(socket_map_actions, 500);
    return;
  }
  if (io_result == SUCCESS) {
    _is_sending_ = false;
    if (shutdown(_socket_fd_, SHUT_WR) == -1) {
      ERROR_LOG_WITH_ERRNO("shutdown error");
    }
  }
}

void CgiSocket::_parse_buffer(SocketMapActions &socket_map_actions) {
  CgiInfo::CgiState cgi_state = _cgi_info_.handle_cgi(_buffer_);
  if (cgi_state == CgiInfo::ERROR) {
    socket_map_actions.add_action(SocketMapAction::DELETE, _socket_fd_, this);
    overwrite_error_response(socket_map_actions, 500);
    return;
  }
  if (cgi_state == CgiInfo::END) {
    _create_cgi_response(socket_map_actions);
  }
}

void CgiSocket::_create_cgi_response(SocketMapActions &socket_map_actions) {
  std::string response_message;
  socket_map_actions.add_action(SocketMapAction::DELETE, _socket_fd_, this);
  switch (_cgi_info_.response_type_) {
  case CgiInfo::DOCUMENT:
  case CgiInfo::CLIENT_REDIR:
  case CgiInfo::CLIENT_REDIRDOC:
    response_message = response_generator::create_response_message(
        _response_generator_.request_info_, _cgi_info_);
    _response_.set_response_message_and_sending(response_message);
    return;
  case CgiInfo::LOCAL_REDIR:
    _redirect_local(socket_map_actions);
    return;
  default:
    break;
  }
}

void CgiSocket::_redirect_local(SocketMapActions &socket_map_actions) {
  ResponseGenerator new_response_generator =
      _response_generator_.create_response_generator(
          _cgi_info_.cgi_location_.path_, _cgi_info_.cgi_location_.query_);

  if (new_response_generator.is_over_max_redirect_count()) {
    ERROR_LOG("too many redirect");
    overwrite_error_response(socket_map_actions, 500);
    return;
  }
  _response_ = new_response_generator.generate_response(500);
  if (new_response_generator.need_socket()) {
    SocketBase *socket =
        new_response_generator.create_socket(_response_, _parent_socket_);
    _parent_socket_->store_child_socket(socket);
    socket_map_actions.add_action(SocketMapAction::INSERT, socket->socket_fd(),
                                  socket);
  }
}

void CgiSocket::_kill_cgi_process() const {
  pid_t cgi_process = _response_generator_.response_info_.cgi_pid_;
  int   status      = 0;
  if (cgi_process == 0) {
    ERROR_LOG("pid is invalid");
    return;
  }
  if (kill(cgi_process, SIGTERM) == -1) {
    ERROR_LOG_WITH_ERRNO("kill");
  }
  if (waitpid(cgi_process, &status, WNOHANG) == -1) {
    ERROR_LOG_WITH_ERRNO("error: waitpid");
  }
  if (WIFEXITED(status)) {
    // LOG("child is dead");
    return;
  }
}

} // namespace ns_socket
