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

#include "srslte/asn1/rrc_asn1_utils.h"
#include "srslte/asn1/rrc_asn1.h"
#include "srslte/config.h"
#include <algorithm>
#include <srslte/common/interfaces_common.h>

namespace srslte {

using namespace asn1::rrc;

/***************************
 *        PLMN ID
 **************************/

bool plmn_is_valid(const asn1::rrc::plmn_id_s& asn1_type)
{
  return asn1_type.mcc_present and (asn1_type.mnc.size() == 3 or asn1_type.mnc.size() == 2);
}

plmn_id_t make_plmn_id_t(const asn1::rrc::plmn_id_s& asn1_type)
{
  if (not plmn_is_valid(asn1_type)) {
    return {};
  }
  plmn_id_t plmn;
  std::copy(&asn1_type.mcc[0], &asn1_type.mcc[3], &plmn.mcc[0]);
  plmn.nof_mnc_digits = asn1_type.mnc.size();
  std::copy(&asn1_type.mnc[0], &asn1_type.mnc[plmn.nof_mnc_digits], &plmn.mnc[0]);

  return plmn;
}

void to_asn1(asn1::rrc::plmn_id_s* asn1_type, const plmn_id_t& cfg)
{
  asn1_type->mcc_present = true;
  std::copy(&cfg.mcc[0], &cfg.mcc[3], &asn1_type->mcc[0]);
  asn1_type->mnc.resize(cfg.nof_mnc_digits);
  std::copy(&cfg.mnc[0], &cfg.mnc[cfg.nof_mnc_digits], &asn1_type->mnc[0]);
}

plmn_id_t make_plmn_id_t(const asn1::fixed_octstring<3, true>& asn1_type) // used for NGAP/S1AP
{
  plmn_id_t plmn;
  uint16_t  mcc, mnc;
  uint32_t  encoded_plmn = asn1_type.to_number();
  s1ap_plmn_to_mccmnc(encoded_plmn, &mcc, &mnc);
  plmn.from_number(mcc, mnc);
  return plmn;
}

void to_asn1(asn1::fixed_octstring<3, true>* asn1_type, const plmn_id_t& cfg)
{
  uint16_t mcc, mnc;
  cfg.to_number(&mcc, &mnc);
  uint32_t encoded_plmn;
  s1ap_mccmnc_to_plmn(mcc, mnc, &encoded_plmn);
  asn1_type->from_number(encoded_plmn);
}

/***************************
 *        s-TMSI
 **************************/

s_tmsi_t make_s_tmsi_t(const asn1::rrc::s_tmsi_s& asn1_type)
{
  s_tmsi_t tmsi;
  tmsi.mmec   = asn1_type.mmec.to_number();
  tmsi.m_tmsi = asn1_type.m_tmsi.to_number();
  return tmsi;
}

void to_asn1(asn1::rrc::s_tmsi_s* asn1_type, const s_tmsi_t& cfg)
{
  asn1_type->mmec.from_number(cfg.mmec);
  asn1_type->m_tmsi.from_number(cfg.m_tmsi);
}

/***************************
 *        CQI Report Aperiodic
 **************************/

srslte_cqi_report_mode_t make_aperiodic_mode(const asn1::rrc::cqi_report_mode_aperiodic_e asn_mode)
{
  switch (asn_mode) {
    case asn1::rrc::cqi_report_mode_aperiodic_e::rm12:
      return SRSLTE_CQI_MODE_12;
    case asn1::rrc::cqi_report_mode_aperiodic_e::rm20:
      return SRSLTE_CQI_MODE_20;
    case asn1::rrc::cqi_report_mode_aperiodic_e::rm22:
      return SRSLTE_CQI_MODE_22;
    case asn1::rrc::cqi_report_mode_aperiodic_e::rm30:
      return SRSLTE_CQI_MODE_30;
    case asn1::rrc::cqi_report_mode_aperiodic_e::rm31:
      return SRSLTE_CQI_MODE_31;
    case asn1::rrc::cqi_report_mode_aperiodic_e::rm10_v1310:
    case asn1::rrc::cqi_report_mode_aperiodic_e::rm11_v1310:
    case asn1::rrc::cqi_report_mode_aperiodic_e::rm32_v1250:
      fprintf(stderr, "Aperiodic mode %s not handled\n", asn_mode.to_string().c_str());
    default:
      return SRSLTE_CQI_MODE_NA;
  }
}

/***************************
 *   Establishment Cause
 **************************/

static_assert((int)establishment_cause_t::nulltype == (int)asn1::rrc::establishment_cause_opts::nulltype,
              "Failed conversion of ASN1 type");

/***************************
 *      RLC Config
 **************************/

srslte::rlc_config_t make_rlc_config_t(const asn1::rrc::rlc_cfg_c& asn1_type)
{
  srslte::rlc_config_t rlc_cfg;
  switch (asn1_type.type().value) {
    case asn1::rrc::rlc_cfg_c::types_opts::am:
      rlc_cfg.rlc_mode             = rlc_mode_t::am;
      rlc_cfg.am.t_poll_retx       = asn1_type.am().ul_am_rlc.t_poll_retx.to_number();
      rlc_cfg.am.poll_pdu          = asn1_type.am().ul_am_rlc.poll_pdu.to_number();
      rlc_cfg.am.poll_byte         = asn1_type.am().ul_am_rlc.poll_byte.to_number() * 1000; // KB
      rlc_cfg.am.max_retx_thresh   = asn1_type.am().ul_am_rlc.max_retx_thres.to_number();
      rlc_cfg.am.t_reordering      = asn1_type.am().dl_am_rlc.t_reordering.to_number();
      rlc_cfg.am.t_status_prohibit = asn1_type.am().dl_am_rlc.t_status_prohibit.to_number();
      break;
    case asn1::rrc::rlc_cfg_c::types_opts::um_bi_dir:
    case asn1::rrc::rlc_cfg_c::types_opts::um_uni_dir_dl:
    case asn1::rrc::rlc_cfg_c::types_opts::um_uni_dir_ul:
      rlc_cfg.rlc_mode              = rlc_mode_t::um;
      rlc_cfg.um.t_reordering       = asn1_type.um_bi_dir().dl_um_rlc.t_reordering.to_number();
      rlc_cfg.um.rx_sn_field_length = (rlc_umd_sn_size_t)asn1_type.um_bi_dir().dl_um_rlc.sn_field_len.value;
      rlc_cfg.um.rx_window_size     = (rlc_cfg.um.rx_sn_field_length == rlc_umd_sn_size_t::size5bits) ? 16 : 512;
      rlc_cfg.um.rx_mod             = (rlc_cfg.um.rx_sn_field_length == rlc_umd_sn_size_t::size5bits) ? 32 : 1024;
      rlc_cfg.um.tx_sn_field_length = (rlc_umd_sn_size_t)asn1_type.um_bi_dir().ul_um_rlc.sn_field_len.value;
      rlc_cfg.um.tx_mod             = (rlc_cfg.um.tx_sn_field_length == rlc_umd_sn_size_t::size5bits) ? 32 : 1024;
      break;
    default:
      break;
  }
  return rlc_cfg;
}

srslte::rlc_config_t make_rlc_config_t(const asn1::rrc::srb_to_add_mod_s& asn1_type)
{
  if (asn1_type.rlc_cfg.type().value == asn1::rrc::srb_to_add_mod_s::rlc_cfg_c_::types_opts::explicit_value) {
    return make_rlc_config_t(asn1_type.rlc_cfg.explicit_value());
  }
  if (asn1_type.srb_id <= 2) {
    return rlc_config_t::srb_config(asn1_type.srb_id);
  } else {
    asn1::log_error("SRB %d does not support default initialization type\n", asn1_type.srb_id);
    return rlc_config_t();
  }
}

void to_asn1(asn1::rrc::rlc_cfg_c* asn1_type, const srslte::rlc_config_t& cfg)
{
  *asn1_type = asn1::rrc::rlc_cfg_c();
  switch (cfg.rlc_mode) {
    case rlc_mode_t::um:
      asn1_type->set_um_bi_dir();
      asn1::number_to_enum(asn1_type->um_bi_dir().dl_um_rlc.t_reordering, cfg.um.t_reordering);
      asn1_type->um_bi_dir().dl_um_rlc.sn_field_len.value =
          (asn1::rrc::sn_field_len_opts::options)cfg.um.rx_sn_field_length;
      asn1_type->um_bi_dir().ul_um_rlc.sn_field_len.value =
          (asn1::rrc::sn_field_len_opts::options)cfg.um.tx_sn_field_length;
      break;
    case rlc_mode_t::am:
      asn1_type->set_am();
      asn1::number_to_enum(asn1_type->am().ul_am_rlc.t_poll_retx, cfg.am.t_poll_retx);
      asn1::number_to_enum(asn1_type->am().ul_am_rlc.poll_pdu, cfg.am.poll_pdu);
      asn1::number_to_enum(asn1_type->am().ul_am_rlc.poll_byte, cfg.am.poll_byte / 1000);
      asn1::number_to_enum(asn1_type->am().ul_am_rlc.max_retx_thres, cfg.am.max_retx_thresh);
      asn1::number_to_enum(asn1_type->am().dl_am_rlc.t_reordering, cfg.am.t_reordering);
      asn1::number_to_enum(asn1_type->am().dl_am_rlc.t_status_prohibit, cfg.am.t_status_prohibit);
      break;
    default:
      // stays TM
      break;
  }
}

/**
 * PDCP Config
 */

srslte::pdcp_config_t make_srb_pdcp_config_t(const uint8_t bearer_id, bool is_ue)
{
  pdcp_config_t cfg(bearer_id,
                    PDCP_RB_IS_SRB,
                    is_ue ? SECURITY_DIRECTION_UPLINK : SECURITY_DIRECTION_DOWNLINK,
                    is_ue ? SECURITY_DIRECTION_DOWNLINK : SECURITY_DIRECTION_UPLINK,
                    PDCP_SN_LEN_5,
                    pdcp_t_reordering_t::ms500,
                    pdcp_discard_timer_t::infinity);
  return cfg;
}

srslte::pdcp_config_t make_drb_pdcp_config_t(const uint8_t bearer_id, bool is_ue)
{
  pdcp_config_t cfg(bearer_id,
                    PDCP_RB_IS_DRB,
                    is_ue ? SECURITY_DIRECTION_UPLINK : SECURITY_DIRECTION_DOWNLINK,
                    is_ue ? SECURITY_DIRECTION_DOWNLINK : SECURITY_DIRECTION_UPLINK,
                    PDCP_SN_LEN_12,
                    pdcp_t_reordering_t::ms500,
                    pdcp_discard_timer_t::infinity);
  return cfg;
}

srslte::pdcp_config_t make_drb_pdcp_config_t(const uint8_t bearer_id, bool is_ue, const asn1::rrc::pdcp_cfg_s& pdcp_cfg)
{
  // TODO: complete config processing
  pdcp_discard_timer_t discard_timer = pdcp_discard_timer_t::infinity;
  if (pdcp_cfg.discard_timer_present) {
    switch (pdcp_cfg.discard_timer.to_number()) {
      case 10:
        discard_timer = pdcp_discard_timer_t::ms10;
        break;
      default:
        discard_timer = pdcp_discard_timer_t::infinity;
        break;
    }
  }

  pdcp_t_reordering_t t_reordering = pdcp_t_reordering_t::ms500;
  if (pdcp_cfg.t_reordering_r12_present) {
    switch (pdcp_cfg.t_reordering_r12.to_number()) {
      case 0:
        t_reordering = pdcp_t_reordering_t::ms0;
        break;
      default:
        t_reordering = pdcp_t_reordering_t::ms500;
    }
  }

  uint8_t sn_len = srslte::PDCP_SN_LEN_12;
  if (pdcp_cfg.rlc_um_present) {
    if (pdcp_cfg.rlc_um.pdcp_sn_size.value == pdcp_cfg_s::rlc_um_s_::pdcp_sn_size_e_::len7bits) {
      sn_len = srslte::PDCP_SN_LEN_7;
    }
  }

  if (pdcp_cfg.rlc_am_present) {
    // TODO: handle RLC AM config for PDCP
  }

  pdcp_config_t cfg(bearer_id,
                    PDCP_RB_IS_DRB,
                    is_ue ? SECURITY_DIRECTION_UPLINK : SECURITY_DIRECTION_DOWNLINK,
                    is_ue ? SECURITY_DIRECTION_DOWNLINK : SECURITY_DIRECTION_UPLINK,
                    sn_len,
                    t_reordering,
                    discard_timer);
  return cfg;
}

/***************************
 *      MAC Config
 **************************/

void set_mac_cfg_t_sched_request_cfg(mac_cfg_t* cfg, const asn1::rrc::sched_request_cfg_c& asn1_type)
{
  cfg->sr_cfg.enabled = asn1_type.type() == asn1::rrc::setup_e::setup;
  if (cfg->sr_cfg.enabled) {
    cfg->sr_cfg.dsr_transmax = asn1_type.setup().dsr_trans_max.to_number();
  }
}

// MAC-MainConfig section is always present
void set_mac_cfg_t_main_cfg(mac_cfg_t* cfg, const asn1::rrc::mac_main_cfg_s& asn1_type)
{
  // Update values only if each section is present
  if (asn1_type.phr_cfg_present) {
    cfg->phr_cfg.enabled = asn1_type.phr_cfg.type() == asn1::rrc::setup_e::setup;
    if (cfg->phr_cfg.enabled) {
      cfg->phr_cfg.prohibit_timer     = asn1_type.phr_cfg.setup().prohibit_phr_timer.to_number();
      cfg->phr_cfg.periodic_timer     = asn1_type.phr_cfg.setup().periodic_phr_timer.to_number();
      cfg->phr_cfg.db_pathloss_change = asn1_type.phr_cfg.setup().dl_pathloss_change.to_number();
    }
  }
  if (asn1_type.mac_main_cfg_v1020.is_present()) {
    cfg->phr_cfg.extended = asn1_type.mac_main_cfg_v1020.get()->extended_phr_r10_present;
  }
  if (asn1_type.ul_sch_cfg_present) {
    cfg->bsr_cfg.periodic_timer = asn1_type.ul_sch_cfg.periodic_bsr_timer.to_number();
    cfg->bsr_cfg.retx_timer     = asn1_type.ul_sch_cfg.retx_bsr_timer.to_number();
    if (asn1_type.ul_sch_cfg.max_harq_tx_present) {
      cfg->harq_cfg.max_harq_tx = asn1_type.ul_sch_cfg.max_harq_tx.to_number();
    }
  }
  // TimeAlignmentDedicated overwrites Common??
  cfg->time_alignment_timer = asn1_type.time_align_timer_ded.to_number();
}

// RACH-Common section is always present
void set_mac_cfg_t_rach_cfg_common(mac_cfg_t* cfg, const asn1::rrc::rach_cfg_common_s& asn1_type)
{

  // Preamble info
  cfg->rach_cfg.nof_preambles = asn1_type.preamb_info.nof_ra_preambs.to_number();
  if (asn1_type.preamb_info.preambs_group_a_cfg_present) {
    cfg->rach_cfg.nof_groupA_preambles =
        asn1_type.preamb_info.preambs_group_a_cfg.size_of_ra_preambs_group_a.to_number();
    cfg->rach_cfg.messageSizeGroupA = asn1_type.preamb_info.preambs_group_a_cfg.msg_size_group_a.to_number();
    cfg->rach_cfg.messagePowerOffsetGroupB =
        asn1_type.preamb_info.preambs_group_a_cfg.msg_pwr_offset_group_b.to_number();
  } else {
    cfg->rach_cfg.nof_groupA_preambles = 0;
  }

  // Power ramping
  cfg->rach_cfg.powerRampingStep       = asn1_type.pwr_ramp_params.pwr_ramp_step.to_number();
  cfg->rach_cfg.iniReceivedTargetPower = asn1_type.pwr_ramp_params.preamb_init_rx_target_pwr.to_number();

  // Supervision info
  cfg->rach_cfg.preambleTransMax          = asn1_type.ra_supervision_info.preamb_trans_max.to_number();
  cfg->rach_cfg.responseWindowSize        = asn1_type.ra_supervision_info.ra_resp_win_size.to_number();
  cfg->rach_cfg.contentionResolutionTimer = asn1_type.ra_supervision_info.mac_contention_resolution_timer.to_number();

  // HARQ Msg3
  cfg->harq_cfg.max_harq_msg3_tx = asn1_type.max_harq_msg3_tx;
}

void set_mac_cfg_t_time_alignment(mac_cfg_t* cfg, const asn1::rrc::time_align_timer_opts asn1_type)
{
  cfg->time_alignment_timer = asn1_type.to_number();
}

srsenb::sched_interface::ant_info_ded_t make_ant_info_ded(const asn1::rrc::ant_info_ded_s& asn1_type)
{
  srsenb::sched_interface::ant_info_ded_t ant_ded = {};
  ant_ded.tx_mode = static_cast<srsenb::sched_interface::ant_info_ded_t::tx_mode_t>(asn1_type.tx_mode.value);
  ant_ded.cookbook_subset_type = srsenb::sched_interface::ant_info_ded_t::codebook_t::none;
  if (asn1_type.codebook_subset_restrict_present) {
    auto& asn1code = asn1_type.codebook_subset_restrict;
    ant_ded.cookbook_subset_type =
        static_cast<srsenb::sched_interface::ant_info_ded_t::codebook_t>(asn1code.type().value);
    switch (asn1code.type().value) {
      case ant_info_ded_s::codebook_subset_restrict_c_::types_opts::n2_tx_ant_tm3:
        ant_ded.codebook_subset_restrict = asn1code.n2_tx_ant_tm3().to_number();
        break;
      case ant_info_ded_s::codebook_subset_restrict_c_::types_opts::n4_tx_ant_tm3:
        ant_ded.codebook_subset_restrict = asn1code.n4_tx_ant_tm3().to_number();
        break;
      case ant_info_ded_s::codebook_subset_restrict_c_::types_opts::n2_tx_ant_tm4:
        ant_ded.codebook_subset_restrict = asn1code.n2_tx_ant_tm4().to_number();
        break;
      case ant_info_ded_s::codebook_subset_restrict_c_::types_opts::n4_tx_ant_tm4:
        ant_ded.codebook_subset_restrict = asn1code.n4_tx_ant_tm4().to_number();
        break;
      case ant_info_ded_s::codebook_subset_restrict_c_::types_opts::n2_tx_ant_tm5:
        ant_ded.codebook_subset_restrict = asn1code.n2_tx_ant_tm5().to_number();
        break;
      case ant_info_ded_s::codebook_subset_restrict_c_::types_opts::n4_tx_ant_tm5:
        ant_ded.codebook_subset_restrict = asn1code.n4_tx_ant_tm5().to_number();
        break;
      case ant_info_ded_s::codebook_subset_restrict_c_::types_opts::n2_tx_ant_tm6:
        ant_ded.codebook_subset_restrict = asn1code.n2_tx_ant_tm6().to_number();
        break;
      case ant_info_ded_s::codebook_subset_restrict_c_::types_opts::n4_tx_ant_tm6:
        ant_ded.codebook_subset_restrict = asn1code.n4_tx_ant_tm6().to_number();
        break;
      default:
        asn1::log_error("Failed to convert antenna codebook type to number\n");
    }
  }
  ant_ded.ue_tx_ant_sel = srsenb::sched_interface::ant_info_ded_t::ue_tx_ant_sel_t::release;
  if (asn1_type.ue_tx_ant_sel.type().value == setup_opts::setup) {
    ant_ded.ue_tx_ant_sel =
        (asn1_type.ue_tx_ant_sel.setup().value == ant_info_ded_s::ue_tx_ant_sel_c_::setup_opts::closed_loop)
            ? srsenb::sched_interface::ant_info_ded_t::ue_tx_ant_sel_t::closed_loop
            : srsenb::sched_interface::ant_info_ded_t::ue_tx_ant_sel_t::open_loop;
  }
  return ant_ded;
}

void set_phy_cfg_t_dedicated_cfg(phy_cfg_t* cfg, const asn1::rrc::phys_cfg_ded_s& asn1_type)
{
  if (asn1_type.pucch_cfg_ded_present) {

    if (asn1_type.pucch_cfg_ded.tdd_ack_nack_feedback_mode_present) {
      cfg->ul_cfg.pucch.tdd_ack_multiplex = asn1_type.pucch_cfg_ded.tdd_ack_nack_feedback_mode ==
                                            asn1::rrc::pucch_cfg_ded_s::tdd_ack_nack_feedback_mode_e_::mux;
    } else {
      cfg->ul_cfg.pucch.tdd_ack_multiplex = false;
    }
  }

  if (asn1_type.pucch_cfg_ded_v1020.is_present()) {
    auto* pucch_cfg_ded = asn1_type.pucch_cfg_ded_v1020.get();

    if (pucch_cfg_ded->pucch_format_r10_present) {

      typedef asn1::rrc::pucch_cfg_ded_v1020_s::pucch_format_r10_c_ pucch_format_r10_t;
      auto*                                                         pucch_format_r10 = &pucch_cfg_ded->pucch_format_r10;

      if (pucch_format_r10->type() == pucch_format_r10_t::types::format3_r10) {
        // Select feedback mode
        cfg->ul_cfg.pucch.ack_nack_feedback_mode = SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_PUCCH3;

        auto* format3_r13 = &pucch_format_r10->format3_r10();
        for (uint32_t n = 0; n < SRSLTE_MIN(format3_r13->n3_pucch_an_list_r13.size(), SRSLTE_PUCCH_SIZE_AN_CS); n++) {
          cfg->ul_cfg.pucch.n3_pucch_an_list[n] = format3_r13->n3_pucch_an_list_r13[n];
        }
        if (format3_r13->two_ant_port_activ_pucch_format3_r13_present) {
          if (format3_r13->two_ant_port_activ_pucch_format3_r13.type() == asn1::rrc::setup_e::setup) {
            // TODO: UL MIMO Configure PUCCH two antenna port
          } else {
            // TODO: UL MIMO Disable two antenna port
          }
        }
      } else if (pucch_format_r10->type() == asn1::rrc::pucch_cfg_ded_v1020_s::pucch_format_r10_c_::types::ch_sel_r10) {

        typedef pucch_format_r10_t::ch_sel_r10_s_ ch_sel_r10_t;
        auto*                                     ch_sel_r10 = &pucch_format_r10->ch_sel_r10();

        if (ch_sel_r10->n1_pucch_an_cs_r10_present) {
          typedef ch_sel_r10_t::n1_pucch_an_cs_r10_c_ n1_pucch_an_cs_r10_t;
          auto*                                       n1_pucch_an_cs_r10 = &ch_sel_r10->n1_pucch_an_cs_r10;

          if (n1_pucch_an_cs_r10->type() == asn1::rrc::setup_e::setup) {
            // Select feedback mode
            cfg->ul_cfg.pucch.ack_nack_feedback_mode = SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_CS;

            typedef n1_pucch_an_cs_r10_t::setup_s_::n1_pucch_an_cs_list_r10_l_ n1_pucch_an_cs_list_r10_t;
            n1_pucch_an_cs_list_r10_t                                          n1_pucch_an_cs_list =
                ch_sel_r10->n1_pucch_an_cs_r10.setup().n1_pucch_an_cs_list_r10;
            for (uint32_t i = 0; i < SRSLTE_MIN(n1_pucch_an_cs_list.size(), SRSLTE_PUCCH_NOF_AN_CS); i++) {
              asn1::rrc::n1_pucch_an_cs_r10_l n1_pucch_an_cs = n1_pucch_an_cs_list[i];
              for (uint32_t j = 0; j < SRSLTE_PUCCH_SIZE_AN_CS; j++) {
                cfg->ul_cfg.pucch.n1_pucch_an_cs[j][i] = n1_pucch_an_cs[j];
              }
            }
          } else {
            cfg->ul_cfg.pucch.ack_nack_feedback_mode = SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_NORMAL;
          }
        }
      } else {
        // Do nothing
      }
    }
  }

  if (asn1_type.pusch_cfg_ded_present) {
    cfg->ul_cfg.pusch.uci_offset.I_offset_ack = asn1_type.pusch_cfg_ded.beta_offset_ack_idx;
    cfg->ul_cfg.pusch.uci_offset.I_offset_cqi = asn1_type.pusch_cfg_ded.beta_offset_cqi_idx;
    cfg->ul_cfg.pusch.uci_offset.I_offset_ri  = asn1_type.pusch_cfg_ded.beta_offset_ri_idx;
  }

  if (asn1_type.ul_pwr_ctrl_ded_present) {
    cfg->ul_cfg.power_ctrl.p0_ue_pusch = asn1_type.ul_pwr_ctrl_ded.p0_ue_pusch;
    cfg->ul_cfg.power_ctrl.delta_mcs_based =
        asn1_type.ul_pwr_ctrl_ded.delta_mcs_enabled == asn1::rrc::ul_pwr_ctrl_ded_s::delta_mcs_enabled_e_::en0;
    cfg->ul_cfg.power_ctrl.acc_enabled  = asn1_type.ul_pwr_ctrl_ded.accumulation_enabled;
    cfg->ul_cfg.power_ctrl.p0_ue_pucch  = asn1_type.ul_pwr_ctrl_ded.p0_ue_pucch;
    cfg->ul_cfg.power_ctrl.p_srs_offset = asn1_type.ul_pwr_ctrl_ded.psrs_offset;
  }

  if (asn1_type.ul_pwr_ctrl_ded.filt_coef_present) {
    // TODO
  }

  if (asn1_type.tpc_pdcch_cfg_pucch_present) {
    // TODO
  }

  if (asn1_type.tpc_pdcch_cfg_pusch_present) {
    // TODO
  }

  if (asn1_type.cqi_report_cfg_present) {

    if (asn1_type.cqi_report_cfg.cqi_report_periodic_present) {

      cfg->dl_cfg.cqi_report.periodic_configured =
          asn1_type.cqi_report_cfg.cqi_report_periodic.type() == asn1::rrc::setup_e::setup;
      if (cfg->dl_cfg.cqi_report.periodic_configured) {
        cfg->ul_cfg.pucch.n_pucch_2     = asn1_type.cqi_report_cfg.cqi_report_periodic.setup().cqi_pucch_res_idx;
        cfg->ul_cfg.pucch.simul_cqi_ack = asn1_type.cqi_report_cfg.cqi_report_periodic.setup().simul_ack_nack_and_cqi;

        cfg->dl_cfg.cqi_report.pmi_idx = asn1_type.cqi_report_cfg.cqi_report_periodic.setup().cqi_pmi_cfg_idx;
        cfg->dl_cfg.cqi_report.format_is_subband =
            asn1_type.cqi_report_cfg.cqi_report_periodic.setup().cqi_format_ind_periodic.type().value ==
            asn1::rrc::cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_::types::subband_cqi;
        if (cfg->dl_cfg.cqi_report.format_is_subband) {
          cfg->dl_cfg.cqi_report.subband_size =
              asn1_type.cqi_report_cfg.cqi_report_periodic.setup().cqi_format_ind_periodic.subband_cqi().k;
        }
        if (asn1_type.cqi_report_cfg.cqi_report_periodic.setup().ri_cfg_idx_present) {
          cfg->dl_cfg.cqi_report.ri_idx         = asn1_type.cqi_report_cfg.cqi_report_periodic.setup().ri_cfg_idx;
          cfg->dl_cfg.cqi_report.ri_idx_present = true;
        } else {
          cfg->dl_cfg.cqi_report.ri_idx_present = false;
        }
      } else {
        cfg->ul_cfg.pucch.n_pucch_2     = 0;
        cfg->ul_cfg.pucch.simul_cqi_ack = false;
      }
    }

    if (asn1_type.cqi_report_cfg.cqi_report_mode_aperiodic_present) {

      cfg->dl_cfg.cqi_report.aperiodic_configured = true;
      cfg->dl_cfg.cqi_report.aperiodic_mode = make_aperiodic_mode(asn1_type.cqi_report_cfg.cqi_report_mode_aperiodic);
    }
  }

  if (asn1_type.cqi_report_cfg_pcell_v1250.is_present()) {
    auto cqi_report_cfg_pcell_v1250 = asn1_type.cqi_report_cfg_pcell_v1250.get();
    if (cqi_report_cfg_pcell_v1250->alt_cqi_table_r12_present) {
      cfg->dl_cfg.pdsch.use_tbs_index_alt = true;
    }
  } else {
    cfg->dl_cfg.pdsch.use_tbs_index_alt = false;
  }

  if (asn1_type.srs_ul_cfg_ded_present) {
    cfg->ul_cfg.srs.dedicated_enabled = asn1_type.srs_ul_cfg_ded.type() == asn1::rrc::setup_e::setup;
    if (cfg->ul_cfg.srs.dedicated_enabled) {
      cfg->ul_cfg.srs.configured = cfg->ul_cfg.srs.dedicated_enabled and cfg->ul_cfg.srs.common_enabled;
      cfg->ul_cfg.srs.I_srs      = asn1_type.srs_ul_cfg_ded.setup().srs_cfg_idx;
      cfg->ul_cfg.srs.B          = asn1_type.srs_ul_cfg_ded.setup().srs_bw;
      cfg->ul_cfg.srs.b_hop      = asn1_type.srs_ul_cfg_ded.setup().srs_hop_bw;
      cfg->ul_cfg.srs.n_rrc      = asn1_type.srs_ul_cfg_ded.setup().freq_domain_position;
      cfg->ul_cfg.srs.k_tc       = asn1_type.srs_ul_cfg_ded.setup().tx_comb;
      cfg->ul_cfg.srs.n_srs      = asn1_type.srs_ul_cfg_ded.setup().cyclic_shift;
    }
  }

  if (asn1_type.ant_info_r10.is_present() &&
      asn1_type.ant_info_r10->type() == asn1::rrc::phys_cfg_ded_s::ant_info_r10_c_::types::explicit_value_r10) {
    // Parse Release 10
    asn1::rrc::ant_info_ded_r10_s::tx_mode_r10_e_::options tx_mode =
        asn1_type.ant_info_r10->explicit_value_r10().tx_mode_r10.value;
    if ((srslte_tm_t)tx_mode < SRSLTE_TMINV) {
      cfg->dl_cfg.tm = (srslte_tm_t)tx_mode;
    } else {
      fprintf(stderr,
              "Transmission mode (R10) %s is not supported\n",
              asn1_type.ant_info_r10->explicit_value_r10().tx_mode_r10.to_string().c_str());
    }
  } else if (asn1_type.ant_info_present &&
             asn1_type.ant_info.type() == asn1::rrc::phys_cfg_ded_s::ant_info_c_::types::explicit_value) {
    // Parse Release 8
    asn1::rrc::ant_info_ded_s::tx_mode_e_::options tx_mode = asn1_type.ant_info.explicit_value().tx_mode.value;
    if ((srslte_tm_t)tx_mode < SRSLTE_TMINV) {
      cfg->dl_cfg.tm = (srslte_tm_t)tx_mode;
    } else {
      fprintf(stderr,
              "Transmission mode (R8) %s is not supported\n",
              asn1_type.ant_info.explicit_value().tx_mode.to_string().c_str());
    }
  }
  if (asn1_type.sched_request_cfg_present) {
    if (asn1_type.sched_request_cfg_present and asn1_type.sched_request_cfg.type() == asn1::rrc::setup_e::setup) {
      cfg->ul_cfg.pucch.I_sr          = asn1_type.sched_request_cfg.setup().sr_cfg_idx;
      cfg->ul_cfg.pucch.n_pucch_sr    = asn1_type.sched_request_cfg.setup().sr_pucch_res_idx;
      cfg->ul_cfg.pucch.sr_configured = true;
    } else {
      cfg->ul_cfg.pucch.I_sr          = 0;
      cfg->ul_cfg.pucch.n_pucch_sr    = 0;
      cfg->ul_cfg.pucch.sr_configured = false;
    }
  }

  if (asn1_type.pdsch_cfg_ded_present) {
    // Configure PDSCH
    if (asn1_type.pdsch_cfg_ded_present && cfg->dl_cfg.pdsch.p_b < 4) {
      cfg->dl_cfg.pdsch.p_a         = asn1_type.pdsch_cfg_ded.p_a.to_number();
      cfg->dl_cfg.pdsch.power_scale = true;
    } else {
      cfg->dl_cfg.pdsch.power_scale = false;
    }
  }
}

void set_phy_cfg_t_common_prach(phy_cfg_t* cfg, const asn1::rrc::prach_cfg_info_s* asn1_type, uint32_t root_seq_idx)
{
  if (asn1_type) {
    cfg->prach_cfg.config_idx     = asn1_type->prach_cfg_idx;
    cfg->prach_cfg.zero_corr_zone = asn1_type->zero_correlation_zone_cfg;
    cfg->prach_cfg.freq_offset    = asn1_type->prach_freq_offset;
    cfg->prach_cfg.hs_flag        = asn1_type->high_speed_flag;
  }
  cfg->prach_cfg.root_seq_idx = root_seq_idx;
  cfg->prach_cfg_present      = true;
}

void set_phy_cfg_t_common_pdsch(phy_cfg_t* cfg, const asn1::rrc::pdsch_cfg_common_s& asn1_type)
{
  cfg->dl_cfg.pdsch.rs_power = (float)asn1_type.ref_sig_pwr;
  cfg->dl_cfg.pdsch.p_b      = asn1_type.p_b;
}

void set_phy_cfg_t_enable_64qam(phy_cfg_t* cfg, const bool enabled)
{
  cfg->ul_cfg.pusch.enable_64qam = enabled;
}

void set_phy_cfg_t_common_pusch(phy_cfg_t* cfg, const asn1::rrc::pusch_cfg_common_s& asn1_type)
{
  /* PUSCH DMRS signal configuration */
  bzero(&cfg->ul_cfg.dmrs, sizeof(srslte_refsignal_dmrs_pusch_cfg_t));
  cfg->ul_cfg.dmrs.group_hopping_en    = asn1_type.ul_ref_sigs_pusch.group_hop_enabled;
  cfg->ul_cfg.dmrs.sequence_hopping_en = asn1_type.ul_ref_sigs_pusch.seq_hop_enabled;
  cfg->ul_cfg.dmrs.cyclic_shift        = asn1_type.ul_ref_sigs_pusch.cyclic_shift;
  cfg->ul_cfg.dmrs.delta_ss            = asn1_type.ul_ref_sigs_pusch.group_assign_pusch;

  /* PUSCH Hopping configuration */
  bzero(&cfg->ul_cfg.hopping, sizeof(srslte_pusch_hopping_cfg_t));
  cfg->ul_cfg.hopping.n_sb           = asn1_type.pusch_cfg_basic.n_sb;
  cfg->ul_cfg.hopping.hopping_offset = asn1_type.pusch_cfg_basic.pusch_hop_offset;
  cfg->ul_cfg.hopping.hop_mode =
      asn1_type.pusch_cfg_basic.hop_mode.value ==
              asn1::rrc::pusch_cfg_common_s::pusch_cfg_basic_s_::hop_mode_e_::intra_and_inter_sub_frame
          ? cfg->ul_cfg.hopping.SRSLTE_PUSCH_HOP_MODE_INTRA_SF
          : cfg->ul_cfg.hopping.SRSLTE_PUSCH_HOP_MODE_INTER_SF;
}

void set_phy_cfg_t_common_pucch(phy_cfg_t* cfg, const asn1::rrc::pucch_cfg_common_s& asn1_type)
{
  /* PUCCH configuration */
  cfg->ul_cfg.pucch.delta_pucch_shift = asn1_type.delta_pucch_shift.to_number();
  cfg->ul_cfg.pucch.N_cs              = asn1_type.ncs_an;
  cfg->ul_cfg.pucch.n_rb_2            = asn1_type.nrb_cqi;
  cfg->ul_cfg.pucch.N_pucch_1         = asn1_type.n1_pucch_an;
}

void set_phy_cfg_t_common_srs(phy_cfg_t* cfg, const asn1::rrc::srs_ul_cfg_common_c& asn1_type)
{
  cfg->ul_cfg.srs.common_enabled = asn1_type.type() == asn1::rrc::setup_e::setup;
  if (cfg->ul_cfg.srs.common_enabled) {
    cfg->ul_cfg.srs.simul_ack       = asn1_type.setup().ack_nack_srs_simul_tx;
    cfg->ul_cfg.srs.bw_cfg          = asn1_type.setup().srs_bw_cfg.to_number();
    cfg->ul_cfg.srs.subframe_config = asn1_type.setup().srs_sf_cfg.to_number();
  }
}

void set_phy_cfg_t_common_pwr_ctrl(phy_cfg_t* cfg, const asn1::rrc::ul_pwr_ctrl_common_s& asn1_type)
{
  cfg->ul_cfg.power_ctrl.p0_nominal_pusch = asn1_type.p0_nominal_pusch;
  cfg->ul_cfg.power_ctrl.alpha            = asn1_type.alpha.to_number();
  cfg->ul_cfg.power_ctrl.p0_nominal_pucch = asn1_type.p0_nominal_pucch;
  cfg->ul_cfg.power_ctrl.delta_f_pucch[0] = asn1_type.delta_flist_pucch.delta_f_pucch_format1.to_number();
  cfg->ul_cfg.power_ctrl.delta_f_pucch[1] = asn1_type.delta_flist_pucch.delta_f_pucch_format1b.to_number();
  cfg->ul_cfg.power_ctrl.delta_f_pucch[2] = asn1_type.delta_flist_pucch.delta_f_pucch_format2.to_number();
  cfg->ul_cfg.power_ctrl.delta_f_pucch[3] = asn1_type.delta_flist_pucch.delta_f_pucch_format2a.to_number();
  cfg->ul_cfg.power_ctrl.delta_f_pucch[4] = asn1_type.delta_flist_pucch.delta_f_pucch_format2b.to_number();

  cfg->ul_cfg.power_ctrl.delta_preamble_msg3 = asn1_type.delta_preamb_msg3;
}

void set_phy_cfg_t_scell_config(phy_cfg_t* cfg, const asn1::rrc::scell_to_add_mod_r10_s& asn1_type)
{
  if (asn1_type.rr_cfg_common_scell_r10_present) {

    // Enable always CSI request extra bit
    cfg->dl_cfg.dci.multiple_csi_request_enabled = true;

    auto* rr_cfg_common_scell_r10 = &asn1_type.rr_cfg_common_scell_r10;

    if (rr_cfg_common_scell_r10->ul_cfg_r10_present) {
      auto* ul_cfg_r10 = &rr_cfg_common_scell_r10->ul_cfg_r10;

      // Parse Power control
      auto* ul_pwr_ctrl_common_scell_r10 = &ul_cfg_r10->ul_pwr_ctrl_common_scell_r10;
      bzero(&cfg->ul_cfg.power_ctrl, sizeof(srslte_ue_ul_powerctrl_t));
      cfg->ul_cfg.power_ctrl.p0_nominal_pusch = ul_pwr_ctrl_common_scell_r10->p0_nominal_pusch_r10;
      cfg->ul_cfg.power_ctrl.alpha            = ul_pwr_ctrl_common_scell_r10->alpha_r10.to_number();

      // Parse SRS
      cfg->ul_cfg.srs.common_enabled = ul_cfg_r10->srs_ul_cfg_common_r10.type() == asn1::rrc::setup_e::setup;
      if (cfg->ul_cfg.srs.common_enabled) {
        auto* srs_ul_cfg_common         = &ul_cfg_r10->srs_ul_cfg_common_r10.setup();
        cfg->ul_cfg.srs.simul_ack       = srs_ul_cfg_common->ack_nack_srs_simul_tx;
        cfg->ul_cfg.srs.bw_cfg          = srs_ul_cfg_common->srs_bw_cfg.to_number();
        cfg->ul_cfg.srs.subframe_config = srs_ul_cfg_common->srs_sf_cfg.to_number();
      }

      // Parse PUSCH
      auto* pusch_cfg_common = &ul_cfg_r10->pusch_cfg_common_r10;
      bzero(&cfg->ul_cfg.hopping, sizeof(srslte_pusch_hopping_cfg_t));
      cfg->ul_cfg.hopping.n_sb = pusch_cfg_common->pusch_cfg_basic.n_sb;
      cfg->ul_cfg.hopping.hop_mode =
          pusch_cfg_common->pusch_cfg_basic.hop_mode.value ==
                  asn1::rrc::pusch_cfg_common_s::pusch_cfg_basic_s_::hop_mode_e_::intra_and_inter_sub_frame
              ? cfg->ul_cfg.hopping.SRSLTE_PUSCH_HOP_MODE_INTRA_SF
              : cfg->ul_cfg.hopping.SRSLTE_PUSCH_HOP_MODE_INTER_SF;
      cfg->ul_cfg.hopping.hopping_offset = pusch_cfg_common->pusch_cfg_basic.pusch_hop_offset;
      cfg->ul_cfg.pusch.enable_64qam     = pusch_cfg_common->pusch_cfg_basic.enable64_qam;
    }
  }

  if (asn1_type.rr_cfg_ded_scell_r10_present) {
    auto* rr_cfg_ded_scell_r10 = &asn1_type.rr_cfg_ded_scell_r10;
    if (rr_cfg_ded_scell_r10->phys_cfg_ded_scell_r10_present) {
      auto* phys_cfg_ded_scell_r10 = &rr_cfg_ded_scell_r10->phys_cfg_ded_scell_r10;

      // Parse nonUL Configuration
      if (phys_cfg_ded_scell_r10->non_ul_cfg_r10_present) {

        auto* non_ul_cfg = &phys_cfg_ded_scell_r10->non_ul_cfg_r10;

        // Parse Transmission mode
        if (non_ul_cfg->ant_info_r10_present) {
          if (non_ul_cfg->ant_info_r10.tx_mode_r10.to_number() < (uint8_t)SRSLTE_TMINV) {
            cfg->dl_cfg.tm = (srslte_tm_t)non_ul_cfg->ant_info_r10.tx_mode_r10.to_number();
          } else {
            fprintf(stderr,
                    "Transmission mode (R10) %s is not supported\n",
                    non_ul_cfg->ant_info_r10.tx_mode_r10.to_string().c_str());
          }
        }

        // Parse Cross carrier scheduling
        if (non_ul_cfg->cross_carrier_sched_cfg_r10_present) {
          typedef asn1::rrc::cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_ sched_info_t;

          typedef sched_info_t::types cross_carrier_type_e;
          auto*                       sched_info = &non_ul_cfg->cross_carrier_sched_cfg_r10.sched_cell_info_r10;

          cross_carrier_type_e cross_carrier_type = sched_info->type();
          if (cross_carrier_type == cross_carrier_type_e::own_r10) {
            cfg->dl_cfg.dci.cif_present = sched_info->own_r10().cif_presence_r10;
          } else {
            cfg->dl_cfg.dci.cif_present = false; // This CC does not have Carrier Indicator Field
            // ue_cfg->dl_cfg.blablabla = sched_info->other_r10().pdsch_start_r10;
            // ue_cfg->dl_cfg.blablabla = sched_info->other_r10().sched_cell_id_r10;
          }
        }

        // Parse pdsch config dedicated
        if (non_ul_cfg->pdsch_cfg_ded_r10_present) {
          cfg->dl_cfg.pdsch.p_b         = asn1_type.rr_cfg_common_scell_r10.non_ul_cfg_r10.pdsch_cfg_common_r10.p_b;
          cfg->dl_cfg.pdsch.p_a         = non_ul_cfg->pdsch_cfg_ded_r10.p_a.to_number();
          cfg->dl_cfg.pdsch.power_scale = true;
        }
      }

      // Parse UL Configuration
      if (phys_cfg_ded_scell_r10->ul_cfg_r10_present) {
        auto* ul_cfg_r10 = &phys_cfg_ded_scell_r10->ul_cfg_r10;

        // Parse CQI param
        if (ul_cfg_r10->cqi_report_cfg_scell_r10_present) {
          auto* cqi_report_cfg = &ul_cfg_r10->cqi_report_cfg_scell_r10;

          // Aperiodic report
          if (cqi_report_cfg->cqi_report_mode_aperiodic_r10_present) {
            cfg->dl_cfg.cqi_report.aperiodic_configured = true;
            cfg->dl_cfg.cqi_report.aperiodic_mode = make_aperiodic_mode(cqi_report_cfg->cqi_report_mode_aperiodic_r10);
          }

          // Periodic report
          if (cqi_report_cfg->cqi_report_periodic_scell_r10_present) {
            if (cqi_report_cfg->cqi_report_periodic_scell_r10.type() == asn1::rrc::setup_e::setup) {
              typedef asn1::rrc::cqi_report_periodic_r10_c::setup_s_ cqi_cfg_t;
              cqi_cfg_t cqi_cfg                          = cqi_report_cfg->cqi_report_periodic_scell_r10.setup();
              cfg->dl_cfg.cqi_report.periodic_configured = true;
              cfg->dl_cfg.cqi_report.pmi_idx             = cqi_cfg.cqi_pmi_cfg_idx;
              cfg->dl_cfg.cqi_report.format_is_subband =
                  cqi_cfg.cqi_format_ind_periodic_r10.type().value ==
                  cqi_cfg_t::cqi_format_ind_periodic_r10_c_::types::subband_cqi_r10;
              if (cfg->dl_cfg.cqi_report.format_is_subband) {
                cfg->dl_cfg.cqi_report.subband_size = cqi_cfg.cqi_format_ind_periodic_r10.subband_cqi_r10().k;
              }
              if (cqi_cfg.ri_cfg_idx_present) {
                cfg->dl_cfg.cqi_report.ri_idx         = cqi_cfg.ri_cfg_idx;
                cfg->dl_cfg.cqi_report.ri_idx_present = true;
              } else {
                cfg->dl_cfg.cqi_report.ri_idx_present = false;
              }
            } else {
              // Release, disable periodic reporting
              cfg->dl_cfg.cqi_report.periodic_configured = false;
            }
          }
        }

        // Sounding reference signals Dedicated
        if (ul_cfg_r10->srs_ul_cfg_ded_r10_present) {
          cfg->ul_cfg.srs.dedicated_enabled = ul_cfg_r10->srs_ul_cfg_ded_r10.type() == asn1::rrc::setup_e::setup;
          if (cfg->ul_cfg.srs.dedicated_enabled) {
            auto* srs_ul_cfg_ded_r10   = &ul_cfg_r10->srs_ul_cfg_ded_r10.setup();
            cfg->ul_cfg.srs.configured = cfg->ul_cfg.srs.dedicated_enabled and cfg->ul_cfg.srs.common_enabled;
            cfg->ul_cfg.srs.I_srs      = srs_ul_cfg_ded_r10->srs_cfg_idx;
            cfg->ul_cfg.srs.B          = srs_ul_cfg_ded_r10->srs_bw;
            cfg->ul_cfg.srs.b_hop      = srs_ul_cfg_ded_r10->srs_hop_bw;
            cfg->ul_cfg.srs.n_rrc      = srs_ul_cfg_ded_r10->freq_domain_position;
            cfg->ul_cfg.srs.k_tc       = srs_ul_cfg_ded_r10->tx_comb;
            cfg->ul_cfg.srs.n_srs      = srs_ul_cfg_ded_r10->cyclic_shift;
          }
        }
      }

      if (phys_cfg_ded_scell_r10->cqi_report_cfg_scell_v1250.is_present()) {
        // Enable/disable PDSCH 256QAM
        auto cqi_report_cfg_scell           = phys_cfg_ded_scell_r10->cqi_report_cfg_scell_v1250.get();
        cfg->dl_cfg.pdsch.use_tbs_index_alt = cqi_report_cfg_scell->alt_cqi_table_r12_present;
      } else {
        // Assume there is no PDSCH 256QAM
        cfg->dl_cfg.pdsch.use_tbs_index_alt = false;
      }
    }
  }
}

/*
 * UE Capabilities parser
 */

template <class T>
static void set_rrc_ue_eutra_cap_t_gen(rrc_ue_capabilities_t& ue_cap, const T& ue_eutra_cap)
{
  if (ue_eutra_cap.non_crit_ext_present) {
    set_rrc_ue_eutra_cap_t_gen(ue_cap, ue_eutra_cap.non_crit_ext);
  }
}

static void set_rrc_ue_eutra_cap_t_gen(rrc_ue_capabilities_t& ue_cap, const asn1::rrc::ue_eutra_cap_s& ue_eutra_cap)
{
  ue_cap.release  = ue_eutra_cap.access_stratum_release.to_number();
  ue_cap.category = ue_eutra_cap.ue_category;

  if (ue_eutra_cap.non_crit_ext_present) {
    set_rrc_ue_eutra_cap_t_gen(ue_cap, ue_eutra_cap.non_crit_ext);
  }
}

static void set_rrc_ue_eutra_cap_t_gen(rrc_ue_capabilities_t&                     ue_cap,
                                       const asn1::rrc::ue_eutra_cap_v1020_ies_s& ue_eutra_cap)
{
  if (ue_eutra_cap.ue_category_v1020_present) {
    ue_cap.category = ue_eutra_cap.ue_category_v1020;
  }

  if (ue_eutra_cap.non_crit_ext_present) {
    set_rrc_ue_eutra_cap_t_gen(ue_cap, ue_eutra_cap.non_crit_ext);
  }
}

static void set_rrc_ue_eutra_cap_t_gen(rrc_ue_capabilities_t&                     ue_cap,
                                       const asn1::rrc::ue_eutra_cap_v1250_ies_s& ue_eutra_cap)
{
  if (ue_eutra_cap.ue_category_dl_r12_present) {
    ue_cap.category_dl = ue_eutra_cap.ue_category_dl_r12;
  }

  if (ue_eutra_cap.ue_category_ul_r12_present) {
    ue_cap.category_ul = ue_eutra_cap.ue_category_ul_r12;
  }

  if (ue_eutra_cap.rf_params_v1250_present) {
    const asn1::rrc::rf_params_v1250_s& rf_params = ue_eutra_cap.rf_params_v1250;
    if (rf_params.supported_band_list_eutra_v1250_present) {
      ue_cap.support_dl_256qam = true;

      for (const asn1::rrc::supported_band_eutra_v1250_s& supported_band : rf_params.supported_band_list_eutra_v1250) {
        ue_cap.support_dl_256qam &= supported_band.dl_minus256_qam_r12_present;
        ue_cap.support_ul_64qam &= supported_band.ul_minus64_qam_r12_present;
      }
    }
  }

  if (ue_eutra_cap.non_crit_ext_present) {
    set_rrc_ue_eutra_cap_t_gen(ue_cap, ue_eutra_cap.non_crit_ext);
  }
}

static void set_rrc_ue_eutra_cap_t_gen(rrc_ue_capabilities_t&                     ue_cap,
                                       const asn1::rrc::ue_eutra_cap_v1530_ies_s& ue_eutra_cap)
{
  ; // Do nothing
}

rrc_ue_capabilities_t make_rrc_ue_capabilities(const asn1::rrc::ue_eutra_cap_s& eutra_cap_s)
{
  rrc_ue_capabilities_t ue_cap;
  set_rrc_ue_eutra_cap_t_gen(ue_cap, eutra_cap_s);
  return ue_cap;
}

// MBMS

mbms_notif_cfg_t make_mbms_notif_cfg(const asn1::rrc::mbms_notif_cfg_r9_s& asn1_type)
{
  mbms_notif_cfg_t ret{};
  ret.notif_repeat_coeff = (mbms_notif_cfg_t::coeff_t)asn1_type.notif_repeat_coeff_r9.value;
  ret.notif_offset       = asn1_type.notif_offset_r9;
  ret.notif_sf_idx       = asn1_type.notif_sf_idx_r9;
  return ret;
}

mbsfn_area_info_t make_mbsfn_area_info(const asn1::rrc::mbsfn_area_info_r9_s& asn1_type)
{
  mbsfn_area_info_t ret{};
  ret.mbsfn_area_id        = asn1_type.mbsfn_area_id_r9;
  ret.non_mbsfn_region_len = (mbsfn_area_info_t::region_len_t)asn1_type.non_mbsfn_region_len.value;
  ret.notif_ind            = asn1_type.notif_ind_r9;
  ret.mcch_cfg.mcch_repeat_period =
      (mbsfn_area_info_t::mcch_cfg_t::repeat_period_t)asn1_type.mcch_cfg_r9.mcch_repeat_period_r9.value;
  ret.mcch_cfg.mcch_offset = asn1_type.mcch_cfg_r9.mcch_offset_r9;
  ret.mcch_cfg.mcch_mod_period =
      (mbsfn_area_info_t::mcch_cfg_t::mod_period_t)asn1_type.mcch_cfg_r9.mcch_mod_period_r9.value;
  ret.mcch_cfg.sf_alloc_info = asn1_type.mcch_cfg_r9.sf_alloc_info_r9.to_number();
  ret.mcch_cfg.sig_mcs       = (mbsfn_area_info_t::mcch_cfg_t::sig_mcs_t)asn1_type.mcch_cfg_r9.sig_mcs_r9.value;
  return ret;
}

mbsfn_sf_cfg_t make_mbsfn_sf_cfg(const asn1::rrc::mbsfn_sf_cfg_s& sf_cfg)
{
  mbsfn_sf_cfg_t cfg{};
  cfg.radioframe_alloc_period = (mbsfn_sf_cfg_t::alloc_period_t)sf_cfg.radioframe_alloc_period.value;
  cfg.radioframe_alloc_offset = sf_cfg.radioframe_alloc_offset;
  cfg.nof_alloc_subfrs        = (mbsfn_sf_cfg_t::sf_alloc_type_t)sf_cfg.sf_alloc.type().value;
  if (sf_cfg.sf_alloc.type().value == asn1::rrc::mbsfn_sf_cfg_s::sf_alloc_c_::types_opts::one_frame) {
    cfg.sf_alloc = sf_cfg.sf_alloc.one_frame().to_number();
  } else {
    cfg.sf_alloc = sf_cfg.sf_alloc.four_frames().to_number();
  }
  return cfg;
}

pmch_info_t make_pmch_info(const asn1::rrc::pmch_info_r9_s& asn1_type)
{
  pmch_info_t ret{};
  ret.sf_alloc_end     = asn1_type.pmch_cfg_r9.sf_alloc_end_r9;
  ret.data_mcs         = asn1_type.pmch_cfg_r9.data_mcs_r9;
  ret.mch_sched_period = (pmch_info_t::mch_sched_period_t)asn1_type.pmch_cfg_r9.mch_sched_period_r9.value;

  ret.nof_mbms_session_info = asn1_type.mbms_session_info_list_r9.size();
  for (uint32_t i = 0; i < ret.nof_mbms_session_info; ++i) {
    auto& asn1item          = asn1_type.mbms_session_info_list_r9[i];
    auto& item              = ret.mbms_session_info_list[i];
    item.session_id_present = asn1item.session_id_r9_present;
    item.lc_ch_id           = asn1item.lc_ch_id_r9;
    item.session_id         = asn1item.session_id_r9[0];
    item.tmgi.plmn_id_type  = (tmgi_t::plmn_id_type_t)asn1item.tmgi_r9.plmn_id_r9.type().value;
    if (item.tmgi.plmn_id_type == tmgi_t::plmn_id_type_t::plmn_idx) {
      item.tmgi.plmn_id.plmn_idx = asn1item.tmgi_r9.plmn_id_r9.plmn_idx_r9();
    } else {
      item.tmgi.plmn_id.explicit_value = make_plmn_id_t(asn1item.tmgi_r9.plmn_id_r9.explicit_value_r9());
    }
    memcpy(item.tmgi.serviced_id, &asn1item.tmgi_r9.service_id_r9[0], 3);
  }
  return ret;
}

mcch_msg_t make_mcch_msg(const asn1::rrc::mcch_msg_s& asn1_type)
{
  mcch_msg_t msg{};
  auto&      r9           = asn1_type.msg.c1().mbsfn_area_cfg_r9();
  msg.nof_common_sf_alloc = r9.common_sf_alloc_r9.size();
  for (uint32_t i = 0; i < msg.nof_common_sf_alloc; ++i) {
    msg.common_sf_alloc[i] = make_mbsfn_sf_cfg(r9.common_sf_alloc_r9[i]);
  }
  msg.common_sf_alloc_period = (mcch_msg_t::common_sf_alloc_period_t)r9.common_sf_alloc_period_r9.value;
  msg.nof_pmch_info          = r9.pmch_info_list_r9.size();
  for (uint32_t i = 0; i < msg.nof_pmch_info; ++i) {
    msg.pmch_info_list[i] = make_pmch_info(r9.pmch_info_list_r9[i]);
  }
  return msg;
}
static_assert(ASN1_RRC_MAX_SESSION_PER_PMCH == pmch_info_t::max_session_per_pmch, "ASN1 to srsLTE interface mismatch");

sib13_t make_sib13(const asn1::rrc::sib_type13_r9_s& asn1_type)
{
  sib13_t sib13{};
  sib13.nof_mbsfn_area_info = asn1_type.mbsfn_area_info_list_r9.size();
  for (uint32_t i = 0; i < asn1_type.mbsfn_area_info_list_r9.size(); ++i) {
    sib13.mbsfn_area_info_list[i] = make_mbsfn_area_info(asn1_type.mbsfn_area_info_list_r9[i]);
  }
  sib13.notif_cfg = make_mbms_notif_cfg(asn1_type.notif_cfg_r9);
  return sib13;
}

} // namespace srslte

namespace asn1 {
namespace rrc {

/***************************
 *      MeasConfig
 **************************/

bool operator==(const cells_to_add_mod_s& lhs, const cells_to_add_mod_s& rhs)
{
  return lhs.cell_idx == rhs.cell_idx and lhs.pci == rhs.pci and
         lhs.cell_individual_offset == rhs.cell_individual_offset;
}

bool operator==(const meas_obj_to_add_mod_s& lhs, const meas_obj_to_add_mod_s& rhs)
{
  if (lhs.meas_obj_id != rhs.meas_obj_id or lhs.meas_obj.type() != lhs.meas_obj.type()) {
    return false;
  }
  auto &lhs_eutra = lhs.meas_obj.meas_obj_eutra(), &rhs_eutra = rhs.meas_obj.meas_obj_eutra();
  if (lhs_eutra.ext or rhs_eutra.ext) {
    printf("[%d] extension of measObjToAddMod not supported\n", __LINE__);
    return false;
  }

  if (lhs_eutra.offset_freq_present != rhs_eutra.offset_freq_present or
      (lhs_eutra.offset_freq_present and lhs_eutra.offset_freq != rhs_eutra.offset_freq)) {
    return false;
  }

  if (lhs_eutra.carrier_freq != rhs_eutra.carrier_freq or not(lhs_eutra.neigh_cell_cfg == rhs_eutra.neigh_cell_cfg) or
      lhs_eutra.presence_ant_port1 != rhs_eutra.presence_ant_port1 or
      lhs_eutra.allowed_meas_bw != rhs_eutra.allowed_meas_bw) {
    return false;
  }

  if (lhs_eutra.cells_to_add_mod_list.size() != rhs_eutra.cells_to_add_mod_list.size()) {
    return false;
  }

  auto cells_are_equal = [](const cells_to_add_mod_s& lhs, const cells_to_add_mod_s& rhs) { return lhs == rhs; };
  return std::equal(lhs_eutra.cells_to_add_mod_list.begin(),
                    lhs_eutra.cells_to_add_mod_list.end(),
                    rhs_eutra.cells_to_add_mod_list.begin(),
                    cells_are_equal);
}

bool operator==(const report_cfg_eutra_s& lhs, const report_cfg_eutra_s& rhs)
{
  if (lhs.ext or rhs.ext) {
    printf("[%d] extension of reportCfgToAddMod not supported\n", __LINE__);
    return false;
  }
  if (lhs.trigger_type.type() != rhs.trigger_type.type()) {
    return false;
  }
  if (lhs.trigger_type.type().value == report_cfg_eutra_s::trigger_type_c_::types_opts::event) {
    auto &lhs_ev = lhs.trigger_type.event(), &rhs_ev = rhs.trigger_type.event();
    if (lhs_ev.hysteresis != rhs_ev.hysteresis or lhs_ev.time_to_trigger != rhs_ev.time_to_trigger or
        lhs_ev.event_id.type() != rhs_ev.event_id.type()) {
      return false;
    }
    if (lhs_ev.event_id.type().value != eutra_event_s::event_id_c_::types_opts::event_a3) {
      printf("[%d] event type != A3 of reportCfgToAddMod not supported\n", __LINE__);
      return false;
    }
    if (lhs_ev.event_id.event_a3().report_on_leave != rhs_ev.event_id.event_a3().report_on_leave or
        lhs_ev.event_id.event_a3().a3_offset != rhs_ev.event_id.event_a3().a3_offset) {
      return false;
    }
  } else {
    if (lhs.trigger_type.periodical().purpose != rhs.trigger_type.periodical().purpose) {
      return false;
    }
  }
  return lhs.trigger_quant == rhs.trigger_quant and lhs.report_quant == rhs.report_quant and
         lhs.max_report_cells == rhs.max_report_cells and lhs.report_interv == rhs.report_interv and
         lhs.report_amount == rhs.report_amount;
}

bool operator==(const report_cfg_to_add_mod_s& lhs, const report_cfg_to_add_mod_s& rhs)
{
  if (lhs.report_cfg_id != rhs.report_cfg_id or lhs.report_cfg.type() != rhs.report_cfg.type()) {
    return false;
  }
  return lhs.report_cfg.report_cfg_eutra() == rhs.report_cfg.report_cfg_eutra();
}

bool operator==(const meas_id_to_add_mod_s& lhs, const meas_id_to_add_mod_s& rhs)
{
  return lhs.meas_id == rhs.meas_id and lhs.meas_obj_id == rhs.meas_obj_id and lhs.report_cfg_id == rhs.report_cfg_id;
}

bool operator==(const asn1::rrc::quant_cfg_s& lhs, const asn1::rrc::quant_cfg_s& rhs)
{
  if (lhs.ext or lhs.quant_cfg_geran_present or lhs.quant_cfg_utra_present or lhs.quant_cfg_cdma2000_present or
      rhs.ext or rhs.quant_cfg_geran_present or rhs.quant_cfg_utra_present or rhs.quant_cfg_cdma2000_present) {
    printf("[%d] quantCfg properties not supported\n", __LINE__);
    return false;
  }
  return lhs.quant_cfg_eutra_present == rhs.quant_cfg_eutra_present and
         lhs.quant_cfg_eutra.filt_coef_rsrp_present == rhs.quant_cfg_eutra.filt_coef_rsrp_present and
         lhs.quant_cfg_eutra.filt_coef_rsrp == rhs.quant_cfg_eutra.filt_coef_rsrp and
         lhs.quant_cfg_eutra.filt_coef_rsrq_present == rhs.quant_cfg_eutra.filt_coef_rsrq_present and
         lhs.quant_cfg_eutra.filt_coef_rsrq == rhs.quant_cfg_eutra.filt_coef_rsrq;
}

/**************************
 *     RRC Obj Id
 *************************/

uint8_t get_rrc_obj_id(const srb_to_add_mod_s& srb)
{
  return srb.srb_id;
}
uint8_t get_rrc_obj_id(const drb_to_add_mod_s& drb)
{
  return drb.drb_id;
}
uint8_t get_rrc_obj_id(const cells_to_add_mod_s& obj)
{
  return obj.cell_idx;
}
uint8_t get_rrc_obj_id(const meas_obj_to_add_mod_s& obj)
{
  return obj.meas_obj_id;
}
uint8_t get_rrc_obj_id(const report_cfg_to_add_mod_s& obj)
{
  return obj.report_cfg_id;
}
uint8_t get_rrc_obj_id(const meas_id_to_add_mod_s& obj)
{
  return obj.meas_id;
}

void set_rrc_obj_id(srb_to_add_mod_s& srb, uint8_t id)
{
  srb.srb_id = id;
}
void set_rrc_obj_id(drb_to_add_mod_s& drb, uint8_t id)
{
  drb.drb_id = id;
}
void set_rrc_obj_id(cells_to_add_mod_s& obj, uint8_t id)
{
  obj.cell_idx = id;
}
void set_rrc_obj_id(meas_obj_to_add_mod_s& obj, uint8_t id)
{
  obj.meas_obj_id = id;
}
void set_rrc_obj_id(report_cfg_to_add_mod_s& obj, uint8_t id)
{
  obj.report_cfg_id = id;
}
void set_rrc_obj_id(meas_id_to_add_mod_s& obj, uint8_t id)
{
  obj.meas_id = id;
}

} // namespace rrc
} // namespace asn1
