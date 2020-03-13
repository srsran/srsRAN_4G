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

#ifndef SRSLTE_BUFFER_POOL_H
#define SRSLTE_BUFFER_POOL_H

#include <algorithm>
#include <map>
#include <pthread.h>
#include <stack>
#include <string>
#include <vector>

/*******************************************************************************
                              INCLUDES
*******************************************************************************/

#include "srslte/common/common.h"
#include "srslte/common/log.h"

namespace srslte {

/******************************************************************************
 * Buffer pool
 *
 * Preallocates a large number of buffer_t and provides allocate and
 * deallocate functions. Provides quick object creation and deletion as well
 * as object reuse.
 * Singleton class of byte_buffer_t (but other pools of different type can be created)
 *****************************************************************************/

template <class buffer_t>
class buffer_pool
{
public:
  // non-static methods
  buffer_pool(int capacity_ = -1)
  {
    uint32_t nof_buffers = POOL_SIZE;
    if (capacity_ > 0) {
      nof_buffers = (uint32_t)capacity_;
    }
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cv_not_empty, NULL);
    for (uint32_t i = 0; i < nof_buffers; i++) {
      buffer_t* b = new buffer_t;
      available.push(b);
    }
    capacity = nof_buffers;
  }

  ~buffer_pool()
  {
    // this destructor assumes all buffers have been properly deallocated
    while (available.size()) {
      delete available.top();
      available.pop();
    }

    for (uint32_t i = 0; i < used.size(); i++) {
      delete used[i];
    }
    pthread_cond_destroy(&cv_not_empty);
    pthread_mutex_destroy(&mutex);
  }

  void print_all_buffers()
  {
    printf("%d buffers in queue\n", (int)used.size());
#ifdef SRSLTE_BUFFER_POOL_LOG_ENABLED
    std::map<std::string, uint32_t> buffer_cnt;
    for (uint32_t i = 0; i < used.size(); i++) {
      buffer_cnt[strlen(used[i]->debug_name) ? used[i]->debug_name : "Undefined"]++;
    }
    std::map<std::string, uint32_t>::iterator it;
    for (it = buffer_cnt.begin(); it != buffer_cnt.end(); it++) {
      printf(" - %dx %s\n", it->second, it->first.c_str());
    }
#endif
  }

  uint32_t nof_available_pdus() { return available.size(); }

  bool is_almost_empty() { return available.size() < capacity / 20; }

  buffer_t* allocate(const char* debug_name = NULL, bool blocking = false)
  {
    pthread_mutex_lock(&mutex);
    buffer_t* b = NULL;

    if (available.size() > 0) {
      b = available.top();
      used.push_back(b);
      available.pop();

      if (is_almost_empty()) {
        printf("Warning buffer pool capacity is %f %%\n", (float)100 * available.size() / capacity);
      }
#ifdef SRSLTE_BUFFER_POOL_LOG_ENABLED
      if (debug_name) {
        strncpy(b->debug_name, debug_name, SRSLTE_BUFFER_POOL_LOG_NAME_LEN);
        b->debug_name[SRSLTE_BUFFER_POOL_LOG_NAME_LEN - 1] = 0;
      }
#endif
    } else if (blocking) {
      // blocking allocation
      while (available.size() == 0) {
        pthread_cond_wait(&cv_not_empty, &mutex);
      }

      // retrieve the new buffer
      b = available.top();
      used.push_back(b);
      available.pop();

      // do not print any warning
    } else {
      printf("Error - buffer pool is empty\n");

#ifdef SRSLTE_BUFFER_POOL_LOG_ENABLED
      print_all_buffers();
#endif
    }

    pthread_mutex_unlock(&mutex);
    return b;
  }

  bool deallocate(buffer_t* b)
  {
    bool ret = false;
    pthread_mutex_lock(&mutex);
    typename std::vector<buffer_t*>::iterator elem = std::find(used.begin(), used.end(), b);
    if (elem != used.end()) {
      used.erase(elem);
      available.push(b);
      ret = true;
    }
    pthread_cond_signal(&cv_not_empty);
    pthread_mutex_unlock(&mutex);
    return ret;
  }

private:
  static const int       POOL_SIZE = 4096;
  std::stack<buffer_t*>  available;
  std::vector<buffer_t*> used;
  pthread_mutex_t        mutex;
  pthread_cond_t         cv_not_empty;
  uint32_t               capacity;
};

class byte_buffer_pool
{
public:
  // Singleton static methods
  static byte_buffer_pool* instance;
  static byte_buffer_pool* get_instance(int capacity = -1);
  static void              cleanup(void);
  byte_buffer_pool(int capacity = -1)
  {
    log  = NULL;
    pool = new buffer_pool<byte_buffer_t>(capacity);
  }
  byte_buffer_pool(const byte_buffer_pool& other) = delete;
  byte_buffer_pool& operator=(const byte_buffer_pool& other) = delete;
  ~byte_buffer_pool() { delete pool; }
  byte_buffer_t* allocate(const char* debug_name = NULL, bool blocking = false)
  {
    return pool->allocate(debug_name, blocking);
  }
  void set_log(srslte::log* log) { this->log = log; }
  void deallocate(byte_buffer_t* b)
  {
    if (!b) {
      return;
    }
    b->clear();
    if (!pool->deallocate(b)) {
      if (log) {
#ifdef SRSLTE_BUFFER_POOL_LOG_ENABLED
        log->error("Deallocating PDU: Addr=0x%p, name=%s not found in pool\n", b, b->debug_name);
#else
        log->error("Deallocating PDU: Addr=0x%p\n", b);
#endif
      } else {
#ifdef SRSLTE_BUFFER_POOL_LOG_ENABLED
        printf("Error deallocating PDU: Addr=0x%p, name=%s not found in pool\n", b, b->debug_name);
#else
        printf("Error deallocating PDU: Addr=0x%p\n", b);
#endif
      }
    }
    b = NULL;
  }
  void print_all_buffers() { pool->print_all_buffers(); }

private:
  srslte::log*                log;
  buffer_pool<byte_buffer_t>* pool;
};

inline void byte_buffer_deleter::operator()(byte_buffer_t* buf) const
{
  if (buf) {
    pool->deallocate(buf);
  }
}

inline unique_byte_buffer_t allocate_unique_buffer(byte_buffer_pool& pool, bool blocking = false)
{
  return unique_byte_buffer_t(pool.allocate(nullptr, blocking), byte_buffer_deleter(&pool));
}

inline unique_byte_buffer_t
allocate_unique_buffer(byte_buffer_pool& pool, const char* debug_name, bool blocking = false)
{
  return unique_byte_buffer_t(pool.allocate(debug_name, blocking), byte_buffer_deleter(&pool));
}

} // namespace srslte

#endif // SRSLTE_BUFFER_POOL_H
