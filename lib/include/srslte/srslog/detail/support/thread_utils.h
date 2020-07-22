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

#ifndef SRSLOG_DETAIL_SUPPORT_THREAD_UTILS_H
#define SRSLOG_DETAIL_SUPPORT_THREAD_UTILS_H

#include <pthread.h>

namespace srslog {

namespace detail {

//:TODO: these are temp helpers that will be replaced by std utils.

/// Abstraction of a pthread mutex.
class mutex
{
public:
  mutex(const mutex&) = delete;
  mutex& operator=(const mutex&) = delete;

  mutex() { ::pthread_mutex_init(&m, nullptr); }
  ~mutex() { ::pthread_mutex_destroy(&m); }

  /// Mutex lock.
  void lock() { ::pthread_mutex_lock(&m); }

  /// Mutex unlock.
  void unlock() { ::pthread_mutex_unlock(&m); }

  /// Mutex try lock. Returns true if the lock was obtained, false otherwise.
  bool try_lock() { return (::pthread_mutex_trylock(&m) == 0); }

  /// Accessor to the raw mutex structure.
  pthread_mutex_t* raw() { return &m; }
  const pthread_mutex_t* raw() const { return &m; }

private:
  pthread_mutex_t m;
};

/// RAII style object for automatically locking and unlocking a mutex.
class scoped_lock
{
  mutex& m;

public:
  explicit scoped_lock(mutex& m) : m(m) { m.lock(); }

  scoped_lock(const scoped_lock&) = delete;
  scoped_lock& operator=(const scoped_lock&) = delete;

  ~scoped_lock() { m.unlock(); }
};

/// This class allows accessing and modifying data atomically.
template <typename T>
class shared_variable
{
  T value;
  mutable mutex m;

public:
  shared_variable(const T& value) : value(value) {}

  shared_variable(const shared_variable&) = delete;
  shared_variable& operator=(const shared_variable&) = delete;

  /// Set this shared variable to a new value guarded by the associated mutex.
  shared_variable<T>& operator=(const T& other)
  {
    scoped_lock lock(m);
    value = other;
    return *this;
  }

  /// Get the value of this shared variable guarded by the associated mutex.
  operator T() const
  {
    scoped_lock lock(m);
    return value;
  }
};

/// Abstraction of a pthread condition variable.
class condition_variable
{
public:
  condition_variable() { ::pthread_cond_init(&cond_var, nullptr); }

  condition_variable(const condition_variable&) = delete;
  condition_variable& operator=(const condition_variable&) = delete;

  ~condition_variable() { ::pthread_cond_destroy(&cond_var); }

  /// Internal mutex control.
  void lock() { m.lock(); }
  void unlock() { m.unlock(); }

  /// Blocks the calling thread on this condition variable.
  void wait() { ::pthread_cond_wait(&cond_var, m.raw()); }

  /// Unblocks at least one waiting thread that is blocked on this condition
  /// variable.
  void signal() { ::pthread_cond_signal(&cond_var); }

  /// Unblocks all waiting threads being blocked on this condition variable.
  void broadcast() { ::pthread_cond_broadcast(&cond_var); }

  /// Blocks the calling thread on this condition variable up to the specified
  /// timeout. Returns true on timeout expiration, otherwise false.
  bool wait(timespec ts)
  {
    return (::pthread_cond_timedwait(&cond_var, m.raw(), &ts) == ETIMEDOUT);
  }

  /// Builds an absolute time timespec structure adding the specified time out
  /// in ms.
  static timespec build_timeout(unsigned timeout_ms)
  {
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000;
    if (ts.tv_nsec >= 1000000000) {
      ++ts.tv_sec;
      ts.tv_nsec -= 1000000000;
    }

    return ts;
  }

private:
  mutable mutex m;
  pthread_cond_t cond_var;
};

/// RAII style object for automatically locking and unlocking a condition
/// variable.
class cond_var_scoped_lock
{
  condition_variable& cond_var;

public:
  explicit cond_var_scoped_lock(condition_variable& cond_var) :
    cond_var(cond_var)
  {
    cond_var.lock();
  }

  cond_var_scoped_lock(const cond_var_scoped_lock&) = delete;
  cond_var_scoped_lock& operator=(const cond_var_scoped_lock&) = delete;

  ~cond_var_scoped_lock() { cond_var.unlock(); }
};

} // namespace detail

} // namespace srslog

#endif // SRSLOG_DETAIL_SUPPORT_THREAD_UTILS_H
