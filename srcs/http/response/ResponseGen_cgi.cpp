#include "http/response/ResponseGenerator.hpp"

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

static char *const *vector_to_array(const std::vector<std::string> &v) {
  char **a = new char *[v.size() + 1];
  for (size_t i = 0; i < v.size(); ++i)
    a[i] = const_cast<char *>(v[i].c_str());
  a[v.size()] = NULL;
  return a;
}

// TODO: error処理
std::string ResponseGenerator::__read_file_tostring_cgi(
    const std::string &path, const std::vector<std::string> &env) {
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
    char *const  argv[]         = {const_cast<char *>(""),
                          const_cast<char *>(path.c_str()), NULL};
    char *const *env_char_array = vector_to_array(env);
    execve("/bin/sh", argv, env_char_array);
    delete[] env_char_array;
    exit(0);
  }
  // parent
  close(pipefd[WRITE_FD]);
  std::string s = read_fd_tostring(pipefd[READ_FD]);
  close(pipefd[READ_FD]);
  if (waitpid(pid, NULL, 0) == -1) {
    ERROR_LOG("error: waitpid in read_file_tostring_cgi");
    return "";
  }
  return s;
}
