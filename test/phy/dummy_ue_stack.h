/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSRAN_DUMMY_UE_STACK_H
#define SRSRAN_DUMMY_UE_STACK_H

#include "dummy_rx_harq_proc.h"
#include "dummy_tx_harq_proc.h"
#include "srsran/asn1/rrc_nr.h"
#include "srsran/interfaces/ue_nr_interfaces.h"

class ue_dummy_stack : public srsue::stack_interface_phy_nr
{
public:
  struct prach_metrics_t {
    uint32_t count;
  };

  struct cell_search_metrics_t {
    // Last cell search result for the PCI and SSB candidate
    srsue::stack_interface_phy_nr::cell_search_result_t last_result;

    // Signal Measurements
    float    epre_db_avg = 0.0f;
    float    epre_db_min = +INFINITY;
    float    epre_db_max = -INFINITY;
    float    rsrp_db_avg = 0.0f;
    float    rsrp_db_min = +INFINITY;
    float    rsrp_db_max = -INFINITY;
    float    snr_db_avg  = 0.0f;
    float    snr_db_min  = +INFINITY;
    float    snr_db_max  = -INFINITY;
    float    cfo_hz_avg  = 0.0f;
    float    cfo_hz_min  = +INFINITY;
    float    cfo_hz_max  = -INFINITY;
    uint32_t count       = 0;
  };

  struct metrics_t {
    std::map<uint32_t, std::map<uint32_t, cell_search_metrics_t> > cell_search;
    std::map<uint32_t, prach_metrics_t> prach    = {}; ///< PRACH metrics indexed with premable index
    uint32_t                            sr_count = 0;  ///< Counts number of transmitted SR
  };

private:
  srslog::basic_logger&          logger = srslog::fetch_basic_logger("UE-STCK");
  std::mutex                     rnti_mutex;
  srsran_random_t                random_gen     = srsran_random_init(0x1323);
  srsran_rnti_type_t             dl_rnti_type   = srsran_rnti_type_c;
  uint16_t                       rnti           = 0;
  bool                           valid          = false;
  uint32_t                       sr_period      = 0;
  uint32_t                       sr_count       = 0;
  uint32_t                       prach_period   = 0;
  uint32_t                       prach_preamble = 0;
  bool                           prach_pending  = false;
  metrics_t                      metrics        = {};
  srsue::phy_interface_stack_nr& phy;

  // Atributes to flag configuration PHy complete
  bool                    configuration_complete = false;
  std::mutex              configuration_complete_mutex;
  std::condition_variable configuration_complete_cvar;

  // Attributes for throttling PHY and avoiding PHY free-running
  bool                    pending_tti = false;
  std::mutex              pending_tti_mutex;
  std::condition_variable pending_tti_cvar;
  std::atomic<bool>       running = {true};

  dummy_tx_harq_entity tx_harq_proc;
  dummy_rx_harq_entity rx_harq_proc;

  std::atomic<bool>    cell_search_finished = {false};
  std::atomic<bool>    cell_select_finished = {false};
  cell_select_result_t cell_select_result   = {};

public:
  struct args_t {
    uint16_t    rnti         = 0x1234; ///< C-RNTI for PUSCH and PDSCH transmissions
    uint32_t    sr_period    = 0; ///< Indicates positive SR period in number of opportunities. Set to 0 to disable.
    uint32_t    prach_period = 0; ///< Requests PHY to transmit PRACH periodically in frames. Set to 0 to disable.
    std::string log_level    = "warning";
  };
  ue_dummy_stack(const args_t& args, srsue::phy_interface_stack_nr& phy_) :
    rnti(args.rnti), sr_period(args.sr_period), prach_period(args.prach_period), phy(phy_)
  {
    logger.set_level(srslog::str_to_basic_level(args.log_level));
    valid = true;
  }
  ~ue_dummy_stack() { srsran_random_free(random_gen); }

  void in_sync() override {}
  void out_of_sync() override {}
  void run_tti(const uint32_t tti, const uint32_t tti_jump) override
  {
    // Wait for tick from test bench
    std::unique_lock<std::mutex> lock(pending_tti_mutex);
    while (not pending_tti and running) {
      pending_tti_cvar.wait_for(lock, std::chrono::milliseconds(1));
    }

    // Let the tick proceed
    pending_tti = false;
    pending_tti_cvar.notify_all();

    // Run PRACH
    if (prach_period != 0) {
      uint32_t slot_idx = tti % SRSRAN_NSLOTS_PER_FRAME_NR(srsran_subcarrier_spacing_15kHz);
      uint32_t sfn      = tti / SRSRAN_NSLOTS_PER_FRAME_NR(srsran_subcarrier_spacing_15kHz);
      if (not prach_pending and slot_idx == 0 and sfn % prach_period == 0) {
        prach_preamble = srsran_random_uniform_int_dist(random_gen, 0, 63);
        phy.send_prach(0, prach_preamble, 0.0f, 0.0f);
        prach_pending = true;
      }
    }
  }
  void tick()
  {
    // Wait for TTI to get processed
    std::unique_lock<std::mutex> lock(pending_tti_mutex);
    while (pending_tti and running) {
      pending_tti_cvar.wait_for(lock, std::chrono::milliseconds(1));
    }

    // Let the TTI proceed
    pending_tti = true;
    pending_tti_cvar.notify_all();
  }

  void stop()
  {
    running = false;
    pending_tti_cvar.notify_all();
  }
  sched_rnti_t get_dl_sched_rnti_nr(const uint32_t tti) override
  {
    std::unique_lock<std::mutex> lock(rnti_mutex);
    return {rnti, dl_rnti_type};
  }
  sched_rnti_t get_ul_sched_rnti_nr(const uint32_t tti) override
  {
    std::unique_lock<std::mutex> lock(rnti_mutex);
    return {rnti, srsran_rnti_type_c};
  }
  void new_grant_dl(const uint32_t cc_idx, const mac_nr_grant_dl_t& grant, tb_action_dl_t* action) override
  {
    action->tb.enabled    = true;
    action->tb.softbuffer = &rx_harq_proc[grant.pid].get_softbuffer(grant.ndi, grant.tbs);
  }
  void tb_decoded(const uint32_t cc_idx, const mac_nr_grant_dl_t& grant, tb_action_dl_result_t result) override {}
  void new_grant_ul(const uint32_t cc_idx, const mac_nr_grant_ul_t& grant, tb_action_ul_t* action) override
  {
    if (action == nullptr) {
      return;
    }
    action->tb.enabled    = true;
    action->tb.payload    = tx_harq_proc[grant.pid].get_tb(grant.tbs);
    action->tb.softbuffer = &tx_harq_proc[grant.pid].get_softbuffer(grant.ndi);
  }
  void prach_sent(uint32_t tti, uint32_t s_id, uint32_t t_id, uint32_t f_id, uint32_t ul_carrier_id) override
  {
    std::unique_lock<std::mutex> lock(rnti_mutex);
    dl_rnti_type = srsran_rnti_type_ra;
    rnti         = 1 + s_id + 14 * t_id + 14 * 80 * f_id + 14 * 80 * 8 * ul_carrier_id;
    metrics.prach[prach_preamble].count++;
    prach_pending = false;
  }
  bool sr_opportunity(uint32_t tti, uint32_t sr_id, bool meas_gap, bool ul_sch_tx) override
  {
    if (sr_period == 0) {
      return false;
    }

    if (sr_count >= (sr_period - 1) and not ul_sch_tx) {
      metrics.sr_count++;
      sr_count = 0;
      return true;
    }

    sr_count++;
    return false;
  }
  bool is_valid() const { return valid; }

  const metrics_t& get_metrics() const { return metrics; }
  void             reset_metrics()
  {
    metrics.cell_search.clear();
    metrics.prach.clear();
    metrics.sr_count = 0;
  }

  void set_phy_config_complete(bool status) override
  {
    std::unique_lock<std::mutex> lock(configuration_complete_mutex);
    configuration_complete = true;
    configuration_complete_cvar.notify_all();
  }

  void wait_phy_config_complete()
  {
    std::unique_lock<std::mutex> lock(configuration_complete_mutex);
    while (not configuration_complete) {
      configuration_complete_cvar.wait(lock);
    }
    configuration_complete = false;
  }

  bool get_cell_search_finished()
  {
    bool ret = cell_search_finished;

    cell_search_finished = false;

    return ret;
  }

  void cell_search_found_cell(const cell_search_result_t& result) override
  {
    if (not result.cell_found) {
      logger.info("Cell search finished without detecting any cell");

      // Flag as cell search is done
      cell_search_finished = true;

      return;
    }

    // Pack PBCH message bits
    std::array<uint8_t, SRSRAN_PBCH_MSG_NR_SZ> bit_pack_pbch_msg = {};
    asn1::cbit_ref                             cbit(bit_pack_pbch_msg.data(), bit_pack_pbch_msg.size());
    srsran_bit_pack_vector((uint8_t*)result.pbch_msg.payload, bit_pack_pbch_msg.data(), SRSRAN_PBCH_MSG_NR_SZ);

    // Unpack MIB with ASN1
    asn1::rrc_nr::bcch_bch_msg_s bcch;
    bcch.unpack(cbit);

    // Convert MIB to JSON
    asn1::json_writer json;
    bcch.to_json(json);

    // Unpack MIB with C lib
    srsran_mib_nr_t mib_c = {};
    srsran_pbch_msg_nr_mib_unpack(&result.pbch_msg, &mib_c);

    // Convert MIB from C lib to info
    std::array<char, 512> mib_info = {};
    srsran_pbch_msg_nr_mib_info(&mib_c, mib_info.data(), (uint32_t)mib_info.size());

    // Convert CSI to string
    std::array<char, 512> csi_info = {};
    srsran_csi_meas_info_short(&result.measurements, csi_info.data(), (uint32_t)csi_info.size());

    logger.info(
        "Cell found pci=%d %s %s ASN1: %s", result.pci, mib_info.data(), csi_info.data(), json.to_string().c_str());

    cell_search_metrics_t& m = metrics.cell_search[result.pci][result.pbch_msg.ssb_idx];
    m.last_result            = result;
    m.epre_db_min            = SRSRAN_MIN(m.epre_db_min, result.measurements.epre_dB);
    m.epre_db_max            = SRSRAN_MAX(m.epre_db_max, result.measurements.epre_dB);
    m.epre_db_avg            = SRSRAN_VEC_SAFE_CMA(result.measurements.epre_dB, m.epre_db_avg, m.count);
    m.rsrp_db_min            = SRSRAN_MIN(m.rsrp_db_min, result.measurements.rsrp_dB);
    m.rsrp_db_max            = SRSRAN_MAX(m.rsrp_db_max, result.measurements.rsrp_dB);
    m.rsrp_db_avg            = SRSRAN_VEC_SAFE_CMA(result.measurements.rsrp_dB, m.rsrp_db_avg, m.count);
    m.snr_db_min             = SRSRAN_MIN(m.snr_db_min, result.measurements.snr_dB);
    m.snr_db_max             = SRSRAN_MAX(m.snr_db_max, result.measurements.snr_dB);
    m.snr_db_avg             = SRSRAN_VEC_SAFE_CMA(result.measurements.snr_dB, m.snr_db_avg, m.count);
    m.cfo_hz_min             = SRSRAN_MIN(m.cfo_hz_min, result.measurements.cfo_hz);
    m.cfo_hz_max             = SRSRAN_MAX(m.cfo_hz_max, result.measurements.cfo_hz);
    m.cfo_hz_avg             = SRSRAN_VEC_SAFE_CMA(result.measurements.cfo_hz, m.cfo_hz_avg, m.count);
    m.count++;

    // Flag as cell search is done
    cell_search_finished = true;
  }

  bool get_cell_select_finished()
  {
    bool ret = cell_select_finished;

    cell_select_finished = false;

    return ret;
  }

  cell_select_result_t get_cell_select_result() { return cell_select_result; }

  void cell_select_completed(const cell_select_result_t& result) override
  {
    cell_select_result   = result;
    cell_select_finished = true;
  }
};

#endif // SRSRAN_DUMMY_UE_STACK_H
