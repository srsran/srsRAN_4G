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

#include "srsenb/hdr/stack/mac/nr/sched_nr_harq.h"

namespace srsenb {
namespace sched_nr_impl {

bool harq_proc::new_tx(tti_point tti_tx_, const rbgmask_t& rbgmask_, uint32_t mcs, uint32_t ack_delay_)
{
  if (not empty()) {
    return false;
  }
  tti_tx    = tti_tx_;
  ack_delay = ack_delay_;
  rbgmask   = rbgmask_;
  tb[0].mcs = mcs;
  return true;
}

harq_entity::harq_entity()
{
  dl_harqs.reserve(SCHED_NR_NOF_HARQS);
  ul_harqs.reserve(SCHED_NR_NOF_HARQS);
  for (uint32_t pid = 0; pid < SCHED_NR_NOF_HARQS; ++pid) {
    dl_harqs.emplace_back(pid);
    ul_harqs.emplace_back(pid);
  }
}

void harq_entity::new_tti(tti_point tti_rx_)
{
  tti_rx = tti_rx_;
}

} // namespace sched_nr_impl
} // namespace srsenb