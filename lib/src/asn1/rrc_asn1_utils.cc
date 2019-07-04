/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

namespace srslte {

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
    asn1::rrc::rrc_log_print(
        asn1::LOG_LEVEL_ERROR, "SRB %d does not support default initialization type\n", asn1_type.srb_id);
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

} // namespace srslte
