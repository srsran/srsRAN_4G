/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include <srsenb/hdr/stack/mac/sched_ue.h>
#include <string.h>

#include "srsenb/hdr/stack/mac/sched.h"
#include "srsenb/hdr/stack/mac/sched_carrier.h"
#include "srsenb/hdr/stack/mac/sched_helpers.h"
#include "srsran/srslog/srslog.h"

#define Console(fmt, ...) srsran::console(fmt, ##__VA_ARGS__)
#define Error(fmt, ...) srslog::fetch_basic_logger("MAC").error(fmt, ##__VA_ARGS__)

using srsran::tti_point;

namespace srsenb {

/*******************************************************
 *
 * Initialization and sched configuration functions
 *
 *******************************************************/

sched::sched() {}

sched::~sched() {}

void sched::init(rrc_interface_mac* rrc_, const sched_args_t& sched_cfg_)
{
  rrc       = rrc_;
  sched_cfg = sched_cfg_;

  // Initialize first carrier scheduler
  carrier_schedulers.emplace_back(new carrier_sched{rrc, &ue_db, 0, &sched_results});

  reset();
}

int sched::reset()
{
  std::lock_guard<std::mutex> lock(sched_mutex);
  for (std::unique_ptr<carrier_sched>& c : carrier_schedulers) {
    c->reset();
  }
  ue_db.clear();
  return 0;
}

/// Called by rrc::init
int sched::cell_cfg(const std::vector<sched_interface::cell_cfg_t>& cell_cfg)
{
  std::lock_guard<std::mutex> lock(sched_mutex);
  // Setup derived config params
  sched_cell_params.resize(cell_cfg.size());
  for (uint32_t cc_idx = 0; cc_idx < cell_cfg.size(); ++cc_idx) {
    if (not sched_cell_params[cc_idx].set_cfg(cc_idx, cell_cfg[cc_idx], sched_cfg)) {
      return SRSRAN_ERROR;
    }
  }

  sched_results.set_nof_carriers(cell_cfg.size());

  // Create remaining cells, if not created yet
  uint32_t prev_size = carrier_schedulers.size();
  carrier_schedulers.resize(sched_cell_params.size());
  for (uint32_t i = prev_size; i < sched_cell_params.size(); ++i) {
    carrier_schedulers[i].reset(new carrier_sched{rrc, &ue_db, i, &sched_results});
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
  {
    // config existing user
    std::lock_guard<std::mutex> lock(sched_mutex);
    auto                        it = ue_db.find(rnti);
    if (it != ue_db.end()) {
      it->second->set_cfg(ue_cfg);
      return SRSRAN_SUCCESS;
    }
  }

  // Add new user case
  std::unique_ptr<sched_ue>   ue{new sched_ue(rnti, sched_cell_params, ue_cfg)};
  std::lock_guard<std::mutex> lock(sched_mutex);
  ue_db.insert(rnti, std::move(ue));
  return SRSRAN_SUCCESS;
}

int sched::ue_rem(uint16_t rnti)
{
  std::lock_guard<std::mutex> lock(sched_mutex);
  if (ue_db.contains(rnti)) {
    ue_db.erase(rnti);
  } else {
    Error("User rnti=0x%x not found", rnti);
    return SRSRAN_ERROR;
  }
  return SRSRAN_SUCCESS;
}

bool sched::ue_exists(uint16_t rnti)
{
  return ue_db_access_locked(
             rnti, [](sched_ue& ue) {}, nullptr, false) >= 0;
}

void sched::phy_config_enabled(uint16_t rnti, bool enabled)
{
  // TODO: Check if correct use of last_tti
  ue_db_access_locked(
      rnti, [this, enabled](sched_ue& ue) { ue.phy_config_enabled(last_tti, enabled); }, __PRETTY_FUNCTION__);
}

int sched::bearer_ue_cfg(uint16_t rnti, uint32_t lc_id, const mac_lc_ch_cfg_t& cfg_)
{
  return ue_db_access_locked(rnti, [lc_id, cfg_](sched_ue& ue) { ue.set_bearer_cfg(lc_id, cfg_); });
}

int sched::bearer_ue_rem(uint16_t rnti, uint32_t lc_id)
{
  return ue_db_access_locked(rnti, [lc_id](sched_ue& ue) { ue.rem_bearer(lc_id); });
}

uint32_t sched::get_dl_buffer(uint16_t rnti)
{
  uint32_t ret = SRSRAN_ERROR;
  ue_db_access_locked(
      rnti, [&ret](sched_ue& ue) { ret = ue.get_pending_dl_rlc_data(); }, __PRETTY_FUNCTION__);
  return ret;
}

uint32_t sched::get_ul_buffer(uint16_t rnti)
{
  // TODO: Check if correct use of last_tti
  uint32_t ret = SRSRAN_ERROR;
  ue_db_access_locked(
      rnti,
      [this, &ret](sched_ue& ue) { ret = ue.get_pending_ul_new_data(to_tx_ul(last_tti), -1); },
      __PRETTY_FUNCTION__);
  return ret;
}

int sched::dl_rlc_buffer_state(uint16_t rnti, uint32_t lc_id, uint32_t tx_queue, uint32_t prio_tx_queue)
{
  return ue_db_access_locked(rnti, [&](sched_ue& ue) { ue.dl_buffer_state(lc_id, tx_queue, prio_tx_queue); });
}

int sched::dl_mac_buffer_state(uint16_t rnti, uint32_t ce_code, uint32_t nof_cmds)
{
  return ue_db_access_locked(rnti, [ce_code, nof_cmds](sched_ue& ue) { ue.mac_buffer_state(ce_code, nof_cmds); });
}

int sched::dl_ack_info(uint32_t tti_rx, uint16_t rnti, uint32_t enb_cc_idx, uint32_t tb_idx, bool ack)
{
  int ret = -1;
  ue_db_access_locked(
      rnti,
      [&](sched_ue& ue) { ret = ue.set_ack_info(tti_point{tti_rx}, enb_cc_idx, tb_idx, ack); },
      __PRETTY_FUNCTION__);
  return ret;
}

int sched::ul_crc_info(uint32_t tti_rx, uint16_t rnti, uint32_t enb_cc_idx, bool crc)
{
  return ue_db_access_locked(
      rnti, [tti_rx, enb_cc_idx, crc](sched_ue& ue) { ue.set_ul_crc(tti_point{tti_rx}, enb_cc_idx, crc); });
}

int sched::dl_ri_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t ri_value)
{
  return ue_db_access_locked(
      rnti, [tti, enb_cc_idx, ri_value](sched_ue& ue) { ue.set_dl_ri(tti_point{tti}, enb_cc_idx, ri_value); });
}

int sched::dl_pmi_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t pmi_value)
{
  return ue_db_access_locked(
      rnti, [tti, enb_cc_idx, pmi_value](sched_ue& ue) { ue.set_dl_pmi(tti_point{tti}, enb_cc_idx, pmi_value); });
}

int sched::dl_cqi_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t cqi_value)
{
  return ue_db_access_locked(
      rnti, [tti, enb_cc_idx, cqi_value](sched_ue& ue) { ue.set_dl_cqi(tti_point{tti}, enb_cc_idx, cqi_value); });
}

int sched::dl_sb_cqi_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t sb_idx, uint32_t cqi_value)
{
  return ue_db_access_locked(rnti, [tti, enb_cc_idx, cqi_value, sb_idx](sched_ue& ue) {
    ue.set_dl_sb_cqi(tti_point{tti}, enb_cc_idx, sb_idx, cqi_value);
  });
}

int sched::dl_rach_info(uint32_t enb_cc_idx, dl_sched_rar_info_t rar_info)
{
  std::lock_guard<std::mutex> lock(sched_mutex);
  return carrier_schedulers[enb_cc_idx]->dl_rach_info(rar_info);
}

int sched::ul_snr_info(uint32_t tti_rx, uint16_t rnti, uint32_t enb_cc_idx, float snr, uint32_t ul_ch_code)
{
  return ue_db_access_locked(rnti,
                             [&](sched_ue& ue) { ue.set_ul_snr(tti_point{tti_rx}, enb_cc_idx, snr, ul_ch_code); });
}

int sched::ul_bsr(uint16_t rnti, uint32_t lcg_id, uint32_t bsr)
{
  return ue_db_access_locked(rnti, [lcg_id, bsr](sched_ue& ue) { ue.ul_buffer_state(lcg_id, bsr); });
}

int sched::ul_buffer_add(uint16_t rnti, uint32_t lcid, uint32_t bytes)
{
  return ue_db_access_locked(rnti, [lcid, bytes](sched_ue& ue) { ue.ul_buffer_add(lcid, bytes); });
}

int sched::ul_phr(uint16_t rnti, int phr, uint32_t ul_nof_prb)
{
  return ue_db_access_locked(
      rnti, [phr, ul_nof_prb](sched_ue& ue) { ue.ul_phr(phr, ul_nof_prb); }, __PRETTY_FUNCTION__);
}

int sched::ul_sr_info(uint32_t tti, uint16_t rnti)
{
  return ue_db_access_locked(
      rnti, [](sched_ue& ue) { ue.set_sr(); }, __PRETTY_FUNCTION__);
}

void sched::set_dl_tti_mask(uint8_t* tti_mask, uint32_t nof_sfs)
{
  std::lock_guard<std::mutex> lock(sched_mutex);
  carrier_schedulers[0]->set_dl_tti_mask(tti_mask, nof_sfs);
}

std::array<int, SRSRAN_MAX_CARRIERS> sched::get_enb_ue_cc_map(uint16_t rnti)
{
  std::array<int, SRSRAN_MAX_CARRIERS> ret{};
  ret.fill(-1); // -1 for inactive & non-existent carriers
  ue_db_access_locked(
      rnti,
      [this, &ret](sched_ue& ue) {
        for (size_t enb_cc_idx = 0; enb_cc_idx < carrier_schedulers.size(); ++enb_cc_idx) {
          const sched_ue_cell* cc_ue = ue.find_ue_carrier(enb_cc_idx);
          if (cc_ue != nullptr) {
            ret[enb_cc_idx] = cc_ue->get_ue_cc_idx();
          }
        }
      },
      __PRETTY_FUNCTION__);
  return ret;
}

std::array<int, SRSRAN_MAX_CARRIERS> sched::get_enb_ue_activ_cc_map(uint16_t rnti)
{
  std::array<int, SRSRAN_MAX_CARRIERS> ret{};
  ret.fill(-1); // -1 for inactive & non-existent carriers
  ue_db_access_locked(
      rnti,
      [this, &ret](sched_ue& ue) {
        for (size_t enb_cc_idx = 0; enb_cc_idx < carrier_schedulers.size(); ++enb_cc_idx) {
          const sched_ue_cell* cc_ue = ue.find_ue_carrier(enb_cc_idx);
          if (cc_ue != nullptr and (cc_ue->cc_state() == cc_st::active or cc_ue->cc_state() == cc_st::activating)) {
            ret[enb_cc_idx] = cc_ue->get_ue_cc_idx();
          }
        }
      },
      __PRETTY_FUNCTION__);
  return ret;
}

int sched::set_pdcch_order(uint32_t enb_cc_idx, dl_sched_po_info_t pdcch_order_info)
{
  std::lock_guard<std::mutex> lock(sched_mutex);
  return carrier_schedulers[enb_cc_idx]->pdcch_order_info(pdcch_order_info);
}

/*******************************************************
 *
 * Main sched functions
 *
 *******************************************************/

// Downlink Scheduler API
int sched::dl_sched(uint32_t tti_tx_dl, uint32_t enb_cc_idx, sched_interface::dl_sched_res_t& sched_result)
{
  std::lock_guard<std::mutex> lock(sched_mutex);
  if (not configured) {
    return 0;
  }
  if (enb_cc_idx >= carrier_schedulers.size()) {
    return 0;
  }

  tti_point tti_rx = tti_point{tti_tx_dl} - TX_ENB_DELAY;
  new_tti(tti_rx);

  // copy result
  sched_result = sched_results.get_sf(tti_rx)->get_cc(enb_cc_idx)->dl_sched_result;

  return 0;
}

// Uplink Scheduler API
int sched::ul_sched(uint32_t tti, uint32_t enb_cc_idx, srsenb::sched_interface::ul_sched_res_t& sched_result)
{
  std::lock_guard<std::mutex> lock(sched_mutex);
  if (not configured) {
    return 0;
  }
  if (enb_cc_idx >= carrier_schedulers.size()) {
    return 0;
  }

  // Compute scheduling Result for tti_rx
  tti_point tti_rx = tti_point{tti} - TX_ENB_DELAY - FDD_HARQ_DELAY_DL_MS;
  new_tti(tti_rx);

  // copy result
  sched_result = sched_results.get_sf(tti_rx)->get_cc(enb_cc_idx)->ul_sched_result;

  return SRSRAN_SUCCESS;
}

/// Generate scheduling decision for tti_rx, if it wasn't already generated
/// NOTE: The scheduling decision is made for all CCs in a single call/lock, otherwise the UE can have different
///       configurations (e.g. different set of activated SCells) in different CC decisions
void sched::new_tti(tti_point tti_rx)
{
  last_tti = std::max(last_tti, tti_rx);

  // Generate sched results for all CCs, if not yet generated
  for (size_t cc_idx = 0; cc_idx < carrier_schedulers.size(); ++cc_idx) {
    if (not is_generated(tti_rx, cc_idx)) {
      // Generate carrier scheduling result
      carrier_schedulers[cc_idx]->generate_tti_result(tti_rx);
    }
  }
}

/// Check if TTI result is generated
bool sched::is_generated(srsran::tti_point tti_rx, uint32_t enb_cc_idx) const
{
  return sched_results.has_sf(tti_rx) and sched_results.get_sf(tti_rx)->is_generated(enb_cc_idx);
}

int sched::metrics_read(uint16_t rnti, mac_ue_metrics_t& metrics)
{
  return ue_db_access_locked(
      rnti, [&metrics](sched_ue& ue) { ue.metrics_read(metrics); }, "metrics_read");
}

// Common way to access ue_db elements in a read locking way
template <typename Func>
int sched::ue_db_access_locked(uint16_t rnti, Func&& f, const char* func_name, bool log_fail)
{
  std::lock_guard<std::mutex> lock(sched_mutex);
  auto                        it = ue_db.find(rnti);
  if (it != ue_db.end()) {
    f(*it->second);
  } else {
    if (log_fail) {
      if (func_name != nullptr) {
        Error("SCHED: User rnti=0x%x not found. Failed to call %s.", rnti, func_name);
      } else {
        Error("SCHED: User rnti=0x%x not found.", rnti);
      }
    }
    return SRSRAN_ERROR;
  }
  return SRSRAN_SUCCESS;
}

} // namespace srsenb
