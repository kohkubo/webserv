#include "http/response/Response.hpp"

#include <sys/wait.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <string>

#define READ_FD  0
#define WRITE_FD 1

static std::string read_fd_tostring(int fd) {
  char        buf[1024];
  std::string s;
  while (true) {
    int n = read(fd, buf, sizeof(buf));
    if (n == 0)
      break;
    buf[n] = '\0';
    s += buf;
  }
  return s;
}

std::string
Response::__read_file_tostring_cgi(const std::string              &path,
                                   const std::vector<std::string> &env) {
  int pipefd[2];
  if (pipe(pipefd) == -1) {
    std::cout << "error: pipe in read_file_tostring_cgi" << std::endl;
    return "";
  }
  pid_t pid = fork();
  if (pid == -1) {
    std::cout << "error: fork in read_file_tostring_cgi" << std::endl;
    return "";
  }
  // child
  if (pid == 0) {
    close(pipefd[READ_FD]);
    dup2(pipefd[WRITE_FD], STDOUT_FILENO);
    close(pipefd[WRITE_FD]);
    char *const argv[]           = {(char *)"", (char *)path.c_str(), NULL};
    char *const env_char_array[] = {(char *)env[0].c_str(), NULL};
    // TODO: envを最初の要素だけでなく全て渡す
    // NOTE: vector<string>で持っていると渡し方が難しいかも
    execve("/bin/sh", argv, env_char_array);
    exit(0);
  }
  // parent
  close(pipefd[WRITE_FD]);
  std::string s = read_fd_tostring(pipefd[READ_FD]);
  close(pipefd[READ_FD]);
  if (waitpid(pid, NULL, 0) == -1) {
    std::cout << "error: waitpid in read_file_tostring_cgi" << std::endl;
    return "";
  }
  return s;
}
