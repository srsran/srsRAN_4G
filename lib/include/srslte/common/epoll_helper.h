/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

/*! \brief Common helper function for epoll
 *
 */

#ifndef SRSLTE_EPOLL_HELPER_H
#define SRSLTE_EPOLL_HELPER_H

#include <functional>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/signalfd.h>
#include <sys/timerfd.h>
#include <unistd.h>

///< A virtual interface to handle epoll events (used by timer and port handler)
class epoll_handler
{
public:
  virtual int handle_event(int fd, epoll_event e, int epoll_fd) = 0;
};

///< Callback function called when timer expires
using epoll_timer_callback = std::function<void(uint64_t res)>;

///< Epoll timer handler
class epoll_timer_handler : public epoll_handler
{
public:
  epoll_timer_handler(int fd_, epoll_timer_callback callback_) : timer_fd(fd_), callback(callback_){};
  int handle_event(int fd, epoll_event e, int epoll_fd)
  {
    uint64_t res;
    int      ret = read(fd, &res, sizeof(res));
    callback(res);
    return ret;
  }

  int get_timer_fd() { return timer_fd; };

private:
  int                  timer_fd = -1;
  epoll_timer_callback callback;
};

///< Basic epoll signal handler
class epoll_signal_handler : public epoll_handler
{
public:
  epoll_signal_handler(bool* running_) : running(running_) {}

  int handle_event(int fd, epoll_event e, int epoll_fd)
  {
    struct signalfd_siginfo info;
    if (read(fd, &info, sizeof(info)) != sizeof(info)) {
      fprintf(stderr, "failed to read signal fd buffer\n");
      return SRSLTE_ERROR;
    }
    switch (info.ssi_signo) {
      case SIGTERM:
      case SIGINT:
      case SIGHUP:
      case SIGQUIT:
        *running = false;
        break;
      default:
        fprintf(stderr, "got signal %d\n", info.ssi_signo);
        break;
    }
    return SRSLTE_SUCCESS;
  }

private:
  bool* running = nullptr;
};

///< Create periodic epoll timer every 1ms
int create_tti_timer()
{
  int timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
  if (timer_fd == -1) {
    printf("timerfd_create() failed: errno=%d\n", errno);
    return SRSLTE_ERROR;
  }

  int               msec = 1; // our 1ms TTI timer
  struct itimerspec ts   = {};
  ts.it_value.tv_sec     = msec / 1000;
  ts.it_value.tv_nsec    = (msec % 1000) * 1000000;
  ts.it_interval.tv_sec  = msec / 1000;
  ts.it_interval.tv_nsec = (msec % 1000) * 1000000;

  if (timerfd_settime(timer_fd, 0, &ts, NULL) < 0) {
    printf("timerfd_settime() failed: errno=%d\n", errno);
    close(timer_fd);
    return SRSLTE_ERROR;
  }
  return timer_fd;
}

///< Create signalfd for handling signals
int add_signalfd()
{
  // block all signals. we take signals synchronously via signalfd
  sigset_t all;
  sigfillset(&all);
  sigprocmask(SIG_SETMASK, &all, NULL);

  // add signals we accept synchronously via signalfd
  std::vector<int> sigs = {SIGIO, SIGHUP, SIGTERM, SIGINT, SIGQUIT, SIGALRM};

  sigset_t sw;
  sigemptyset(&sw);
  for (auto& sig : sigs) {
    sigaddset(&sw, sig);
  }

  // create the signalfd for receiving signals
  int sig_fd = signalfd(-1, &sw, 0);
  if (sig_fd == -1) {
    fprintf(stderr, "signalfd: %s\n", strerror(errno));
    return SRSLTE_ERROR;
  }
  return sig_fd;
}

///< Add fd to epoll fd
int add_epoll(int fd, int epoll_fd)
{
  struct epoll_event ev = {};
  ev.data.fd            = fd;
  ev.events             = EPOLLIN;
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
    fprintf(stderr, "epoll_ctl failed for fd=%d\n", fd);
    return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
}

///< Remove fd from epoll
int del_epoll(int fd, int epoll_fd)
{
  struct epoll_event ev = {};
  ev.data.fd            = fd;
  ev.events             = EPOLLIN;
  if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &ev) == -1) {
    fprintf(stderr, "epoll_ctl failed for fd=%d\n", fd);
    return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
}

#endif // SRSLTE_EPOLL_HELPER_H
