#include "http/response/ResponseGenerator.hpp"

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include "http/request/RequestInfo.hpp"
#include "http/response/CgiEnviron.hpp"
#include "http/response/ResponseInfo.hpp"
#include "utils/utils.hpp"

namespace response_generator {

Result<ResponseInfo> create_cgi_content(const RequestInfo &request_info,
                                        const std::string &peer_name,
                                        const Path        &target_path) {
  int socket_pair[2] = {0, 0};
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, socket_pair) == -1) {
    ERROR_LOG_WITH_ERRNO("error: socketpair");
    return Error<ResponseInfo>();
  }
  pid_t pid = fork();
  if (pid == -1) {
    ERROR_LOG_WITH_ERRNO("error: fork");
    return Error<ResponseInfo>();
  }
  // child
  if (pid == 0) {
    if (close(socket_pair[0]) == -1) {
      ERROR_EXIT_WITH_ERRNO("error: close");
    }
    if (dup2(socket_pair[1], STDOUT_FILENO) == -1) {
      ERROR_EXIT_WITH_ERRNO("error: dup2");
    }
    if (dup2(socket_pair[1], STDIN_FILENO) == -1) {
      ERROR_EXIT_WITH_ERRNO("error: dup2");
    }
    if (close(socket_pair[1]) == -1) {
      ERROR_EXIT_WITH_ERRNO("error: close");
    }
    std::string path   = target_path.script_name();
    char       *argv[] = {const_cast<char *>("/usr/bin/python3"),
                          const_cast<char *>(path.c_str()), NULL};
    CgiEnviron  cgi_environ(request_info, peer_name, target_path);
    if (chdir(target_path.dirname().c_str()) == -1) {
      ERROR_EXIT_WITH_ERRNO("error: chdir");
    }
    if (execve("/usr/bin/python3", argv, cgi_environ.environ()) == -1) {
      ERROR_EXIT_WITH_ERRNO("error: execve");
    }
  }
  // parent
  if (close(socket_pair[1]) == -1) {
    ERROR_LOG_WITH_ERRNO("error: close");
    kill(pid, SIGTERM);
    return Error<ResponseInfo>();
  }
  return Ok<ResponseInfo>(
      ResponseInfo(ResponseInfo::CGI, request_info.content_info_.content_,
                   HttpStatusCode::S_200_OK, socket_pair[0], pid));
}

} // namespace response_generator
