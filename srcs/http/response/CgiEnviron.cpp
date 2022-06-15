#include "http/response/CgiEnviron.hpp"

#include <limits.h>
#include <stdlib.h>

#include <cstring>

#include "utils/utils.hpp"

static std::string get_realpath(const std::string &path) {
  char *abs_path = realpath(path.c_str(), NULL);
  if (abs_path == NULL) {
    ERROR_EXIT_WITH_ERRNO("get_realpath");
  }
  std::string res = std::string(abs_path);
  free(abs_path);
  return res;
}

static std::map<std::string, std::string>
create_environ_map(const std::string &path, const RequestInfo &request_info) {
  std::map<std::string, std::string> environ_map;

  // 内容は仮
  environ_map["SCRIPT_NAME"]       = path;
  environ_map["SERVER_SOFTWARE"]   = "webserv 0.0.0";

  environ_map["QUERY_STRING"]      = request_info.query_string_;
  environ_map["PATH_INFO"]         = get_realpath(path);
  environ_map["PATH_TRANSLATED"]   = get_realpath(path);
  environ_map["GATEWAY_INTERFACE"] = "CGI/1.1";
  environ_map["SERVER_PROTOCOL"]   = "HTTP/1.1";

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

CgiEnviron::CgiEnviron(const std::string &path,
                       const RequestInfo &request_info) {
  std::map<std::string, std::string> environ_map =
      create_environ_map(path, request_info);
  __environ_ = create_cgi_environ(environ_map);
}

CgiEnviron::~CgiEnviron() {
  for (size_t i = 0; __environ_[i] != NULL; i++) {
    delete __environ_[i];
  }
  delete[] __environ_;
}