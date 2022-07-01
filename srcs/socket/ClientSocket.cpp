#include "socket/ClientSocket.hpp"

#include <poll.h>

#include "SocketMapActions.hpp"
#include "http/response/ResponseGenerator.hpp"
#include "socket.hpp"
#include "socket/FileReadSocket.hpp"

namespace ns_socket {

ClientSocket::ClientSocket(int client_fd, const ConfigGroup &config_group)
    : SocketBase(client_fd)
    , _config_group_(config_group)
    , _timeout_(TIMEOUT_SECONDS_) {}

struct pollfd ClientSocket::pollfd() {
  struct pollfd pfd = {_socket_fd_, POLLIN, 0};
  if (!_response_queue_.empty() && _response_queue_.front().is_sending()) {
    pfd.events = POLLIN | POLLOUT;
  }
  return pfd;
}

bool ClientSocket::is_timed_out() { return _timeout_.is_timed_out(); }

// SocketMapAction* linked list
// std::vector<SocketMapAction> SocketMapActions.add(socket_map_action)

SocketMapActions ClientSocket::handle_event(short int revents) {
  SocketMapActions socket_map_actions;
  _timeout_.update_last_event();
  if ((revents & (POLLHUP | POLLERR)) != 0) {
    LOG("connection was closed by client.");
    socket_map_actions.add_socket_map_action(
        SocketMapAction(SocketMapAction::DELETE, _socket_fd_, this));
    return socket_map_actions;
  }
  if ((revents & POLLIN) != 0) {
    // LOG("got POLLIN  event of fd " << _socket_fd_);
    bool is_close = _handle_receive_event(socket_map_actions);
    if (is_close)
      return socket_map_actions;
  }
  if ((revents & POLLOUT) != 0) {
    // LOG("got POLLOUT event of fd " << _socket_fd_);
    _handle_send_event();
  }
  return socket_map_actions;
}

bool ClientSocket::_handle_receive_event(SocketMapActions &socket_map_actions) {
  ReceiveResult receive_result = receive(_socket_fd_, HTTP_BUFFER_SIZE_);
  if (receive_result.rc_ == 0) {
    // LOG("got FIN from connection");
    socket_map_actions.add_socket_map_action(
        SocketMapAction(SocketMapAction::DELETE, _socket_fd_, this));
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
  (void)socket_map_actions;
  try {
    for (;;) {
      Request::RequestState request_state =
          _request_.handle_request(_buffer_, _config_group_);
      if (request_state != Request::SUCCESS) {
        break;
      }
      // fileを読むか、handle_methodやるところまで -> http status code
      // 結果Result<openされたfd>が返ってくる。
      // ResponseGenerator::Content body =
      // ResponseGenerator::create_fd_or_body(
      //     _request_.request_info(), HttpStatusCode::S_200_OK);
      // if (body.has_fd()) {
      //   SocketBase *file_socket =
      //       new FileReadSocket(body.fd_, _response_queue_.back());
      // }
      // if (result.has_read_file_) {
      //  SocketBase *file_socket =
      //      new FileReadSocket(result.object_, _response_queue_.back());
      //  socket_map_actions.add_socket_map_action(SocketMapAction(
      //      SocketMapAction::INSERT, file_socket->socket_fd(), file_socket));
      //} else {
      ResponseGenerator response_generator(_request_.request_info());
      _response_queue_.push_back(response_generator.generate_response());
      if (response_generator.content_state() ==
          ResponseGenerator::Content::READ) {
        SocketBase *file_socket =
            new FileReadSocket(_response_queue_.back(), response_generator);
        socket_map_actions.add_socket_map_action(SocketMapAction(
            SocketMapAction::INSERT, file_socket->socket_fd(), file_socket));
      }
      _request_ = Request();
    }
  } catch (const RequestInfo::BadRequestException &e) {
    // TODO: Fdを開く部分が書けていない
    ResponseGenerator response_generator(_request_.request_info(), e.status());
    _response_queue_.push_back(response_generator.generate_response());
    _request_ = Request();
  }
}

} // namespace ns_socket