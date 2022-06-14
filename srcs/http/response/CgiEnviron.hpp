#ifndef SRCS_HTTP_RESPONSE_CGIENVIRON_HPP
#define SRCS_HTTP_RESPONSE_CGIENVIRON_HPP

#include "http/request/RequestInfo.hpp"

class CgiEnviron {
private:
  char **__environ_;

private:
  CgiEnviron() {}

public:
  char **data() { return __environ_; }
  CgiEnviron(const std::string &path, const RequestInfo &request_info);
  ~CgiEnviron();
};

#endif /* SRCS_HTTP_RESPONSE_CGIENVIRON_HPP */
