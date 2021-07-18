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

#include "srsenb/hdr/stack/mac/nr/sched_nr_harq.h"

namespace srsenb {
namespace sched_nr_impl {

bool harq_proc::ack_info(uint32_t tb_idx, bool ack)
{
  if (empty(tb_idx)) {
    return false;
  }
  tb[tb_idx].ack_state = ack;
  if (ack) {
    tb[tb_idx].active = false;
  }
  return true;
}

void harq_proc::new_tti(tti_point tti_rx)
{
  if (has_pending_retx(tti_rx) and nof_retx() + 1 >= max_nof_retx()) {
    tb[0].active = false;
  }
}

void harq_proc::reset()
{
  tb[0].ack_state = false;
  tb[0].active    = false;
  tb[0].n_rtx     = 0;
  tb[0].mcs       = std::numeric_limits<uint32_t>::max();
  tb[0].tbs       = std::numeric_limits<uint32_t>::max();
}

bool harq_proc::new_tx(tti_point        tti_tx_,
                       tti_point        tti_ack_,
                       const prb_grant& grant,
                       uint32_t         mcs,
                       uint32_t         tbs,
                       uint32_t         max_retx_)
{
  if (not empty()) {
    return false;
  }
  reset();
  max_retx     = max_retx_;
  tti_tx       = tti_tx_;
  tti_ack      = tti_ack_;
  prbs_        = grant;
  tb[0].ndi    = !tb[0].ndi;
  tb[0].mcs    = mcs;
  tb[0].tbs    = tbs;
  tb[0].active = true;
  return true;
}

bool harq_proc::set_tbs(uint32_t tbs)
{
  if (empty() or nof_retx() > 0) {
    return false;
  }
  tb[0].tbs = tbs;
  return true;
}

bool harq_proc::new_retx(tti_point tti_tx_, tti_point tti_ack_, const prb_grant& grant)
{
  if (grant.is_alloc_type0() != prbs_.is_alloc_type0() or
      (grant.is_alloc_type0() and grant.rbgs().count() != prbs_.rbgs().count()) or
      (grant.is_alloc_type1() and grant.prbs().length() == prbs_.prbs().length())) {
    return false;
  }
  if (new_retx(tti_tx_, tti_ack_)) {
    prbs_ = grant;
    return true;
  }
  return false;
}

bool harq_proc::new_retx(tti_point tti_tx_, tti_point tti_ack_)
{
  if (empty()) {
    return false;
  }
  tti_tx          = tti_tx_;
  tti_ack         = tti_ack_;
  tb[0].ack_state = false;
  tb[0].n_rtx++;
  return true;
}

harq_entity::harq_entity(uint32_t nprb, uint32_t nof_harq_procs)
{
  // Create HARQs
  dl_harqs.reserve(nof_harq_procs);
  ul_harqs.reserve(nof_harq_procs);
  for (uint32_t pid = 0; pid < nof_harq_procs; ++pid) {
    dl_harqs.emplace_back(pid, nprb);
    ul_harqs.emplace_back(pid, nprb);
  }
}

void harq_entity::new_tti(tti_point tti_rx_)
{
  tti_rx = tti_rx_;
  for (harq_proc& dl_h : dl_harqs) {
    dl_h.new_tti(tti_rx);
  }
  for (harq_proc& ul_h : ul_harqs) {
    ul_h.new_tti(tti_rx);
  }
}

} // namespace sched_nr_impl
} // namespace srsenb