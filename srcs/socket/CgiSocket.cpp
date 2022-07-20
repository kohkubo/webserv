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
    : LocalIOSocket(response, response_generator, parent_socket)
    , _timeout_(TIMEOUT_SECONDS_)
    , _response_(response)
    , _response_generator_(response_generator)
    , _is_sending_(response_generator.response_info_.content_.size() != 0)
    , _send_count_(0) {
  if (!_is_sending_) {
    shutdown(_socket_fd_, SHUT_WR);
  }
}

CgiSocket::~CgiSocket() {}

struct pollfd CgiSocket::pollfd() {
  struct pollfd pfd = {_socket_fd_, POLLIN, 0};
  if (_is_sending_) {
    pfd.events = POLLIN | POLLOUT;
  }
  return pfd;
}

bool CgiSocket::is_timed_out() { return _timeout_.is_timed_out(); }

SocketBase *CgiSocket::handle_timed_out() {
  _kill_cgi_process();
  SocketBase *file_socket = NULL;
  _response_              = _response_generator_.new_status_response(504);
  if (_response_generator_.need_socket()) {
    file_socket =
        _response_generator_.create_socket(_response_, _parent_socket_);
  }
  // TODO: exchange new child
  return file_socket;
}

SocketMapActions CgiSocket::handle_event(short int revents) {
  SocketMapActions socket_map_actions;
  _timeout_.update_last_event();

  if ((revents & POLLIN) != 0) {
    LOG("got POLLIN  event of cgi " << _socket_fd_);
    bool is_close = _handle_receive_event(socket_map_actions);
    if (is_close) {
      _kill_cgi_process();
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
  ReceiveResult receive_result = receive(_socket_fd_, CGI_BUFFER_SIZE_);
  if (receive_result.rc_ == -1) {
    LOG("receive error");
    _kill_cgi_process();
    socket_map_actions.add_action(SocketMapAction::DELETE, _socket_fd_, this);
    overwrite_error_response(socket_map_actions, 500);
    return false;
  }
  if (receive_result.rc_ == 0) {
    LOG("got FIN from connection");
    return true;
  }
  _buffer_ += receive_result.str_;
  _parse_buffer(socket_map_actions);
  return false;
}

void CgiSocket::_handle_send_event(SocketMapActions &socket_map_actions) {
  std::string &sending_content = _response_generator_.response_info_.content_;
  const char  *rest_str        = sending_content.c_str() + _send_count_;
  size_t       rest_count      = sending_content.size() - _send_count_;
  ssize_t      wc              = write(_socket_fd_, rest_str, rest_count);
  if (wc == -1) {
    LOG("write error");
    _kill_cgi_process();
    socket_map_actions.add_action(SocketMapAction::DELETE, _socket_fd_, this);
    overwrite_error_response(socket_map_actions, 500);
    return;
  }
  _send_count_ += wc;
  if (_send_count_ == static_cast<ssize_t>(sending_content.size())) {
    _is_sending_ = false;
    shutdown(_socket_fd_, SHUT_WR);
  }
}

void CgiSocket::_parse_buffer(SocketMapActions &socket_map_actions) {
  CgiInfo::CgiState cgi_state = _cgi_info_.handle_cgi(_buffer_);
  if (cgi_state == CgiInfo::ERROR) {
    _kill_cgi_process();
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
    response_message = _response_generator_.create_response_message(_cgi_info_);
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
  RequestInfo request_info = _response_generator_.request_info_;
  request_info.request_line_.absolute_path_ = _cgi_info_.cgi_location_.path_;
  request_info.request_line_.query_         = _cgi_info_.cgi_location_.query_;

  ResponseGenerator response_generator(request_info,
                                       _response_generator_.peer_name_);
  _response_ = response_generator.generate_response();
  if (response_generator.need_socket()) {
    SocketBase *socket =
        response_generator.create_socket(_response_, _parent_socket_);
    socket_map_actions.add_action(SocketMapAction::INSERT, socket->socket_fd(),
                                  socket);
  }
  // TODO: exchange child
}

void CgiSocket::_kill_cgi_process() const {
  pid_t cgi_process = _response_generator_.response_info_.cgi_pid_;
  int   status      = 0;
  if (kill(cgi_process, SIGTERM) == -1) {
    ERROR_LOG_WITH_ERRNO("kill");
  }
  if (waitpid(cgi_process, &status, WNOHANG) == -1) {
    ERROR_LOG("error: waitpid");
  }
  if (WIFEXITED(status)) {
    // LOG("child is dead");
    return;
  }
}

} // namespace ns_socket
