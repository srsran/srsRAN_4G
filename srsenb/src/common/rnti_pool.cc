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
#include "srsenb/hdr/stack/mac/ue.h"
#include "srsenb/hdr/stack/rrc/rrc_endc.h"
#include "srsenb/hdr/stack/rrc/rrc_mobility.h"
#include "srsenb/hdr/stack/rrc/rrc_ue.h"
#include "srsran/adt/pool/circular_stack_pool.h"
#include "srsran/rlc/rlc.h"
#include "srsran/upper/pdcp.h"

namespace srsenb {

const static size_t UE_MEM_BLOCK_SIZE = 1024 + sizeof(ue) + sizeof(rrc::ue) + sizeof(rrc::ue::rrc_mobility) +
                                        sizeof(rrc::ue::rrc_endc) + sizeof(srsran::rlc) + sizeof(srsran::pdcp);

srsran::circular_stack_pool<SRSENB_MAX_UES>* get_rnti_pool()
{
  static std::unique_ptr<srsran::circular_stack_pool<SRSENB_MAX_UES> > pool(
      new srsran::circular_stack_pool<SRSENB_MAX_UES>(8, UE_MEM_BLOCK_SIZE, 4));
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
