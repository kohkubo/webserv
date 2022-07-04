#include "socket/CgiSocket.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "http/response/CgiEnviron.hpp"
#include "socket/SocketMapActions.hpp"

// TODO: cgiからのレスポンスは、ヘッダー＋レスポンスbody、要パース
// local redirectどうするか

namespace ns_socket {

#define READ_FD  0
#define WRITE_FD 1

CgiSocket::CgiSocket(Response &response, ResponseGenerator response_generator)
    : _timeout_(TIMEOUT_SECONDS_)
    , _response_(response)
    , _response_generator_(response_generator) {
  // やりたいことsocketpairを作る。片側をsocket_fdに、もう片方を子プロセスのstdin,stdoutにmapping
  int socket_pair[2] = {0, 0};
  if (socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0, socket_pair) == -1) {
    ERROR_LOG("error: socketpair");
    // set error response;
    return;
  }
  pid_t pid = fork();
  if (pid == -1) {
    ERROR_LOG("error: fork");
    // set error response;
    return;
  }
  // child
  if (pid == 0) {
    close(socket_pair[READ_FD]);
    dup2(socket_pair[WRITE_FD], STDOUT_FILENO);
    close(socket_pair[WRITE_FD]);
    char      *argv[] = {const_cast<char *>("/usr/bin/python3"),
                    const_cast<char *>(
                        response_generator.content_.target_path_.str().c_str()),
                    NULL};
    CgiEnviron cgi_environ(response_generator.request_info,
                           response_generator.content_.target_path_);
    // TODO: execveの前にスクリプトのあるディレクトリに移動
    // error handling
    if (execve("/usr/bin/python3", argv, cgi_environ.environ()) == -1) {
      ERROR_LOG("error: execve");
      // TODO: can't not set error response
      return;
    }
  }

  // _socket_fd_ = cgi_socket_fd;
}

struct pollfd CgiSocket::pollfd() {
  struct pollfd pfd = {_socket_fd_, POLLIN, 0};
  return pfd;
}

bool CgiSocket::is_timed_out() { return _timeout_.is_timed_out(); }

SocketMapActions CgiSocket::handle_event(short int revents) {
  SocketMapActions socket_map_actions;
  _timeout_.update_last_event();

  if ((revents & POLLIN) != 0) {
    // LOG("got POLLIN  event of fd " << _socket_fd_);
    // bool is_close = _handle_receive_event(socket_map_actions);
    // if (is_close)
    //   return socket_map_actions;
  }

  // リクエストにbodyがあるとき、bodyを入力。書き込み終了したらshutdown。
  if ((revents & POLLOUT) != 0) {
    // LOG("got POLLOUT event of fd " << _socket_fd_);
    // _handle_send_event();
  }
  // POLLIN
  // close(pipefd[WRITE_FD]);
  // std::string str = read_fd_to_str(pipefd[READ_FD]);

  // close(pipefd[READ_FD]);
  // if (waitpid(pid, NULL, 0) == -1) {
  //   ERROR_LOG("error: waitpid in read_file_to_str_cgi");
  //   return Error<std::string>();
  // }
  // return Ok<std::string>(str);
  return socket_map_actions;
}

} // namespace ns_socket
