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

#include "../../../srsue/hdr/stack/rrc/rrc.h" // for rrc_args_t
#include "srslte/asn1/rrc_asn1.h"
#include "srslte/common/bcd_helpers.h"
#include "srslte/common/log_filter.h"
#include "srslte/common/mac_pcap.h"
#include <iostream>

using namespace asn1::rrc;

#define PCAP 0

#define TESTASSERT(cond)                                                                                               \
  {                                                                                                                    \
    if (!(cond)) {                                                                                                     \
      std::cout << "[" << __FUNCTION__ << "][Line " << __LINE__ << "]: FAIL at " << (#cond) << std::endl;              \
      return -1;                                                                                                       \
    }                                                                                                                  \
  }

int rrc_ue_cap_info_test(srslte::mac_pcap* pcap)
{
  srslte::log_filter log1("RRC");
  log1.set_level(srslte::LOG_LEVEL_DEBUG);
  log1.set_hex_limit(128);

  rrc_args_t args          = {};
  args.feature_group       = 0xe6041c00;
  args.nof_supported_bands = 1;
  args.supported_bands[0]  = 8;
  args.ue_category         = 4;

  asn1::rrc::ul_dcch_msg_s ul_dcch_msg;
  ul_dcch_msg.msg.set(ul_dcch_msg_type_c::types::c1);
  ul_dcch_msg.msg.c1().set(ul_dcch_msg_type_c::c1_c_::types::ue_cap_info);
  ul_dcch_msg.msg.c1().ue_cap_info().rrc_transaction_id = 0;

  ul_dcch_msg.msg.c1().ue_cap_info().crit_exts.set(ue_cap_info_s::crit_exts_c_::types::c1);
  ul_dcch_msg.msg.c1().ue_cap_info().crit_exts.c1().set(ue_cap_info_s::crit_exts_c_::c1_c_::types::ue_cap_info_r8);
  ue_cap_info_r8_ies_s* info = &ul_dcch_msg.msg.c1().ue_cap_info().crit_exts.c1().ue_cap_info_r8();
  info->ue_cap_rat_container_list.resize(1);
  info->ue_cap_rat_container_list[0].rat_type = rat_type_e::eutra;

  ue_eutra_cap_s cap;
  cap.access_stratum_release                            = access_stratum_release_e::rel8;
  cap.ue_category                                       = (uint8_t)args.ue_category;
  cap.pdcp_params.max_num_rohc_context_sessions_present = false;

  cap.phy_layer_params.ue_specific_ref_sigs_supported = false;
  cap.phy_layer_params.ue_tx_ant_sel_supported        = false;

  cap.rf_params.supported_band_list_eutra.resize(args.nof_supported_bands);
  cap.meas_params.band_list_eutra.resize(args.nof_supported_bands);
  for (uint32_t i = 0; i < args.nof_supported_bands; i++) {
    cap.rf_params.supported_band_list_eutra[i].band_eutra  = args.supported_bands[i];
    cap.rf_params.supported_band_list_eutra[i].half_duplex = false;
    cap.meas_params.band_list_eutra[i].inter_freq_band_list.resize(1);
    cap.meas_params.band_list_eutra[i].inter_freq_band_list[0].inter_freq_need_for_gaps = true;
  }

  cap.feature_group_inds_present = true;
  cap.feature_group_inds.from_number(args.feature_group);

  uint8_t buf[64];
  bzero(buf, sizeof(buf));
  asn1::bit_ref bref(buf, sizeof(buf));
  cap.pack(bref);
  bref.align_bytes_zero();
  uint32_t cap_len = (uint32_t)bref.distance_bytes(buf);

  info->ue_cap_rat_container_list[0].ue_cap_rat_container.resize(cap_len);
  memcpy(info->ue_cap_rat_container_list[0].ue_cap_rat_container.data(), buf, cap_len);
  log1.debug_hex(buf, cap_len, "UE-Cap (%d/%zd B)\n", cap_len, sizeof(buf));

  // pack the message
  uint8_t byte_buf[32];
  bzero(byte_buf, sizeof(byte_buf));
  asn1::bit_ref bref3(byte_buf, sizeof(byte_buf));
  ul_dcch_msg.pack(bref3);
  bref3.align_bytes_zero();

  uint32_t len = (uint32_t)bref3.distance_bytes(byte_buf);
  log1.debug_hex(byte_buf, len, "UL-DCCH (%d/%zd B)\n", len, sizeof(byte_buf));

  if (pcap != NULL) {
    pcap->write_ul_rrc_pdu(byte_buf, len);
  }

  return 0;
}

int pack_fail_test()
{
  srslte::log_filter log1("RRC");
  log1.set_level(srslte::LOG_LEVEL_DEBUG);
  log1.set_hex_limit(128);

  rrc_args_t args          = {};
  args.feature_group       = 0xe6041c00;
  args.nof_supported_bands = 1;
  args.supported_bands[0]  = 8;
  args.ue_category         = 4;

  asn1::rrc::ul_dcch_msg_s ul_dcch_msg;
  ul_dcch_msg.msg.set(ul_dcch_msg_type_c::types::c1);
  ul_dcch_msg.msg.c1().set(ul_dcch_msg_type_c::c1_c_::types::ue_cap_info);
  ul_dcch_msg.msg.c1().ue_cap_info().rrc_transaction_id = 0;

  ul_dcch_msg.msg.c1().ue_cap_info().crit_exts.set(ue_cap_info_s::crit_exts_c_::types::c1);
  ul_dcch_msg.msg.c1().ue_cap_info().crit_exts.c1().set(ue_cap_info_s::crit_exts_c_::c1_c_::types::ue_cap_info_r8);
  ue_cap_info_r8_ies_s* info = &ul_dcch_msg.msg.c1().ue_cap_info().crit_exts.c1().ue_cap_info_r8();
  info->ue_cap_rat_container_list.resize(1);
  info->ue_cap_rat_container_list[0].rat_type = rat_type_e::eutra;

  ue_eutra_cap_s cap;
  cap.access_stratum_release                            = access_stratum_release_e::rel8;
  cap.ue_category                                       = (uint8_t)args.ue_category;
  cap.pdcp_params.max_num_rohc_context_sessions_present = false;

  cap.phy_layer_params.ue_specific_ref_sigs_supported = false;
  cap.phy_layer_params.ue_tx_ant_sel_supported        = false;

  cap.rf_params.supported_band_list_eutra.resize(args.nof_supported_bands);
  cap.meas_params.band_list_eutra.resize(args.nof_supported_bands);
  for (uint32_t i = 0; i < args.nof_supported_bands; i++) {
    cap.rf_params.supported_band_list_eutra[i].band_eutra  = args.supported_bands[i];
    cap.rf_params.supported_band_list_eutra[i].half_duplex = false;
    cap.meas_params.band_list_eutra[i].inter_freq_band_list.resize(1);
    cap.meas_params.band_list_eutra[i].inter_freq_band_list[0].inter_freq_need_for_gaps = true;
  }

  cap.feature_group_inds_present = true;
  cap.feature_group_inds.from_number(args.feature_group);

  uint8_t       buff[3];
  asn1::bit_ref bref(buff, sizeof(buff));
  if (ul_dcch_msg.pack(bref) != asn1::SRSASN_SUCCESS) {
    fprintf(stderr, "Error while packing message.\n");
    return -1;
  }

  return 0;
}

int main(int argc, char** argv)
{
#if PCAP
  srslte::mac_pcap pcap;
  pcap.open("ul_dcch.pcap");
  TESTASSERT(rrc_ue_cap_info_test(&pcap) == 0);
#else
  TESTASSERT(rrc_ue_cap_info_test(NULL) == 0);
#endif
  TESTASSERT(pack_fail_test() == -1);

#if PCAP
  pcap.close();
#endif
  return 0;
}