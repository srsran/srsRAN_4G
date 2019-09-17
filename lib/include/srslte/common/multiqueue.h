/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include <algorithm>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <vector>

namespace srslte {

template <typename myobj>
class multiqueue_handler
{
  // NOTE: needed to create a queue wrapper to make its move ctor noexcept.
  // otherwise we couldnt use the resize method of std::vector<queue<myobj>> if myobj is move-only
  class queue_wrapper : private std::queue<myobj>
  {
  public:
    queue_wrapper() = default;
    queue_wrapper(queue_wrapper&& other) noexcept : std::queue<myobj>(std::move(other)) {}
    using std::queue<myobj>::push;
    using std::queue<myobj>::pop;
    using std::queue<myobj>::size;
    using std::queue<myobj>::empty;
    using std::queue<myobj>::front;

    std::condition_variable cv_full;
    bool                    active = true;
  };

public:
  explicit multiqueue_handler(uint32_t capacity_ = std::numeric_limits<uint32_t>::max()) : capacity(capacity_) {}
  ~multiqueue_handler() { reset(); }

  void reset()
  {
    std::unique_lock<std::mutex> lock(mutex);
    running = false;
    while (nof_threads_waiting > 0) {
      uint32_t size = queues.size();
      lock.unlock();
      cv_empty.notify_one();
      for (uint32_t i = 0; i < size; ++i) {
        queues[i].cv_full.notify_all();
      }
      lock.lock();
      // wait for all threads to unblock
      cv_exit.wait(lock);
    }
    queues.clear();
  }

  int add_queue()
  {
    uint32_t                    qidx = 0;
    std::lock_guard<std::mutex> lock(mutex);
    if (not running) {
      return -1;
    }
    for (; qidx < queues.size() and queues[qidx].active; ++qidx)
      ;
    if (qidx == queues.size()) {
      // create new queue
      queues.emplace_back();
    } else {
      queues[qidx].active = true;
    }
    return (int)qidx;
  }

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
      while (is_queue_active_(q_idx) and queues[q_idx].size() >= capacity) {
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
      if (not is_queue_active_(q_idx) or queues[q_idx].size() >= capacity) {
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
      if (not is_queue_active_(q_idx) or queues[q_idx].size() >= capacity) {
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
      // Round-robin for all queues
      for (const queue_wrapper& q : queues) {
        spin_idx = (spin_idx + 1) % queues.size();
        if (is_queue_active_(spin_idx) and not queues[spin_idx].empty()) {
          if (value) {
            *value = std::move(queues[spin_idx].front());
          }
          queues[spin_idx].pop();
          if (nof_threads_waiting > 0) {
            lock.unlock();
            queues[spin_idx].cv_full.notify_one();
          }
          return spin_idx;
        }
      }
      nof_threads_waiting++;
      cv_empty.wait(lock);
      nof_threads_waiting--;
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

private:
  bool is_queue_active_(int qidx) const { return running and queues[qidx].active; }

  std::mutex                 mutex;
  std::condition_variable    cv_empty, cv_exit;
  uint32_t                   spin_idx = 0;
  bool                       running  = true;
  std::vector<queue_wrapper> queues;
  uint32_t                   capacity            = 0;
  uint32_t                   nof_threads_waiting = 0;
};

} // namespace srslte

#endif // SRSLTE_MULTIQUEUE_H
