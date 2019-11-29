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

#include <srsenb/hdr/stack/mac/scheduler_ue.h>
#include <string.h>

#include "srsenb/hdr/stack/mac/scheduler.h"
#include "srsenb/hdr/stack/mac/scheduler_carrier.h"
#include "srslte/common/pdu.h"
#include "srslte/srslte.h"

#define Error(fmt, ...) log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...) log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...) log_h->debug(fmt, ##__VA_ARGS__)

namespace srsenb {

namespace sched_utils {

uint32_t tti_subtract(uint32_t tti1, uint32_t tti2)
{
  return (tti1 + 10240 - tti2) % 10240;
}

uint32_t max_tti(uint32_t tti1, uint32_t tti2)
{
  return ((tti1 - tti2) > 10240 / 2) ? SRSLTE_MIN(tti1, tti2) : SRSLTE_MAX(tti1, tti2);
}

} // namespace sched_utils

/*******************************************************
 *                 Sched Params
 *******************************************************/

bool sched_params_t::set_derived()
{
  // Compute Common locations for DCI for each CFI
  for (uint32_t cfi = 0; cfi < 3; cfi++) {
    sched::generate_cce_location(regs, &common_locations[cfi], cfi + 1);
  }

  // Compute UE locations for RA-RNTI
  for (uint32_t cfi = 0; cfi < 3; cfi++) {
    for (uint32_t sf_idx = 0; sf_idx < 10; sf_idx++) {
      sched::generate_cce_location(regs, &rar_locations[cfi][sf_idx], cfi + 1, sf_idx);
    }
  }

  P        = srslte_ra_type0_P(cfg->cell.nof_prb);
  nof_rbgs = srslte::ceil_div(cfg->cell.nof_prb, P);

  // precompute nof cces in PDCCH for each CFI
  for (uint32_t cfix = 0; cfix < nof_cce_table.size(); ++cfix) {
    int ret = srslte_regs_pdcch_ncce(regs, cfix + 1);
    if (ret < 0) {
      log_h->error("SCHED: Failed to calculate the number of CCEs in the PDCCH\n");
      return false;
    }
    nof_cce_table[cfix] = (uint32_t)ret;
  }

  if (common_locations[sched_cfg.nof_ctrl_symbols - 1].nof_loc[2] == 0) {
    log_h->error("SCHED: Current cfi=%d is not valid for broadcast (check scheduler.nof_ctrl_symbols in conf file).\n",
                 sched_cfg.nof_ctrl_symbols);
    log_h->console(
        "SCHED: Current cfi=%d is not valid for broadcast (check scheduler.nof_ctrl_symbols in conf file).\n",
        sched_cfg.nof_ctrl_symbols);
    return false;
  }

  return true;
}

/*******************************************************
 *
 * Initialization and sched configuration functions
 *
 *******************************************************/
sched::sched()
{
  current_tti = 0;
  log_h       = nullptr;
  rrc         = nullptr;

  bzero(&cfg, sizeof(cfg));
  bzero(&regs, sizeof(regs));
  bzero(&pdsch_re, sizeof(pdsch_re));

  pthread_rwlock_init(&rwlock, nullptr);

  // Initialize Independent carrier schedulers
  carrier_schedulers.emplace_back(new carrier_sched{this, 0});

  reset();
}

sched::~sched()
{
  srslte_regs_free(&regs);
  pthread_rwlock_wrlock(&rwlock);
  pthread_rwlock_unlock(&rwlock);
  pthread_rwlock_destroy(&rwlock);
}

void sched::init(rrc_interface_mac* rrc_, srslte::log* log)
{
  sched_params.sched_cfg.pdsch_max_mcs    = 28;
  sched_params.sched_cfg.pdsch_mcs        = -1;
  sched_params.sched_cfg.pusch_max_mcs    = 28;
  sched_params.sched_cfg.pusch_mcs        = -1;
  sched_params.sched_cfg.nof_ctrl_symbols = 3;
  sched_params.sched_cfg.max_aggr_level   = 3;
  sched_params.log_h                      = log;

  log_h = log;
  rrc   = rrc_;
  reset();
}

int sched::reset()
{
  configured = false;
  for (std::unique_ptr<carrier_sched>& c : carrier_schedulers) {
    c->reset();
  }
  pthread_rwlock_wrlock(&rwlock);
  ue_db.clear();
  pthread_rwlock_unlock(&rwlock);
  return 0;
}

void sched::set_sched_cfg(sched_interface::sched_args_t* sched_cfg_)
{
  if (sched_cfg_ != nullptr) {
    sched_params.sched_cfg = *sched_cfg_;
  }
}

void sched::set_metric(sched::metric_dl* dl_metric_, sched::metric_ul* ul_metric_)
{
  for (std::unique_ptr<carrier_sched>& c : carrier_schedulers) {
    c->set_metric(dl_metric_, ul_metric_);
  }
}

int sched::cell_cfg(sched_interface::cell_cfg_t* cell_cfg)
{
  // Basic cell config checks
  if (cell_cfg->si_window_ms == 0) {
    Error("SCHED: Invalid si-window length 0 ms\n");
    return -1;
  }

  cfg = *cell_cfg;

  // Get DCI locations
  if (srslte_regs_init(&regs, cfg.cell) != LIBLTE_SUCCESS) {
    Error("Getting DCI locations\n");
    return SRSLTE_ERROR;
  }

  sched_params.cfg  = &cfg;
  sched_params.regs = &regs;
  if (not sched_params.set_derived()) {
    return -1;
  }

  // Initiate the tti_scheduler for each TTI
  for (std::unique_ptr<carrier_sched>& c : carrier_schedulers) {
    c->carrier_cfg();
  }
  configured = true;

  // PRACH has to fit within the PUSCH space
  bool invalid_prach = cfg.cell.nof_prb == 6 and (cfg.prach_freq_offset + 6 > cfg.cell.nof_prb);
  invalid_prach |= cfg.cell.nof_prb > 6 and ((cfg.prach_freq_offset + 6) > (cfg.cell.nof_prb - cfg.nrb_pucch) or
                                             (int) cfg.prach_freq_offset < cfg.nrb_pucch);
  if (invalid_prach) {
    log_h->error("Invalid PRACH configuration: frequency offset=%d outside bandwidth limits\n", cfg.prach_freq_offset);
    log_h->console("Invalid PRACH configuration: frequency offset=%d outside bandwidth limits\n",
                   cfg.prach_freq_offset);
    return -1;
  }

  return 0;
}

/*******************************************************
 *
 * FAPI-like main sched interface. Wrappers to UE object
 *
 *******************************************************/

int sched::ue_cfg(uint16_t rnti, sched_interface::ue_cfg_t* ue_cfg)
{
  // Add or config user
  pthread_rwlock_wrlock(&rwlock);
  ue_db[rnti].set_cfg(rnti, sched_params, ue_cfg);
  pthread_rwlock_unlock(&rwlock);

  return 0;
}

int sched::ue_rem(uint16_t rnti)
{
  int ret = 0;
  pthread_rwlock_wrlock(&rwlock);
  if (ue_db.count(rnti) > 0) {
    ue_db.erase(rnti);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

bool sched::ue_exists(uint16_t rnti)
{
  return ue_db_access(rnti, [](sched_ue& ue) {}) >= 0;
}

void sched::ue_needs_ta_cmd(uint16_t rnti, uint32_t nof_ta_cmd)
{
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti) > 0) {
    ue_db[rnti].set_needs_ta_cmd(nof_ta_cmd);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
  pthread_rwlock_unlock(&rwlock);
}

void sched::phy_config_enabled(uint16_t rnti, uint32_t cc_idx, bool enabled)
{
  // FIXME: Check if correct use of current_tti
  ue_db_access(rnti, [this, cc_idx, enabled](sched_ue& ue) { ue.phy_config_enabled(current_tti, cc_idx, enabled); });
}

int sched::bearer_ue_cfg(uint16_t rnti, uint32_t lc_id, sched_interface::ue_bearer_cfg_t* cfg_)
{
  return ue_db_access(rnti, [lc_id, cfg_](sched_ue& ue) { ue.set_bearer_cfg(lc_id, cfg_); });
}

int sched::bearer_ue_rem(uint16_t rnti, uint32_t lc_id)
{
  return ue_db_access(rnti, [lc_id](sched_ue& ue) { ue.rem_bearer(lc_id); });
}

uint32_t sched::get_dl_buffer(uint16_t rnti)
{
  // FIXME: Check if correct use of current_tti
  uint32_t ret = 0;
  ue_db_access(rnti, [&ret](sched_ue& ue) { ret = ue.get_pending_dl_new_data(); });
  return ret;
}

uint32_t sched::get_ul_buffer(uint16_t rnti)
{
  // FIXME: Check if correct use of current_tti
  uint32_t ret = 0;
  ue_db_access(rnti, [this, &ret](sched_ue& ue) { ret = ue.get_pending_ul_new_data(current_tti); });
  return ret;
}

int sched::dl_rlc_buffer_state(uint16_t rnti, uint32_t lc_id, uint32_t tx_queue, uint32_t retx_queue)
{
  return ue_db_access(rnti,
                      [lc_id, tx_queue, retx_queue](sched_ue& ue) { ue.dl_buffer_state(lc_id, tx_queue, retx_queue); });
}

int sched::dl_mac_buffer_state(uint16_t rnti, uint32_t ce_code)
{
  return ue_db_access(rnti, [ce_code](sched_ue& ue) { ue.mac_buffer_state(ce_code); });
}

int sched::dl_ant_info(uint16_t rnti, asn1::rrc::phys_cfg_ded_s::ant_info_c_* dl_ant_info)
{
  return ue_db_access(rnti, [dl_ant_info](sched_ue& ue) { ue.set_dl_ant_info(dl_ant_info); });
}

int sched::dl_ack_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t tb_idx, bool ack)
{
  int ret = -1;
  ue_db_access(rnti,
               [tti, cc_idx, tb_idx, ack, &ret](sched_ue& ue) { ret = ue.set_ack_info(tti, cc_idx, tb_idx, ack); });
  return ret;
}

int sched::ul_crc_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, bool crc)
{
  return ue_db_access(rnti, [tti, cc_idx, crc](sched_ue& ue) { ue.set_ul_crc(tti, cc_idx, crc); });
}

int sched::dl_ri_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t ri_value)
{
  return ue_db_access(rnti, [tti, cc_idx, ri_value](sched_ue& ue) { ue.set_dl_ri(tti, cc_idx, ri_value); });
}

int sched::dl_pmi_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t pmi_value)
{
  return ue_db_access(rnti, [tti, cc_idx, pmi_value](sched_ue& ue) { ue.set_dl_pmi(tti, cc_idx, pmi_value); });
}

int sched::dl_cqi_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t cqi_value)
{
  return ue_db_access(rnti, [tti, cc_idx, cqi_value](sched_ue& ue) { ue.set_dl_cqi(tti, cc_idx, cqi_value); });
}

int sched::dl_rach_info(dl_sched_rar_info_t rar_info)
{
  return carrier_schedulers[0]->dl_rach_info(rar_info);
}

int sched::ul_cqi_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t cqi, uint32_t ul_ch_code)
{
  return ue_db_access(rnti,
                      [tti, cc_idx, cqi, ul_ch_code](sched_ue& ue) { ue.set_ul_cqi(tti, cc_idx, cqi, ul_ch_code); });
}

int sched::ul_bsr(uint16_t rnti, uint32_t lcid, uint32_t bsr, bool set_value)
{
  return ue_db_access(rnti, [lcid, bsr, set_value](sched_ue& ue) { ue.ul_buffer_state(lcid, bsr, set_value); });
}

int sched::ul_recv_len(uint16_t rnti, uint32_t lcid, uint32_t len)
{
  return ue_db_access(rnti, [lcid, len](sched_ue& ue) { ue.ul_recv_len(lcid, len); });
}

int sched::ul_phr(uint16_t rnti, int phr)
{
  return ue_db_access(rnti, [phr](sched_ue& ue) { ue.ul_phr(phr); });
}

int sched::ul_sr_info(uint32_t tti, uint16_t rnti)
{
  return ue_db_access(rnti, [](sched_ue& ue) { ue.set_sr(); });
}

void sched::set_dl_tti_mask(uint8_t* tti_mask, uint32_t nof_sfs)
{
  carrier_schedulers[0]->set_dl_tti_mask(tti_mask, nof_sfs);
}

void sched::tpc_inc(uint16_t rnti)
{
  ue_db_access(rnti, [](sched_ue& ue) { ue.tpc_inc(); });
}

void sched::tpc_dec(uint16_t rnti)
{
  ue_db_access(rnti, [](sched_ue& ue) { ue.tpc_dec(); });
}

/*******************************************************
 *
 * Main sched functions
 *
 *******************************************************/

// Downlink Scheduler API
int sched::dl_sched(uint32_t tti, sched_interface::dl_sched_res_t* sched_result)
{
  if (!configured) {
    return 0;
  }

  uint32_t tti_rx = sched_utils::tti_subtract(tti, TX_DELAY);
  current_tti     = sched_utils::max_tti(current_tti, tti_rx);

  // Compute scheduling Result for tti_rx
  pthread_rwlock_rdlock(&rwlock);
  tti_sched_result_t* tti_sched = carrier_schedulers[0]->generate_tti_result(tti_rx);
  pthread_rwlock_unlock(&rwlock);

  // Copy result
  *sched_result = tti_sched->dl_sched_result;

  return 0;
}

// Uplink Scheduler API
int sched::ul_sched(uint32_t tti, srsenb::sched_interface::ul_sched_res_t* sched_result)
{
  if (!configured) {
    return 0;
  }

  // Compute scheduling Result for tti_rx
  uint32_t tti_rx = sched_utils::tti_subtract(tti, 2 * FDD_HARQ_DELAY_MS);
  pthread_rwlock_rdlock(&rwlock);
  tti_sched_result_t* tti_sched = carrier_schedulers[0]->generate_tti_result(tti_rx);
  pthread_rwlock_unlock(&rwlock);

  // Copy result
  *sched_result = tti_sched->ul_sched_result;

  return SRSLTE_SUCCESS;
}

/*******************************************************
 *
 * Helper functions
 *
 *******************************************************/

void sched::generate_cce_location(srslte_regs_t*             regs_,
                                  sched_ue::sched_dci_cce_t* location,
                                  uint32_t                   cfi,
                                  uint32_t                   sf_idx,
                                  uint16_t                   rnti)
{
  bzero(location, sizeof(sched_ue::sched_dci_cce_t));

  srslte_dci_location_t loc[64];
  uint32_t              nloc = 0;
  if (rnti == 0) {
    nloc = srslte_pdcch_common_locations_ncce(srslte_regs_pdcch_ncce(regs_, cfi), loc, 64);
  } else {
    nloc = srslte_pdcch_ue_locations_ncce(srslte_regs_pdcch_ncce(regs_, cfi), loc, 64, sf_idx, rnti);
  }

  // Save to different format
  for (uint32_t i = 0; i < nloc; i++) {
    uint32_t l                                   = loc[i].L;
    location->cce_start[l][location->nof_loc[l]] = loc[i].ncce;
    location->nof_loc[l]++;
  }
}

// Common way to access ue_db elements in a read locking way
template <typename Func>
int sched::ue_db_access(uint16_t rnti, Func f)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  auto it = ue_db.find(rnti);
  if (it != ue_db.end()) {
    f(it->second);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

} // namespace srsenb
