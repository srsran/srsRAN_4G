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

#include "srsenb/hdr/stack/rrc/ue_rr_cfg.h"
#include "srsenb/hdr/stack/rrc/rrc_bearer_cfg.h"
#include "srsenb/hdr/stack/rrc/rrc_cell_cfg.h"
#include "srsenb/hdr/stack/rrc/rrc_config.h"
#include "srsran/asn1/rrc_utils.h"
#include "srsran/rrc/rrc_cfg_utils.h"

#define SET_OPT_FIELD(fieldname, out, in)                                                                              \
  if (in.fieldname##_present) {                                                                                        \
    out.fieldname##_present = true;                                                                                    \
    out.fieldname           = in.fieldname;                                                                            \
  }                                                                                                                    \
  do {                                                                                                                 \
  } while (0)

using namespace asn1::rrc;

namespace srsenb {

/******************************
 *        SRBs / DRBs
 *****************************/

srb_to_add_mod_s* add_srb(srb_to_add_mod_list_l& srbs, uint8_t srb_id)
{
  if (srb_id > 2 or srb_id == 0) {
    srslog::fetch_basic_logger("RRC").error("Invalid SRB id=%d", srb_id);
    return nullptr;
  }

  // Set SRBtoAddMod
  auto srb_it               = srsran::add_rrc_obj_id(srbs, srb_id);
  srb_it->lc_ch_cfg_present = true;
  srb_it->lc_ch_cfg.set(srb_to_add_mod_s::lc_ch_cfg_c_::types_opts::default_value);
  srb_it->rlc_cfg_present = true;
  srb_it->rlc_cfg.set(srb_to_add_mod_s::rlc_cfg_c_::types_opts::default_value);

  return srb_it;
}

void fill_srbs_reconf(srb_to_add_mod_list_l& srbs, const srb_to_add_mod_list_l& current_srbs)
{
  // NOTE: In case of Handover, the Reconf includes SRB1
  if (srsran::find_rrc_obj_id(current_srbs, 1) == current_srbs.end()) {
    add_srb(srbs, 1);
  }

  if (srsran::find_rrc_obj_id(current_srbs, 2) == current_srbs.end()) {
    add_srb(srbs, 2);
  }
}

/******************************
 *        SR Config
 *****************************/

void fill_sr_cfg_setup(sched_request_cfg_c& sr_cfg, const ue_cell_ded_list& ue_cell_list)
{
  auto& setup            = sr_cfg.setup();
  setup.sr_cfg_idx       = ue_cell_list.get_sr_res()->sr_I;
  setup.sr_pucch_res_idx = ue_cell_list.get_sr_res()->sr_N_pucch;
}

/******************************
 *     CQI Report Config
 *****************************/

int16_t get_ri(uint32_t m_ri)
{
  int16_t ri_idx = 0;

  int32_t N_offset_ri = 0; // Naivest approach: overlap RI with PMI
  switch (m_ri) {
    case 0:
      // Disabled
      break;
    case 1:
      ri_idx = -N_offset_ri;
      break;
    case 2:
      ri_idx = 161 - N_offset_ri;
      break;
    case 4:
      ri_idx = 322 - N_offset_ri;
      break;
    case 8:
      ri_idx = 483 - N_offset_ri;
      break;
    case 16:
      ri_idx = 644 - N_offset_ri;
      break;
    case 32:
      ri_idx = 805 - N_offset_ri;
      break;
    default:
      srslog::fetch_basic_logger("RRC").error("Allocating RI: invalid m_ri=%d", m_ri);
      return -1;
  }

  return ri_idx;
}

void fill_cqi_report_enb_cfg(cqi_report_cfg_s& cqi_report_cfg, const rrc_cfg_t& enb_cfg)
{
  if (enb_cfg.cqi_cfg.mode == RRC_CFG_CQI_MODE_APERIODIC) {
    cqi_report_cfg.cqi_report_mode_aperiodic_present = true;
    cqi_report_cfg.cqi_report_mode_aperiodic         = cqi_report_mode_aperiodic_e::rm30;
  } else {
    cqi_report_cfg.cqi_report_periodic_present = true;
    auto& cqi_setup                            = cqi_report_cfg.cqi_report_periodic.set_setup();
    cqi_setup.cqi_format_ind_periodic.set(
        cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_::types::wideband_cqi);
    cqi_setup.simul_ack_nack_and_cqi = enb_cfg.cqi_cfg.simultaneousAckCQI;
  }
  cqi_report_cfg.nom_pdsch_rs_epre_offset = 0;
}

int fill_cqi_report_setup(cqi_report_cfg_s& cqi_rep, const rrc_cfg_t& enb_cfg, const ue_cell_ded_list& ue_cell_list)
{
  // eNB params set at this point

  // For Periodic CQI, set PUCCH resources
  if (cqi_rep.cqi_report_periodic_present) {
    const ue_cell_ded* pcell = ue_cell_list.get_ue_cc_idx(UE_PCELL_CC_IDX);
    if (pcell == nullptr or not pcell->cqi_res_present) {
      srslog::fetch_basic_logger("RRC").warning("PCell CQI resources haven\'t been allocated yet");
      return SRSRAN_ERROR;
    }
    auto& cqi_periodic             = cqi_rep.cqi_report_periodic.setup();
    cqi_periodic.cqi_pmi_cfg_idx   = pcell->cqi_res.pmi_idx;
    cqi_periodic.cqi_pucch_res_idx = pcell->cqi_res.pucch_res;
  }

  return SRSRAN_SUCCESS;
}

void fill_cqi_report_reconf(cqi_report_cfg_s& cqi_rep, const rrc_cfg_t& enb_cfg, const ue_cell_ded_list& ue_cell_list)
{
  // Get RRC setup CQI config
  if (fill_cqi_report_setup(cqi_rep, enb_cfg, ue_cell_list) == SRSRAN_ERROR) {
    return;
  }

  if (cqi_rep.cqi_report_mode_aperiodic_present) {
    if (enb_cfg.antenna_info.tx_mode.value == ant_info_ded_s::tx_mode_e_::tm4) {
      cqi_rep.cqi_report_mode_aperiodic = cqi_report_mode_aperiodic_e::rm31;
    } else {
      cqi_rep.cqi_report_mode_aperiodic = cqi_report_mode_aperiodic_e::rm30;
    }
  }

  if (cqi_rep.cqi_report_periodic_present) {
    auto& cqi_setup = cqi_rep.cqi_report_periodic.setup();

    // Add RI config
    cqi_setup.ri_cfg_idx_present = false;
    if ((enb_cfg.antenna_info.tx_mode == ant_info_ded_s::tx_mode_e_::tm3) ||
        (enb_cfg.antenna_info.tx_mode == ant_info_ded_s::tx_mode_e_::tm4)) {
      int16_t ri_idx = get_ri(enb_cfg.cqi_cfg.m_ri);
      if (ri_idx >= 0) {
        cqi_setup.ri_cfg_idx_present = true;
        cqi_setup.ri_cfg_idx         = ri_idx;
      } else {
        srslog::fetch_basic_logger("RRC").warning("Warning: Configured wrong M_ri parameter.");
      }
    }
  }
}

/******************************
 *     PHYConfigDedicated
 *****************************/

/// Fills ASN1 PhysicalConfigurationDedicated struct with eNB config params at startup
void fill_phy_cfg_ded_enb_cfg(phys_cfg_ded_s& phy_cfg, const rrc_cfg_t& enb_cfg)
{
  // PUSCH
  phy_cfg.pusch_cfg_ded_present = true;
  phy_cfg.pusch_cfg_ded         = enb_cfg.pusch_cfg;

  // SR config
  phy_cfg.sched_request_cfg_present                   = true;
  phy_cfg.sched_request_cfg.set_setup().dsr_trans_max = enb_cfg.sr_cfg.dsr_max;

  // Default Antenna Config
  phy_cfg.ant_info_present = true;
  ant_info_ded_s& ant_info = phy_cfg.ant_info.set_explicit_value();
  if (enb_cfg.cell.nof_ports == 1) {
    ant_info.tx_mode.value = ant_info_ded_s::tx_mode_e_::tm1;
  } else {
    ant_info.tx_mode.value = ant_info_ded_s::tx_mode_e_::tm2;
  }
  ant_info.ue_tx_ant_sel.set(setup_e::release);

  // UL Power control
  phy_cfg.ul_pwr_ctrl_ded_present              = true;
  phy_cfg.ul_pwr_ctrl_ded.p0_ue_pusch          = 0;
  phy_cfg.ul_pwr_ctrl_ded.delta_mcs_enabled    = ul_pwr_ctrl_ded_s::delta_mcs_enabled_e_::en0;
  phy_cfg.ul_pwr_ctrl_ded.accumulation_enabled = true;
  phy_cfg.ul_pwr_ctrl_ded.p0_ue_pucch          = 0;
  phy_cfg.ul_pwr_ctrl_ded.psrs_offset          = 3;

  // PDSCH
  phy_cfg.pdsch_cfg_ded_present = true;
  phy_cfg.pdsch_cfg_ded.p_a     = enb_cfg.pdsch_cfg;

  // PUCCH ACK
  phy_cfg.pucch_cfg_ded_present = true;
  phy_cfg.pucch_cfg_ded.ack_nack_repeat.set(pucch_cfg_ded_s::ack_nack_repeat_c_::types::release);

  // Setup CQI PUCCH config
  phy_cfg.cqi_report_cfg_present = true;
  fill_cqi_report_enb_cfg(phy_cfg.cqi_report_cfg, enb_cfg);
}

void fill_phy_cfg_ded_setup(phys_cfg_ded_s& phy_cfg, const rrc_cfg_t& enb_cfg, const ue_cell_ded_list& ue_cell_list)
{
  // Set PHYConfigDedicated base
  fill_phy_cfg_ded_enb_cfg(phy_cfg, enb_cfg);

  // Setup SR PUCCH config
  fill_sr_cfg_setup(phy_cfg.sched_request_cfg, ue_cell_list);

  // Setup CQI PUCCH config
  fill_cqi_report_setup(phy_cfg.cqi_report_cfg, enb_cfg, ue_cell_list);
}

/// Fills ASN1 PhysicalConfigurationDedicated struct with eNB config params at RRCReconf
void fill_phy_cfg_ded_reconf(phys_cfg_ded_s&                      phy_cfg,
                             const rrc_cfg_t&                     enb_cfg,
                             const ue_cell_ded_list&              ue_cell_list,
                             const srsran::rrc_ue_capabilities_t& ue_caps)
{
  // Use RRCSetup as starting point
  fill_phy_cfg_ded_setup(phy_cfg, enb_cfg, ue_cell_list);

  // Antenna Configuration
  ant_info_ded_s& ant_info = phy_cfg.ant_info.explicit_value();
  ant_info                 = enb_cfg.antenna_info;

  // CQI Report Config
  fill_cqi_report_reconf(phy_cfg.cqi_report_cfg, enb_cfg, ue_cell_list);

  // DL 256-QAM
  if (ue_caps.support_dl_256qam) {
    phy_cfg.ext = true;
    phy_cfg.cqi_report_cfg_pcell_v1250.set_present(true);
    phy_cfg.cqi_report_cfg_pcell_v1250->alt_cqi_table_r12_present = true;
    phy_cfg.cqi_report_cfg_pcell_v1250->alt_cqi_table_r12.value =
        cqi_report_cfg_v1250_s::alt_cqi_table_r12_opts::all_sfs;
  }
}

/***********************************
 *   radioResourceConfigDedicated
 **********************************/

/// fill radioResourceConfigDedicated with values provided in eNB config
void fill_rr_cfg_ded_enb_cfg(asn1::rrc::rr_cfg_ded_s& rr_cfg, const rrc_cfg_t& enb_cfg)
{
  rr_cfg = {};

  // Fill mac-MainConfig
  rr_cfg.mac_main_cfg_present  = true;
  mac_main_cfg_s& mac_cfg      = rr_cfg.mac_main_cfg.set_explicit_value();
  mac_cfg.ul_sch_cfg_present   = true;
  mac_cfg.ul_sch_cfg           = enb_cfg.mac_cnfg.ul_sch_cfg;
  mac_cfg.phr_cfg_present      = true;
  mac_cfg.phr_cfg              = enb_cfg.mac_cnfg.phr_cfg;
  mac_cfg.time_align_timer_ded = enb_cfg.mac_cnfg.time_align_timer_ded;

  // Fill physicalConfigDedicated
  rr_cfg.phys_cfg_ded_present = true;
  fill_phy_cfg_ded_enb_cfg(rr_cfg.phys_cfg_ded, enb_cfg);

  // Other RR params
  rr_cfg.rlf_timers_and_consts_r9.set_present(false);
  rr_cfg.sps_cfg_present = false;
}

void fill_rr_cfg_ded_setup(asn1::rrc::rr_cfg_ded_s& rr_cfg,
                           const rrc_cfg_t&         enb_cfg,
                           const ue_cell_ded_list&  ue_cell_list)
{
  // Establish default enb config
  fill_rr_cfg_ded_enb_cfg(rr_cfg, enb_cfg);

  // (Re)establish SRB1
  rr_cfg.srb_to_add_mod_list_present = true;
  add_srb(rr_cfg.srb_to_add_mod_list, 1);

  // Setup SR/CQI configs
  rr_cfg.phys_cfg_ded_present = true;
  fill_phy_cfg_ded_setup(rr_cfg.phys_cfg_ded, enb_cfg, ue_cell_list);
}

void fill_rr_cfg_ded_reconf(asn1::rrc::rr_cfg_ded_s&             rr_cfg,
                            const rr_cfg_ded_s&                  current_rr_cfg,
                            const rrc_cfg_t&                     enb_cfg,
                            const ue_cell_ded_list&              ue_cell_list,
                            const bearer_cfg_handler&            bearers,
                            const srsran::rrc_ue_capabilities_t& ue_caps,
                            bool                                 phy_cfg_updated)
{
  // (Re)establish SRBs
  fill_srbs_reconf(rr_cfg.srb_to_add_mod_list, current_rr_cfg.srb_to_add_mod_list);
  rr_cfg.srb_to_add_mod_list_present = rr_cfg.srb_to_add_mod_list.size() > 0;

  // Update DRBs if required
  srsran::compute_cfg_diff(current_rr_cfg.drb_to_add_mod_list,
                           bearers.get_established_drbs(),
                           rr_cfg.drb_to_add_mod_list,
                           rr_cfg.drb_to_release_list);
  rr_cfg.drb_to_add_mod_list_present = rr_cfg.drb_to_add_mod_list.size() > 0;
  rr_cfg.drb_to_release_list_present = rr_cfg.drb_to_release_list.size() > 0;

  // PhysCfgDed update needed
  if (phy_cfg_updated) {
    rr_cfg.phys_cfg_ded_present = true;
    fill_phy_cfg_ded_reconf(rr_cfg.phys_cfg_ded, enb_cfg, ue_cell_list, ue_caps);
  }
}

/**
 * Applies radioResourceReconfiguration
 * @remark Follows TS 36.331 Sec. 5.3.10
 * @param current_rr_cfg_ded current configuration seen by the UE
 * @param pending_rr_cfg_ded pending changes to apply to UE configuration
 */
void apply_rr_cfg_ded_diff(rr_cfg_ded_s& current_rr_cfg_ded, const rr_cfg_ded_s& pending_rr_cfg_ded)
{
  // 5.3.10.1 - SRB addition/Modification
  if (pending_rr_cfg_ded.srb_to_add_mod_list_present) {
    current_rr_cfg_ded.srb_to_add_mod_list_present = true;
    srsran::apply_addmodlist_diff(current_rr_cfg_ded.srb_to_add_mod_list,
                                  pending_rr_cfg_ded.srb_to_add_mod_list,
                                  current_rr_cfg_ded.srb_to_add_mod_list);
  }

  // 5.3.10.(2/3) - DRB release/addition/Modification
  if (pending_rr_cfg_ded.drb_to_add_mod_list_present or pending_rr_cfg_ded.drb_to_release_list_present) {
    srsran::apply_addmodremlist_diff(current_rr_cfg_ded.drb_to_add_mod_list,
                                     pending_rr_cfg_ded.drb_to_add_mod_list,
                                     pending_rr_cfg_ded.drb_to_release_list,
                                     current_rr_cfg_ded.drb_to_add_mod_list);
    current_rr_cfg_ded.drb_to_add_mod_list_present = current_rr_cfg_ded.drb_to_add_mod_list.size() > 0;
  }

  // 5.3.10.3a/b - TODO

  // 5.3.10.4 - MAC main reconfiguration
  SET_OPT_FIELD(mac_main_cfg, current_rr_cfg_ded, pending_rr_cfg_ded);

  // 5.3.10.5 - Semi-persistent scheduling reconfiguration
  SET_OPT_FIELD(sps_cfg, current_rr_cfg_ded, pending_rr_cfg_ded);

  // 5.3.10.6 - Physical channel reconfiguration
  SET_OPT_FIELD(phys_cfg_ded, current_rr_cfg_ded, pending_rr_cfg_ded);

  if (pending_rr_cfg_ded.ext) {
    current_rr_cfg_ded.ext = true;

    // 5.3.10.7 - Radio Link Failure Timers and Constants reconfigurations
    if (pending_rr_cfg_ded.rlf_timers_and_consts_r9.is_present()) {
      *current_rr_cfg_ded.rlf_timers_and_consts_r9 = *pending_rr_cfg_ded.rlf_timers_and_consts_r9;
    }
  }
}

/// Fill rrcConnectionReconfiguration with SCells that were added/mod/removed since last RRC config update
void fill_scells_reconf(asn1::rrc::rrc_conn_recfg_r8_ies_s&  recfg_r8,
                        const scell_to_add_mod_list_r10_l&   current_scells,
                        const rrc_cfg_t&                     enb_cfg,
                        const ue_cell_ded_list&              ue_cell_list,
                        const srsran::rrc_ue_capabilities_t& ue_caps)
{
  // check whether there has been scell updates
  // TODO: check scell modifications and released as well
  if (current_scells.size() + 1 == ue_cell_list.nof_cells()) {
    uint32_t ue_cc_idx = 1;
    for (; ue_cc_idx < ue_cell_list.nof_cells(); ++ue_cc_idx) {
      auto& cellcfg = ue_cell_list.get_ue_cc_idx(ue_cc_idx)->cell_common->cell_cfg;
      if (cellcfg.pci != current_scells[ue_cc_idx - 1].cell_identif_r10.pci_r10 or
          cellcfg.dl_earfcn != current_scells[ue_cc_idx - 1].cell_identif_r10.dl_carrier_freq_r10) {
        break;
      }
    }
    if (ue_cc_idx == ue_cell_list.nof_cells()) {
      // no change has occurred
      return;
    }
  }

  scell_to_add_mod_list_r10_l target_scells(ue_cell_list.nof_cells() - 1);
  for (size_t ue_cc_idx = 1; ue_cc_idx < ue_cell_list.nof_cells(); ++ue_cc_idx) {
    const ue_cell_ded&     scell     = *ue_cell_list.get_ue_cc_idx(ue_cc_idx);
    const enb_cell_common& scell_cfg = *scell.cell_common;
    const sib_type1_s&     cell_sib1 = scell_cfg.sib1;
    const sib_type2_s&     cell_sib2 = scell_cfg.sib2;

    scell_to_add_mod_r10_s& asn1cell              = target_scells[ue_cc_idx - 1];
    asn1cell.scell_idx_r10                        = ue_cc_idx;
    asn1cell.cell_identif_r10_present             = true;
    asn1cell.cell_identif_r10.pci_r10             = scell_cfg.cell_cfg.pci;
    asn1cell.cell_identif_r10.dl_carrier_freq_r10 = scell_cfg.cell_cfg.dl_earfcn;
    asn1cell.rr_cfg_common_scell_r10_present      = true;
    // RadioResourceConfigCommon
    const rr_cfg_common_sib_s& cc_cfg_sib = cell_sib2.rr_cfg_common;
    auto&                      nonul_cfg  = asn1cell.rr_cfg_common_scell_r10.non_ul_cfg_r10;
    asn1::number_to_enum(nonul_cfg.dl_bw_r10, enb_cfg.cell.nof_prb);
    asn1::number_to_enum(nonul_cfg.ant_info_common_r10.ant_ports_count, enb_cfg.cell.nof_ports);
    nonul_cfg.phich_cfg_r10        = scell_cfg.mib.phich_cfg;
    nonul_cfg.pdsch_cfg_common_r10 = cc_cfg_sib.pdsch_cfg_common;
    // RadioResourceConfigCommonSCell-r10::ul-Configuration-r10
    asn1cell.rr_cfg_common_scell_r10.ul_cfg_r10_present      = true;
    auto& ul_cfg                                             = asn1cell.rr_cfg_common_scell_r10.ul_cfg_r10;
    ul_cfg.ul_freq_info_r10.ul_carrier_freq_r10_present      = true;
    ul_cfg.ul_freq_info_r10.ul_carrier_freq_r10              = scell_cfg.cell_cfg.ul_earfcn;
    ul_cfg.p_max_r10_present                                 = cell_sib1.p_max_present;
    ul_cfg.p_max_r10                                         = cell_sib1.p_max;
    ul_cfg.ul_freq_info_r10.add_spec_emission_scell_r10      = 1;
    ul_cfg.ul_pwr_ctrl_common_scell_r10.p0_nominal_pusch_r10 = cc_cfg_sib.ul_pwr_ctrl_common.p0_nominal_pusch;
    ul_cfg.ul_pwr_ctrl_common_scell_r10.alpha_r10.value      = cc_cfg_sib.ul_pwr_ctrl_common.alpha;
    ul_cfg.srs_ul_cfg_common_r10                             = cc_cfg_sib.srs_ul_cfg_common;
    ul_cfg.ul_cp_len_r10.value                               = cc_cfg_sib.ul_cp_len.value;
    ul_cfg.pusch_cfg_common_r10                              = cc_cfg_sib.pusch_cfg_common;
    // RadioResourceConfigDedicatedSCell-r10
    asn1cell.rr_cfg_ded_scell_r10_present                                       = true;
    asn1cell.rr_cfg_ded_scell_r10.phys_cfg_ded_scell_r10_present                = true;
    asn1cell.rr_cfg_ded_scell_r10.phys_cfg_ded_scell_r10.non_ul_cfg_r10_present = true;
    auto& nonul_cfg_ded                = asn1cell.rr_cfg_ded_scell_r10.phys_cfg_ded_scell_r10.non_ul_cfg_r10;
    nonul_cfg_ded.ant_info_r10_present = true;
    asn1::number_to_enum(nonul_cfg_ded.ant_info_r10.tx_mode_r10, enb_cfg.cell.nof_ports);
    nonul_cfg_ded.ant_info_r10.ue_tx_ant_sel.set(setup_opts::release);
    nonul_cfg_ded.cross_carrier_sched_cfg_r10_present                                            = true;
    nonul_cfg_ded.cross_carrier_sched_cfg_r10.sched_cell_info_r10.set_own_r10().cif_presence_r10 = false;
    nonul_cfg_ded.pdsch_cfg_ded_r10_present                                                      = true;
    nonul_cfg_ded.pdsch_cfg_ded_r10.p_a.value                               = enb_cfg.pdsch_cfg.value;
    asn1cell.rr_cfg_ded_scell_r10.phys_cfg_ded_scell_r10.ul_cfg_r10_present = true;
    auto& ul_cfg_ded                                  = asn1cell.rr_cfg_ded_scell_r10.phys_cfg_ded_scell_r10.ul_cfg_r10;
    ul_cfg_ded.ant_info_ul_r10_present                = true;
    ul_cfg_ded.ant_info_ul_r10.tx_mode_ul_r10_present = true;
    asn1::number_to_enum(ul_cfg_ded.ant_info_ul_r10.tx_mode_ul_r10, enb_cfg.cell.nof_ports);
    ul_cfg_ded.pusch_cfg_ded_scell_r10_present           = true;
    ul_cfg_ded.ul_pwr_ctrl_ded_scell_r10_present         = true;
    ul_cfg_ded.ul_pwr_ctrl_ded_scell_r10.p0_ue_pusch_r10 = 0;
    ul_cfg_ded.ul_pwr_ctrl_ded_scell_r10.delta_mcs_enabled_r10.value =
        ul_pwr_ctrl_ded_scell_r10_s::delta_mcs_enabled_r10_opts::en0;
    ul_cfg_ded.ul_pwr_ctrl_ded_scell_r10.accumulation_enabled_r10   = true;
    ul_cfg_ded.ul_pwr_ctrl_ded_scell_r10.psrs_offset_ap_r10_present = true;
    ul_cfg_ded.ul_pwr_ctrl_ded_scell_r10.psrs_offset_ap_r10         = 3;
    ul_cfg_ded.ul_pwr_ctrl_ded_scell_r10.pathloss_ref_linking_r10.value =
        ul_pwr_ctrl_ded_scell_r10_s::pathloss_ref_linking_r10_opts::scell;
    ul_cfg_ded.cqi_report_cfg_scell_r10_present                               = true;
    ul_cfg_ded.cqi_report_cfg_scell_r10.nom_pdsch_rs_epre_offset_r10          = 0;
    ul_cfg_ded.cqi_report_cfg_scell_r10.cqi_report_periodic_scell_r10_present = true;
    if (ue_caps.support_dl_256qam) {
      asn1cell.rr_cfg_ded_scell_r10.phys_cfg_ded_scell_r10.ext = true;
      asn1cell.rr_cfg_ded_scell_r10.phys_cfg_ded_scell_r10.cqi_report_cfg_scell_v1250.set_present(true);
      asn1cell.rr_cfg_ded_scell_r10.phys_cfg_ded_scell_r10.cqi_report_cfg_scell_v1250->alt_cqi_table_r12_present = true;
      asn1cell.rr_cfg_ded_scell_r10.phys_cfg_ded_scell_r10.cqi_report_cfg_scell_v1250->alt_cqi_table_r12.value =
          cqi_report_cfg_v1250_s::alt_cqi_table_r12_opts::all_sfs;
    }

    // Get CQI allocation for secondary cell
    if (scell.cqi_res_present) {
      auto& cqi_setup                 = ul_cfg_ded.cqi_report_cfg_scell_r10.cqi_report_periodic_scell_r10.set_setup();
      cqi_setup.cqi_pmi_cfg_idx       = scell.cqi_res.pmi_idx;
      cqi_setup.cqi_pucch_res_idx_r10 = scell.cqi_res.pucch_res;
      cqi_setup.cqi_format_ind_periodic_r10.set_wideband_cqi_r10();
      cqi_setup.simul_ack_nack_and_cqi = enb_cfg.cqi_cfg.simultaneousAckCQI;
    }

#if SRS_ENABLED
    ul_cfg_ded.srs_ul_cfg_ded_r10_present   = true;
    auto& srs_setup                         = ul_cfg_ded.srs_ul_cfg_ded_r10.set_setup();
    srs_setup.srs_bw.value                  = srs_ul_cfg_ded_c::setup_s_::srs_bw_opts::bw0;
    srs_setup.srs_hop_bw.value              = srs_ul_cfg_ded_c::setup_s_::srs_hop_bw_opts::hbw0;
    srs_setup.freq_domain_position          = 0;
    srs_setup.dur                           = true;
    srs_setup.srs_cfg_idx                   = 167;
    srs_setup.tx_comb                       = 0;
    srs_setup.cyclic_shift.value            = srs_ul_cfg_ded_c::setup_s_::cyclic_shift_opts::cs0;
    ul_cfg_ded.srs_ul_cfg_ded_v1020_present = true;
    asn1::number_to_enum(ul_cfg_ded.srs_ul_cfg_ded_v1020.srs_ant_port_r10, enb_cfg.cell.nof_ports);
    ul_cfg_ded.srs_ul_cfg_ded_aperiodic_r10_present = true;
    ul_cfg_ded.srs_ul_cfg_ded_aperiodic_r10.set(setup_opts::release);
#endif // SRS_ENABLED
  }

  // Fill RRCConnectionReconf message
  recfg_r8.non_crit_ext_present                                                     = true;
  recfg_r8.non_crit_ext.non_crit_ext_present                                        = true;
  recfg_r8.non_crit_ext.non_crit_ext.non_crit_ext_present                           = true;
  recfg_r8.non_crit_ext.non_crit_ext.non_crit_ext.scell_to_add_mod_list_r10_present = true;
  auto& recfg_v1020 = recfg_r8.non_crit_ext.non_crit_ext.non_crit_ext;
  srsran::compute_cfg_diff(
      current_scells, target_scells, recfg_v1020.scell_to_add_mod_list_r10, recfg_v1020.scell_to_release_list_r10);

  // Set DL HARQ Feedback mode
  recfg_r8.rr_cfg_ded.phys_cfg_ded.pucch_cfg_ded_v1020.set_present(true);
  recfg_r8.rr_cfg_ded.phys_cfg_ded.pucch_cfg_ded_v1020->pucch_format_r10_present = true;
  recfg_r8.rr_cfg_ded.phys_cfg_ded.ext                                           = true;
  auto pucch_format_r10                      = recfg_r8.rr_cfg_ded.phys_cfg_ded.pucch_cfg_ded_v1020.get();
  pucch_format_r10->pucch_format_r10_present = true;
  if (ue_cell_list.nof_cells() <= 2) {
    // Use PUCCH format 1b with channel selection for 2 serving cells
    auto& ch_sel_r10                      = pucch_format_r10->pucch_format_r10.set_ch_sel_r10();
    ch_sel_r10.n1_pucch_an_cs_r10_present = true;
    ch_sel_r10.n1_pucch_an_cs_r10.set_setup();
    n1_pucch_an_cs_r10_l item0(4);
    // TODO: should we use a different n1PUCCH-AN-CS-List configuration?
    for (auto& it : item0) {
      it = ue_cell_list.is_pucch_cs_allocated() ? *ue_cell_list.get_n_pucch_cs() : 0;
    }
    ch_sel_r10.n1_pucch_an_cs_r10.setup().n1_pucch_an_cs_list_r10.push_back(item0);
  } else {
    // Use PUCCH format 3 for more than 2 serving cells
    auto& format3_r10                        = pucch_format_r10->pucch_format_r10.set_format3_r10();
    format3_r10.n3_pucch_an_list_r13_present = true;
    format3_r10.n3_pucch_an_list_r13.resize(4);
    for (auto& it : format3_r10.n3_pucch_an_list_r13) {
      // Hard-coded resource, only one user is supported
      it = 0;
    }
  }
}

void apply_scells_to_add_diff(asn1::rrc::scell_to_add_mod_list_r10_l& current_scells,
                              const rrc_conn_recfg_r8_ies_s&          recfg_r8)
{
  if (recfg_r8.non_crit_ext_present or recfg_r8.non_crit_ext.non_crit_ext_present or
      recfg_r8.non_crit_ext.non_crit_ext.non_crit_ext_present) {
    const rrc_conn_recfg_v1020_ies_s& recfg_v1020 = recfg_r8.non_crit_ext.non_crit_ext.non_crit_ext;
    srsran::apply_addmodremlist_diff(
        current_scells, recfg_v1020.scell_to_add_mod_list_r10, recfg_v1020.scell_to_release_list_r10, current_scells);
  }
}

/***********************************
 *        measConfig
 **********************************/

/// Apply Reconf updates and update current state
void apply_reconf_updates(asn1::rrc::rrc_conn_recfg_r8_ies_s&  recfg_r8,
                          ue_var_cfg_t&                        current_ue_cfg,
                          const rrc_cfg_t&                     enb_cfg,
                          const ue_cell_ded_list&              ue_cell_list,
                          bearer_cfg_handler&                  bearers,
                          const srsran::rrc_ue_capabilities_t& ue_caps,
                          bool                                 phy_cfg_updated)
{
  // Compute pending updates and fill reconf msg
  recfg_r8.rr_cfg_ded_present = true;
  fill_rr_cfg_ded_reconf(
      recfg_r8.rr_cfg_ded, current_ue_cfg.rr_cfg, enb_cfg, ue_cell_list, bearers, ue_caps, phy_cfg_updated);
  fill_scells_reconf(recfg_r8, current_ue_cfg.scells, enb_cfg, ue_cell_list, ue_caps);
  recfg_r8.meas_cfg_present |= recfg_r8.meas_cfg.meas_gap_cfg_present;

  // Add pending NAS info
  bearers.fill_pending_nas_info(&recfg_r8);

  // Update current rr_cfg_ded and scells state
  apply_rr_cfg_ded_diff(current_ue_cfg.rr_cfg, recfg_r8.rr_cfg_ded);
  apply_scells_to_add_diff(current_ue_cfg.scells, recfg_r8);
}

} // namespace srsenb
