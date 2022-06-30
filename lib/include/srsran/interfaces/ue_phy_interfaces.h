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

#ifndef SRSRAN_UE_PHY_INTERFACES_H
#define SRSRAN_UE_PHY_INTERFACES_H

#include "srsran/srsran.h"

#include "phy_interface_types.h"
#include "rrc_interface_types.h"
#include "srsran/common/interfaces_common.h"
#include "srsran/phy/channel/channel.h"

#include <map>
#include <set>
#include <string>

namespace srsue {

struct cfr_args_t {
  bool              enable           = false;
  srsran_cfr_mode_t mode             = SRSRAN_CFR_THR_MANUAL;
  float             manual_thres     = 2.0f;
  float             strength         = 1.0f;
  float             auto_target_papr = 7.0f;
  float             ema_alpha        = 1.0f / (float)SRSRAN_CP_NORM_NSYMB;
};

struct phy_args_t {
  std::string            type = "lte";
  srsran::phy_log_args_t log;

  std::string           dl_earfcn      = "3400"; // comma-separated list of DL EARFCNs
  std::string           ul_earfcn      = "";     // comma-separated list of UL EARFCNs
  std::vector<uint32_t> dl_earfcn_list = {3400}; // vectorized version of dl_earfcn that gets populated during init
  std::map<uint32_t, uint32_t> ul_earfcn_map;    // Map linking DL EARFCN and UL EARFCN

  int force_N_id_2 = -1; // Cell identity within the identity group (PSS) to filter.
  int force_N_id_1 = -1; // Cell identity group (SSS) to filter.

  float dl_freq = -1.0f;
  float ul_freq = -1.0f;

  bool     ul_pwr_ctrl_en  = false;
  float    prach_gain      = -1;
  uint32_t pdsch_max_its   = 8;
  bool     meas_evm        = false;
  uint32_t nof_phy_threads = 3;

  int worker_cpu_mask   = -1;
  int sync_cpu_affinity = -1;

  uint32_t    nof_lte_carriers             = 1;
  uint32_t    nof_nr_carriers              = 0;
  uint32_t    nr_max_nof_prb               = 52;
  uint32_t    nof_rx_ant                   = 1;
  std::string equalizer_mode               = "mmse";
  int         cqi_max                      = 15;
  int         cqi_fixed                    = -1;
  float       snr_ema_coeff                = 0.1f;
  std::string snr_estim_alg                = "refs";
  bool        agc_enable                   = true;
  bool        correct_sync_error           = false;
  bool        cfo_is_doppler               = false;
  bool        cfo_integer_enabled          = false;
  float       cfo_correct_tol_hz           = 1.0f;
  float       cfo_pss_ema                  = DEFAULT_CFO_EMA_TRACK;
  float       cfo_loop_bw_pss              = DEFAULT_CFO_BW_PSS;
  float       cfo_loop_bw_ref              = DEFAULT_CFO_BW_REF;
  float       cfo_loop_ref_min             = DEFAULT_CFO_REF_MIN;
  float       cfo_loop_pss_tol             = DEFAULT_CFO_PSS_MIN;
  float       sfo_ema                      = DEFAULT_SFO_EMA_COEFF;
  uint32_t    sfo_correct_period           = DEFAULT_SAMPLE_OFFSET_CORRECT_PERIOD;
  uint32_t    cfo_loop_pss_conv            = DEFAULT_PSS_STABLE_TIMEOUT;
  uint32_t    cfo_ref_mask                 = 1023;
  bool        interpolate_subframe_enabled = false;
  bool        estimator_fil_auto           = false;
  float       estimator_fil_stddev         = 1.0f;
  uint32_t    estimator_fil_order          = 4;
  float       snr_to_cqi_offset            = 0.0f;
  std::string sss_algorithm                = "full";
  float       rx_gain_offset               = 62;
  bool        pdsch_csi_enabled            = true;
  bool        pdsch_8bit_decoder           = false;
  uint32_t    intra_freq_meas_len_ms       = 20;
  uint32_t    intra_freq_meas_period_ms    = 200;
  float       force_ul_amplitude           = 0.0f;
  bool        detect_cp                    = false;

  bool nr_store_pdsch_ko = false;

  float    in_sync_rsrp_dbm_th    = -130.0f;
  float    in_sync_snr_db_th      = 1.0f;
  uint32_t nof_in_sync_events     = 10;
  uint32_t nof_out_of_sync_events = 20;

  srsran::channel::args_t dl_channel_args;
  srsran::channel::args_t ul_channel_args;

  cfr_args_t cfr_args; ///< Stores user-defined CFR configuration
};

/* RAT agnostic Interface MAC -> PHY */
class phy_interface_mac_common
{
public:
  /* Time advance commands */
  virtual void set_timeadv_rar(uint32_t tti, uint32_t ta_cmd) = 0;
  virtual void set_timeadv(uint32_t tti, uint32_t ta_cmd)     = 0;

  /* Activate / Disactivate SCell*/
  virtual void set_activation_deactivation_scell(uint32_t cmd, uint32_t tti) = 0;

  /* Sets RAR dci payload */
  virtual void set_rar_grant(uint8_t grant_payload[SRSRAN_RAR_GRANT_LEN], uint16_t rnti) = 0;

  virtual uint32_t get_current_tti() = 0;

  virtual float get_phr()         = 0;
  virtual float get_pathloss_db() = 0;
};

/* Interface MAC -> PHY */
class phy_interface_mac_lte : public phy_interface_mac_common
{
public:
  typedef struct {
    bool     is_transmitted;
    uint32_t tti_ra;
    uint32_t f_id;
    uint32_t preamble_format;
  } prach_info_t;

  virtual void
  prach_send(uint32_t preamble_idx, int allowed_subframe, float target_power_dbm, float ta_base_sec = 0.0f) = 0;
  virtual prach_info_t prach_get_info()                                                                     = 0;

  /* Indicates the transmission of a SR signal in the next opportunity */
  virtual void sr_send()        = 0;
  virtual int  sr_last_tx_tti() = 0;

  virtual void set_mch_period_stop(uint32_t stop) = 0;
};

class phy_interface_rrc_lte
{
public:
  virtual bool set_config(const srsran::phy_cfg_t& config, uint32_t cc_idx = 0)           = 0;
  virtual bool set_scell(srsran_cell_t cell_info, uint32_t cc_idx, uint32_t earfcn)       = 0;
  virtual void set_config_tdd(srsran_tdd_config_t& tdd_config)                            = 0;
  virtual void set_config_mbsfn_sib2(srsran::mbsfn_sf_cfg_t* cfg_list, uint32_t nof_cfgs) = 0;
  virtual void set_config_mbsfn_sib13(const srsran::sib13_t& sib13)                       = 0;
  virtual void set_config_mbsfn_mcch(const srsran::mcch_msg_t& mcch)                      = 0;

  virtual void deactivate_scells() = 0;

  /* Measurements interface */
  virtual void set_cells_to_meas(uint32_t earfcn, const std::set<uint32_t>& pci) = 0;
  virtual void meas_stop()                                                       = 0;

  /* Cell search and selection procedures */
  virtual bool cell_search(int earfcn)      = 0;
  virtual bool cell_select(phy_cell_t cell) = 0;
  virtual bool cell_is_camping()            = 0;
};

// Combined interface for stack (MAC and RRC) to access PHY
class phy_interface_stack_lte : public phy_interface_mac_lte, public phy_interface_rrc_lte
{};

} // namespace srsue

#endif // SRSRAN_UE_PHY_INTERFACES_H
