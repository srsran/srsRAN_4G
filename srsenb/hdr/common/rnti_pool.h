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

#ifndef SRSRAN_RNTI_POOL_H
#define SRSRAN_RNTI_POOL_H

#include "srsran/adt/pool/pool_interface.h"
#include "srsran/phy/common/phy_common.h"
#include <memory>

namespace srsenb {

// Allocation of objects in rnti-dedicated memory pool
void  reserve_rnti_memblocks(size_t nof_blocks);
void* allocate_rnti_dedicated_mem(uint16_t rnti, std::size_t size, std::size_t align);
void  deallocate_rnti_dedicated_mem(uint16_t rnti, void* p);

template <typename T>
using unique_rnti_ptr = srsran::unique_pool_ptr<T>;

template <typename T, typename... Args>
unique_rnti_ptr<T> make_rnti_obj(uint16_t rnti, Args&&... args)
{
  void* block = allocate_rnti_dedicated_mem(rnti, sizeof(T), alignof(T));
  if (block == nullptr) {
    // allocated with "new" as a fallback
    return unique_rnti_ptr<T>(new T(std::forward<Args>(args)...), std::default_delete<T>());
  }
  // allocation using rnti-dedicated memory pool was successful
  new (block) T(std::forward<Args>(args)...);
  return unique_rnti_ptr<T>(static_cast<T*>(block), [rnti](T* ptr) {
    ptr->~T();
    deallocate_rnti_dedicated_mem(rnti, ptr);
  });
}

} // namespace srsenb

#endif // SRSRAN_RNTI_POOL_H
