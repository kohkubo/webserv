#include "socket/ClientSocket.hpp"

#include <poll.h>
#include <sys/socket.h>

#include "SocketMapActions.hpp"
#include "http/response/ResponseGenerator.hpp"
#include "socket.hpp"
#include "socket/CgiSocket.hpp"
#include "socket/FileReadSocket.hpp"
#include "socket/FileWriteSocket.hpp"

namespace ns_socket {

ClientSocket::ClientSocket(int                        client_fd,
                           const config::ConfigGroup &config_group)
    : SocketBase(client_fd)
    , _config_group_(config_group)
    , _timeout_(TIMEOUT_SECONDS_) {
  struct sockaddr_in peer_addr;
  socklen_t          serv_len = sizeof(peer_addr);
  if (getpeername(_socket_fd_, (struct sockaddr *)&peer_addr, &serv_len) ==
      -1) {
    ERROR_LOG_WITH_ERRNO("getpeername() failed.");
  }
  _peer_name_ = inet_ntoa(peer_addr.sin_addr);
}

struct pollfd ClientSocket::pollfd() {
  struct pollfd pfd = {_socket_fd_, POLLIN, 0};
  if (!_response_queue_.empty() && _response_queue_.front().is_sending()) {
    pfd.events = POLLIN | POLLOUT;
  }
  return pfd;
}

bool ClientSocket::is_timed_out() { return _timeout_.is_timed_out(); }

SocketMapActions ClientSocket::handle_timed_out() {
  SocketMapActions socket_map_actions;
  _add_delete_child_socket(socket_map_actions);
  return socket_map_actions;
}

SocketMapActions ClientSocket::handle_event(short int revents) {
  SocketMapActions socket_map_actions;
  _timeout_.update_last_event();
  if ((revents & (POLLHUP | POLLERR)) != 0) {
    LOG("connection was closed by client.");
    _add_delete_child_socket(socket_map_actions);
    socket_map_actions.add_action(SocketMapAction::DELETE, _socket_fd_, this);
    return socket_map_actions;
  }
  if ((revents & POLLIN) != 0) {
    // LOG("got POLLIN  event of client " << _socket_fd_);
    bool is_close = _handle_receive_event(socket_map_actions);
    if (is_close)
      return socket_map_actions;
  }
  if ((revents & POLLOUT) != 0) {
    // LOG("got POLLOUT event of client " << _socket_fd_);
    _handle_send_event();
  }
  return socket_map_actions;
}

bool ClientSocket::_handle_receive_event(SocketMapActions &socket_map_actions) {
  ReceiveResult receive_result = receive(_socket_fd_, HTTP_BUFFER_SIZE_);
  if (receive_result.rc_ == 0) {
    LOG("got FIN from connection");
    _add_delete_child_socket(socket_map_actions);
    socket_map_actions.add_action(SocketMapAction::DELETE, _socket_fd_, this);
    return true;
  }
  _buffer_ += receive_result.str_;
  _parse_buffer(socket_map_actions);
  return false;
}

void ClientSocket::_handle_send_event() {
  Response::ResponseState response_state =
      _response_queue_.front().send(_socket_fd_);
  if (response_state == Response::COMPLETE) {
    _response_queue_.pop_front();
  }
}

typedef response_generator::ResponseGenerator ResponseGenerator;

void ClientSocket::_parse_buffer(SocketMapActions &socket_map_actions) {
  try {
    for (;;) {
      Request::RequestState request_state =
          _request_.handle_request(_buffer_, _config_group_);
      if (request_state != Request::SUCCESS) {
        break;
      }
      ResponseGenerator response_generator(_request_.request_info(),
                                           _peer_name_);
      _response_queue_.push_back(response_generator.generate_response());
      if (response_generator.need_socket()) {
        SocketBase *socket =
            response_generator.create_socket(_response_queue_.back(), this);
        LOG("add new socket:" << socket->socket_fd());
        socket_map_actions.add_action(SocketMapAction::INSERT,
                                      socket->socket_fd(), socket);
        store_new_child_socket(socket);
      }
      _request_ = Request();
    }
  } catch (const RequestInfo::BadRequestException &e) {
    ResponseGenerator response_generator(_request_.request_info(), _peer_name_,
                                         e.status());
    _response_queue_.push_back(response_generator.generate_response());
    if (response_generator.need_socket()) {
      SocketBase *socket =
          response_generator.create_socket(_response_queue_.back(), this);
      LOG("add new socket:" << socket->socket_fd());
      socket_map_actions.add_action(SocketMapAction::INSERT,
                                    socket->socket_fd(), socket);
      store_new_child_socket(socket);
    }
  }
}

void ClientSocket::_add_delete_child_socket(
    SocketMapActions &socket_map_actions) {
  LOG("reserved deleting all child socket");
  std::set<SocketBase *>::iterator it = _child_socket_set_.begin();
  for (; it != _child_socket_set_.end(); it++) {
    socket_map_actions.add_action(SocketMapAction::DELETE, (*it)->socket_fd(),
                                  *it);
  }
}

void ClientSocket::store_new_child_socket(SocketBase *child_socket) {
  _child_socket_set_.insert(child_socket);
}

void ClientSocket::notify_accomplished(SocketBase *child_socket) {
  _child_socket_set_.erase(child_socket);
}

} // namespace ns_socket