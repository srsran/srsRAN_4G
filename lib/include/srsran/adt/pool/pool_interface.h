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

/// Common object pool interface
template <typename T>
class obj_pool_itf
{
public:
  struct pool_deallocator {
    obj_pool_itf<T>* pool;
    explicit pool_deallocator(obj_pool_itf<T>* pool_ = nullptr) : pool(pool_) {}
    void operator()(void* ptr)
    {
      if (ptr != nullptr and pool != nullptr) {
        pool->do_deallocate(ptr);
      }
    }
  };
  using object_type = T;

  obj_pool_itf() = default;
  // Object pool address should not change
  obj_pool_itf(const obj_pool_itf&) = delete;
  obj_pool_itf(obj_pool_itf&&)      = delete;
  obj_pool_itf& operator=(const obj_pool_itf&) = delete;
  obj_pool_itf& operator=(obj_pool_itf&&) = delete;

  virtual ~obj_pool_itf() = default;

  std::unique_ptr<T, pool_deallocator> make()
  {
    return std::unique_ptr<T, pool_deallocator>(do_allocate(), pool_deallocator(this));
  }

private:
  // defined in child class
  virtual T*   do_allocate()            = 0;
  virtual void do_deallocate(void* ptr) = 0;
};

/// unique ptr with type-erased dtor, so that it can be used by any object pool
template <typename T>
using unique_pool_ptr = std::unique_ptr<T, typename obj_pool_itf<T>::pool_deallocator>;

} // namespace srsran

#endif // SRSRAN_POOL_INTERFACE_H
