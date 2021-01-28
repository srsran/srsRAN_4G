/**
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsenb/hdr/stack/mac/schedulers/sched_base.h"

namespace srsenb {

rbgmask_t find_available_dl_rbgs(uint32_t max_rbgs, const rbgmask_t& current_mask)
{
  if (max_rbgs == 0 or current_mask.all()) {
    return rbgmask_t{};
  }
  // 1's for free rbgs
  rbgmask_t localmask = ~(current_mask);

  uint32_t i = 0, nof_alloc = 0;
  for (; i < localmask.size() and nof_alloc < max_rbgs; ++i) {
    if (localmask.test(i)) {
      nof_alloc++;
    }
  }
  localmask.fill(i, localmask.size(), false);
  return localmask;
}

prb_interval find_contiguous_ul_prbs(uint32_t L, const prbmask_t& current_mask)
{
  prb_interval prb_interv, prb_interv2;
  for (uint32_t n = 0; n < current_mask.size() and prb_interv.length() < L; n++) {
    if (not current_mask.test(n) and prb_interv.length() == 0) {
      // new interval
      prb_interv.set(n, n + 1);
    } else if (not current_mask.test(n)) {
      // extend current interval
      prb_interv.resize_by(1);
    } else if (prb_interv.length() > 0) {
      // reset interval
      prb_interv2 = prb_interv.length() > prb_interv2.length() ? prb_interv : prb_interv2;
      prb_interv  = {};
    }
  }
  prb_interv = prb_interv2.length() > prb_interv.length() ? prb_interv2 : prb_interv;
  if (prb_interv.empty()) {
    return prb_interv;
  }

  // Make sure L is allowed by SC-FDMA modulation
  prb_interv2 = prb_interv;
  while (not srslte_dft_precoding_valid_prb(prb_interv.length()) and prb_interv.stop() < current_mask.size() and
         not current_mask.test(prb_interv.stop())) {
    prb_interv.resize_by(1);
  }
  if (not srslte_dft_precoding_valid_prb(prb_interv.length())) {
    // if length increase failed, try to decrease
    prb_interv = prb_interv2;
    prb_interv.resize_by(-1);
    while (not srslte_dft_precoding_valid_prb(prb_interv.length()) and not prb_interv.empty()) {
      prb_interv.resize_by(-1);
    }
  }
  return prb_interv;
}

int get_ue_cc_idx_if_pdsch_enabled(const sched_ue& user, sf_sched* tti_sched)
{
  // Do not allocate a user multiple times in the same tti
  if (tti_sched->is_dl_alloc(user.get_rnti())) {
    return -1;
  }
  // Do not allocate a user to an inactive carrier
  auto p = user.get_active_cell_index(tti_sched->get_enb_cc_idx());
  if (not p.first) {
    return -1;
  }
  uint32_t cell_idx = p.second;
  // Do not allow allocations when PDSCH is deactivated
  if (not user.pdsch_enabled(tti_sched->get_tti_rx(), tti_sched->get_enb_cc_idx())) {
    return -1;
  }
  return cell_idx;
}
const dl_harq_proc* get_dl_retx_harq(sched_ue& user, sf_sched* tti_sched)
{
  if (get_ue_cc_idx_if_pdsch_enabled(user, tti_sched) < 0) {
    return nullptr;
  }
  dl_harq_proc* h = user.get_pending_dl_harq(tti_sched->get_tti_tx_dl(), tti_sched->get_enb_cc_idx());
  return h;
}
const dl_harq_proc* get_dl_newtx_harq(sched_ue& user, sf_sched* tti_sched)
{
  if (get_ue_cc_idx_if_pdsch_enabled(user, tti_sched) < 0) {
    return nullptr;
  }
  return user.get_empty_dl_harq(tti_sched->get_tti_tx_dl(), tti_sched->get_enb_cc_idx());
}

int get_ue_cc_idx_if_pusch_enabled(const sched_ue& user, sf_sched* tti_sched, bool needs_pdcch)
{
  // Do not allocate a user multiple times in the same tti
  if (tti_sched->is_ul_alloc(user.get_rnti())) {
    return -1;
  }
  // Do not allocate a user to an inactive carrier
  auto p = user.get_active_cell_index(tti_sched->get_enb_cc_idx());
  if (not p.first) {
    return -1;
  }
  uint32_t cell_idx = p.second;
  // Do not allow allocations when PDSCH is deactivated
  if (not user.pusch_enabled(tti_sched->get_tti_rx(), tti_sched->get_enb_cc_idx(), needs_pdcch)) {
    return -1;
  }
  return cell_idx;
}
const ul_harq_proc* get_ul_retx_harq(sched_ue& user, sf_sched* tti_sched)
{
  if (get_ue_cc_idx_if_pusch_enabled(user, tti_sched, false) < 0) {
    return nullptr;
  }
  const ul_harq_proc* h = user.get_ul_harq(tti_sched->get_tti_tx_ul(), tti_sched->get_enb_cc_idx());
  return h->has_pending_retx() ? h : nullptr;
}
const ul_harq_proc* get_ul_newtx_harq(sched_ue& user, sf_sched* tti_sched)
{
  if (get_ue_cc_idx_if_pusch_enabled(user, tti_sched, true) < 0) {
    return nullptr;
  }
  const ul_harq_proc* h = user.get_ul_harq(tti_sched->get_tti_tx_ul(), tti_sched->get_enb_cc_idx());
  return h->is_empty() ? h : nullptr;
}

alloc_outcome_t try_dl_retx_alloc(sf_sched& tti_sched, sched_ue& ue, const dl_harq_proc& h)
{
  // Try to reuse the same mask
  rbgmask_t       retx_mask = h.get_rbgmask();
  alloc_outcome_t code      = tti_sched.alloc_dl_user(&ue, retx_mask, h.get_id());
  if (code == alloc_outcome_t::SUCCESS or code == alloc_outcome_t::DCI_COLLISION) {
    return code;
  }

  // If previous mask does not fit, find another with exact same number of rbgs
  size_t nof_rbg = retx_mask.count();
  retx_mask      = find_available_dl_rbgs(nof_rbg, tti_sched.get_dl_mask());
  if (retx_mask.count() == nof_rbg) {
    return tti_sched.alloc_dl_user(&ue, retx_mask, h.get_id());
  }
  return alloc_outcome_t::RB_COLLISION;
}

alloc_outcome_t try_ul_retx_alloc(sf_sched& tti_sched, sched_ue& ue, const ul_harq_proc& h)
{
  // If can schedule the same mask, do it
  prb_interval    alloc = h.get_alloc();
  alloc_outcome_t ret   = tti_sched.alloc_ul_user(&ue, alloc);
  if (ret == alloc_outcome_t::SUCCESS or ret == alloc_outcome_t::DCI_COLLISION) {
    return ret;
  }

  // Avoid measGaps accounting for PDCCH
  if (not ue.pusch_enabled(tti_sched.get_tti_rx(), tti_sched.get_enb_cc_idx(), true)) {
    return alloc_outcome_t::MEASGAP_COLLISION;
  }
  uint32_t nof_prbs = alloc.length();
  alloc             = find_contiguous_ul_prbs(nof_prbs, tti_sched.get_ul_mask());
  if (alloc.length() != nof_prbs) {
    return alloc_outcome_t::RB_COLLISION;
  }
  return tti_sched.alloc_ul_user(&ue, alloc);
}

} // namespace srsenb
