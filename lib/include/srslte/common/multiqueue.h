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

/******************************************************************************
 *  File:         multiqueue.h
 *  Description:  General-purpose non-blocking multiqueue. It behaves as a list
 *                of bounded/unbounded queues.
 *****************************************************************************/

#ifndef SRSLTE_MULTIQUEUE_H
#define SRSLTE_MULTIQUEUE_H

#include "srslte/adt/move_callback.h"
#include <algorithm>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <vector>

namespace srslte {

#define MULTIQUEUE_DEFAULT_CAPACITY (8192) // Default per-queue capacity

template <typename myobj>
class multiqueue_handler
{
  class circular_buffer
  {
  public:
    circular_buffer(uint32_t cap) : buffer(cap + 1) {}
    circular_buffer(circular_buffer&& other) noexcept
    {
      active       = other.active;
      other.active = false;
      widx         = other.widx;
      ridx         = other.ridx;
      buffer       = std::move(other.buffer);
    }

    std::condition_variable cv_full;
    bool                    active = true;

    bool   empty() const { return widx == ridx; }
    size_t size() const { return widx >= ridx ? widx - ridx : widx + (buffer.size() - ridx); }
    bool   full() const { return (ridx > 0) ? widx == ridx - 1 : widx == buffer.size() - 1; }
    size_t capacity() const { return buffer.size() - 1; }

    template <typename T>
    void push(T&& o) noexcept
    {
      buffer[widx++] = std::forward<T>(o);
      if (widx >= buffer.size()) {
        widx = 0;
      }
    }

    void pop() noexcept
    {
      ridx++;
      if (ridx >= buffer.size()) {
        ridx = 0;
      }
    }

    myobj&       front() noexcept { return buffer[ridx]; }
    const myobj& front() const noexcept { return buffer[ridx]; }

  private:
    std::vector<myobj> buffer;
    size_t             widx = 0, ridx = 0;
  };

public:
  class queue_handle
  {
  public:
    queue_handle() = default;
    queue_handle(multiqueue_handler<myobj>* parent_, int id) : parent(parent_), queue_id(id) {}
    template <typename FwdRef>
    void push(FwdRef&& value)
    {
      parent->push(queue_id, std::forward<FwdRef>(value));
    }
    bool                   try_push(const myobj& value) { return parent->try_push(queue_id, value); }
    std::pair<bool, myobj> try_push(myobj&& value) { return parent->try_push(queue_id, std::move(value)); }
    size_t                 size() { return parent->size(queue_id); }

  private:
    multiqueue_handler<myobj>* parent   = nullptr;
    int                        queue_id = -1;
  };

  explicit multiqueue_handler(uint32_t capacity_ = MULTIQUEUE_DEFAULT_CAPACITY) : capacity(capacity_) {}
  ~multiqueue_handler() { reset(); }

  void reset()
  {
    std::unique_lock<std::mutex> lock(mutex);
    running = false;
    while (nof_threads_waiting > 0) {
      uint32_t size = queues.size();
      cv_empty.notify_one();
      for (uint32_t i = 0; i < size; ++i) {
        queues[i].cv_full.notify_all();
      }
      // wait for all threads to unblock
      cv_exit.wait(lock);
    }
    queues.clear();
  }

  /**
   * Adds a new queue with fixed capacity
   * @param capacity_ The capacity of the queue.
   * @return The index of the newly created (or reused) queue within the vector of queues.
   */
  int add_queue(uint32_t capacity_)
  {
    uint32_t                    qidx = 0;
    std::lock_guard<std::mutex> lock(mutex);
    if (not running) {
      return -1;
    }
    for (; qidx < queues.size() and queues[qidx].active; ++qidx)
      ;

    // check if there is a free queue of the required size
    if (qidx == queues.size() || queues[qidx].capacity() != capacity_) {
      // create new queue
      queues.emplace_back(capacity_);
      qidx = queues.size() - 1; // update qidx to the last element
    } else {
      queues[qidx].active = true;
    }
    return (int)qidx;
  }

  /**
   * Add queue using the default capacity of the underlying multiqueue
   * @return The queue index
   */
  int add_queue() { return add_queue(capacity); }

  int nof_queues()
  {
    std::lock_guard<std::mutex> lock(mutex);
    uint32_t                    count = 0;
    for (uint32_t i = 0; i < queues.size(); ++i) {
      count += queues[i].active ? 1 : 0;
    }
    return count;
  }

  template <typename FwdRef>
  void push(int q_idx, FwdRef&& value)
  {
    {
      std::unique_lock<std::mutex> lock(mutex);
      while (is_queue_active_(q_idx) and queues[q_idx].full()) {
        nof_threads_waiting++;
        queues[q_idx].cv_full.wait(lock);
        nof_threads_waiting--;
      }
      if (not is_queue_active_(q_idx)) {
        cv_exit.notify_one();
        return;
      }
      queues[q_idx].push(std::forward<FwdRef>(value));
    }
    cv_empty.notify_one();
  }

  bool try_push(int q_idx, const myobj& value)
  {
    {
      std::lock_guard<std::mutex> lock(mutex);
      if (not is_queue_active_(q_idx) or queues[q_idx].full()) {
        return false;
      }
      queues[q_idx].push(value);
    }
    cv_empty.notify_one();
    return true;
  }

  std::pair<bool, myobj> try_push(int q_idx, myobj&& value)
  {
    {
      std::lock_guard<std::mutex> lck(mutex);
      if (not is_queue_active_(q_idx) or queues[q_idx].full()) {
        return {false, std::move(value)};
      }
      queues[q_idx].push(std::move(value));
    }
    cv_empty.notify_one();
    return {true, std::move(value)};
  }

  int wait_pop(myobj* value)
  {
    std::unique_lock<std::mutex> lock(mutex);
    while (running) {
      if (round_robin_pop_(value)) {
        if (nof_threads_waiting > 0) {
          lock.unlock();
          queues[spin_idx].cv_full.notify_one();
        }
        return spin_idx;
      }
      nof_threads_waiting++;
      cv_empty.wait(lock);
      nof_threads_waiting--;
    }
    cv_exit.notify_one();
    return -1;
  }

  int try_pop(myobj* value)
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (running) {
      if (round_robin_pop_(value)) {
        if (nof_threads_waiting > 0) {
          lock.unlock();
          queues[spin_idx].cv_full.notify_one();
        }
        return spin_idx;
      }
      // didn't find any task
      return -1;
    }
    cv_exit.notify_one();
    return -1;
  }

  bool empty(int qidx)
  {
    std::lock_guard<std::mutex> lck(mutex);
    return queues[qidx].empty();
  }

  size_t size(int qidx)
  {
    std::lock_guard<std::mutex> lck(mutex);
    return queues[qidx].size();
  }

  size_t max_size(int qidx)
  {
    std::lock_guard<std::mutex> lck(mutex);
    return queues[qidx].capacity();
  }

  const myobj& front(int qidx)
  {
    std::lock_guard<std::mutex> lck(mutex);
    return queues[qidx].front();
  }

  void erase_queue(int qidx)
  {
    std::lock_guard<std::mutex> lck(mutex);
    if (is_queue_active_(qidx)) {
      queues[qidx].active = false;
      while (not queues[qidx].empty()) {
        queues[qidx].pop();
      }
    }
  }

  bool is_queue_active(int qidx)
  {
    std::lock_guard<std::mutex> lck(mutex);
    return is_queue_active_(qidx);
  }

  queue_handle get_queue_handler() { return {this, add_queue()}; }
  queue_handle get_queue_handler(uint32_t size) { return {this, add_queue(size)}; }

private:
  bool is_queue_active_(int qidx) const { return running and queues[qidx].active; }

  bool round_robin_pop_(myobj* value)
  {
    // Round-robin for all queues
    for (const circular_buffer& q : queues) {
      spin_idx = (spin_idx + 1) % queues.size();
      if (is_queue_active_(spin_idx) and not queues[spin_idx].empty()) {
        if (value) {
          *value = std::move(queues[spin_idx].front());
        }
        queues[spin_idx].pop();
        return true;
      }
    }
    return false;
  }

  std::mutex                   mutex;
  std::condition_variable      cv_empty, cv_exit;
  uint32_t                     spin_idx = 0;
  bool                         running  = true;
  std::vector<circular_buffer> queues;
  uint32_t                     capacity            = 0;
  uint32_t                     nof_threads_waiting = 0;
};

//! Specialization for tasks
using task_multiqueue   = multiqueue_handler<move_task_t>;
using task_queue_handle = task_multiqueue::queue_handle;

} // namespace srslte

#endif // SRSLTE_MULTIQUEUE_H
