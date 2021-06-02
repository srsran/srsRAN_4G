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

#ifndef SRSRAN_RRC_CONFIG_H
#define SRSRAN_RRC_CONFIG_H

#include "rrc_config_common.h"
#include "srsran/asn1/rrc.h"
#include "srsran/common/security.h"
#include "srsran/interfaces/enb_rrc_interface_types.h"
#include "srsran/phy/common/phy_common.h"
#include <array>

namespace srsenb {

struct rrc_cfg_sr_t {
  uint32_t                                                   period;
  asn1::rrc::sched_request_cfg_c::setup_s_::dsr_trans_max_e_ dsr_max;
  uint32_t                                                   nof_prb;
  uint32_t                                                   sf_mapping[80];
  uint32_t                                                   nof_subframes;
};

struct rrc_cfg_qci_t {
  bool                                          configured = false;
  asn1::rrc::lc_ch_cfg_s::ul_specific_params_s_ lc_cfg;
  asn1::rrc::pdcp_cfg_s                         pdcp_cfg;
  asn1::rrc::rlc_cfg_c                          rlc_cfg;
};

struct rrc_cfg_t {
  uint32_t enb_id; ///< Required to pack SIB1
  // Per eNB SIBs
  asn1::rrc::sib_type1_s     sib1;
  asn1::rrc::sib_info_item_c sibs[ASN1_RRC_MAX_SIB];
  asn1::rrc::mac_main_cfg_s  mac_cnfg;

  asn1::rrc::pusch_cfg_ded_s                                                              pusch_cfg;
  asn1::rrc::ant_info_ded_s                                                               antenna_info;
  asn1::rrc::pdsch_cfg_ded_s::p_a_e_                                                      pdsch_cfg;
  rrc_cfg_sr_t                                                                            sr_cfg;
  rrc_cfg_cqi_t                                                                           cqi_cfg;
  std::map<uint32_t, rrc_cfg_qci_t>                                                       qci_cfg;
  bool                                                                                    enable_mbsfn;
  uint16_t                                                                                mbms_mcs;
  uint32_t                                                                                inactivity_timeout_ms;
  std::array<srsran::CIPHERING_ALGORITHM_ID_ENUM, srsran::CIPHERING_ALGORITHM_ID_N_ITEMS> eea_preference_list;
  std::array<srsran::INTEGRITY_ALGORITHM_ID_ENUM, srsran::INTEGRITY_ALGORITHM_ID_N_ITEMS> eia_preference_list;
  bool                                                                                    meas_cfg_present = false;
  srsran_cell_t                                                                           cell;
  cell_list_t                                                                             cell_list;
  cell_list_t                                                                             cell_list_nr;
  uint32_t                                                                                max_mac_dl_kos;
  uint32_t                                                                                max_mac_ul_kos;
  uint32_t                                                                                rlf_release_timer_ms;
  asn1::rrc::srb_to_add_mod_s::rlc_cfg_c_                                                 srb1_cfg;
  asn1::rrc::srb_to_add_mod_s::rlc_cfg_c_                                                 srb2_cfg;
};

constexpr uint32_t UE_PCELL_CC_IDX = 0;

struct ue_var_cfg_t {
  asn1::rrc::rr_cfg_ded_s                rr_cfg;
  asn1::rrc::meas_cfg_s                  meas_cfg;
  asn1::rrc::scell_to_add_mod_list_r10_l scells;
};

} // namespace srsenb

#endif // SRSRAN_RRC_CONFIG_H
