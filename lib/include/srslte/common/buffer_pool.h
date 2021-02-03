/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
#include "srslte/srslog/srslog.h"

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
    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&cv_not_empty, nullptr);
    for (uint32_t i = 0; i < nof_buffers; i++) {
      buffer_t* b = new (std::nothrow) buffer_t;
      if (!b) {
        perror("Error allocating memory. Exiting...\n");
        exit(-1);
      }
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

  buffer_t* allocate(const char* debug_name = nullptr, bool blocking = false)
  {
    pthread_mutex_lock(&mutex);
    buffer_t* b = nullptr;

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
  static std::unique_ptr<byte_buffer_pool> instance;
  static byte_buffer_pool*                 get_instance(int capacity = -1);
  static void                              cleanup();
  byte_buffer_pool(int capacity = -1) { pool = new buffer_pool<byte_buffer_t>(capacity); }
  byte_buffer_pool(const byte_buffer_pool& other) = delete;
  byte_buffer_pool& operator=(const byte_buffer_pool& other) = delete;
  ~byte_buffer_pool() { delete pool; }
  byte_buffer_t* allocate(const char* debug_name = nullptr, bool blocking = false)
  {
    return pool->allocate(debug_name, blocking);
  }
  void enable_logger(bool enabled) { print_to_log = enabled; }
  void deallocate(byte_buffer_t* b)
  {
    if (!b) {
      return;
    }
    b->clear();
    if (!pool->deallocate(b)) {
#ifdef SRSLTE_BUFFER_POOL_LOG_ENABLED
      print_error("Error deallocating PDU: Addr=0x%p, name=%s not found in pool", (void*)b, b->debug_name);
#else
      print_error("Error deallocating PDU: Addr=0x%p", (void*)b);
#endif
    }
    b = nullptr;
  }
  void print_all_buffers() { pool->print_all_buffers(); }

private:
  /// Formats and prints the input string and arguments into the configured output stream.
  template <typename... Args>
  void print_error(const char* str, Args&&... args)
  {
    if (print_to_log) {
      srslog::fetch_basic_logger("POOL", false).error(str, std::forward<Args>(args)...);
    } else {
      fmt::printf(std::string(str) + "\n", std::forward<Args>(args)...);
    }
  }

private:
  bool                        print_to_log = false;
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
