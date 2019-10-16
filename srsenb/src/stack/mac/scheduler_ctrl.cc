/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include "srsenb/hdr/stack/mac/scheduler_ctrl.h"

namespace srsenb {

sched::bc_sched_t::bc_sched_t(cell_cfg_t* cfg_) : cfg(cfg_) {}

void sched::bc_sched_t::init(srsenb::rrc_interface_mac* rrc_)
{
  rrc = rrc_;
}

void sched::bc_sched_t::dl_sched(sched::tti_sched_result_t* tti_sched)
{
  current_sf_idx = tti_sched->get_sf_idx();
  current_sfn    = tti_sched->get_sfn();
  current_tti    = tti_sched->get_tti_tx_dl();
  bc_aggr_level  = 2;

  /* Activate/deactivate SI windows */
  update_si_windows(tti_sched);

  /* Allocate DCIs and RBGs for each SIB */
  alloc_sibs(tti_sched);

  /* Allocate Paging */
  // NOTE: It blocks
  alloc_paging(tti_sched);
}

void sched::bc_sched_t::update_si_windows(tti_sched_result_t* tti_sched)
{
  uint32_t tti_tx_dl = tti_sched->get_tti_tx_dl();

  for (uint32_t i = 0; i < pending_sibs.size(); ++i) {
    // There is SIB data
    if (cfg->sibs[i].len == 0) {
      continue;
    }

    if (not pending_sibs[i].is_in_window) {
      uint32_t sf = 5;
      uint32_t x  = 0;
      if (i > 0) {
        x  = (i - 1) * cfg->si_window_ms;
        sf = x % 10;
      }
      if ((current_sfn % (cfg->sibs[i].period_rf)) == x / 10 && current_sf_idx == sf) {
        pending_sibs[i].is_in_window = true;
        pending_sibs[i].window_start = tti_tx_dl;
        pending_sibs[i].n_tx         = 0;
      }
    } else {
      if (i > 0) {
        if (srslte_tti_interval(tti_tx_dl, pending_sibs[i].window_start) > cfg->si_window_ms) {
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

void sched::bc_sched_t::alloc_sibs(tti_sched_result_t* tti_sched)
{
  for (uint32_t i = 0; i < pending_sibs.size(); i++) {
    if (cfg->sibs[i].len > 0 and pending_sibs[i].is_in_window and pending_sibs[i].n_tx < 4) {
      uint32_t nof_tx = (i > 0) ? SRSLTE_MIN(srslte::ceil_div(cfg->si_window_ms, 10), 4) : 4;
      uint32_t n_sf   = (tti_sched->get_tti_tx_dl() - pending_sibs[i].window_start);

      // Check if there is any SIB to tx
      bool sib1_flag = (i == 0) and (current_sfn % 2) == 0 and current_sf_idx == 5;
      bool other_sibs_flag =
          (i > 0) and (n_sf >= (cfg->si_window_ms / nof_tx) * pending_sibs[i].n_tx) and current_sf_idx == 9;
      if (not sib1_flag and not other_sibs_flag) {
        continue;
      }

      // Schedule SIB
      tti_sched->alloc_bc(bc_aggr_level, i, pending_sibs[i].n_tx);
      pending_sibs[i].n_tx++;
    }
  }
}

void sched::bc_sched_t::alloc_paging(srsenb::sched::tti_sched_result_t* tti_sched)
{
  /* Allocate DCIs and RBGs for paging */
  if (rrc != nullptr) {
    uint32_t paging_payload = 0;
    if (rrc->is_paging_opportunity(current_tti, &paging_payload) and paging_payload > 0) {
      tti_sched->alloc_paging(bc_aggr_level, paging_payload);
    }
  }
}

void sched::bc_sched_t::reset()
{
  for (auto& sib : pending_sibs) {
    sib = {};
  }
}

/*******************************************************
 *                 RAR scheduling
 *******************************************************/

sched::ra_sched_t::ra_sched_t(cell_cfg_t* cfg_) : cfg(cfg_) {}

void sched::ra_sched_t::init(srslte::log* log_, std::map<uint16_t, sched_ue>& ue_db_)
{
  log_h = log_;
  ue_db = &ue_db_;
}

// Schedules RAR
// On every call to this function, we schedule the oldest RAR which is still within the window. If outside the window we discard it.
void sched::ra_sched_t::dl_sched(srsenb::sched::tti_sched_result_t* tti_sched)
{
  tti_tx_dl      = tti_sched->get_tti_tx_dl();
  rar_aggr_level = 2;

  // Discard all RARs out of the window. The first one inside the window is scheduled, if we can't we exit
  while (!pending_rars.empty()) {
    dl_sched_rar_info_t rar = pending_rars.front();
    if (not sched_utils::is_in_tti_interval(tti_tx_dl,
                                            rar.prach_tti + 3,
                                            rar.prach_tti + 3 + cfg->prach_rar_window))
    {
      if (tti_tx_dl >= rar.prach_tti + 3 + cfg->prach_rar_window) {
        log_h->console("SCHED: Could not transmit RAR within the window (RA TTI=%d, Window=%d, Now=%d)\n",
                       rar.prach_tti,
                       cfg->prach_rar_window,
                       tti_tx_dl);
        log_h->error("SCHED: Could not transmit RAR within the window (RA TTI=%d, Window=%d, Now=%d)\n",
                     rar.prach_tti,
                     cfg->prach_rar_window,
                     tti_tx_dl);
        // Remove from pending queue and get next one if window has passed already
        pending_rars.pop();
        continue;
      }
      // If window not yet started do not look for more pending RARs
      return;
    }

    /* Since we do a fixed Msg3 scheduling for all RAR, we can only allocate 1 RAR per TTI.
     * If we have enough space in the window, every call to this function we'll allocate 1 pending RAR and associate a
     * Msg3 transmission
     */
    dl_sched_rar_t rar_grant;
    uint32_t L_prb = 3;
    uint32_t n_prb = cfg->nrb_pucch>0?cfg->nrb_pucch:2;
    bzero(&rar_grant, sizeof(rar_grant));
    uint32_t rba = srslte_ra_type2_to_riv(L_prb, n_prb, cfg->cell.nof_prb);

    dl_sched_rar_grant_t *grant = &rar_grant.msg3_grant[0];
    grant->grant.tpc_pusch = 3;
    grant->grant.trunc_mcs = 0;
    grant->grant.rba = rba;
    grant->data = rar;
    rar_grant.nof_grants++;

    // Try to schedule DCI + RBGs for RAR Grant
    tti_sched_result_t::rar_code_t ret = tti_sched->alloc_rar(rar_aggr_level,
                                                       rar_grant,
                                                       rar.prach_tti,
                                                       7 * rar_grant.nof_grants); //fixme: check RAR size

    // If we can allocate, schedule Msg3 and remove from pending
    if (!ret.first) {
      return;
    }

    // Schedule Msg3 only if there is a requirement for Msg3 data
    uint32_t pending_tti = (tti_sched->get_tti_tx_dl() + MSG3_DELAY_MS + TX_DELAY) % TTIMOD_SZ;
    pending_msg3[pending_tti].enabled = true;
    pending_msg3[pending_tti].rnti = rar.temp_crnti; // FIXME
    pending_msg3[pending_tti].L = L_prb;
    pending_msg3[pending_tti].n_prb = n_prb;
    dl_sched_rar_grant_t *last_msg3 = &rar_grant.msg3_grant[rar_grant.nof_grants - 1];
    pending_msg3[pending_tti].mcs = last_msg3->grant.trunc_mcs;
    log_h->info("SCHED: Allocating Msg3 for rnti=%d at tti=%d\n", rar.temp_crnti, tti_sched->get_tti_tx_dl() + MSG3_DELAY_MS + TX_DELAY);

    // Remove pending RAR and exit
    pending_rars.pop();
    return;
  }
}

// Schedules Msg3
void sched::ra_sched_t::ul_sched(srsenb::sched::tti_sched_result_t* tti_sched)
{
  uint32_t pending_tti = tti_sched->get_tti_tx_ul() % TTIMOD_SZ;

  // check if there is a Msg3 to allocate
  if (not pending_msg3[pending_tti].enabled) {
    return;
  }

  uint16_t rnti    = pending_msg3[pending_tti].rnti;
  auto     user_it = ue_db->find(rnti);
  if (user_it == ue_db->end()) {
    log_h->warning("SCHED: Msg3 allocated for user rnti=0x%x that no longer exists\n", rnti);
    return;
  }

  /* Allocate RBGs and HARQ for Msg3 */
  ul_harq_proc::ul_alloc_t msg3 = {pending_msg3[pending_tti].n_prb, pending_msg3[pending_tti].L};
  if (not tti_sched->alloc_ul_msg3(&user_it->second, msg3, pending_msg3[pending_tti].mcs)) {
    log_h->warning("SCHED: Could not allocate msg3 within (%d,%d)\n", msg3.RB_start, msg3.RB_start + msg3.L);
    return;
  }
  pending_msg3[pending_tti].enabled = false;
}

int sched::ra_sched_t::dl_rach_info(dl_sched_rar_info_t rar_info)
{
  log_h->info("SCHED: New RAR tti=%d, preamble=%d, temp_crnti=0x%x, ta_cmd=%d, msg3_size=%d\n",
       rar_info.prach_tti, rar_info.preamble_idx, rar_info.temp_crnti, rar_info.ta_cmd, rar_info.msg3_size);
  pending_rars.push(rar_info);
  return 0;
}

void sched::ra_sched_t::reset()
{
  tti_tx_dl = 0;
  for (auto& msg3 : pending_msg3) {
    msg3 = {};
  }
  while (not pending_rars.empty()) {
    pending_rars.pop();
  }
}

const sched::ra_sched_t::pending_msg3_t& sched::ra_sched_t::find_pending_msg3(uint32_t tti)
{
  uint32_t pending_tti = tti % TTIMOD_SZ;
  return pending_msg3[pending_tti];
}

} // namespace srsenb
