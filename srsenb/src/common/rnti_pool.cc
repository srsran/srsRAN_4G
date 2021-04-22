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

#include "srsenb/hdr/common/rnti_pool.h"
#include "srsenb/hdr/common/common_enb.h"
#include "srsran/adt/pool/circular_stack_pool.h"

namespace srsenb {

srsran::circular_stack_pool<SRSENB_MAX_UES>* get_rnti_pool()
{
  static std::unique_ptr<srsran::circular_stack_pool<SRSENB_MAX_UES> > pool(
      new srsran::circular_stack_pool<SRSENB_MAX_UES>(8, 32768, 4));
  return pool.get();
}

void reserve_rnti_memblocks(size_t nof_blocks)
{
  while (get_rnti_pool()->cache_size() < nof_blocks) {
    get_rnti_pool()->allocate_batch();
  }
}

void* allocate_rnti_dedicated_mem(uint16_t rnti, size_t size, size_t align)
{
  return get_rnti_pool()->allocate(rnti, size, align);
}
void deallocate_rnti_dedicated_mem(uint16_t rnti, void* ptr)
{
  get_rnti_pool()->deallocate(rnti, ptr);
}

} // namespace srsenb
