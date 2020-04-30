/*
 * Copyright 2013-2020 Software Radio Systems Limited
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

#ifndef SRSLTE_RRC_CONFIG_H
#define SRSLTE_RRC_CONFIG_H

#include "srslte/asn1/rrc_asn1.h"
#include "srslte/common/security.h"
#include "srslte/interfaces/enb_rrc_interface_types.h"

namespace srsenb {

struct rrc_cfg_sr_t {
  uint32_t                                                   period;
  asn1::rrc::sched_request_cfg_c::setup_s_::dsr_trans_max_e_ dsr_max;
  uint32_t                                                   nof_prb;
  uint32_t                                                   sf_mapping[80];
  uint32_t                                                   nof_subframes;
};

enum rrc_cfg_cqi_mode_t { RRC_CFG_CQI_MODE_PERIODIC = 0, RRC_CFG_CQI_MODE_APERIODIC, RRC_CFG_CQI_MODE_N_ITEMS };

static const char rrc_cfg_cqi_mode_text[RRC_CFG_CQI_MODE_N_ITEMS][20] = {"periodic", "aperiodic"};

typedef struct {
  uint32_t           sf_mapping[80];
  uint32_t           nof_subframes;
  uint32_t           nof_prb;
  uint32_t           period;
  uint32_t           m_ri;
  bool               simultaneousAckCQI;
  rrc_cfg_cqi_mode_t mode;
} rrc_cfg_cqi_t;

typedef struct {
  bool                                          configured;
  asn1::rrc::lc_ch_cfg_s::ul_specific_params_s_ lc_cfg;
  asn1::rrc::pdcp_cfg_s                         pdcp_cfg;
  asn1::rrc::rlc_cfg_c                          rlc_cfg;
} rrc_cfg_qci_t;

#define MAX_NOF_QCI 10

struct rrc_cfg_t {
  uint32_t enb_id; ///< Required to pack SIB1
  // Per eNB SIBs
  asn1::rrc::sib_type1_s     sib1;
  asn1::rrc::sib_info_item_c sibs[ASN1_RRC_MAX_SIB];
  asn1::rrc::mac_main_cfg_s  mac_cnfg;

  asn1::rrc::pusch_cfg_ded_s          pusch_cfg;
  asn1::rrc::ant_info_ded_s           antenna_info;
  asn1::rrc::pdsch_cfg_ded_s::p_a_e_  pdsch_cfg;
  rrc_cfg_sr_t                        sr_cfg;
  rrc_cfg_cqi_t                       cqi_cfg;
  rrc_cfg_qci_t                       qci_cfg[MAX_NOF_QCI];
  bool                                enable_mbsfn;
  uint32_t                            inactivity_timeout_ms;
  srslte::CIPHERING_ALGORITHM_ID_ENUM eea_preference_list[srslte::CIPHERING_ALGORITHM_ID_N_ITEMS];
  srslte::INTEGRITY_ALGORITHM_ID_ENUM eia_preference_list[srslte::INTEGRITY_ALGORITHM_ID_N_ITEMS];
  bool                                meas_cfg_present = false;
  srslte_cell_t                       cell;
  cell_list_t                         cell_list;
};

constexpr uint32_t UE_PCELL_CC_IDX = 0;

} // namespace srsenb

#endif // SRSLTE_RRC_CONFIG_H
