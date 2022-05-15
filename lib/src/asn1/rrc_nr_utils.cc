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

#include "srsran/asn1/rrc_nr_utils.h"
#include "srsran/asn1/obj_id_cmp_utils.h"
#include "srsran/asn1/rrc_nr.h"
#include "srsran/common/band_helper.h"
#include "srsran/config.h"
#include "srsran/interfaces/pdcp_interface_types.h"
#include "srsran/interfaces/rlc_interface_types.h"
#include <algorithm>

namespace srsran {

using namespace asn1::rrc_nr;
/***************************
 *        PLMN ID
 **************************/

bool plmn_is_valid(const asn1::rrc_nr::plmn_id_s& asn1_type)
{
  return asn1_type.mcc_present and (asn1_type.mnc.size() == 3 or asn1_type.mnc.size() == 2);
}

plmn_id_t make_plmn_id_t(const asn1::rrc_nr::plmn_id_s& asn1_type)
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

void to_asn1(asn1::rrc_nr::plmn_id_s* asn1_type, const plmn_id_t& cfg)
{
  asn1_type->mcc_present = true;
  std::copy(&cfg.mcc[0], &cfg.mcc[3], &asn1_type->mcc[0]);
  asn1_type->mnc.resize(cfg.nof_mnc_digits);
  std::copy(&cfg.mnc[0], &cfg.mnc[cfg.nof_mnc_digits], &asn1_type->mnc[0]);
}

logical_channel_config_t make_mac_logical_channel_cfg_t(uint8_t lcid, const lc_ch_cfg_s& asn1_type)
{
  logical_channel_config_t logical_channel_config = {};
  logical_channel_config.lcid                     = lcid;

  if (asn1_type.ul_specific_params.lc_ch_group_present) {
    logical_channel_config.lcg = asn1_type.ul_specific_params.lc_ch_group;
  }
  logical_channel_config.priority    = asn1_type.ul_specific_params.prio;
  logical_channel_config.PBR         = asn1_type.ul_specific_params.prioritised_bit_rate.to_number();
  logical_channel_config.BSD         = asn1_type.ul_specific_params.bucket_size_dur.to_number();
  logical_channel_config.bucket_size = logical_channel_config.PBR * logical_channel_config.BSD;

  return logical_channel_config;
}

bool make_mac_dl_harq_cfg_nr_t(const pdsch_serving_cell_cfg_s& asn1_type, dl_harq_cfg_nr_t* out_dl_harq_cfg_nr)
{
  dl_harq_cfg_nr_t dl_harq_cfg_nr;
  if (asn1_type.nrof_harq_processes_for_pdsch_present) {
    dl_harq_cfg_nr.nof_procs = asn1_type.nrof_harq_processes_for_pdsch.to_number();
  } else {
    asn1::log_warning("Option nrof_harq_processes_for_pdsch not present");
    return false;
  }
  *out_dl_harq_cfg_nr = dl_harq_cfg_nr;
  return true;
}

bool make_mac_phr_cfg_t(const phr_cfg_s& asn1_type, phr_cfg_nr_t* phr_cfg_nr)
{
  phr_cfg_nr->extended             = asn1_type.ext;
  phr_cfg_nr->periodic_timer       = asn1_type.phr_periodic_timer.to_number();
  phr_cfg_nr->prohibit_timer       = asn1_type.phr_prohibit_timer.to_number();
  phr_cfg_nr->tx_pwr_factor_change = asn1_type.phr_tx_pwr_factor_change.to_number();
  return true;
}

void make_mac_rach_cfg(const rach_cfg_common_s& asn1_type, rach_cfg_nr_t* rach_cfg_nr)
{
  rach_cfg_nr->powerRampingStep             = asn1_type.rach_cfg_generic.pwr_ramp_step.to_number();
  rach_cfg_nr->ra_responseWindow            = asn1_type.rach_cfg_generic.ra_resp_win.to_number();
  rach_cfg_nr->prach_ConfigurationIndex     = asn1_type.rach_cfg_generic.prach_cfg_idx;
  rach_cfg_nr->PreambleReceivedTargetPower  = asn1_type.rach_cfg_generic.preamb_rx_target_pwr;
  rach_cfg_nr->preambleTransMax             = asn1_type.rach_cfg_generic.preamb_trans_max.to_number();
  rach_cfg_nr->ra_ContentionResolutionTimer = asn1_type.ra_contention_resolution_timer.to_number();
};

int make_rlc_config_t(const rlc_cfg_c& asn1_type, uint8_t bearer_id, rlc_config_t* cfg_out)
{
  rlc_config_t rlc_cfg = {};
  rlc_cfg.rat          = srsran_rat_t::nr;
  switch (asn1_type.type().value) {
    case rlc_cfg_c::types_opts::am:
      rlc_cfg = rlc_config_t::default_rlc_am_nr_config();
      if (asn1_type.am().dl_am_rlc.sn_field_len_present && asn1_type.am().ul_am_rlc.sn_field_len_present &&
          asn1_type.am().dl_am_rlc.sn_field_len != asn1_type.am().ul_am_rlc.sn_field_len) {
        asn1::log_warning("NR RLC sequence number length is not the same in uplink and downlink");
        return SRSRAN_ERROR;
      }
      rlc_cfg.rlc_mode = rlc_mode_t::am;
      switch (asn1_type.am().dl_am_rlc.sn_field_len.value) {
        case asn1::rrc_nr::sn_field_len_am_opts::options::size12:
          rlc_cfg.am_nr.tx_sn_field_length = rlc_am_nr_sn_size_t::size12bits;
          rlc_cfg.am_nr.rx_sn_field_length = rlc_am_nr_sn_size_t::size12bits;
          break;
        case asn1::rrc_nr::sn_field_len_am_opts::options::size18:
          rlc_cfg.am_nr.tx_sn_field_length = rlc_am_nr_sn_size_t::size18bits;
          rlc_cfg.am_nr.rx_sn_field_length = rlc_am_nr_sn_size_t::size18bits;
          break;
        default:
          break;
      }
      rlc_cfg.am_nr.t_poll_retx       = asn1_type.am().ul_am_rlc.t_poll_retx.to_number();
      rlc_cfg.am_nr.poll_pdu          = asn1_type.am().ul_am_rlc.poll_pdu.to_number();
      rlc_cfg.am_nr.poll_byte         = asn1_type.am().ul_am_rlc.poll_byte.to_number();
      rlc_cfg.am_nr.max_retx_thresh   = asn1_type.am().ul_am_rlc.max_retx_thres.to_number();
      rlc_cfg.am_nr.t_reassembly      = asn1_type.am().dl_am_rlc.t_reassembly.to_number();
      rlc_cfg.am_nr.t_status_prohibit = asn1_type.am().dl_am_rlc.t_status_prohibit.to_number();
      break;
    case rlc_cfg_c::types_opts::um_bi_dir:
      rlc_cfg                       = rlc_config_t::default_rlc_um_nr_config();
      rlc_cfg.rlc_mode              = rlc_mode_t::um;
      rlc_cfg.um_nr.t_reassembly_ms = asn1_type.um_bi_dir().dl_um_rlc.t_reassembly.to_number();
      rlc_cfg.um_nr.bearer_id       = bearer_id;
      if (asn1_type.um_bi_dir().dl_um_rlc.sn_field_len_present &&
          asn1_type.um_bi_dir().ul_um_rlc.sn_field_len_present &&
          asn1_type.um_bi_dir().dl_um_rlc.sn_field_len != asn1_type.um_bi_dir().ul_um_rlc.sn_field_len) {
        asn1::log_warning("NR RLC sequence number length is not the same in uplink and downlink");
        return SRSRAN_ERROR;
      }

      switch (asn1_type.um_bi_dir().dl_um_rlc.sn_field_len.value) {
        case asn1::rrc_nr::sn_field_len_um_opts::options::size6:
          rlc_cfg.um_nr.sn_field_length = rlc_um_nr_sn_size_t::size6bits;
          break;
        case asn1::rrc_nr::sn_field_len_um_opts::options::size12:
          rlc_cfg.um_nr.sn_field_length = rlc_um_nr_sn_size_t::size12bits;
          break;
        default:
          break;
      }
      rlc_cfg.um_nr.t_reassembly_ms = asn1_type.um_bi_dir().dl_um_rlc.t_reassembly.to_number();
      break;
    case rlc_cfg_c::types_opts::um_uni_dir_dl:
      asn1::log_warning("NR RLC type %s is not supported", asn1_type.type().to_string());
      return SRSRAN_ERROR;
    case rlc_cfg_c::types_opts::um_uni_dir_ul:
      asn1::log_warning("NR RLC type %s is not supported", asn1_type.type().to_string());
      return SRSRAN_ERROR;
    default:
      break;
  }

  *cfg_out = rlc_cfg;
  return SRSRAN_SUCCESS;
}

srsran::pdcp_config_t make_nr_srb_pdcp_config_t(const uint8_t bearer_id, bool is_ue)
{
  pdcp_config_t cfg(bearer_id,
                    PDCP_RB_IS_SRB,
                    is_ue ? SECURITY_DIRECTION_UPLINK : SECURITY_DIRECTION_DOWNLINK,
                    is_ue ? SECURITY_DIRECTION_DOWNLINK : SECURITY_DIRECTION_UPLINK,
                    PDCP_SN_LEN_12,
                    pdcp_t_reordering_t::ms500,
                    pdcp_discard_timer_t::infinity,
                    false,
                    srsran_rat_t::nr);
  return cfg;
}

srsran::pdcp_config_t make_drb_pdcp_config_t(const uint8_t bearer_id, bool is_ue, const pdcp_cfg_s& pdcp_cfg)
{
  // TODO: complete config processing
  // TODO: check if is drb_cfg.pdcp_cfg.drb_present if not return Error
  // TODO: different pdcp sn size for ul and dl
  pdcp_discard_timer_t discard_timer = pdcp_discard_timer_t::infinity;
  if (pdcp_cfg.drb.discard_timer_present) {
    switch (pdcp_cfg.drb.discard_timer.to_number()) {
      case 10:
        discard_timer = pdcp_discard_timer_t::ms10;
        break;
      case 20:
        discard_timer = pdcp_discard_timer_t::ms20;
        break;
      case 30:
        discard_timer = pdcp_discard_timer_t::ms30;
        break;
      case 40:
        discard_timer = pdcp_discard_timer_t::ms40;
        break;
      case 50:
        discard_timer = pdcp_discard_timer_t::ms50;
        break;
      case 60:
        discard_timer = pdcp_discard_timer_t::ms60;
        break;
      case 75:
        discard_timer = pdcp_discard_timer_t::ms75;
        break;
      case 100:
        discard_timer = pdcp_discard_timer_t::ms100;
        break;
      case 150:
        discard_timer = pdcp_discard_timer_t::ms150;
        break;
      case 200:
        discard_timer = pdcp_discard_timer_t::ms200;
        break;
      case 250:
        discard_timer = pdcp_discard_timer_t::ms250;
        break;
      case 300:
        discard_timer = pdcp_discard_timer_t::ms300;
        break;
      case 500:
        discard_timer = pdcp_discard_timer_t::ms500;
        break;
      case 750:
        discard_timer = pdcp_discard_timer_t::ms750;
        break;
      case 1500:
        discard_timer = pdcp_discard_timer_t::ms1500;
        break;
      default:
        discard_timer = pdcp_discard_timer_t::infinity;
        break;
    }
  }

  pdcp_t_reordering_t t_reordering = pdcp_t_reordering_t::infinity;
  if (pdcp_cfg.t_reordering_present) {
    switch (pdcp_cfg.t_reordering.to_number()) {
      case 0:
        t_reordering = pdcp_t_reordering_t::ms0;
        break;
      case 1:
        t_reordering = pdcp_t_reordering_t::ms1;
        break;
      case 2:
        t_reordering = pdcp_t_reordering_t::ms2;
        break;
      case 4:
        t_reordering = pdcp_t_reordering_t::ms4;
        break;
      case 5:
        t_reordering = pdcp_t_reordering_t::ms5;
        break;
      case 8:
        t_reordering = pdcp_t_reordering_t::ms8;
        break;
      case 10:
        t_reordering = pdcp_t_reordering_t::ms10;
        break;
      case 15:
        t_reordering = pdcp_t_reordering_t::ms15;
        break;
      case 20:
        t_reordering = pdcp_t_reordering_t::ms20;
        break;
      case 30:
        t_reordering = pdcp_t_reordering_t::ms30;
        break;
      case 40:
        t_reordering = pdcp_t_reordering_t::ms40;
        break;
      case 50:
        t_reordering = pdcp_t_reordering_t::ms50;
        break;
      case 60:
        t_reordering = pdcp_t_reordering_t::ms60;
        break;
      case 80:
        t_reordering = pdcp_t_reordering_t::ms80;
        break;
      case 100:
        t_reordering = pdcp_t_reordering_t::ms100;
        break;
      case 120:
        t_reordering = pdcp_t_reordering_t::ms120;
        break;
      case 140:
        t_reordering = pdcp_t_reordering_t::ms140;
        break;
      case 160:
        t_reordering = pdcp_t_reordering_t::ms160;
        break;
      case 180:
        t_reordering = pdcp_t_reordering_t::ms180;
        break;
      case 200:
        t_reordering = pdcp_t_reordering_t::ms200;
        break;
      case 220:
        t_reordering = pdcp_t_reordering_t::ms220;
        break;
      case 240:
        t_reordering = pdcp_t_reordering_t::ms240;
        break;
      case 260:
        t_reordering = pdcp_t_reordering_t::ms260;
        break;
      case 280:
        t_reordering = pdcp_t_reordering_t::ms280;
        break;
      case 300:
        t_reordering = pdcp_t_reordering_t::ms300;
        break;
      case 500:
        t_reordering = pdcp_t_reordering_t::ms500;
        break;
      case 750:
        t_reordering = pdcp_t_reordering_t::ms750;
        break;
      case 1000:
        t_reordering = pdcp_t_reordering_t::ms1000;
        break;
      case 1250:
        t_reordering = pdcp_t_reordering_t::ms1250;
        break;
      case 1500:
        t_reordering = pdcp_t_reordering_t::ms1500;
        break;
      case 1750:
        t_reordering = pdcp_t_reordering_t::ms1750;
        break;
      case 2000:
        t_reordering = pdcp_t_reordering_t::ms2000;
        break;
      case 2250:
        t_reordering = pdcp_t_reordering_t::ms2250;
        break;
      case 2500:
        t_reordering = pdcp_t_reordering_t::ms2500;
        break;
      case 2750:
        t_reordering = pdcp_t_reordering_t::ms2750;
        break;
      case 3000:
        t_reordering = pdcp_t_reordering_t::ms3000;
        break;
      default:
        t_reordering = pdcp_t_reordering_t::ms50;
    }
  }

  uint8_t sn_len = srsran::PDCP_SN_LEN_12;
  if (pdcp_cfg.drb.pdcp_sn_size_dl_present) {
    switch (pdcp_cfg.drb.pdcp_sn_size_dl.value) {
      case pdcp_cfg_s::drb_s_::pdcp_sn_size_dl_opts::options::len12bits:
        sn_len = srsran::PDCP_SN_LEN_12;
        break;
      case pdcp_cfg_s::drb_s_::pdcp_sn_size_dl_opts::options::len18bits:
        sn_len = srsran::PDCP_SN_LEN_18;
      default:
        break;
    }
  }

  pdcp_config_t cfg(bearer_id,
                    PDCP_RB_IS_DRB,
                    is_ue ? SECURITY_DIRECTION_UPLINK : SECURITY_DIRECTION_DOWNLINK,
                    is_ue ? SECURITY_DIRECTION_DOWNLINK : SECURITY_DIRECTION_UPLINK,
                    sn_len,
                    t_reordering,
                    discard_timer,
                    false,
                    srsran_rat_t::nr);
  return cfg;
}

bool make_phy_rach_cfg(const rach_cfg_common_s& asn1_type,
                       srsran_duplex_mode_t     duplex_mode,
                       srsran_prach_cfg_t*      prach_cfg)
{
  prach_cfg->is_nr            = true;
  prach_cfg->config_idx       = asn1_type.rach_cfg_generic.prach_cfg_idx;
  prach_cfg->zero_corr_zone   = (uint32_t)asn1_type.rach_cfg_generic.zero_correlation_zone_cfg;
  prach_cfg->num_ra_preambles = 64;
  if (asn1_type.total_nof_ra_preambs_present) {
    prach_cfg->num_ra_preambles = asn1_type.total_nof_ra_preambs;
  }
  prach_cfg->hs_flag    = false; // Hard-coded
  prach_cfg->tdd_config = {};
  if (duplex_mode == SRSRAN_DUPLEX_MODE_TDD) {
    prach_cfg->tdd_config.configured = true;
  }

  // As the current PRACH is based on LTE, the freq-offset shall be subtracted 1 for aligning with NR bandwidth
  // For example. A 52 PRB cell with an freq_offset of 1 will match a LTE 50 PRB cell with freq_offset of 0
  prach_cfg->freq_offset = (uint32_t)asn1_type.rach_cfg_generic.msg1_freq_start;
  if (prach_cfg->freq_offset == 0) {
    asn1::log_error("PRACH freq offset must be at least one");
    return false;
  }

  switch (asn1_type.prach_root_seq_idx.type().value) {
    case rach_cfg_common_s::prach_root_seq_idx_c_::types_opts::l839:
      prach_cfg->root_seq_idx = (uint32_t)asn1_type.prach_root_seq_idx.l839();
      break;
    case rach_cfg_common_s::prach_root_seq_idx_c_::types_opts::l139:
      prach_cfg->root_seq_idx = (uint32_t)asn1_type.prach_root_seq_idx.l139();
    default:
      asn1::log_error("Not-implemented option for prach_root_seq_idx type %s",
                      asn1_type.prach_root_seq_idx.type().to_string());
      return false;
  }

  return true;
};

bool fill_rach_cfg_common(const srsran_prach_cfg_t& prach_cfg, asn1::rrc_nr::rach_cfg_common_s& asn1_type)
{
  asn1_type = {};
  // rach-ConfigGeneric
  asn1_type.rach_cfg_generic.prach_cfg_idx             = prach_cfg.config_idx;
  asn1_type.rach_cfg_generic.msg1_fdm.value            = rach_cfg_generic_s::msg1_fdm_opts::one;
  asn1_type.rach_cfg_generic.msg1_freq_start           = prach_cfg.freq_offset;
  asn1_type.rach_cfg_generic.zero_correlation_zone_cfg = prach_cfg.zero_corr_zone;
  asn1_type.rach_cfg_generic.preamb_rx_target_pwr      = -110;
  asn1_type.rach_cfg_generic.preamb_trans_max.value    = rach_cfg_generic_s::preamb_trans_max_opts::n7;
  asn1_type.rach_cfg_generic.pwr_ramp_step.value       = rach_cfg_generic_s::pwr_ramp_step_opts::db4;
  asn1_type.rach_cfg_generic.ra_resp_win.value         = rach_cfg_generic_s::ra_resp_win_opts::sl10;

  // totalNumberOfRA-Preambles
  if (prach_cfg.num_ra_preambles != 64) {
    asn1_type.total_nof_ra_preambs_present = true;
    asn1_type.total_nof_ra_preambs         = prach_cfg.num_ra_preambles;
  }

  // ssb-perRACH-OccasionAndCB-PreamblesPerSSB
  asn1_type.ssb_per_rach_occasion_and_cb_preambs_per_ssb_present = true;
  if (not asn1::number_to_enum(asn1_type.ssb_per_rach_occasion_and_cb_preambs_per_ssb.set_one(),
                               prach_cfg.num_ra_preambles)) {
    asn1::log_error("Invalid number of RA preambles=%d", prach_cfg.num_ra_preambles);
    return false;
  }

  asn1_type.ra_contention_resolution_timer.value = rach_cfg_common_s::ra_contention_resolution_timer_opts::sf64;
  asn1_type.prach_root_seq_idx.set_l839()        = prach_cfg.root_seq_idx;
  asn1_type.restricted_set_cfg.value             = rach_cfg_common_s::restricted_set_cfg_opts::unrestricted_set;

  return true;
}

bool make_phy_tdd_cfg(const tdd_ul_dl_cfg_common_s& tdd_ul_dl_cfg_common,
                      srsran_duplex_config_nr_t*    in_srsran_duplex_config_nr)
{
  srsran_duplex_config_nr_t srsran_duplex_config_nr = {};
  srsran_duplex_config_nr.mode                      = SRSRAN_DUPLEX_MODE_TDD;

  switch (tdd_ul_dl_cfg_common.pattern1.dl_ul_tx_periodicity) {
    case tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms1:
      srsran_duplex_config_nr.tdd.pattern1.period_ms = 1;
      break;
    case tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms2:
      srsran_duplex_config_nr.tdd.pattern1.period_ms = 2;
      break;
    case tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms5:
      srsran_duplex_config_nr.tdd.pattern1.period_ms = 5;
      break;
    case tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms10:
      srsran_duplex_config_nr.tdd.pattern1.period_ms = 10;
      break;

    case tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms1p25:
    case tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms0p5:
    case tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms0p625:
    case tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms2p5:
    default:
      asn1::log_warning("Invalid option for dl_ul_tx_periodicity_opts %s",
                        tdd_ul_dl_cfg_common.pattern1.dl_ul_tx_periodicity.to_string());
      return false;
  }
  srsran_duplex_config_nr.tdd.pattern1.nof_dl_slots   = tdd_ul_dl_cfg_common.pattern1.nrof_dl_slots;
  srsran_duplex_config_nr.tdd.pattern1.nof_dl_symbols = tdd_ul_dl_cfg_common.pattern1.nrof_dl_symbols;
  srsran_duplex_config_nr.tdd.pattern1.nof_ul_slots   = tdd_ul_dl_cfg_common.pattern1.nrof_ul_slots;
  srsran_duplex_config_nr.tdd.pattern1.nof_ul_symbols = tdd_ul_dl_cfg_common.pattern1.nrof_ul_symbols;

  if (tdd_ul_dl_cfg_common.pattern2_present) {
    switch (tdd_ul_dl_cfg_common.pattern2.dl_ul_tx_periodicity) {
      case tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms1:
        srsran_duplex_config_nr.tdd.pattern2.period_ms = 1;
        break;
      case tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms2:
        srsran_duplex_config_nr.tdd.pattern2.period_ms = 2;
        break;
      case tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms5:
        srsran_duplex_config_nr.tdd.pattern2.period_ms = 5;
        break;
      case tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms10:
        srsran_duplex_config_nr.tdd.pattern2.period_ms = 10;
        break;

      case tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms1p25:
      case tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms0p5:
      case tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms0p625:
      case tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms2p5:
      default:
        asn1::log_warning("Invalid option for pattern2 dl_ul_tx_periodicity_opts %s",
                          tdd_ul_dl_cfg_common.pattern2.dl_ul_tx_periodicity.to_string());
        return false;
    }

    srsran_duplex_config_nr.tdd.pattern2.nof_dl_slots   = tdd_ul_dl_cfg_common.pattern2.nrof_dl_slots;
    srsran_duplex_config_nr.tdd.pattern2.nof_dl_symbols = tdd_ul_dl_cfg_common.pattern2.nrof_dl_symbols;
    srsran_duplex_config_nr.tdd.pattern2.nof_ul_slots   = tdd_ul_dl_cfg_common.pattern2.nrof_ul_slots;
    srsran_duplex_config_nr.tdd.pattern2.nof_ul_symbols = tdd_ul_dl_cfg_common.pattern2.nrof_ul_symbols;
  }

  // Copy and return struct
  *in_srsran_duplex_config_nr = srsran_duplex_config_nr;

  return true;
}

bool make_phy_tdd_cfg(const srsran_duplex_config_nr_t&      srsran_duplex_config_nr,
                      srsran_subcarrier_spacing_t           scs,
                      asn1::rrc_nr::tdd_ul_dl_cfg_common_s* tdd_ul_dl_cfg_common)
{
  if (srsran_duplex_config_nr.mode == SRSRAN_DUPLEX_MODE_FDD) {
    return true;
  }
  tdd_ul_dl_cfg_common->ref_subcarrier_spacing.value = (asn1::rrc_nr::subcarrier_spacing_e::options)scs;

  switch (srsran_duplex_config_nr.tdd.pattern1.period_ms) {
    case 1:
      tdd_ul_dl_cfg_common->pattern1.dl_ul_tx_periodicity = tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms1;
      break;
    case 2:
      tdd_ul_dl_cfg_common->pattern1.dl_ul_tx_periodicity = tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms2;
      break;
    case 5:
      tdd_ul_dl_cfg_common->pattern1.dl_ul_tx_periodicity = tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms5;
      break;
    case 10:
      tdd_ul_dl_cfg_common->pattern1.dl_ul_tx_periodicity = tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms10;
      break;
    default:
      asn1::log_warning("Invalid option for dl_ul_tx_periodicity_opts %d",
                        srsran_duplex_config_nr.tdd.pattern1.period_ms);
      return false;
  }
  tdd_ul_dl_cfg_common->pattern1.nrof_dl_slots   = srsran_duplex_config_nr.tdd.pattern1.nof_dl_slots;
  tdd_ul_dl_cfg_common->pattern1.nrof_dl_symbols = srsran_duplex_config_nr.tdd.pattern1.nof_dl_symbols;
  tdd_ul_dl_cfg_common->pattern1.nrof_ul_slots   = srsran_duplex_config_nr.tdd.pattern1.nof_ul_slots;
  tdd_ul_dl_cfg_common->pattern1.nrof_ul_symbols = srsran_duplex_config_nr.tdd.pattern1.nof_ul_symbols;

  if (srsran_duplex_config_nr.tdd.pattern2.period_ms == 0) {
    return true;
  }

  tdd_ul_dl_cfg_common->pattern2_present = true;
  switch (srsran_duplex_config_nr.tdd.pattern2.period_ms) {
    case 1:
      tdd_ul_dl_cfg_common->pattern2.dl_ul_tx_periodicity.value = tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms1;
      break;
    case 2:
      tdd_ul_dl_cfg_common->pattern2.dl_ul_tx_periodicity.value = tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms2;
      break;
    case 5:
      tdd_ul_dl_cfg_common->pattern2.dl_ul_tx_periodicity.value = tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms5;
      break;
    case 10:
      tdd_ul_dl_cfg_common->pattern2.dl_ul_tx_periodicity.value = tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms10;
      break;
    default:
      asn1::log_warning("Invalid option for pattern2 dl_ul_tx_periodicity_opts %d",
                        srsran_duplex_config_nr.tdd.pattern2.period_ms);
      return false;
  }
  tdd_ul_dl_cfg_common->pattern2.nrof_dl_slots   = srsran_duplex_config_nr.tdd.pattern2.nof_dl_slots;
  tdd_ul_dl_cfg_common->pattern2.nrof_dl_symbols = srsran_duplex_config_nr.tdd.pattern2.nof_dl_symbols;
  tdd_ul_dl_cfg_common->pattern2.nrof_ul_slots   = srsran_duplex_config_nr.tdd.pattern2.nof_ul_slots;
  tdd_ul_dl_cfg_common->pattern2.nrof_ul_symbols = srsran_duplex_config_nr.tdd.pattern2.nof_ul_symbols;

  return true;
}

bool make_phy_harq_ack_cfg(const phys_cell_group_cfg_s& phys_cell_group_cfg,
                           srsran_harq_ack_cfg_hl_t*    in_srsran_ue_dl_nr_harq_ack_cfg)
{
  srsran_harq_ack_cfg_hl_t srsran_ue_dl_nr_harq_ack_cfg = {};
  switch (phys_cell_group_cfg.pdsch_harq_ack_codebook) {
    case phys_cell_group_cfg_s::pdsch_harq_ack_codebook_opts::dynamic_value:
      srsran_ue_dl_nr_harq_ack_cfg.harq_ack_codebook = srsran_pdsch_harq_ack_codebook_dynamic;
      break;
    case phys_cell_group_cfg_s::pdsch_harq_ack_codebook_opts::semi_static:
      srsran_ue_dl_nr_harq_ack_cfg.harq_ack_codebook = srsran_pdsch_harq_ack_codebook_semi_static;
      break;
    case phys_cell_group_cfg_s::pdsch_harq_ack_codebook_opts::nulltype:
      srsran_ue_dl_nr_harq_ack_cfg.harq_ack_codebook = srsran_pdsch_harq_ack_codebook_none;
      break;
    default:
      asn1::log_warning("Invalid option for pdsch_harq_ack_codebook %s",
                        phys_cell_group_cfg.pdsch_harq_ack_codebook.to_string());
      return false;
  }
  *in_srsran_ue_dl_nr_harq_ack_cfg = srsran_ue_dl_nr_harq_ack_cfg;
  return true;
}

void make_phy_search_space0_cfg(srsran_search_space_t* in_srsran_search_space)
{
  in_srsran_search_space->id                = 0;
  in_srsran_search_space->coreset_id        = 0;
  in_srsran_search_space->type              = srsran_search_space_type_common_0;
  in_srsran_search_space->nof_candidates[0] = 0;
  in_srsran_search_space->nof_candidates[1] = 0;
  in_srsran_search_space->nof_candidates[2] = 4;
  in_srsran_search_space->nof_candidates[3] = 2;
  in_srsran_search_space->nof_candidates[4] = 0;
  in_srsran_search_space->nof_formats       = 1;
  in_srsran_search_space->formats[0]        = srsran_dci_format_nr_1_0;
  in_srsran_search_space->duration          = 1;
}

bool make_phy_search_space_cfg(const search_space_s& search_space, srsran_search_space_t* in_srsran_search_space)
{
  srsran_search_space_t srsran_search_space = {};
  srsran_search_space.id                    = search_space.search_space_id;
  if (not search_space.ctrl_res_set_id_present) {
    asn1::log_warning("ctrl_res_set_id option not present");
    return false;
  }
  srsran_search_space.coreset_id = search_space.ctrl_res_set_id;

  srsran_search_space.duration = 1;
  if (search_space.dur_present) {
    srsran_search_space.duration = search_space.dur;
  }

  if (not search_space.nrof_candidates_present) {
    asn1::log_warning("nrof_candidates_present option not present");
    return false;
  }
  srsran_search_space.nof_candidates[0] = search_space.nrof_candidates.aggregation_level1.to_number();
  srsran_search_space.nof_candidates[1] = search_space.nrof_candidates.aggregation_level2.to_number();
  srsran_search_space.nof_candidates[2] = search_space.nrof_candidates.aggregation_level4.to_number();
  srsran_search_space.nof_candidates[3] = search_space.nrof_candidates.aggregation_level8.to_number();
  srsran_search_space.nof_candidates[4] = search_space.nrof_candidates.aggregation_level16.to_number();

  if (not search_space.search_space_type_present) {
    asn1::log_warning("nrof_candidates option not present");
    return false;
  }
  switch (search_space.search_space_type.type()) {
    case search_space_s::search_space_type_c_::types_opts::options::common:
      srsran_search_space.type = srsran_search_space_type_common_3;

      // dci-Format0-0-AndFormat1-0
      // If configured, the UE monitors the DCI formats 0_0 and 1_0 according to TS 38.213 [13], clause 10.1.
      if (search_space.search_space_type.common().dci_format0_minus0_and_format1_minus0_present) {
        srsran_search_space.formats[srsran_search_space.nof_formats++] = srsran_dci_format_nr_0_0;
        srsran_search_space.formats[srsran_search_space.nof_formats++] = srsran_dci_format_nr_1_0;
      }

      // dci-Format2-0
      // If configured, UE monitors the DCI format 2_0 according to TS 38.213 [13], clause 10.1, 11.1.1.
      if (search_space.search_space_type.common().dci_format2_minus0_present) {
        srsran_search_space.formats[srsran_search_space.nof_formats++] = srsran_dci_format_nr_2_0;
      }

      // dci-Format2-1
      // If configured, UE monitors the DCI format 2_1 according to TS 38.213 [13], clause 10.1, 11.2.
      if (search_space.search_space_type.common().dci_format2_minus1_present) {
        srsran_search_space.formats[srsran_search_space.nof_formats++] = srsran_dci_format_nr_2_1;
      }

      // dci-Format2-2
      // If configured, UE monitors the DCI format 2_2 according to TS 38.213 [13], clause 10.1, 11.3.
      if (search_space.search_space_type.common().dci_format2_minus2_present) {
        srsran_search_space.formats[srsran_search_space.nof_formats++] = srsran_dci_format_nr_2_2;
      }

      // dci-Format2-3
      // If configured, UE monitors the DCI format 2_3 according to TS 38.213 [13], clause 10.1, 11.4
      if (search_space.search_space_type.common().dci_format2_minus3_present) {
        srsran_search_space.formats[srsran_search_space.nof_formats++] = srsran_dci_format_nr_2_3;
      }

      break;
    case search_space_s::search_space_type_c_::types_opts::options::ue_specific:
      srsran_search_space.type = srsran_search_space_type_ue;
      switch (search_space.search_space_type.ue_specific().dci_formats.value) {
        case search_space_s::search_space_type_c_::ue_specific_s_::dci_formats_e_::formats0_minus0_and_minus1_minus0:
          srsran_search_space.formats[srsran_search_space.nof_formats++] = srsran_dci_format_nr_0_0;
          srsran_search_space.formats[srsran_search_space.nof_formats++] = srsran_dci_format_nr_1_0;
          break;
        case search_space_s::search_space_type_c_::ue_specific_s_::dci_formats_e_::formats0_minus1_and_minus1_minus1:
          srsran_search_space.formats[srsran_search_space.nof_formats++] = srsran_dci_format_nr_0_1;
          srsran_search_space.formats[srsran_search_space.nof_formats++] = srsran_dci_format_nr_1_1;
          break;
      }
      break;
    default:
      asn1::log_warning("Invalid option for search_space_type %s", search_space.search_space_type.type().to_string());
      return false;
  }
  // Copy struct and return value
  *in_srsran_search_space = srsran_search_space;
  return true;
}

bool make_phy_csi_report(const csi_report_cfg_s&     csi_report_cfg,
                         srsran_csi_hl_report_cfg_t* in_srsran_csi_hl_report_cfg)
{
  srsran_csi_hl_report_cfg_t srsran_csi_hl_report_cfg = {};
  switch (csi_report_cfg.report_cfg_type.type()) {
    case csi_report_cfg_s::report_cfg_type_c_::types_opts::options::nulltype:
      srsran_csi_hl_report_cfg.type = SRSRAN_CSI_REPORT_TYPE_NONE;
      break;
    case csi_report_cfg_s::report_cfg_type_c_::types_opts::options::periodic:
      srsran_csi_hl_report_cfg.type = SRSRAN_CSI_REPORT_TYPE_PERIODIC;
      break;
    case csi_report_cfg_s::report_cfg_type_c_::types_opts::options::aperiodic:
      srsran_csi_hl_report_cfg.type = SRSRAN_CSI_REPORT_TYPE_APERIODIC;
      break;
    case csi_report_cfg_s::report_cfg_type_c_::types_opts::options::semi_persistent_on_pucch:
      srsran_csi_hl_report_cfg.type = SRSRAN_CSI_REPORT_TYPE_SEMI_PERSISTENT_ON_PUCCH;
      break;
    case csi_report_cfg_s::report_cfg_type_c_::types_opts::options::semi_persistent_on_pusch:
      srsran_csi_hl_report_cfg.type = SRSRAN_CSI_REPORT_TYPE_SEMI_PERSISTENT_ON_PUSCH;
      break;
    default:
      asn1::log_warning("Invalid option for report_cfg_type %s", csi_report_cfg.report_cfg_type.type().to_string());
      return false;
  }

  if (srsran_csi_hl_report_cfg.type == SRSRAN_CSI_REPORT_TYPE_PERIODIC) {
    const auto& csi_periodic                 = csi_report_cfg.report_cfg_type.periodic();
    srsran_csi_hl_report_cfg.periodic.period = csi_periodic.report_slot_cfg.type().to_number();
    switch (csi_periodic.report_slot_cfg.type()) {
      case csi_report_periodicity_and_offset_c::types_opts::slots4:
        srsran_csi_hl_report_cfg.periodic.offset = csi_periodic.report_slot_cfg.slots4();
        break;
      case csi_report_periodicity_and_offset_c::types_opts::slots5:
        srsran_csi_hl_report_cfg.periodic.offset = csi_periodic.report_slot_cfg.slots5();
        break;
      case csi_report_periodicity_and_offset_c::types_opts::slots8:
        srsran_csi_hl_report_cfg.periodic.offset = csi_periodic.report_slot_cfg.slots8();
        break;
      case csi_report_periodicity_and_offset_c::types_opts::slots10:
        srsran_csi_hl_report_cfg.periodic.offset = csi_periodic.report_slot_cfg.slots10();
        break;
      case csi_report_periodicity_and_offset_c::types_opts::slots16:
        srsran_csi_hl_report_cfg.periodic.offset = csi_periodic.report_slot_cfg.slots16();
        break;
      case csi_report_periodicity_and_offset_c::types_opts::slots20:
        srsran_csi_hl_report_cfg.periodic.offset = csi_periodic.report_slot_cfg.slots20();
        break;
      case csi_report_periodicity_and_offset_c::types_opts::slots40:
        srsran_csi_hl_report_cfg.periodic.offset = csi_periodic.report_slot_cfg.slots40();
        break;
      case csi_report_periodicity_and_offset_c::types_opts::slots80:
        srsran_csi_hl_report_cfg.periodic.offset = csi_periodic.report_slot_cfg.slots80();
        break;
      case csi_report_periodicity_and_offset_c::types_opts::slots160:
        srsran_csi_hl_report_cfg.periodic.offset = csi_periodic.report_slot_cfg.slots160();
        break;
      case csi_report_periodicity_and_offset_c::types_opts::slots320:
        srsran_csi_hl_report_cfg.periodic.offset = csi_periodic.report_slot_cfg.slots320();
        break;
      default:
        asn1::log_warning("Invalid option for report_slot_cfg %s", csi_periodic.report_slot_cfg.type().to_string());
        return false;
    }
  }

  srsran_csi_hl_report_cfg.channel_meas_id = csi_report_cfg.res_for_ch_meas;

  srsran_csi_hl_report_cfg.interf_meas_present = csi_report_cfg.csi_im_res_for_interference_present;
  srsran_csi_hl_report_cfg.interf_meas_id      = csi_report_cfg.csi_im_res_for_interference;

  switch (csi_report_cfg.report_quant.type()) {
    case csi_report_cfg_s::report_quant_c_::types_opts::none:
      srsran_csi_hl_report_cfg.quantity = SRSRAN_CSI_REPORT_QUANTITY_NONE;
      break;
    case csi_report_cfg_s::report_quant_c_::types_opts::cri_ri_pmi_cqi:
      srsran_csi_hl_report_cfg.quantity = SRSRAN_CSI_REPORT_QUANTITY_CRI_RI_PMI_CQI;
      break;
    case csi_report_cfg_s::report_quant_c_::types_opts::cri_ri_i1:
      srsran_csi_hl_report_cfg.quantity = SRSRAN_CSI_REPORT_QUANTITY_CRI_RI_I1;
      break;
    case csi_report_cfg_s::report_quant_c_::types_opts::cri_ri_i1_cqi:
      srsran_csi_hl_report_cfg.quantity = SRSRAN_CSI_REPORT_QUANTITY_CRI_RI_I1_CQI;
      break;
    case csi_report_cfg_s::report_quant_c_::types_opts::cri_ri_cqi:
      srsran_csi_hl_report_cfg.quantity = SRSRAN_CSI_REPORT_QUANTITY_CRI_RI_CQI;
      break;
    case csi_report_cfg_s::report_quant_c_::types_opts::cri_rsrp:
      srsran_csi_hl_report_cfg.quantity = SRSRAN_CSI_REPORT_QUANTITY_CRI_RSRP;
      break;
    case csi_report_cfg_s::report_quant_c_::types_opts::ssb_idx_rsrp:
      srsran_csi_hl_report_cfg.quantity = SRSRAN_CSI_REPORT_QUANTITY_SSB_INDEX_RSRP;
      break;
    case csi_report_cfg_s::report_quant_c_::types_opts::cri_ri_li_pmi_cqi:
      srsran_csi_hl_report_cfg.quantity = SRSRAN_CSI_REPORT_QUANTITY_CRI_RI_LI_PMI_CQI;
      break;
    default:
      asn1::log_warning("Invalid option for report_quant %s", csi_report_cfg.report_quant.type().to_string());
      return false;
  }

  if (not csi_report_cfg.report_freq_cfg_present) {
    asn1::log_warning("report_freq_cfg_present option not present");
    return false;
  }

  if (not csi_report_cfg.report_freq_cfg.cqi_format_ind_present) {
    asn1::log_warning("cqi_format_ind option not present");
    return false;
  }

  switch (csi_report_cfg.report_freq_cfg.cqi_format_ind) {
    case csi_report_cfg_s::report_freq_cfg_s_::cqi_format_ind_opts::wideband_cqi:
      srsran_csi_hl_report_cfg.freq_cfg = SRSRAN_CSI_REPORT_FREQ_WIDEBAND;
      break;
    case csi_report_cfg_s::report_freq_cfg_s_::cqi_format_ind_opts::subband_cqi:
      srsran_csi_hl_report_cfg.freq_cfg = SRSRAN_CSI_REPORT_FREQ_SUBBAND;
      break;
    default:
      asn1::log_warning("Invalid option for cqi_format_ind %s",
                        csi_report_cfg.report_freq_cfg.cqi_format_ind.to_string());
      return false;

      break;
  }

  if (not csi_report_cfg.cqi_table_present) {
    asn1::log_warning("cqi_table_present not present");
    return false;
  }

  switch (csi_report_cfg.cqi_table) {
    case csi_report_cfg_s::cqi_table_opts::table1:
      srsran_csi_hl_report_cfg.cqi_table = SRSRAN_CSI_CQI_TABLE_1;
      break;
    case csi_report_cfg_s::cqi_table_opts::table2:
      srsran_csi_hl_report_cfg.cqi_table = SRSRAN_CSI_CQI_TABLE_2;
      break;
    case csi_report_cfg_s::cqi_table_opts::table3:
      srsran_csi_hl_report_cfg.cqi_table = SRSRAN_CSI_CQI_TABLE_3;
      break;
    default:
      asn1::log_warning("Invalid option for cqi_table %s", csi_report_cfg.cqi_table.to_string());
      return false;
  }

  *in_srsran_csi_hl_report_cfg = srsran_csi_hl_report_cfg;
  return true;
}

bool make_phy_coreset_cfg(const ctrl_res_set_s& ctrl_res_set, srsran_coreset_t* in_srsran_coreset)
{
  srsran_coreset_t srsran_coreset = {};
  srsran_coreset.id               = ctrl_res_set.ctrl_res_set_id;
  switch (ctrl_res_set.precoder_granularity) {
    case ctrl_res_set_s::precoder_granularity_opts::same_as_reg_bundle:
      srsran_coreset.precoder_granularity = srsran_coreset_precoder_granularity_reg_bundle;
      break;
    case ctrl_res_set_s::precoder_granularity_opts::all_contiguous_rbs:
      srsran_coreset.precoder_granularity = srsran_coreset_precoder_granularity_contiguous;
    default:
      asn1::log_warning("Invalid option for precoder_granularity %s", ctrl_res_set.precoder_granularity.to_string());
      return false;
  };

  switch (ctrl_res_set.cce_reg_map_type.type()) {
    case ctrl_res_set_s::cce_reg_map_type_c_::types_opts::options::interleaved:
      srsran_coreset.mapping_type = srsran_coreset_mapping_type_interleaved;
      break;
    case ctrl_res_set_s::cce_reg_map_type_c_::types_opts::options::non_interleaved:
      srsran_coreset.mapping_type = srsran_coreset_mapping_type_non_interleaved;
      break;
    default:
      asn1::log_warning("Invalid option for cce_reg_map_type: %s", ctrl_res_set.cce_reg_map_type.type().to_string());
      return false;
  }
  srsran_coreset.duration = ctrl_res_set.dur;
  for (uint32_t i = 0; i < SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE; i++) {
    srsran_coreset.freq_resources[i] = ctrl_res_set.freq_domain_res.get(SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE - 1 - i);
  }
  *in_srsran_coreset = srsran_coreset;
  return true;
}

bool make_phy_common_time_ra(const pdsch_time_domain_res_alloc_s& pdsch_time_domain_res_alloc,
                             srsran_sch_time_ra_t*                in_srsran_sch_time_ra)
{
  srsran_sch_time_ra_t srsran_sch_time_ra = {};
  srsran_sch_time_ra.sliv                 = pdsch_time_domain_res_alloc.start_symbol_and_len;
  switch (pdsch_time_domain_res_alloc.map_type) {
    case pdsch_time_domain_res_alloc_s::map_type_opts::type_a:
      srsran_sch_time_ra.mapping_type = srsran_sch_mapping_type_A;
      break;
    case pdsch_time_domain_res_alloc_s::map_type_opts::type_b:
      srsran_sch_time_ra.mapping_type = srsran_sch_mapping_type_B;
      break;
    default:
      asn1::log_warning("Invalid option for map_type: %s", pdsch_time_domain_res_alloc.map_type.to_string());
      return false;
  }

  if (pdsch_time_domain_res_alloc.k0_present) {
    srsran_sch_time_ra.k = pdsch_time_domain_res_alloc.k0;
  } else {
    srsran_sch_time_ra.k = 0;
  }
  *in_srsran_sch_time_ra = srsran_sch_time_ra;
  return true;
}

bool make_phy_common_time_ra(const pusch_time_domain_res_alloc_s& pusch_time_domain_res_alloc,
                             srsran_sch_time_ra_t*                in_srsran_sch_time_ra)
{
  srsran_sch_time_ra_t srsran_sch_time_ra = {};
  srsran_sch_time_ra.sliv                 = pusch_time_domain_res_alloc.start_symbol_and_len;
  switch (pusch_time_domain_res_alloc.map_type) {
    case pusch_time_domain_res_alloc_s::map_type_opts::type_a:
      srsran_sch_time_ra.mapping_type = srsran_sch_mapping_type_A;
      break;
    case pusch_time_domain_res_alloc_s::map_type_opts::type_b:
      srsran_sch_time_ra.mapping_type = srsran_sch_mapping_type_B;
      break;
    default:
      asn1::log_warning("Invalid option for map_type: %s", pusch_time_domain_res_alloc.map_type.to_string());
      return false;
  }

  if (pusch_time_domain_res_alloc.k2_present) {
    srsran_sch_time_ra.k = pusch_time_domain_res_alloc.k2;
  } else {
    srsran_sch_time_ra.k = 0;
  }
  *in_srsran_sch_time_ra = srsran_sch_time_ra;
  return true;
}

bool make_phy_max_code_rate(const pucch_format_cfg_s& pucch_format_cfg, uint32_t* in_max_code_rate)
{
  if (not pucch_format_cfg.max_code_rate_present) {
    asn1::log_warning("max_code_rate option not present");
    return false;
  }
  *in_max_code_rate = pucch_format_cfg.max_code_rate.value;
  return true;
}

bool make_phy_res_config(const pucch_res_s&          pucch_res,
                         uint32_t                    format_2_max_code_rate,
                         srsran_pucch_nr_resource_t* in_srsran_pucch_nr_resource)
{
  srsran_pucch_nr_resource_t srsran_pucch_nr_resource = {};
  srsran_pucch_nr_resource.starting_prb               = pucch_res.start_prb;
  srsran_pucch_nr_resource.intra_slot_hopping         = pucch_res.intra_slot_freq_hop_present;
  if (pucch_res.second_hop_prb_present) {
    srsran_pucch_nr_resource.second_hop_prb = pucch_res.second_hop_prb;
  }
  switch (pucch_res.format.type()) {
    case pucch_res_s::format_c_::types_opts::format0:
      srsran_pucch_nr_resource.format = SRSRAN_PUCCH_NR_FORMAT_0;
      break;
    case pucch_res_s::format_c_::types_opts::format1:
      srsran_pucch_nr_resource.format               = SRSRAN_PUCCH_NR_FORMAT_1;
      srsran_pucch_nr_resource.initial_cyclic_shift = pucch_res.format.format1().init_cyclic_shift;
      srsran_pucch_nr_resource.nof_symbols          = pucch_res.format.format1().nrof_symbols;
      srsran_pucch_nr_resource.start_symbol_idx     = pucch_res.format.format1().start_symbol_idx;
      srsran_pucch_nr_resource.time_domain_occ      = pucch_res.format.format1().time_domain_occ;
      break;
    case pucch_res_s::format_c_::types_opts::format2:
      srsran_pucch_nr_resource.format           = SRSRAN_PUCCH_NR_FORMAT_2;
      srsran_pucch_nr_resource.nof_symbols      = pucch_res.format.format2().nrof_symbols;
      srsran_pucch_nr_resource.start_symbol_idx = pucch_res.format.format2().start_symbol_idx;
      srsran_pucch_nr_resource.nof_prb          = pucch_res.format.format2().nrof_prbs;
      break;
    case pucch_res_s::format_c_::types_opts::format3:
      srsran_pucch_nr_resource.format = SRSRAN_PUCCH_NR_FORMAT_3;
      asn1::log_warning("SRSRAN_PUCCH_NR_FORMAT_3 conversion not supported");
      return false;
    case pucch_res_s::format_c_::types_opts::format4:
      srsran_pucch_nr_resource.format = SRSRAN_PUCCH_NR_FORMAT_4;
      asn1::log_warning("SRSRAN_PUCCH_NR_FORMAT_4 conversion not supported");
      return false;
    default:
      srsran_pucch_nr_resource.format = SRSRAN_PUCCH_NR_FORMAT_ERROR;
      return false;
  }
  srsran_pucch_nr_resource.max_code_rate = format_2_max_code_rate;
  *in_srsran_pucch_nr_resource           = srsran_pucch_nr_resource;
  return true;
}

bool make_phy_res_config(const srsran_pucch_nr_resource_t& in_pucch_res,
                         asn1::rrc_nr::pucch_res_s&        out_pucch_res,
                         uint32_t                          pucch_res_id)
{
  out_pucch_res.pucch_res_id = pucch_res_id;
  out_pucch_res.start_prb    = in_pucch_res.starting_prb;

  switch (in_pucch_res.format) {
    case SRSRAN_PUCCH_NR_FORMAT_0:
      asn1::log_warning("SRSRAN_PUCCH_NR_FORMAT_0 conversion not supported");
      return false;
    case SRSRAN_PUCCH_NR_FORMAT_1:
      out_pucch_res.format.set_format1();
      out_pucch_res.format.format1().init_cyclic_shift = in_pucch_res.initial_cyclic_shift;
      out_pucch_res.format.format1().nrof_symbols      = in_pucch_res.nof_symbols;
      out_pucch_res.format.format1().start_symbol_idx  = in_pucch_res.start_symbol_idx;
      out_pucch_res.format.format1().time_domain_occ   = in_pucch_res.time_domain_occ;
      return true;
    case SRSRAN_PUCCH_NR_FORMAT_2:
      out_pucch_res.format.set_format2();
      out_pucch_res.format.format2().nrof_symbols     = in_pucch_res.nof_symbols;
      out_pucch_res.format.format2().start_symbol_idx = in_pucch_res.start_symbol_idx;
      out_pucch_res.format.format2().nrof_prbs        = in_pucch_res.nof_prb;
      return true;
    case SRSRAN_PUCCH_NR_FORMAT_3:
      asn1::log_warning("SRSRAN_PUCCH_NR_FORMAT_3 conversion not supported");
      return true;
    case SRSRAN_PUCCH_NR_FORMAT_4:
      asn1::log_warning("SRSRAN_PUCCH_NR_FORMAT_4 conversion not supported");
      return false;
    default:
      asn1::log_warning("Invalid NR PUCCH format");
  }
  return false;
}

bool make_phy_sr_resource(const sched_request_res_cfg_s& sched_request_res_cfg,
                          srsran_pucch_nr_sr_resource_t* in_srsran_pucch_nr_sr_resource)
{
  srsran_pucch_nr_sr_resource_t srsran_pucch_nr_sr_resource = {};
  srsran_pucch_nr_sr_resource.sr_id                         = sched_request_res_cfg.sched_request_id;
  if (sched_request_res_cfg.periodicity_and_offset_present && sched_request_res_cfg.res_present) {
    srsran_pucch_nr_sr_resource.configured = true;
    switch (sched_request_res_cfg.periodicity_and_offset.type()) {
      case sched_request_res_cfg_s::periodicity_and_offset_c_::types_opts::sl2:
        srsran_pucch_nr_sr_resource.period = 2;
        srsran_pucch_nr_sr_resource.offset = sched_request_res_cfg.periodicity_and_offset.sl2();
        break;
      case sched_request_res_cfg_s::periodicity_and_offset_c_::types_opts::sl4:
        srsran_pucch_nr_sr_resource.period = 4;
        srsran_pucch_nr_sr_resource.offset = sched_request_res_cfg.periodicity_and_offset.sl4();
        break;
      case sched_request_res_cfg_s::periodicity_and_offset_c_::types_opts::sl5:
        srsran_pucch_nr_sr_resource.period = 5;
        srsran_pucch_nr_sr_resource.offset = sched_request_res_cfg.periodicity_and_offset.sl5();
        break;
      case sched_request_res_cfg_s::periodicity_and_offset_c_::types_opts::sl8:
        srsran_pucch_nr_sr_resource.period = 8;
        srsran_pucch_nr_sr_resource.offset = sched_request_res_cfg.periodicity_and_offset.sl8();
        break;
      case sched_request_res_cfg_s::periodicity_and_offset_c_::types_opts::sl10:
        srsran_pucch_nr_sr_resource.period = 10;
        srsran_pucch_nr_sr_resource.offset = sched_request_res_cfg.periodicity_and_offset.sl10();
        break;
      case sched_request_res_cfg_s::periodicity_and_offset_c_::types_opts::sl16:
        srsran_pucch_nr_sr_resource.period = 16;
        srsran_pucch_nr_sr_resource.offset = sched_request_res_cfg.periodicity_and_offset.sl16();
        break;
      case sched_request_res_cfg_s::periodicity_and_offset_c_::types_opts::sl20:
        srsran_pucch_nr_sr_resource.period = 20;
        srsran_pucch_nr_sr_resource.offset = sched_request_res_cfg.periodicity_and_offset.sl20();
        break;
      case sched_request_res_cfg_s::periodicity_and_offset_c_::types_opts::sl40:
        srsran_pucch_nr_sr_resource.period = 40;
        srsran_pucch_nr_sr_resource.offset = sched_request_res_cfg.periodicity_and_offset.sl40();
        break;
      case sched_request_res_cfg_s::periodicity_and_offset_c_::types_opts::sl80:
        srsran_pucch_nr_sr_resource.period = 80;
        srsran_pucch_nr_sr_resource.offset = sched_request_res_cfg.periodicity_and_offset.sl80();
        break;
      case sched_request_res_cfg_s::periodicity_and_offset_c_::types_opts::sl160:
        srsran_pucch_nr_sr_resource.period = 160;
        srsran_pucch_nr_sr_resource.offset = sched_request_res_cfg.periodicity_and_offset.sl160();
        break;
      case sched_request_res_cfg_s::periodicity_and_offset_c_::types_opts::sl320:
        srsran_pucch_nr_sr_resource.period = 320;
        srsran_pucch_nr_sr_resource.offset = sched_request_res_cfg.periodicity_and_offset.sl320();
        break;
      case sched_request_res_cfg_s::periodicity_and_offset_c_::types_opts::sl640:
        srsran_pucch_nr_sr_resource.period = 640;
        srsran_pucch_nr_sr_resource.offset = sched_request_res_cfg.periodicity_and_offset.sl640();
        break;
      case sched_request_res_cfg_s::periodicity_and_offset_c_::types_opts::sym2:
      case sched_request_res_cfg_s::periodicity_and_offset_c_::types_opts::sym6or7:
      case sched_request_res_cfg_s::periodicity_and_offset_c_::types_opts::sl1:
      default:
        srsran_pucch_nr_sr_resource.configured = false;
        asn1::log_warning("Invalid option for periodicity_and_offset %s",
                          sched_request_res_cfg.periodicity_and_offset.type().to_string());
        return false;
    }

  } else {
    srsran_pucch_nr_sr_resource.configured = false;
  }
  *in_srsran_pucch_nr_sr_resource = srsran_pucch_nr_sr_resource;
  return true;
}

bool make_phy_pusch_alloc_type(const asn1::rrc_nr::pusch_cfg_s& pusch_cfg,
                               srsran_resource_alloc_t*         in_srsran_resource_alloc)
{
  srsran_resource_alloc_t srsran_resource_alloc = {};

  switch (pusch_cfg.res_alloc) {
    case asn1::rrc_nr::pusch_cfg_s::res_alloc_e_::res_alloc_type0:
      srsran_resource_alloc = srsran_resource_alloc_type0;
      break;
    case asn1::rrc_nr::pusch_cfg_s::res_alloc_e_::res_alloc_type1:
      srsran_resource_alloc = srsran_resource_alloc_type1;
      break;
    case asn1::rrc_nr::pusch_cfg_s::res_alloc_e_::dynamic_switch:
      srsran_resource_alloc = srsran_resource_alloc_dynamic;
      break;
    default:
      asn1::log_warning("Invalid option for pusch::resource_alloc %s", pusch_cfg.res_alloc.to_string());
      return false;
  }
  *in_srsran_resource_alloc = srsran_resource_alloc;
  return true;
}

bool make_phy_pdsch_alloc_type(const asn1::rrc_nr::pdsch_cfg_s& pdsch_cfg,
                               srsran_resource_alloc_t*         in_srsran_resource_alloc)
{
  srsran_resource_alloc_t srsran_resource_alloc = {};

  switch (pdsch_cfg.res_alloc) {
    case asn1::rrc_nr::pdsch_cfg_s::res_alloc_e_::res_alloc_type0:
      srsran_resource_alloc = srsran_resource_alloc_type0;
      break;
    case asn1::rrc_nr::pdsch_cfg_s::res_alloc_e_::res_alloc_type1:
      srsran_resource_alloc = srsran_resource_alloc_type1;
      break;
    case asn1::rrc_nr::pdsch_cfg_s::res_alloc_e_::dynamic_switch:
      srsran_resource_alloc = srsran_resource_alloc_dynamic;
      break;
    default:
      asn1::log_warning("Invalid option for pusch::resource_alloc %s", pdsch_cfg.res_alloc.to_string());
      return false;
  }
  *in_srsran_resource_alloc = srsran_resource_alloc;
  return true;
}

bool make_phy_dmrs_dl_additional_pos(const dmrs_dl_cfg_s&       dmrs_dl_cfg,
                                     srsran_dmrs_sch_add_pos_t* in_srsran_dmrs_sch_add_pos)
{
  srsran_dmrs_sch_add_pos_t srsran_dmrs_sch_add_pos = {};
  if (not dmrs_dl_cfg.dmrs_add_position_present) {
    asn1::log_warning("dmrs_add_position option not present");
  }

  switch (dmrs_dl_cfg.dmrs_add_position) {
    case dmrs_dl_cfg_s::dmrs_add_position_opts::pos0:
      srsran_dmrs_sch_add_pos = srsran_dmrs_sch_add_pos_0;
      break;
    case dmrs_dl_cfg_s::dmrs_add_position_opts::pos1:
      srsran_dmrs_sch_add_pos = srsran_dmrs_sch_add_pos_1;
      break;
    case dmrs_dl_cfg_s::dmrs_add_position_opts::pos3:
      srsran_dmrs_sch_add_pos = srsran_dmrs_sch_add_pos_3;
      break;
    default:
      asn1::log_warning("Invalid option for dmrs_add_position %s", dmrs_dl_cfg.dmrs_add_position.to_string());
      return false;
  }
  *in_srsran_dmrs_sch_add_pos = srsran_dmrs_sch_add_pos;
  return true;
}

bool make_phy_dmrs_ul_additional_pos(const dmrs_ul_cfg_s&       dmrs_ul_cfg,
                                     srsran_dmrs_sch_add_pos_t* in_srsran_dmrs_sch_add_pos)
{
  srsran_dmrs_sch_add_pos_t srsran_dmrs_sch_add_pos = {};
  if (not dmrs_ul_cfg.dmrs_add_position_present) {
    asn1::log_warning("dmrs_add_position option not present");
  }

  switch (dmrs_ul_cfg.dmrs_add_position) {
    case dmrs_ul_cfg_s::dmrs_add_position_opts::pos0:
      srsran_dmrs_sch_add_pos = srsran_dmrs_sch_add_pos_0;
      break;
    case dmrs_ul_cfg_s::dmrs_add_position_opts::pos1:
      srsran_dmrs_sch_add_pos = srsran_dmrs_sch_add_pos_1;
      break;
    case dmrs_ul_cfg_s::dmrs_add_position_opts::pos3:
      srsran_dmrs_sch_add_pos = srsran_dmrs_sch_add_pos_3;
      break;
    default:
      asn1::log_warning("Invalid option for dmrs_add_position %s", dmrs_ul_cfg.dmrs_add_position.to_string());
      return false;
  }
  *in_srsran_dmrs_sch_add_pos = srsran_dmrs_sch_add_pos;
  return true;
}

bool make_phy_beta_offsets(const beta_offsets_s& beta_offsets, srsran_beta_offsets_t* in_srsran_beta_offsets)
{
  srsran_beta_offsets_t srsran_beta_offsets = {};

  srsran_beta_offsets.ack_index1 = beta_offsets.beta_offset_ack_idx1_present ? beta_offsets.beta_offset_ack_idx1 : 11;
  srsran_beta_offsets.ack_index2 = beta_offsets.beta_offset_ack_idx2_present ? beta_offsets.beta_offset_ack_idx2 : 11;
  srsran_beta_offsets.ack_index3 = beta_offsets.beta_offset_ack_idx3_present ? beta_offsets.beta_offset_ack_idx3 : 11;
  srsran_beta_offsets.csi1_index1 =
      beta_offsets.beta_offset_csi_part1_idx1_present ? beta_offsets.beta_offset_csi_part1_idx1 : 13;
  srsran_beta_offsets.csi1_index2 =
      beta_offsets.beta_offset_csi_part1_idx2_present ? beta_offsets.beta_offset_csi_part1_idx2 : 13;
  srsran_beta_offsets.csi2_index1 =
      beta_offsets.beta_offset_csi_part2_idx1_present ? beta_offsets.beta_offset_csi_part2_idx1 : 13;
  srsran_beta_offsets.csi2_index2 =
      beta_offsets.beta_offset_csi_part2_idx2_present ? beta_offsets.beta_offset_csi_part2_idx2 : 13;
  *in_srsran_beta_offsets = srsran_beta_offsets;
  return true;
}

bool make_phy_pusch_scaling(const uci_on_pusch_s& uci_on_pusch, float* in_scaling)
{
  float pusch_scaling = 0;
  switch (uci_on_pusch.scaling) {
    case uci_on_pusch_s::scaling_opts::f0p5:
      pusch_scaling = 0.5;
      break;
    case uci_on_pusch_s::scaling_opts::f0p65:
      pusch_scaling = 0.65;
      break;
    case uci_on_pusch_s::scaling_opts::f0p8:
      pusch_scaling = 0.8;
      break;
    case uci_on_pusch_s::scaling_opts::f1:
      pusch_scaling = 1.0;
      break;
    default:
      asn1::log_warning("Invalid option for scaling %s", uci_on_pusch.scaling.to_string());
      return false;
  }
  *in_scaling = pusch_scaling;
  return true;
}

bool make_phy_zp_csi_rs_resource(const asn1::rrc_nr::zp_csi_rs_res_s& zp_csi_rs_res,
                                 srsran_csi_rs_zp_resource_t*         out_zp_csi_rs_resource)
{
  srsran_csi_rs_zp_resource_t zp_csi_rs_resource = {};
  zp_csi_rs_resource.id                          = zp_csi_rs_res.zp_csi_rs_res_id;
  switch (zp_csi_rs_res.res_map.freq_domain_alloc.type()) {
    case csi_rs_res_map_s::freq_domain_alloc_c_::types_opts::options::row1:
      zp_csi_rs_resource.resource_mapping.row = srsran_csi_rs_resource_mapping_row_1;
      for (uint32_t i = 0; i < zp_csi_rs_res.res_map.freq_domain_alloc.row1().length(); i++) {
        zp_csi_rs_resource.resource_mapping.frequency_domain_alloc[i] =
            zp_csi_rs_res.res_map.freq_domain_alloc.row1().get(zp_csi_rs_res.res_map.freq_domain_alloc.row1().length() -
                                                               1 - i);
      }
      break;
    case csi_rs_res_map_s::freq_domain_alloc_c_::types_opts::options::row2:
      zp_csi_rs_resource.resource_mapping.row = srsran_csi_rs_resource_mapping_row_2;
      for (uint32_t i = 0; i < zp_csi_rs_res.res_map.freq_domain_alloc.row2().length(); i++) {
        zp_csi_rs_resource.resource_mapping.frequency_domain_alloc[i] =
            zp_csi_rs_res.res_map.freq_domain_alloc.row2().get(zp_csi_rs_res.res_map.freq_domain_alloc.row2().length() -
                                                               1 - i);
      }
      break;
    case csi_rs_res_map_s::freq_domain_alloc_c_::types_opts::options::row4:
      zp_csi_rs_resource.resource_mapping.row = srsran_csi_rs_resource_mapping_row_4;
      for (uint32_t i = 0; i < zp_csi_rs_res.res_map.freq_domain_alloc.row4().length(); i++) {
        zp_csi_rs_resource.resource_mapping.frequency_domain_alloc[i] =
            zp_csi_rs_res.res_map.freq_domain_alloc.row4().get(zp_csi_rs_res.res_map.freq_domain_alloc.row4().length() -
                                                               1 - i);
      }
      break;
    case csi_rs_res_map_s::freq_domain_alloc_c_::types_opts::options::other:
      zp_csi_rs_resource.resource_mapping.row = srsran_csi_rs_resource_mapping_row_other;
      break;
    default:
      asn1::log_warning("Invalid option for freq_domain_alloc %s",
                        zp_csi_rs_res.res_map.freq_domain_alloc.type().to_string());
      return false;
  }
  zp_csi_rs_resource.resource_mapping.nof_ports        = zp_csi_rs_res.res_map.nrof_ports.to_number();
  zp_csi_rs_resource.resource_mapping.first_symbol_idx = zp_csi_rs_res.res_map.first_ofdm_symbol_in_time_domain;

  switch (zp_csi_rs_res.res_map.cdm_type) {
    case csi_rs_res_map_s::cdm_type_opts::options::no_cdm:
      zp_csi_rs_resource.resource_mapping.cdm = srsran_csi_rs_cdm_t::srsran_csi_rs_cdm_nocdm;
      break;
    case csi_rs_res_map_s::cdm_type_opts::options::fd_cdm2:
      zp_csi_rs_resource.resource_mapping.cdm = srsran_csi_rs_cdm_t::srsran_csi_rs_cdm_fd_cdm2;
      break;
    case csi_rs_res_map_s::cdm_type_opts::options::cdm4_fd2_td2:
      zp_csi_rs_resource.resource_mapping.cdm = srsran_csi_rs_cdm_t::srsran_csi_rs_cdm_cdm4_fd2_td2;
      break;
    case csi_rs_res_map_s::cdm_type_opts::options::cdm8_fd2_td4:
      zp_csi_rs_resource.resource_mapping.cdm = srsran_csi_rs_cdm_t::srsran_csi_rs_cdm_cdm8_fd2_td4;
      break;
    default:
      asn1::log_warning("Invalid option for cdm_type %s", zp_csi_rs_res.res_map.cdm_type.to_string());
      return false;
  }

  switch (zp_csi_rs_res.res_map.density.type()) {
    case csi_rs_res_map_s::density_c_::types_opts::options::dot5:
      switch (zp_csi_rs_res.res_map.density.dot5()) {
        case csi_rs_res_map_s::density_c_::dot5_opts::options::even_prbs:
          zp_csi_rs_resource.resource_mapping.density = srsran_csi_rs_resource_mapping_density_dot5_even;
          break;
        case csi_rs_res_map_s::density_c_::dot5_opts::options::odd_prbs:
          zp_csi_rs_resource.resource_mapping.density = srsran_csi_rs_resource_mapping_density_dot5_odd;
          break;
        default:
          asn1::log_warning("Invalid option for dot5 %s", zp_csi_rs_res.res_map.density.dot5().to_string());
          return false;
      }
      break;
    case csi_rs_res_map_s::density_c_::types_opts::options::one:
      zp_csi_rs_resource.resource_mapping.density = srsran_csi_rs_resource_mapping_density_one;
      break;
    case csi_rs_res_map_s::density_c_::types_opts::options::three:
      zp_csi_rs_resource.resource_mapping.density = srsran_csi_rs_resource_mapping_density_three;
      break;
    case csi_rs_res_map_s::density_c_::types_opts::options::spare:
      zp_csi_rs_resource.resource_mapping.density = srsran_csi_rs_resource_mapping_density_spare;
      break;
    default:
      asn1::log_warning("Invalid option for density %s", zp_csi_rs_res.res_map.density.type().to_string());
      return false;
  }
  zp_csi_rs_resource.resource_mapping.freq_band.nof_rb   = zp_csi_rs_res.res_map.freq_band.nrof_rbs;
  zp_csi_rs_resource.resource_mapping.freq_band.start_rb = zp_csi_rs_res.res_map.freq_band.start_rb;

  // Validate CSI-RS resource mapping
  if (not srsran_csi_rs_resource_mapping_is_valid(&zp_csi_rs_resource.resource_mapping)) {
    asn1::json_writer json_writer;
    zp_csi_rs_res.res_map.to_json(json_writer);
    asn1::log_error("Resource mapping is invalid or not implemented: %s", json_writer.to_string());
    return false;
  }

  if (zp_csi_rs_res.periodicity_and_offset_present) {
    switch (zp_csi_rs_res.periodicity_and_offset.type()) {
      case csi_res_periodicity_and_offset_c::types_opts::options::slots4:
        zp_csi_rs_resource.periodicity.period = 4;
        zp_csi_rs_resource.periodicity.offset = zp_csi_rs_res.periodicity_and_offset.slots4();
        break;
      case csi_res_periodicity_and_offset_c::types_opts::options::slots5:
        zp_csi_rs_resource.periodicity.period = 5;
        zp_csi_rs_resource.periodicity.offset = zp_csi_rs_res.periodicity_and_offset.slots5();
        break;
      case csi_res_periodicity_and_offset_c::types_opts::options::slots8:
        zp_csi_rs_resource.periodicity.period = 8;
        zp_csi_rs_resource.periodicity.offset = zp_csi_rs_res.periodicity_and_offset.slots8();
        break;
      case csi_res_periodicity_and_offset_c::types_opts::options::slots10:
        zp_csi_rs_resource.periodicity.period = 10;
        zp_csi_rs_resource.periodicity.offset = zp_csi_rs_res.periodicity_and_offset.slots10();
        break;
      case csi_res_periodicity_and_offset_c::types_opts::options::slots16:
        zp_csi_rs_resource.periodicity.period = 16;
        zp_csi_rs_resource.periodicity.offset = zp_csi_rs_res.periodicity_and_offset.slots16();
        break;
      case csi_res_periodicity_and_offset_c::types_opts::options::slots20:
        zp_csi_rs_resource.periodicity.period = 20;
        zp_csi_rs_resource.periodicity.offset = zp_csi_rs_res.periodicity_and_offset.slots20();
        break;
      case csi_res_periodicity_and_offset_c::types_opts::options::slots32:
        zp_csi_rs_resource.periodicity.period = 32;
        zp_csi_rs_resource.periodicity.offset = zp_csi_rs_res.periodicity_and_offset.slots32();
        break;
      case csi_res_periodicity_and_offset_c::types_opts::options::slots40:
        zp_csi_rs_resource.periodicity.period = 40;
        zp_csi_rs_resource.periodicity.offset = zp_csi_rs_res.periodicity_and_offset.slots40();
        break;
      case csi_res_periodicity_and_offset_c::types_opts::options::slots64:
        zp_csi_rs_resource.periodicity.period = 64;
        zp_csi_rs_resource.periodicity.offset = zp_csi_rs_res.periodicity_and_offset.slots64();
        break;
      case csi_res_periodicity_and_offset_c::types_opts::options::slots80:
        zp_csi_rs_resource.periodicity.period = 80;
        zp_csi_rs_resource.periodicity.offset = zp_csi_rs_res.periodicity_and_offset.slots80();
        break;
      case csi_res_periodicity_and_offset_c::types_opts::options::slots160:
        zp_csi_rs_resource.periodicity.period = 160;
        zp_csi_rs_resource.periodicity.offset = zp_csi_rs_res.periodicity_and_offset.slots160();
        break;
      case csi_res_periodicity_and_offset_c::types_opts::options::slots320:
        zp_csi_rs_resource.periodicity.period = 320;
        zp_csi_rs_resource.periodicity.offset = zp_csi_rs_res.periodicity_and_offset.slots320();
        break;
      case csi_res_periodicity_and_offset_c::types_opts::options::slots640:
        zp_csi_rs_resource.periodicity.period = 640;
        zp_csi_rs_resource.periodicity.offset = zp_csi_rs_res.periodicity_and_offset.slots640();
        break;
      default:
        asn1::log_warning("Invalid option for periodicity_and_offset %s",
                          zp_csi_rs_res.periodicity_and_offset.type().to_string());
        return false;
    }
  } else {
    asn1::log_warning("Option periodicity_and_offset not present");
    return false;
  }

  *out_zp_csi_rs_resource = zp_csi_rs_resource;
  return true;
}

bool make_phy_nzp_csi_rs_resource(const asn1::rrc_nr::nzp_csi_rs_res_s& asn1_nzp_csi_rs_res,
                                  srsran_csi_rs_nzp_resource_t*         out_csi_rs_nzp_resource)
{
  srsran_csi_rs_nzp_resource_t csi_rs_nzp_resource = {};
  csi_rs_nzp_resource.id                           = asn1_nzp_csi_rs_res.nzp_csi_rs_res_id;
  switch (asn1_nzp_csi_rs_res.res_map.freq_domain_alloc.type()) {
    case csi_rs_res_map_s::freq_domain_alloc_c_::types_opts::options::row1:
      csi_rs_nzp_resource.resource_mapping.row = srsran_csi_rs_resource_mapping_row_1;
      for (uint32_t i = 0; i < asn1_nzp_csi_rs_res.res_map.freq_domain_alloc.row1().length(); i++) {
        csi_rs_nzp_resource.resource_mapping.frequency_domain_alloc[i] =
            asn1_nzp_csi_rs_res.res_map.freq_domain_alloc.row1().get(
                asn1_nzp_csi_rs_res.res_map.freq_domain_alloc.row1().length() - 1 - i);
      }
      break;
    case csi_rs_res_map_s::freq_domain_alloc_c_::types_opts::options::row2:
      csi_rs_nzp_resource.resource_mapping.row = srsran_csi_rs_resource_mapping_row_2;
      for (uint32_t i = 0; i < asn1_nzp_csi_rs_res.res_map.freq_domain_alloc.row2().length(); i++) {
        csi_rs_nzp_resource.resource_mapping.frequency_domain_alloc[i] =
            asn1_nzp_csi_rs_res.res_map.freq_domain_alloc.row2().get(
                asn1_nzp_csi_rs_res.res_map.freq_domain_alloc.row2().length() - 1 - i);
      }
      break;
    case csi_rs_res_map_s::freq_domain_alloc_c_::types_opts::options::row4:
      csi_rs_nzp_resource.resource_mapping.row = srsran_csi_rs_resource_mapping_row_4;
      for (uint32_t i = 0; i < asn1_nzp_csi_rs_res.res_map.freq_domain_alloc.row4().length(); i++) {
        csi_rs_nzp_resource.resource_mapping.frequency_domain_alloc[i] =
            asn1_nzp_csi_rs_res.res_map.freq_domain_alloc.row4().get(
                asn1_nzp_csi_rs_res.res_map.freq_domain_alloc.row4().length() - 1 - i);
      }
      break;
    case csi_rs_res_map_s::freq_domain_alloc_c_::types_opts::options::other:
      csi_rs_nzp_resource.resource_mapping.row = srsran_csi_rs_resource_mapping_row_other;
      break;
    default:
      asn1::log_warning("Invalid option for freq_domain_alloc %s",
                        asn1_nzp_csi_rs_res.res_map.freq_domain_alloc.type().to_string());
      return false;
  }

  csi_rs_nzp_resource.resource_mapping.nof_ports        = asn1_nzp_csi_rs_res.res_map.nrof_ports.to_number();
  csi_rs_nzp_resource.resource_mapping.first_symbol_idx = asn1_nzp_csi_rs_res.res_map.first_ofdm_symbol_in_time_domain;

  switch (asn1_nzp_csi_rs_res.res_map.cdm_type) {
    case csi_rs_res_map_s::cdm_type_opts::options::no_cdm:
      csi_rs_nzp_resource.resource_mapping.cdm = srsran_csi_rs_cdm_t::srsran_csi_rs_cdm_nocdm;
      break;
    case csi_rs_res_map_s::cdm_type_opts::options::fd_cdm2:
      csi_rs_nzp_resource.resource_mapping.cdm = srsran_csi_rs_cdm_t::srsran_csi_rs_cdm_fd_cdm2;
      break;
    case csi_rs_res_map_s::cdm_type_opts::options::cdm4_fd2_td2:
      csi_rs_nzp_resource.resource_mapping.cdm = srsran_csi_rs_cdm_t::srsran_csi_rs_cdm_cdm4_fd2_td2;
      break;
    case csi_rs_res_map_s::cdm_type_opts::options::cdm8_fd2_td4:
      csi_rs_nzp_resource.resource_mapping.cdm = srsran_csi_rs_cdm_t::srsran_csi_rs_cdm_cdm8_fd2_td4;
      break;
    default:
      asn1::log_warning("Invalid option for cdm_type %s", asn1_nzp_csi_rs_res.res_map.cdm_type.to_string());
      return false;
  }

  switch (asn1_nzp_csi_rs_res.res_map.density.type()) {
    case csi_rs_res_map_s::density_c_::types_opts::options::dot5:
      switch (asn1_nzp_csi_rs_res.res_map.density.dot5()) {
        case csi_rs_res_map_s::density_c_::dot5_opts::options::even_prbs:
          csi_rs_nzp_resource.resource_mapping.density = srsran_csi_rs_resource_mapping_density_dot5_even;
          break;
        case csi_rs_res_map_s::density_c_::dot5_opts::options::odd_prbs:
          csi_rs_nzp_resource.resource_mapping.density = srsran_csi_rs_resource_mapping_density_dot5_odd;
          break;
        default:
          asn1::log_warning("Invalid option for dot5 %s", asn1_nzp_csi_rs_res.res_map.density.dot5().to_string());
          return false;
      }
      break;
    case csi_rs_res_map_s::density_c_::types_opts::options::one:
      csi_rs_nzp_resource.resource_mapping.density = srsran_csi_rs_resource_mapping_density_one;
      break;
    case csi_rs_res_map_s::density_c_::types_opts::options::three:
      csi_rs_nzp_resource.resource_mapping.density = srsran_csi_rs_resource_mapping_density_three;
      break;
    case csi_rs_res_map_s::density_c_::types_opts::options::spare:
      csi_rs_nzp_resource.resource_mapping.density = srsran_csi_rs_resource_mapping_density_spare;
      break;
    default:
      asn1::log_warning("Invalid option for density %s", asn1_nzp_csi_rs_res.res_map.density.type().to_string());
      return false;
  }
  csi_rs_nzp_resource.resource_mapping.freq_band.nof_rb   = asn1_nzp_csi_rs_res.res_map.freq_band.nrof_rbs;
  csi_rs_nzp_resource.resource_mapping.freq_band.start_rb = asn1_nzp_csi_rs_res.res_map.freq_band.start_rb;

  // Validate CSI-RS resource mapping
  if (not srsran_csi_rs_resource_mapping_is_valid(&csi_rs_nzp_resource.resource_mapping)) {
    asn1::json_writer json_writer;
    asn1_nzp_csi_rs_res.res_map.to_json(json_writer);
    asn1::log_error("Resource mapping is invalid or not implemented: %s", json_writer.to_string());
    return false;
  }

  csi_rs_nzp_resource.power_control_offset = asn1_nzp_csi_rs_res.pwr_ctrl_offset;
  if (asn1_nzp_csi_rs_res.pwr_ctrl_offset_ss_present) {
    csi_rs_nzp_resource.power_control_offset_ss = asn1_nzp_csi_rs_res.pwr_ctrl_offset_ss.to_number();
  }

  if (asn1_nzp_csi_rs_res.periodicity_and_offset_present) {
    switch (asn1_nzp_csi_rs_res.periodicity_and_offset.type()) {
      case csi_res_periodicity_and_offset_c::types_opts::options::slots4:
        csi_rs_nzp_resource.periodicity.period = 4;
        csi_rs_nzp_resource.periodicity.offset = asn1_nzp_csi_rs_res.periodicity_and_offset.slots4();
        break;
      case csi_res_periodicity_and_offset_c::types_opts::options::slots5:
        csi_rs_nzp_resource.periodicity.period = 5;
        csi_rs_nzp_resource.periodicity.offset = asn1_nzp_csi_rs_res.periodicity_and_offset.slots5();
        break;
      case csi_res_periodicity_and_offset_c::types_opts::options::slots8:
        csi_rs_nzp_resource.periodicity.period = 8;
        csi_rs_nzp_resource.periodicity.offset = asn1_nzp_csi_rs_res.periodicity_and_offset.slots8();
        break;
      case csi_res_periodicity_and_offset_c::types_opts::options::slots10:
        csi_rs_nzp_resource.periodicity.period = 10;
        csi_rs_nzp_resource.periodicity.offset = asn1_nzp_csi_rs_res.periodicity_and_offset.slots10();
        break;
      case csi_res_periodicity_and_offset_c::types_opts::options::slots16:
        csi_rs_nzp_resource.periodicity.period = 16;
        csi_rs_nzp_resource.periodicity.offset = asn1_nzp_csi_rs_res.periodicity_and_offset.slots16();
        break;
      case csi_res_periodicity_and_offset_c::types_opts::options::slots20:
        csi_rs_nzp_resource.periodicity.period = 20;
        csi_rs_nzp_resource.periodicity.offset = asn1_nzp_csi_rs_res.periodicity_and_offset.slots20();
        break;
      case csi_res_periodicity_and_offset_c::types_opts::options::slots32:
        csi_rs_nzp_resource.periodicity.period = 32;
        csi_rs_nzp_resource.periodicity.offset = asn1_nzp_csi_rs_res.periodicity_and_offset.slots32();
        break;
      case csi_res_periodicity_and_offset_c::types_opts::options::slots40:
        csi_rs_nzp_resource.periodicity.period = 40;
        csi_rs_nzp_resource.periodicity.offset = asn1_nzp_csi_rs_res.periodicity_and_offset.slots40();
        break;
      case csi_res_periodicity_and_offset_c::types_opts::options::slots64:
        csi_rs_nzp_resource.periodicity.period = 64;
        csi_rs_nzp_resource.periodicity.offset = asn1_nzp_csi_rs_res.periodicity_and_offset.slots64();
        break;
      case csi_res_periodicity_and_offset_c::types_opts::options::slots80:
        csi_rs_nzp_resource.periodicity.period = 80;
        csi_rs_nzp_resource.periodicity.offset = asn1_nzp_csi_rs_res.periodicity_and_offset.slots80();
        break;
      case csi_res_periodicity_and_offset_c::types_opts::options::slots160:
        csi_rs_nzp_resource.periodicity.period = 160;
        csi_rs_nzp_resource.periodicity.offset = asn1_nzp_csi_rs_res.periodicity_and_offset.slots160();
        break;
      case csi_res_periodicity_and_offset_c::types_opts::options::slots320:
        csi_rs_nzp_resource.periodicity.period = 320;
        csi_rs_nzp_resource.periodicity.offset = asn1_nzp_csi_rs_res.periodicity_and_offset.slots320();
        break;
      case csi_res_periodicity_and_offset_c::types_opts::options::slots640:
        csi_rs_nzp_resource.periodicity.period = 640;
        csi_rs_nzp_resource.periodicity.offset = asn1_nzp_csi_rs_res.periodicity_and_offset.slots640();
        break;
      default:
        asn1::log_warning("Invalid option for periodicity_and_offset %s",
                          asn1_nzp_csi_rs_res.periodicity_and_offset.type().to_string());
        return false;
    }
  } else {
    asn1::log_warning("Option periodicity_and_offset not present");
    return false;
  }

  csi_rs_nzp_resource.scrambling_id = asn1_nzp_csi_rs_res.scrambling_id;

  *out_csi_rs_nzp_resource = csi_rs_nzp_resource;
  return true;
}

static inline srsran_subcarrier_spacing_t make_subcarrier_spacing(const subcarrier_spacing_e& asn1_scs)
{
  switch (asn1_scs) {
    case subcarrier_spacing_opts::options::khz15:
      return srsran_subcarrier_spacing_15kHz;
    case subcarrier_spacing_opts::options::khz30:
      return srsran_subcarrier_spacing_30kHz;
    case subcarrier_spacing_opts::options::khz60:
      return srsran_subcarrier_spacing_60kHz;
    case subcarrier_spacing_opts::options::khz120:
      return srsran_subcarrier_spacing_120kHz;
    case subcarrier_spacing_opts::options::khz240:
      return srsran_subcarrier_spacing_240kHz;
    case subcarrier_spacing_opts::spare3:
    case subcarrier_spacing_opts::spare2:
    case subcarrier_spacing_opts::spare1:
    case subcarrier_spacing_opts::nulltype:
    default:
      asn1::log_warning("Not supported subcarrier spacing ");
      break;
  }

  return srsran_subcarrier_spacing_invalid;
}

bool make_phy_carrier_cfg(const freq_info_dl_s& asn1_freq_info_dl, srsran_carrier_nr_t* out_carrier_nr)
{
  uint32_t absolute_frequency_ssb = 0;
  if (asn1_freq_info_dl.absolute_freq_ssb_present) {
    absolute_frequency_ssb = asn1_freq_info_dl.absolute_freq_ssb;
  } else {
    asn1::log_warning("Option absolute_freq_ssb not present");
    return false;
  }
  if (asn1_freq_info_dl.scs_specific_carrier_list.size() != 1) {
    asn1::log_warning("Option absolute_freq_ssb not present");
    return false;
  }

  srsran_subcarrier_spacing_t scs =
      make_subcarrier_spacing(asn1_freq_info_dl.scs_specific_carrier_list[0].subcarrier_spacing);
  if (scs == srsran_subcarrier_spacing_invalid) {
    return false;
  }

  // As the carrier structure requires parameters from different objects, set fields separately
  srsran::srsran_band_helper bands;
  out_carrier_nr->ssb_center_freq_hz     = bands.nr_arfcn_to_freq(absolute_frequency_ssb);
  out_carrier_nr->dl_center_frequency_hz = bands.get_center_freq_from_abs_freq_point_a(
      asn1_freq_info_dl.scs_specific_carrier_list[0].carrier_bw, asn1_freq_info_dl.absolute_freq_point_a);
  out_carrier_nr->ul_center_frequency_hz = out_carrier_nr->dl_center_frequency_hz; // needs to be updated for FDD
  out_carrier_nr->offset_to_carrier      = asn1_freq_info_dl.scs_specific_carrier_list[0].offset_to_carrier;
  out_carrier_nr->nof_prb                = asn1_freq_info_dl.scs_specific_carrier_list[0].carrier_bw;
  out_carrier_nr->scs                    = scs;
  return true;
}

template <class bitstring_t>
static inline void make_ssb_positions_in_burst(const bitstring_t&                           ans1_position_in_burst,
                                               std::array<bool, SRSRAN_SSB_NOF_CANDIDATES>& position_in_burst)
{
  for (uint32_t i = 0; i < SRSRAN_SSB_NOF_CANDIDATES; i++) {
    if (i < ans1_position_in_burst.length()) {
      position_in_burst[i] = ans1_position_in_burst.get(ans1_position_in_burst.length() - 1 - i);
    } else {
      position_in_burst[i] = false;
    }
  }
}

void fill_ssb_pos_in_burst(const asn1::rrc_nr::serving_cell_cfg_common_sib_s& cfg, phy_cfg_nr_t::ssb_cfg_t* out_ssb)
{
  auto& ssb_pos = cfg.ssb_positions_in_burst;

  out_ssb->position_in_burst = {};
  uint32_t N                 = ssb_pos.in_one_group.length();
  for (uint32_t i = 0; i < N; ++i) {
    out_ssb->position_in_burst[i] = ssb_pos.in_one_group.get(i);
  }
  if (ssb_pos.group_presence_present) {
    for (uint32_t i = 1; i < ssb_pos.group_presence.length(); ++i) {
      if (ssb_pos.group_presence.get(i)) {
        std::copy(out_ssb->position_in_burst.begin(),
                  out_ssb->position_in_burst.begin() + N,
                  out_ssb->position_in_burst.begin() + i * N);
      }
    }
  }
}

bool fill_ssb_pattern_scs(const srsran_carrier_nr_t&   carrier,
                          srsran_ssb_pattern_t*        pattern,
                          srsran_subcarrier_spacing_t* ssb_scs)
{
  srsran::srsran_band_helper bands;
  uint16_t                   band = bands.get_band_from_dl_freq_Hz(carrier.ssb_center_freq_hz);
  if (band == UINT16_MAX) {
    asn1::log_error("Invalid band for SSB frequency %.3f MHz", carrier.ssb_center_freq_hz);
    return false;
  }

  // TODO: Generalize conversion for other SCS
  *pattern = bands.get_ssb_pattern(band, srsran_subcarrier_spacing_15kHz);
  if (*pattern == SRSRAN_SSB_PATTERN_A) {
    *ssb_scs = carrier.scs;
  } else {
    // try to optain SSB pattern for same band with 30kHz SCS
    *pattern = bands.get_ssb_pattern(band, srsran_subcarrier_spacing_30kHz);
    if (*pattern == SRSRAN_SSB_PATTERN_B || *pattern == SRSRAN_SSB_PATTERN_C) {
      // SSB SCS is 30 kHz
      *ssb_scs = srsran_subcarrier_spacing_30kHz;
    } else {
      asn1::log_error("Can't derive SSB pattern from band %d", band);
      return false;
    }
  }
  return true;
}

bool fill_phy_ssb_cfg(const srsran_carrier_nr_t&                         carrier,
                      const asn1::rrc_nr::serving_cell_cfg_common_sib_s& serv_cell_cfg,
                      srsran_ssb_cfg_t*                                  out_ssb)
{
  *out_ssb = {};

  out_ssb->center_freq_hz = carrier.dl_center_frequency_hz;
  out_ssb->ssb_freq_hz    = carrier.ssb_center_freq_hz;
  if (not fill_ssb_pattern_scs(carrier, &out_ssb->pattern, &out_ssb->scs)) {
    return false;
  }

  out_ssb->duplex_mode = SRSRAN_DUPLEX_MODE_FDD;
  if (serv_cell_cfg.tdd_ul_dl_cfg_common_present) {
    out_ssb->duplex_mode = SRSRAN_DUPLEX_MODE_TDD;
  }

  out_ssb->periodicity_ms = serv_cell_cfg.ssb_periodicity_serving_cell.to_number();
  return true;
}

// SA case
bool fill_phy_ssb_cfg(const srsran_carrier_nr_t&                         carrier,
                      const asn1::rrc_nr::serving_cell_cfg_common_sib_s& serv_cell_cfg,
                      phy_cfg_nr_t::ssb_cfg_t*                           out_ssb)
{
  // Derive SSB pattern and SCS
  if (not fill_ssb_pattern_scs(carrier, &out_ssb->pattern, &out_ssb->scs)) {
    return false;
  }
  fill_ssb_pos_in_burst(serv_cell_cfg, out_ssb);
  out_ssb->periodicity_ms = (uint32_t)serv_cell_cfg.ssb_periodicity_serving_cell.to_number();

  return true;
}

bool make_phy_ssb_cfg(const srsran_carrier_nr_t&                     carrier,
                      const asn1::rrc_nr::serving_cell_cfg_common_s& serv_cell_cfg,
                      phy_cfg_nr_t::ssb_cfg_t*                       out_ssb)
{
  srsran::srsran_band_helper bands;
  uint16_t                   band = bands.get_band_from_dl_freq_Hz(carrier.ssb_center_freq_hz);
  if (band == UINT16_MAX) {
    asn1::log_error("Invalid band for SSB frequency %.3f MHz", carrier.ssb_center_freq_hz);
    return false;
  }

  phy_cfg_nr_t::ssb_cfg_t ssb = {};

  // Parse subcarrier spacing
  if (serv_cell_cfg.ssb_subcarrier_spacing_present) {
    switch (serv_cell_cfg.ssb_subcarrier_spacing) {
      case subcarrier_spacing_e::khz15:
        ssb.scs = srsran_subcarrier_spacing_15kHz;
        break;
      case subcarrier_spacing_e::khz30:
        ssb.scs = srsran_subcarrier_spacing_30kHz;
        break;
      default:
        asn1::log_error("SSB SCS %s not supported", serv_cell_cfg.ssb_subcarrier_spacing.to_string());
        return false;
    }
  } else {
    ssb.scs = bands.get_ssb_scs(band);
    if (ssb.scs == srsran_subcarrier_spacing_invalid) {
      asn1::log_error("SSB SCS not available for band %d", band);
      return false;
    }
  }

  // Get the SSB pattern
  ssb.pattern = bands.get_ssb_pattern(band, ssb.scs);
  if (ssb.pattern == SRSRAN_SSB_PATTERN_INVALID) {
    asn1::log_error(
        "Band %d and SS/PBCH block SCS %s results on invalid pattern", band, srsran_subcarrier_spacing_to_str(ssb.scs));
    return false;
  }

  if (serv_cell_cfg.ssb_positions_in_burst_present) {
    switch (serv_cell_cfg.ssb_positions_in_burst.type()) {
      case serving_cell_cfg_common_s::ssb_positions_in_burst_c_::types_opts::short_bitmap:
        make_ssb_positions_in_burst(serv_cell_cfg.ssb_positions_in_burst.short_bitmap(), ssb.position_in_burst);
        break;
      case serving_cell_cfg_common_s::ssb_positions_in_burst_c_::types_opts::medium_bitmap:
        make_ssb_positions_in_burst(serv_cell_cfg.ssb_positions_in_burst.medium_bitmap(), ssb.position_in_burst);
        break;
      case serving_cell_cfg_common_s::ssb_positions_in_burst_c_::types_opts::long_bitmap:
        make_ssb_positions_in_burst(serv_cell_cfg.ssb_positions_in_burst.long_bitmap(), ssb.position_in_burst);
        break;
      case serving_cell_cfg_common_s::ssb_positions_in_burst_c_::types_opts::nulltype:
        asn1::log_warning("SSB position in burst nulltype");
        return false;
    }
  } else {
    asn1::log_warning("SSB position in burst not present");
    return false;
  }
  if (serv_cell_cfg.ssb_periodicity_serving_cell_present) {
    ssb.periodicity_ms = (uint32_t)serv_cell_cfg.ssb_periodicity_serving_cell.to_number();
  } else {
    asn1::log_warning("SSB periodicity not present");
    return false;
  }
  if (serv_cell_cfg.ssb_subcarrier_spacing_present) {
    ssb.scs = make_subcarrier_spacing(serv_cell_cfg.ssb_subcarrier_spacing);
    if (ssb.scs == srsran_subcarrier_spacing_invalid) {
      return false;
    }
  } else {
    asn1::log_warning("SSB subcarrier spacing not present");
    return false;
  }
  if (out_ssb != nullptr) {
    *out_ssb = ssb;
  }
  return true;
}

bool make_phy_mib(const asn1::rrc_nr::mib_s& mib_cfg, srsran_mib_nr_t* mib)
{
  mib->sfn     = 0;
  mib->ssb_idx = 0;
  mib->hrf     = false;
  mib->scs_common =
      mib_cfg.sub_carrier_spacing_common.value == asn1::rrc_nr::mib_s::sub_carrier_spacing_common_opts::scs15or60
          ? srsran_subcarrier_spacing_15kHz
          : srsran_subcarrier_spacing_30kHz;
  mib->ssb_offset             = mib_cfg.ssb_subcarrier_offset;
  mib->dmrs_typeA_pos         = (srsran_dmrs_sch_typeA_pos_t)mib_cfg.dmrs_type_a_position.value;
  mib->coreset0_idx           = mib_cfg.pdcch_cfg_sib1.ctrl_res_set_zero;
  mib->ss0_idx                = mib_cfg.pdcch_cfg_sib1.search_space_zero;
  mib->cell_barred            = mib_cfg.cell_barred.value == asn1::rrc_nr::mib_s::cell_barred_opts::barred;
  mib->intra_freq_reselection = mib_cfg.intra_freq_resel.value == asn1::rrc_nr::mib_s::intra_freq_resel_opts::allowed;
  return true;
}

bool make_pdsch_cfg_from_serv_cell(const asn1::rrc_nr::serving_cell_cfg_s& serv_cell, srsran_sch_hl_cfg_nr_t* sch_hl)
{
  if (serv_cell.csi_meas_cfg_present and serv_cell.csi_meas_cfg.is_setup()) {
    auto& setup = serv_cell.csi_meas_cfg.setup();

    // Configure NZP-CSI
    for (auto& nzp_set : setup.nzp_csi_rs_res_set_to_add_mod_list) {
      auto& uecfg_set    = sch_hl->nzp_csi_rs_sets[nzp_set.nzp_csi_res_set_id];
      uecfg_set.trs_info = nzp_set.trs_info_present;
      uecfg_set.count    = nzp_set.nzp_csi_rs_res.size();
      uint32_t count     = 0;
      for (uint8_t nzp_rs_idx : nzp_set.nzp_csi_rs_res) {
        auto& res = uecfg_set.data[count++];
        if (not srsran::make_phy_nzp_csi_rs_resource(setup.nzp_csi_rs_res_to_add_mod_list[nzp_rs_idx], &res)) {
          return false;
        }
      }
    }
  }

  if (serv_cell.init_dl_bwp.pdsch_cfg_present and serv_cell.init_dl_bwp.pdsch_cfg.is_setup()) {
    const auto& setup = serv_cell.init_dl_bwp.pdsch_cfg.setup();
    if (setup.p_zp_csi_rs_res_set_present) {
      auto& setup_set               = setup.p_zp_csi_rs_res_set.setup();
      sch_hl->p_zp_csi_rs_set.count = setup_set.zp_csi_rs_res_id_list.size();
      for (uint8_t zp_res_id : setup_set.zp_csi_rs_res_id_list) {
        const asn1::rrc_nr::zp_csi_rs_res_s& setup_res = setup.zp_csi_rs_res_to_add_mod_list[zp_res_id];
        auto&                                res       = sch_hl->p_zp_csi_rs_set.data[zp_res_id];
        if (not srsran::make_phy_zp_csi_rs_resource(setup_res, &res)) {
          return false;
        }
      }
    }
  }

  return true;
}

bool make_csi_cfg_from_serv_cell(const asn1::rrc_nr::serving_cell_cfg_s& serv_cell, srsran_csi_hl_cfg_t* csi_hl)
{
  if (serv_cell.csi_meas_cfg_present and serv_cell.csi_meas_cfg.is_setup()) {
    auto& setup = serv_cell.csi_meas_cfg.setup();

    // Configure CSI-Report
    for (uint32_t i = 0; i < setup.csi_report_cfg_to_add_mod_list.size(); ++i) {
      const auto& csi_rep = setup.csi_report_cfg_to_add_mod_list[i];
      if (not make_phy_csi_report(csi_rep, &csi_hl->reports[i])) {
        return false;
      }
      if (csi_rep.report_cfg_type.type().value == csi_report_cfg_s::report_cfg_type_c_::types_opts::periodic) {
        const auto&                 pucch_setup = serv_cell.ul_cfg.init_ul_bwp.pucch_cfg.setup();
        srsran_pucch_nr_resource_t& resource    = csi_hl->reports[i].periodic.resource;
        uint32_t    pucch_resource_id           = csi_rep.report_cfg_type.periodic().pucch_csi_res_list[0].pucch_res;
        const auto& asn1_resource               = pucch_setup.res_to_add_mod_list[pucch_resource_id];
        uint32_t    format2_rate                = 0;
        if (pucch_setup.format2_present and
            pucch_setup.format2.type().value == asn1::setup_release_c<pucch_format_cfg_s>::types_opts::setup and
            pucch_setup.format2.setup().max_code_rate_present) {
          format2_rate = pucch_setup.format2.setup().max_code_rate.to_number();
        }
        if (not make_phy_res_config(asn1_resource, format2_rate, &resource)) {
          return false;
        }
      }
    }
  }

  return true;
}

bool make_duplex_cfg_from_serv_cell(const asn1::rrc_nr::serving_cell_cfg_common_s& serv_cell,
                                    srsran_duplex_config_nr_t*                     duplex_cfg)
{
  duplex_cfg->mode = serv_cell.tdd_ul_dl_cfg_common_present ? SRSRAN_DUPLEX_MODE_TDD : SRSRAN_DUPLEX_MODE_FDD;
  if (serv_cell.tdd_ul_dl_cfg_common_present) {
    if (not make_phy_tdd_cfg(serv_cell.tdd_ul_dl_cfg_common, duplex_cfg)) {
      return false;
    }
  }
  return true;
}

bool fill_phy_pdcch_cfg(const asn1::rrc_nr::pdcch_cfg_s& pdcch_cfg, srsran_pdcch_cfg_nr_t* pdcch)
{
  for (const ctrl_res_set_s& coreset : pdcch_cfg.ctrl_res_set_to_add_mod_list) {
    pdcch->coreset_present[coreset.ctrl_res_set_id] = true;
    make_phy_coreset_cfg(coreset, &pdcch->coreset[coreset.ctrl_res_set_id]);
  }

  for (const search_space_s& ss : pdcch_cfg.search_spaces_to_add_mod_list) {
    pdcch->search_space_present[ss.search_space_id] = true;
    make_phy_search_space_cfg(ss, &pdcch->search_space[ss.search_space_id]);
  }
  return true;
}

bool fill_phy_pdcch_cfg_common(const asn1::rrc_nr::pdcch_cfg_common_s& pdcch_cfg, srsran_pdcch_cfg_nr_t* pdcch)
{
  if (pdcch_cfg.common_ctrl_res_set_present) {
    pdcch->coreset_present[pdcch_cfg.common_ctrl_res_set.ctrl_res_set_id] = true;
    make_phy_coreset_cfg(pdcch_cfg.common_ctrl_res_set, &pdcch->coreset[pdcch_cfg.common_ctrl_res_set.ctrl_res_set_id]);
  }
  for (const search_space_s& ss : pdcch_cfg.common_search_space_list) {
    pdcch->search_space_present[ss.search_space_id] = true;
    if (not make_phy_search_space_cfg(ss, &pdcch->search_space[ss.search_space_id])) {
      asn1::log_error("Failed to convert SearchSpace Configuration");
      return false;
    }
    if (pdcch_cfg.ra_search_space_present and pdcch_cfg.ra_search_space == ss.search_space_id) {
      pdcch->ra_search_space_present     = true;
      pdcch->ra_search_space             = pdcch->search_space[ss.search_space_id];
      pdcch->ra_search_space.type        = srsran_search_space_type_common_1;
      pdcch->ra_search_space.nof_formats = 1;
      pdcch->ra_search_space.formats[0]  = srsran_dci_format_nr_1_0;
    }
  }
  return true;
}

void fill_phy_pucch_cfg_common(const asn1::rrc_nr::pucch_cfg_common_s& pucch_cfg, srsran_pucch_nr_common_cfg_t* pucch)
{
  if (pucch_cfg.pucch_res_common_present) {
    pucch->resource_common = pucch_cfg.pucch_res_common;
  }
  if (pucch_cfg.hop_id_present) {
    pucch->hopping_id_present = true;
    pucch->hopping_id         = pucch_cfg.hop_id;
  }
  if (pucch_cfg.p0_nominal_present) {
    pucch->p0_nominal = pucch_cfg.p0_nominal;
  }

  switch (pucch_cfg.pucch_group_hop) {
    case pucch_cfg_common_s::pucch_group_hop_opts::enable:
      pucch->group_hopping = SRSRAN_PUCCH_NR_GROUP_HOPPING_ENABLE;
      break;
    case pucch_cfg_common_s::pucch_group_hop_opts::disable:
      pucch->group_hopping = SRSRAN_PUCCH_NR_GROUP_HOPPING_DISABLE;
      break;
    default:
      pucch->group_hopping = SRSRAN_PUCCH_NR_GROUP_HOPPING_NEITHER;
      break;
  }
}

bool fill_phy_pucch_cfg(const asn1::rrc_nr::pucch_cfg_s& pucch_cfg, srsran_pucch_nr_hl_cfg_t* pucch)
{
  // sanity check to avoid pucch->sets[n] goes out of bound
  if (pucch_cfg.res_set_to_add_mod_list.size() > SRSRAN_PUCCH_NR_MAX_NOF_SETS) {
    return false;
  }

  // iterate over the sets of resourceSetToAddModList
  for (size_t n = 0; n < pucch_cfg.res_set_to_add_mod_list.size(); n++) {
    auto& res_set                = pucch_cfg.res_set_to_add_mod_list[n];
    pucch->sets[n].nof_resources = res_set.res_list.size();
    if (res_set.max_payload_size_present) {
      pucch->sets[n].max_payload_size = res_set.max_payload_size;
    }
    // NOTE:  res_set.pucch_res_set_id does not have a corresponding field in the PHY struct

    // for each set, iterate over the elements (an element is an index). For each of the element or index, find the
    // corresponding pucch_res_s object in the pucch_cfg.res_to_add_mod_list
    for (size_t res_idx = 0; res_idx < res_set.res_list.size(); res_idx++) {
      size_t pucch_resource_id = res_set.res_list[res_idx];

      // Find the pucch_res_s object corresponding to pucch_resource_id in the pucch_cfg.res_to_add_mod_list
      size_t m = 0;
      while (m <= pucch_cfg.res_to_add_mod_list.size()) {
        if (m == pucch_cfg.res_to_add_mod_list.size()) {
          // if we get here, the list pucch_cfg.res_to_add_mod_list does not contain any object corresponding to
          // pucch_resource_id
          return false;
        }
        if (pucch_cfg.res_to_add_mod_list[m].pucch_res_id == pucch_resource_id) {
          break; // item found, exit the loop
        }
        m++;
      }

      // Below is the object corresponding to pucch_resource_id in the pucch_cfg.res_to_add_mod_list
      const auto& asn1_resource = pucch_cfg.res_to_add_mod_list[m];

      // sanity check to avoid pucch->sets[n].resources[res_idx] goes out of bound;
      if (res_idx >= SRSRAN_PUCCH_NR_MAX_NOF_RESOURCES_PER_SET) {
        return false;
      }

      auto&    resource     = pucch->sets[n].resources[res_idx];
      uint32_t format2_rate = 0;
      if (pucch_cfg.format2_present and
          pucch_cfg.format2.type().value == asn1::setup_release_c<pucch_format_cfg_s>::types_opts::setup and
          pucch_cfg.format2.setup().max_code_rate_present) {
        format2_rate = pucch_cfg.format2.setup().max_code_rate.to_number();
      }
      if (not make_phy_res_config(asn1_resource, format2_rate, &resource)) {
        return false;
      }
    }
  }

  // configure scheduling request resources
  return fill_phy_pucch_hl_cfg(pucch_cfg, pucch);
}

bool fill_phy_pucch_hl_cfg(const asn1::rrc_nr::pucch_cfg_s& pucch_cfg, srsran_pucch_nr_hl_cfg_t* pucch)
{
  for (size_t n = 0; n < pucch_cfg.sched_request_res_to_add_mod_list.size(); n++) {
    // fill each sr_resource's cnf
    auto&                          asn_sr_res = pucch_cfg.sched_request_res_to_add_mod_list[n];
    srsran_pucch_nr_sr_resource_t* sr_res     = &pucch->sr_resources[asn_sr_res.sched_request_res_id];
    make_phy_sr_resource(asn_sr_res, sr_res);

    // get the pucch_resource from pucch_cfg.res_to_add_mod_list and copy it into the sr_resouce.resource
    const auto& asn1_pucch_resource = pucch_cfg.res_to_add_mod_list[asn_sr_res.res];
    auto&       pucch_resource      = sr_res->resource;
    uint32_t    format2_rate        = 0;
    if (pucch_cfg.format2_present and
        pucch_cfg.format2.type().value == asn1::setup_release_c<pucch_format_cfg_s>::types_opts::setup and
        pucch_cfg.format2.setup().max_code_rate_present) {
      format2_rate = pucch_cfg.format2.setup().max_code_rate.to_number();
    }
    if (not make_phy_res_config(asn1_pucch_resource, format2_rate, &pucch_resource)) {
      return false;
    }
  }

  return true;
}

bool fill_phy_pdsch_cfg_common(const asn1::rrc_nr::pdsch_cfg_common_s& pdsch_cfg, srsran_sch_hl_cfg_nr_t* pdsch)
{
  for (uint32_t i = 0; i < pdsch_cfg.pdsch_time_domain_alloc_list.size(); i++) {
    srsran_sch_time_ra_t common_time_ra;
    if (make_phy_common_time_ra(pdsch_cfg.pdsch_time_domain_alloc_list[i], &common_time_ra) == true) {
      pdsch->common_time_ra[i]  = common_time_ra;
      pdsch->nof_common_time_ra = i + 1;
    } else {
      asn1::log_warning("Warning while building common_time_ra structure");
      return false;
    }
  }
  return true;
}

bool fill_phy_pusch_cfg_common(const asn1::rrc_nr::pusch_cfg_common_s& pusch_cfg, srsran_sch_hl_cfg_nr_t* pusch)
{
  for (uint32_t i = 0; i < pusch_cfg.pusch_time_domain_alloc_list.size(); i++) {
    srsran_sch_time_ra_t common_time_ra;
    if (make_phy_common_time_ra(pusch_cfg.pusch_time_domain_alloc_list[i], &common_time_ra) == true) {
      pusch->common_time_ra[i]  = common_time_ra;
      pusch->nof_common_time_ra = i + 1;
    } else {
      asn1::log_warning("Warning while building common_time_ra structure");
      return false;
    }
  }
  return true;
}

void fill_phy_carrier_cfg(const asn1::rrc_nr::serving_cell_cfg_common_sib_s& serv_cell_cfg,
                          srsran_carrier_nr_t*                               out_carrier_nr)
{
  // TODO: Currently ony one carrier is supported
  auto& freq_info_dl                = serv_cell_cfg.dl_cfg_common.freq_info_dl;
  out_carrier_nr->offset_to_carrier = freq_info_dl.scs_specific_carrier_list[0].offset_to_carrier;
  out_carrier_nr->scs     = make_subcarrier_spacing(freq_info_dl.scs_specific_carrier_list[0].subcarrier_spacing);
  out_carrier_nr->nof_prb = freq_info_dl.scs_specific_carrier_list[0].carrier_bw;

  auto&                      freq_info_ul = serv_cell_cfg.ul_cfg_common.freq_info_ul;
  srsran::srsran_band_helper bands;
  out_carrier_nr->ul_center_frequency_hz = bands.get_center_freq_from_abs_freq_point_a(
      freq_info_ul.scs_specific_carrier_list[0].carrier_bw, freq_info_ul.absolute_freq_point_a);
}

void fill_phy_ssb_cfg(const asn1::rrc_nr::serving_cell_cfg_common_sib_s& serv_cell_cfg,
                      phy_cfg_nr_t::ssb_cfg_t*                           out_ssb)
{
  out_ssb->periodicity_ms = serv_cell_cfg.ssb_periodicity_serving_cell.to_number();

  if (serv_cell_cfg.ssb_positions_in_burst.group_presence_present) {
    make_ssb_positions_in_burst(serv_cell_cfg.ssb_positions_in_burst.group_presence, out_ssb->position_in_burst);
  } else {
    make_ssb_positions_in_burst(serv_cell_cfg.ssb_positions_in_burst.in_one_group, out_ssb->position_in_burst);
  }
}

/**************************
 *     Asn1 Obj Id
 *************************/

ASN1_OBJ_ID_DEFINE(asn1::rrc_nr::srb_to_add_mod_s, srb_id);
ASN1_OBJ_ID_DEFINE(asn1::rrc_nr::drb_to_add_mod_s, drb_id);
ASN1_OBJ_ID_DEFINE(asn1::rrc_nr::meas_obj_to_add_mod_s, meas_obj_id);
ASN1_OBJ_ID_DEFINE(asn1::rrc_nr::report_cfg_to_add_mod_s, report_cfg_id);
ASN1_OBJ_ID_DEFINE(asn1::rrc_nr::meas_id_to_add_mod_s, meas_id);

} // namespace srsran

namespace asn1 {

namespace rrc_nr {

bool operator==(const srb_to_add_mod_s& lhs, const srb_to_add_mod_s& rhs)
{
  if (lhs.srb_id != rhs.srb_id or lhs.pdcp_cfg_present != rhs.pdcp_cfg_present) {
    return false;
  }
  // TODO: check remaining fields
  return true;
}
bool operator==(const drb_to_add_mod_s& lhs, const drb_to_add_mod_s& rhs)
{
  if (lhs.drb_id != rhs.drb_id or lhs.pdcp_cfg_present != rhs.pdcp_cfg_present or
      lhs.cn_assoc_present != rhs.cn_assoc_present) {
    return false;
  }
  // TODO: check remaining fields
  return true;
}

} // namespace rrc_nr

} // namespace asn1
