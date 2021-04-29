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

#ifndef SRSRAN_RRC_NR_UTILS_H
#define SRSRAN_RRC_NR_UTILS_H

#include "srsran/interfaces/mac_interface_types.h"
#include "srsran/interfaces/pdcp_interface_types.h"
#include "srsran/interfaces/rlc_interface_types.h"
#include "srsran/interfaces/rrc_interface_types.h"
#include "srsran/interfaces/sched_interface.h"

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
struct phr_cfg_s;

// Phy
struct tdd_ul_dl_cfg_common_s;
struct phys_cell_group_cfg_s;
struct search_space_s;
struct search_space_s;
struct csi_report_cfg_s;
struct ctrl_res_set_s;
struct pdsch_time_domain_res_alloc_s;
struct pusch_time_domain_res_alloc_s;
struct pucch_format_cfg_s;
struct pucch_res_s;
struct sched_request_res_cfg_s;
struct pusch_cfg_s;
struct pdsch_cfg_s;
struct dmrs_dl_cfg_s;
struct dmrs_ul_cfg_s;
struct beta_offsets_s;
struct uci_on_pusch_s;
struct zp_csi_rs_res_s;
struct nzp_csi_rs_res_s;
struct pdsch_serving_cell_cfg_s;
struct freq_info_dl_s;

} // namespace rrc_nr
} // namespace asn1

/************************
 *  Conversion Helpers
 ***********************/
namespace srsran {

plmn_id_t make_plmn_id_t(const asn1::rrc_nr::plmn_id_s& asn1_type);
void      to_asn1(asn1::rrc_nr::plmn_id_s* asn1_type, const plmn_id_t& cfg);
/***************************
 *      PHY Config
 **************************/
bool make_phy_rach_cfg(const asn1::rrc_nr::rach_cfg_common_s& asn1_type, srsran_prach_cfg_t* prach_cfg);

bool make_phy_tdd_cfg(const asn1::rrc_nr::tdd_ul_dl_cfg_common_s& tdd_ul_dl_cfg_common,
                      srsran_tdd_config_nr_t*                     srsran_tdd_config_nr);
bool make_phy_harq_ack_cfg(const asn1::rrc_nr::phys_cell_group_cfg_s& phys_cell_group_cfg,
                           srsran_ue_dl_nr_harq_ack_cfg_t*            srsran_ue_dl_nr_harq_ack_cfg);
bool make_phy_coreset_cfg(const asn1::rrc_nr::ctrl_res_set_s& ctrl_res_set, srsran_coreset_t* srsran_coreset);
bool make_phy_search_space_cfg(const asn1::rrc_nr::search_space_s& search_space,
                               srsran_search_space_t*              srsran_search_space);
bool make_phy_csi_report(const asn1::rrc_nr::csi_report_cfg_s& csi_report_cfg,
                         srsran_csi_hl_report_cfg_t*           srsran_csi_hl_report_cfg);
bool make_phy_common_time_ra(const asn1::rrc_nr::pdsch_time_domain_res_alloc_s& pdsch_time_domain_res_alloc,
                             srsran_sch_time_ra_t*                              srsran_sch_time_ra);
bool make_phy_common_time_ra(const asn1::rrc_nr::pusch_time_domain_res_alloc_s& pusch_time_domain_res_allo,
                             srsran_sch_time_ra_t*                              srsran_sch_time_ra);
bool make_phy_max_code_rate(const asn1::rrc_nr::pucch_format_cfg_s& pucch_format_cfg, uint32_t* max_code_rate);
bool make_phy_res_config(const asn1::rrc_nr::pucch_res_s& pucch_res,
                         uint32_t                         format_2_max_code_rate,
                         srsran_pucch_nr_resource_t*      srsran_pucch_nr_resource);
bool make_phy_sr_resource(const asn1::rrc_nr::sched_request_res_cfg_s& sched_request_res_cfg,
                          srsran_pucch_nr_sr_resource_t*               srsran_pucch_nr_sr_resource);
bool make_phy_pusch_alloc_type(const asn1::rrc_nr::pusch_cfg_s& pusch_cfg,
                               srsran_resource_alloc_t*         in_srsran_resource_alloc);
bool make_phy_pdsch_alloc_type(const asn1::rrc_nr::pdsch_cfg_s& pdsch_cfg,
                               srsran_resource_alloc_t*         in_srsran_resource_alloc);
bool make_phy_dmrs_dl_additional_pos(const asn1::rrc_nr::dmrs_dl_cfg_s& dmrs_dl_cfg,
                                     srsran_dmrs_sch_add_pos_t*         in_srsran_dmrs_sch_add_pos);
bool make_phy_dmrs_ul_additional_pos(const asn1::rrc_nr::dmrs_ul_cfg_s& dmrs_ul_cfg,
                                     srsran_dmrs_sch_add_pos_t*         srsran_dmrs_sch_add_pos);
bool make_phy_beta_offsets(const asn1::rrc_nr::beta_offsets_s& beta_offsets,
                           srsran_beta_offsets_t*              srsran_beta_offsets);
bool make_phy_pusch_scaling(const asn1::rrc_nr::uci_on_pusch_s& uci_on_pusch, float* scaling);
bool make_phy_zp_csi_rs_resource(const asn1::rrc_nr::zp_csi_rs_res_s& zp_csi_rs_res,
                                 srsran_csi_rs_zp_resource_t*         zp_csi_rs_resource);
bool make_phy_nzp_csi_rs_resource(const asn1::rrc_nr::nzp_csi_rs_res_s& nzp_csi_rs_res,
                                  srsran_csi_rs_nzp_resource_t*         csi_rs_nzp_resource);
bool make_phy_carrier_cfg(const asn1::rrc_nr::freq_info_dl_s& freq_info_dl, srsran_carrier_nr_t* carrier_nr);
/***************************
 *      MAC Config
 **************************/
logical_channel_config_t make_mac_logical_channel_cfg_t(uint8_t lcid, const asn1::rrc_nr::lc_ch_cfg_s& asn1_type);
rach_nr_cfg_t            make_mac_rach_cfg(const asn1::rrc_nr::rach_cfg_common_s& asn1_type);
bool                     make_mac_phr_cfg_t(const asn1::rrc_nr::phr_cfg_s& asn1_type, phr_cfg_nr_t* phr_cfg_nr);
bool                     make_mac_dl_harq_cfg_nr_t(const asn1::rrc_nr::pdsch_serving_cell_cfg_s& asn1_type,
                                                   dl_harq_cfg_nr_t*                             out_dl_harq_cfg_nr);
/***************************
 *      RLC Config
 **************************/
rlc_config_t make_rlc_config_t(const asn1::rrc_nr::rlc_cfg_c& asn1_type);

/***************************
 *      PDCP Config
 **************************/
pdcp_config_t make_drb_pdcp_config_t(const uint8_t bearer_id, bool is_ue, const asn1::rrc_nr::pdcp_cfg_s& pdcp_cfg);

} // namespace srsran

namespace srsenb {

int set_sched_cell_cfg_sib1(srsenb::sched_interface::cell_cfg_t* sched_cfg, const asn1::rrc_nr::sib1_s& sib1);
}

#endif // SRSRAN_RRC_NR_UTILS_H
