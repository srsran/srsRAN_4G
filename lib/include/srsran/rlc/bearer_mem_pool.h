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
