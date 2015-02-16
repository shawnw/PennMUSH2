/* Classes and utility functions for dealing with lower level OS stuff. */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "syswrap.h"

sys_open::sys_open(const char *path, int flags) {
  fd_ = open(path, flags);
  if (fd_ < 0)
    throw errno_exception("open");
}

sys_open::sys_open(const char *path, int flags, mode_t mode) {
  fd_ = open(path, flags, mode);
  if (fd_ < 0)
    throw errno_exception("open");
}

sys_open::~sys_open() {
  if (fd_ >= 0)
    close(fd_);
}

pid_t
sys_fork(void) {
  pid_t p = fork();
  if (p < 0)
    throw errno_exception("fork");
  else
    return p;
}

pid_t
sys_setsid(void) {
  pid_t p = setsid();
  if (p < 0)
    throw errno_exception("setsid");
  else
    return p;
}

/*
pid_t
sys_getsid(pid_t p) {
 pid_t s = getsid(p);
 if (s < 0)
    throw errno_exception("getsid");
  else
    return s;
}
*/

