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

#ifndef SRSLOG_DETAIL_SUPPORT_WORK_QUEUE_H
#define SRSLOG_DETAIL_SUPPORT_WORK_QUEUE_H

#include "srslte/srslog/detail/support/thread_utils.h"
#include <queue>

#ifndef SRSLOG_QUEUE_CAPACITY
#define SRSLOG_QUEUE_CAPACITY 8192
#endif

namespace srslog {

namespace detail {

//:TODO: this is a temp work queue.

/// Thread safe generic data type work queue.
template <typename T, size_t capacity = SRSLOG_QUEUE_CAPACITY>
class work_queue
{
  std::queue<T> queue;
  mutable condition_variable cond_var;
  static constexpr size_t threshold = capacity * 0.98;

public:
  work_queue() = default;

  work_queue(const work_queue&) = delete;
  work_queue& operator=(const work_queue&) = delete;

  /// Inserts a new element into the back of the queue.
  void push(const T& value)
  {
    cond_var.lock();
    // Discard the new element if we reach the maximum capacity.
    if (queue.size() > capacity) {
      cond_var.unlock();
      return;
    }
    queue.push(value);
    cond_var.signal();
    cond_var.unlock();
  }

  /// Inserts a new element into the back of the queue.
  void push(T&& value)
  {
    cond_var.lock();
    // Discard the new element if we reach the maximum capacity.
    if (queue.size() > capacity) {
      cond_var.unlock();
      return;
    }
    queue.push(std::move(value));
    cond_var.signal();
    cond_var.unlock();
  }

  /// Extracts the top most element from the queue.
  /// NOTE: This method blocks while the queue is empty.
  T pop()
  {
    cond_var.lock();

    while (queue.empty()) {
      cond_var.wait();
    }

    T elem = std::move(queue.front());
    queue.pop();

    cond_var.unlock();

    return elem;
  }

  /// Extracts the top most element from the queue.
  /// NOTE: This method blocks while the queue is empty or or until the
  /// programmed timeout expires. Returns a pair with a bool indicating if the
  /// pop has been successful.
  std::pair<bool, T> timed_pop(unsigned timeout_ms)
  {
    // Build an absolute time reference for the expiration time.
    timespec ts = condition_variable::build_timeout(timeout_ms);

    cond_var.lock();

    bool timedout = false;
    while (queue.empty() && !timedout) {
      timedout = cond_var.wait(ts);
    }

    // Did we wake up on timeout?
    if (timedout && queue.empty()) {
      cond_var.unlock();
      return {false, T{}};
    }

    // Here we have been woken up normally.
    T Item = std::move(queue.front());
    queue.pop();

    cond_var.unlock();

    return {true, std::move(Item)};
  }

  /// Capacity of the queue.
  size_t get_capacity() const { return capacity; }

  /// Returns true when the queue is almost full, otherwise returns false.
  bool is_almost_full() const
  {
    cond_var_scoped_lock lock(cond_var);

    return queue.size() > threshold;
  }
};

} // namespace detail

} // namespace srslog

#endif // SRSLOG_DETAIL_SUPPORT_WORK_QUEUE_H
