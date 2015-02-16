#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

/* Exception class thrown by sys* versions of functions that would normally return -1 and set errno */
class errno_exception : public std::runtime_error {
 private:
  int errno_;
  std::string func_;
  std::string msg_;
 public:
 errno_exception(const char *f)
   : errno_(errno), func_(f), std::runtime_error("") {
    msg_ = func_ + ": " + std::strerror(errno_);
  };
  virtual const char *what(void) const noexcept {
    return msg_.c_str();
  }
  const std::string& funcname(void) { return func_; }
  int errcode(void) { return errno_; } 
};

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

