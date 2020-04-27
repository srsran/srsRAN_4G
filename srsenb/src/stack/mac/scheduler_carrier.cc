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

#include "srsenb/hdr/stack/mac/scheduler_carrier.h"
#include "srsenb/hdr/stack/mac/scheduler_metric.h"
#include "srslte/common/log_helper.h"
#include "srslte/common/logmap.h"

namespace srsenb {

/*******************************************************
 *        Broadcast (SIB+Paging) scheduling
 *******************************************************/

bc_sched::bc_sched(const sched_cell_params_t& cfg_, srsenb::rrc_interface_mac* rrc_) : cc_cfg(&cfg_), rrc(rrc_) {}

void bc_sched::dl_sched(sf_sched* tti_sched)
{
  current_tti   = tti_sched->get_tti_tx_dl();
  bc_aggr_level = 2;

  /* Activate/deactivate SI windows */
  update_si_windows(tti_sched);

  /* Allocate DCIs and RBGs for each SIB */
  alloc_sibs(tti_sched);

  /* Allocate Paging */
  // NOTE: It blocks
  alloc_paging(tti_sched);
}

void bc_sched::update_si_windows(sf_sched* tti_sched)
{
  uint32_t tti_tx_dl      = tti_sched->get_tti_tx_dl();
  uint32_t current_sf_idx = tti_sched->get_tti_params().sf_idx_tx_dl;
  uint32_t current_sfn    = tti_sched->get_tti_params().sfn_tx_dl;

  for (uint32_t i = 0; i < pending_sibs.size(); ++i) {
    // There is SIB data
    if (cc_cfg->cfg.sibs[i].len == 0) {
      continue;
    }

    if (not pending_sibs[i].is_in_window) {
      uint32_t sf = 5;
      uint32_t x  = 0;
      if (i > 0) {
        x  = (i - 1) * cc_cfg->cfg.si_window_ms;
        sf = x % 10;
      }
      if ((current_sfn % (cc_cfg->cfg.sibs[i].period_rf)) == x / 10 && current_sf_idx == sf) {
        pending_sibs[i].is_in_window = true;
        pending_sibs[i].window_start = tti_tx_dl;
        pending_sibs[i].n_tx         = 0;
      }
    } else {
      if (i > 0) {
        if (srslte_tti_interval(tti_tx_dl, pending_sibs[i].window_start) > cc_cfg->cfg.si_window_ms) {
          // the si window has passed
          pending_sibs[i] = {};
        }
      } else {
        // SIB1 is always in window
        if (pending_sibs[0].n_tx == 4) {
          pending_sibs[0].n_tx = 0;
        }
      }
    }
  }
}

void bc_sched::alloc_sibs(sf_sched* tti_sched)
{
  uint32_t current_sf_idx = tti_sched->get_tti_params().sf_idx_tx_dl;
  uint32_t current_sfn    = tti_sched->get_tti_params().sfn_tx_dl;

  for (uint32_t i = 0; i < pending_sibs.size(); i++) {
    if (cc_cfg->cfg.sibs[i].len > 0 and pending_sibs[i].is_in_window and pending_sibs[i].n_tx < 4) {
      uint32_t nof_tx = (i > 0) ? SRSLTE_MIN(srslte::ceil_div(cc_cfg->cfg.si_window_ms, 10), 4) : 4;
      uint32_t n_sf   = (tti_sched->get_tti_tx_dl() - pending_sibs[i].window_start);

      // Check if there is any SIB to tx
      bool sib1_flag = (i == 0) and (current_sfn % 2) == 0 and current_sf_idx == 5;
      bool other_sibs_flag =
          (i > 0) and (n_sf >= (cc_cfg->cfg.si_window_ms / nof_tx) * pending_sibs[i].n_tx) and current_sf_idx == 9;
      if (not sib1_flag and not other_sibs_flag) {
        continue;
      }

      // Schedule SIB
      tti_sched->alloc_bc(bc_aggr_level, i, pending_sibs[i].n_tx);
      pending_sibs[i].n_tx++;
    }
  }
}

void bc_sched::alloc_paging(sf_sched* tti_sched)
{
  /* Allocate DCIs and RBGs for paging */
  if (rrc != nullptr) {
    uint32_t paging_payload = 0;
    if (rrc->is_paging_opportunity(current_tti, &paging_payload) and paging_payload > 0) {
      tti_sched->alloc_paging(bc_aggr_level, paging_payload);
    }
  }
}

void bc_sched::reset()
{
  for (auto& sib : pending_sibs) {
    sib = {};
  }
}

/*******************************************************
 *                 RAR scheduling
 *******************************************************/

ra_sched::ra_sched(const sched_cell_params_t& cfg_, std::map<uint16_t, sched_ue>& ue_db_) :
  cc_cfg(&cfg_),
  log_h(srslte::logmap::get("MAC")),
  ue_db(&ue_db_)
{}

// Schedules RAR
// On every call to this function, we schedule the oldest RAR which is still within the window. If outside the window we
// discard it.
void ra_sched::dl_sched(sf_sched* tti_sched)
{
  uint32_t tti_tx_dl = tti_sched->get_tti_tx_dl();
  rar_aggr_level     = 2;

  while (not pending_rars.empty()) {
    sf_sched::pending_rar_t& rar       = pending_rars.front();
    uint32_t                 prach_tti = rar.prach_tti;

    // Discard all RARs out of the window. The first one inside the window is scheduled, if we can't we exit
    if (not sched_utils::is_in_tti_interval(tti_tx_dl, prach_tti + 3, prach_tti + 3 + cc_cfg->cfg.prach_rar_window)) {
      if (tti_tx_dl >= prach_tti + 3 + cc_cfg->cfg.prach_rar_window) {
        log_h->console("SCHED: Could not transmit RAR within the window (RA TTI=%d, Window=%d, Now=%d)\n",
                       prach_tti,
                       cc_cfg->cfg.prach_rar_window,
                       tti_tx_dl);
        log_h->error("SCHED: Could not transmit RAR within the window (RA TTI=%d, Window=%d, Now=%d)\n",
                     prach_tti,
                     cc_cfg->cfg.prach_rar_window,
                     tti_tx_dl);
        // Remove from pending queue and get next one if window has passed already
        pending_rars.pop_front();
        continue;
      }
      // If window not yet started do not look for more pending RARs
      return;
    }

    // Try to schedule DCI + RBGs for RAR Grant
    std::pair<alloc_outcome_t, uint32_t> ret = tti_sched->alloc_rar(rar_aggr_level, rar);
    if (ret.first == alloc_outcome_t::RB_COLLISION) {
      // there are not enough RBs for RAR or Msg3 allocation. We can skip this TTI
      return;
    }
    if (ret.first != alloc_outcome_t::SUCCESS) {
      // try to scheduler next RAR with different RA-RNTI
      continue;
    }

    uint32_t nof_rar_allocs = ret.second;
    if (nof_rar_allocs == rar.nof_grants) {
      // all RAR grants were allocated. Remove pending RAR
      pending_rars.pop_front();
    } else {
      // keep the RAR grants that were not scheduled, so we can schedule in next TTI
      std::copy(&rar.msg3_grant[nof_rar_allocs], &rar.msg3_grant[rar.nof_grants], &rar.msg3_grant[0]);
      rar.nof_grants -= nof_rar_allocs;
    }
  }
}

int ra_sched::dl_rach_info(dl_sched_rar_info_t rar_info)
{
  log_h->info("SCHED: New PRACH tti=%d, preamble=%d, temp_crnti=0x%x, ta_cmd=%d, msg3_size=%d\n",
              rar_info.prach_tti,
              rar_info.preamble_idx,
              rar_info.temp_crnti,
              rar_info.ta_cmd,
              rar_info.msg3_size);
  // RA-RNTI = 1 + t_id + f_id
  // t_id = index of first subframe specified by PRACH (0<=t_id<10)
  // f_id = index of the PRACH within subframe, in ascending order of freq domain (0<=f_id<6) (for FDD, f_id=0)
  uint16_t ra_rnti = 1 + (uint16_t)(rar_info.prach_tti % 10u);

  // find pending rar with same RA-RNTI
  for (sf_sched::pending_rar_t& r : pending_rars) {
    if (r.prach_tti == rar_info.prach_tti and ra_rnti == r.ra_rnti) {
      r.msg3_grant[r.nof_grants] = rar_info;
      r.nof_grants++;
      return SRSLTE_SUCCESS;
    }
  }

  // create new RAR
  sf_sched::pending_rar_t p;
  p.ra_rnti       = ra_rnti;
  p.prach_tti     = rar_info.prach_tti;
  p.nof_grants    = 1;
  p.msg3_grant[0] = rar_info;
  pending_rars.push_back(p);

  return SRSLTE_SUCCESS;
}

//! Schedule Msg3 grants in UL based on allocated RARs
void ra_sched::ul_sched(sf_sched* sf_dl_sched, sf_sched* sf_msg3_sched)
{
  const std::vector<sf_sched::rar_alloc_t>& alloc_rars = sf_dl_sched->get_allocated_rars();

  for (const auto& rar : alloc_rars) {
    for (uint32_t j = 0; j < rar.rar_grant.nof_grants; ++j) {
      const auto& msg3grant = rar.rar_grant.msg3_grant[j];

      uint16_t crnti   = msg3grant.data.temp_crnti;
      auto     user_it = ue_db->find(crnti);
      if (user_it != ue_db->end() and sf_msg3_sched->alloc_msg3(&user_it->second, msg3grant)) {
        log_h->debug("SCHED: Queueing Msg3 for rnti=0x%x at tti=%d\n", crnti, sf_msg3_sched->get_tti_tx_ul());
      } else {
        log_h->error("SCHED: Failed to allocate Msg3 for rnti=0x%x at tti=%d\n", crnti, sf_msg3_sched->get_tti_tx_ul());
      }
    }
  }
}

void ra_sched::reset()
{
  pending_rars.clear();
}

/*******************************************************
 *                 Carrier scheduling
 *******************************************************/

sched::carrier_sched::carrier_sched(rrc_interface_mac*            rrc_,
                                    std::map<uint16_t, sched_ue>* ue_db_,
                                    uint32_t                      enb_cc_idx_) :
  rrc(rrc_),
  ue_db(ue_db_),
  log_h(srslte::logmap::get("MAC ")),
  enb_cc_idx(enb_cc_idx_)
{
  sf_dl_mask.resize(1, 0);
}

sched::carrier_sched::~carrier_sched() {}

void sched::carrier_sched::reset()
{
  ra_sched_ptr.reset();
  bc_sched_ptr.reset();
}

void sched::carrier_sched::carrier_cfg(const sched_cell_params_t& cell_params_)
{
  // carrier_sched is now fully set
  cc_cfg = &cell_params_;

  // init Broadcast/RA schedulers
  bc_sched_ptr.reset(new bc_sched{*cc_cfg, rrc});
  ra_sched_ptr.reset(new ra_sched{*cc_cfg, *ue_db});

  // Setup data scheduling algorithms
  dl_metric.reset(new srsenb::dl_metric_rr{});
  dl_metric->set_params(*cc_cfg);
  ul_metric.reset(new srsenb::ul_metric_rr{});
  ul_metric->set_params(*cc_cfg);

  // Setup constant PUCCH/PRACH mask
  pucch_mask.resize(cc_cfg->nof_prb());
  if (cc_cfg->cfg.nrb_pucch > 0) {
    pucch_mask.fill(0, (uint32_t)cc_cfg->cfg.nrb_pucch);
    pucch_mask.fill(cc_cfg->nof_prb() - cc_cfg->cfg.nrb_pucch, cc_cfg->nof_prb());
  }
  prach_mask.resize(cc_cfg->nof_prb());
  prach_mask.fill(cc_cfg->cfg.prach_freq_offset, cc_cfg->cfg.prach_freq_offset + 6);

  // Initiate the tti_scheduler for each TTI
  for (sf_sched& tti_sched : sf_scheds) {
    tti_sched.init(*cc_cfg);
  }
}

void sched::carrier_sched::set_dl_tti_mask(uint8_t* tti_mask, uint32_t nof_sfs)
{
  sf_dl_mask.assign(tti_mask, tti_mask + nof_sfs);
}

const sf_sched_result& sched::carrier_sched::generate_tti_result(uint32_t tti_rx)
{
  sf_sched_result* sf_result = get_next_sf_result(tti_rx);

  // if it is the first time tti is run, reset vars
  if (tti_rx != sf_result->tti_params.tti_rx) {
    sf_sched* tti_sched = get_sf_sched(tti_rx);
    *sf_result          = {};

    bool dl_active = sf_dl_mask[tti_sched->get_tti_tx_dl() % sf_dl_mask.size()] == 0;

    /* Schedule PHICH */
    for (auto& ue_pair : *ue_db) {
      tti_sched->alloc_phich(&ue_pair.second, &sf_result->ul_sched_result);
    }

    /* Schedule DL control data */
    if (dl_active) {
      /* Schedule Broadcast data (SIB and paging) */
      bc_sched_ptr->dl_sched(tti_sched);

      /* Schedule RAR */
      ra_sched_ptr->dl_sched(tti_sched);

      /* Schedule Msg3 */
      sf_sched* sf_msg3_sched = get_sf_sched(tti_rx + MSG3_DELAY_MS);
      ra_sched_ptr->ul_sched(tti_sched, sf_msg3_sched);
    }

    /* Prioritize PDCCH scheduling for DL and UL data in a RoundRobin fashion */
    if ((tti_rx % 2) == 0) {
      alloc_ul_users(tti_sched);
    }

    /* Schedule DL user data */
    alloc_dl_users(tti_sched);

    if ((tti_rx % 2) == 1) {
      alloc_ul_users(tti_sched);
    }

    /* Select the winner DCI allocation combination, store all the scheduling results */
    tti_sched->generate_sched_results(sf_result);

    /* Reset ue harq pending ack state, clean-up blocked pids */
    for (auto& user : *ue_db) {
      user.second.finish_tti(sf_result->tti_params, enb_cc_idx);
    }
  }

  return *sf_result;
}

void sched::carrier_sched::alloc_dl_users(sf_sched* tti_result)
{
  if (sf_dl_mask[tti_result->get_tti_tx_dl() % sf_dl_mask.size()] != 0) {
    return;
  }

  // NOTE: In case of 6 PRBs, do not transmit if there is going to be a PRACH in the UL to avoid collisions
  if (cc_cfg->nof_prb() == 6) {
    uint32_t tti_rx_ack = tti_result->get_tti_params().tti_rx_ack_dl();
    if (srslte_prach_tti_opportunity_config_fdd(cc_cfg->cfg.prach_config, tti_rx_ack, -1)) {
      tti_result->reserve_dl_rbgs(0, cc_cfg->nof_rbgs);
    }
  }

  // call DL scheduler metric to fill RB grid
  dl_metric->sched_users(*ue_db, tti_result);
}

int sched::carrier_sched::alloc_ul_users(sf_sched* tti_sched)
{
  uint32_t tti_tx_ul = tti_sched->get_tti_tx_ul();

  /* reserve PRBs for PRACH */
  if (srslte_prach_tti_opportunity_config_fdd(cc_cfg->cfg.prach_config, tti_tx_ul, -1)) {
    tti_sched->reserve_ul_prbs(prach_mask, false);
    log_h->debug("SCHED: Allocated PRACH RBs. Mask: 0x%s\n", prach_mask.to_hex().c_str());
  }

  /* reserve PRBs for PUCCH */
  tti_sched->reserve_ul_prbs(pucch_mask, cc_cfg->nof_prb() != 6);

  /* Call scheduler for UL data */
  ul_metric->sched_users(*ue_db, tti_sched);

  return SRSLTE_SUCCESS;
}

sf_sched* sched::carrier_sched::get_sf_sched(uint32_t tti_rx)
{
  sf_sched* ret = &sf_scheds[tti_rx % sf_scheds.size()];
  if (ret->get_tti_rx() != tti_rx) {
    // start new TTI. Bind the struct where the result is going to be stored
    ret->new_tti(tti_rx);
  }
  return ret;
}

sf_sched_result* sched::carrier_sched::get_next_sf_result(uint32_t tti_rx)
{
  return &sf_sched_results[tti_rx % sf_sched_results.size()];
}

const sf_sched_result& sched::carrier_sched::get_sf_result(uint32_t tti_rx) const
{
  return sf_sched_results[tti_rx % sf_sched_results.size()];
}

int sched::carrier_sched::dl_rach_info(dl_sched_rar_info_t rar_info)
{
  return ra_sched_ptr->dl_rach_info(rar_info);
}

} // namespace srsenb
