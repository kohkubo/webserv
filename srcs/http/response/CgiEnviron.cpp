#include "http/response/CgiEnviron.hpp"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include <cstring>
#include <map>

#include "utils/utils.hpp"

static std::string get_realpath(const std::string &file_path) {
  char *abs_path = realpath(file_path.c_str(), NULL);
  if (abs_path == NULL) {
    // TODO: ここはexitで大丈夫？？ kohkubo
    ERROR_EXIT_WITH_ERRNO("get_realpath");
  }
  std::string res = std::string(abs_path);
  free(abs_path);
  return res;
}

static std::string get_script_name(const std::string &absolute_path) {
  std::size_t last_slash  = absolute_path.find_last_of('/');
  std::string script_name = absolute_path.substr(last_slash);
  return script_name;
}

static std::map<std::string, std::string>
create_environ_map(const RequestInfo &request_info) {
  std::map<std::string, std::string> environ_map;

  if (request_info.has_body()) {
    environ_map["CONTENT_LENGTH"] = to_string(request_info.body_.size());
    environ_map["CONTENT_TYPE"]   = request_info.content_type_;
  }
  environ_map["GATEWAY_INTERFACE"] = "CGI/1.1";
  environ_map["PATH_INFO"]         = get_realpath(request_info.target_path_);
  environ_map["PATH_TRANSLATED"]   = get_realpath(request_info.target_path_);
  // クライアントのネットワークアドレス sockfdからgetpeername
  // clientSocketからsockfd渡す
  environ_map["REMOTE_ADDR"]       = "";
  // REMOTE_ADDRを名前解決
  environ_map["REMOTE_HOST"]       = "";
  environ_map["QUERY_STRING"]      = request_info.request_line_.query_;
  environ_map["REQUEST_METHOD"]    = request_info.request_line_.method_;
  environ_map["SCRIPT_NAME"] =
      get_script_name(request_info.request_line_.absolute_path_);
  environ_map["SERVER_NAME"] = request_info.config_.server_name_;
  environ_map["SERVER_PORT"] =
      to_string(ntohs(request_info.config_.sockaddr_in_.sin_port));
  environ_map["SERVER_PROTOCOL"] = request_info.request_line_.http_version_;
  environ_map["SERVER_SOFTWARE"] = "webserv/ver0.0.0";

  return environ_map;
}

static char **
create_cgi_environ(const std::map<std::string, std::string> &environ_map) {
  char **cgi_environ = new char *[environ_map.size() + 1];
  std::map<std::string, std::string>::const_iterator it = environ_map.begin();
  for (std::size_t i = 0; it != environ_map.end(); i++, it++) {
    std::string value = it->first + "=" + it->second;
    cgi_environ[i]    = ::strdup(value.c_str());
  }
  cgi_environ[environ_map.size()] = NULL;
  return cgi_environ;
}

CgiEnviron::CgiEnviron(const RequestInfo &request_info) {
  std::map<std::string, std::string> environ_map =
      create_environ_map(request_info);
  _environ_ = create_cgi_environ(environ_map);
}

CgiEnviron::~CgiEnviron() {
  for (size_t i = 0; _environ_[i] != NULL; i++) {
    delete _environ_[i];
  }
  delete[] _environ_;
}