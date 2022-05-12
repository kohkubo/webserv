#ifndef SRCS_EVENT_EVENT_HPP
#define SRCS_EVENT_EVENT_HPP

#include "config/ServerConfig.hpp"
#include <map>
#include <poll.h>

int                        open_new_socket(const ServerConfig &config);
void                       listen_event(const server_group_type &server_group);

typedef std::map<int, int> connection_list_type; // <accetpted fd, listen fd>
typedef connection_list_type::iterator       connection_list_iterator;
typedef std::vector<struct pollfd>           pollfds_type;
typedef std::vector<struct pollfd>::iterator pollfds_type_iterator;

// pollに渡すpollfd構造体の配列の各要素をセット
// 現状, 各fdが待つイベントはPOLLIN(読み出し可能なデータがあるか)で固定
template <typename T>
void set_pollfds(pollfds_type &pollfds, const std::map<int, T> &list) {
  typename std::map<int, T>::const_iterator it = list.begin();
  for (; it != list.end(); it++) {
    struct pollfd new_pfd;
    new_pfd.fd     = it->first;
    new_pfd.events = POLLIN;
    pollfds.push_back(new_pfd);
  }
}

#endif /* SRCS_EVENT_EVENT_HPP */
