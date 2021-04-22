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

#ifndef SRSRAN_MEM_POOL_H
#define SRSRAN_MEM_POOL_H

#include "memblock_cache.h"
#include "srsran/common/thread_pool.h"
#include <cassert>
#include <cstdint>
#include <memory>
#include <mutex>

namespace srsran {

/**
 * Pool specialized for big objects. Created objects are not contiguous in memory.
 * Relevant methods:
 * - ::allocate_node(sz) - allocate memory of sizeof(T), or reuse memory already present in cache
 * - ::deallocate_node(void* p) - return memory addressed by p back to the pool to be cached.
 * - ::reserve(N) - prereserve memory slots for faster object creation
 * @tparam ObjSize object memory size
 * @tparam ThreadSafe if object pool is thread-safe or not
 */
template <typename T, bool ThreadSafe = false>
class big_obj_pool
{
  // memory stack type derivation (thread safe or not)
  using stack_type = typename std::conditional<ThreadSafe, concurrent_free_memblock_list, free_memblock_list>::type;

  // memory stack to cache allocate memory chunks
  stack_type stack;

public:
  ~big_obj_pool() { clear(); }

  /// alloc new object space. If no memory is pre-reserved in the pool, malloc is called.
  void* allocate_node(size_t sz)
  {
    assert(sz == sizeof(T));
    static const size_t blocksize = std::max(sizeof(T), free_memblock_list::min_memblock_size());
    void*               block     = stack.try_pop();
    if (block == nullptr) {
      block = new uint8_t[blocksize];
    }
    return block;
  }

  void deallocate_node(void* p)
  {
    if (p != nullptr) {
      stack.push(p);
    }
  }

  /// Pre-reserve N memory chunks for future object allocations
  void reserve(size_t N)
  {
    static const size_t blocksize = std::max(sizeof(T), free_memblock_list::min_memblock_size());
    for (size_t i = 0; i < N; ++i) {
      stack.push(static_cast<void*>(new uint8_t[blocksize]));
    }
  }

  size_t capacity() const { return stack.size(); }

  void clear()
  {
    uint8_t* block = static_cast<uint8_t*>(stack.try_pop());
    while (block != nullptr) {
      delete[] block;
      block = static_cast<uint8_t*>(stack.try_pop());
    }
  }
};

/**
 * Pool specialized for in allocating batches of objects in a preemptive way in a background thread to minimize latency.
 * Note: Current implementation assumes that the pool object will outlive the background callbacks to allocate new
 *       batches
 * @tparam T individual object type that is being allocated
 * @tparam BatchSize number of T objects in a batch
 * @tparam ThresholdSize number of T objects below which a new batch needs to be allocated
 */
template <typename T, size_t BatchSize, size_t ThresholdSize>
class background_allocator_obj_pool
{
  static_assert(ThresholdSize > 0, "ThresholdSize needs to be positive");
  static_assert(BatchSize > 1, "BatchSize needs to be higher than 1");

public:
  background_allocator_obj_pool(bool lazy_start = false)
  {
    if (not lazy_start) {
      allocate_batch_in_background();
    }
  }
  background_allocator_obj_pool(background_allocator_obj_pool&&)      = delete;
  background_allocator_obj_pool(const background_allocator_obj_pool&) = delete;
  background_allocator_obj_pool& operator=(background_allocator_obj_pool&&) = delete;
  background_allocator_obj_pool& operator=(const background_allocator_obj_pool&) = delete;
  ~background_allocator_obj_pool()
  {
    std::lock_guard<std::mutex> lock(mutex);
    batches.clear();
  }

  /// alloc new object space. If no memory is pre-reserved in the pool, malloc is called to allocate new batch.
  void* allocate_node(size_t sz)
  {
    assert(sz == sizeof(T));
    std::lock_guard<std::mutex> lock(mutex);
    void*                       block = obj_cache.try_pop();

    if (block != nullptr) {
      // allocation successful
      if (obj_cache.size() < ThresholdSize) {
        get_background_workers().push_task([this]() {
          std::lock_guard<std::mutex> lock(mutex);
          allocate_batch_();
        });
      }
      return block;
    }

    // try allocation of new batch in same thread as caller.
    allocate_batch_();
    return obj_cache.try_pop();
  }

  void deallocate_node(void* p)
  {
    std::lock_guard<std::mutex> lock(mutex);
    assert(p != nullptr);
    if (p != nullptr) {
      obj_cache.push(static_cast<uint8_t*>(p));
    }
  }

  void allocate_batch_in_background()
  {
    get_background_workers().push_task([this]() {
      std::lock_guard<std::mutex> lock(mutex);
      allocate_batch_();
    });
  }

private:
  using obj_storage_t = typename std::aligned_storage<sizeof(T), alignof(T)>::type;
  using batch_obj_t   = std::array<obj_storage_t, BatchSize>;

  /// Unprotected allocation of new Batch of Objects
  void allocate_batch_()
  {
    batches.emplace_back(new batch_obj_t());
    batch_obj_t& batch = *batches.back();
    for (obj_storage_t& obj_store : batch) {
      obj_cache.push(reinterpret_cast<uint8_t*>(&obj_store));
    }
  }

  // memory stack to cache allocate memory chunks
  std::mutex                                 mutex;
  free_memblock_list                         obj_cache;
  std::vector<std::unique_ptr<batch_obj_t> > batches;
};

} // namespace srsran

#endif // SRSRAN_MEM_POOL_H
