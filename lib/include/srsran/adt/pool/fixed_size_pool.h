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

#ifndef SRSRAN_FIXED_SIZE_POOL_H
#define SRSRAN_FIXED_SIZE_POOL_H

#include "memblock_cache.h"
#include "srsran/adt/circular_buffer.h"
#include <thread>

namespace srsran {

/**
 * Concurrent fixed size memory pool made of blocks of equal size
 * Each worker keeps a separate thread-local memory block cache that it uses for fast allocation/deallocation.
 * When this cache gets depleted, the worker tries to obtain blocks from a central memory block cache.
 * When accessing a thread local cache, no locks are required.
 * Since there is no stealing of blocks between workers, it is possible that a worker can't allocate while another
 * worker still has blocks in its own cache. To minimize the impact of this event, an upper bound is place on a worker
 * thread cache size. Once a worker reaches that upper bound, it sends half of its stored blocks to the central cache.
 * Note: Taking into account the usage of thread_local, this class is made a singleton
 * Note2: No considerations were made regarding false sharing between threads. It is assumed that the blocks are big
 *        enough to fill a cache line.
 * @tparam NofObjects number of objects in the pool
 * @tparam ObjSize object size
 */
template <size_t ObjSize, bool DebugSanitizeAddress = false>
class concurrent_fixed_memory_pool
{
  static_assert(ObjSize > 256, "This pool is particularly designed for large objects.");
  using pool_type = concurrent_fixed_memory_pool<ObjSize, DebugSanitizeAddress>;

  struct obj_storage_t {
    typename std::aligned_storage<ObjSize, alignof(detail::max_alignment_t)>::type buffer;
  };

  const static size_t batch_steal_size = 16;

  // ctor only accessible from singleton get_instance()
  explicit concurrent_fixed_memory_pool(size_t nof_objects_)
  {
    srsran_assert(nof_objects_ > batch_steal_size, "A positive pool size must be provided");

    std::lock_guard<std::mutex> lock(mutex);
    allocated_blocks.resize(nof_objects_);
    for (std::unique_ptr<obj_storage_t>& b : allocated_blocks) {
      b.reset(new obj_storage_t());
      srsran_assert(b.get() != nullptr, "Failed to instantiate fixed memory pool");
      central_mem_cache.push(static_cast<void*>(b.get()));
    }
    local_growth_thres = allocated_blocks.size() / 16;
    local_growth_thres = local_growth_thres < batch_steal_size ? batch_steal_size : local_growth_thres;
  }

public:
  const static size_t BLOCK_SIZE = ObjSize;

  concurrent_fixed_memory_pool(const concurrent_fixed_memory_pool&) = delete;
  concurrent_fixed_memory_pool(concurrent_fixed_memory_pool&&)      = delete;
  concurrent_fixed_memory_pool& operator=(const concurrent_fixed_memory_pool&) = delete;
  concurrent_fixed_memory_pool& operator=(concurrent_fixed_memory_pool&&) = delete;

  ~concurrent_fixed_memory_pool()
  {
    std::lock_guard<std::mutex> lock(mutex);
    allocated_blocks.clear();
  }

  static concurrent_fixed_memory_pool<ObjSize, DebugSanitizeAddress>* get_instance(size_t size = 4096)
  {
    static concurrent_fixed_memory_pool<ObjSize, DebugSanitizeAddress> pool(size);
    return &pool;
  }

  size_t size() { return allocated_blocks.size(); }

  void* allocate_node(size_t sz)
  {
    srsran_assert(sz <= ObjSize, "Allocated node size=%zd exceeds max object size=%zd", sz, ObjSize);
    worker_ctxt* worker_ctxt = get_worker_cache();

    void* node = worker_ctxt->cache.try_pop();
    if (node == nullptr) {
      // fill the thread local cache enough for this and next allocations
      std::array<void*, batch_steal_size> popped_blocks;
      size_t                              n = central_mem_cache.try_pop(popped_blocks);
      for (size_t i = 0; i < n; ++i) {
        new (popped_blocks[i]) obj_storage_t();
        worker_ctxt->cache.push(static_cast<void*>(popped_blocks[i]));
      }
      node = worker_ctxt->cache.try_pop();
    }

#ifdef SRSRAN_BUFFER_POOL_LOG_ENABLED
    if (node == nullptr) {
      print_error("Error allocating buffer in pool of ObjSize=%zd", ObjSize);
    }
#endif
    return node;
  }

  void deallocate_node(void* p)
  {
    srsran_assert(p != nullptr, "Deallocated nodes must have valid address");
    worker_ctxt*   worker_ctxt = get_worker_cache();
    obj_storage_t* block_ptr   = static_cast<obj_storage_t*>(p);

    if (DebugSanitizeAddress) {
      std::lock_guard<std::mutex> lock(mutex);
      srsran_assert(std::any_of(allocated_blocks.begin(),
                                allocated_blocks.end(),
                                [block_ptr](const std::unique_ptr<obj_storage_t>& b) { return b.get() == block_ptr; }),
                    "Error deallocating block with address 0x%lx",
                    (long unsigned)block_ptr);
    }

    // push to local memory block cache
    worker_ctxt->cache.push(static_cast<void*>(p));

    if (worker_ctxt->cache.size() >= local_growth_thres) {
      // if local cache reached max capacity, send half of the blocks to central cache
      central_mem_cache.steal_blocks(worker_ctxt->cache, worker_ctxt->cache.size() / 2);
    }
  }

  void enable_logger(bool enabled)
  {
    if (enabled) {
      logger = &srslog::fetch_basic_logger("POOL");
      logger->set_level(srslog::basic_levels::debug);
    } else {
      logger = nullptr;
    }
  }

  void print_all_buffers()
  {
    auto*  worker     = get_worker_cache();
    size_t tot_blocks = 0;
    {
      std::lock_guard<std::mutex> lock(mutex);
      tot_blocks = allocated_blocks.size();
    }
    printf("There are %zd/%zd buffers in shared block container. This thread contains %zd in its local cache\n",
           central_mem_cache.size(),
           tot_blocks,
           worker->cache.size());
  }

private:
  struct worker_ctxt {
    std::thread::id    id;
    free_memblock_list cache;

    worker_ctxt() : id(std::this_thread::get_id()) {}
    ~worker_ctxt()
    {
      concurrent_free_memblock_list& central_cache = pool_type::get_instance()->central_mem_cache;
      central_cache.steal_blocks(cache, cache.size());
    }
  };

  worker_ctxt* get_worker_cache()
  {
    thread_local worker_ctxt worker_cache;
    return &worker_cache;
  }

  /// Formats and prints the input string and arguments into the configured output stream.
  template <typename... Args>
  void print_error(const char* str, Args&&... args)
  {
    if (logger != nullptr) {
      logger->error(str, std::forward<Args>(args)...);
    } else {
      fmt::printf(std::string(str) + "\n", std::forward<Args>(args)...);
    }
  }

  size_t                local_growth_thres = 0;
  srslog::basic_logger* logger             = nullptr;

  concurrent_free_memblock_list                central_mem_cache;
  std::mutex                                   mutex;
  std::vector<std::unique_ptr<obj_storage_t> > allocated_blocks;
};

} // namespace srsran

#endif // SRSRAN_FIXED_SIZE_POOL_H
