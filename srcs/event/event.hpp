#ifndef SRCS_EVENT_EVENT_HPP
#define SRCS_EVENT_EVENT_HPP

#include "config/ServerConfig.hpp"
#include <map>
#include <poll.h>

int  open_new_socket(const ServerConfig &config);
void listen_event(const server_group_type &server_group);

// pollに渡すpollfd構造体の配列の各要素をセット
// 現状, 各fdが待つイベントはPOLLIN(読み出し可能なデータがあるか)で固定
template <typename T>
void set_fd_list(struct pollfd *pfds, const int start_idx,
                 const std::map<int, T> &list) {
  int                                       idx = start_idx;
  typename std::map<int, T>::const_iterator it  = list.begin();
  for (; it != list.end(); it++) {
    pfds[idx].fd     = it->first;
    pfds[idx].events = POLLIN;
    idx++;
  }
}

typedef std::map<int, int> connection_list_type; // <accetpted fd, listen fd>
typedef connection_list_type::iterator connection_list_iterator;

#endif /* SRCS_EVENT_EVENT_HPP */
