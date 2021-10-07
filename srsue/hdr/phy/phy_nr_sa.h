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

#ifndef SRSUE_PHY_NR_SA_H
#define SRSUE_PHY_NR_SA_H

#include "srsran/interfaces/ue_nr_interfaces.h"
#include "srsue/hdr/phy/nr/sync_sa.h"

namespace srsue {

/**
 * @brief NR Standalone PHY
 */
class phy_nr_sa : public phy_interface_stack_sa_nr
{
public:
  struct args_t {
    std::string log_level = "info";  ///< General PHY logging level
    double      srate_hz  = 61.44e6; ///< Sampling rate in Hz

    // Frequency allocation parameters
    uint32_t pointA_arfcn = 0; ///< Resource grid PointA ARFCN
    float    pointA_Hz    = 0; ///< Resource grid PointA frequency in Hz. Overrides pointA_arfcn if valid
    uint32_t ssb_arfcn    = 0; ///< SS/PBCH block center point ARFCN
    float    ssb_Hz       = 0; ///< SS/PBCH block center point ARFCN. Overrides ssb_arfcn if valid
  };

  phy_nr_sa(stack_interface_phy_sa_nr& stack_, srsran::radio_interface_phy& radio_);

  bool init(const args_t& args);

  int set_ul_grant(uint32_t                                       rar_slot_idx,
                   std::array<uint8_t, SRSRAN_RAR_UL_GRANT_NBITS> packed_ul_grant,
                   uint16_t                                       rnti,
                   srsran_rnti_type_t                             rnti_type) override
  {
    return 0;
  }
  int  tx_request(const tx_request_t& request) override { return 0; }
  void send_prach(const uint32_t prach_occasion,
                  const int      preamble_index,
                  const float    preamble_received_target_power,
                  const float    ta_base_sec) override
  {}
  bool has_valid_sr_resource(uint32_t sr_id) override { return false; }
  void clear_pending_grants() override {}
  bool set_config(const srsran::phy_cfg_nr_t& cfg) override { return false; }

  phy_nr_sa_state_t get_state() const override { return PHY_NR_SA_STATE_CELL_SELECT; }
  void              reset() override {}
  bool              start_cell_search(const cell_search_args_t& req) override { return false; }
  bool              start_cell_select(const cell_search_args_t& req) override { return false; }

private:
  nr::worker_pool       workers;
  nr::sync_sa           sync;
  srslog::basic_logger& logger;
};

} // namespace srsue
#endif // SRSUE_PHY_NR_SA_H
