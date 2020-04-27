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

#ifndef SRSLTE_THREADS_H
#define SRSLTE_THREADS_H

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/timerfd.h>
#include <unistd.h>

// Default priority for all threads below UHD threads
#define DEFAULT_PRIORITY 60

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

bool threads_new_rt(pthread_t* thread, void* (*start_routine)(void*), void* arg);
bool threads_new_rt_prio(pthread_t* thread, void* (*start_routine)(void*), void* arg, int prio_offset);
bool threads_new_rt_cpu(pthread_t* thread, void* (*start_routine)(void*), void* arg, int cpu, int prio_offset);
bool threads_new_rt_mask(pthread_t* thread, void* (*start_routine)(void*), void* arg, int mask, int prio_offset);
void threads_print_self();

#ifdef __cplusplus
}

#include <string>

namespace srslte {

class thread
{
public:
  thread(const std::string& name_) : _thread(0), name(name_) {}

  thread(const thread&) = delete;

  thread(thread&& other) noexcept
  {
    _thread       = other._thread;
    name          = std::move(other.name);
    other._thread = 0;
    other.name    = "";
  }

  virtual ~thread() = default;

  thread& operator=(const thread&) = delete;

  thread& operator=(thread&&) noexcept = delete;

  bool start(int prio = -1) { return threads_new_rt_prio(&_thread, thread_function_entry, this, prio); }

  bool start_cpu(int prio, int cpu) { return threads_new_rt_cpu(&_thread, thread_function_entry, this, cpu, prio); }

  bool start_cpu_mask(int prio, int mask)
  {
    return threads_new_rt_mask(&_thread, thread_function_entry, this, mask, prio);
  }

  void print_priority() { threads_print_self(); }

  void set_name(const std::string& name_)
  {
    name = name_;
    pthread_setname_np(pthread_self(), name.c_str());
  }

  void wait_thread_finish() { pthread_join(_thread, NULL); }

  void thread_cancel() { pthread_cancel(_thread); }

  static std::string get_name()
  {
    const uint32_t  MAX_LEN       = 16;
    char            name[MAX_LEN] = {};
    const pthread_t tid           = pthread_self();
    if (pthread_getname_np(tid, name, MAX_LEN)) {
      perror("Could not get pthread name");
    }
    return std::string(name);
  }

protected:
  virtual void run_thread() = 0;

private:
  static void* thread_function_entry(void* _this)
  {
    pthread_setname_np(pthread_self(), ((thread*)_this)->name.c_str());
    ((thread*)_this)->run_thread();
    return NULL;
  }

  pthread_t   _thread;
  std::string name;
};

class periodic_thread : public thread
{
public:
  periodic_thread(const std::string name_) : thread(name_) {}

  void start_periodic(int period_us_, int priority = -1)
  {
    run_enable = true;
    period_us  = period_us_;
    start(priority);
  }

  void stop_thread()
  {
    run_enable = false;
    wait_thread_finish();
  }

protected:
  virtual void run_period() = 0;

private:
  int  wakeups_missed;
  int  timer_fd;
  int  period_us;
  bool run_enable;

  void run_thread()
  {
    if (make_periodic()) {
      return;
    }
    while (run_enable) {
      run_period();
      if (run_enable) {
        wait_period();
      }
    }
  }

  int make_periodic()
  {
    int               ret = -1;
    unsigned int      ns;
    unsigned int      sec;
    struct itimerspec itval;

    /* Create the timer */
    ret            = timerfd_create(CLOCK_MONOTONIC, 0);
    wakeups_missed = 0;
    timer_fd       = ret;
    if (ret > 0) {
      /* Make the timer periodic */
      sec                       = period_us / 1e6;
      ns                        = (period_us - (sec * 1000000)) * 1000;
      itval.it_interval.tv_sec  = sec;
      itval.it_interval.tv_nsec = ns;
      itval.it_value.tv_sec     = sec;
      itval.it_value.tv_nsec    = ns;
      ret                       = timerfd_settime(timer_fd, 0, &itval, NULL);
      if (ret < 0) {
        perror("timerfd_settime");
      }
    } else {
      perror("timerfd_create");
    }
    return ret;
  }

  void wait_period()
  {
    unsigned long long missed;
    int                ret;

    /* Wait for the next timer event. If we have missed any the
        number is written to "missed" */
    ret = read(timer_fd, &missed, sizeof(missed));
    if (ret == -1) {
      perror("read timer");
      return;
    }

    /* "missed" should always be >= 1, but just to be sure, check it is not 0 anyway */
    if (missed > 0) {
      wakeups_missed += (missed - 1);
    }
  }
};

} // namespace srslte

#endif // __cplusplus

#endif // SRSLTE_THREADS_H
