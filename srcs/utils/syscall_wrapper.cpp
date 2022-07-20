#include "utils/syscall_wrapper.hpp"

#include <dirent.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstdlib>
#include <ctime>

#include "utils/utils.hpp"

struct dirent *w_readdir(DIR *dir) {
  errno                   = 0;
  struct dirent *dir_read = readdir(dir);
  if (errno != 0) {
    ERROR_LOG_WITH_ERRNO("w_readdir() failed");
  }
  return dir_read;
}

void w_closedir(DIR *dir) {
  if (closedir(dir) == -1) {
    ERROR_LOG_WITH_ERRNO("w_closedir() failed");
  }
}
