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

#ifndef SRSLTE_RRC_ASN1_UTILS_H
#define SRSLTE_RRC_ASN1_UTILS_H

#include "srslte/interfaces/mac_interface_types.h"
#include "srslte/interfaces/pdcp_interface_types.h"
#include "srslte/interfaces/rlc_interface_types.h"
#include "srslte/interfaces/rrc_interface_types.h"
#include "srslte/interfaces/sched_interface.h"

/************************
 * Forward declarations
 ***********************/
namespace asn1 {
namespace rrc {

struct plmn_id_s;
struct s_tmsi_s;
struct rlc_cfg_c;
struct pdcp_cfg_s;
struct srb_to_add_mod_s;
// mac
struct sched_request_cfg_c;
struct mac_main_cfg_s;
struct rach_cfg_common_s;
struct time_align_timer_opts;
struct ant_info_ded_s;

struct phys_cfg_ded_s;
struct prach_cfg_info_s;
struct pdsch_cfg_common_s;
struct pusch_cfg_common_s;
struct pucch_cfg_common_s;
struct srs_ul_cfg_common_c;
struct ul_pwr_ctrl_common_s;
struct scell_to_add_mod_r10_s;
struct mbms_notif_cfg_r9_s;
struct mbsfn_area_info_r9_s;
struct mbsfn_sf_cfg_s;
struct mcch_msg_s;
struct sib_type13_r9_s;
// MeasConfig
struct cells_to_add_mod_s;
struct report_cfg_eutra_s;
struct meas_obj_to_add_mod_s;
struct report_cfg_to_add_mod_s;
struct meas_id_to_add_mod_s;
struct quant_cfg_s;

// UE Capabilities
struct ue_eutra_cap_s;

} // namespace rrc
} // namespace asn1

/************************
 *  Conversion Helpers
 ***********************/
namespace srslte {

plmn_id_t make_plmn_id_t(const asn1::rrc::plmn_id_s& asn1_type);
void      to_asn1(asn1::rrc::plmn_id_s* asn1_type, const plmn_id_t& cfg);

s_tmsi_t make_s_tmsi_t(const asn1::rrc::s_tmsi_s& asn1_type);
void     to_asn1(asn1::rrc::s_tmsi_s* asn1_type, const s_tmsi_t& cfg);

/***************************
 *      RLC Config
 **************************/
rlc_config_t make_rlc_config_t(const asn1::rrc::rlc_cfg_c& asn1_type);
rlc_config_t make_rlc_config_t(const asn1::rrc::srb_to_add_mod_s& asn1_type);
void         to_asn1(asn1::rrc::rlc_cfg_c* asn1_type, const rlc_config_t& cfg);

/***************************
 *      PDCP Config
 **************************/
srslte::pdcp_config_t make_srb_pdcp_config_t(const uint8_t bearer_id, bool is_ue);
srslte::pdcp_config_t make_drb_pdcp_config_t(const uint8_t bearer_id, bool is_ue);
srslte::pdcp_config_t
make_drb_pdcp_config_t(const uint8_t bearer_id, bool is_ue, const asn1::rrc::pdcp_cfg_s& pdcp_cfg);

/***************************
 *      MAC Config
 **************************/
void set_mac_cfg_t_sched_request_cfg(mac_cfg_t* cfg, const asn1::rrc::sched_request_cfg_c& asn1_type);
void set_mac_cfg_t_main_cfg(mac_cfg_t* cfg, const asn1::rrc::mac_main_cfg_s& asn1_type);
void set_mac_cfg_t_rach_cfg_common(mac_cfg_t* cfg, const asn1::rrc::rach_cfg_common_s& asn1_type);
void set_mac_cfg_t_time_alignment(mac_cfg_t* cfg, const asn1::rrc::time_align_timer_opts asn1_type);

srsenb::sched_interface::ant_info_ded_t make_ant_info_ded(const asn1::rrc::ant_info_ded_s& asn1_type);

/***************************
 *      PHY Config
 **************************/
void set_phy_cfg_t_dedicated_cfg(phy_cfg_t* cfg, const asn1::rrc::phys_cfg_ded_s& asn1_type);
void set_phy_cfg_t_common_prach(phy_cfg_t* cfg, const asn1::rrc::prach_cfg_info_s* asn1_type, uint32_t root_seq_idx);
void set_phy_cfg_t_common_pdsch(phy_cfg_t* cfg, const asn1::rrc::pdsch_cfg_common_s& asn1_type);
void set_phy_cfg_t_common_pusch(phy_cfg_t* cfg, const asn1::rrc::pusch_cfg_common_s& asn1_type);
void set_phy_cfg_t_common_pucch(phy_cfg_t* cfg, const asn1::rrc::pucch_cfg_common_s& asn1_type);
void set_phy_cfg_t_common_srs(phy_cfg_t* cfg, const asn1::rrc::srs_ul_cfg_common_c& asn1_type);
void set_phy_cfg_t_common_pwr_ctrl(phy_cfg_t* cfg, const asn1::rrc::ul_pwr_ctrl_common_s& asn1_type);
void set_phy_cfg_t_scell_config(phy_cfg_t* cfg, const asn1::rrc::scell_to_add_mod_r10_s& asn1_type);
void set_phy_cfg_t_enable_64qam(phy_cfg_t* cfg, const bool enabled);

/***************************
 *  EUTRA UE Capabilities
 **************************/
void set_rrc_ue_capabilities_t(rrc_ue_capabilities_t& ue_cap, const asn1::rrc::ue_eutra_cap_s& eutra_cap_s);

// mbms
mbms_notif_cfg_t  make_mbms_notif_cfg(const asn1::rrc::mbms_notif_cfg_r9_s& asn1_type);
mbsfn_area_info_t make_mbsfn_area_info(const asn1::rrc::mbsfn_area_info_r9_s& asn1_type);
mbsfn_sf_cfg_t    make_mbsfn_sf_cfg(const asn1::rrc::mbsfn_sf_cfg_s& sf_cfg);
mcch_msg_t        make_mcch_msg(const asn1::rrc::mcch_msg_s& asn1_type);
sib13_t           make_sib13(const asn1::rrc::sib_type13_r9_s& asn1_type);

} // namespace srslte

/************************
 * ASN1 RRC extensions
 ***********************/
namespace asn1 {
namespace rrc {

/***************************
 *      MeasConfig
 **************************/
bool operator==(const asn1::rrc::cells_to_add_mod_s& lhs, const asn1::rrc::cells_to_add_mod_s& rhs);
bool operator==(const asn1::rrc::meas_obj_to_add_mod_s& lhs, const asn1::rrc::meas_obj_to_add_mod_s& rhs);
bool operator==(const asn1::rrc::report_cfg_eutra_s& lhs, const asn1::rrc::report_cfg_eutra_s& rhs);
bool operator==(const asn1::rrc::report_cfg_to_add_mod_s& lhs, const asn1::rrc::report_cfg_to_add_mod_s& rhs);
bool operator==(const asn1::rrc::meas_id_to_add_mod_s& lhs, const asn1::rrc::meas_id_to_add_mod_s& rhs);
bool operator==(const asn1::rrc::quant_cfg_s& lhs, const asn1::rrc::quant_cfg_s& rhs);

} // namespace rrc
} // namespace asn1

#endif // SRSLTE_RRC_ASN1_UTILS_H
