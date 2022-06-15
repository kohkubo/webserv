#include "http/response/ResponseGenerator.hpp"

#include <sys/wait.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include "http/response/CgiEnviron.hpp"
#include "utils/utils.hpp"

#define READ_FD  0
#define WRITE_FD 1

static std::string read_fd_tostring(int fd) {
  char        buf[1024];
  std::string s;
  for (;;) {
    int n = read(fd, buf, sizeof(buf));
    if (n == 0)
      break;
    buf[n] = '\0';
    s += buf;
  }
  return s;
}

// TODO: error処理
std::string
ResponseGenerator::__read_file_tostring_cgi(const std::string &path,
                                            const RequestInfo &request_info) {
  int pipefd[2] = {0, 0};
  if (pipe(pipefd) == -1) {
    ERROR_LOG("error: pipe in read_file_tostring_cgi");
    return "";
  }
  pid_t pid = fork();
  if (pid == -1) {
    ERROR_LOG("error: fork in read_file_tostring_cgi");
    return "";
  }
  // child
  if (pid == 0) {
    close(pipefd[READ_FD]);
    dup2(pipefd[WRITE_FD], STDOUT_FILENO);
    close(pipefd[WRITE_FD]);
    char      *argv[] = {const_cast<char *>("/usr/bin/python3"),
                    const_cast<char *>(path.c_str()), NULL};
    CgiEnviron cgi_environ(path, request_info);
    // TODO: request_info.env_valueはパースせずに標準出力へ
    // TODO: execveの前にスクリプトのあるディレクトリに移動
    // TODO: timeout
    // error handling
    execve("/usr/bin/python3", argv, cgi_environ.data());
    exit(0);
  }
  // parent
  close(pipefd[WRITE_FD]);
  // TODO: cgiからのレスポンスは、ヘッダー＋レスポンスbody、要パース
  std::string s = read_fd_tostring(pipefd[READ_FD]);
  close(pipefd[READ_FD]);
  if (waitpid(pid, NULL, 0) == -1) {
    ERROR_LOG("error: waitpid in read_file_tostring_cgi");
    return "";
  }
  return s;
}
