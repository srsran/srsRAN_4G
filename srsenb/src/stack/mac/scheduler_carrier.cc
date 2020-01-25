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

#include "srsenb/hdr/stack/mac/scheduler_carrier.h"

#define Error(fmt, ...) log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...) log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...) log_h->debug(fmt, ##__VA_ARGS__)

namespace srsenb {

/*******************************************************
 *        Broadcast (SIB+Paging) scheduling
 *******************************************************/

bc_sched::bc_sched(const sched::cell_cfg_t& cfg_, srsenb::rrc_interface_mac* rrc_) : cfg(&cfg_), rrc(rrc_) {}

void bc_sched::dl_sched(sf_sched* tti_sched)
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

void bc_sched::update_si_windows(sf_sched* tti_sched)
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

void bc_sched::alloc_sibs(sf_sched* tti_sched)
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

ra_sched::ra_sched(const sched::cell_cfg_t& cfg_, srslte::log* log_, std::map<uint16_t, sched_ue>& ue_db_) :
  cfg(&cfg_),
  log_h(log_),
  ue_db(&ue_db_)
{
}

// Schedules RAR
// On every call to this function, we schedule the oldest RAR which is still within the window. If outside the window we
// discard it.
void ra_sched::dl_sched(srsenb::sf_sched* tti_sched)
{
  tti_tx_dl      = tti_sched->get_tti_tx_dl();
  rar_aggr_level = 2;

  // Discard all RARs out of the window. The first one inside the window is scheduled, if we can't we exit
  while (!pending_rars.empty()) {
    dl_sched_rar_info_t rar = pending_rars.front();
    if (not sched_utils::is_in_tti_interval(tti_tx_dl, rar.prach_tti + 3, rar.prach_tti + 3 + cfg->prach_rar_window)) {
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
    uint32_t       L_prb = 3;
    uint32_t       n_prb = cfg->nrb_pucch > 0 ? cfg->nrb_pucch : 2;
    bzero(&rar_grant, sizeof(rar_grant));
    uint32_t rba = srslte_ra_type2_to_riv(L_prb, n_prb, cfg->cell.nof_prb);

    dl_sched_rar_grant_t* grant = &rar_grant.msg3_grant[0];
    grant->grant.tpc_pusch      = 3;
    grant->grant.trunc_mcs      = 0;
    grant->grant.rba            = rba;
    grant->data                 = rar;
    rar_grant.nof_grants++;

    // Try to schedule DCI + RBGs for RAR Grant
    sf_sched::rar_code_t ret = tti_sched->alloc_rar(rar_aggr_level,
                                                    rar_grant,
                                                    rar.prach_tti,
                                                    7 * rar_grant.nof_grants); // fixme: check RAR size

    // If we can allocate, schedule Msg3 and remove from pending
    if (!ret.first) {
      return;
    }

    // Schedule Msg3 only if there is a requirement for Msg3 data
    uint32_t pending_tti              = (tti_sched->get_tti_tx_dl() + MSG3_DELAY_MS + TX_DELAY) % TTIMOD_SZ;
    pending_msg3[pending_tti].enabled = true;
    pending_msg3[pending_tti].rnti    = rar.temp_crnti; // TODO
    pending_msg3[pending_tti].L       = L_prb;
    pending_msg3[pending_tti].n_prb   = n_prb;
    dl_sched_rar_grant_t* last_msg3   = &rar_grant.msg3_grant[rar_grant.nof_grants - 1];
    pending_msg3[pending_tti].mcs     = last_msg3->grant.trunc_mcs;
    log_h->info("SCHED: Queueing Msg3 for rnti=0x%x at tti=%d\n",
                rar.temp_crnti,
                tti_sched->get_tti_tx_dl() + MSG3_DELAY_MS + TX_DELAY);

    // Remove pending RAR
    pending_rars.pop();

    return;
  }
}

// Schedules Msg3
void ra_sched::ul_sched(sf_sched* tti_sched)
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

int ra_sched::dl_rach_info(dl_sched_rar_info_t rar_info)
{
  log_h->info("SCHED: New RAR tti=%d, preamble=%d, temp_crnti=0x%x, ta_cmd=%d, msg3_size=%d\n",
              rar_info.prach_tti,
              rar_info.preamble_idx,
              rar_info.temp_crnti,
              rar_info.ta_cmd,
              rar_info.msg3_size);
  pending_rars.push(rar_info);
  return 0;
}

void ra_sched::reset()
{
  tti_tx_dl = 0;
  for (auto& msg3 : pending_msg3) {
    msg3 = {};
  }
  while (not pending_rars.empty()) {
    pending_rars.pop();
  }
}

const ra_sched::pending_msg3_t& ra_sched::find_pending_msg3(uint32_t tti) const
{
  uint32_t pending_tti = tti % TTIMOD_SZ;
  return pending_msg3[pending_tti];
}

/*******************************************************
 *                 Carrier scheduling
 *******************************************************/

sched::carrier_sched::carrier_sched(rrc_interface_mac*            rrc_,
                                    std::map<uint16_t, sched_ue>* ue_db_,
                                    uint32_t                      enb_cc_idx_) :
  rrc(rrc_),
  ue_db(ue_db_),
  enb_cc_idx(enb_cc_idx_)
{
  sf_dl_mask.resize(1, 0);
}

void sched::carrier_sched::reset()
{
  std::lock_guard<std::mutex> lock(carrier_mutex);
  ra_sched_ptr.reset();
  bc_sched_ptr.reset();
}

void sched::carrier_sched::carrier_cfg(const sched_params_t& sched_params_)
{
  // sched::cfg is now fully set
  sched_params = &sched_params_;
  log_h        = sched_params->log_h;

  const cell_cfg_t*           cfg_ = sched_params->cfg;
  std::lock_guard<std::mutex> lock(carrier_mutex);

  // init Broadcast/RA schedulers
  bc_sched_ptr.reset(new bc_sched{*sched_params->cfg, rrc});
  ra_sched_ptr.reset(new ra_sched{*sched_params->cfg, log_h, *ue_db});

  dl_metric->set_params(*sched_params);
  ul_metric->set_params(*sched_params);

  // Setup constant PUCCH/PRACH mask
  pucch_mask.resize(cfg_->cell.nof_prb);
  if (cfg_->nrb_pucch > 0) {
    pucch_mask.fill(0, (uint32_t)cfg_->nrb_pucch);
    pucch_mask.fill(cfg_->cell.nof_prb - cfg_->nrb_pucch, cfg_->cell.nof_prb);
  }
  prach_mask.resize(cfg_->cell.nof_prb);
  prach_mask.fill(cfg_->prach_freq_offset, cfg_->prach_freq_offset + 6);

  // Initiate the tti_scheduler for each TTI
  for (sf_sched& tti_sched : sf_scheds) {
    tti_sched.init(*sched_params, enb_cc_idx);
  }
}

void sched::carrier_sched::set_metric(sched::metric_dl* dl_metric_, sched::metric_ul* ul_metric_)
{
  dl_metric = dl_metric_;
  ul_metric = ul_metric_;
}

void sched::carrier_sched::set_dl_tti_mask(uint8_t* tti_mask, uint32_t nof_sfs)
{
  sf_dl_mask.assign(tti_mask, tti_mask + nof_sfs);
}

sf_sched* sched::carrier_sched::generate_tti_result(uint32_t tti_rx)
{
  sf_sched* tti_sched = get_sf_sched(tti_rx);

  // if it is the first time tti is run, reset vars
  if (tti_rx != tti_sched->get_tti_rx()) {
    uint32_t start_cfi = sched_params->sched_cfg.nof_ctrl_symbols;
    tti_sched->new_tti(tti_rx, start_cfi);

    // Protects access to pending_rar[], pending_msg3[], ra_sched, bc_sched, rlc buffers
    std::lock_guard<std::mutex> lock(carrier_mutex);

    /* Schedule PHICH */
    generate_phich(tti_sched);

    /* Schedule DL control data */
    if (sf_dl_mask[tti_sched->get_tti_tx_dl() % sf_dl_mask.size()] == 0) {
      /* Schedule Broadcast data (SIB and paging) */
      bc_sched_ptr->dl_sched(tti_sched);

      /* Schedule RAR */
      ra_sched_ptr->dl_sched(tti_sched);
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

    /* Select the winner DCI allocation combination */
    tti_sched->generate_dcis();

    /* clean-up blocked pids */
    for (auto& user : *ue_db) {
      user.second.finish_tti(tti_sched->get_tti_params(), enb_cc_idx);
    }
  }

  return tti_sched;
}

void sched::carrier_sched::generate_phich(sf_sched* tti_sched)
{
  // Allocate user PHICHs
  uint32_t nof_phich_elems = 0;
  for (auto& ue_pair : *ue_db) {
    sched_ue& user = ue_pair.second;
    uint16_t  rnti = ue_pair.first;
    auto      p    = user.get_cell_index(enb_cc_idx);
    if (not p.first) {
      // user does not support this carrier
      continue;
    }
    uint32_t cell_index = p.second;

    //    user.has_pucch = false; // TODO: What is this for?

    ul_harq_proc* h = user.get_ul_harq(tti_sched->get_tti_rx(), cell_index);

    /* Indicate PHICH acknowledgment if needed */
    if (h->has_pending_ack()) {
      tti_sched->ul_sched_result.phich[nof_phich_elems].phich =
          h->get_pending_ack() ? ul_sched_phich_t::ACK : ul_sched_phich_t::NACK;
      tti_sched->ul_sched_result.phich[nof_phich_elems].rnti = rnti;
      log_h->debug("SCHED: Allocated PHICH for rnti=0x%x, value=%d\n",
                   rnti,
                   tti_sched->ul_sched_result.phich[nof_phich_elems].phich);
      nof_phich_elems++;
    }
  }
  tti_sched->ul_sched_result.nof_phich_elems = nof_phich_elems;
}

void sched::carrier_sched::alloc_dl_users(sf_sched* tti_result)
{
  if (sf_dl_mask[tti_result->get_tti_tx_dl() % sf_dl_mask.size()] != 0) {
    return;
  }

  // NOTE: In case of 6 PRBs, do not transmit if there is going to be a PRACH in the UL to avoid collisions
  if (sched_params->cfg->cell.nof_prb == 6) {
    uint32_t tti_rx_ack   = TTI_RX_ACK(tti_result->get_tti_rx());
    bool     msg3_enabled = false;
    if (ra_sched_ptr != nullptr and ra_sched_ptr->find_pending_msg3(tti_rx_ack).enabled) {
      msg3_enabled = true;
    }
    if (srslte_prach_tti_opportunity_config_fdd(sched_params->cfg->prach_config, tti_rx_ack, -1) or msg3_enabled) {
      tti_result->get_dl_mask().fill(0, tti_result->get_dl_mask().size());
    }
  }

  // call DL scheduler metric to fill RB grid
  dl_metric->sched_users(*ue_db, tti_result, enb_cc_idx);
}

int sched::carrier_sched::alloc_ul_users(sf_sched* tti_sched)
{
  uint32_t   tti_tx_ul = tti_sched->get_tti_tx_ul();
  prbmask_t& ul_mask   = tti_sched->get_ul_mask();

  /* reserve PRBs for PRACH */
  if (srslte_prach_tti_opportunity_config_fdd(sched_params->cfg->prach_config, tti_tx_ul, -1)) {
    ul_mask = prach_mask;
    log_h->debug("SCHED: Allocated PRACH RBs. Mask: 0x%s\n", prach_mask.to_hex().c_str());
  }

  /* Allocate Msg3 if there's a pending RAR */
  ra_sched_ptr->ul_sched(tti_sched);

  /* reserve PRBs for PUCCH */
  if (sched_params->cfg->cell.nof_prb != 6 and (ul_mask & pucch_mask).any()) {
    log_h->error("There was a collision with the PUCCH. current mask=0x%s, pucch_mask=0x%s\n",
                 ul_mask.to_hex().c_str(),
                 pucch_mask.to_hex().c_str());
  }
  ul_mask |= pucch_mask;

  /* Call scheduler for UL data */
  ul_metric->sched_users(*ue_db, tti_sched, enb_cc_idx);

  return SRSLTE_SUCCESS;
}

int sched::carrier_sched::dl_rach_info(dl_sched_rar_info_t rar_info)
{
  std::lock_guard<std::mutex> lock(carrier_mutex);
  return ra_sched_ptr->dl_rach_info(rar_info);
}

} // namespace srsenb
