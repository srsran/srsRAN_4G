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
  };

public:
  explicit multiqueue_handler(uint32_t capacity_ = std::numeric_limits<uint32_t>::max()) : capacity(capacity_) {}
  ~multiqueue_handler()
  {
    std::lock_guard<std::mutex> lck(mutex);
    queues_active.clear();
    queues.clear();
    running = false;
  }

  int add_queue()
  {
    uint32_t qidx = 0;
    for (; qidx < queues_active.size() and queues_active[qidx]; ++qidx)
      ;
    if (qidx == queues_active.size()) {
      // create new queue
      std::lock_guard<std::mutex> lck(mutex);
      queues_active.push_back(true);
      queues.emplace_back();
    } else {
      queues_active[qidx] = true;
    }
    return (int)qidx;
  }

  int nof_queues()
  {
    std::lock_guard<std::mutex> lck(mutex);
    return std::count(queues_active.begin(), queues_active.end(), true);
  }

  bool try_push(int q_idx, const myobj& value)
  {
    if (not running) {
      return false;
    }
    {
      std::lock_guard<std::mutex> lck(mutex);
      if (queues[q_idx].size() >= capacity) {
        return false;
      }
      queues[q_idx].push(value);
    }
    cv.notify_one();
    return true;
  }

  std::pair<bool, myobj> try_push(int q_idx, myobj&& value)
  {
    if (not running) {
      return {false, std::move(value)};
    }
    {
      std::lock_guard<std::mutex> lck(mutex);
      if (queues[q_idx].size() >= capacity) {
        return {false, std::move(value)};
      }
      queues[q_idx].push(std::move(value));
    }
    cv.notify_one();
    return {true, std::move(value)};
  }

  int wait_pop(myobj* value)
  {
    std::unique_lock<std::mutex> lock(mutex);
    while (running) {
      cv.wait(lock);
      // Round-robin for all queues
      for (uint32_t i = 0; queues.size(); ++i) {
        spin_idx = (spin_idx + 1) % queues.size();
        if (queues_active[spin_idx] and not queues[spin_idx].empty()) {
          if (value) {
            *value = std::move(queues[spin_idx].front());
          }
          queues[spin_idx].pop();
          return spin_idx;
        }
      }
    }
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
    return queues.front();
  }

  void erase_queue(int qidx)
  {
    std::lock_guard<std::mutex> lck(mutex);
    if (queues_active[qidx]) {
      queues_active[qidx] = false;
      while (not queues[qidx].empty()) {
        queues[qidx].pop();
      }
    }
  }

  bool is_queue_active(int qidx)
  {
    std::lock_guard<std::mutex> lck(mutex);
    return queues_active[qidx];
  }

private:
  std::mutex                 mutex;
  std::condition_variable    cv;
  uint32_t                   spin_idx = 0;
  bool                       running  = true;
  std::vector<bool>          queues_active;
  std::vector<queue_wrapper> queues;
  uint32_t                   capacity;
};

} // namespace srslte

#endif // SRSLTE_MULTIQUEUE_H
