/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_FIXED_SIZE_POOL_H
#define SRSRAN_FIXED_SIZE_POOL_H

#include "memblock_cache.h"
#include "srsran/adt/circular_buffer.h"
#include "srsran/adt/singleton.h"
#include <thread>

namespace srsran {

/**
 * Concurrent fixed size memory pool made of blocks of equal size
 * Each worker keeps a separate thread-local memory block cache that uses for fast allocation/deallocation.
 * When this cache gets depleted, the worker tries to obtain blocks from a shared memory block cache
 * Note: This pool does not implement stealing of blocks between workers, so it is possible that a worker can't allocate
 *       while another worker still has blocks in its own cache. This situation is avoided by upper bounding the
 *       size of each worker cache
 * Note2: Taking into account the usage of thread_local, this class is made a singleton
 * @tparam NofObjects number of objects in the pool
 * @tparam ObjSize object size
 */
template <size_t NofObjects, size_t ObjSize, size_t MaxWorkerCacheSize = NofObjects / 16>
class concurrent_fixed_memory_pool : public singleton_t<concurrent_fixed_memory_pool<NofObjects, ObjSize> >
{
  static_assert(NofObjects > 256, "This pool is particularly designed for a high number of objects");
  static_assert(ObjSize > 256, "This pool is particularly designed for large objects");

  struct obj_storage_t {
    typename std::aligned_storage<ObjSize, alignof(detail::max_alignment_t)>::type buffer;
    std::thread::id                                                                worker_id;
    explicit obj_storage_t(std::thread::id id_) : worker_id(id_) {}
  };

  const static size_t batch_steal_size = 10;

protected:
  // ctor only accessible from singleton
  concurrent_fixed_memory_pool()
  {
    allocated_blocks.resize(NofObjects);
    for (std::unique_ptr<obj_storage_t>& b : allocated_blocks) {
      b.reset(new obj_storage_t(std::this_thread::get_id()));
      srsran_assert(b.get() != nullptr, "Failed to instantiate fixed memory pool");
      shared_mem_cache.push(static_cast<void*>(b.get()));
    }
  }

public:
  static size_t size() { return NofObjects; }

  void* allocate_node(size_t sz)
  {
    srsran_assert(sz <= ObjSize, "Allocated node size=%zd exceeds max object size=%zd", sz, ObjSize);
    memblock_cache* worker_cache = get_worker_cache();

    void* node = worker_cache->try_pop();
    if (node == nullptr) {
      // fill the thread local cache enough for this and next allocations
      std::array<void*, batch_steal_size> popped_blocks;
      size_t                              n = shared_mem_cache.try_pop(popped_blocks);
      for (size_t i = 0; i < n; ++i) {
        new (popped_blocks[i]) obj_storage_t(std::this_thread::get_id());
        worker_cache->push(static_cast<uint8_t*>(popped_blocks[i]));
      }
      node = worker_cache->try_pop();
    }
    return node;
  }

  void deallocate_node(void* p)
  {
    srsran_assert(p != nullptr, "Deallocated nodes must have valid address");
    memblock_cache* worker_cache = get_worker_cache();
    obj_storage_t*  block_ptr    = static_cast<obj_storage_t*>(p);

    if (block_ptr->worker_id != std::this_thread::get_id() or worker_cache->size() >= MaxWorkerCacheSize) {
      // if block was allocated in a different thread or local cache reached max capacity, send block to shared
      // container
      shared_mem_cache.push(static_cast<void*>(block_ptr));
      return;
    }

    // push to local memory block cache
    worker_cache->push(static_cast<uint8_t*>(p));
  }

private:
  memblock_cache* get_worker_cache()
  {
    thread_local memblock_cache worker_cache;
    return &worker_cache;
  }

  mutexed_memblock_cache                       shared_mem_cache;
  std::mutex                                   mutex;
  std::vector<std::unique_ptr<obj_storage_t> > allocated_blocks;
};

} // namespace srsran

#endif // SRSRAN_FIXED_SIZE_POOL_H
