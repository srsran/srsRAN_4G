/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsenb/hdr/stack/mac/schedulers/sched_time_rr.h"
#include <string.h>

namespace srsenb {

sched_time_rr::sched_time_rr(const sched_cell_params_t& cell_params_, const sched_interface::sched_args_t& sched_args)
{
  cc_cfg = &cell_params_;
}

/*****************************************************************
 *                         Dowlink
 *****************************************************************/

void sched_time_rr::sched_dl_users(std::map<uint16_t, sched_ue>& ue_db, sf_sched* tti_sched)
{
  if (ue_db.empty()) {
    return;
  }

  // give priority in a time-domain RR basis.
  uint32_t priority_idx = tti_sched->get_tti_tx_dl().to_uint() % (uint32_t)ue_db.size();
  sched_dl_retxs(ue_db, tti_sched, priority_idx);
  sched_dl_newtxs(ue_db, tti_sched, priority_idx);
}

void sched_time_rr::sched_dl_retxs(std::map<uint16_t, sched_ue>& ue_db, sf_sched* tti_sched, size_t prio_idx)
{
  auto iter = ue_db.begin();
  std::advance(iter, prio_idx);
  for (uint32_t ue_count = 0; ue_count < ue_db.size(); ++iter, ++ue_count) {
    if (iter == ue_db.end()) {
      iter = ue_db.begin(); // wrap around
    }
    sched_ue&           user = iter->second;
    const dl_harq_proc* h    = get_dl_retx_harq(user, tti_sched);
    // Check if there is a pending retx
    if (h == nullptr) {
      continue;
    }
    alloc_outcome_t code = try_dl_retx_alloc(*tti_sched, user, *h);
    if (code == alloc_outcome_t::DCI_COLLISION) {
      log_h->info("SCHED: Couldn't find space in PDCCH for DL retx for rnti=0x%x\n", user.get_rnti());
    }
  }
}

void sched_time_rr::sched_dl_newtxs(std::map<uint16_t, sched_ue>& ue_db, sf_sched* tti_sched, size_t prio_idx)
{
  auto iter = ue_db.begin();
  std::advance(iter, prio_idx);
  for (uint32_t ue_count = 0; ue_count < ue_db.size(); ++iter, ++ue_count) {
    if (iter == ue_db.end()) {
      iter = ue_db.begin(); // wrap around
    }
    sched_ue& user = iter->second;
    if (user.enb_to_ue_cc_idx(cc_cfg->enb_cc_idx) < 0) {
      continue;
    }
    const dl_harq_proc* h        = get_dl_newtx_harq(user, tti_sched);
    rbg_interval        req_rbgs = user.get_required_dl_rbgs(cc_cfg->enb_cc_idx);
    // Check if there is an empty harq for the newtx
    if (h == nullptr or req_rbgs.stop() == 0) {
      continue;
    }
    // Allocate resources based on pending data
    rbgmask_t newtx_mask = find_available_dl_rbgs(req_rbgs.stop(), tti_sched->get_dl_mask());
    if (newtx_mask.count() >= req_rbgs.start()) {
      // empty RBGs were found
      alloc_outcome_t code = tti_sched->alloc_dl_user(&user, newtx_mask, h->get_id());
      if (code == alloc_outcome_t::DCI_COLLISION) {
        log_h->info("SCHED: Couldn't find space in PDCCH for DL tx for rnti=0x%x\n", user.get_rnti());
      }
    }
  }
}

/*****************************************************************
 *                         Uplink
 *****************************************************************/

void sched_time_rr::sched_ul_users(std::map<uint16_t, sched_ue>& ue_db, sf_sched* tti_sched)
{
  if (ue_db.empty()) {
    return;
  }
  // give priority in a time-domain RR basis.
  uint32_t priority_idx = tti_sched->get_tti_tx_ul().to_uint() % (uint32_t)ue_db.size();
  sched_ul_retxs(ue_db, tti_sched, priority_idx);
  sched_ul_newtxs(ue_db, tti_sched, priority_idx);
}

void sched_time_rr::sched_ul_retxs(std::map<uint16_t, sched_ue>& ue_db, sf_sched* tti_sched, size_t prio_idx)
{
  auto iter = ue_db.begin();
  std::advance(iter, prio_idx);
  for (uint32_t ue_count = 0; ue_count < ue_db.size(); ++iter, ++ue_count) {
    if (iter == ue_db.end()) {
      iter = ue_db.begin(); // wrap around
    }
    sched_ue&           user = iter->second;
    const ul_harq_proc* h    = get_ul_retx_harq(user, tti_sched);
    // Check if there is a pending retx
    if (h == nullptr) {
      continue;
    }
    alloc_outcome_t code = try_ul_retx_alloc(*tti_sched, user, *h);
    if (code == alloc_outcome_t::DCI_COLLISION) {
      log_h->info("SCHED: Couldn't find space in PDCCH for UL retx of rnti=0x%x\n", user.get_rnti());
    }
  }
}

void sched_time_rr::sched_ul_newtxs(std::map<uint16_t, sched_ue>& ue_db, sf_sched* tti_sched, size_t prio_idx)
{
  auto iter = ue_db.begin();
  std::advance(iter, prio_idx);
  for (uint32_t ue_count = 0; ue_count < ue_db.size(); ++iter, ++ue_count) {
    if (iter == ue_db.end()) {
      iter = ue_db.begin(); // wrap around
    }
    sched_ue&           user = iter->second;
    const ul_harq_proc* h    = get_ul_newtx_harq(user, tti_sched);
    // Check if there is a empty harq
    if (h == nullptr) {
      continue;
    }
    uint32_t pending_data = user.get_pending_ul_new_data(tti_sched->get_tti_tx_ul(), cc_cfg->enb_cc_idx);
    // Check if there is a empty harq, and data to transmit
    if (pending_data == 0) {
      continue;
    }
    uint32_t     pending_rb = user.get_required_prb_ul(cc_cfg->enb_cc_idx, pending_data);
    prb_interval alloc      = find_contiguous_ul_prbs(pending_rb, tti_sched->get_ul_mask());
    if (alloc.empty()) {
      continue;
    }
    alloc_outcome_t ret = tti_sched->alloc_ul_user(&user, alloc);
    if (ret == alloc_outcome_t::DCI_COLLISION) {
      log_h->info("SCHED: Couldn't find space in PDCCH for UL tx of rnti=0x%x\n", user.get_rnti());
    }
  }
}

} // namespace srsenb
