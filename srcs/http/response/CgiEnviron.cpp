#include "http/response/CgiEnviron.hpp"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include <cstring>

#include "utils/utils.hpp"

static std::string get_realpath(const std::string &path) {
  char *abs_path = realpath(path.c_str(), NULL);
  if (abs_path == NULL) {
    // TODO: ここはexitで大丈夫？？ kohkubo
    ERROR_EXIT_WITH_ERRNO("get_realpath");
  }
  std::string res = std::string(abs_path);
  free(abs_path);
  return res;
}

static environMap create_environ_map(const std::string &path,
                                     const RequestInfo &request_info) {
  environMap environ_map;

  if (request_info.has_body()) {
    environ_map["CONTENT_LENGTH"] = to_string(request_info.body_.size());
    environ_map["CONTENT_TYPE"]   = request_info.content_type_.type_;
  }
  environ_map["GATEWAY_INTERFACE"] = "CGI/1.1";
  environ_map["PATH_INFO"]         = get_realpath(path);
  environ_map["PATH_TRANSLATED"]   = get_realpath(path);
  environ_map["QUERY_STRING"]      = request_info.query_string_;
  environ_map["REQUEST_METHOD"]    = request_info.method_;
  environ_map["SERVER_PROTOCOL"]   = "HTTP/1.1";
  environ_map["SERVER_SOFTWARE"]   = "webserv 0.0.0";

  // TODO: addrinfoからアドレスを文字列に変換
  environ_map["REMOTE_ADDR"]       = "";

  // TODO: request_targetからファイルのパスだけ取る。
  // ex) target: /hoge/test.py script_name: /test.py
  environ_map["SCRIPT_NAME"]       = request_info.request_target_;

  // TODO: addrinfoからポートを変換
  environ_map["SERVER_PORT"]       = "";

  // なくても良さそうなもの
  // environ_map["AUTH_TYPE"] = "";
  // environ_map["REMOTE_HOST"]       = "";
  // environ_map["REMOTE_IDENT"]      = "";
  // environ_map["REMOTE_USER"]      = "";

  return environ_map;
}

static char **create_cgi_environ(const environMap &environ_map) {
  char               **cgi_environ = new char *[environ_map.size() + 1];
  environMap::const_iterator it          = environ_map.begin();
  for (std::size_t i = 0; it != environ_map.end(); i++, it++) {
    std::string value = it->first + "=" + it->second;
    cgi_environ[i]    = ::strdup(value.c_str());
  }
  cgi_environ[environ_map.size()] = NULL;
  return cgi_environ;
}

CgiEnviron::CgiEnviron(const std::string &path,
                       const RequestInfo &request_info) {
  environMap environ_map =
      create_environ_map(path, request_info);
  __environ_ = create_cgi_environ(environ_map);
}

CgiEnviron::~CgiEnviron() {
  for (size_t i = 0; __environ_[i] != NULL; i++) {
    delete __environ_[i];
  }
  delete[] __environ_;
}