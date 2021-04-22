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

#ifndef SRSRAN_BATCH_MEM_POOL_H
#define SRSRAN_BATCH_MEM_POOL_H

#include "memblock_cache.h"
#include "pool_utils.h"
#include "srsran/common/srsran_assert.h"
#include "srsran/common/thread_pool.h"
#include <memory>
#include <mutex>

namespace srsran {

/**
 * Non-thread-safe, node-based memory pool that allocates nodes in batches of "objs_per_batch" > 1, and caches
 * allocated blocks on deallocation
 */
class growing_batch_mem_pool
{
public:
  explicit growing_batch_mem_pool(size_t objs_per_batch_,
                                  size_t node_size_,
                                  size_t node_alignment_,
                                  int    init_size = -1) :
    objs_per_batch(objs_per_batch_),
    memblock_size(std::max(node_size_, free_memblock_list::min_memblock_size())),
    allocated(objs_per_batch * memblock_size, std::max(node_alignment_, free_memblock_list::min_memblock_align()))
  {
    size_t N = init_size < 0 ? objs_per_batch_ : init_size;
    while (N > cache_size()) {
      allocate_batch();
    }
  }
  ~growing_batch_mem_pool()
  {
    srsran_assert(cache_size() == size(), "Not all nodes have been deallocated yet (%zd < %zd)", cache_size(), size());
  }

  size_t get_node_max_size() const { return memblock_size; }

  void clear()
  {
    free_list.clear();
    allocated.clear();
  }

  size_t cache_size() const { return free_list.size(); }
  size_t size() const { return allocated.size() * objs_per_batch; }

  void allocate_batch()
  {
    uint8_t* batch_payload = static_cast<uint8_t*>(allocated.allocate_block());
    for (size_t i = 0; i < objs_per_batch; ++i) {
      void* cache_node = batch_payload + i * memblock_size;
      free_list.push(cache_node);
    }
  }

  void* allocate_node()
  {
    if (free_list.empty()) {
      allocate_batch();
    }
    return free_list.pop();
  }

  void deallocate_node(void* ptr) { free_list.push(ptr); }

private:
  const size_t objs_per_batch;
  const size_t memblock_size;

  memblock_stack     allocated;
  free_memblock_list free_list;
};

/**
 * Thread-safe object pool specialized in allocating batches of objects in a preemptive way in a background thread
 * to minimize latency.
 * Note: The dispatched allocation jobs may outlive the pool. To handle this, the pool state is passed to jobs via a
 *       shared ptr.
 */
class background_mem_pool
{
public:
  const size_t batch_threshold;

  explicit background_mem_pool(size_t nodes_per_batch_, size_t node_size_, size_t thres_, int initial_size = -1) :
    batch_threshold(thres_),
    state(std::make_shared<detached_pool_state>(this)),
    grow_pool(nodes_per_batch_, node_size_, detail::max_alignment, initial_size)
  {
    srsran_assert(batch_threshold > 1, "Invalid arguments for background memory pool");
  }
  ~background_mem_pool()
  {
    std::lock_guard<std::mutex> lock(state->mutex);
    state->pool = nullptr;
    grow_pool.clear();
  }

  /// alloc new object space. If no memory is pre-reserved in the pool, malloc is called to allocate new batch.
  void* allocate_node(size_t sz)
  {
    srsran_assert(sz <= grow_pool.get_node_max_size(),
                  "Mismatch of allocated node size=%zd and object size=%zd",
                  sz,
                  grow_pool.get_node_max_size());
    std::lock_guard<std::mutex> lock(state->mutex);
    void*                       node = grow_pool.allocate_node();

    if (grow_pool.size() < batch_threshold) {
      allocate_batch_in_background_unlocked();
    }
    return node;
  }

  void deallocate_node(void* p)
  {
    std::lock_guard<std::mutex> lock(state->mutex);
    grow_pool.deallocate_node(p);
  }

  void allocate_batch()
  {
    std::lock_guard<std::mutex> lock(state->mutex);
    grow_pool.allocate_batch();
  }

  size_t get_node_max_size() const { return grow_pool.get_node_max_size(); }
  size_t cache_size() const
  {
    std::lock_guard<std::mutex> lock(state->mutex);
    return grow_pool.cache_size();
  }

private:
  void allocate_batch_in_background_unlocked()
  {
    if (state->dispatched) {
      // new batch allocation already ongoing
      return;
    }
    state->dispatched                               = true;
    std::shared_ptr<detached_pool_state> state_sptr = state;
    get_background_workers().push_task([state_sptr]() {
      std::lock_guard<std::mutex> lock(state_sptr->mutex);
      // check if pool has not been destroyed
      if (state_sptr->pool != nullptr) {
        auto* pool = state_sptr->pool;
        do {
          pool->grow_pool.allocate_batch();
        } while (pool->grow_pool.cache_size() < pool->batch_threshold);
      }
      state_sptr->dispatched = false;
    });
  }

  // State is stored in a shared_ptr that may outlive the pool.
  struct detached_pool_state {
    std::mutex           mutex;
    background_mem_pool* pool;
    bool                 dispatched = false;
    explicit detached_pool_state(background_mem_pool* pool_) : pool(pool_) {}
  };
  std::shared_ptr<detached_pool_state> state;

  growing_batch_mem_pool grow_pool;
};

} // namespace srsran

#endif // SRSRAN_BATCH_MEM_POOL_H
