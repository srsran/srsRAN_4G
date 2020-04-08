/*
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

#include "srsenb/hdr/stack/mac/scheduler_metric.h"
#include "srsenb/hdr/stack/mac/scheduler_harq.h"
#include "srslte/common/log_helper.h"
#include "srslte/common/logmap.h"
#include <string.h>

namespace srsenb {

/*****************************************************************
 *
 * Downlink Metric
 *
 *****************************************************************/

void dl_metric_rr::set_params(const sched_cell_params_t& cell_params_)
{
  cc_cfg = &cell_params_;
  log_h  = srslte::logmap::get("MAC ");
}

void dl_metric_rr::sched_users(std::map<uint16_t, sched_ue>& ue_db, dl_sf_sched_itf* tti_sched)
{
  tti_alloc = tti_sched;

  if (ue_db.empty()) {
    return;
  }

  // give priority in a time-domain RR basis.
  uint32_t priority_idx = tti_alloc->get_tti_tx_dl() % (uint32_t)ue_db.size();
  auto     iter         = ue_db.begin();
  std::advance(iter, priority_idx);
  for (uint32_t ue_count = 0; ue_count < ue_db.size(); ++iter, ++ue_count) {
    if (iter == ue_db.end()) {
      iter = ue_db.begin(); // wrap around
    }
    sched_ue* user = &iter->second;
    allocate_user(user);
  }
}

bool dl_metric_rr::find_allocation(uint32_t min_nof_rbg, uint32_t max_nof_rbg, rbgmask_t* rbgmask)
{
  if (tti_alloc->get_dl_mask().all()) {
    return false;
  }
  // 1's for free rbgs
  rbgmask_t localmask = ~(tti_alloc->get_dl_mask());

  uint32_t i = 0, nof_alloc = 0;
  for (; i < localmask.size() and nof_alloc < max_nof_rbg; ++i) {
    if (localmask.test(i)) {
      nof_alloc++;
    }
  }
  if (nof_alloc < min_nof_rbg) {
    return false;
  }
  localmask.fill(i, localmask.size(), false);
  *rbgmask = localmask;
  return true;
}

dl_harq_proc* dl_metric_rr::allocate_user(sched_ue* user)
{
  // Do not allocate a user multiple times in the same tti
  if (tti_alloc->is_dl_alloc(user)) {
    return nullptr;
  }
  // Do not allocate a user to an inactive carrier
  auto p = user->get_cell_index(cc_cfg->enb_cc_idx);
  if (not p.first) {
    return nullptr;
  }
  uint32_t cell_idx = p.second;

  alloc_outcome_t code;
  uint32_t        tti_dl = tti_alloc->get_tti_tx_dl();
  dl_harq_proc*   h      = user->get_pending_dl_harq(tti_dl, cell_idx);

  // Schedule retx if we have space
  if (h != nullptr) {
    // Try to reuse the same mask
    rbgmask_t retx_mask = h->get_rbgmask();
    code                = tti_alloc->alloc_dl_user(user, retx_mask, h->get_id());
    if (code == alloc_outcome_t::SUCCESS) {
      return h;
    }
    if (code == alloc_outcome_t::DCI_COLLISION) {
      // No DCIs available for this user. Move to next
      log_h->warning("SCHED: Couldn't find space in PDCCH for DL retx for rnti=0x%x\n", user->get_rnti());
      return nullptr;
    }

    // If previous mask does not fit, find another with exact same number of rbgs
    size_t nof_rbg = retx_mask.count();
    if (find_allocation(nof_rbg, nof_rbg, &retx_mask)) {
      code = tti_alloc->alloc_dl_user(user, retx_mask, h->get_id());
      if (code == alloc_outcome_t::SUCCESS) {
        return h;
      }
      if (code == alloc_outcome_t::DCI_COLLISION) {
        log_h->warning("SCHED: Couldn't find space in PDCCH for DL retx for rnti=0x%x\n", user->get_rnti());
        return nullptr;
      }
    }
  }

  // If could not schedule the reTx, or there wasn't any pending retx, find an empty PID
  h = user->get_empty_dl_harq(tti_dl, cell_idx);
  if (h != nullptr) {
    // Allocate resources based on pending data
    rbg_range_t req_rbgs = user->get_required_dl_rbgs(cell_idx);
    if (req_rbgs.rbg_min > 0) {
      rbgmask_t newtx_mask(tti_alloc->get_dl_mask().size());
      if (find_allocation(req_rbgs.rbg_min, req_rbgs.rbg_max, &newtx_mask)) {
        // some empty spaces were found
        code = tti_alloc->alloc_dl_user(user, newtx_mask, h->get_id());
        if (code == alloc_outcome_t::SUCCESS) {
          return h;
        } else if (code == alloc_outcome_t::DCI_COLLISION) {
          log_h->warning("SCHED: Couldn't find space in PDCCH for DL tx for rnti=0x%x\n", user->get_rnti());
        }
      }
    }
  }

  return nullptr;
}

/*****************************************************************
 *
 * Uplink Metric
 *
 *****************************************************************/

void ul_metric_rr::set_params(const sched_cell_params_t& cell_params_)
{
  cc_cfg = &cell_params_;
  log_h  = srslte::logmap::get("MAC ");
}

void ul_metric_rr::sched_users(std::map<uint16_t, sched_ue>& ue_db, ul_sf_sched_itf* tti_sched)
{
  tti_alloc   = tti_sched;
  current_tti = tti_alloc->get_tti_tx_ul();

  if (ue_db.empty()) {
    return;
  }

  // give priority in a time-domain RR basis
  uint32_t priority_idx =
      (current_tti + (uint32_t)ue_db.size() / 2) % (uint32_t)ue_db.size(); // make DL and UL interleaved

  // allocate reTxs first
  auto iter = ue_db.begin();
  std::advance(iter, priority_idx);
  for (uint32_t ue_count = 0; ue_count < ue_db.size(); ++iter, ++ue_count) {
    if (iter == ue_db.end()) {
      iter = ue_db.begin(); // wrap around
    }
    sched_ue* user = &iter->second;
    allocate_user_retx_prbs(user);
  }

  // give priority in a time-domain RR basis
  iter = ue_db.begin();
  std::advance(iter, priority_idx);
  for (uint32_t ue_count = 0; ue_count < ue_db.size(); ++iter, ++ue_count) {
    if (iter == ue_db.end()) {
      iter = ue_db.begin(); // wrap around
    }
    sched_ue* user = &iter->second;
    allocate_user_newtx_prbs(user);
  }
}

/**
 * Finds a range of L contiguous PRBs that are empty
 * @param L Size of the requested UL allocation in PRBs
 * @param alloc Found allocation. It is guaranteed that 0 <= alloc->L <= L
 * @return true if the requested allocation of size L was strictly met
 */
bool ul_metric_rr::find_allocation(uint32_t L, ul_harq_proc::ul_alloc_t* alloc)
{
  const prbmask_t* used_rb = &tti_alloc->get_ul_mask();
  bzero(alloc, sizeof(ul_harq_proc::ul_alloc_t));
  for (uint32_t n = 0; n < used_rb->size() && alloc->L < L; n++) {
    if (not used_rb->test(n) && alloc->L == 0) {
      alloc->RB_start = n;
    }
    if (not used_rb->test(n)) {
      alloc->L++;
    } else if (alloc->L > 0) {
      // avoid edges
      if (n < 3) {
        alloc->RB_start = 0;
        alloc->L        = 0;
      } else {
        break;
      }
    }
  }
  if (alloc->L == 0) {
    return false;
  }

  // Make sure L is allowed by SC-FDMA modulation
  while (!srslte_dft_precoding_valid_prb(alloc->L)) {
    alloc->L--;
  }
  return alloc->L == L;
}

ul_harq_proc* ul_metric_rr::allocate_user_retx_prbs(sched_ue* user)
{
  if (tti_alloc->is_ul_alloc(user)) {
    return nullptr;
  }
  auto p = user->get_cell_index(cc_cfg->enb_cc_idx);
  if (not p.first) {
    // this cc is not activated for this user
    return nullptr;
  }
  uint32_t cell_idx = p.second;

  alloc_outcome_t ret;
  ul_harq_proc*   h = user->get_ul_harq(current_tti, cell_idx);

  // if there are procedures and we have space
  if (h->has_pending_retx()) {
    ul_harq_proc::ul_alloc_t alloc = h->get_alloc();

    // If can schedule the same mask, do it
    ret = tti_alloc->alloc_ul_user(user, alloc);
    if (ret == alloc_outcome_t::SUCCESS) {
      return h;
    }
    if (ret == alloc_outcome_t::DCI_COLLISION) {
      log_h->warning("SCHED: Couldn't find space in PDCCH for UL retx of rnti=0x%x\n", user->get_rnti());
      return nullptr;
    }

    if (find_allocation(alloc.L, &alloc)) {
      ret = tti_alloc->alloc_ul_user(user, alloc);
      if (ret == alloc_outcome_t::SUCCESS) {
        return h;
      }
      if (ret == alloc_outcome_t::DCI_COLLISION) {
        log_h->warning("SCHED: Couldn't find space in PDCCH for UL retx of rnti=0x%x\n", user->get_rnti());
      }
    }
  }
  return nullptr;
}

ul_harq_proc* ul_metric_rr::allocate_user_newtx_prbs(sched_ue* user)
{
  if (tti_alloc->is_ul_alloc(user)) {
    return nullptr;
  }
  auto p = user->get_cell_index(cc_cfg->enb_cc_idx);
  if (not p.first) {
    // this cc is not activated for this user
    return nullptr;
  }
  uint32_t cell_idx = p.second;

  uint32_t      pending_data = user->get_pending_ul_new_data(current_tti);
  ul_harq_proc* h            = user->get_ul_harq(current_tti, cell_idx);

  // find an empty PID
  if (h->is_empty(0) and pending_data > 0) {
    uint32_t                 pending_rb = user->get_required_prb_ul(cell_idx, pending_data);
    ul_harq_proc::ul_alloc_t alloc{};

    find_allocation(pending_rb, &alloc);
    if (alloc.L > 0) { // at least one PRB was scheduled
      alloc_outcome_t ret = tti_alloc->alloc_ul_user(user, alloc);
      if (ret == alloc_outcome_t::SUCCESS) {
        return h;
      }
      if (ret == alloc_outcome_t::DCI_COLLISION) {
        log_h->warning("SCHED: Couldn't find space in PDCCH for UL tx of rnti=0x%x\n", user->get_rnti());
      }
    }
  }
  return nullptr;
}

} // namespace srsenb
