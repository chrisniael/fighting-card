/*
 * file : daemon.h
 * author : caihanwu
 * date : 2018.12.7
 * */
#ifndef COMMON_DAEMON_H_
#define COMMON_DAEMON_H_

#include <execinfo.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/resource.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>

namespace common {

inline int CreateDaemon() {
  pid_t pid = fork();
  if (pid < 0) {
    return -1;
  } else if (pid) {
    // parent terminates
    _exit(0);
  }

  // child 1 continues
  // become session leader
  if (setsid() < 0) {
    return -1;
  }

  signal(SIGHUP, SIG_IGN);
  pid = fork();
  if (pid < 0) {
    return -1;
  } else if (pid) {
    // child 1 terminates
    _exit(0);
  }
  // child 2 continues...

  // close off file descriptors
  for (auto i = 0; i < 64; i++) close(i);

  // redirect stdin, stdout, and stderr to /dev/null
  open("/dev/null", O_RDONLY);
  open("/dev/null", O_RDWR);
  open("/dev/null", O_RDWR);

  return 0;
}

}  // namespace common

#endif  // COMMON_DAEMON_H_
