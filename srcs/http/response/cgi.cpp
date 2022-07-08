#include "http/response/ResponseGenerator.hpp"

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include "http/response/CgiEnviron.hpp"
#include "http/response/ResponseInfo.hpp"
#include "utils/utils.hpp"

namespace response_generator {

Result<ResponseInfo> create_cgi_content(const RequestInfo &request_info,
                                        const Path        &target_path) {
  int socket_pair[2] = {0, 0};
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, socket_pair) == -1) {
    ERROR_LOG("error: socketpair");
    return Error<ResponseInfo>();
  }
  pid_t pid = fork();
  if (pid == -1) {
    ERROR_LOG("error: fork in read_file_to_str_cgi");
    return Error<ResponseInfo>();
  }
  // child
  if (pid == 0) {
    close(socket_pair[0]);
    // TODO: error handling
    dup2(socket_pair[1], STDOUT_FILENO);
    dup2(socket_pair[1], STDIN_FILENO);
    close(socket_pair[1]);
    char      *argv[] = {const_cast<char *>("/usr/bin/python3"),
                    const_cast<char *>(target_path.str().c_str()), NULL};
    CgiEnviron cgi_environ(request_info, target_path);
    // TODO: execveの前にスクリプトのあるディレクトリに移動
    if (execve("/usr/bin/python3", argv, cgi_environ.environ()) == -1) {
      ERROR_LOG("error: execve");
      return Error<ResponseInfo>();
    }
  }
  // parent
  if (fcntl(socket_pair[0], F_SETFL, O_NONBLOCK) == -1) {
    return Error<ResponseInfo>();
  }
  close(socket_pair[1]);
  return Ok<ResponseInfo>(CgiContent(socket_pair[0], pid));
}

} // namespace response_generator