#include "utils/syscall_wrapper.hpp"

#include <dirent.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstdlib>
#include <ctime>

#include "utils/utils.hpp"

struct dirent *xreaddir(DIR *dir) {
  errno                   = 0;
  struct dirent *dir_read = readdir(dir);
  if (errno != 0) {
    ERROR_EXIT_WITH_ERRNO("readdir() failed");
  }
  return dir_read;
}

void xclosedir(DIR *dir) {
  if (closedir(dir) == -1) {
    ERROR_EXIT_WITH_ERRNO("closedir() failed");
  }
}
