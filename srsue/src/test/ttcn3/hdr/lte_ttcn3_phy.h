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

#ifndef SRSUE_TTCN3_LTE_PHY_H
#define SRSUE_TTCN3_LTE_PHY_H

#include "srsran/common/task_scheduler.h"
#include "srsran/interfaces/ue_interfaces.h"
#include "srsran/interfaces/ue_phy_interfaces.h"
#include "srsue/hdr/phy/ue_phy_base.h"
#include "srsue/hdr/ue.h"
#include "ttcn3_interfaces.h"
#include <srsran/phy/phch/dci.h>

using namespace srsue;
using namespace srsran;

namespace srsue {

class lte_ttcn3_phy : public ue_phy_base, public phy_interface_stack_lte
{
public:
  void set_cells_to_meas(uint32_t earfcn, const std::set<uint32_t>& pci) override;
  typedef struct {
    srsran_cell_t info;
    float         power;
    uint32_t      earfcn;
  } cell_t;
  typedef std::vector<cell_t> cell_list_t;

  lte_ttcn3_phy();
  ~lte_ttcn3_phy() = default;

  int init(const phy_args_t& args_, stack_interface_phy_lte* stack_, syssim_interface_phy* syssim_);

  void        stop() override;
  void        wait_initialize() override;
  bool        is_initialized() override;
  void        start_plot() override;
  void        get_metrics(const srsran::srsran_rat_t& rat, phy_metrics_t* m) override;
  std::string get_type() override;

  // The interface for the SS
  void set_cell_map(const cell_list_t& cells_);

  // phy_interface_rrc_lte
  void deactivate_scells() override;
  void set_activation_deactivation_scell(uint32_t cmd, uint32_t tti) override;
  bool set_config(const srsran::phy_cfg_t& config, uint32_t cc_idx = 0) override;
  bool set_scell(srsran_cell_t cell_info, uint32_t cc_idx, uint32_t earfcn) override;
  void set_config_tdd(srsran_tdd_config_t& tdd_config) override;
  void set_config_mbsfn_sib2(srsran::mbsfn_sf_cfg_t* cfg_list, uint32_t nof_cfgs) override{};
  void set_config_mbsfn_sib13(const srsran::sib13_t& sib13) override{};
  void set_config_mbsfn_mcch(const srsran::mcch_msg_t& mcch) override{};

  // Measurements interface
  void meas_stop() override;

  // phy_interface_mac_lte
  void set_mch_period_stop(uint32_t stop) override{};

  // Cell search and selection procedures
  bool cell_search(int earfcn) override;
  bool cell_select(phy_cell_t cell) override;
  bool cell_is_camping() override;

  // phy_interface_mac_lte
  void prach_send(uint32_t preamble_idx, int allowed_subframe, float target_power_dbm, float ta_base_sec) override;
  prach_info_t prach_get_info() override;
  void         sr_send() override;
  int          sr_last_tx_tti() override;

  // phy_interface_mac_common
  void     set_timeadv_rar(uint32_t tti, uint32_t ta_cmd) override;
  void     set_timeadv(uint32_t tti, uint32_t ta_cmd) override;
  void     set_rar_grant(uint8_t grant_payload[SRSRAN_RAR_GRANT_LEN], uint16_t rnti) override;
  uint32_t get_current_tti() override;
  float    get_phr() override;
  float    get_pathloss_db() override;

  // phy_interface_syssim
  void set_current_tti(uint32_t tti);

  void new_grant_ul(mac_interface_phy_lte::mac_grant_ul_t ul_mac_grant);
  void new_tb(const srsue::mac_interface_phy_lte::mac_grant_dl_t, const uint8_t* data);

  void run_tti();

private:
  srslog::basic_logger& logger;

  // All available cells
  cell_list_t cells;
  uint32_t    cell_idx = 0;

  // The current PCell
  cell_t pcell     = {};
  bool   pcell_set = false;

  srsran::phy_cfg_t phy_cfg = {};

  uint32_t current_tti = 0;
  uint32_t cc_idx      = 0;

  int prach_tti_tx = -1;

  std::atomic<int32_t> sr_tx_tti  = {-1};
  std::atomic<bool>    sr_pending = {false};

  std::mutex phy_mutex;

  srsran::task_scheduler task_sched;

  uint32_t ra_trans_cnt = 0;

  stack_interface_phy_lte* stack  = nullptr;
  syssim_interface_phy*    syssim = nullptr;
};

} // namespace srsue

#endif
