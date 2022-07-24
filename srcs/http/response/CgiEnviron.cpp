#include "http/response/CgiEnviron.hpp"

#include <string.h>

#include <cstring>
#include <map>

#include "utils/Path.hpp"
#include "utils/utils.hpp"

static std::map<std::string, std::string>
create_environ_map(const RequestInfo &request_info,
                   const std::string &peer_name, const Path &target_path) {
  std::map<std::string, std::string> environ_map;

  if (request_info.has_body()) {
    environ_map["CONTENT_LENGTH"] =
        to_string(request_info.content_info_.content_.size());
    environ_map["CONTENT_TYPE"] = request_info.content_info_.content_type_;
  }
  environ_map["GATEWAY_INTERFACE"] = "CGI/1.1";
  environ_map["PATH_INFO"]         = target_path.str();
  environ_map["PATH_TRANSLATED"]   = environ_map["PATH_INFO"];
  environ_map["REMOTE_ADDR"]       = peer_name;
  environ_map["REMOTE_HOST"]       = environ_map["REMOTE_ADDR"];
  environ_map["QUERY_STRING"]      = request_info.request_line_.query_;
  environ_map["REQUEST_METHOD"]    = request_info.request_line_.method_;
  environ_map["SCRIPT_NAME"] =
      request_info.request_line_.absolute_path_.script_name();
  environ_map["SERVER_NAME"] = request_info.config_->server_name_;
  environ_map["SERVER_PORT"] =
      to_string(ntohs(request_info.config_->sockaddr_in_.sin_port));
  environ_map["SERVER_PROTOCOL"] = "HTTP/1.1";
  environ_map["SERVER_SOFTWARE"] = "webserv 0.0.0";

  return environ_map;
}

static char **
create_cgi_environ(const std::map<std::string, std::string> &environ_map) {
  char **cgi_environ = new char *[environ_map.size() + 1];
  std::map<std::string, std::string>::const_iterator it = environ_map.begin();
  for (std::size_t i = 0; it != environ_map.end(); i++, it++) {
    std::string value = it->first + "=" + it->second;
    cgi_environ[i]    = ::strdup(value.c_str());
    if (cgi_environ[i] == NULL) {
      ERROR_LOG_WITH_ERRNO("strdup() failed");
    }
  }
  cgi_environ[environ_map.size()] = NULL;
  return cgi_environ;
}

CgiEnviron::CgiEnviron(const RequestInfo &request_info,
                       const std::string &peer_name, const Path &target_path) {
  std::map<std::string, std::string> environ_map =
      create_environ_map(request_info, peer_name, target_path);
  _environ_ = create_cgi_environ(environ_map);
}

CgiEnviron::~CgiEnviron() {
  for (size_t i = 0; _environ_[i] != NULL; i++) {
    delete _environ_[i];
  }
  delete[] _environ_;
}