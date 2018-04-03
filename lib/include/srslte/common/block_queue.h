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

#ifndef SRSLTE_BLOCK_QUEUE_H
#define SRSLTE_BLOCK_QUEUE_H

#include <queue>
#include <memory>
#include <utility>
#include <pthread.h>
#include <stdio.h>
namespace srslte {

template<typename myobj>
class block_queue {

public:
  block_queue<myobj>() {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cvar, NULL);
  }
  void push(const myobj& value) {
    pthread_mutex_lock(&mutex);
    q.push(value);
    pthread_cond_signal(&cvar);    
    pthread_mutex_unlock(&mutex);
  }

  bool try_pop(myobj *value) { 
    pthread_mutex_lock(&mutex);
    if (q.empty()) {
      pthread_mutex_unlock(&mutex);
      return false;
    }
    if (value) {
      *value = q.front(); 
      q.pop();
    }
    pthread_mutex_unlock(&mutex);
    return true;
  }

  myobj wait_pop() { // blocking pop
    pthread_mutex_lock(&mutex);
    while(q.empty()) {
      pthread_cond_wait(&cvar, &mutex);
    }
    myobj value = q.front();
    q.pop();
    pthread_mutex_unlock(&mutex);
    return value;
  }

  bool empty() const { // queue is empty?
    pthread_mutex_lock(&mutex);
    bool ret = q.empty();
    pthread_mutex_unlock(&mutex);
    return ret;
  }

  void clear() { // remove all items
    myobj item;
    while (try_pop(item));
  }

private:
  std::queue<myobj> q; 
  pthread_mutex_t mutex;
  pthread_cond_t  cvar;
};

}

#endif // SRSLTE_BLOCK_QUEUE_H