#include "socket/ClientSocket.hpp"

#include <poll.h>
#include <sys/socket.h>

#include "SocketMapActions.hpp"
#include "http/response/ResponseGenerator.hpp"
#include "socket/CgiSocket.hpp"
#include "socket/FileReadSocket.hpp"
#include "socket/FileWriteSocket.hpp"

namespace ns_socket {

ClientSocket::ClientSocket(int                        client_fd,
                           const config::ConfigGroup &config_group)
    : SocketBase(client_fd, TIMEOUT_SECONDS_)
    , _config_group_(config_group) {
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
  if (!_transaction_queue_.empty() &&
      _transaction_queue_.front().response_.is_sending()) {
    pfd.events = POLLIN | POLLOUT;
  }
  return pfd;
}

bool ClientSocket::is_timed_out() { return _timeout_.is_timed_out(); }

SocketMapActions ClientSocket::destroy_timedout_socket() {
  SocketMapActions socket_map_actions;
  _add_delete_child_socket(socket_map_actions);
  return socket_map_actions;
}

SocketMapActions ClientSocket::handle_event(short int revents) {
  SocketMapActions socket_map_actions;
  _timeout_.update_last_event();
  if ((revents & (POLLHUP | POLLERR)) != 0) {
    // LOG("connection was closed by client.");
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
    _handle_send_event(socket_map_actions);
  }
  return socket_map_actions;
}

bool ClientSocket::_handle_receive_event(SocketMapActions &socket_map_actions) {
  ReceiveResult receive_result = receive_content(HTTP_BUFFER_SIZE_);
  if (receive_result.status_ == ReceiveResult::ERROR ||
      receive_result.status_ == ReceiveResult::END) {
    LOG("got FIN from connection");
    _add_delete_child_socket(socket_map_actions);
    socket_map_actions.add_action(SocketMapAction::DELETE, _socket_fd_, this);
    return true;
  }
  _buffer_ += receive_result.str_;
  _parse_buffer(socket_map_actions);
  return false;
}

void ClientSocket::_handle_send_event(SocketMapActions &socket_map_actions) {
  Response::ResponseState response_state =
      _transaction_queue_.front().response_.send(_socket_fd_);
  if (response_state == Response::ERROR) {
    _add_delete_child_socket(socket_map_actions);
    socket_map_actions.add_action(SocketMapAction::DELETE, _socket_fd_, this);
    return;
  }
  if (response_state == Response::COMPLETE) {
    _transaction_queue_.pop_front();
  }
}

typedef response_generator::ResponseGenerator ResponseGenerator;

void ClientSocket::_parse_buffer(SocketMapActions &socket_map_actions) {
  try {
    for (;;) {
      RequestHandler::RequestState request_state =
          _request_handler_.handle_request(_buffer_, _config_group_);
      if (request_state != RequestHandler::SUCCESS) {
        break;
      }
      _transaction_queue_.push_back(
          Transaction(_request_handler_.request_info()));
      ResponseGenerator response_generator(
          _transaction_queue_.back().request_info_, _peer_name_,
          HttpStatusCode::S_200_OK);
      _transaction_queue_.back().response_ =
          response_generator.generate_response();
      if (response_generator.need_socket()) {
        SocketBase *socket = response_generator.create_socket(
            _transaction_queue_.back().response_, this);
        // LOG("add new socket:" << socket->socket_fd());
        socket_map_actions.add_action(SocketMapAction::INSERT,
                                      socket->socket_fd(), socket);
        store_child_socket(socket);
      }
      _request_handler_ = RequestHandler();
    }
  } catch (const RequestInfo::BadRequestException &e) {
    LOG("bad request: " << e.status());
    _transaction_queue_.push_back(
        Transaction(_request_handler_.request_info()));
    ResponseGenerator response_generator(
        _transaction_queue_.back().request_info_, _peer_name_, e.status());
    _transaction_queue_.back().response_ =
        response_generator.generate_response();
    if (response_generator.need_socket()) {
      SocketBase *socket = response_generator.create_socket(
          _transaction_queue_.back().response_, this);
      // LOG("add new socket:" << socket->socket_fd());
      socket_map_actions.add_action(SocketMapAction::INSERT,
                                    socket->socket_fd(), socket);
      store_child_socket(socket);
    }
  }
}

void ClientSocket::_add_delete_child_socket(
    SocketMapActions &socket_map_actions) {
  // LOG("reserved deleting all child socket");
  std::set<SocketBase *>::iterator it = _child_socket_set_.begin();
  for (; it != _child_socket_set_.end(); it++) {
    socket_map_actions.add_action(SocketMapAction::DELETE, (*it)->socket_fd(),
                                  *it);
  }
}

void ClientSocket::store_child_socket(SocketBase *child_socket) {
  _child_socket_set_.insert(child_socket);
}

void ClientSocket::notify_accomplished(SocketBase *child_socket) {
  _child_socket_set_.erase(child_socket);
}

} // namespace ns_socket