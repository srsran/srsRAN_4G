/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSLOG_DETAIL_SUPPORT_WORK_QUEUE_H
#define SRSLOG_DETAIL_SUPPORT_WORK_QUEUE_H

#include "srsran/adt/circular_buffer.h"
#include "srsran/srslog/detail/support/backend_capacity.h"
#include "srsran/srslog/detail/support/thread_utils.h"

namespace srslog {

namespace detail {

//: TODO: this is a temp work queue.

/// Thread safe generic data type work queue.
template <typename T, size_t capacity = SRSLOG_QUEUE_CAPACITY>
class work_queue
{
  srsran::dyn_circular_buffer<T> queue;
  mutable mutex                  m;
  static constexpr size_t        threshold = capacity * 0.98;

public:
  work_queue() : queue(capacity) {}

  work_queue(const work_queue&) = delete;
  work_queue& operator=(const work_queue&) = delete;

  /// Inserts a new element into the back of the queue. Returns false when the
  /// queue is full, otherwise true.
  bool push(const T& value)
  {
    m.lock();
    // Discard the new element if we reach the maximum capacity.
    if (queue.full()) {
      m.unlock();
      return false;
    }
    queue.push(value);
    m.unlock();

    return true;
  }

  /// Inserts a new element into the back of the queue. Returns false when the
  /// queue is full, otherwise true.
  bool push(T&& value)
  {
    m.lock();
    // Discard the new element if we reach the maximum capacity.
    if (queue.full()) {
      m.unlock();
      return false;
    }
    queue.push(std::move(value));
    m.unlock();

    return true;
  }

  /// Extracts the top most element from the queue if it exists.
  /// Returns a pair with a bool indicating if the pop has been successful.
  std::pair<bool, T> try_pop()
  {
    m.lock();

    if (queue.empty()) {
      m.unlock();
      return {false, T()};
    }

    // Here we have been woken up normally.
    T Item = std::move(queue.top());
    queue.pop();

    m.unlock();

    return {true, std::move(Item)};
  }

  /// Capacity of the queue.
  size_t get_capacity() const { return capacity; }

  /// Returns true when the queue is almost full, otherwise returns false.
  bool is_almost_full() const
  {
    scoped_lock lock(m);

    return queue.size() > threshold;
  }
};

} // namespace detail

} // namespace srslog

#endif // SRSLOG_DETAIL_SUPPORT_WORK_QUEUE_H
