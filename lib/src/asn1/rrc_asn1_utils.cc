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
#include "srslte/common/bcd_helpers.h"
#include "srslte/config.h"
#include "srslte/upper/rlc_interface.h"
#include <algorithm>

namespace srslte {

bool plmn_is_valid(const asn1::rrc::plmn_id_s& asn1_type)
{
  return asn1_type.mcc_present and (asn1_type.mnc.size() == 3 or asn1_type.mnc.size() == 2);
}

plmn_id_t::plmn_id_t(const asn1::rrc::plmn_id_s& asn1_type)
{
  from_asn1(asn1_type);
}

void plmn_id_t::reset()
{
  nof_mnc_digits = 2;
  std::fill(&mnc[0], &mnc[3], 3);
  std::fill(&mcc[0], &mcc[3], 3);
}

int plmn_id_t::from_asn1(const asn1::rrc::plmn_id_s& asn1_type)
{
  if (not plmn_is_valid(asn1_type)) {
    reset();
    return SRSLTE_ERROR;
  }
  std::copy(&asn1_type.mcc[0], &asn1_type.mcc[3], &mcc[0]);
  nof_mnc_digits = asn1_type.mnc.size();
  std::copy(&asn1_type.mnc[0], &asn1_type.mnc[nof_mnc_digits], &mnc[0]);

  return SRSLTE_SUCCESS;
}

int plmn_id_t::to_asn1(asn1::rrc::plmn_id_s* asn1_type) const
{
  asn1_type->mcc_present = true;
  std::copy(&mcc[0], &mcc[3], &asn1_type->mcc[0]);
  asn1_type->mnc.resize(nof_mnc_digits);
  std::copy(&mnc[0], &mnc[nof_mnc_digits], &asn1_type->mnc[0]);
  return SRSLTE_SUCCESS;
}

int plmn_id_t::from_number(uint16_t mcc_num, uint16_t mnc_num)
{
  srslte::mcc_to_bytes(mcc_num, &mcc[0]);
  if (not srslte::mnc_to_bytes(mnc_num, &mnc[0], &nof_mnc_digits)) {
    reset();
    return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
}

int plmn_id_t::from_string(const std::string& plmn_str)
{
  if (plmn_str.size() < 5 or plmn_str.size() > 6) {
    reset();
    return SRSLTE_ERROR;
  }
  uint16_t mnc_num, mcc_num;
  if (not string_to_mcc(std::string(plmn_str.begin(), plmn_str.begin() + 3), &mcc_num)) {
    reset();
    return SRSLTE_ERROR;
  }
  if (not string_to_mnc(std::string(plmn_str.begin() + 3, plmn_str.end()), &mnc_num)) {
    reset();
    return SRSLTE_ERROR;
  }
  if (not mcc_to_bytes(mcc_num, &mcc[0])) {
    reset();
    return SRSLTE_ERROR;
  }
  return mnc_to_bytes(mnc_num, &mnc[0], &nof_mnc_digits) ? SRSLTE_SUCCESS : SRSLTE_ERROR;
}

std::string plmn_id_t::to_string() const
{
  std::string mcc_str, mnc_str;
  uint16_t    mnc_num, mcc_num;
  bytes_to_mnc(&mnc[0], &mnc_num, nof_mnc_digits);
  bytes_to_mcc(&mcc[0], &mcc_num);
  mnc_to_string(mnc_num, &mnc_str);
  mcc_to_string(mcc_num, &mcc_str);
  return mcc_str + mnc_str;
}

bool plmn_id_t::operator==(const plmn_id_t& other)
{
  return std::equal(&mcc[0], &mcc[3], &other.mcc[0]) and nof_mnc_digits == other.nof_mnc_digits and
         std::equal(&mnc[0], &mnc[nof_mnc_digits], &other.mnc[0]);
}

s_tmsi_t::s_tmsi_t(const asn1::rrc::s_tmsi_s& asn1_type)
{
  from_asn1(asn1_type);
}

void s_tmsi_t::from_asn1(const asn1::rrc::s_tmsi_s& asn1_type)
{
  mmec   = asn1_type.mmec.to_number();
  m_tmsi = asn1_type.m_tmsi.to_number();
}

void s_tmsi_t::to_asn1(asn1::rrc::s_tmsi_s* asn1_type) const
{
  asn1_type->mmec.from_number(mmec);
  asn1_type->m_tmsi.from_number(m_tmsi);
}

static_assert((int)establishment_cause_t::nulltype == (int)asn1::rrc::establishment_cause_opts::nulltype,
              "Failed conversion of ASN1 type");
std::string to_string(const establishment_cause_t& cause)
{
  asn1::rrc::establishment_cause_e asn1_cause((asn1::rrc::establishment_cause_opts::options)cause);
  return asn1_cause.to_string();
}

void convert_from_asn1(srslte_rlc_config_t* out, const asn1::rrc::rlc_cfg_c& asn1_type)
{
  out->am = srslte_rlc_am_config_t();
  out->um = srslte_rlc_um_config_t();

  // update RLC mode to internal mode struct
  out->rlc_mode = (asn1_type.type() == asn1::rrc::rlc_cfg_c::types::am) ? RLC_MODE_AM : RLC_MODE_UM;

  switch (out->rlc_mode) {
    case RLC_MODE_AM:
      out->am.t_poll_retx       = asn1_type.am().ul_am_rlc.t_poll_retx.to_number();
      out->am.poll_pdu          = asn1_type.am().ul_am_rlc.poll_pdu.to_number();
      out->am.poll_byte         = asn1_type.am().ul_am_rlc.poll_byte.to_number() * 1000; // KB
      out->am.max_retx_thresh   = asn1_type.am().ul_am_rlc.max_retx_thres.to_number();
      out->am.t_reordering      = asn1_type.am().dl_am_rlc.t_reordering.to_number();
      out->am.t_status_prohibit = asn1_type.am().dl_am_rlc.t_status_prohibit.to_number();
      break;
    case RLC_MODE_UM:
      out->um.t_reordering       = asn1_type.um_bi_dir().dl_um_rlc.t_reordering.to_number();
      out->um.rx_sn_field_length = (rlc_umd_sn_size_t)asn1_type.um_bi_dir().dl_um_rlc.sn_field_len.value;
      out->um.rx_window_size     = (RLC_UMD_SN_SIZE_5_BITS == out->um.rx_sn_field_length) ? 16 : 512;
      out->um.rx_mod             = (RLC_UMD_SN_SIZE_5_BITS == out->um.rx_sn_field_length) ? 32 : 1024;
      out->um.tx_sn_field_length = (rlc_umd_sn_size_t)asn1_type.um_bi_dir().ul_um_rlc.sn_field_len.value;
      out->um.tx_mod             = (RLC_UMD_SN_SIZE_5_BITS == out->um.tx_sn_field_length) ? 32 : 1024;
      break;
      //    case asn1::rrc::rlc_cfg_c::types::um_uni_dir_ul:
      //      um.tx_sn_field_length = (rlc_umd_sn_size_t)cnfg->um_uni_dir_ul().ul_um_rlc.sn_field_len.value;
      //      um.tx_mod             = (RLC_UMD_SN_SIZE_5_BITS == um.tx_sn_field_length) ? 32 : 1024;
      //      break;
      //    case asn1::rrc::rlc_cfg_c::types::um_uni_dir_dl:
      //      um.t_reordering       = cnfg->um_uni_dir_dl().dl_um_rlc.t_reordering.to_number();
      //      um.rx_sn_field_length = (rlc_umd_sn_size_t)cnfg->um_uni_dir_dl().dl_um_rlc.sn_field_len.value;
      //      um.rx_window_size     = (RLC_UMD_SN_SIZE_5_BITS == um.rx_sn_field_length) ? 16 : 512;
      //      um.rx_mod             = (RLC_UMD_SN_SIZE_5_BITS == um.rx_sn_field_length) ? 32 : 1024;
      //      break;
    default:
      // Handle default case
      break;
  }

  out->tx_queue_length = RLC_TX_QUEUE_LEN;
}

} // namespace