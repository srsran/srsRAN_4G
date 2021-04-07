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
