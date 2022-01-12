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

#ifndef SRSRAN_DUMMY_PHY_H
#define SRSRAN_DUMMY_PHY_H

#include "srsran/interfaces/phy_interface_types.h"
#include "srsue/hdr/phy/ue_phy_base.h"

namespace srsue {

class dummy_phy final : public ue_phy_base, public phy_interface_stack_lte
{
public:
  // ue_phy_base
  std::string get_type() final { return "dummy_phy"; }
  void        stop() final {}
  void        wait_initialize() final {}
  bool        is_initialized() final { return false; }
  void        start_plot() final {}
  void        get_metrics(const srsran::srsran_rat_t& rat, phy_metrics_t* m) final {}

  // phy_interface_stack_lte
  void prach_send(uint32_t preamble_idx, int allowed_subframe, float target_power_dbm, float ta_base_sec = 0.0f) final
  {}
  prach_info_t prach_get_info() final { return {}; }

  /* Indicates the transmission of a SR signal in the next opportunity */
  void sr_send() final {}
  int  sr_last_tx_tti() final { return 0; }

  void set_mch_period_stop(uint32_t stop) final {}
  bool set_config(const srsran::phy_cfg_t& config, uint32_t cc_idx = 0) final { return false; }
  bool set_scell(srsran_cell_t cell_info, uint32_t cc_idx, uint32_t earfcn) final { return false; }
  void set_config_tdd(srsran_tdd_config_t& tdd_config) final {}
  void set_config_mbsfn_sib2(srsran::mbsfn_sf_cfg_t* cfg_list, uint32_t nof_cfgs) final {}
  void set_config_mbsfn_sib13(const srsran::sib13_t& sib13) final {}
  void set_config_mbsfn_mcch(const srsran::mcch_msg_t& mcch) final {}

  void deactivate_scells() final {}

  /* Measurements interface */
  void set_cells_to_meas(uint32_t earfcn, const std::set<uint32_t>& pci) final {}
  void meas_stop() final {}

  /* Cell search and selection procedures */
  bool cell_search(int earfcn) final { return false; }
  bool cell_select(phy_cell_t cell) final { return false; }
  bool cell_is_camping() final { return false; }

  /* Time advance commands */
  void set_timeadv_rar(uint32_t tti, uint32_t ta_cmd) final {}
  void set_timeadv(uint32_t tti, uint32_t ta_cmd) final {}

  /* Activate / Disactivate SCell*/
  void set_activation_deactivation_scell(uint32_t cmd, uint32_t tti) final {}

  /* Sets RAR dci payload */
  void set_rar_grant(uint8_t grant_payload[SRSRAN_RAR_GRANT_LEN], uint16_t rnti) final {}

  uint32_t get_current_tti() final { return 0; }

  float get_phr() final { return 0.0; }
  float get_pathloss_db() final { return 0.0; }
};
} // namespace srsue
#endif // SRSRAN_DUMMY_PHY_H
