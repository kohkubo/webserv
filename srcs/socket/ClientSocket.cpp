#include "socket/ClientSocket.hpp"

#include <poll.h>

#include "SocketMapActions.hpp"
#include "http/response/ResponseGenerator.hpp"
#include "socket.hpp"

namespace socket_base {

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
  _timeout_.update_last_event();
  if ((revents & (POLLHUP | POLLERR)) != 0) {
    LOG("connection was closed by client.");
    _socket_map_actions_.add_socket_map_action(
        SocketMapAction(SocketMapAction::DELETE, _socket_fd_, this));
    return _socket_map_actions_;
  }
  if ((revents & POLLIN) != 0) {
    // LOG("got POLLIN  event of fd " << _socket_fd_);
    bool is_close = handle_receive_event();
    if (is_close)
      return _socket_map_actions_;
  }
  if ((revents & POLLOUT) != 0) {
    // LOG("got POLLOUT event of fd " << _socket_fd_);
    handle_send_event();
  }
  return _socket_map_actions_;
}

bool ClientSocket::handle_receive_event() {
  Result<std::string> result = receive(_socket_fd_, HTTP_BUFFER_SIZE_);
  if (result.is_err_) {
    // LOG("got FIN from connection");
    _socket_map_actions_.add_socket_map_action(
        SocketMapAction(SocketMapAction::DELETE, _socket_fd_, this));
    return true;
  }
  _buffer_ += result.object_;
  parse_buffer(_buffer_);
  return false;
}

void ClientSocket::handle_send_event() {
  Response::FdState response_state = _response_queue_.front().send(_socket_fd_);
  if (response_state == Response::COMPLETE) {
    _response_queue_.pop_front();
  }
}

void ClientSocket::parse_buffer(std::string &buffer) {
  try {
    for (;;) {
      Request::RequestState request_state =
          _request_.handle_request(buffer, _config_group_);
      if (request_state != Request::SUCCESS) {
        break;
      }
      // fileを読むか、handle_methodやるところまで -> http status code
      // 結果Result<openされたfd>が返ってくる。
      // ResponseGenerator::Body body = ResponseGenerator::create_fd_or_body(
      //     _request_.request_info(), HttpStatusCode::S_200_OK);
      // if (body.has_fd()) {
      //   SocketBase *file_socket =
      //       new FileSocket(body.fd_, _response_queue_.back());
      // }
      // if (result.has_read_file_) {
      //  SocketBase *file_socket =
      //      new FileSocket(result.object_, _response_queue_.back());
      //  _socket_map_actions_.add_socket_map_action(SocketMapAction(
      //      SocketMapAction::INSERT, file_socket->socket_fd(), file_socket));
      //} else {
      response_generator::ResponseGenerator response_generator(
          _request_.request_info());
      _response_queue_.push_back(response_generator.generate_response());
      // if (response_generator.has_fd()) {
      //   SocketBase *file_socket =
      //       new FileSocket(response_generator.fd(), _response_queue_.back());
      // }
      // cgi or file or nothing
      //}
      _request_ = Request();
    }
  } catch (const RequestInfo::BadRequestException &e) {
    response_generator::ResponseGenerator response_generator(
        _request_.request_info(), e.status());
    _response_queue_.push_back(response_generator.generate_response());
    _request_ = Request();
  }
}

} // namespace socket_base