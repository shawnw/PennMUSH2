/* Classes and utility functions for dealing with lower level OS stuff. */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "syswrap.h"

sys_open::sys_open(const char *path, int flags) {
  fd_ = open(path, flags);
  if (fd_ < 0)
    throw std::system_error(errno, std::system_category(), "open");
}

sys_open::sys_open(const char *path, int flags, mode_t mode) {
  fd_ = open(path, flags, mode);
  if (fd_ < 0)
    throw std::system_error(errno, std::system_category(), "open");
}

sys_open::~sys_open() {
  if (fd_ >= 0)
    close(fd_);
}

pid_t
sys_fork(void) {
  pid_t p = fork();
  if (p < 0)
    throw std::system_error(errno, std::system_category(), "fork");
  else
    return p;
}

pid_t
sys_setsid(void) {
  pid_t p = setsid();
  if (p < 0)
    throw std::system_error(errno, std::system_category(), "setsid");
  else
    return p;
}

void
sys_seteuid(uid_t id) {
  if (seteuid(id) < 0)
    throw std::system_error(errno, std::system_category(), "seteuid");
}
