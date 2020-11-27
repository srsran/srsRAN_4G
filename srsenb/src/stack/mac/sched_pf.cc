/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsenb/hdr/stack/mac/sched_pf.h"
#include "srsenb/hdr/stack/mac/sched_harq.h"

namespace srsenb {

void sched_dl_pf::set_params(const sched_cell_params_t& cell_params_)
{
  cc_cfg = &cell_params_;
  log_h  = srslte::logmap::get("MAC");
}

void sched_dl_pf::sched_users(std::map<uint16_t, sched_ue>& ue_db, dl_sf_sched_itf* tti_sched)
{
  if (ue_db.empty()) {
    return;
  }
  // remove deleted users from history
  for (auto it = ue_history_db.begin(); it != ue_history_db.end();) {
    if (not ue_db.count(it->first)) {
      it = ue_history_db.erase(it);
    } else {
      ++it;
    }
  }
  // add new users to history db, and update priority queue
  for (auto& u : ue_db) {
    auto it = ue_history_db.find(u.first);
    if (it == ue_history_db.end()) {
      it = ue_history_db.insert(std::make_pair(u.first, ue_ctxt{u.first})).first;
    }
    it->second.new_tti(*cc_cfg, u.second, tti_sched);
    ue_queue.push(&it->second);
  }

  while (not ue_queue.empty()) {
    ue_ctxt& ue            = *ue_queue.top();
    bool     alloc_success = try_dl_alloc(ue, ue_db[ue.rnti], tti_sched);
    ue.save_history(alloc_success, 0.01);
    ue_queue.pop();
  }
}

void sched_dl_pf::ue_ctxt::new_tti(const sched_cell_params_t& cell, sched_ue& ue, dl_sf_sched_itf* tti_sched)
{
  h       = nullptr;
  prio    = 0;
  is_retx = false;

  auto p = ue.get_active_cell_index(cell.enb_cc_idx);
  if (not p.first) {
    return;
  }
  if (not ue.pdsch_enabled(srslte::tti_point(tti_sched->get_tti_tx_dl() - TX_ENB_DELAY), cell.enb_cc_idx)) {
    return;
  }
  ue_cc_idx = p.second;

  // search for DL HARQ
  h       = ue.get_pending_dl_harq(tti_sched->get_tti_tx_dl(), ue_cc_idx);
  is_retx = h != nullptr;
  if (h == nullptr) {
    h = ue.get_empty_dl_harq(tti_sched->get_tti_tx_dl(), ue_cc_idx);
    if (h == nullptr) {
      return;
    }
  }

  // calculate PF priority
  float r = ue.get_expected_dl_bitrate(ue_cc_idx) / 8;
  float R = avg_rate();
  prio    = (R != 0) ? r / R : (r == 0 ? 0 : std::numeric_limits<float>::max());
}

void sched_dl_pf::ue_ctxt::save_history(bool alloc, float alpha)
{
  float sample = alloc ? (h->get_tbs(0) + h->get_tbs(1)) : 0;
  if (nof_samples < 1 / alpha) {
    // fast start
    rate = rate + (sample - rate) / (nof_samples + 1);
  } else {
    rate = (1 - alpha) * rate + (alpha)*sample;
  }
  nof_samples++;
}

bool find_allocation(uint32_t min_nof_rbg, uint32_t max_nof_rbg, rbgmask_t* rbgmask, dl_sf_sched_itf* tti_alloc)
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

bool sched_dl_pf::try_dl_alloc(ue_ctxt& ue_ctxt, sched_ue& ue, dl_sf_sched_itf* tti_sched)
{
  if (tti_sched->is_dl_alloc(ue_ctxt.rnti) or ue_ctxt.prio == 0) {
    return false;
  }
  alloc_outcome_t code;
  if (ue_ctxt.is_retx) {
    // Try to reuse the same mask
    rbgmask_t retx_mask = ue_ctxt.h->get_rbgmask();
    code                = tti_sched->alloc_dl_user(&ue, retx_mask, ue_ctxt.h->get_id());
    if (code == alloc_outcome_t::SUCCESS) {
      return true;
    }
    if (code == alloc_outcome_t::DCI_COLLISION) {
      // No DCIs available for this user. Move to next
      log_h->info("SCHED: Couldn't find space in PDCCH for DL retx for rnti=0x%x\n", ue_ctxt.rnti);
      return false;
    }

    // If previous mask does not fit, find another with exact same number of rbgs
    size_t nof_rbg = retx_mask.count();
    if (find_allocation(nof_rbg, nof_rbg, &retx_mask, tti_sched)) {
      code = tti_sched->alloc_dl_user(&ue, retx_mask, ue_ctxt.h->get_id());
      if (code == alloc_outcome_t::SUCCESS) {
        return true;
      }
      if (code == alloc_outcome_t::DCI_COLLISION) {
        log_h->info("SCHED: Couldn't find space in PDCCH for DL retx for rnti=0x%x\n", ue.get_rnti());
        return false;
      }
    }
  } else {
    // Allocate resources based on pending data
    rbg_interval req_rbgs = ue.get_required_dl_rbgs(ue_ctxt.ue_cc_idx);
    if (req_rbgs.stop() > 0) {
      rbgmask_t newtx_mask(tti_sched->get_dl_mask().size());
      if (find_allocation(req_rbgs.start(), req_rbgs.stop(), &newtx_mask, tti_sched)) {
        // some empty spaces were found
        code = tti_sched->alloc_dl_user(&ue, newtx_mask, ue_ctxt.h->get_id());
        if (code == alloc_outcome_t::SUCCESS) {
          return true;
        } else if (code == alloc_outcome_t::DCI_COLLISION) {
          log_h->info("SCHED: Couldn't find space in PDCCH for DL tx for rnti=0x%x\n", ue_ctxt.rnti);
        }
      }
    }
  }
  return false;
}

bool sched_dl_pf::ue_prio_compare::operator()(const sched_dl_pf::ue_ctxt* lhs, const sched_dl_pf::ue_ctxt* rhs) const
{
  return (not lhs->is_retx and rhs->is_retx) or (lhs->is_retx == rhs->is_retx and lhs->prio < rhs->prio);
}

/*****************************************************************
 *
 * Uplink Metric
 *
 *****************************************************************/

void sched_ul_pf::set_params(const sched_cell_params_t& cell_params_)
{
  cc_cfg = &cell_params_;
  log_h  = srslte::logmap::get("MAC");
}

void sched_ul_pf::sched_users(std::map<uint16_t, sched_ue>& ue_db, ul_sf_sched_itf* tti_sched)
{
  if (ue_db.empty()) {
    return;
  }
  // remove deleted users from history
  for (auto it = ue_history_db.begin(); it != ue_history_db.end();) {
    if (not ue_db.count(it->first)) {
      it = ue_history_db.erase(it);
    } else {
      ++it;
    }
  }
  // add new users to history db, and update priority queue
  for (auto& u : ue_db) {
    auto it = ue_history_db.find(u.first);
    if (it == ue_history_db.end()) {
      it = ue_history_db.insert(std::make_pair(u.first, ue_ctxt{u.first})).first;
    }
    it->second.new_tti(*cc_cfg, u.second, tti_sched);
    ue_queue.push(&it->second);
  }
  while (not ue_queue.empty()) {
    ue_ctxt& ue            = *ue_queue.top();
    bool     alloc_success = try_ul_alloc(ue, ue_db[ue.rnti], tti_sched);
    ue.save_history(alloc_success, 0.01);
    ue_queue.pop();
  }
}

/**
 * Finds a range of L contiguous PRBs that are empty
 * @param L Size of the requested UL allocation in PRBs
 * @param alloc Found allocation. It is guaranteed that 0 <= alloc->L <= L
 * @return true if the requested allocation of size L was strictly met
 */
bool find_allocation(uint32_t L, prb_interval* alloc, ul_sf_sched_itf* tti_sched)
{
  const prbmask_t* used_rb = &tti_sched->get_ul_mask();
  *alloc                   = {};
  for (uint32_t n = 0; n < used_rb->size() && alloc->length() < L; n++) {
    if (not used_rb->test(n) && alloc->length() == 0) {
      alloc->displace_to(n);
    }
    if (not used_rb->test(n)) {
      alloc->resize_by(1);
    } else if (alloc->length() > 0) {
      // avoid edges
      if (n < 3) {
        *alloc = {};
      } else {
        break;
      }
    }
  }
  if (alloc->length() == 0) {
    return false;
  }

  // Make sure L is allowed by SC-FDMA modulation
  while (!srslte_dft_precoding_valid_prb(alloc->length())) {
    alloc->resize_by(-1);
  }
  return alloc->length() == L;
}

bool sched_ul_pf::try_ul_alloc(ue_ctxt& ue_ctxt, sched_ue& ue, ul_sf_sched_itf* tti_sched)
{
  if (ue_ctxt.h == nullptr or tti_sched->is_ul_alloc(ue_ctxt.rnti) or ue_ctxt.prio == 0) {
    return false;
  }
  srslte::tti_point tti_rx{tti_sched->get_tti_tx_ul() - (TX_ENB_DELAY + FDD_HARQ_DELAY_DL_MS)};

  alloc_outcome_t ret;
  if (ue_ctxt.h->has_pending_retx()) {
    prb_interval alloc = ue_ctxt.h->get_alloc();

    // If can schedule the same mask, do it
    ret = tti_sched->alloc_ul_user(&ue, alloc);
    if (ret == alloc_outcome_t::SUCCESS) {
      return true;
    }
    if (ret == alloc_outcome_t::DCI_COLLISION) {
      log_h->info("SCHED: Couldn't find space in PDCCH for UL retx of rnti=0x%x\n", ue.get_rnti());
      return false;
    }

    // Avoid measGaps accounting for PDCCH
    if (not ue.pusch_enabled(tti_rx, cc_cfg->enb_cc_idx, true)) {
      return false;
    }
    if (find_allocation(alloc.length(), &alloc, tti_sched)) {
      ret = tti_sched->alloc_ul_user(&ue, alloc);
      if (ret == alloc_outcome_t::SUCCESS) {
        return true;
      }
      if (ret == alloc_outcome_t::DCI_COLLISION) {
        log_h->info("SCHED: Couldn't find space in PDCCH for UL retx of rnti=0x%x\n", ue.get_rnti());
      }
    }
  } else {
    // Avoid measGaps accounting for PDCCH
    if (not ue.pusch_enabled(tti_rx, cc_cfg->enb_cc_idx, true)) {
      return false;
    }

    uint32_t pending_data = ue.get_pending_ul_new_data(tti_sched->get_tti_tx_ul(), ue_ctxt.ue_cc_idx);
    // find an empty PID
    if (ue_ctxt.h->is_empty(0) and pending_data > 0) {
      uint32_t     pending_rb = ue.get_required_prb_ul(ue_ctxt.ue_cc_idx, pending_data);
      prb_interval alloc{};

      find_allocation(pending_rb, &alloc, tti_sched);
      if (alloc.length() > 0) { // at least one PRB was scheduled
        ret = tti_sched->alloc_ul_user(&ue, alloc);
        if (ret == alloc_outcome_t::SUCCESS) {
          return true;
        }
        if (ret == alloc_outcome_t::DCI_COLLISION) {
          log_h->info("SCHED: Couldn't find space in PDCCH for UL tx of rnti=0x%x\n", ue.get_rnti());
        }
      }
    }
  }
  return false;
}

void sched_ul_pf::ue_ctxt::new_tti(const sched_cell_params_t& cell, sched_ue& ue, ul_sf_sched_itf* tti_sched)
{
  srslte::tti_point tti_rx = srslte::tti_point(tti_sched->get_tti_tx_ul() - TX_ENB_DELAY - FDD_HARQ_DELAY_DL_MS);
  h                        = nullptr;
  prio                     = 0;

  auto p = ue.get_active_cell_index(cell.enb_cc_idx);
  if (not p.first) {
    return;
  }
  if (not ue.pusch_enabled(tti_rx, cell.enb_cc_idx, false)) {
    return;
  }
  ue_cc_idx = p.second;
  h         = ue.get_ul_harq(tti_sched->get_tti_tx_ul(), ue_cc_idx);

  // calculate PF priority
  float r = ue.get_expected_ul_bitrate(ue_cc_idx) / 8;
  float R = avg_rate();
  prio    = (R != 0) ? r / R : (r == 0 ? 0 : std::numeric_limits<float>::max());
}

void sched_ul_pf::ue_ctxt::save_history(bool alloc, float alpha)
{
  float sample = alloc ? h->get_pending_data() : 0;
  if (nof_samples < 1 / alpha) {
    // fast start
    rate = rate + (sample - rate) / (nof_samples + 1);
  } else {
    rate = (1 - alpha) * rate + (alpha)*sample;
  }
  nof_samples++;
}

bool sched_ul_pf::ue_prio_compare::operator()(const sched_ul_pf::ue_ctxt* lhs, const sched_ul_pf::ue_ctxt* rhs) const
{
  bool is_retx1 = lhs->h != nullptr and lhs->h->has_pending_retx(),
       is_retx2 = rhs->h != nullptr and rhs->h->has_pending_retx();
  return (not is_retx1 and is_retx2) or (is_retx1 == is_retx2 and lhs->prio < rhs->prio);
}

} // namespace srsenb
