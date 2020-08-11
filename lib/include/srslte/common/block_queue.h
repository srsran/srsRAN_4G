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
 *  File:         block_queue.h
 *  Description:  General-purpose blocking queue. It can behave as a bounded or
 *                unbounded blocking queue and allows blocking and non-blocking
 *                operations in both push and pop
 *****************************************************************************/

#ifndef SRSLTE_BLOCK_QUEUE_H
#define SRSLTE_BLOCK_QUEUE_H

#include "srslte/adt/expected.h"
#include <memory>
#include <pthread.h>
#include <queue>
#include <stdint.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <utility>

namespace srslte {

template <typename myobj>
class block_queue
{

public:
  // Callback functions for mutexed operations inside pop/push methods
  class call_mutexed_itf
  {
  public:
    virtual void popping(const myobj& obj) = 0;
    virtual void pushing(const myobj& obj) = 0;
  };

  explicit block_queue<myobj>(int capacity_ = -1)
  {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cv_empty, NULL);
    pthread_cond_init(&cv_full, NULL);
    capacity         = capacity_;
    mutexed_callback = NULL;
    enable           = true;
    num_threads      = 0;
  }
  ~block_queue()
  {
    // Unlock threads waiting at push or pop
    pthread_mutex_lock(&mutex);
    enable = false;
    pthread_cond_signal(&cv_full);
    pthread_cond_signal(&cv_empty);
    pthread_mutex_unlock(&mutex);

    // Wait threads blocked in push/pop to exit
    while (num_threads > 0) {
      usleep(100);
    }

    // Wait them to exit and destroy cv and mutex
    pthread_mutex_lock(&mutex);
    pthread_cond_destroy(&cv_full);
    pthread_cond_destroy(&cv_empty);
    pthread_mutex_unlock(&mutex);
    pthread_mutex_destroy(&mutex);
  }
  void set_mutexed_itf(call_mutexed_itf* itf) { mutexed_callback = itf; }
  void resize(int new_capacity) { capacity = new_capacity; }

  void push(const myobj& value) { push_(value, true); }

  void push(myobj&& value) { push_(std::move(value), true); }

  bool try_push(const myobj& value) { return push_(value, false); }

  srslte::error_type<myobj> try_push(myobj&& value) { return push_(std::move(value), false); }

  bool try_pop(myobj* value) { return pop_(value, false); }

  myobj wait_pop()
  { // blocking pop
    myobj value = myobj();
    pop_(&value, true);
    return value;
  }

  bool empty()
  { // queue is empty?
    pthread_mutex_lock(&mutex);
    bool ret = q.empty();
    pthread_mutex_unlock(&mutex);
    return ret;
  }

  bool full()
  { // queue is full?
    pthread_mutex_lock(&mutex);
    bool ret = not check_queue_space_unlocked(false);
    pthread_mutex_unlock(&mutex);
    return ret;
  }

  void clear()
  { // remove all items
    myobj* item = NULL;
    while (try_pop(item))
      ;
  }

  const myobj& front() const { return q.front(); }

  size_t size() { return q.size(); }

private:
  bool pop_(myobj* value, bool block)
  {
    if (!enable) {
      return false;
    }
    pthread_mutex_lock(&mutex);
    num_threads++;
    bool ret = false;
    if (q.empty() && !block) {
      goto exit;
    }
    while (q.empty() && enable) {
      pthread_cond_wait(&cv_empty, &mutex);
    }
    if (!enable) {
      goto exit;
    }
    if (value) {
      *value = std::move(q.front());
    }
    if (mutexed_callback) {
      mutexed_callback->popping(*value); // TODO: Value might be null!
    }
    q.pop();
    ret = true;
    pthread_cond_signal(&cv_full);
  exit:
    num_threads--;
    pthread_mutex_unlock(&mutex);
    return ret;
  }

  bool check_queue_space_unlocked(bool block)
  {
    num_threads++;
    if (capacity > 0) {
      if (block) {
        while (q.size() >= (uint32_t)capacity && enable) {
          pthread_cond_wait(&cv_full, &mutex);
        }
        if (!enable) {
          num_threads--;
          return false;
        }
      } else if (q.size() >= (uint32_t)capacity) {
        num_threads--;
        return false;
      }
    }
    num_threads--;
    return true;
  }

  srslte::error_type<myobj> push_(myobj&& value, bool block)
  {
    if (!enable) {
      return std::move(value);
    }
    pthread_mutex_lock(&mutex);
    bool ret = check_queue_space_unlocked(block);
    if (ret) {
      if (mutexed_callback) {
        mutexed_callback->pushing(value);
      }
      q.push(std::move(value));
      pthread_mutex_unlock(&mutex);
      pthread_cond_signal(&cv_empty);
      return {};
    }
    pthread_mutex_unlock(&mutex);
    return std::move(value);
  }

  bool push_(const myobj& value, bool block)
  {
    if (!enable) {
      return false;
    }
    pthread_mutex_lock(&mutex);
    bool ret = check_queue_space_unlocked(block);
    if (ret) {
      if (mutexed_callback) {
        mutexed_callback->pushing(value);
      }
      q.push(value);
      pthread_cond_signal(&cv_empty);
    }
    pthread_mutex_unlock(&mutex);
    return ret;
  }

  std::queue<myobj> q;
  pthread_mutex_t   mutex;
  pthread_cond_t    cv_empty;
  pthread_cond_t    cv_full;
  call_mutexed_itf* mutexed_callback;
  int               capacity;
  bool              enable;
  uint32_t          num_threads;
};

} // namespace srslte

#endif // SRSLTE_BLOCK_QUEUE_H
