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

#ifndef SRSRAN_COMMON_POOL_H
#define SRSRAN_COMMON_POOL_H

#include "srsran/adt/move_callback.h"

namespace srsran {

/// unique ptr with type-erased dtor, so that it can be used by any pool
template <typename T>
using unique_pool_ptr = std::unique_ptr<T, srsran::move_callback<void(void*)> >;

/// Common object pool interface
template <typename T>
class obj_pool_itf
{
public:
  using object_type = T;

  obj_pool_itf()                    = default;
  obj_pool_itf(const obj_pool_itf&) = delete;
  obj_pool_itf(obj_pool_itf&&)      = delete;
  obj_pool_itf& operator=(const obj_pool_itf&) = delete;
  obj_pool_itf& operator=(obj_pool_itf&&) = delete;

  virtual ~obj_pool_itf()                      = default;
  virtual unique_pool_ptr<T> allocate_object() = 0;
};

} // namespace srsran

#endif // SRSRAN_COMMON_POOL_H
