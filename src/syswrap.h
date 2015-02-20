#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <system_error>
#include <fcntl.h>
#include <unistd.h>

/* Wrapper functions for syscalls, signalling errors via raising
   std::system_error instead of returning -1 and setting errno */

/* RAII wrapper for open(2). Automatically closes the fd when it goes out of scope. Movable, not copyable. */
class sys_open {
private:
  int fd_;
public:
 sys_open(const char *path, int flags = O_RDONLY);
  sys_open(const char *path, int flags, mode_t mode);
  sys_open(sys_open &&r_) {
    fd_ = r_.fd_;
    r_.fd_ = -1;
  }
  sys_open& operator=(sys_open &&r_) {
    fd_ = r_.fd_;
    r_.fd_ = -1;
    return *this;
  }
  sys_open(const sys_open &) = delete;
  sys_open& operator=(const sys_open&) = delete;
  ~sys_open();
  operator int() { return fd_; }
};

pid_t sys_fork(void);
pid_t sys_setsid(void);
void sys_seteuid(uid_t);

