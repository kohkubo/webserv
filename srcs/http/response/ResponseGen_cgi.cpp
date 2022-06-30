#include "http/response/ResponseGenerator.hpp"

#include <sys/wait.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include "http/response/CgiEnviron.hpp"
#include "utils/file_io_utils.hpp"
#include "utils/utils.hpp"

namespace response_generator {

#define READ_FD  0
#define WRITE_FD 1

static std::string read_fd_to_str(int fd) {
  char        buf[1024 + 1];
  std::string s;
  for (;;) {
    int n = read(fd, buf, 1024);
    if (n == 0)
      break;
    buf[n] = '\0';
    s += buf;
  }
  return s;
}

// TODO: error処理
Result<std::string>
ResponseGenerator::_read_file_to_str_cgi(const RequestInfo &request_info,
                                         const std::string &target_path) {
  int pipefd[2] = {0, 0};
  if (pipe(pipefd) == -1) {
    ERROR_LOG("error: pipe in read_file_to_str_cgi");
    return Error<std::string>();
  }
  pid_t pid = fork();
  if (pid == -1) {
    ERROR_LOG("error: fork in read_file_to_str_cgi");
    return Error<std::string>();
  }
  // child
  if (pid == 0) {
    close(pipefd[READ_FD]);
    dup2(pipefd[WRITE_FD], STDOUT_FILENO);
    close(pipefd[WRITE_FD]);
    char      *argv[] = {const_cast<char *>("/usr/bin/python3"),
                    const_cast<char *>(target_path.c_str()), NULL};
    CgiEnviron cgi_environ(request_info, target_path);
    // TODO: request_info.body_はパースせずに標準出力へ
    // TODO: execveの前にスクリプトのあるディレクトリに移動
    // TODO: timeout
    // error handling
    execve("/usr/bin/python3", argv, cgi_environ.environ());
    exit(0);
  }
  // parent
  close(pipefd[WRITE_FD]);
  // TODO: cgiからのレスポンスは、ヘッダー＋レスポンスbody、要パース
  // local redirectどうするか
  std::string str = read_fd_to_str(pipefd[READ_FD]);
  // LOG("######################################");
  // LOG(str);
  // LOG("######################################");

  close(pipefd[READ_FD]);
  if (waitpid(pid, NULL, 0) == -1) {
    ERROR_LOG("error: waitpid in read_file_to_str_cgi");
    return Error<std::string>();
  }
  return Ok<std::string>(str);
}

} // namespace response_generator