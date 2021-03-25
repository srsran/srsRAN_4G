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
template <size_t ObjSize, bool DebugSanitizeAddress = false>
class concurrent_fixed_memory_pool
{
  static_assert(ObjSize > 256, "This pool is particularly designed for large objects");

  struct obj_storage_t {
    typename std::aligned_storage<ObjSize, alignof(detail::max_alignment_t)>::type buffer;
    std::thread::id                                                                worker_id;
    explicit obj_storage_t(std::thread::id id_) : worker_id(id_) {}
  };

  const static size_t batch_steal_size = 10;

  // ctor only accessible from singleton get_instance()
  explicit concurrent_fixed_memory_pool(size_t nof_objects_)
  {
    srsran_assert(nof_objects_ > 0, "A positive pool size must be provided");

    std::lock_guard<std::mutex> lock(mutex);
    allocated_blocks.resize(nof_objects_);
    for (std::unique_ptr<obj_storage_t>& b : allocated_blocks) {
      b.reset(new obj_storage_t(std::this_thread::get_id()));
      srsran_assert(b.get() != nullptr, "Failed to instantiate fixed memory pool");
      shared_mem_cache.push(static_cast<void*>(b.get()));
    }
    max_objs_per_cache = allocated_blocks.size() / 16;
    max_objs_per_cache = max_objs_per_cache < batch_steal_size ? batch_steal_size : max_objs_per_cache;
  }

public:
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
    memblock_cache* worker_cache = get_worker_cache();
    obj_storage_t*  block_ptr    = static_cast<obj_storage_t*>(p);

    if (DebugSanitizeAddress) {
      std::lock_guard<std::mutex> lock(mutex);
      srsran_assert(std::any_of(allocated_blocks.begin(),
                                allocated_blocks.end(),
                                [block_ptr](const std::unique_ptr<obj_storage_t>& b) { return b.get() == block_ptr; }),
                    "Error deallocating block with address 0x%lx.",
                    (long unsigned)block_ptr);
    }

    if (block_ptr->worker_id != std::this_thread::get_id() or worker_cache->size() >= max_objs_per_cache) {
      // if block was allocated in a different thread or local cache reached max capacity, send block to shared
      // container
      shared_mem_cache.push(static_cast<void*>(block_ptr));
      return;
    }

    // push to local memory block cache
    worker_cache->push(static_cast<uint8_t*>(p));
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
    std::lock_guard<std::mutex> lock(mutex);
    printf("There are %zd/%zd buffers in shared block container.\n", shared_mem_cache.size(), allocated_blocks.size());
  }

private:
  memblock_cache* get_worker_cache()
  {
    thread_local memblock_cache worker_cache;
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

  size_t                max_objs_per_cache = 0;
  srslog::basic_logger* logger             = nullptr;

  mutexed_memblock_cache                       shared_mem_cache;
  std::mutex                                   mutex;
  std::vector<std::unique_ptr<obj_storage_t> > allocated_blocks;
};

} // namespace srsran

#endif // SRSRAN_FIXED_SIZE_POOL_H
