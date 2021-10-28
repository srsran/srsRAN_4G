/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSRAN_RRC_CONFIG_NR_H
#define SRSRAN_RRC_CONFIG_NR_H

#include "../rrc_config_common.h"
#include "srsran/asn1/rrc_nr.h"
#include "srsran/interfaces/gnb_rrc_nr_interfaces.h"
#include "srsue/hdr/phy/phy_common.h"

namespace srsenb {

// TODO: Make this common to NR and LTE
struct rrc_nr_cfg_sr_t {
  uint32_t period;
  //  asn1::rrc::sched_request_cfg_c::setup_s_::dsr_trans_max_e_ dsr_max;
  uint32_t nof_prb;
  uint32_t sf_mapping[80];
  uint32_t nof_subframes;
};

struct rrc_nr_cfg_t {
  asn1::rrc_nr::mib_s                                     mib;
  asn1::rrc_nr::sib1_s                                    sib1;
  asn1::rrc_nr::sys_info_ies_s::sib_type_and_info_item_c_ sibs[ASN1_RRC_NR_MAX_SIB];
  uint32_t                                                nof_sibs;
  rrc_nr_cfg_sr_t                                         sr_cfg;
  rrc_cfg_cqi_t                                           cqi_cfg;
  rrc_cell_list_nr_t                                      cell_list;
  asn1::rrc_nr::rach_cfg_common_s                         rach_cfg_common;
  uint16_t                                                prach_root_seq_idx_type;

  std::string log_name = "RRC-NR";
  std::string log_level;
  uint32_t    log_hex_limit;
};

} // namespace srsenb

#endif // SRSRAN_RRC_CONFIG_NR_H
