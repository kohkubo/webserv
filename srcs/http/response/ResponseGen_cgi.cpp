#include "http/response/ResponseGenerator.hpp"

#include <cstring>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <string>

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

static std::map<std::string, std::string>
create_environ_map(const std::string &path, const RequestInfo &request_info) {
  std::map<std::string, std::string> environ_map;

  // 内容は仮
  environ_map["SCRIPT_NAME"]     = path;
  environ_map["SERVER_SOFTWARE"] = "webserv 0.0.0";
  environ_map["QUERY_STRING"]    = request_info.query_string_;

  return environ_map;
}

static char *my_strdup(std::string str) {
  char *res = new char[str.size() + 1];
  strncpy(res, str.c_str(), str.size());
  res[str.size()] = '\0';
  return res;
}

static char **
create_cgi_environ(std::map<std::string, std::string> environ_map) {
  char **cgi_environ = new char *[environ_map.size() + 1];
  std::map<std::string, std::string>::iterator it = environ_map.begin();
  for (std::size_t i = 0; it != environ_map.end(); i++, it++) {
    std::string value = it->first + "=" + it->second;
    cgi_environ[i]    = my_strdup(value);
  }
  cgi_environ[environ_map.size()] = NULL;
  return cgi_environ;
}

static void delete_cgi_environ(char **cgi_environ) {
  for (size_t i = 0; cgi_environ[i] != NULL; i++) {
    delete cgi_environ[i];
  }
  delete[] cgi_environ;
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
    // cgiスクリプトを実行可能ファイルとして扱う
    char *argv[] = {const_cast<char *>(path.c_str()), NULL};
    // TODO: request_info.env_valueはパースせずに標準出力へ
    std::map<std::string, std::string> environ_map =
        create_environ_map(path, request_info);
    char **cgi_environ = create_cgi_environ(environ_map);
    // TODO: execveの前にスクリプトのあるディレクトリに移動
    // error handling
    execve(path.c_str(), argv, cgi_environ);
    delete_cgi_environ(cgi_environ);
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
