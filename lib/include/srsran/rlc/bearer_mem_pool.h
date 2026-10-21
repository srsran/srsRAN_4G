// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

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
