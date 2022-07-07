#include "http/response/ResponseGenerator.hpp"

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include "http/response/CgiEnviron.hpp"
#include "utils/utils.hpp"

namespace response_generator {

Result<ResponseGenerator::Content>
create_cgi_content(const RequestInfo &request_info, const Path &target_path) {
  int socket_pair[2] = {0, 0};
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, socket_pair) == -1) {
    ERROR_LOG("error: socketpair");
    return Error<ResponseGenerator::Content>();
  }
  pid_t pid = fork();
  if (pid == -1) {
    ERROR_LOG("error: fork in read_file_to_str_cgi");
    return Error<ResponseGenerator::Content>();
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
      return Error<ResponseGenerator::Content>();
    }
  }
  // parent
  if (fcntl(socket_pair[0], F_SETFL, O_NONBLOCK) == -1) {
    return Error<ResponseGenerator::Content>();
  }
  close(socket_pair[1]);
  ResponseGenerator::Content content;
  content.action_  = ResponseGenerator::Content::CGI;
  content.cgi_pid_ = pid;
  content.fd_      = socket_pair[0];
  return Ok<ResponseGenerator::Content>(content);
}

} // namespace response_generator