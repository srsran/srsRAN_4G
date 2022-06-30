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

#ifndef SRSRAN_CIRCULAR_MAP_STACK_POOL_H
#define SRSRAN_CIRCULAR_MAP_STACK_POOL_H

#include "batch_mem_pool.h"
#include "linear_allocator.h"
#include "srsran/adt/circular_array.h"
#include <mutex>

namespace srsran {

template <size_t NofStacks>
class circular_stack_pool
{
  struct mem_block_elem_t {
    std::mutex       mutex;
    size_t           key   = std::numeric_limits<size_t>::max();
    size_t           count = 0;
    linear_allocator alloc;

    void clear()
    {
      key   = std::numeric_limits<size_t>::max();
      count = 0;
      alloc.clear();
    }
  };

public:
  circular_stack_pool(size_t nof_objs_per_batch, size_t stack_size, size_t batch_thres, int initial_size = -1) :
    central_cache(std::min(NofStacks, nof_objs_per_batch), stack_size, batch_thres, initial_size),
    logger(srslog::fetch_basic_logger("POOL"))
  {}
  circular_stack_pool(circular_stack_pool&&)      = delete;
  circular_stack_pool(const circular_stack_pool&) = delete;
  circular_stack_pool& operator=(circular_stack_pool&&) = delete;
  circular_stack_pool& operator=(const circular_stack_pool&) = delete;
  ~circular_stack_pool()
  {
    for (mem_block_elem_t& elem : pools) {
      std::unique_lock<std::mutex> lock(elem.mutex);
      srsran_expect(elem.count == 0, "There are missing deallocations for stack id=%zd", elem.key);
      if (elem.alloc.is_init()) {
        void* ptr = elem.alloc.memblock_ptr();
        elem.alloc.clear();
        central_cache.deallocate_node(ptr);
      }
    }
  }

  void* allocate(size_t key, size_t size, size_t alignment) noexcept
  {
    size_t                       idx  = key % NofStacks;
    mem_block_elem_t&            elem = pools[idx];
    std::unique_lock<std::mutex> lock(elem.mutex);
    if (not elem.alloc.is_init()) {
      void* block = central_cache.allocate_node(central_cache.get_node_max_size());
      if (block == nullptr) {
        logger.warning("Failed to allocate memory block from central cache");
        return nullptr;
      }
      elem.key   = key;
      elem.alloc = linear_allocator(block, central_cache.get_node_max_size());
    }
    void* ptr = elem.alloc.allocate(size, alignment);
    if (ptr == nullptr) {
      logger.warning("No space left in memory block with key=%zd of circular stack pool", key);
    } else {
      elem.count++;
    }
    return ptr;
  }

  void deallocate(size_t key, void* p)
  {
    size_t                      idx  = key % NofStacks;
    mem_block_elem_t&           elem = pools[idx];
    std::lock_guard<std::mutex> lock(elem.mutex);
    elem.alloc.deallocate(p);
    elem.count--;
    if (elem.count == 0) {
      // return back to central cache
      void* ptr = elem.alloc.memblock_ptr();
      elem.clear();
      central_cache.deallocate_node(ptr);
    }
  }

  void allocate_batch() { central_cache.allocate_batch(); }

  size_t cache_size() const { return central_cache.cache_size(); }

private:
  srsran::circular_array<mem_block_elem_t, NofStacks> pools;
  srsran::background_mem_pool                         central_cache;
  srslog::basic_logger&                               logger;
};

template <typename T, size_t N, typename... Args>
unique_pool_ptr<T> make_pool_obj_with_fallback(circular_stack_pool<N>& pool, size_t key, Args&&... args)
{
  void* block = pool.allocate(key, sizeof(T), alignof(T));
  if (block == nullptr) {
    // allocated with "new" as a fallback
    return unique_pool_ptr<T>(new T(std::forward<Args>(args)...), std::default_delete<T>());
  }
  // allocation using memory pool was successful
  new (block) T(std::forward<Args>(args)...);
  return unique_pool_ptr<T>(static_cast<T*>(block), [key, &pool](T* ptr) {
    ptr->~T();
    pool.deallocate(key, ptr);
  });
}

} // namespace srsran

#endif // SRSRAN_CIRCULAR_MAP_STACK_POOL_H
