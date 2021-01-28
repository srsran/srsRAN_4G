/**
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

#include "srslte/asn1/rrc_nr_utils.h"
#include "srslte/asn1/rrc_nr.h"
#include "srslte/config.h"
#include "srslte/interfaces/pdcp_interface_types.h"
#include "srslte/interfaces/rlc_interface_types.h"
#include <algorithm>

namespace srslte {

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

rlc_config_t make_rlc_config_t(const rlc_cfg_c& asn1_type)
{
  rlc_config_t rlc_cfg = rlc_config_t::default_rlc_um_nr_config();
  rlc_cfg.rat = srslte_rat_t::nr;
  switch (asn1_type.type().value) {
    case rlc_cfg_c::types_opts::am:
      break;
    case rlc_cfg_c::types_opts::um_bi_dir:
    case rlc_cfg_c::types_opts::um_uni_dir_dl:
    case rlc_cfg_c::types_opts::um_uni_dir_ul:
      rlc_cfg.rlc_mode              = rlc_mode_t::um;
      rlc_cfg.um_nr.t_reassembly_ms = asn1_type.um_bi_dir().dl_um_rlc.t_reassembly.value;
      rlc_cfg.um_nr.sn_field_length = (rlc_um_nr_sn_size_t)asn1_type.um_bi_dir().dl_um_rlc.sn_field_len.value;
      rlc_cfg.um_nr.mod             = (rlc_cfg.um_nr.sn_field_length == rlc_um_nr_sn_size_t::size6bits) ? 64 : 4096;
      rlc_cfg.um_nr.UM_Window_Size  = (rlc_cfg.um_nr.sn_field_length == rlc_um_nr_sn_size_t::size6bits) ? 32 : 2048;
      break;
    default:
      break;
  }
  return rlc_cfg;
}

srslte::pdcp_config_t make_drb_pdcp_config_t(const uint8_t bearer_id, bool is_ue, const pdcp_cfg_s& pdcp_cfg)
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
      case 100:
        discard_timer = pdcp_discard_timer_t::ms100;
        break;
      default:
        discard_timer = pdcp_discard_timer_t::infinity;
        break;
    }
  }

  pdcp_t_reordering_t t_reordering = pdcp_t_reordering_t::ms500;
  if (pdcp_cfg.t_reordering_present) {
    switch (pdcp_cfg.t_reordering.to_number()) {
      case 0:
        t_reordering = pdcp_t_reordering_t::ms0;
        break;
      default:
        t_reordering = pdcp_t_reordering_t::ms500;
    }
  }

  uint8_t sn_len = srslte::PDCP_SN_LEN_12;
  if (pdcp_cfg.drb.pdcp_sn_size_dl_present) {
    switch (pdcp_cfg.drb.pdcp_sn_size_dl.value) {
      case pdcp_cfg_s::drb_s_::pdcp_sn_size_dl_opts::options::len12bits:
        sn_len = srslte::PDCP_SN_LEN_12;
        break;
      case pdcp_cfg_s::drb_s_::pdcp_sn_size_dl_opts::options::len18bits:
        sn_len = srslte::PDCP_SN_LEN_18;
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
                    discard_timer);
  return cfg;
}

} // namespace srslte

namespace srsenb {

int set_sched_cell_cfg_sib1(srsenb::sched_interface::cell_cfg_t* sched_cfg, const asn1::rrc_nr::sib1_s& sib1)
{
  bzero(sched_cfg, sizeof(srsenb::sched_interface::cell_cfg_t));

  // set SIB1 and SIB2+ period
  sched_cfg->sibs[0].period_rf = 16; // SIB1 is always 16 rf
  for (uint32_t i = 0; i < sib1.si_sched_info.sched_info_list.size(); i++) {
    sched_cfg->sibs[i + 1].period_rf = sib1.si_sched_info.sched_info_list[i].si_periodicity.to_number();
  }

  // si-WindowLength
  sched_cfg->si_window_ms = sib1.si_sched_info.si_win_len.to_number();

  // setup PRACH
  if (not sib1.si_sched_info.si_request_cfg.rach_occasions_si_present) {
    asn1::log_error("Expected RA Resp Win present\n");
    return SRSLTE_ERROR;
  }
  sched_cfg->prach_rar_window = sib1.si_sched_info.si_request_cfg.rach_occasions_si.rach_cfg_si.ra_resp_win.to_number();
  sched_cfg->prach_freq_offset = sib1.si_sched_info.si_request_cfg.rach_occasions_si.rach_cfg_si.msg1_freq_start;
  sched_cfg->maxharq_msg3tx    = sib1.si_sched_info.si_request_cfg.rach_occasions_si.rach_cfg_si.preamb_trans_max;

  return SRSLTE_SUCCESS;
}

} // namespace srsenb