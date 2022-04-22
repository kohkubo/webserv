#include "event.hpp"

bool add_connection_list(int                        socket_fd,
                         std::map<int, Connection> &connection_list) {
  int connection = accept(socket_fd, (struct sockaddr *)NULL, NULL);
  if (connection == -1) {
    return false;
  }
  connection_list.insert(std::make_pair(connection, Connection(socket_fd)));
  return true;
}