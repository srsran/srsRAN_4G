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

#ifndef SRSRAN_POOL_INTERFACE_H
#define SRSRAN_POOL_INTERFACE_H

#include "srsran/adt/move_callback.h"

namespace srsran {

/// unique ptr with type-erased dtor, so that it can be used by any object or memory pool
constexpr size_t unique_pool_deleter_small_buffer = sizeof(void*) * 2u;
template <typename T>
using unique_pool_ptr = std::unique_ptr<T, srsran::move_callback<void(T*), unique_pool_deleter_small_buffer> >;

/// Common object pool interface
template <typename T>
class obj_pool_itf
{
public:
  using object_type = T;

  obj_pool_itf() = default;
  // Object pool address should not change
  obj_pool_itf(const obj_pool_itf&) = delete;
  obj_pool_itf(obj_pool_itf&&)      = delete;
  obj_pool_itf& operator=(const obj_pool_itf&) = delete;
  obj_pool_itf& operator=(obj_pool_itf&&) = delete;

  virtual ~obj_pool_itf()           = default;
  virtual unique_pool_ptr<T> make() = 0;
};

/// Allocate object in memory pool
template <typename T, typename MemPool, typename... Args>
unique_pool_ptr<T> make_pool_obj_with_heap_fallback(MemPool& mempool, Args&&... args)
{
  void* block = mempool.allocate(sizeof(T), alignof(T));
  if (block == nullptr) {
    return unique_pool_ptr<T>(new T(std::forward<Args>(args)...), std::default_delete<T>());
  }
  new (block) T(std::forward<Args>(args)...);
  return unique_pool_ptr<T>(block, [&mempool](T* ptr) {
    if (ptr != nullptr) {
      ptr->~T();
      mempool.deallocate(ptr);
    }
  });
}

} // namespace srsran

#endif // SRSRAN_POOL_INTERFACE_H
