/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSLTE_RRC_NR_UTILS_H
#define SRSLTE_RRC_NR_UTILS_H

#include "srslte/interfaces/mac_interface_types.h"
#include "srslte/interfaces/pdcp_interface_types.h"
#include "srslte/interfaces/rlc_interface_types.h"
#include "srslte/interfaces/rrc_interface_types.h"
#include "srslte/interfaces/sched_interface.h"

/************************
 * Forward declarations
 ***********************/
namespace asn1 {
namespace rrc_nr {

struct plmn_id_s;
struct sib1_s;
struct rlc_cfg_c;
struct pdcp_cfg_s;
struct lc_ch_cfg_s;
struct rach_cfg_common_s;

} // namespace rrc_nr
} // namespace asn1

/************************
 *  Conversion Helpers
 ***********************/
namespace srslte {

plmn_id_t make_plmn_id_t(const asn1::rrc_nr::plmn_id_s& asn1_type);
void      to_asn1(asn1::rrc_nr::plmn_id_s* asn1_type, const plmn_id_t& cfg);

/***************************
 *      MAC Config
 **************************/
logical_channel_config_t make_mac_logical_channel_cfg_t(uint8_t lcid, const asn1::rrc_nr::lc_ch_cfg_s& asn1_type);
rach_nr_cfg_t            make_mac_rach_cfg(const asn1::rrc_nr::rach_cfg_common_s& asn1_type);
/***************************
 *      RLC Config
 **************************/
rlc_config_t make_rlc_config_t(const asn1::rrc_nr::rlc_cfg_c& asn1_type);

/***************************
 *      PDCP Config
 **************************/
pdcp_config_t make_drb_pdcp_config_t(const uint8_t bearer_id, bool is_ue, const asn1::rrc_nr::pdcp_cfg_s& pdcp_cfg);

} // namespace srslte

namespace srsenb {

int set_sched_cell_cfg_sib1(srsenb::sched_interface::cell_cfg_t* sched_cfg, const asn1::rrc_nr::sib1_s& sib1);
}

#endif // SRSLTE_RRC_NR_UTILS_H
