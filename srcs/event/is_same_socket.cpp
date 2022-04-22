#include "event.hpp"

#include <netdb.h>

struct addrinfo *getaddrinfo_via_serv(const ServerConfig &conf) {
  struct addrinfo hints, *info;
  memset(&hints, 0, sizeof(hints));
  memset(&info, 0, sizeof(info));
  hints.ai_family   = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  int error         = getaddrinfo(conf.listen_address_.c_str(),
                                  conf.listen_port_.c_str(), &hints, &info);
  if (error) {
    std::cerr << "getaddrinfo: " << gai_strerror(error) << std::endl;
  }
  return info;
}

bool is_same_socket(const ServerConfig &serv_x, const ServerConfig &serv_y) {
  bool                res    = false;
  struct addrinfo    *x_info = getaddrinfo_via_serv(serv_x);
  struct addrinfo    *y_info = getaddrinfo_via_serv(serv_y);
  struct sockaddr_in *x      = reinterpret_cast<sockaddr_in *>(x_info->ai_addr);
  struct sockaddr_in *y      = reinterpret_cast<sockaddr_in *>(y_info->ai_addr);

  res                        = (x->sin_addr.s_addr == y->sin_addr.s_addr) &&
        (x->sin_port == y->sin_port);
  freeaddrinfo(x_info);
  freeaddrinfo(y_info);
  return res;
}
