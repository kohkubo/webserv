#ifndef SRCS_HTTP_RESPONSE_CGIENVIRON_HPP
#define SRCS_HTTP_RESPONSE_CGIENVIRON_HPP

#include "http/request/RequestInfo.hpp"
#include "utils/Path.hpp"

class CgiEnviron {
private:
  char **_environ_;

private:
  CgiEnviron() {}

public:
  char **environ() { return _environ_; }
  CgiEnviron(const RequestInfo &request_info, const std::string &peer_name,
             const Path &target_path);
  ~CgiEnviron();
};

#endif /* SRCS_HTTP_RESPONSE_CGIENVIRON_HPP */
