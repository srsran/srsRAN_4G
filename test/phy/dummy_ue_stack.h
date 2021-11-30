/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

  struct metrics_t {
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

  dummy_tx_harq_entity tx_harq_proc;
  dummy_rx_harq_entity rx_harq_proc;

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

  virtual void wait_tti()
  {
    // Do nothing
  }
  void in_sync() override {}
  void out_of_sync() override {}
  void run_tti(const uint32_t tti) override
  {
    wait_tti();

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
  int          sf_indication(const uint32_t tti) override { return 0; }
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

  metrics_t get_metrics() { return metrics; }

  void set_phy_config_complete(bool status) override {}

  void cell_search_found_cell(const cell_search_result_t& result) override
  {
    if (result.cell_found) {
      // Unpack MIB with ASN1
      asn1::rrc_nr::mib_s mib_asn1;
      asn1::cbit_ref      cbit(result.pbch_msg.payload, SRSRAN_PBCH_MSG_NR_SZ);
      mib_asn1.unpack(cbit);

      // Convert MIB to JSON
      asn1::json_writer json;
      mib_asn1.to_json(json);

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
    } else {
      logger.info("Cell not found\n");
    }
  }
};

#endif // SRSRAN_DUMMY_UE_STACK_H
