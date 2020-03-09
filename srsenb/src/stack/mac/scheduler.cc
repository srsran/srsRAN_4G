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
#include "srslte/common/logmap.h"
#include "srslte/common/pdu.h"
#include "srslte/srslte.h"

#define Console(fmt, ...) srslte::logmap::get("MAC ")->console(fmt, ##__VA_ARGS__)
#define Error(fmt, ...) srslte::logmap::get("MAC ")->error(fmt, ##__VA_ARGS__)
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

bool sched_cell_params_t::set_cfg(uint32_t                             enb_cc_idx_,
                                  const sched_interface::cell_cfg_t&   cfg_,
                                  const sched_interface::sched_args_t& sched_args)
{
  enb_cc_idx = enb_cc_idx_;
  cfg        = cfg_;
  sched_cfg  = &sched_args;

  // Basic cell config checks
  if (cfg.si_window_ms == 0) {
    Error("SCHED: Invalid si-window length 0 ms\n");
    return false;
  }

  // PRACH has to fit within the PUSCH space
  bool invalid_prach = cfg.cell.nof_prb == 6 and (cfg.prach_freq_offset + 6 > cfg.cell.nof_prb);
  invalid_prach |= cfg.cell.nof_prb > 6 and ((cfg.prach_freq_offset + 6) > (cfg.cell.nof_prb - cfg.nrb_pucch) or
                                             (int) cfg.prach_freq_offset < cfg.nrb_pucch);
  if (invalid_prach) {
    Error("Invalid PRACH configuration: frequency offset=%d outside bandwidth limits\n", cfg.prach_freq_offset);
    Console("Invalid PRACH configuration: frequency offset=%d outside bandwidth limits\n", cfg.prach_freq_offset);
    return false;
  }

  // Set derived sched parameters

  // init regs
  regs.reset(new srslte_regs_t{});
  if (srslte_regs_init(regs.get(), cfg.cell) != SRSLTE_SUCCESS) {
    Error("Getting DCI locations\n");
    return false;
  }

  // Compute Common locations for DCI for each CFI
  for (uint32_t cfi = 0; cfi < 3; cfi++) {
    sched_utils::generate_cce_location(regs.get(), &common_locations[cfi], cfi + 1);
  }
  if (common_locations[sched_cfg->nof_ctrl_symbols - 1].nof_loc[2] == 0) {
    Error("SCHED: Current cfi=%d is not valid for broadcast (check scheduler.nof_ctrl_symbols in conf file).\n",
          sched_cfg->nof_ctrl_symbols);
    Console("SCHED: Current cfi=%d is not valid for broadcast (check scheduler.nof_ctrl_symbols in conf file).\n",
            sched_cfg->nof_ctrl_symbols);
    return false;
  }

  // Compute UE locations for RA-RNTI
  for (uint32_t cfi = 0; cfi < 3; cfi++) {
    for (uint32_t sf_idx = 0; sf_idx < 10; sf_idx++) {
      sched_utils::generate_cce_location(regs.get(), &rar_locations[cfi][sf_idx], cfi + 1, sf_idx);
    }
  }

  // precompute nof cces in PDCCH for each CFI
  for (uint32_t cfix = 0; cfix < nof_cce_table.size(); ++cfix) {
    int ret = srslte_regs_pdcch_ncce(regs.get(), cfix + 1);
    if (ret < 0) {
      Error("SCHED: Failed to calculate the number of CCEs in the PDCCH\n");
      return false;
    }
    nof_cce_table[cfix] = (uint32_t)ret;
  }

  P        = srslte_ra_type0_P(cfg.cell.nof_prb);
  nof_rbgs = srslte::ceil_div(cfg.cell.nof_prb, P);

  return true;
}

/*******************************************************
 *
 * Initialization and sched configuration functions
 *
 *******************************************************/

sched::sched() : log_h(srslte::logmap::get("MAC")) {}

sched::~sched() {}

void sched::init(rrc_interface_mac* rrc_)
{
  rrc = rrc_;

  // Initialize first carrier scheduler
  carrier_schedulers.emplace_back(new carrier_sched{rrc, &ue_db, 0});

  reset();
}

int sched::reset()
{
  std::lock_guard<std::mutex> lock(sched_mutex);
  configured = false;
  for (std::unique_ptr<carrier_sched>& c : carrier_schedulers) {
    c->reset();
  }
  ue_db.clear();
  return 0;
}

void sched::set_sched_cfg(sched_interface::sched_args_t* sched_cfg_)
{
  std::lock_guard<std::mutex> lock(sched_mutex);
  if (sched_cfg_ != nullptr) {
    sched_cfg = *sched_cfg_;
  }
}

int sched::cell_cfg(const std::vector<sched_interface::cell_cfg_t>& cell_cfg)
{
  std::lock_guard<std::mutex> lock(sched_mutex);
  // Setup derived config params
  sched_cell_params.resize(cell_cfg.size());
  for (uint32_t cc_idx = 0; cc_idx < cell_cfg.size(); ++cc_idx) {
    if (not sched_cell_params[cc_idx].set_cfg(cc_idx, cell_cfg[cc_idx], sched_cfg)) {
      return SRSLTE_ERROR;
    }
  }

  // Create remaining cells, if not created yet
  uint32_t prev_size = carrier_schedulers.size();
  carrier_schedulers.resize(sched_cell_params.size());
  for (uint32_t i = prev_size; i < sched_cell_params.size(); ++i) {
    carrier_schedulers[i].reset(new carrier_sched{rrc, &ue_db, i});
  }

  // setup all carriers cfg params
  for (uint32_t i = 0; i < sched_cell_params.size(); ++i) {
    carrier_schedulers[i]->carrier_cfg(sched_cell_params[i]);
  }

  configured = true;

  return 0;
}

/*******************************************************
 *
 * FAPI-like main sched interface. Wrappers to UE object
 *
 *******************************************************/

int sched::ue_cfg(uint16_t rnti, const sched_interface::ue_cfg_t& ue_cfg)
{
  std::lock_guard<std::mutex> lock(sched_mutex);
  // Add or config user
  auto it = ue_db.find(rnti);
  if (it == ue_db.end()) {
    // create new user
    ue_db[rnti].init(rnti, sched_cell_params);
    it = ue_db.find(rnti);
  }
  it->second.set_cfg(ue_cfg);

  return SRSLTE_SUCCESS;
}

int sched::ue_rem(uint16_t rnti)
{
  std::lock_guard<std::mutex> lock(sched_mutex);
  if (ue_db.count(rnti) > 0) {
    ue_db.erase(rnti);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
}

bool sched::ue_exists(uint16_t rnti)
{
  return ue_db_access(rnti, [](sched_ue& ue) {}) >= 0;
}

void sched::ue_needs_ta_cmd(uint16_t rnti, uint32_t nof_ta_cmd)
{
  std::lock_guard<std::mutex> lock(sched_mutex);
  if (ue_db.count(rnti) > 0) {
    ue_db[rnti].set_needs_ta_cmd(nof_ta_cmd);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
}

void sched::phy_config_enabled(uint16_t rnti, bool enabled)
{
  // TODO: Check if correct use of last_tti
  ue_db_access(rnti, [this, enabled](sched_ue& ue) { ue.phy_config_enabled(last_tti, enabled); });
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
  // TODO: Check if correct use of last_tti
  uint32_t ret = 0;
  ue_db_access(rnti, [&ret](sched_ue& ue) { ret = ue.get_pending_dl_new_data(); });
  return ret;
}

uint32_t sched::get_ul_buffer(uint16_t rnti)
{
  // TODO: Check if correct use of last_tti
  uint32_t ret = 0;
  ue_db_access(rnti, [this, &ret](sched_ue& ue) { ret = ue.get_pending_ul_new_data(last_tti); });
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

int sched::dl_ack_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t tb_idx, bool ack)
{
  int ret = -1;
  ue_db_access(rnti, [tti, enb_cc_idx, tb_idx, ack, &ret](sched_ue& ue) {
    ret = ue.set_ack_info(tti, enb_cc_idx, tb_idx, ack);
  });
  return ret;
}

int sched::ul_crc_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, bool crc)
{
  return ue_db_access(rnti, [tti, enb_cc_idx, crc](sched_ue& ue) { ue.set_ul_crc(tti, enb_cc_idx, crc); });
}

int sched::dl_ri_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t ri_value)
{
  return ue_db_access(rnti, [tti, enb_cc_idx, ri_value](sched_ue& ue) { ue.set_dl_ri(tti, enb_cc_idx, ri_value); });
}

int sched::dl_pmi_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t pmi_value)
{
  return ue_db_access(rnti, [tti, enb_cc_idx, pmi_value](sched_ue& ue) { ue.set_dl_pmi(tti, enb_cc_idx, pmi_value); });
}

int sched::dl_cqi_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t cqi_value)
{
  return ue_db_access(rnti, [tti, enb_cc_idx, cqi_value](sched_ue& ue) { ue.set_dl_cqi(tti, enb_cc_idx, cqi_value); });
}

int sched::dl_rach_info(uint32_t enb_cc_idx, dl_sched_rar_info_t rar_info)
{
  std::lock_guard<std::mutex> lock(sched_mutex);
  return carrier_schedulers[enb_cc_idx]->dl_rach_info(rar_info);
}

int sched::ul_cqi_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t cqi, uint32_t ul_ch_code)
{
  return ue_db_access(
      rnti, [tti, enb_cc_idx, cqi, ul_ch_code](sched_ue& ue) { ue.set_ul_cqi(tti, enb_cc_idx, cqi, ul_ch_code); });
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
  std::lock_guard<std::mutex> lock(sched_mutex);
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

const sched::ue_cfg_t* sched::get_ue_cfg(uint16_t rnti)
{
  const ue_cfg_t* cfg = nullptr;
  ue_db_access(rnti, [&cfg](sched_ue& ue) { cfg = &ue.get_ue_cfg(); });
  return cfg;
}

/*******************************************************
 *
 * Main sched functions
 *
 *******************************************************/

// Downlink Scheduler API
int sched::dl_sched(uint32_t tti, uint32_t cc_idx, sched_interface::dl_sched_res_t& sched_result)
{
  if (!configured) {
    return 0;
  }

  std::lock_guard<std::mutex> lock(sched_mutex);
  uint32_t                    tti_rx = sched_utils::tti_subtract(tti, TX_DELAY_DL);
  last_tti                           = sched_utils::max_tti(last_tti, tti_rx);

  if (cc_idx < carrier_schedulers.size()) {
    // Compute scheduling Result for tti_rx
    const sf_sched_result& tti_sched = carrier_schedulers[cc_idx]->generate_tti_result(tti_rx);

    // copy result
    sched_result = tti_sched.dl_sched_result;
  }

  return 0;
}

// Uplink Scheduler API
int sched::ul_sched(uint32_t tti, uint32_t cc_idx, srsenb::sched_interface::ul_sched_res_t& sched_result)
{
  if (!configured) {
    return 0;
  }

  std::lock_guard<std::mutex> lock(sched_mutex);
  // Compute scheduling Result for tti_rx
  uint32_t tti_rx = sched_utils::tti_subtract(tti, TX_DELAY_DL + FDD_HARQ_DELAY_DL_MS);

  if (cc_idx < carrier_schedulers.size()) {
    const sf_sched_result& tti_sched = carrier_schedulers[cc_idx]->generate_tti_result(tti_rx);

    // copy result
    sched_result = tti_sched.ul_sched_result;
  }

  return SRSLTE_SUCCESS;
}

// Common way to access ue_db elements in a read locking way
template <typename Func>
int sched::ue_db_access(uint16_t rnti, Func f)
{
  std::lock_guard<std::mutex> lock(sched_mutex);
  auto                        it = ue_db.find(rnti);
  if (it != ue_db.end()) {
    f(it->second);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
}

/*******************************************************
 *
 * Helper functions and common data types
 *
 *******************************************************/

void sched_cell_params_t::regs_deleter::operator()(srslte_regs_t* p)
{
  if (p != nullptr) {
    srslte_regs_free(p);
    delete p;
  }
}

namespace sched_utils {

void generate_cce_location(srslte_regs_t*   regs_,
                           sched_dci_cce_t* location,
                           uint32_t         cfi,
                           uint32_t         sf_idx,
                           uint16_t         rnti)
{
  *location = {};

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

} // namespace sched_utils

} // namespace srsenb
