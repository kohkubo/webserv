#include "utils/file_io_utils.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "utils/utils.hpp"

Result<std::string> read_fd(int fd) {
  std::stringstream buffer;
  char              buf[1024];
  while (true) {
    ssize_t read_size = read(fd, buf, sizeof(buf));
    if (read_size == -1) {
      return Error<std::string>();
    }
    if (read_size == 0) {
      break;
    }
    buffer << std::string(buf, read_size);
  }
  return Ok<std::string>(buffer.str());
}
