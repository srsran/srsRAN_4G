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

#ifndef SRSRAN_BACKGROUND_MEM_POOL_H
#define SRSRAN_BACKGROUND_MEM_POOL_H

#include "memblock_cache.h"
#include "pool_utils.h"
#include "srsran/common/srsran_assert.h"
#include "srsran/common/thread_pool.h"
#include <memory>
#include <mutex>
#include <vector>

namespace srsran {

namespace detail {

/**
 * Pool specialized for in allocating batches of objects in a preemptive way in a background thread to minimize latency.
 * Note: Current implementation assumes that the pool object will outlive the background callbacks to allocate new
 *       batches
 * @tparam T individual object type that is being allocated
 * @tparam BatchSize number of T objects in a batch
 * @tparam ThresholdSize number of T objects below which a new batch needs to be allocated
 */
template <typename T, size_t BatchSize, size_t ThresholdSize, typename CtorFunc, typename RecycleFunc>
class base_background_pool
{
  static_assert(ThresholdSize > 0, "ThresholdSize needs to be positive");
  static_assert(BatchSize > 1, "BatchSize needs to be higher than 1");
  using pool_type = base_background_pool<T, BatchSize, ThresholdSize, CtorFunc, RecycleFunc>;

public:
  explicit base_background_pool(size_t      initial_size  = BatchSize,
                                CtorFunc    ctor_func_    = {},
                                RecycleFunc recycle_func_ = {}) :
    ctor_func(ctor_func_), recycle_func(recycle_func_), state(std::make_shared<detached_pool_state>(this))
  {
    int nof_batches = ceilf(initial_size / (float)BatchSize);
    while (nof_batches-- > 0) {
      allocate_batch_();
    }
  }
  base_background_pool(base_background_pool&&)      = delete;
  base_background_pool(const base_background_pool&) = delete;
  base_background_pool& operator=(base_background_pool&&) = delete;
  base_background_pool& operator=(const base_background_pool&) = delete;
  ~base_background_pool()
  {
    std::lock_guard<std::mutex> lock(state->mutex);
    state->pool = nullptr;
    for (std::unique_ptr<batch_obj_t>& batch : batches) {
      for (obj_storage_t& obj_store : *batch) {
        obj_store.destroy();
      }
    }
    batches.clear();
  }

  /// alloc new object space. If no memory is pre-reserved in the pool, malloc is called to allocate new batch.
  void* allocate_node(size_t sz)
  {
    srsran_assert(sz == sizeof(T), "Mismatch of allocated node size=%zd and object size=%zd", sz, sizeof(T));
    std::lock_guard<std::mutex> lock(state->mutex);
    void*                       block = obj_cache.try_pop();

    if (block != nullptr) {
      // allocation successful
      if (obj_cache.size() < ThresholdSize) {
        allocate_batch_in_background();
      }
      return block;
    }

    // try allocation of new batch in same thread as caller.
    allocate_batch_();
    return obj_cache.try_pop();
  }

  void deallocate_node(void* p)
  {
    std::lock_guard<std::mutex> lock(state->mutex);
    recycle_func(static_cast<void*>(p));
    obj_cache.push(static_cast<void*>(p));
  }

  void allocate_batch_in_background()
  {
    std::shared_ptr<detached_pool_state> state_copy = state;
    get_background_workers().push_task([state_copy]() {
      std::lock_guard<std::mutex> lock(state_copy->mutex);
      if (state_copy->pool != nullptr) {
        state_copy->pool->allocate_batch_();
      }
    });
  }

private:
  using obj_storage_t = type_storage<T, memblock_cache::min_memblock_size(), memblock_cache::min_memblock_align()>;
  using batch_obj_t   = std::array<obj_storage_t, BatchSize>;

  /// Unprotected allocation of new Batch of Objects
  void allocate_batch_()
  {
    std::unique_ptr<batch_obj_t> batch(new batch_obj_t());
    if (batch == nullptr) {
      srslog::fetch_basic_logger("POOL").warning("Failed to allocate new batch in background thread");
      return;
    }
    for (obj_storage_t& obj_store : *batch) {
      ctor_func(obj_store.addr());
      obj_cache.push(&obj_store.buffer);
    }
    batches.emplace_back(std::move(batch));
  }

  CtorFunc    ctor_func;
  RecycleFunc recycle_func;

  struct detached_pool_state {
    std::mutex mutex;
    pool_type* pool;
    explicit detached_pool_state(pool_type* pool_) : pool(pool_) {}
  };
  std::shared_ptr<detached_pool_state> state;

  // memory stack to cache allocate memory chunks
  memblock_cache                             obj_cache;
  std::vector<std::unique_ptr<batch_obj_t> > batches;
};

} // namespace detail

template <typename T, size_t BatchSize, size_t ThresholdSize>
using background_mem_pool =
    detail::base_background_pool<T, BatchSize, ThresholdSize, detail::noop_operator, detail::noop_operator>;

template <typename T,
          size_t BatchSize,
          size_t ThresholdSize,
          typename CtorFunc    = detail::default_ctor_operator<T>,
          typename RecycleFunc = detail::noop_operator>
class background_obj_pool
{
  using pool_type     = background_obj_pool<T, BatchSize, ThresholdSize, CtorFunc, RecycleFunc>;
  using mem_pool_type = detail::base_background_pool<T, BatchSize, ThresholdSize, CtorFunc, RecycleFunc>;

  struct pool_deleter {
    mem_pool_type* pool;
    explicit pool_deleter(mem_pool_type* pool_) : pool(pool_) {}
    void operator()(void* ptr)
    {
      if (ptr != nullptr) {
        pool->deallocate_node(ptr);
      }
    }
  };

public:
  explicit background_obj_pool(size_t initial_size, CtorFunc&& ctor_func = {}, RecycleFunc&& recycle_func = {}) :
    pool(initial_size, std::forward<CtorFunc>(ctor_func), std::forward<RecycleFunc>(recycle_func))
  {}

  unique_pool_ptr<T> allocate_object()
  {
    void* ptr = pool.allocate_node(sizeof(T));
    return std::unique_ptr<T, pool_deleter>(static_cast<T*>(ptr), pool_deleter(&pool));
  }

private:
  mem_pool_type pool;
};

} // namespace srsran

#endif // SRSRAN_BACKGROUND_MEM_POOL_H
