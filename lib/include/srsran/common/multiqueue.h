/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

/******************************************************************************
 *  File:         multiqueue.h
 *  Description:  General-purpose non-blocking multiqueue. It behaves as a list
 *                of bounded/unbounded queues.
 *****************************************************************************/

#ifndef SRSRAN_MULTIQUEUE_H
#define SRSRAN_MULTIQUEUE_H

#include "srsran/adt/circular_buffer.h"
#include "srsran/adt/move_callback.h"
#include <algorithm>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <vector>

namespace srsran {

#define MULTIQUEUE_DEFAULT_CAPACITY (8192) // Default per-queue capacity

/**
 * N-to-1 Message-Passing Broker that manages the creation, destruction of input ports, and popping of messages that
 * are pushed to these ports.
 * Each port provides a thread-safe push(...) / try_push(...) interface to enqueue messages
 * The class will pop from the several created ports in a round-robin fashion.
 * The popping() interface is not safe-thread. That means, that it is expected that only one thread will
 * be popping tasks.
 * @tparam myobj message type
 */
template <typename myobj>
class multiqueue_handler
{
  class input_port_impl
  {
  public:
    input_port_impl(uint32_t cap, multiqueue_handler<myobj>* parent_) : buffer(cap), parent(parent_) {}
    input_port_impl(const input_port_impl&) = delete;
    input_port_impl(input_port_impl&&)      = delete;
    input_port_impl& operator=(const input_port_impl&) = delete;
    input_port_impl& operator=(input_port_impl&&) = delete;
    ~input_port_impl() { deactivate_blocking(); }

    size_t capacity() const { return buffer.max_size(); }
    size_t size() const
    {
      std::lock_guard<std::mutex> lock(q_mutex);
      return buffer.size();
    }
    bool active() const
    {
      std::lock_guard<std::mutex> lock(q_mutex);
      return active_;
    }

    void set_active(bool val)
    {
      std::unique_lock<std::mutex> lock(q_mutex);
      if (val == active_) {
        // no-op
        return;
      }
      active_                = val;
      consumer_notify_needed = true;

      if (not active_) {
        buffer.clear();
        lock.unlock();
        // unlock blocked pushing threads
        cv_full.notify_all();
      }
    }

    void deactivate_blocking()
    {
      set_active(false);

      // wait for all the pushers to unlock
      std::unique_lock<std::mutex> lock(q_mutex);
      while (nof_waiting > 0) {
        cv_exit.wait(lock);
      }
    }

    template <typename T>
    void push(T&& o) noexcept
    {
      push_(&o, true);
    }

    bool try_push(const myobj& o) { return push_(&o, false); }

    srsran::error_type<myobj> try_push(myobj&& o)
    {
      if (push_(&o, false)) {
        return {};
      }
      return {std::move(o)};
    }

    bool try_pop(myobj& obj)
    {
      std::unique_lock<std::mutex> lock(q_mutex);
      if (buffer.empty()) {
        consumer_notify_needed = true;
        return false;
      }
      obj = std::move(buffer.top());
      buffer.pop();
      consumer_notify_needed = false;
      if (nof_waiting > 0) {
        lock.unlock();
        cv_full.notify_one();
      }
      return true;
    }

  private:
    template <typename T>
    bool push_(T* o, bool blocking) noexcept
    {
      std::unique_lock<std::mutex> lock(q_mutex);
      if (not blocking) {
        // non-blocking case
        if (not active_ or buffer.full()) {
          return false;
        }
      } else {
        // blocking case
        while (active_ and buffer.full()) {
          nof_waiting++;
          cv_full.wait(lock);
          nof_waiting--;
        }
        if (not active_) {
          lock.unlock();
          cv_exit.notify_one();
          return false;
        }
      }
      buffer.push(std::forward<T>(*o));
      if (consumer_notify_needed) {
        // Note: The consumer thread only needs to be notified and awaken when queues transition from empty to non-empty
        //       To ensure that the consumer noticed that the queue was empty before a push, we store the last
        //       try_pop() return in a member variable.
        //       Doing this reduces the contention of multiple producers for the same condition variable
        lock.unlock();
        parent->signal_pushed_data();
      }
      return true;
    }

    multiqueue_handler<myobj>* parent = nullptr;

    mutable std::mutex                 q_mutex;
    srsran::dyn_circular_buffer<myobj> buffer;
    std::condition_variable            cv_full, cv_exit;
    bool                               active_                = true;
    bool                               consumer_notify_needed = true;
    int                                nof_waiting            = 0;
  };

public:
  class queue_handle
  {
  public:
    explicit queue_handle(input_port_impl* impl_ = nullptr) : impl(impl_) {}
    template <typename FwdRef>
    void push(FwdRef&& value)
    {
      impl->push(std::forward<FwdRef>(value));
    }
    bool                      try_push(const myobj& value) { return impl->try_push(value); }
    srsran::error_type<myobj> try_push(myobj&& value) { return impl->try_push(std::move(value)); }
    void                      reset()
    {
      if (impl != nullptr) {
        impl->deactivate_blocking();
        impl = nullptr;
      }
    }

    size_t size() { return impl->size(); }
    size_t capacity() { return impl->capacity(); }
    bool   active() const { return impl != nullptr and impl->active(); }
    bool   empty() const { return impl->size() == 0; }

    bool operator==(const queue_handle& other) const { return impl == other.impl; }
    bool operator!=(const queue_handle& other) const { return impl != other.impl; }

  private:
    struct recycle_op {
      void operator()(input_port_impl* p)
      {
        if (p != nullptr) {
          p->deactivate_blocking();
        }
      }
    };
    std::unique_ptr<input_port_impl, recycle_op> impl;
  };

  explicit multiqueue_handler(uint32_t default_capacity_ = MULTIQUEUE_DEFAULT_CAPACITY) :
    default_capacity(default_capacity_)
  {}
  ~multiqueue_handler() { stop(); }

  void stop()
  {
    std::unique_lock<std::mutex> lock(mutex);
    running = false;
    for (auto& q : queues) {
      // signal deactivation to pushing threads in a non-blocking way
      q.set_active(false);
    }
    while (wait_state) {
      pushed_data = true;
      cv_empty.notify_one();
      cv_exit.wait(lock);
    }
    for (auto& q : queues) {
      // ensure the queues are finished being deactivated
      q.deactivate_blocking();
    }
  }

  /**
   * Adds a new queue with fixed capacity
   * @param capacity_ The capacity of the queue.
   * @return The index of the newly created (or reused) queue within the vector of queues.
   */
  queue_handle add_queue(uint32_t capacity_)
  {
    uint32_t                    qidx = 0;
    std::lock_guard<std::mutex> lock(mutex);
    if (not running) {
      return queue_handle();
    }
    for (; qidx < queues.size() and (queues[qidx].active() or (queues[qidx].capacity() != capacity_)); ++qidx)
      ;

    // check if there is a free queue of the required size
    if (qidx == queues.size()) {
      // create new queue
      queues.emplace_back(capacity_, this);
      qidx = queues.size() - 1; // update qidx to the last element
    } else {
      queues[qidx].set_active(true);
    }
    return queue_handle(&queues[qidx]);
  }

  /**
   * Add queue using the default capacity of the underlying multiqueue
   * @return The queue index
   */
  queue_handle add_queue() { return add_queue(default_capacity); }

  uint32_t nof_queues() const
  {
    std::lock_guard<std::mutex> lock(mutex);
    uint32_t                    count = 0;
    for (uint32_t i = 0; i < queues.size(); ++i) {
      count += queues[i].active() ? 1 : 0;
    }
    return count;
  }

  bool wait_pop(myobj* value)
  {
    std::unique_lock<std::mutex> lock(mutex);
    while (running) {
      if (round_robin_pop_(value)) {
        return true;
      }
      pushed_data = false;
      wait_state  = true;
      while (not pushed_data) {
        cv_empty.wait(lock);
      }
      wait_state = false;
    }
    cv_exit.notify_one();
    return false;
  }

  bool try_pop(myobj* value)
  {
    std::unique_lock<std::mutex> lock(mutex);
    return running and round_robin_pop_(value);
  }

private:
  bool round_robin_pop_(myobj* value)
  {
    // Round-robin for all queues
    auto     it    = queues.begin() + spin_idx;
    uint32_t count = 0;
    for (; count < queues.size(); ++count, ++it) {
      if (it == queues.end()) {
        it = queues.begin(); // wrap-around
      }
      if (it->try_pop(*value)) {
        spin_idx = (spin_idx + count + 1) % queues.size();
        return true;
      }
    }
    return false;
  }
  /// Called by the producer threads to signal the consumer to unlock in wait_pop
  void signal_pushed_data()
  {
    {
      std::lock_guard<std::mutex> lock(mutex);
      if (pushed_data) {
        return;
      }
      pushed_data = true;
    }
    cv_empty.notify_one();
  }

  mutable std::mutex          mutex;
  std::condition_variable     cv_empty, cv_exit;
  uint32_t                    spin_idx = 0;
  bool                        running = true, pushed_data = false, wait_state = false;
  std::deque<input_port_impl> queues;
  uint32_t                    default_capacity = 0;
};

template <typename T>
using queue_handle = typename multiqueue_handler<T>::queue_handle;

//! Specialization for tasks
using task_multiqueue   = multiqueue_handler<move_task_t>;
using task_queue_handle = task_multiqueue::queue_handle;

} // namespace srsran

#endif // SRSRAN_MULTIQUEUE_H
