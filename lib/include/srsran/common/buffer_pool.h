/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#ifndef SRSRAN_BUFFER_POOL_H
#define SRSRAN_BUFFER_POOL_H

#include "byte_buffer.h"
#include "srsran/adt/bounded_vector.h"
#include <algorithm>
#include <map>
#include <pthread.h>
#include <stack>
#include <string>
#include <vector>

#include "srsran/adt/pool/fixed_size_pool.h"
#include "srsran/common/common.h"
#include "srsran/srslog/srslog.h"

namespace srsran {

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
    pool.reserve(nof_buffers);
    free_list.reserve(nof_buffers);
    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&cv_not_empty, nullptr);
    for (uint32_t i = 0; i < nof_buffers; i++) {
      buffer_t* b = new (std::nothrow) buffer_t;
      if (!b) {
        perror("Error allocating memory. Exiting...\n");
        exit(-1);
      }
      pool.push_back(b);
      free_list.push_back(b);
    }
    capacity = nof_buffers;
  }

  ~buffer_pool()
  {
    for (auto* p : pool) {
      delete p;
    }
    pthread_cond_destroy(&cv_not_empty);
    pthread_mutex_destroy(&mutex);
  }

  void print_all_buffers()
  {
    printf("%d buffers in queue\n", static_cast<int>(pool.size() - free_list.size()));
#ifdef SRSRAN_BUFFER_POOL_LOG_ENABLED
    std::map<std::string, uint32_t> buffer_cnt;
    for (uint32_t i = 0; i < pool.size(); i++) {
      if (std::find(free_list.cbegin(), free_list.cend(), pool[i]) == free_list.cend()) {
        buffer_cnt[strlen(pool[i]->debug_name) ? pool[i]->debug_name : "Undefined"]++;
      }
    }
    std::map<std::string, uint32_t>::iterator it;
    for (it = buffer_cnt.begin(); it != buffer_cnt.end(); it++) {
      printf(" - %dx %s\n", it->second, it->first.c_str());
    }
#endif
  }

  uint32_t nof_available_pdus() { return free_list.size(); }

  bool is_almost_empty() { return free_list.size() < capacity / 20; }

  buffer_t* allocate(const char* debug_name = nullptr, bool blocking = false)
  {
    pthread_mutex_lock(&mutex);
    buffer_t* b = nullptr;

    if (!free_list.empty()) {
      b = free_list.back();
      free_list.pop_back();

      if (is_almost_empty()) {
        printf("Warning buffer pool capacity is %f %%\n", (float)100 * free_list.size() / capacity);
      }
#ifdef SRSRAN_BUFFER_POOL_LOG_ENABLED
      if (debug_name) {
        strncpy(b->debug_name, debug_name, SRSRAN_BUFFER_POOL_LOG_NAME_LEN);
        b->debug_name[SRSRAN_BUFFER_POOL_LOG_NAME_LEN - 1] = 0;
      }
#endif
    } else if (blocking) {
      // blocking allocation
      while (free_list.empty()) {
        pthread_cond_wait(&cv_not_empty, &mutex);
      }

      // retrieve the new buffer
      b = free_list.back();
      free_list.pop_back();

      // do not print any warning
    } else {
      printf("Error - buffer pool is empty\n");

#ifdef SRSRAN_BUFFER_POOL_LOG_ENABLED
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
    if (std::find(pool.cbegin(), pool.cend(), b) != pool.cend()) {
      free_list.push_back(b);
      ret = true;
    }
    pthread_cond_signal(&cv_not_empty);
    pthread_mutex_unlock(&mutex);
    return ret;
  }

private:
  static const int       POOL_SIZE = 4096;
  std::vector<buffer_t*> pool;
  std::vector<buffer_t*> free_list;
  pthread_mutex_t        mutex;
  pthread_cond_t         cv_not_empty;
  uint32_t               capacity;
};

/// Type of global byte buffer pool
using byte_buffer_pool = concurrent_fixed_memory_pool<sizeof(byte_buffer_t)>;

/// Function used to generate unique byte buffers
inline unique_byte_buffer_t make_byte_buffer() noexcept
{
  return std::unique_ptr<byte_buffer_t>(new (std::nothrow) byte_buffer_t());
}

inline unique_byte_buffer_t make_byte_buffer(uint32_t size, uint8_t value) noexcept
{
  return std::unique_ptr<byte_buffer_t>(new (std::nothrow) byte_buffer_t(size, value));
}

inline unique_byte_buffer_t make_byte_buffer(const char* debug_ctxt) noexcept
{
  std::unique_ptr<byte_buffer_t> buffer(new (std::nothrow) byte_buffer_t());
  if (buffer == nullptr) {
    srslog::fetch_basic_logger("POOL").error("Failed to allocate byte buffer in %s", debug_ctxt);
  }
  return buffer;
}

inline unique_byte_buffer_t make_byte_buffer(const uint8_t* payload, uint32_t len, const char* debug_ctxt) noexcept
{
  std::unique_ptr<byte_buffer_t> buffer(new (std::nothrow) byte_buffer_t());
  if (buffer == nullptr) {
    srslog::fetch_basic_logger("POOL").error("Failed to allocate byte buffer in %s", debug_ctxt);
  } else {
    if (buffer->get_tailroom() >= len) {
      memcpy(buffer->msg, payload, len);
      buffer->N_bytes = len;
    } else {
      srslog::fetch_basic_logger("POOL").error(
          "Failed to create byte buffer in %s. Payload too large (%d > %d)", debug_ctxt, len, buffer->get_tailroom());
    }
  }
  return buffer;
}

namespace detail {

template <typename T>
struct byte_buffer_pool_deleter {
  void operator()(T* ptr) const { byte_buffer_pool::get_instance()->deallocate_node(ptr); }
};

} // namespace detail

/// Unique ptr to global byte buffer pool
template <typename T>
using buffer_pool_ptr = std::unique_ptr<T, detail::byte_buffer_pool_deleter<T> >;

/// Method to create unique_ptrs of type T allocated in global byte buffer pool
template <typename T, typename... CtorArgs>
buffer_pool_ptr<T> make_buffer_pool_obj(CtorArgs&&... args) noexcept
{
  static_assert(sizeof(T) <= byte_buffer_pool::BLOCK_SIZE, "pool_bounded_vector does not fit buffer pool block size");
  void* memblock = byte_buffer_pool::get_instance()->allocate_node(sizeof(T));
  if (memblock == nullptr) {
    return buffer_pool_ptr<T>();
  }
  new (memblock) T(std::forward<CtorArgs>(args)...);
  return buffer_pool_ptr<T>(static_cast<T*>(memblock), detail::byte_buffer_pool_deleter<T>());
}

/**
 * Class to wrap objects of type T which get allocated/deallocated using the byte_buffer_pool
 * @tparam T type of the object being allocated
 */
template <typename T>
struct byte_buffer_pool_ptr {
  static_assert(sizeof(T) <= byte_buffer_pool::BLOCK_SIZE, "pool_bounded_vector does not fit buffer pool block size");

public:
  byte_buffer_pool_ptr() = default;
  void reset() { ptr.reset(); }

  T*       operator->() { return ptr.get(); }
  const T* operator->() const { return ptr.get(); }
  T&       operator*() { return *ptr; }
  const T& operator*() const { return *ptr; }
  bool     has_value() const { return ptr.get() != nullptr; }

  template <typename... CtorArgs>
  void emplace(CtorArgs&&... args)
  {
    ptr.reset(make(std::forward<CtorArgs>(args)...).ptr.release());
  }

  template <typename... CtorArgs>
  static byte_buffer_pool_ptr<T> make(CtorArgs&&... args)
  {
    byte_buffer_pool_ptr<T> ret;
    ret.ptr = make_buffer_pool_obj<T>(std::forward<CtorArgs>(args)...);
    return ret;
  };

private:
  buffer_pool_ptr<T> ptr;
};

/// unique_ptr with virtual deleter, so it can be used by any pool
template <typename T>
using any_pool_ptr = std::unique_ptr<T, std::function<void(T*)> >;

} // namespace srsran

#endif // SRSRAN_BUFFER_POOL_H
