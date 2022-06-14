#include "http/response/ResponseGenerator.hpp"

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

static char *const *
create_cgi_environ(std::map<std::string, std::string> environ_map) {
  char **cgi_environ = new char *[environ_map.size() + 1];
  std::map<std::string, std::string>::iterator it = environ_map.begin();
  for (std::size_t i = 0; it != environ_map.end(); i++, it++) {
    // newしたポインタ消失してるので要修正
    std::string *value = new std::string(it->first + "=" + it->second);
    cgi_environ[i]     = const_cast<char *>(value->c_str());
  }
  cgi_environ[environ_map.size()] = NULL;
  return cgi_environ;
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
    char *const argv[] = {const_cast<char *>(path.c_str()), NULL};
    // TODO: request_info.env_valueはパースせずに標準出力へ
    std::map<std::string, std::string> environ_map =
        create_environ_map(path, request_info);
    char *const *cgi_environ = create_cgi_environ(environ_map);
    // TODO: execveの前にスクリプトのあるディレクトリに移動
    execve(path.c_str(), argv, cgi_environ);
    delete[] cgi_environ;
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
