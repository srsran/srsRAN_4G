/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/******************************************************************************
 *  File:         timeout.h
 *  Description:  Millisecond resolution timeouts. Uses a dedicated thread to
 *                call an optional callback function upon timeout expiry.
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_TIMEOUT_H
#define SRSLTE_TIMEOUT_H

#include "srslte/srslte.h"
#include <pthread.h>
#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>

namespace srslte {

class timeout_callback
{
public:
  virtual void timeout_expired(uint32_t timeout_id) = 0;
};

class timeout
{
public:
  timeout() : running(false), callback(NULL), thread(0), timeout_id(0) {}
  ~timeout()
  {
    if (running && callback)
      pthread_join(thread, NULL);
  }
  void start(int duration_msec_, uint32_t timeout_id_ = 0, timeout_callback* callback_ = NULL)
  {
    if (duration_msec_ < 0)
      return;
    reset();
    gettimeofday(&start_time[1], NULL);
    duration_msec = duration_msec_;
    running       = true;
    timeout_id    = timeout_id_;
    callback      = callback_;
    if (callback)
      pthread_create(&thread, NULL, &thread_start, this);
  }
  void reset()
  {
    if (callback)
      pthread_cancel(thread);
    running = false;
  }
  static void* thread_start(void* t_)
  {
    timeout* t = (timeout*)t_;
    t->thread_func();
    return NULL;
  }
  void thread_func()
  {
    // substract time elapsed until now from timer duration
    gettimeofday(&start_time[2], NULL);
    get_time_interval(start_time);

    int32_t usec = duration_msec * 1000 - start_time[0].tv_usec;
    if (usec > 0)
      usleep(usec);
    if (callback && running)
      callback->timeout_expired(timeout_id);
  }
  bool expired()
  {
    if (running) {
      gettimeofday(&start_time[2], NULL);
      get_time_interval(start_time);
      return start_time[0].tv_usec > duration_msec * 1000;
    } else {
      return false;
    }
  }
  int32_t get_msec_to_expire()
  {
    if (running) {
      gettimeofday(&start_time[2], NULL);
      get_time_interval(start_time);
      return (duration_msec * 1000 - start_time[0].tv_usec) / 1000;
    }
    return 0;
  }
  bool is_running() { return running; }

private:
  struct timeval    start_time[3];
  pthread_t         thread;
  uint32_t          timeout_id;
  timeout_callback* callback;
  bool              running;
  int               duration_msec;
};

} // namespace srslte

#endif // SRSLTE_TIMEOUT_H
