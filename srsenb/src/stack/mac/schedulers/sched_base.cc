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

#include "srsenb/hdr/stack/mac/schedulers/sched_base.h"

namespace srsenb {

/*********************************
 * Common UL/DL Helper methods
 ********************************/

template <typename RBMask,
          typename RBInterval =
              typename std::conditional<std::is_same<RBMask, prbmask_t>::value, prb_interval, rbg_interval>::type>
RBInterval find_contiguous_interval(const RBMask& in_mask, uint32_t max_size)
{
  RBInterval max_interv;

  for (size_t n = 0; n < in_mask.size();) {
    int pos = in_mask.find_lowest(n, in_mask.size(), false);
    if (pos < 0) {
      break;
    }

    size_t     max_pos = std::min(in_mask.size(), (size_t)pos + max_size);
    int        pos2    = in_mask.find_lowest(pos, max_pos, true);
    RBInterval interv(pos, pos2 < 0 ? max_pos : pos2);
    if (interv.length() >= max_size) {
      return interv;
    }
    if (interv.length() > max_interv.length()) {
      max_interv = interv;
    }
    n = interv.stop();
  }
  return max_interv;
}

/****************************
 *    DL Helper methods
 ***************************/

rbgmask_t find_available_rb_mask(const rbgmask_t& in_mask, uint32_t max_size)
{
  // 1's for free RBs
  rbgmask_t localmask = ~(in_mask);

  uint32_t i = 0, nof_alloc = 0;
  for (; i < localmask.size() and nof_alloc < max_size; ++i) {
    if (localmask.test(i)) {
      nof_alloc++;
    }
  }
  localmask.fill(i, localmask.size(), false);
  return localmask;
}

rbg_interval find_empty_rbg_interval(uint32_t max_nof_rbgs, const rbgmask_t& current_mask)
{
  return find_contiguous_interval(current_mask, max_nof_rbgs);
}

rbgmask_t compute_rbgmask_greedy(uint32_t max_nof_rbgs, bool is_contiguous, const rbgmask_t& current_mask)
{
  // Allocate enough RBs that accommodate pending data
  rbgmask_t newtx_mask(current_mask.size());
  if (is_contiguous) {
    rbg_interval interv = find_contiguous_interval(current_mask, max_nof_rbgs);
    newtx_mask.fill(interv.start(), interv.stop());
  } else {
    newtx_mask = find_available_rb_mask(current_mask, max_nof_rbgs);
  }
  return newtx_mask;
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

alloc_result try_dl_retx_alloc(sf_sched& tti_sched, sched_ue& ue, const dl_harq_proc& h)
{
  // Try to reuse the same mask
  rbgmask_t    retx_mask = h.get_rbgmask();
  alloc_result code      = tti_sched.alloc_dl_user(&ue, retx_mask, h.get_id());
  if (code != alloc_result::sch_collision) {
    return code;
  }

  // If previous mask does not fit, find another with exact same number of rbgs
  size_t nof_rbg             = retx_mask.count();
  bool   is_contiguous_alloc = ue.get_dci_format() == SRSRAN_DCI_FORMAT1A;
  retx_mask                  = compute_rbgmask_greedy(nof_rbg, is_contiguous_alloc, tti_sched.get_dl_mask());
  if (retx_mask.count() == nof_rbg) {
    return tti_sched.alloc_dl_user(&ue, retx_mask, h.get_id());
  }
  return alloc_result::sch_collision;
}

alloc_result try_dl_newtx_alloc_greedy(sf_sched& tti_sched, sched_ue& ue, const dl_harq_proc& h, rbgmask_t* result_mask)
{
  if (result_mask != nullptr) {
    *result_mask = {};
  }

  // If all RBGs are occupied, the next steps can be shortcut
  const rbgmask_t& current_mask = tti_sched.get_dl_mask();
  if (current_mask.all()) {
    return alloc_result::no_sch_space;
  }

  // If there is no data to transmit, no need to allocate
  rbg_interval req_rbgs = ue.get_required_dl_rbgs(tti_sched.get_enb_cc_idx());
  if (req_rbgs.stop() == 0) {
    return alloc_result::no_rnti_opportunity;
  }

  // Find RBG mask that accommodates pending data
  bool      is_contiguous_alloc = ue.get_dci_format() == SRSRAN_DCI_FORMAT1A;
  rbgmask_t newtxmask           = compute_rbgmask_greedy(req_rbgs.stop(), is_contiguous_alloc, current_mask);
  if (newtxmask.none() or newtxmask.count() < req_rbgs.start()) {
    return alloc_result::no_sch_space;
  }

  // empty RBGs were found. Attempt allocation
  alloc_result ret = tti_sched.alloc_dl_user(&ue, newtxmask, h.get_id());
  if (ret == alloc_result::success and result_mask != nullptr) {
    *result_mask = newtxmask;
  }
  return ret;
}

/*****************
 *  UL Helpers
 ****************/

prb_interval find_contiguous_ul_prbs(uint32_t L, const prbmask_t& current_mask)
{
  prb_interval prb_interv = find_contiguous_interval(current_mask, L);
  if (prb_interv.empty()) {
    return prb_interv;
  }

  // Make sure L is allowed by SC-FDMA modulation
  prb_interval prb_interv2 = prb_interv;
  while (not srsran_dft_precoding_valid_prb(prb_interv.length()) and prb_interv.stop() < current_mask.size() and
         not current_mask.test(prb_interv.stop())) {
    prb_interv.resize_by(1);
  }
  if (not srsran_dft_precoding_valid_prb(prb_interv.length())) {
    // if length increase failed, try to decrease
    prb_interv = prb_interv2;
    prb_interv.resize_by(-1);
    while (not srsran_dft_precoding_valid_prb(prb_interv.length()) and not prb_interv.empty()) {
      prb_interv.resize_by(-1);
    }
  }
  return prb_interv;
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

alloc_result try_ul_retx_alloc(sf_sched& tti_sched, sched_ue& ue, const ul_harq_proc& h)
{
  prb_interval alloc = h.get_alloc();
  if (tti_sched.get_cc_cfg()->nof_prb() == 6 and h.is_msg3()) {
    // We allow collisions with PUCCH for special case of Msg3 and 6 PRBs
    return tti_sched.alloc_ul_user(&ue, alloc);
  }

  // If can schedule the same mask as in earlier tx, do it
  if (not tti_sched.get_ul_mask().any(alloc.start(), alloc.stop())) {
    alloc_result ret = tti_sched.alloc_ul_user(&ue, alloc);
    if (ret != alloc_result::sch_collision) {
      return ret;
    }
  }

  // Avoid measGaps accounting for PDCCH
  if (not ue.pusch_enabled(tti_sched.get_tti_rx(), tti_sched.get_enb_cc_idx(), true)) {
    return alloc_result::no_rnti_opportunity;
  }
  uint32_t nof_prbs = alloc.length();
  alloc             = find_contiguous_ul_prbs(nof_prbs, tti_sched.get_ul_mask());
  if (alloc.length() != nof_prbs) {
    return alloc_result::no_sch_space;
  }
  return tti_sched.alloc_ul_user(&ue, alloc);
}

} // namespace srsenb
