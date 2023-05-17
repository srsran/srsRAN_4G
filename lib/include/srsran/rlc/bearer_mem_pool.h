/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSRAN_BEARER_MEM_POOL_H
#define SRSRAN_BEARER_MEM_POOL_H

#include <cstddef>

namespace srsran {

// Allocation of objects in rnti-dedicated memory pool
void  reserve_rlc_memblocks(size_t nof_blocks);
void* allocate_rlc_bearer(std::size_t size);
void  deallocate_rlc_bearer(void* p);

} // namespace srsran

#endif // SRSRAN_BEARER_MEM_POOL_H
