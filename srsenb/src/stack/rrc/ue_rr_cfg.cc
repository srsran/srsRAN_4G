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

#include "srsenb/hdr/stack/rrc/ue_rr_cfg.h"
#include "srsenb/hdr/stack/rrc/rrc_bearer_cfg.h"
#include "srsenb/hdr/stack/rrc/rrc_cell_cfg.h"
#include "srsenb/hdr/stack/rrc/rrc_config.h"
#include "srslte/asn1/rrc_asn1_utils.h"
#include "srslte/rrc/rrc_cfg_utils.h"

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
    srslte::logmap::get("RRC")->error("Invalid SRB id=%d\n", srb_id);
    return nullptr;
  }

  // Set SRBtoAddMod
  auto srb_it               = srslte::add_rrc_obj_id(srbs, srb_id);
  srb_it->lc_ch_cfg_present = true;
  srb_it->lc_ch_cfg.set(srb_to_add_mod_s::lc_ch_cfg_c_::types_opts::default_value);
  srb_it->rlc_cfg_present = true;
  srb_it->rlc_cfg.set(srb_to_add_mod_s::rlc_cfg_c_::types_opts::default_value);

  return srb_it;
}

void fill_srbs_reconf(srb_to_add_mod_list_l& srbs, const srb_to_add_mod_list_l& current_srbs)
{
  // NOTE: In case of Handover, the Reconf includes SRB1
  if (not srslte::find_rrc_obj_id(current_srbs, 1)) {
    add_srb(srbs, 1);
  }

  if (not srslte::find_rrc_obj_id(current_srbs, 2)) {
    add_srb(srbs, 2);
  }
}

/******************************
 *        SR Config
 *****************************/

void fill_sr_cfg_setup(sched_request_cfg_c& sr_cfg, const cell_ctxt_dedicated_list& ue_cell_list)
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
      srslte::logmap::get("RRC")->error("Allocating RI: invalid m_ri=%d\n", m_ri);
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

int fill_cqi_report_setup(cqi_report_cfg_s&               cqi_rep,
                          const rrc_cfg_t&                enb_cfg,
                          const cell_ctxt_dedicated_list& ue_cell_list)
{
  // eNB params set at this point

  // For Periodic CQI, set PUCCH resources
  if (cqi_rep.cqi_report_periodic_present) {
    const cell_ctxt_dedicated* pcell = ue_cell_list.get_ue_cc_idx(UE_PCELL_CC_IDX);
    if (pcell == nullptr or not pcell->cqi_res_present) {
      srslte::logmap::get("RRC")->warning("PCell CQI resources haven\'t been allocated yet\n");
      return SRSLTE_ERROR;
    }
    auto& cqi_periodic             = cqi_rep.cqi_report_periodic.setup();
    cqi_periodic.cqi_pmi_cfg_idx   = pcell->cqi_res.pmi_idx;
    cqi_periodic.cqi_pucch_res_idx = pcell->cqi_res.pucch_res;
  }

  return SRSLTE_SUCCESS;
}

void fill_cqi_report_reconf(cqi_report_cfg_s&               cqi_rep,
                            const rrc_cfg_t&                enb_cfg,
                            const cell_ctxt_dedicated_list& ue_cell_list)
{
  // Get RRC setup CQI config
  if (fill_cqi_report_setup(cqi_rep, enb_cfg, ue_cell_list) == SRSLTE_ERROR) {
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
        srslte::logmap::get("RRC")->warning("Warning: Configured wrong M_ri parameter.\n");
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

/// Fills ASN1 PhysicalConfigurationDedicated struct with eNB config params at RRCReconf
void fill_phy_cfg_ded_enb_cfg_reconf(phys_cfg_ded_s&                 phy_cfg,
                                     const rrc_cfg_t&                enb_cfg,
                                     const cell_ctxt_dedicated_list& ue_cell_list)
{
  // Set PHYConfigDedicated base
  fill_phy_cfg_ded_enb_cfg(phy_cfg, enb_cfg);

  // Antenna Configuration
  ant_info_ded_s& ant_info = phy_cfg.ant_info.explicit_value();
  ant_info                 = enb_cfg.antenna_info;

  // CQI Report Config
  fill_cqi_report_reconf(phy_cfg.cqi_report_cfg, enb_cfg, ue_cell_list);
}

/***********************************
 *   radioResourceConfigDedicated
 **********************************/

/// Function to fill rrc asn1 radioResourceConfigDedicated with values provided in eNB config
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
  fill_phy_cfg_ded_enb_cfg(rr_cfg.phys_cfg_ded, enb_cfg);

  // Other RR params
  rr_cfg.rlf_timers_and_consts_r9.set_present(false);
  rr_cfg.sps_cfg_present = false;
}

void fill_rr_cfg_ded_setup(asn1::rrc::rr_cfg_ded_s&        rr_cfg,
                           const rrc_cfg_t&                enb_cfg,
                           const cell_ctxt_dedicated_list& ue_cell_list)
{
  // Establish default enb config
  fill_rr_cfg_ded_enb_cfg(rr_cfg, enb_cfg);

  // (Re)establish SRB1
  rr_cfg.srb_to_add_mod_list_present = true;
  add_srb(rr_cfg.srb_to_add_mod_list, 1);

  // Setup SR PUCCH config
  fill_sr_cfg_setup(rr_cfg.phys_cfg_ded.sched_request_cfg, ue_cell_list);

  // Setup CQI PUCCH config
  fill_cqi_report_setup(rr_cfg.phys_cfg_ded.cqi_report_cfg, enb_cfg, ue_cell_list);
}

enum class reconf_cause { setup, reest, crnti_ce, handover, other };

void fill_rr_cfg_ded_reconf(asn1::rrc::rr_cfg_ded_s&        rr_cfg,
                            const rr_cfg_ded_s&             current_rr_cfg,
                            const rrc_cfg_t&                enb_cfg,
                            const cell_ctxt_dedicated_list& ue_cell_list,
                            const bearer_cfg_handler&       bearers,
                            reconf_cause                    cause)
{
  // Establish SRBs if required
  fill_srbs_reconf(rr_cfg.srb_to_add_mod_list, current_rr_cfg.srb_to_add_mod_list);
  rr_cfg.srb_to_add_mod_list_present = rr_cfg.srb_to_add_mod_list.size() > 0;

  // Update DRBs if required
  srslte::compute_cfg_diff(current_rr_cfg.drb_to_add_mod_list,
                           bearers.get_established_drbs(),
                           rr_cfg.drb_to_add_mod_list,
                           rr_cfg.drb_to_release_list);
  rr_cfg.drb_to_add_mod_list_present = rr_cfg.drb_to_add_mod_list.size() > 0;
  rr_cfg.drb_to_release_list_present = rr_cfg.drb_to_release_list.size() > 0;

  // PhysCfgDed update needed
  if (cause != reconf_cause::other) {
    fill_phy_cfg_ded_enb_cfg_reconf(rr_cfg.phys_cfg_ded, enb_cfg, ue_cell_list);
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
    srslte::apply_addmodlist_diff(current_rr_cfg_ded.srb_to_add_mod_list,
                                  pending_rr_cfg_ded.srb_to_add_mod_list,
                                  current_rr_cfg_ded.srb_to_add_mod_list);
  }

  // 5.3.10.(2/3) - DRB release/addition/Modification
  if (pending_rr_cfg_ded.drb_to_add_mod_list_present or pending_rr_cfg_ded.drb_to_release_list_present) {
    srslte::apply_addmodremlist_diff(current_rr_cfg_ded.drb_to_add_mod_list,
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

} // namespace srsenb
