#include "socket/ClientSocket.hpp"

#include <poll.h>

#include "SocketMapActions.hpp"
#include "http/response/ResponseGenerator.hpp"

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
    bool is_close = handle_receive_event(socket_map_actions);
    if (is_close)
      return socket_map_actions;
  }
  if ((revents & POLLOUT) != 0) {
    // LOG("got POLLOUT event of fd " << _socket_fd_);
    handle_send_event();
  }
  return socket_map_actions;
}

bool ClientSocket::handle_receive_event(SocketMapActions &socket_map_actions) {
  bool is_socket_closed_from_client = append_receive_buffer();
  if (is_socket_closed_from_client) {
    // LOG("got FIN from connection");
    socket_map_actions.add_socket_map_action(
        SocketMapAction(SocketMapAction::DELETE, _socket_fd_, this));
    return true;
  }
  parse_buffer(socket_map_actions);
  return false;
}

void ClientSocket::handle_send_event() {
  Response::ResponseState response_state =
      _response_queue_.front().send(_socket_fd_);
  if (response_state == Response::COMPLETE) {
    _response_queue_.pop_front();
  }
}

void ClientSocket::parse_buffer(SocketMapActions &socket_map_actions) {
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
      // ResponseGenerator::Body body = ResponseGenerator::create_fd_or_body(
      //     _request_.request_info(), HttpStatusCode::OK_200);
      // if (body.has_fd()) {
      //   SocketBase *file_socket =
      //       new FileSocket(body.fd_, _response_queue_.back());
      // }
      // if (result.has_read_file_) {
      //  SocketBase *file_socket =
      //      new FileSocket(result.object_, _response_queue_.back());
      //  socket_map_actions.add_socket_map_action(SocketMapAction(
      //      SocketMapAction::INSERT, file_socket->socket_fd(), file_socket));
      //} else {
      ResponseGenerator response_generator(_request_.request_info());
      // cgi or file or nothing
      _response_queue_.push_back(response_generator.generate_response());
      //}
      _request_ = Request();
    }
  } catch (const RequestInfo::BadRequestException &e) {
    ResponseGenerator response_generator(_request_.request_info(),
                                         e.status_code());
    _response_queue_.push_back(response_generator.generate_response());
    _request_ = Request();
  }
}

bool ClientSocket::append_receive_buffer() {
  char    buf[HTTP_BUFFER_SIZE_] = {};
  ssize_t rc = recv(_socket_fd_, buf, HTTP_BUFFER_SIZE_, MSG_DONTWAIT);
  if (rc == -1) {
    ERROR_LOG("recv error.");
    return false;
  }
  // fin from client
  if (rc == 0) {
    return true;
  }
  _buffer_.append(buf, rc);
  return false;
}
