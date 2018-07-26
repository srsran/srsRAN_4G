/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
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

#include <queue>
#include <memory>
#include <utility>
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <strings.h>

namespace srslte {

template<typename myobj>
class block_queue {

public:

  // Callback functions for mutexed operations inside pop/push methods
  class call_mutexed_itf {
  public:
    virtual void popping(myobj obj) = 0;
    virtual void pushing(myobj obj) = 0;
  };

  block_queue<myobj>(int capacity = -1) {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cv_empty, NULL);
    pthread_cond_init(&cv_full, NULL);
    this->capacity = capacity;
    mutexed_callback = NULL;
    enable = true;
    num_threads = 0;
  }
  ~block_queue() {
    // Unlock threads waiting at push or pop
    pthread_mutex_lock(&mutex);
    enable = false;
    pthread_cond_signal(&cv_full);
    pthread_cond_signal(&cv_empty);
    pthread_mutex_unlock(&mutex);

    // Wait threads blocked in push/pop to exit
    while(num_threads>0) {
      usleep(100);
    }
    
    // Wait them to exit and destroy cv and mutex
    pthread_mutex_lock(&mutex);
    pthread_cond_destroy(&cv_full);
    pthread_cond_destroy(&cv_empty);
    pthread_mutex_unlock(&mutex);
    pthread_mutex_destroy(&mutex);
  }
  void set_mutexed_itf(call_mutexed_itf *itf) {
    mutexed_callback = itf;
  }
  void resize(int new_capacity) {
    capacity = new_capacity;
  }

  void push(const myobj& value) {
    push_(value, true);
  }

  bool try_push(const myobj& value) {
    return push_(value, false);
  }

  bool try_pop(myobj *value) {
    return pop_(value, false);
  }

  myobj wait_pop() { // blocking pop
    myobj value;
    bzero(&value, sizeof(myobj));
    pop_(&value, true);
    return value;
  }

  bool empty() { // queue is empty?
    pthread_mutex_lock(&mutex);
    bool ret = q.empty();
    pthread_mutex_unlock(&mutex);
    return ret;
  }

  void clear() { // remove all items
    myobj *item = NULL;
    while (try_pop(item));
  }

  myobj front() {
    return q.front();
  }

  size_t size() {
    return q.size();
  }

private:

  bool pop_(myobj *value, bool block) {
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
      *value = q.front();
      q.pop();
    }
    ret = true;
    if (mutexed_callback) {
      mutexed_callback->popping(*value);
    }
    pthread_cond_signal(&cv_full);
  exit:
    num_threads--;
    pthread_mutex_unlock(&mutex);
    return ret;
  }

  bool push_(const myobj& value, bool block) {
    if (!enable) {
      return false;
    }
    pthread_mutex_lock(&mutex);
    num_threads++;
    bool ret = false;
    if (capacity > 0) {
      if (block) {
        while(q.size() >= (uint32_t) capacity && enable) {
          pthread_cond_wait(&cv_full, &mutex);
        }
        if (!enable) {
          goto exit;
        }
      } else if (q.size() >= (uint32_t) capacity) {
        goto exit;
      }
    }
    q.push(value);
    ret = true;
    if (mutexed_callback) {
      mutexed_callback->pushing(value);
    }
    pthread_cond_signal(&cv_empty);
  exit:
    num_threads--;
    pthread_mutex_unlock(&mutex);
    return ret;
  }

  std::queue<myobj> q; 
  pthread_mutex_t mutex;
  pthread_cond_t  cv_empty;
  pthread_cond_t  cv_full;
  call_mutexed_itf *mutexed_callback;
  int capacity;
  bool enable;
  uint32_t num_threads;
};

}

#endif // SRSLTE_BLOCK_QUEUE_H