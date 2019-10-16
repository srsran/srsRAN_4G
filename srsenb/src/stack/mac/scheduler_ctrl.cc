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

void sched::bc_sched_t::dl_sched(sched::tti_sched_t* tti_sched)
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

void sched::bc_sched_t::update_si_windows(tti_sched_t* tti_sched)
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

void sched::bc_sched_t::alloc_sibs(tti_sched_t* tti_sched)
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

void sched::bc_sched_t::alloc_paging(srsenb::sched::tti_sched_t* tti_sched)
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

} // namespace srsenb
