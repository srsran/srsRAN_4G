/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include "srsran/rlc/bearer_mem_pool.h"
#include "srsran/adt/pool/batch_mem_pool.h"
#include "srsran/rlc/rlc_am_lte.h"
#include "srsran/rlc/rlc_am_nr.h"
#include "srsran/rlc/rlc_tm.h"
#include "srsran/rlc/rlc_um_lte.h"
#include "srsran/rlc/rlc_um_nr.h"

namespace srsran {

srsran::background_mem_pool* get_bearer_pool()
{
  static background_mem_pool pool(
      4,
      std::max(std::max(std::max(std::max(sizeof(rlc_am), sizeof(rlc_am)), sizeof(rlc_um_lte)), sizeof(rlc_um_nr)),
               sizeof(rlc_tm)),
      8,
      8);
  return &pool;
}

void reserve_rlc_memblocks(size_t nof_blocks)
{
  srsran::background_mem_pool* pool = get_bearer_pool();
  while (pool->cache_size() < nof_blocks) {
    pool->allocate_batch();
  }
}
void* allocate_rlc_bearer(std::size_t sz)
{
  return get_bearer_pool()->allocate_node(sz);
}
void deallocate_rlc_bearer(void* p)
{
  get_bearer_pool()->deallocate_node(p);
}

} // namespace srsran
