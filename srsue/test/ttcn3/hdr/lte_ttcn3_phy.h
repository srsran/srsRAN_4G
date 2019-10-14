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

#ifndef SRSUE_TTCN3_LTE_PHY_H
#define SRSUE_TTCN3_LTE_PHY_H

#include "srsue/hdr/phy/ue_lte_phy_base.h"
#include "srsue/hdr/ue.h"
#include "ttcn3_interfaces.h"
#include <srslte/interfaces/ue_interfaces.h>
#include <srslte/phy/phch/dci.h>

using namespace srsue;
using namespace srslte;

namespace srsue {

class lte_ttcn3_phy : public ue_lte_phy_base
{
public:
  typedef struct {
    srslte_cell_t info;
    float         power;
    uint32_t      earfcn;
  } cell_t;
  typedef std::vector<cell_t> cell_list_t;

  lte_ttcn3_phy(srslte::logger* logger_);
  ~lte_ttcn3_phy();

  int init(const phy_args_t& args_, stack_interface_phy_lte* stack_, syssim_interface_phy* syssim_);

  int init(const phy_args_t& args_, stack_interface_phy_lte* stack_, radio_interface_phy* radio_);

  // ue_phy_base interface
  int         init(const phy_args_t& args_);
  void        stop();
  void        set_earfcn(std::vector<uint32_t> earfcns);
  void        force_freq(float dl_freq, float ul_freq);
  void        wait_initialize();
  void        start_plot();
  void        get_metrics(phy_metrics_t* m);
  std::string get_type();

  // The interface for the SS
  void set_cell_map(const cell_list_t& cells_);

  // phy_interface_rrc_lte
  void     get_current_cell(srslte_cell_t* cell_, uint32_t* earfcn_ = NULL);
  uint32_t get_current_earfcn();
  uint32_t get_current_pci();
  void     set_config_scell(asn1::rrc::scell_to_add_mod_r10_s* scell_config);
  void     enable_pregen_signals(bool enable);
  void     set_activation_deactivation_scell(uint32_t cmd);
  void
       set_config(srslte::phy_cfg_t& config, uint32_t cc_idx = 0, uint32_t earfcn = 0, srslte_cell_t* cell_info = nullptr);
  void set_config_tdd(srslte_tdd_config_t& tdd_config);
  void set_config_mbsfn_sib2(srslte::mbsfn_sf_cfg_t* cfg_list, uint32_t nof_cfgs){};
  void set_config_mbsfn_sib13(const srslte::sib13_t& sib13){};
  void set_config_mbsfn_mcch(const srslte::mcch_msg_t& mcch){};

  // Measurements interface
  void meas_reset();
  int  meas_start(uint32_t earfcn, int pci = -1);
  int  meas_stop(uint32_t earfcn, int pci = -1);

  // phy_interface_mac_lte
  void set_mch_period_stop(uint32_t stop){};

  // Cell search and selection procedures
  cell_search_ret_t cell_search(phy_cell_t* found_cell);
  bool              cell_select(phy_cell_t* cell);
  bool              cell_is_camping();
  void              reset();

  // phy_interface_mac_lte
  void         configure_prach_params();
  void         prach_send(uint32_t preamble_idx, int allowed_subframe, float target_power_dbm);
  prach_info_t prach_get_info();
  void         sr_send();
  int          sr_last_tx_tti();

  // phy_interface_mac_common
  void     set_crnti(uint16_t rnti);
  void     set_timeadv_rar(uint32_t ta_cmd);
  void     set_timeadv(uint32_t ta_cmd);
  void     set_rar_grant(uint8_t grant_payload[SRSLTE_RAR_GRANT_LEN], uint16_t rnti);
  uint32_t get_current_tti();
  float    get_phr();
  float    get_pathloss_db();

  // phy_interface_syssim
  void set_current_tti(uint32_t tti);

  void new_grant_ul(mac_interface_phy_lte::mac_grant_ul_t ul_mac_grant);
  void new_tb(const srsue::mac_interface_phy_lte::mac_grant_dl_t, const uint8_t* data);

  // Radio interface
  void radio_overflow();
  void radio_failure();

  void run_tti();

private:
  srslte::logger*    logger = nullptr;
  srslte::log_filter log;

  // The current cell
  cell_list_t cells;
  cell_t      pcell = {};

  phy_cfg_t phy_cfg = {};

  uint32_t current_tti       = 0;
  uint16_t current_temp_rnti = 0;
  uint32_t cc_idx            = 0;

  int prach_tti_tx = -1;

  int  sr_tx_tti  = -1;
  bool sr_pending = false;

  std::mutex mutex;

  uint32_t ra_trans_cnt = 0;

  stack_interface_phy_lte* stack  = nullptr;
  syssim_interface_phy*    syssim = nullptr;
};

} // namespace srsue

#endif