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

#include "srsran/asn1/rrc.h"
#include <sstream>

using namespace asn1;
using namespace asn1::rrc;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// MasterInformationBlock-MBMS-r14 ::= SEQUENCE
SRSASN_CODE mib_mbms_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(dl_bw_mbms_r14.pack(bref));
  HANDLE_CODE(sys_frame_num_r14.pack(bref));
  HANDLE_CODE(pack_integer(bref, add_non_mbsfn_sfs_r14, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mib_mbms_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(dl_bw_mbms_r14.unpack(bref));
  HANDLE_CODE(sys_frame_num_r14.unpack(bref));
  HANDLE_CODE(unpack_integer(add_non_mbsfn_sfs_r14, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void mib_mbms_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("dl-Bandwidth-MBMS-r14", dl_bw_mbms_r14.to_string());
  j.write_str("systemFrameNumber-r14", sys_frame_num_r14.to_string());
  j.write_int("additionalNonMBSFNSubframes-r14", add_non_mbsfn_sfs_r14);
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

const char* mib_mbms_r14_s::dl_bw_mbms_r14_opts::to_string() const
{
  static const char* options[] = {"n6", "n15", "n25", "n50", "n75", "n100"};
  return convert_enum_idx(options, 6, value, "mib_mbms_r14_s::dl_bw_mbms_r14_e_");
}
uint8_t mib_mbms_r14_s::dl_bw_mbms_r14_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(options, 6, value, "mib_mbms_r14_s::dl_bw_mbms_r14_e_");
}

// BCCH-BCH-Message-MBMS ::= SEQUENCE
SRSASN_CODE bcch_bch_msg_mbms_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_bch_msg_mbms_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void bcch_bch_msg_mbms_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("BCCH-BCH-Message-MBMS");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

const char* plmn_id_info_r15_s::plmn_id_minus5_gc_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"plmn-Identity-r15", "plmn-Index-r15"};
  return convert_enum_idx(options, 2, value, "plmn_id_info_r15_s::plmn_id_minus5_gc_r15_c_::types");
}

const char* cell_id_minus5_gc_r15_c::types_opts::to_string() const
{
  static const char* options[] = {"cellIdentity-r15", "cellId-Index-r15"};
  return convert_enum_idx(options, 2, value, "cell_id_minus5_gc_r15_c::types");
}

const char* sl_tx_pwr_r14_c::types_opts::to_string() const
{
  static const char* options[] = {"minusinfinity-r14", "txPower-r14"};
  return convert_enum_idx(options, 2, value, "sl_tx_pwr_r14_c::types");
}
int8_t sl_tx_pwr_r14_c::types_opts::to_number() const
{
  static const int8_t options[] = {-1};
  return map_enum_number(options, 1, value, "sl_tx_pwr_r14_c::types");
}

const char* sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"crs-IntfMitigEnabled", "crs-IntfMitigNumPRBs"};
  return convert_enum_idx(options, 2, value, "sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::types");
}

const char* sl_offset_ind_r12_c::types_opts::to_string() const
{
  static const char* options[] = {"small-r12", "large-r12"};
  return convert_enum_idx(options, 2, value, "sl_offset_ind_r12_c::types");
}

const char* sf_bitmap_sl_r12_c::types_opts::to_string() const
{
  static const char* options[] = {"bs4-r12", "bs8-r12", "bs12-r12", "bs16-r12", "bs30-r12", "bs40-r12", "bs42-r12"};
  return convert_enum_idx(options, 7, value, "sf_bitmap_sl_r12_c::types");
}
uint8_t sf_bitmap_sl_r12_c::types_opts::to_number() const
{
  static const uint8_t options[] = {4, 8, 12, 16, 30, 40, 42};
  return map_enum_number(options, 7, value, "sf_bitmap_sl_r12_c::types");
}

const char* sf_bitmap_sl_r14_c::types_opts::to_string() const
{
  static const char* options[] = {
      "bs10-r14", "bs16-r14", "bs20-r14", "bs30-r14", "bs40-r14", "bs50-r14", "bs60-r14", "bs100-r14"};
  return convert_enum_idx(options, 8, value, "sf_bitmap_sl_r14_c::types");
}
uint8_t sf_bitmap_sl_r14_c::types_opts::to_number() const
{
  static const uint8_t options[] = {10, 16, 20, 30, 40, 50, 60, 100};
  return map_enum_number(options, 8, value, "sf_bitmap_sl_r14_c::types");
}

const char* setup_opts::to_string() const
{
  static const char* options[] = {"release", "setup"};
  return convert_enum_idx(options, 2, value, "setup_e");
}

const char*
sl_disc_res_pool_r12_s::tx_params_r12_s_::ue_sel_res_cfg_r12_s_::pool_sel_r12_c_::types_opts::to_string() const
{
  static const char* options[] = {"rsrpBased-r12", "random-r12"};
  return convert_enum_idx(
      options, 2, value, "sl_disc_res_pool_r12_s::tx_params_r12_s_::ue_sel_res_cfg_r12_s_::pool_sel_r12_c_::types");
}

const char* plmn_id_info2_r12_c::types_opts::to_string() const
{
  static const char* options[] = {"plmn-Index-r12", "plmnIdentity-r12"};
  return convert_enum_idx(options, 2, value, "plmn_id_info2_r12_c::types");
}

const char* sl_disc_tx_res_inter_freq_r13_c::types_opts::to_string() const
{
  static const char* options[] = {
      "acquireSI-FromCarrier-r13", "discTxPoolCommon-r13", "requestDedicated-r13", "noTxOnCarrier-r13"};
  return convert_enum_idx(options, 4, value, "sl_disc_tx_res_inter_freq_r13_c::types");
}

const char*
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"interval-FDD-r13", "interval-TDD-r13"};
  return convert_enum_idx(
      options,
      2,
      value,
      "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::types");
}

const char*
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"interval-FDD-r13", "interval-TDD-r13"};
  return convert_enum_idx(
      options,
      2,
      value,
      "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::types");
}

const char* sys_time_info_cdma2000_s::cdma_sys_time_c_::types_opts::to_string() const
{
  static const char* options[] = {"synchronousSystemTime", "asynchronousSystemTime"};
  return convert_enum_idx(options, 2, value, "sys_time_info_cdma2000_s::cdma_sys_time_c_::types");
}

const char* carrier_freqs_geran_s::following_arfcns_c_::types_opts::to_string() const
{
  static const char* options[] = {"explicitListOfARFCNs", "equallySpacedARFCNs", "variableBitMapOfARFCNs"};
  return convert_enum_idx(options, 3, value, "carrier_freqs_geran_s::following_arfcns_c_::types");
}

const char* mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"sf5-r15", "sf10-r15", "sf20-r15", "sf40-r15", "sf80-r15", "sf160-r15"};
  return convert_enum_idx(options, 6, value, "mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_::types");
}
uint8_t mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {5, 10, 20, 40, 80, 160};
  return map_enum_number(options, 6, value, "mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_::types");
}

const char* params_cdma2000_r11_s::sys_time_info_r11_c_::types_opts::to_string() const
{
  static const char* options[] = {"explicitValue", "defaultValue"};
  return convert_enum_idx(options, 2, value, "params_cdma2000_r11_s::sys_time_info_r11_c_::types");
}

const char* sl_inter_freq_info_v2x_r14_s::add_spec_emission_v2x_r14_c_::types_opts::to_string() const
{
  static const char* options[] = {"additionalSpectrumEmission-r14", "additionalSpectrumEmission-v1440"};
  return convert_enum_idx(options, 2, value, "sl_inter_freq_info_v2x_r14_s::add_spec_emission_v2x_r14_c_::types");
}

const char* ssb_to_measure_r15_c::types_opts::to_string() const
{
  static const char* options[] = {"shortBitmap-r15", "mediumBitmap-r15", "longBitmap-r15"};
  return convert_enum_idx(options, 3, value, "ssb_to_measure_r15_c::types");
}

const char*
sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::fdd_dl_or_tdd_sf_bitmap_br_r13_c_::types_opts::to_string()
    const
{
  static const char* options[] = {"subframePattern10-r13", "subframePattern40-r13"};
  return convert_enum_idx(
      options,
      2,
      value,
      "sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::fdd_dl_or_tdd_sf_bitmap_br_r13_c_::types");
}
uint8_t
sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::fdd_dl_or_tdd_sf_bitmap_br_r13_c_::types_opts::to_number()
    const
{
  static const uint8_t options[] = {10, 40};
  return map_enum_number(
      options,
      2,
      value,
      "sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::fdd_dl_or_tdd_sf_bitmap_br_r13_c_::types");
}

const char* freq_hop_params_r13_s::dummy2_c_::types_opts::to_string() const
{
  static const char* options[] = {"interval-FDD-r13", "interval-TDD-r13"};
  return convert_enum_idx(options, 2, value, "freq_hop_params_r13_s::dummy2_c_::types");
}

const char* freq_hop_params_r13_s::dummy3_c_::types_opts::to_string() const
{
  static const char* options[] = {"interval-FDD-r13", "interval-TDD-r13"};
  return convert_enum_idx(options, 2, value, "freq_hop_params_r13_s::dummy3_c_::types");
}

const char* freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"interval-FDD-r13", "interval-TDD-r13"};
  return convert_enum_idx(options, 2, value, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::types");
}

const char* freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"interval-FDD-r13", "interval-TDD-r13"};
  return convert_enum_idx(options, 2, value, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::types");
}

const char* mbsfn_sf_cfg_s::sf_alloc_c_::types_opts::to_string() const
{
  static const char* options[] = {"oneFrame", "fourFrames"};
  return convert_enum_idx(options, 2, value, "mbsfn_sf_cfg_s::sf_alloc_c_::types");
}
uint8_t mbsfn_sf_cfg_s::sf_alloc_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1, 4};
  return map_enum_number(options, 2, value, "mbsfn_sf_cfg_s::sf_alloc_c_::types");
}

const char* mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_::types_opts::to_string() const
{
  static const char* options[] = {"oneFrame-v1430", "fourFrames-v1430"};
  return convert_enum_idx(options, 2, value, "mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_::types");
}
uint8_t mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1, 4};
  return map_enum_number(options, 2, value, "mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_::types");
}

const char* prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"fdd-r13", "tdd-r13"};
  return convert_enum_idx(options, 2, value, "prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::types");
}

const char* sib8_per_plmn_r11_s::params_cdma2000_r11_c_::types_opts::to_string() const
{
  static const char* options[] = {"explicitValue", "defaultValue"};
  return convert_enum_idx(options, 2, value, "sib8_per_plmn_r11_s::params_cdma2000_r11_c_::types");
}

const char* uac_barr_per_plmn_r15_s::uac_ac_barr_list_type_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"uac-ImplicitAC-BarringList-r15", "uac-ExplicitAC-BarringList-r15"};
  return convert_enum_idx(options, 2, value, "uac_barr_per_plmn_r15_s::uac_ac_barr_list_type_r15_c_::types");
}

const char* sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::types_opts::to_string() const
{
  static const char* options[] = {"sf10",
                                  "sf20",
                                  "sf32",
                                  "sf40",
                                  "sf64",
                                  "sf80",
                                  "sf128",
                                  "sf160",
                                  "sf256",
                                  "sf320",
                                  "sf512",
                                  "sf640",
                                  "sf1024",
                                  "sf2048",
                                  "sf4096",
                                  "sf8192"};
  return convert_enum_idx(
      options, 16, value, "sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::types");
}
uint16_t sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::types_opts::to_number() const
{
  static const uint16_t options[] = {10, 20, 32, 40, 64, 80, 128, 160, 256, 320, 512, 640, 1024, 2048, 4096, 8192};
  return map_enum_number(options, 16, value, "sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::types");
}

const char* sib_type14_r11_s::eab_param_r11_c_::types_opts::to_string() const
{
  static const char* options[] = {"eab-Common-r11", "eab-PerPLMN-List-r11"};
  return convert_enum_idx(options, 2, value, "sib_type14_r11_s::eab_param_r11_c_::types");
}

const char* sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::types_opts::to_string() const
{
  static const char* options[] = {"fdd-r14", "tdd-r14"};
  return convert_enum_idx(
      options, 2, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::types");
}

const char* sib_type25_r15_s::uac_ac1_select_assist_info_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"plmnCommon-r15", "individualPLMNList-r15"};
  return convert_enum_idx(options, 2, value, "sib_type25_r15_s::uac_ac1_select_assist_info_r15_c_::types");
}

const char* pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "posSib1-1-r15",  "posSib1-2-r15",  "posSib1-3-r15",  "posSib1-4-r15",  "posSib1-5-r15",  "posSib1-6-r15",
      "posSib1-7-r15",  "posSib2-1-r15",  "posSib2-2-r15",  "posSib2-3-r15",  "posSib2-4-r15",  "posSib2-5-r15",
      "posSib2-6-r15",  "posSib2-7-r15",  "posSib2-8-r15",  "posSib2-9-r15",  "posSib2-10-r15", "posSib2-11-r15",
      "posSib2-12-r15", "posSib2-13-r15", "posSib2-14-r15", "posSib2-15-r15", "posSib2-16-r15", "posSib2-17-r15",
      "posSib2-18-r15", "posSib2-19-r15", "posSib3-1-r15"};
  return convert_enum_idx(options, 27, value, "pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::types");
}

const char* sys_info_r8_ies_s::sib_type_and_info_item_c_::types_opts::to_string() const
{
  static const char* options[] = {"sib2",        "sib3",        "sib4",        "sib5",        "sib6",
                                  "sib7",        "sib8",        "sib9",        "sib10",       "sib11",
                                  "sib12-v920",  "sib13-v920",  "sib14-v1130", "sib15-v1130", "sib16-v1130",
                                  "sib17-v1250", "sib18-v1250", "sib19-v1250", "sib20-v1310", "sib21-v1430",
                                  "sib24-v1530", "sib25-v1530", "sib26-v1530"};
  return convert_enum_idx(options, 23, value, "sib_info_item_c::types");
}
uint8_t sib_info_item_c::types_opts::to_number() const
{
  static const uint8_t options[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 24, 25, 26};
  return map_enum_number(options, 23, value, "sib_info_item_c::types");
}

const char* sys_info_s::crit_exts_c_::crit_exts_future_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"posSystemInformation-r15", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "sys_info_s::crit_exts_c_::crit_exts_future_r15_c_::types");
}

const char* sys_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"systemInformation-r8", "criticalExtensionsFuture-r15"};
  return convert_enum_idx(options, 2, value, "sys_info_s::crit_exts_c_::types");
}

const char* bcch_dl_sch_msg_type_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"systemInformation", "systemInformationBlockType1"};
  return convert_enum_idx(options, 2, value, "bcch_dl_sch_msg_type_c::c1_c_::types");
}
uint8_t bcch_dl_sch_msg_type_c::c1_c_::types_opts::to_number() const
{
  if (value == sib_type1) {
    return 1;
  }
  invalid_enum_number(value, "bcch_dl_sch_msg_type_c::c1_c_::types");
  return 0;
}

const char* bcch_dl_sch_msg_type_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "bcch_dl_sch_msg_type_c::types");
}
uint8_t bcch_dl_sch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "bcch_dl_sch_msg_type_c::types");
}

const char* bcch_dl_sch_msg_type_br_r13_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"systemInformation-BR-r13", "systemInformationBlockType1-BR-r13"};
  return convert_enum_idx(options, 2, value, "bcch_dl_sch_msg_type_br_r13_c::c1_c_::types");
}
uint8_t bcch_dl_sch_msg_type_br_r13_c::c1_c_::types_opts::to_number() const
{
  if (value == sib_type1_br_r13) {
    return 1;
  }
  invalid_enum_number(value, "bcch_dl_sch_msg_type_br_r13_c::c1_c_::types");
  return 0;
}

const char* bcch_dl_sch_msg_type_br_r13_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "bcch_dl_sch_msg_type_br_r13_c::types");
}
uint8_t bcch_dl_sch_msg_type_br_r13_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "bcch_dl_sch_msg_type_br_r13_c::types");
}

// BCCH-DL-SCH-MessageType-MBMS-r14 ::= CHOICE
void bcch_dl_sch_msg_type_mbms_r14_c::set(types::options e)
{
  type_ = e;
}
bcch_dl_sch_msg_type_mbms_r14_c::c1_c_& bcch_dl_sch_msg_type_mbms_r14_c::set_c1()
{
  set(types::c1);
  return c;
}
void bcch_dl_sch_msg_type_mbms_r14_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
void bcch_dl_sch_msg_type_mbms_r14_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.to_json(j);
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_mbms_r14_c");
  }
  j.end_obj();
}
SRSASN_CODE bcch_dl_sch_msg_type_mbms_r14_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_mbms_r14_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_dl_sch_msg_type_mbms_r14_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_mbms_r14_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void bcch_dl_sch_msg_type_mbms_r14_c::c1_c_::destroy_()
{
  switch (type_) {
    case types::sys_info_mbms_r14:
      c.destroy<sys_info_mbms_r14_s>();
      break;
    case types::sib_type1_mbms_r14:
      c.destroy<sib_type1_mbms_r14_s>();
      break;
    default:
      break;
  }
}
void bcch_dl_sch_msg_type_mbms_r14_c::c1_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sys_info_mbms_r14:
      c.init<sys_info_mbms_r14_s>();
      break;
    case types::sib_type1_mbms_r14:
      c.init<sib_type1_mbms_r14_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_mbms_r14_c::c1_c_");
  }
}
bcch_dl_sch_msg_type_mbms_r14_c::c1_c_::c1_c_(const bcch_dl_sch_msg_type_mbms_r14_c::c1_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sys_info_mbms_r14:
      c.init(other.c.get<sys_info_mbms_r14_s>());
      break;
    case types::sib_type1_mbms_r14:
      c.init(other.c.get<sib_type1_mbms_r14_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_mbms_r14_c::c1_c_");
  }
}
bcch_dl_sch_msg_type_mbms_r14_c::c1_c_&
bcch_dl_sch_msg_type_mbms_r14_c::c1_c_::operator=(const bcch_dl_sch_msg_type_mbms_r14_c::c1_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sys_info_mbms_r14:
      c.set(other.c.get<sys_info_mbms_r14_s>());
      break;
    case types::sib_type1_mbms_r14:
      c.set(other.c.get<sib_type1_mbms_r14_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_mbms_r14_c::c1_c_");
  }

  return *this;
}
sys_info_mbms_r14_s& bcch_dl_sch_msg_type_mbms_r14_c::c1_c_::set_sys_info_mbms_r14()
{
  set(types::sys_info_mbms_r14);
  return c.get<sys_info_mbms_r14_s>();
}
sib_type1_mbms_r14_s& bcch_dl_sch_msg_type_mbms_r14_c::c1_c_::set_sib_type1_mbms_r14()
{
  set(types::sib_type1_mbms_r14);
  return c.get<sib_type1_mbms_r14_s>();
}
void bcch_dl_sch_msg_type_mbms_r14_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sys_info_mbms_r14:
      j.write_fieldname("systemInformation-MBMS-r14");
      c.get<sys_info_mbms_r14_s>().to_json(j);
      break;
    case types::sib_type1_mbms_r14:
      j.write_fieldname("systemInformationBlockType1-MBMS-r14");
      c.get<sib_type1_mbms_r14_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_mbms_r14_c::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE bcch_dl_sch_msg_type_mbms_r14_c::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sys_info_mbms_r14:
      HANDLE_CODE(c.get<sys_info_mbms_r14_s>().pack(bref));
      break;
    case types::sib_type1_mbms_r14:
      HANDLE_CODE(c.get<sib_type1_mbms_r14_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_mbms_r14_c::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_dl_sch_msg_type_mbms_r14_c::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sys_info_mbms_r14:
      HANDLE_CODE(c.get<sys_info_mbms_r14_s>().unpack(bref));
      break;
    case types::sib_type1_mbms_r14:
      HANDLE_CODE(c.get<sib_type1_mbms_r14_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bcch_dl_sch_msg_type_mbms_r14_c::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* bcch_dl_sch_msg_type_mbms_r14_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"systemInformation-MBMS-r14", "systemInformationBlockType1-MBMS-r14"};
  return convert_enum_idx(options, 2, value, "bcch_dl_sch_msg_type_mbms_r14_c::c1_c_::types");
}
uint8_t bcch_dl_sch_msg_type_mbms_r14_c::c1_c_::types_opts::to_number() const
{
  if (value == sib_type1_mbms_r14) {
    return 1;
  }
  invalid_enum_number(value, "bcch_dl_sch_msg_type_mbms_r14_c::c1_c_::types");
  return 0;
}

const char* bcch_dl_sch_msg_type_mbms_r14_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "bcch_dl_sch_msg_type_mbms_r14_c::types");
}
uint8_t bcch_dl_sch_msg_type_mbms_r14_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "bcch_dl_sch_msg_type_mbms_r14_c::types");
}

// BCCH-DL-SCH-Message-MBMS ::= SEQUENCE
SRSASN_CODE bcch_dl_sch_msg_mbms_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_dl_sch_msg_mbms_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void bcch_dl_sch_msg_mbms_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("BCCH-DL-SCH-Message-MBMS");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

const char* p_c_and_cbsr_r13_s::cbsr_sel_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"nonPrecoded-r13", "beamformedK1a-r13", "beamformedKN-r13"};
  return convert_enum_idx(options, 3, value, "p_c_and_cbsr_r13_s::cbsr_sel_r13_c_::types");
}
uint8_t p_c_and_cbsr_r13_s::cbsr_sel_r13_c_::types_opts::to_number() const
{
  if (value == bf_k1a_r13) {
    return 1;
  }
  invalid_enum_number(value, "p_c_and_cbsr_r13_s::cbsr_sel_r13_c_::types");
  return 0;
}

const char* csi_rs_cfg_emimo_r13_c::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"nonPrecoded-r13", "beamformed-r13"};
  return convert_enum_idx(options, 2, value, "csi_rs_cfg_emimo_r13_c::setup_c_::types");
}

const char* csi_rs_cfg_emimo_v1430_c::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"nonPrecoded-v1430", "beamformed-v1430"};
  return convert_enum_idx(options, 2, value, "csi_rs_cfg_emimo_v1430_c::setup_c_::types");
}

const char* csi_rs_cfg_emimo_v1480_c::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"nonPrecoded-v1480", "beamformed-v1480"};
  return convert_enum_idx(options, 2, value, "csi_rs_cfg_emimo_v1480_c::setup_c_::types");
}

const char* csi_rs_cfg_emimo_v1530_c::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"nonPrecoded-v1530"};
  return convert_enum_idx(options, 1, value, "csi_rs_cfg_emimo_v1530_c::setup_c_::types");
}

const char* cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::types_opts::to_string() const
{
  static const char* options[] = {"widebandCQI-r11", "subbandCQI-r11"};
  return convert_enum_idx(
      options, 2, value, "cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::types");
}

const char* cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_::types_opts::to_string() const
{
  static const char* options[] = {"oneBit-r14", "twoBit-r14", "threeBit-r14"};
  return convert_enum_idx(options, 3, value, "cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_::types");
}
uint8_t cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3};
  return map_enum_number(options, 3, value, "cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_::types");
}

const char* cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::types_opts::to_string() const
{
  static const char* options[] = {"widebandCQI-r10", "subbandCQI-r10"};
  return convert_enum_idx(
      options, 2, value, "cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::types");
}

const char* meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_::types_opts::to_string() const
{
  static const char* options[] = {"subframeConfig1-5-r10", "subframeConfig0-r10", "subframeConfig6-r10"};
  return convert_enum_idx(options, 3, value, "meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_::types");
}
uint8_t meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1, 0, 6};
  return map_enum_number(options, 3, value, "meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_::types");
}

const char* meas_sf_pattern_r10_c::types_opts::to_string() const
{
  static const char* options[] = {"subframePatternFDD-r10", "subframePatternTDD-r10"};
  return convert_enum_idx(options, 2, value, "meas_sf_pattern_r10_c::types");
}

const char* tpc_idx_c::types_opts::to_string() const
{
  static const char* options[] = {"indexOfFormat3", "indexOfFormat3A"};
  return convert_enum_idx(options, 2, value, "tpc_idx_c::types");
}

const char*
epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"fdd-r13", "tdd-r13"};
  return convert_enum_idx(
      options, 2, value, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::types");
}

const char* enable256_qam_r14_c::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"tpc-SubframeSet-Configured-r14", "tpc-SubframeSet-NotConfigured-r14"};
  return convert_enum_idx(options, 2, value, "enable256_qam_r14_c::setup_c_::types");
}

const char* rlc_cfg_r15_s::mode_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "am-r15", "um-Bi-Directional-r15", "um-Uni-Directional-UL-r15", "um-Uni-Directional-DL-r15"};
  return convert_enum_idx(options, 4, value, "rlc_cfg_r15_s::mode_r15_c_::types");
}

const char* ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_::types_opts::to_string() const
{
  static const char* options[] = {"n2TxAntenna-tm3-r15",
                                  "n4TxAntenna-tm3-r15",
                                  "n2TxAntenna-tm4-r15",
                                  "n4TxAntenna-tm4-r15",
                                  "n2TxAntenna-tm5-r15",
                                  "n4TxAntenna-tm5-r15",
                                  "n2TxAntenna-tm6-r15",
                                  "n4TxAntenna-tm6-r15",
                                  "n2TxAntenna-tm8-r15",
                                  "n4TxAntenna-tm8-r15",
                                  "n2TxAntenna-tm9and10-r15",
                                  "n4TxAntenna-tm9and10-r15",
                                  "n8TxAntenna-tm9and10-r15"};
  return convert_enum_idx(options, 13, value, "ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_::types");
}

const char* cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_::types_opts::to_string() const
{
  static const char* options[] = {"widebandCQI", "subbandCQI"};
  return convert_enum_idx(options, 2, value, "cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_::types");
}

const char* pdcp_cfg_s::hdr_compress_c_::types_opts::to_string() const
{
  static const char* options[] = {"notUsed", "rohc"};
  return convert_enum_idx(options, 2, value, "pdcp_cfg_s::hdr_compress_c_::types");
}

const char* pdcp_cfg_s::ul_only_hdr_compress_r14_c_::types_opts::to_string() const
{
  static const char* options[] = {"notUsed-r14", "rohc-r14"};
  return convert_enum_idx(options, 2, value, "pdcp_cfg_s::ul_only_hdr_compress_r14_c_::types");
}

const char* rlc_bearer_cfg_r15_c::setup_s_::lc_ch_id_cfg_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"logicalChannelIdentity-r15", "logicalChannelIdentityExt-r15"};
  return convert_enum_idx(options, 2, value, "rlc_bearer_cfg_r15_c::setup_s_::lc_ch_id_cfg_r15_c_::types");
}

const char* rlc_cfg_c::types_opts::to_string() const
{
  static const char* options[] = {"am", "um-Bi-Directional", "um-Uni-Directional-UL", "um-Uni-Directional-DL"};
  return convert_enum_idx(options, 4, value, "rlc_cfg_c::types");
}

const char* ant_info_ded_s::codebook_subset_restrict_c_::types_opts::to_string() const
{
  static const char* options[] = {"n2TxAntenna-tm3",
                                  "n4TxAntenna-tm3",
                                  "n2TxAntenna-tm4",
                                  "n4TxAntenna-tm4",
                                  "n2TxAntenna-tm5",
                                  "n4TxAntenna-tm5",
                                  "n2TxAntenna-tm6",
                                  "n4TxAntenna-tm6"};
  return convert_enum_idx(options, 8, value, "ant_info_ded_s::codebook_subset_restrict_c_::types");
}

const char* ant_info_ded_v1530_c::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"ue-TxAntennaSelection-SRS-1T4R-Config-r15",
                                  "ue-TxAntennaSelection-SRS-2T4R-NrOfPairs-r15"};
  return convert_enum_idx(options, 2, value, "ant_info_ded_v1530_c::setup_c_::types");
}
int8_t ant_info_ded_v1530_c::setup_c_::types_opts::to_number() const
{
  static const int8_t options[] = {-1, -2};
  return map_enum_number(options, 2, value, "ant_info_ded_v1530_c::setup_c_::types");
}

const char* ant_info_ded_v920_s::codebook_subset_restrict_v920_c_::types_opts::to_string() const
{
  static const char* options[] = {"n2TxAntenna-tm8-r9", "n4TxAntenna-tm8-r9"};
  return convert_enum_idx(options, 2, value, "ant_info_ded_v920_s::codebook_subset_restrict_v920_c_::types");
}
uint8_t ant_info_ded_v920_s::codebook_subset_restrict_v920_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "ant_info_ded_v920_s::codebook_subset_restrict_v920_c_::types");
}

const char* drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::types_opts::to_string() const
{
  static const char* options[] = {"sf10",
                                  "sf20",
                                  "sf32",
                                  "sf40",
                                  "sf64",
                                  "sf80",
                                  "sf128",
                                  "sf160",
                                  "sf256",
                                  "sf320",
                                  "sf512",
                                  "sf640",
                                  "sf1024",
                                  "sf1280",
                                  "sf2048",
                                  "sf2560"};
  return convert_enum_idx(options, 16, value, "drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::types");
}
uint16_t drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::types_opts::to_number() const
{
  static const uint16_t options[] = {10, 20, 32, 40, 64, 80, 128, 160, 256, 320, 512, 640, 1024, 1280, 2048, 2560};
  return map_enum_number(options, 16, value, "drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::types");
}

const char* drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_::types_opts::to_string() const
{
  static const char* options[] = {"sf60-v1130", "sf70-v1130"};
  return convert_enum_idx(options, 2, value, "drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_::types");
}
uint8_t drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {60, 70};
  return map_enum_number(options, 2, value, "drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_::types");
}

const char* pucch_cfg_ded_r13_s::pucch_format_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"format3-r13", "channelSelection-r13", "format4-r13", "format5-r13"};
  return convert_enum_idx(options, 4, value, "pucch_cfg_ded_r13_s::pucch_format_r13_c_::types");
}
uint8_t pucch_cfg_ded_r13_s::pucch_format_r13_c_::types_opts::to_number() const
{
  switch (value) {
    case format3_r13:
      return 3;
    case format4_r13:
      return 4;
    case format5_r13:
      return 5;
    default:
      invalid_enum_number(value, "pucch_cfg_ded_r13_s::pucch_format_r13_c_::types");
  }
  return 0;
}

const char* pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"modeA", "modeB"};
  return convert_enum_idx(options, 2, value, "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::types");
}

const char* pucch_cfg_ded_v1020_s::pucch_format_r10_c_::types_opts::to_string() const
{
  static const char* options[] = {"format3-r10", "channelSelection-r10"};
  return convert_enum_idx(options, 2, value, "pucch_cfg_ded_v1020_s::pucch_format_r10_c_::types");
}
uint8_t pucch_cfg_ded_v1020_s::pucch_format_r10_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {3};
  return map_enum_number(options, 1, value, "pucch_cfg_ded_v1020_s::pucch_format_r10_c_::types");
}

const char* pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::types_opts::to_string() const
{
  static const char* options[] = {"interval-FDD-PUSCH-Enh-r14", "interval-TDD-PUSCH-Enh-r14"};
  return convert_enum_idx(
      options, 2, value, "pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::types");
}

const char* srb_to_add_mod_s::rlc_cfg_c_::types_opts::to_string() const
{
  static const char* options[] = {"explicitValue", "defaultValue"};
  return convert_enum_idx(options, 2, value, "srb_to_add_mod_s::rlc_cfg_c_::types");
}

const char* srb_to_add_mod_s::lc_ch_cfg_c_::types_opts::to_string() const
{
  static const char* options[] = {"explicitValue", "defaultValue"};
  return convert_enum_idx(options, 2, value, "srb_to_add_mod_s::lc_ch_cfg_c_::types");
}

const char* mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"sf5120", "sf10240"};
  return convert_enum_idx(options, 2, value, "mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::setup_c_::types");
}
uint16_t mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::setup_c_::types_opts::to_number() const
{
  static const uint16_t options[] = {5120, 10240};
  return map_enum_number(options, 2, value, "mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::setup_c_::types");
}

const char* phys_cfg_ded_s::ant_info_c_::types_opts::to_string() const
{
  static const char* options[] = {"explicitValue", "defaultValue"};
  return convert_enum_idx(options, 2, value, "phys_cfg_ded_s::ant_info_c_::types");
}

const char* phys_cfg_ded_s::ant_info_r10_c_::types_opts::to_string() const
{
  static const char* options[] = {"explicitValue-r10", "defaultValue"};
  return convert_enum_idx(options, 2, value, "phys_cfg_ded_s::ant_info_r10_c_::types");
}

const char* phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"cfi-Config-r15", "cfi-PatternConfig-r15"};
  return convert_enum_idx(options, 2, value, "phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::setup_c_::types");
}

const char* rr_cfg_ded_s::mac_main_cfg_c_::types_opts::to_string() const
{
  static const char* options[] = {"explicitValue", "defaultValue"};
  return convert_enum_idx(options, 2, value, "rr_cfg_ded_s::mac_main_cfg_c_::types");
}

const char* rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"crs-IntfMitigEnabled", "crs-IntfMitigNumPRBs"};
  return convert_enum_idx(options, 2, value, "rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::types");
}

const char* redirected_carrier_info_r15_ies_c::types_opts::to_string() const
{
  static const char* options[] = {
      "eutra-r15", "geran-r15", "utra-FDD-r15", "cdma2000-HRPD-r15", "cdma2000-1xRTT-r15", "utra-TDD-r15"};
  return convert_enum_idx(options, 6, value, "redirected_carrier_info_r15_ies_c::types");
}

const char* c1_or_crit_ext_opts::to_string() const
{
  static const char* options[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "c1_or_crit_ext_e");
}
uint8_t c1_or_crit_ext_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "c1_or_crit_ext_e");
}

const char* rrc_conn_reest_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "rrcConnectionReestablishment-r8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "rrc_conn_reest_s::crit_exts_c_::c1_c_::types");
}

const char* rrc_conn_reest_reject_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionReestablishmentReject-r8", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_conn_reest_reject_s::crit_exts_c_::types");
}

const char* rrc_conn_reject_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionReject-r8", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "rrc_conn_reject_s::crit_exts_c_::c1_c_::types");
}

const char* rrc_conn_setup_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "rrcConnectionSetup-r8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "rrc_conn_setup_s::crit_exts_c_::c1_c_::types");
}

const char* rrc_early_data_complete_r15_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcEarlyDataComplete-r15", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_early_data_complete_r15_s::crit_exts_c_::types");
}

const char* dl_ccch_msg_type_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionReestablishment",
                                  "rrcConnectionReestablishmentReject",
                                  "rrcConnectionReject",
                                  "rrcConnectionSetup"};
  return convert_enum_idx(options, 4, value, "dl_ccch_msg_type_c::c1_c_::types");
}

const char* dl_ccch_msg_type_c::msg_class_ext_c_::c2_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcEarlyDataComplete-r15", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "dl_ccch_msg_type_c::msg_class_ext_c_::c2_c_::types");
}

const char* dl_ccch_msg_type_c::msg_class_ext_c_::types_opts::to_string() const
{
  static const char* options[] = {"c2", "messageClassExtensionFuture-r15"};
  return convert_enum_idx(options, 2, value, "dl_ccch_msg_type_c::msg_class_ext_c_::types");
}
uint8_t dl_ccch_msg_type_c::msg_class_ext_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {2};
  return map_enum_number(options, 1, value, "dl_ccch_msg_type_c::msg_class_ext_c_::types");
}

const char* dl_ccch_msg_type_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "dl_ccch_msg_type_c::types");
}
uint8_t dl_ccch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "dl_ccch_msg_type_c::types");
}

const char* cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"widebandCQI-r15", "subbandCQI-r15"};
  return convert_enum_idx(
      options, 2, value, "cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::types");
}

const char* lbt_cfg_r14_c::types_opts::to_string() const
{
  static const char* options[] = {"maxEnergyDetectionThreshold-r14", "energyDetectionThresholdOffset-r14"};
  return convert_enum_idx(options, 2, value, "lbt_cfg_r14_c::types");
}

const char* cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"widebandCQI-Short-r15", "subbandCQI-Short-r15"};
  return convert_enum_idx(options, 2, value, "cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::types");
}

const char* cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_::types_opts::to_string() const
{
  static const char* options[] = {"own-r10", "other-r10"};
  return convert_enum_idx(options, 2, value, "cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_::types");
}

const char* cross_carrier_sched_cfg_r13_s::sched_cell_info_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"own-r13", "other-r13"};
  return convert_enum_idx(options, 2, value, "cross_carrier_sched_cfg_r13_s::sched_cell_info_r13_c_::types");
}

const char* phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"cfi-Config-r15", "cfi-PatternConfig-r15"};
  return convert_enum_idx(options, 2, value, "phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::setup_c_::types");
}

const char* meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_::types_opts::to_string() const
{
  static const char* options[] = {"ms40-r12", "ms80-r12", "ms160-r12"};
  return convert_enum_idx(options, 3, value, "meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_::types");
}
uint8_t meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {40, 80, 160};
  return map_enum_number(options, 3, value, "meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_::types");
}

const char* meas_ds_cfg_r12_c::setup_s_::ds_occasion_dur_r12_c_::types_opts::to_string() const
{
  static const char* options[] = {"durationFDD-r12", "durationTDD-r12"};
  return convert_enum_idx(options, 2, value, "meas_ds_cfg_r12_c::setup_s_::ds_occasion_dur_r12_c_::types");
}

const char* meas_gap_cfg_c::setup_s_::gap_offset_c_::types_opts::to_string() const
{
  static const char* options[] = {"gp0",
                                  "gp1",
                                  "gp2-r14",
                                  "gp3-r14",
                                  "gp-ncsg0-r14",
                                  "gp-ncsg1-r14",
                                  "gp-ncsg2-r14",
                                  "gp-ncsg3-r14",
                                  "gp-nonUniform1-r14",
                                  "gp-nonUniform2-r14",
                                  "gp-nonUniform3-r14",
                                  "gp-nonUniform4-r14",
                                  "gp4-r15",
                                  "gp5-r15",
                                  "gp6-r15",
                                  "gp7-r15",
                                  "gp8-r15",
                                  "gp9-r15",
                                  "gp10-r15",
                                  "gp11-r15"};
  return convert_enum_idx(options, 20, value, "meas_gap_cfg_c::setup_s_::gap_offset_c_::types");
}

const char* thres_eutra_c::types_opts::to_string() const
{
  static const char* options[] = {"threshold-RSRP", "threshold-RSRQ"};
  return convert_enum_idx(options, 2, value, "thres_eutra_c::types");
}

const char* thres_nr_r15_c::types_opts::to_string() const
{
  static const char* options[] = {"nr-RSRP-r15", "nr-RSRQ-r15", "nr-SINR-r15"};
  return convert_enum_idx(options, 3, value, "thres_nr_r15_c::types");
}

const char* thres_utra_c::types_opts::to_string() const
{
  static const char* options[] = {"utra-RSCP", "utra-EcN0"};
  return convert_enum_idx(options, 2, value, "thres_utra_c::types");
}
uint8_t thres_utra_c::types_opts::to_number() const
{
  if (value == utra_ec_n0) {
    return 0;
  }
  invalid_enum_number(value, "thres_utra_c::types");
  return 0;
}

const char* meas_obj_utra_s::cells_to_add_mod_list_c_::types_opts::to_string() const
{
  static const char* options[] = {"cellsToAddModListUTRA-FDD", "cellsToAddModListUTRA-TDD"};
  return convert_enum_idx(options, 2, value, "meas_obj_utra_s::cells_to_add_mod_list_c_::types");
}

const char* meas_obj_utra_s::cell_for_which_to_report_cgi_c_::types_opts::to_string() const
{
  static const char* options[] = {"utra-FDD", "utra-TDD"};
  return convert_enum_idx(options, 2, value, "meas_obj_utra_s::cell_for_which_to_report_cgi_c_::types");
}

const char* meas_obj_wlan_r13_s::carrier_freq_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"bandIndicatorListWLAN-r13", "carrierInfoListWLAN-r13"};
  return convert_enum_idx(options, 2, value, "meas_obj_wlan_r13_s::carrier_freq_r13_c_::types");
}

const char* eutra_event_s::event_id_c_::types_opts::to_string() const
{
  static const char* options[] = {"eventA1",
                                  "eventA2",
                                  "eventA3",
                                  "eventA4",
                                  "eventA5",
                                  "eventA6-r10",
                                  "eventC1-r12",
                                  "eventC2-r12",
                                  "eventV1-r14",
                                  "eventV2-r14",
                                  "eventH1-r15",
                                  "eventH2-r15"};
  return convert_enum_idx(options, 12, value, "eutra_event_s::event_id_c_::types");
}

const char* report_cfg_eutra_s::trigger_type_c_::types_opts::to_string() const
{
  static const char* options[] = {"event", "periodical"};
  return convert_enum_idx(options, 2, value, "report_cfg_eutra_s::trigger_type_c_::types");
}

const char*
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b1_s_::b1_thres_c_::types_opts::to_string() const
{
  static const char* options[] = {"b1-ThresholdUTRA", "b1-ThresholdGERAN", "b1-ThresholdCDMA2000"};
  return convert_enum_idx(
      options,
      3,
      value,
      "report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b1_s_::b1_thres_c_::types");
}

const char*
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b2_s_::b2_thres2_c_::types_opts::to_string() const
{
  static const char* options[] = {"b2-Threshold2UTRA", "b2-Threshold2GERAN", "b2-Threshold2CDMA2000"};
  return convert_enum_idx(
      options,
      3,
      value,
      "report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b2_s_::b2_thres2_c_::types");
}

const char* report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "eventB1", "eventB2", "eventW1-r13", "eventW2-r13", "eventW3-r13", "eventB1-NR-r15", "eventB2-NR-r15"};
  return convert_enum_idx(options, 7, value, "report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::types");
}

const char* report_cfg_inter_rat_s::trigger_type_c_::types_opts::to_string() const
{
  static const char* options[] = {"event", "periodical"};
  return convert_enum_idx(options, 2, value, "report_cfg_inter_rat_s::trigger_type_c_::types");
}

const char* drb_to_add_mod_scg_r12_s::drb_type_r12_c_::types_opts::to_string() const
{
  static const char* options[] = {"split-r12", "scg-r12"};
  return convert_enum_idx(options, 2, value, "drb_to_add_mod_scg_r12_s::drb_type_r12_c_::types");
}

const char* ip_address_r13_c::types_opts::to_string() const
{
  static const char* options[] = {"ipv4-r13", "ipv6-r13"};
  return convert_enum_idx(options, 2, value, "ip_address_r13_c::types");
}
uint8_t ip_address_r13_c::types_opts::to_number() const
{
  static const uint8_t options[] = {4, 6};
  return map_enum_number(options, 2, value, "ip_address_r13_c::types");
}

const char* meas_obj_to_add_mod_s::meas_obj_c_::types_opts::to_string() const
{
  static const char* options[] = {"measObjectEUTRA",
                                  "measObjectUTRA",
                                  "measObjectGERAN",
                                  "measObjectCDMA2000",
                                  "measObjectWLAN-r13",
                                  "measObjectNR-r15"};
  return convert_enum_idx(options, 6, value, "meas_obj_to_add_mod_s::meas_obj_c_::types");
}
uint16_t meas_obj_to_add_mod_s::meas_obj_c_::types_opts::to_number() const
{
  if (value == meas_obj_cdma2000) {
    return 2000;
  }
  invalid_enum_number(value, "meas_obj_to_add_mod_s::meas_obj_c_::types");
  return 0;
}

const char* meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"measObjectEUTRA-r13",
                                  "measObjectUTRA-r13",
                                  "measObjectGERAN-r13",
                                  "measObjectCDMA2000-r13",
                                  "measObjectWLAN-v1320",
                                  "measObjectNR-r15"};
  return convert_enum_idx(options, 6, value, "meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_::types");
}
uint16_t meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_::types_opts::to_number() const
{
  if (value == meas_obj_cdma2000_r13) {
    return 2000;
  }
  invalid_enum_number(value, "meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_::types");
  return 0;
}

const char* report_cfg_to_add_mod_s::report_cfg_c_::types_opts::to_string() const
{
  static const char* options[] = {"reportConfigEUTRA", "reportConfigInterRAT"};
  return convert_enum_idx(options, 2, value, "report_cfg_to_add_mod_s::report_cfg_c_::types");
}

const char* security_cfg_ho_v1530_s::handov_type_v1530_c_::types_opts::to_string() const
{
  static const char* options[] = {"intra5GC-r15", "fivegc-ToEPC-r15", "epc-To5GC-r15"};
  return convert_enum_idx(options, 3, value, "security_cfg_ho_v1530_s::handov_type_v1530_c_::types");
}

const char* meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"rstd0-r15",  "rstd1-r15",  "rstd2-r15",  "rstd3-r15",  "rstd4-r15",  "rstd5-r15",
                                  "rstd6-r15",  "rstd7-r15",  "rstd8-r15",  "rstd9-r15",  "rstd10-r15", "rstd11-r15",
                                  "rstd12-r15", "rstd13-r15", "rstd14-r15", "rstd15-r15", "rstd16-r15", "rstd17-r15",
                                  "rstd18-r15", "rstd19-r15", "rstd20-r15"};
  return convert_enum_idx(
      options, 21, value, "meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::types");
}
uint8_t meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
  return map_enum_number(
      options, 21, value, "meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::types");
}

const char* rach_skip_r14_s::target_ta_r14_c_::types_opts::to_string() const
{
  static const char* options[] = {"ta0-r14", "mcg-PTAG-r14", "scg-PTAG-r14", "mcg-STAG-r14", "scg-STAG-r14"};
  return convert_enum_idx(options, 5, value, "rach_skip_r14_s::target_ta_r14_c_::types");
}
uint8_t rach_skip_r14_s::target_ta_r14_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {0};
  return map_enum_number(options, 1, value, "rach_skip_r14_s::target_ta_r14_c_::types");
}

const char* sl_disc_tx_ref_carrier_ded_r13_c::types_opts::to_string() const
{
  static const char* options[] = {"pCell", "sCell"};
  return convert_enum_idx(options, 2, value, "sl_disc_tx_ref_carrier_ded_r13_c::types");
}

const char* sl_disc_tx_res_r13_c::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"scheduled-r13", "ue-Selected-r13"};
  return convert_enum_idx(options, 2, value, "sl_disc_tx_res_r13_c::setup_c_::types");
}

const char* rclwi_cfg_r13_s::cmd_c_::types_opts::to_string() const
{
  static const char* options[] = {"steerToWLAN-r13", "steerToLTE-r13"};
  return convert_enum_idx(options, 2, value, "rclwi_cfg_r13_s::cmd_c_::types");
}

const char* sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"scheduled-r14", "ue-Selected-r14"};
  return convert_enum_idx(options, 2, value, "sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::setup_c_::types");
}

const char* sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"scheduled-v1530", "ue-Selected-v1530"};
  return convert_enum_idx(options, 2, value, "sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::setup_c_::types");
}

const char* ran_notif_area_info_r15_c::types_opts::to_string() const
{
  static const char* options[] = {"cellList-r15", "ran-AreaConfigList-r15"};
  return convert_enum_idx(options, 2, value, "ran_notif_area_info_r15_c::types");
}

const char* sl_comm_cfg_r12_s::comm_tx_res_r12_c_::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"scheduled-r12", "ue-Selected-r12"};
  return convert_enum_idx(options, 2, value, "sl_comm_cfg_r12_s::comm_tx_res_r12_c_::setup_c_::types");
}

const char* sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"scheduled-v1310", "ue-Selected-v1310"};
  return convert_enum_idx(options, 2, value, "sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::setup_c_::types");
}

const char* sl_disc_cfg_r12_s::disc_tx_res_r12_c_::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"scheduled-r12", "ue-Selected-r12"};
  return convert_enum_idx(options, 2, value, "sl_disc_cfg_r12_s::disc_tx_res_r12_c_::setup_c_::types");
}

const char* sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"scheduled-r13", "ue-Selected-r13"};
  return convert_enum_idx(options, 2, value, "sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::setup_c_::types");
}

const char* prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"fdd-r13", "tdd-r13"};
  return convert_enum_idx(options, 2, value, "prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::types");
}

const char* rrc_conn_release_v920_ies_s::cell_info_list_r9_c_::types_opts::to_string() const
{
  static const char* options[] = {"geran-r9", "utra-FDD-r9", "utra-TDD-r9", "utra-TDD-r10"};
  return convert_enum_idx(options, 4, value, "rrc_conn_release_v920_ies_s::cell_info_list_r9_c_::types");
}

const char* si_or_psi_geran_c::types_opts::to_string() const
{
  static const char* options[] = {"si", "psi"};
  return convert_enum_idx(options, 2, value, "si_or_psi_geran_c::types");
}

const char* area_cfg_r10_c::types_opts::to_string() const
{
  static const char* options[] = {"cellGlobalIdList-r10", "trackingAreaCodeList-r10"};
  return convert_enum_idx(options, 2, value, "area_cfg_r10_c::types");
}

const char* cell_change_order_s::target_rat_type_c_::types_opts::to_string() const
{
  static const char* options[] = {"geran"};
  return convert_enum_idx(options, 1, value, "cell_change_order_s::target_rat_type_c_::types");
}

const char* rn_sf_cfg_r10_s::sf_cfg_pattern_r10_c_::types_opts::to_string() const
{
  static const char* options[] = {"subframeConfigPatternFDD-r10", "subframeConfigPatternTDD-r10"};
  return convert_enum_idx(options, 2, value, "rn_sf_cfg_r10_s::sf_cfg_pattern_r10_c_::types");
}

const char* rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_::types_opts::to_string() const
{
  static const char* options[] = {"nrb6-r10", "nrb15-r10", "nrb25-r10", "nrb50-r10", "nrb75-r10", "nrb100-r10"};
  return convert_enum_idx(
      options, 6, value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_::types");
}
uint8_t rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(
      options, 6, value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_::types");
}

const char* rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_::types_opts::to_string() const
{
  static const char* options[] = {"nrb6-r10", "nrb15-r10", "nrb25-r10", "nrb50-r10", "nrb75-r10", "nrb100-r10"};
  return convert_enum_idx(
      options, 6, value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_::types");
}
uint8_t rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(
      options, 6, value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_::types");
}

const char* rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::types_opts::to_string() const
{
  static const char* options[] = {"type01-r10", "type2-r10"};
  return convert_enum_idx(options, 2, value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::types");
}
float rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::types_opts::to_number() const
{
  static const float options[] = {0.1, 2.0};
  return map_enum_number(options, 2, value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::types");
}
const char* rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::types_opts::to_number_string() const
{
  static const char* options[] = {"0.1", "2"};
  return convert_enum_idx(options, 2, value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::types");
}

const char* rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::demod_rs_r10_c_::types_opts::to_string() const
{
  static const char* options[] = {"interleaving-r10", "noInterleaving-r10"};
  return convert_enum_idx(options, 2, value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::demod_rs_r10_c_::types");
}

const char* rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_::types_opts::to_string() const
{
  static const char* options[] = {"channelSelectionMultiplexingBundling", "fallbackForFormat3"};
  return convert_enum_idx(options, 2, value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_::types");
}
uint8_t rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_::types_opts::to_number() const
{
  if (value == fallback_for_format3) {
    return 3;
  }
  invalid_enum_number(value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_::types");
  return 0;
}

const char* rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::types_opts::to_string() const
{
  static const char* options[] = {"tdd", "fdd"};
  return convert_enum_idx(options, 2, value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::types");
}

const char* redirected_carrier_info_c::types_opts::to_string() const
{
  static const char* options[] = {
      "eutra", "geran", "utra-FDD", "utra-TDD", "cdma2000-HRPD", "cdma2000-1xRTT", "utra-TDD-r10", "nr-r15"};
  return convert_enum_idx(options, 8, value, "redirected_carrier_info_c::types");
}

const char* security_cfg_ho_s::handov_type_c_::types_opts::to_string() const
{
  static const char* options[] = {"intraLTE", "interRAT"};
  return convert_enum_idx(options, 2, value, "security_cfg_ho_s::handov_type_c_::types");
}

const char* dl_info_transfer_r15_ies_s::ded_info_type_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "dedicatedInfoNAS-r15", "dedicatedInfoCDMA2000-1XRTT-r15", "dedicatedInfoCDMA2000-HRPD-r15"};
  return convert_enum_idx(options, 3, value, "dl_info_transfer_r15_ies_s::ded_info_type_r15_c_::types");
}

const char* dl_info_transfer_r8_ies_s::ded_info_type_c_::types_opts::to_string() const
{
  static const char* options[] = {"dedicatedInfoNAS", "dedicatedInfoCDMA2000-1XRTT", "dedicatedInfoCDMA2000-HRPD"};
  return convert_enum_idx(options, 3, value, "dl_info_transfer_r8_ies_s::ded_info_type_c_::types");
}

const char* mob_from_eutra_cmd_r8_ies_s::purpose_c_::types_opts::to_string() const
{
  static const char* options[] = {"handover", "cellChangeOrder"};
  return convert_enum_idx(options, 2, value, "mob_from_eutra_cmd_r8_ies_s::purpose_c_::types");
}

const char* mob_from_eutra_cmd_r9_ies_s::purpose_c_::types_opts::to_string() const
{
  static const char* options[] = {"handover", "cellChangeOrder", "e-CSFB-r9"};
  return convert_enum_idx(options, 3, value, "mob_from_eutra_cmd_r9_ies_s::purpose_c_::types");
}

const char* csfb_params_resp_cdma2000_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"csfbParametersResponseCDMA2000-r8", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "csfb_params_resp_cdma2000_s::crit_exts_c_::types");
}
uint16_t csfb_params_resp_cdma2000_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint16_t options[] = {2000};
  return map_enum_number(options, 1, value, "csfb_params_resp_cdma2000_s::crit_exts_c_::types");
}

const char* counter_check_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"counterCheck-r8", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "counter_check_s::crit_exts_c_::c1_c_::types");
}

const char* dl_info_transfer_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"dlInformationTransfer-r8", "dlInformationTransfer-r15", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "dl_info_transfer_s::crit_exts_c_::c1_c_::types");
}

const char* ho_from_eutra_prep_request_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"handoverFromEUTRAPreparationRequest-r8", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ho_from_eutra_prep_request_s::crit_exts_c_::c1_c_::types");
}

const char* logged_meas_cfg_r10_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"loggedMeasurementConfiguration-r10", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "logged_meas_cfg_r10_s::crit_exts_c_::c1_c_::types");
}

const char* mob_from_eutra_cmd_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"mobilityFromEUTRACommand-r8", "mobilityFromEUTRACommand-r9", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "mob_from_eutra_cmd_s::crit_exts_c_::c1_c_::types");
}

const char* rn_recfg_r10_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"rnReconfiguration-r10", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "rn_recfg_r10_s::crit_exts_c_::c1_c_::types");
}

const char* rrc_conn_recfg_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "rrcConnectionReconfiguration-r8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "rrc_conn_recfg_s::crit_exts_c_::c1_c_::types");
}

const char* rrc_conn_release_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionRelease-r8", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "rrc_conn_release_s::crit_exts_c_::c1_c_::types");
}

const char* rrc_conn_resume_r13_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionResume-r13", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "rrc_conn_resume_r13_s::crit_exts_c_::c1_c_::types");
}

const char* security_mode_cmd_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"securityModeCommand-r8", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "security_mode_cmd_s::crit_exts_c_::c1_c_::types");
}

const char* ue_cap_enquiry_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"ueCapabilityEnquiry-r8", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ue_cap_enquiry_s::crit_exts_c_::c1_c_::types");
}

const char* ue_info_request_r9_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"ueInformationRequest-r9", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ue_info_request_r9_s::crit_exts_c_::c1_c_::types");
}

const char* dl_dcch_msg_type_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"csfbParametersResponseCDMA2000",
                                  "dlInformationTransfer",
                                  "handoverFromEUTRAPreparationRequest",
                                  "mobilityFromEUTRACommand",
                                  "rrcConnectionReconfiguration",
                                  "rrcConnectionRelease",
                                  "securityModeCommand",
                                  "ueCapabilityEnquiry",
                                  "counterCheck",
                                  "ueInformationRequest-r9",
                                  "loggedMeasurementConfiguration-r10",
                                  "rnReconfiguration-r10",
                                  "rrcConnectionResume-r13",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "dl_dcch_msg_type_c::c1_c_::types");
}
uint16_t dl_dcch_msg_type_c::c1_c_::types_opts::to_number() const
{
  static const uint16_t options[] = {2000};
  return map_enum_number(options, 1, value, "dl_dcch_msg_type_c::c1_c_::types");
}

const char* dl_dcch_msg_type_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "dl_dcch_msg_type_c::types");
}
uint8_t dl_dcch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "dl_dcch_msg_type_c::types");
}

const char* tmgi_r9_s::plmn_id_r9_c_::types_opts::to_string() const
{
  static const char* options[] = {"plmn-Index-r9", "explicitValue-r9"};
  return convert_enum_idx(options, 2, value, "tmgi_r9_s::plmn_id_r9_c_::types");
}

// MBMS-SessionInfo-r9 ::= SEQUENCE
SRSASN_CODE mbms_session_info_r9_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(session_id_r9_present, 1));

  HANDLE_CODE(tmgi_r9.pack(bref));
  if (session_id_r9_present) {
    HANDLE_CODE(session_id_r9.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, lc_ch_id_r9, (uint8_t)0u, (uint8_t)28u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbms_session_info_r9_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(session_id_r9_present, 1));

  HANDLE_CODE(tmgi_r9.unpack(bref));
  if (session_id_r9_present) {
    HANDLE_CODE(session_id_r9.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(lc_ch_id_r9, bref, (uint8_t)0u, (uint8_t)28u));

  return SRSASN_SUCCESS;
}
void mbms_session_info_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("tmgi-r9");
  tmgi_r9.to_json(j);
  if (session_id_r9_present) {
    j.write_str("sessionId-r9", session_id_r9.to_string());
  }
  j.write_int("logicalChannelIdentity-r9", lc_ch_id_r9);
  j.end_obj();
}

// PMCH-Config-r12 ::= SEQUENCE
SRSASN_CODE pmch_cfg_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, sf_alloc_end_r12, (uint16_t)0u, (uint16_t)1535u));
  HANDLE_CODE(data_mcs_r12.pack(bref));
  HANDLE_CODE(mch_sched_period_r12.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= mch_sched_period_v1430_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(mch_sched_period_v1430_present, 1));
      if (mch_sched_period_v1430_present) {
        HANDLE_CODE(mch_sched_period_v1430.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pmch_cfg_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(sf_alloc_end_r12, bref, (uint16_t)0u, (uint16_t)1535u));
  HANDLE_CODE(data_mcs_r12.unpack(bref));
  HANDLE_CODE(mch_sched_period_r12.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(mch_sched_period_v1430_present, 1));
      if (mch_sched_period_v1430_present) {
        HANDLE_CODE(mch_sched_period_v1430.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void pmch_cfg_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sf-AllocEnd-r12", sf_alloc_end_r12);
  j.write_fieldname("dataMCS-r12");
  data_mcs_r12.to_json(j);
  j.write_str("mch-SchedulingPeriod-r12", mch_sched_period_r12.to_string());
  if (ext) {
    if (mch_sched_period_v1430_present) {
      j.write_str("mch-SchedulingPeriod-v1430", mch_sched_period_v1430.to_string());
    }
  }
  j.end_obj();
}

void pmch_cfg_r12_s::data_mcs_r12_c_::destroy_() {}
void pmch_cfg_r12_s::data_mcs_r12_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
pmch_cfg_r12_s::data_mcs_r12_c_::data_mcs_r12_c_(const pmch_cfg_r12_s::data_mcs_r12_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::normal_r12:
      c.init(other.c.get<uint8_t>());
      break;
    case types::higer_order_r12:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pmch_cfg_r12_s::data_mcs_r12_c_");
  }
}
pmch_cfg_r12_s::data_mcs_r12_c_&
pmch_cfg_r12_s::data_mcs_r12_c_::operator=(const pmch_cfg_r12_s::data_mcs_r12_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::normal_r12:
      c.set(other.c.get<uint8_t>());
      break;
    case types::higer_order_r12:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "pmch_cfg_r12_s::data_mcs_r12_c_");
  }

  return *this;
}
uint8_t& pmch_cfg_r12_s::data_mcs_r12_c_::set_normal_r12()
{
  set(types::normal_r12);
  return c.get<uint8_t>();
}
uint8_t& pmch_cfg_r12_s::data_mcs_r12_c_::set_higer_order_r12()
{
  set(types::higer_order_r12);
  return c.get<uint8_t>();
}
void pmch_cfg_r12_s::data_mcs_r12_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::normal_r12:
      j.write_int("normal-r12", c.get<uint8_t>());
      break;
    case types::higer_order_r12:
      j.write_int("higerOrder-r12", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "pmch_cfg_r12_s::data_mcs_r12_c_");
  }
  j.end_obj();
}
SRSASN_CODE pmch_cfg_r12_s::data_mcs_r12_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::normal_r12:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)28u));
      break;
    case types::higer_order_r12:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)27u));
      break;
    default:
      log_invalid_choice_id(type_, "pmch_cfg_r12_s::data_mcs_r12_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE pmch_cfg_r12_s::data_mcs_r12_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::normal_r12:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)28u));
      break;
    case types::higer_order_r12:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)27u));
      break;
    default:
      log_invalid_choice_id(type_, "pmch_cfg_r12_s::data_mcs_r12_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* pmch_cfg_r12_s::data_mcs_r12_c_::types_opts::to_string() const
{
  static const char* options[] = {"normal-r12", "higerOrder-r12"};
  return convert_enum_idx(options, 2, value, "pmch_cfg_r12_s::data_mcs_r12_c_::types");
}

const char* pmch_cfg_r12_s::mch_sched_period_r12_opts::to_string() const
{
  static const char* options[] = {"rf4", "rf8", "rf16", "rf32", "rf64", "rf128", "rf256", "rf512", "rf1024"};
  return convert_enum_idx(options, 9, value, "pmch_cfg_r12_s::mch_sched_period_r12_e_");
}
uint16_t pmch_cfg_r12_s::mch_sched_period_r12_opts::to_number() const
{
  static const uint16_t options[] = {4, 8, 16, 32, 64, 128, 256, 512, 1024};
  return map_enum_number(options, 9, value, "pmch_cfg_r12_s::mch_sched_period_r12_e_");
}

const char* pmch_cfg_r12_s::mch_sched_period_v1430_opts::to_string() const
{
  static const char* options[] = {"rf1", "rf2"};
  return convert_enum_idx(options, 2, value, "pmch_cfg_r12_s::mch_sched_period_v1430_e_");
}
uint8_t pmch_cfg_r12_s::mch_sched_period_v1430_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "pmch_cfg_r12_s::mch_sched_period_v1430_e_");
}

// PMCH-InfoExt-r12 ::= SEQUENCE
SRSASN_CODE pmch_info_ext_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pmch_cfg_r12.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, mbms_session_info_list_r12, 0, 29));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pmch_info_ext_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(pmch_cfg_r12.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(mbms_session_info_list_r12, bref, 0, 29));

  return SRSASN_SUCCESS;
}
void pmch_info_ext_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("pmch-Config-r12");
  pmch_cfg_r12.to_json(j);
  j.start_array("mbms-SessionInfoList-r12");
  for (const auto& e1 : mbms_session_info_list_r12) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// MBSFNAreaConfiguration-v1430-IEs ::= SEQUENCE
SRSASN_CODE mbsfn_area_cfg_v1430_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, common_sf_alloc_r14, 1, 8));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbsfn_area_cfg_v1430_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(common_sf_alloc_r14, bref, 1, 8));

  return SRSASN_SUCCESS;
}
void mbsfn_area_cfg_v1430_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("commonSF-Alloc-r14");
  for (const auto& e1 : common_sf_alloc_r14) {
    e1.to_json(j);
  }
  j.end_array();
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// PMCH-Config-r9 ::= SEQUENCE
SRSASN_CODE pmch_cfg_r9_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, sf_alloc_end_r9, (uint16_t)0u, (uint16_t)1535u));
  HANDLE_CODE(pack_integer(bref, data_mcs_r9, (uint8_t)0u, (uint8_t)28u));
  HANDLE_CODE(mch_sched_period_r9.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pmch_cfg_r9_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(sf_alloc_end_r9, bref, (uint16_t)0u, (uint16_t)1535u));
  HANDLE_CODE(unpack_integer(data_mcs_r9, bref, (uint8_t)0u, (uint8_t)28u));
  HANDLE_CODE(mch_sched_period_r9.unpack(bref));

  return SRSASN_SUCCESS;
}
void pmch_cfg_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sf-AllocEnd-r9", sf_alloc_end_r9);
  j.write_int("dataMCS-r9", data_mcs_r9);
  j.write_str("mch-SchedulingPeriod-r9", mch_sched_period_r9.to_string());
  j.end_obj();
}

const char* pmch_cfg_r9_s::mch_sched_period_r9_opts::to_string() const
{
  static const char* options[] = {"rf8", "rf16", "rf32", "rf64", "rf128", "rf256", "rf512", "rf1024"};
  return convert_enum_idx(options, 8, value, "pmch_cfg_r9_s::mch_sched_period_r9_e_");
}
uint16_t pmch_cfg_r9_s::mch_sched_period_r9_opts::to_number() const
{
  static const uint16_t options[] = {8, 16, 32, 64, 128, 256, 512, 1024};
  return map_enum_number(options, 8, value, "pmch_cfg_r9_s::mch_sched_period_r9_e_");
}

// CountingRequestInfo-r10 ::= SEQUENCE
SRSASN_CODE count_request_info_r10_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(tmgi_r10.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE count_request_info_r10_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(tmgi_r10.unpack(bref));

  return SRSASN_SUCCESS;
}
void count_request_info_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("tmgi-r10");
  tmgi_r10.to_json(j);
  j.end_obj();
}

// MBSFNAreaConfiguration-v1250-IEs ::= SEQUENCE
SRSASN_CODE mbsfn_area_cfg_v1250_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pmch_info_list_ext_r12_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (pmch_info_list_ext_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, pmch_info_list_ext_r12, 0, 15));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbsfn_area_cfg_v1250_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pmch_info_list_ext_r12_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (pmch_info_list_ext_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(pmch_info_list_ext_r12, bref, 0, 15));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mbsfn_area_cfg_v1250_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pmch_info_list_ext_r12_present) {
    j.start_array("pmch-InfoListExt-r12");
    for (const auto& e1 : pmch_info_list_ext_r12) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// PMCH-Info-r9 ::= SEQUENCE
SRSASN_CODE pmch_info_r9_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pmch_cfg_r9.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, mbms_session_info_list_r9, 0, 29));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pmch_info_r9_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(pmch_cfg_r9.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(mbms_session_info_list_r9, bref, 0, 29));

  return SRSASN_SUCCESS;
}
void pmch_info_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("pmch-Config-r9");
  pmch_cfg_r9.to_json(j);
  j.start_array("mbms-SessionInfoList-r9");
  for (const auto& e1 : mbms_session_info_list_r9) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// MBSFNAreaConfiguration-v930-IEs ::= SEQUENCE
SRSASN_CODE mbsfn_area_cfg_v930_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbsfn_area_cfg_v930_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mbsfn_area_cfg_v930_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// MBMSCountingRequest-r10 ::= SEQUENCE
SRSASN_CODE mbms_count_request_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, count_request_list_r10, 1, 16));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbms_count_request_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(count_request_list_r10, bref, 1, 16));
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mbms_count_request_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("countingRequestList-r10");
  for (const auto& e1 : count_request_list_r10) {
    e1.to_json(j);
  }
  j.end_array();
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// MBSFNAreaConfiguration-r9 ::= SEQUENCE
SRSASN_CODE mbsfn_area_cfg_r9_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, common_sf_alloc_r9, 1, 8));
  HANDLE_CODE(common_sf_alloc_period_r9.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, pmch_info_list_r9, 0, 15));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbsfn_area_cfg_r9_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(common_sf_alloc_r9, bref, 1, 8));
  HANDLE_CODE(common_sf_alloc_period_r9.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(pmch_info_list_r9, bref, 0, 15));
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mbsfn_area_cfg_r9_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("commonSF-Alloc-r9");
  for (const auto& e1 : common_sf_alloc_r9) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_str("commonSF-AllocPeriod-r9", common_sf_alloc_period_r9.to_string());
  j.start_array("pmch-InfoList-r9");
  for (const auto& e1 : pmch_info_list_r9) {
    e1.to_json(j);
  }
  j.end_array();
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

const char* mbsfn_area_cfg_r9_s::common_sf_alloc_period_r9_opts::to_string() const
{
  static const char* options[] = {"rf4", "rf8", "rf16", "rf32", "rf64", "rf128", "rf256"};
  return convert_enum_idx(options, 7, value, "mbsfn_area_cfg_r9_s::common_sf_alloc_period_r9_e_");
}
uint16_t mbsfn_area_cfg_r9_s::common_sf_alloc_period_r9_opts::to_number() const
{
  static const uint16_t options[] = {4, 8, 16, 32, 64, 128, 256};
  return map_enum_number(options, 7, value, "mbsfn_area_cfg_r9_s::common_sf_alloc_period_r9_e_");
}

// MCCH-MessageType ::= CHOICE
void mcch_msg_type_c::destroy_()
{
  switch (type_) {
    case types::c1:
      c.destroy<c1_c_>();
      break;
    case types::later:
      c.destroy<later_c_>();
      break;
    default:
      break;
  }
}
void mcch_msg_type_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::c1:
      c.init<c1_c_>();
      break;
    case types::later:
      c.init<later_c_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mcch_msg_type_c");
  }
}
mcch_msg_type_c::mcch_msg_type_c(const mcch_msg_type_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::c1:
      c.init(other.c.get<c1_c_>());
      break;
    case types::later:
      c.init(other.c.get<later_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mcch_msg_type_c");
  }
}
mcch_msg_type_c& mcch_msg_type_c::operator=(const mcch_msg_type_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::c1:
      c.set(other.c.get<c1_c_>());
      break;
    case types::later:
      c.set(other.c.get<later_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mcch_msg_type_c");
  }

  return *this;
}
mcch_msg_type_c::c1_c_& mcch_msg_type_c::set_c1()
{
  set(types::c1);
  return c.get<c1_c_>();
}
mcch_msg_type_c::later_c_& mcch_msg_type_c::set_later()
{
  set(types::later);
  return c.get<later_c_>();
}
void mcch_msg_type_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.get<c1_c_>().to_json(j);
      break;
    case types::later:
      j.write_fieldname("later");
      c.get<later_c_>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "mcch_msg_type_c");
  }
  j.end_obj();
}
SRSASN_CODE mcch_msg_type_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().pack(bref));
      break;
    case types::later:
      HANDLE_CODE(c.get<later_c_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mcch_msg_type_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mcch_msg_type_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().unpack(bref));
      break;
    case types::later:
      HANDLE_CODE(c.get<later_c_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mcch_msg_type_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void mcch_msg_type_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("mbsfnAreaConfiguration-r9");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE mcch_msg_type_c::c1_c_::pack(bit_ref& bref) const
{
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE mcch_msg_type_c::c1_c_::unpack(cbit_ref& bref)
{
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* mcch_msg_type_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"mbsfnAreaConfiguration-r9"};
  return convert_enum_idx(options, 1, value, "mcch_msg_type_c::c1_c_::types");
}

void mcch_msg_type_c::later_c_::set(types::options e)
{
  type_ = e;
}
mcch_msg_type_c::later_c_::c2_c_& mcch_msg_type_c::later_c_::set_c2()
{
  set(types::c2);
  return c;
}
void mcch_msg_type_c::later_c_::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
void mcch_msg_type_c::later_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c2:
      j.write_fieldname("c2");
      c.to_json(j);
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "mcch_msg_type_c::later_c_");
  }
  j.end_obj();
}
SRSASN_CODE mcch_msg_type_c::later_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c2:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "mcch_msg_type_c::later_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mcch_msg_type_c::later_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c2:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::msg_class_ext:
      break;
    default:
      log_invalid_choice_id(type_, "mcch_msg_type_c::later_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void mcch_msg_type_c::later_c_::c2_c_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("mbmsCountingRequest-r10");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE mcch_msg_type_c::later_c_::c2_c_::pack(bit_ref& bref) const
{
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE mcch_msg_type_c::later_c_::c2_c_::unpack(cbit_ref& bref)
{
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* mcch_msg_type_c::later_c_::c2_c_::types_opts::to_string() const
{
  static const char* options[] = {"mbmsCountingRequest-r10"};
  return convert_enum_idx(options, 1, value, "mcch_msg_type_c::later_c_::c2_c_::types");
}

const char* mcch_msg_type_c::later_c_::types_opts::to_string() const
{
  static const char* options[] = {"c2", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "mcch_msg_type_c::later_c_::types");
}
uint8_t mcch_msg_type_c::later_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {2};
  return map_enum_number(options, 1, value, "mcch_msg_type_c::later_c_::types");
}

const char* mcch_msg_type_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "later"};
  return convert_enum_idx(options, 2, value, "mcch_msg_type_c::types");
}
uint8_t mcch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "mcch_msg_type_c::types");
}

// MCCH-Message ::= SEQUENCE
SRSASN_CODE mcch_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE mcch_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void mcch_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("MCCH-Message");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

const char* paging_ue_id_c::types_opts::to_string() const
{
  static const char* options[] = {"s-TMSI", "imsi", "ng-5G-S-TMSI-r15", "fullI-RNTI-r15"};
  return convert_enum_idx(options, 4, value, "paging_ue_id_c::types");
}
int8_t paging_ue_id_c::types_opts::to_number() const
{
  if (value == ng_minus5_g_s_tmsi_r15) {
    return -5;
  }
  invalid_enum_number(value, "paging_ue_id_c::types");
  return 0;
}

const char* pcch_msg_type_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"paging"};
  return convert_enum_idx(options, 1, value, "pcch_msg_type_c::c1_c_::types");
}

const char* pcch_msg_type_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "pcch_msg_type_c::types");
}
uint8_t pcch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "pcch_msg_type_c::types");
}

// MBMSSessionInfo-r13 ::= SEQUENCE
SRSASN_CODE mbms_session_info_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(session_id_r13_present, 1));

  HANDLE_CODE(tmgi_r13.pack(bref));
  if (session_id_r13_present) {
    HANDLE_CODE(session_id_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbms_session_info_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(session_id_r13_present, 1));

  HANDLE_CODE(tmgi_r13.unpack(bref));
  if (session_id_r13_present) {
    HANDLE_CODE(session_id_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mbms_session_info_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("tmgi-r13");
  tmgi_r13.to_json(j);
  if (session_id_r13_present) {
    j.write_str("sessionId-r13", session_id_r13.to_string());
  }
  j.end_obj();
}

// SC-MTCH-SchedulingInfo-BR-r14 ::= SEQUENCE
SRSASN_CODE sc_mtch_sched_info_br_r14_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(on_dur_timer_scptm_r14.pack(bref));
  HANDLE_CODE(drx_inactivity_timer_scptm_r14.pack(bref));
  HANDLE_CODE(sched_period_start_offset_scptm_r14.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sc_mtch_sched_info_br_r14_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(on_dur_timer_scptm_r14.unpack(bref));
  HANDLE_CODE(drx_inactivity_timer_scptm_r14.unpack(bref));
  HANDLE_CODE(sched_period_start_offset_scptm_r14.unpack(bref));

  return SRSASN_SUCCESS;
}
void sc_mtch_sched_info_br_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("onDurationTimerSCPTM-r14", on_dur_timer_scptm_r14.to_string());
  j.write_str("drx-InactivityTimerSCPTM-r14", drx_inactivity_timer_scptm_r14.to_string());
  j.write_fieldname("schedulingPeriodStartOffsetSCPTM-r14");
  sched_period_start_offset_scptm_r14.to_json(j);
  j.end_obj();
}

const char* sc_mtch_sched_info_br_r14_s::on_dur_timer_scptm_r14_opts::to_string() const
{
  static const char* options[] = {"psf300", "psf400", "psf500", "psf600", "psf800", "psf1000", "psf1200", "psf1600"};
  return convert_enum_idx(options, 8, value, "sc_mtch_sched_info_br_r14_s::on_dur_timer_scptm_r14_e_");
}
uint16_t sc_mtch_sched_info_br_r14_s::on_dur_timer_scptm_r14_opts::to_number() const
{
  static const uint16_t options[] = {300, 400, 500, 600, 800, 1000, 1200, 1600};
  return map_enum_number(options, 8, value, "sc_mtch_sched_info_br_r14_s::on_dur_timer_scptm_r14_e_");
}

const char* sc_mtch_sched_info_br_r14_s::drx_inactivity_timer_scptm_r14_opts::to_string() const
{
  static const char* options[] = {"psf0",
                                  "psf1",
                                  "psf2",
                                  "psf4",
                                  "psf8",
                                  "psf16",
                                  "psf32",
                                  "psf64",
                                  "psf128",
                                  "psf256",
                                  "ps512",
                                  "psf1024",
                                  "psf2048",
                                  "psf4096",
                                  "psf8192",
                                  "psf16384"};
  return convert_enum_idx(options, 16, value, "sc_mtch_sched_info_br_r14_s::drx_inactivity_timer_scptm_r14_e_");
}
uint16_t sc_mtch_sched_info_br_r14_s::drx_inactivity_timer_scptm_r14_opts::to_number() const
{
  static const uint16_t options[] = {0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384};
  return map_enum_number(options, 16, value, "sc_mtch_sched_info_br_r14_s::drx_inactivity_timer_scptm_r14_e_");
}

void sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::destroy_() {}
void sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::sched_period_start_offset_scptm_r14_c_(
    const sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sf10:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf20:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf32:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf40:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf64:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf80:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf128:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf160:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf256:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf320:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf512:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf640:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf1024:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf2048:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf4096:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf8192:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_");
  }
}
sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_&
sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::operator=(
    const sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sf10:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf20:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf32:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf40:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf64:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf80:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf128:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf160:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf256:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf320:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf512:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf640:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf1024:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf2048:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf4096:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf8192:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_");
  }

  return *this;
}
uint8_t& sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf10()
{
  set(types::sf10);
  return c.get<uint8_t>();
}
uint8_t& sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf20()
{
  set(types::sf20);
  return c.get<uint8_t>();
}
uint8_t& sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf32()
{
  set(types::sf32);
  return c.get<uint8_t>();
}
uint8_t& sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf40()
{
  set(types::sf40);
  return c.get<uint8_t>();
}
uint8_t& sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf64()
{
  set(types::sf64);
  return c.get<uint8_t>();
}
uint8_t& sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf80()
{
  set(types::sf80);
  return c.get<uint8_t>();
}
uint8_t& sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf128()
{
  set(types::sf128);
  return c.get<uint8_t>();
}
uint8_t& sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf160()
{
  set(types::sf160);
  return c.get<uint8_t>();
}
uint16_t& sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf256()
{
  set(types::sf256);
  return c.get<uint16_t>();
}
uint16_t& sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf320()
{
  set(types::sf320);
  return c.get<uint16_t>();
}
uint16_t& sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf512()
{
  set(types::sf512);
  return c.get<uint16_t>();
}
uint16_t& sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf640()
{
  set(types::sf640);
  return c.get<uint16_t>();
}
uint16_t& sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf1024()
{
  set(types::sf1024);
  return c.get<uint16_t>();
}
uint16_t& sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf2048()
{
  set(types::sf2048);
  return c.get<uint16_t>();
}
uint16_t& sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf4096()
{
  set(types::sf4096);
  return c.get<uint16_t>();
}
uint16_t& sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::set_sf8192()
{
  set(types::sf8192);
  return c.get<uint16_t>();
}
void sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sf10:
      j.write_int("sf10", c.get<uint8_t>());
      break;
    case types::sf20:
      j.write_int("sf20", c.get<uint8_t>());
      break;
    case types::sf32:
      j.write_int("sf32", c.get<uint8_t>());
      break;
    case types::sf40:
      j.write_int("sf40", c.get<uint8_t>());
      break;
    case types::sf64:
      j.write_int("sf64", c.get<uint8_t>());
      break;
    case types::sf80:
      j.write_int("sf80", c.get<uint8_t>());
      break;
    case types::sf128:
      j.write_int("sf128", c.get<uint8_t>());
      break;
    case types::sf160:
      j.write_int("sf160", c.get<uint8_t>());
      break;
    case types::sf256:
      j.write_int("sf256", c.get<uint16_t>());
      break;
    case types::sf320:
      j.write_int("sf320", c.get<uint16_t>());
      break;
    case types::sf512:
      j.write_int("sf512", c.get<uint16_t>());
      break;
    case types::sf640:
      j.write_int("sf640", c.get<uint16_t>());
      break;
    case types::sf1024:
      j.write_int("sf1024", c.get<uint16_t>());
      break;
    case types::sf2048:
      j.write_int("sf2048", c.get<uint16_t>());
      break;
    case types::sf4096:
      j.write_int("sf4096", c.get<uint16_t>());
      break;
    case types::sf8192:
      j.write_int("sf8192", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sf10:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)9u));
      break;
    case types::sf20:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
      break;
    case types::sf32:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)31u));
      break;
    case types::sf40:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::sf64:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::sf80:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    case types::sf128:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    case types::sf160:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
      break;
    case types::sf256:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u));
      break;
    case types::sf320:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)319u));
      break;
    case types::sf512:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)511u));
      break;
    case types::sf640:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)639u));
      break;
    case types::sf1024:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1023u));
      break;
    case types::sf2048:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)2047u));
      break;
    case types::sf4096:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u));
      break;
    case types::sf8192:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)8191u));
      break;
    default:
      log_invalid_choice_id(type_, "sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sf10:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)9u));
      break;
    case types::sf20:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
      break;
    case types::sf32:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)31u));
      break;
    case types::sf40:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::sf64:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::sf80:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    case types::sf128:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    case types::sf160:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
      break;
    case types::sf256:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u));
      break;
    case types::sf320:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)319u));
      break;
    case types::sf512:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)511u));
      break;
    case types::sf640:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)639u));
      break;
    case types::sf1024:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1023u));
      break;
    case types::sf2048:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)2047u));
      break;
    case types::sf4096:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u));
      break;
    case types::sf8192:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)8191u));
      break;
    default:
      log_invalid_choice_id(type_, "sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::types_opts::to_string() const
{
  static const char* options[] = {"sf10",
                                  "sf20",
                                  "sf32",
                                  "sf40",
                                  "sf64",
                                  "sf80",
                                  "sf128",
                                  "sf160",
                                  "sf256",
                                  "sf320",
                                  "sf512",
                                  "sf640",
                                  "sf1024",
                                  "sf2048",
                                  "sf4096",
                                  "sf8192"};
  return convert_enum_idx(
      options, 16, value, "sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::types");
}
uint16_t sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::types_opts::to_number() const
{
  static const uint16_t options[] = {10, 20, 32, 40, 64, 80, 128, 160, 256, 320, 512, 640, 1024, 2048, 4096, 8192};
  return map_enum_number(
      options, 16, value, "sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::types");
}

// SC-MTCH-SchedulingInfo-r13 ::= SEQUENCE
SRSASN_CODE sc_mtch_sched_info_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(on_dur_timer_scptm_r13.pack(bref));
  HANDLE_CODE(drx_inactivity_timer_scptm_r13.pack(bref));
  HANDLE_CODE(sched_period_start_offset_scptm_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sc_mtch_sched_info_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(on_dur_timer_scptm_r13.unpack(bref));
  HANDLE_CODE(drx_inactivity_timer_scptm_r13.unpack(bref));
  HANDLE_CODE(sched_period_start_offset_scptm_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void sc_mtch_sched_info_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("onDurationTimerSCPTM-r13", on_dur_timer_scptm_r13.to_string());
  j.write_str("drx-InactivityTimerSCPTM-r13", drx_inactivity_timer_scptm_r13.to_string());
  j.write_fieldname("schedulingPeriodStartOffsetSCPTM-r13");
  sched_period_start_offset_scptm_r13.to_json(j);
  j.end_obj();
}

const char* sc_mtch_sched_info_r13_s::on_dur_timer_scptm_r13_opts::to_string() const
{
  static const char* options[] = {"psf1",
                                  "psf2",
                                  "psf3",
                                  "psf4",
                                  "psf5",
                                  "psf6",
                                  "psf8",
                                  "psf10",
                                  "psf20",
                                  "psf30",
                                  "psf40",
                                  "psf50",
                                  "psf60",
                                  "psf80",
                                  "psf100",
                                  "psf200"};
  return convert_enum_idx(options, 16, value, "sc_mtch_sched_info_r13_s::on_dur_timer_scptm_r13_e_");
}
uint8_t sc_mtch_sched_info_r13_s::on_dur_timer_scptm_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5, 6, 8, 10, 20, 30, 40, 50, 60, 80, 100, 200};
  return map_enum_number(options, 16, value, "sc_mtch_sched_info_r13_s::on_dur_timer_scptm_r13_e_");
}

const char* sc_mtch_sched_info_r13_s::drx_inactivity_timer_scptm_r13_opts::to_string() const
{
  static const char* options[] = {"psf0",
                                  "psf1",
                                  "psf2",
                                  "psf4",
                                  "psf8",
                                  "psf10",
                                  "psf20",
                                  "psf40",
                                  "psf80",
                                  "psf160",
                                  "ps320",
                                  "psf640",
                                  "psf960",
                                  "psf1280",
                                  "psf1920",
                                  "psf2560"};
  return convert_enum_idx(options, 16, value, "sc_mtch_sched_info_r13_s::drx_inactivity_timer_scptm_r13_e_");
}
uint16_t sc_mtch_sched_info_r13_s::drx_inactivity_timer_scptm_r13_opts::to_number() const
{
  static const uint16_t options[] = {0, 1, 2, 4, 8, 10, 20, 40, 80, 160, 320, 640, 960, 1280, 1920, 2560};
  return map_enum_number(options, 16, value, "sc_mtch_sched_info_r13_s::drx_inactivity_timer_scptm_r13_e_");
}

void sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::destroy_() {}
void sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::sched_period_start_offset_scptm_r13_c_(
    const sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sf10:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf20:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf32:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf40:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf64:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf80:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf128:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf160:
      c.init(other.c.get<uint8_t>());
      break;
    case types::sf256:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf320:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf512:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf640:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf1024:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf2048:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf4096:
      c.init(other.c.get<uint16_t>());
      break;
    case types::sf8192:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_");
  }
}
sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_&
sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::operator=(
    const sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sf10:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf20:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf32:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf40:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf64:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf80:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf128:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf160:
      c.set(other.c.get<uint8_t>());
      break;
    case types::sf256:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf320:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf512:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf640:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf1024:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf2048:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf4096:
      c.set(other.c.get<uint16_t>());
      break;
    case types::sf8192:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_");
  }

  return *this;
}
uint8_t& sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::set_sf10()
{
  set(types::sf10);
  return c.get<uint8_t>();
}
uint8_t& sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::set_sf20()
{
  set(types::sf20);
  return c.get<uint8_t>();
}
uint8_t& sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::set_sf32()
{
  set(types::sf32);
  return c.get<uint8_t>();
}
uint8_t& sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::set_sf40()
{
  set(types::sf40);
  return c.get<uint8_t>();
}
uint8_t& sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::set_sf64()
{
  set(types::sf64);
  return c.get<uint8_t>();
}
uint8_t& sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::set_sf80()
{
  set(types::sf80);
  return c.get<uint8_t>();
}
uint8_t& sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::set_sf128()
{
  set(types::sf128);
  return c.get<uint8_t>();
}
uint8_t& sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::set_sf160()
{
  set(types::sf160);
  return c.get<uint8_t>();
}
uint16_t& sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::set_sf256()
{
  set(types::sf256);
  return c.get<uint16_t>();
}
uint16_t& sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::set_sf320()
{
  set(types::sf320);
  return c.get<uint16_t>();
}
uint16_t& sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::set_sf512()
{
  set(types::sf512);
  return c.get<uint16_t>();
}
uint16_t& sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::set_sf640()
{
  set(types::sf640);
  return c.get<uint16_t>();
}
uint16_t& sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::set_sf1024()
{
  set(types::sf1024);
  return c.get<uint16_t>();
}
uint16_t& sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::set_sf2048()
{
  set(types::sf2048);
  return c.get<uint16_t>();
}
uint16_t& sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::set_sf4096()
{
  set(types::sf4096);
  return c.get<uint16_t>();
}
uint16_t& sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::set_sf8192()
{
  set(types::sf8192);
  return c.get<uint16_t>();
}
void sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sf10:
      j.write_int("sf10", c.get<uint8_t>());
      break;
    case types::sf20:
      j.write_int("sf20", c.get<uint8_t>());
      break;
    case types::sf32:
      j.write_int("sf32", c.get<uint8_t>());
      break;
    case types::sf40:
      j.write_int("sf40", c.get<uint8_t>());
      break;
    case types::sf64:
      j.write_int("sf64", c.get<uint8_t>());
      break;
    case types::sf80:
      j.write_int("sf80", c.get<uint8_t>());
      break;
    case types::sf128:
      j.write_int("sf128", c.get<uint8_t>());
      break;
    case types::sf160:
      j.write_int("sf160", c.get<uint8_t>());
      break;
    case types::sf256:
      j.write_int("sf256", c.get<uint16_t>());
      break;
    case types::sf320:
      j.write_int("sf320", c.get<uint16_t>());
      break;
    case types::sf512:
      j.write_int("sf512", c.get<uint16_t>());
      break;
    case types::sf640:
      j.write_int("sf640", c.get<uint16_t>());
      break;
    case types::sf1024:
      j.write_int("sf1024", c.get<uint16_t>());
      break;
    case types::sf2048:
      j.write_int("sf2048", c.get<uint16_t>());
      break;
    case types::sf4096:
      j.write_int("sf4096", c.get<uint16_t>());
      break;
    case types::sf8192:
      j.write_int("sf8192", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sf10:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)9u));
      break;
    case types::sf20:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)19u));
      break;
    case types::sf32:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)31u));
      break;
    case types::sf40:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)39u));
      break;
    case types::sf64:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)63u));
      break;
    case types::sf80:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)79u));
      break;
    case types::sf128:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    case types::sf160:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)159u));
      break;
    case types::sf256:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u));
      break;
    case types::sf320:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)319u));
      break;
    case types::sf512:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)511u));
      break;
    case types::sf640:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)639u));
      break;
    case types::sf1024:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1023u));
      break;
    case types::sf2048:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)2048u));
      break;
    case types::sf4096:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4096u));
      break;
    case types::sf8192:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)8192u));
      break;
    default:
      log_invalid_choice_id(type_, "sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sf10:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)9u));
      break;
    case types::sf20:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)19u));
      break;
    case types::sf32:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)31u));
      break;
    case types::sf40:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)39u));
      break;
    case types::sf64:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)63u));
      break;
    case types::sf80:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)79u));
      break;
    case types::sf128:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    case types::sf160:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)159u));
      break;
    case types::sf256:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u));
      break;
    case types::sf320:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)319u));
      break;
    case types::sf512:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)511u));
      break;
    case types::sf640:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)639u));
      break;
    case types::sf1024:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1023u));
      break;
    case types::sf2048:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)2048u));
      break;
    case types::sf4096:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4096u));
      break;
    case types::sf8192:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)8192u));
      break;
    default:
      log_invalid_choice_id(type_, "sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"sf10",
                                  "sf20",
                                  "sf32",
                                  "sf40",
                                  "sf64",
                                  "sf80",
                                  "sf128",
                                  "sf160",
                                  "sf256",
                                  "sf320",
                                  "sf512",
                                  "sf640",
                                  "sf1024",
                                  "sf2048",
                                  "sf4096",
                                  "sf8192"};
  return convert_enum_idx(
      options, 16, value, "sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::types");
}
uint16_t sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::types_opts::to_number() const
{
  static const uint16_t options[] = {10, 20, 32, 40, 64, 80, 128, 160, 256, 320, 512, 640, 1024, 2048, 4096, 8192};
  return map_enum_number(options, 16, value, "sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::types");
}

// PCI-ARFCN-r13 ::= SEQUENCE
SRSASN_CODE pci_arfcn_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(carrier_freq_r13_present, 1));

  HANDLE_CODE(pack_integer(bref, pci_r13, (uint16_t)0u, (uint16_t)503u));
  if (carrier_freq_r13_present) {
    HANDLE_CODE(pack_integer(bref, carrier_freq_r13, (uint32_t)0u, (uint32_t)262143u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pci_arfcn_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(carrier_freq_r13_present, 1));

  HANDLE_CODE(unpack_integer(pci_r13, bref, (uint16_t)0u, (uint16_t)503u));
  if (carrier_freq_r13_present) {
    HANDLE_CODE(unpack_integer(carrier_freq_r13, bref, (uint32_t)0u, (uint32_t)262143u));
  }

  return SRSASN_SUCCESS;
}
void pci_arfcn_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("physCellId-r13", pci_r13);
  if (carrier_freq_r13_present) {
    j.write_int("carrierFreq-r13", carrier_freq_r13);
  }
  j.end_obj();
}

// SC-MTCH-Info-BR-r14 ::= SEQUENCE
SRSASN_CODE sc_mtch_info_br_r14_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sc_mtch_sched_info_r14_present, 1));
  HANDLE_CODE(bref.pack(sc_mtch_neighbour_cell_r14_present, 1));
  HANDLE_CODE(bref.pack(p_a_r14_present, 1));

  HANDLE_CODE(pack_integer(bref, sc_mtch_carrier_freq_r14, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(mbms_session_info_r14.pack(bref));
  HANDLE_CODE(g_rnti_r14.pack(bref));
  if (sc_mtch_sched_info_r14_present) {
    HANDLE_CODE(sc_mtch_sched_info_r14.pack(bref));
  }
  if (sc_mtch_neighbour_cell_r14_present) {
    HANDLE_CODE(sc_mtch_neighbour_cell_r14.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, mpdcch_nb_sc_mtch_r14, (uint8_t)1u, (uint8_t)16u));
  HANDLE_CODE(mpdcch_num_repeat_sc_mtch_r14.pack(bref));
  HANDLE_CODE(mpdcch_start_sf_sc_mtch_r14.pack(bref));
  HANDLE_CODE(mpdcch_pdsch_hop_cfg_sc_mtch_r14.pack(bref));
  HANDLE_CODE(mpdcch_pdsch_cemode_cfg_sc_mtch_r14.pack(bref));
  HANDLE_CODE(mpdcch_pdsch_max_bw_sc_mtch_r14.pack(bref));
  HANDLE_CODE(mpdcch_offset_sc_mtch_r14.pack(bref));
  if (p_a_r14_present) {
    HANDLE_CODE(p_a_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sc_mtch_info_br_r14_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sc_mtch_sched_info_r14_present, 1));
  HANDLE_CODE(bref.unpack(sc_mtch_neighbour_cell_r14_present, 1));
  HANDLE_CODE(bref.unpack(p_a_r14_present, 1));

  HANDLE_CODE(unpack_integer(sc_mtch_carrier_freq_r14, bref, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(mbms_session_info_r14.unpack(bref));
  HANDLE_CODE(g_rnti_r14.unpack(bref));
  if (sc_mtch_sched_info_r14_present) {
    HANDLE_CODE(sc_mtch_sched_info_r14.unpack(bref));
  }
  if (sc_mtch_neighbour_cell_r14_present) {
    HANDLE_CODE(sc_mtch_neighbour_cell_r14.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(mpdcch_nb_sc_mtch_r14, bref, (uint8_t)1u, (uint8_t)16u));
  HANDLE_CODE(mpdcch_num_repeat_sc_mtch_r14.unpack(bref));
  HANDLE_CODE(mpdcch_start_sf_sc_mtch_r14.unpack(bref));
  HANDLE_CODE(mpdcch_pdsch_hop_cfg_sc_mtch_r14.unpack(bref));
  HANDLE_CODE(mpdcch_pdsch_cemode_cfg_sc_mtch_r14.unpack(bref));
  HANDLE_CODE(mpdcch_pdsch_max_bw_sc_mtch_r14.unpack(bref));
  HANDLE_CODE(mpdcch_offset_sc_mtch_r14.unpack(bref));
  if (p_a_r14_present) {
    HANDLE_CODE(p_a_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sc_mtch_info_br_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("sc-mtch-CarrierFreq-r14", sc_mtch_carrier_freq_r14);
  j.write_fieldname("mbmsSessionInfo-r14");
  mbms_session_info_r14.to_json(j);
  j.write_str("g-RNTI-r14", g_rnti_r14.to_string());
  if (sc_mtch_sched_info_r14_present) {
    j.write_fieldname("sc-mtch-schedulingInfo-r14");
    sc_mtch_sched_info_r14.to_json(j);
  }
  if (sc_mtch_neighbour_cell_r14_present) {
    j.write_str("sc-mtch-neighbourCell-r14", sc_mtch_neighbour_cell_r14.to_string());
  }
  j.write_int("mpdcch-Narrowband-SC-MTCH-r14", mpdcch_nb_sc_mtch_r14);
  j.write_str("mpdcch-NumRepetition-SC-MTCH-r14", mpdcch_num_repeat_sc_mtch_r14.to_string());
  j.write_fieldname("mpdcch-StartSF-SC-MTCH-r14");
  mpdcch_start_sf_sc_mtch_r14.to_json(j);
  j.write_str("mpdcch-PDSCH-HoppingConfig-SC-MTCH-r14", mpdcch_pdsch_hop_cfg_sc_mtch_r14.to_string());
  j.write_str("mpdcch-PDSCH-CEmodeConfig-SC-MTCH-r14", mpdcch_pdsch_cemode_cfg_sc_mtch_r14.to_string());
  j.write_str("mpdcch-PDSCH-MaxBandwidth-SC-MTCH-r14", mpdcch_pdsch_max_bw_sc_mtch_r14.to_string());
  j.write_str("mpdcch-Offset-SC-MTCH-r14", mpdcch_offset_sc_mtch_r14.to_string());
  if (p_a_r14_present) {
    j.write_str("p-a-r14", p_a_r14.to_string());
  }
  j.end_obj();
}

const char* sc_mtch_info_br_r14_s::mpdcch_num_repeat_sc_mtch_r14_opts::to_string() const
{
  static const char* options[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "r128", "r256"};
  return convert_enum_idx(options, 9, value, "sc_mtch_info_br_r14_s::mpdcch_num_repeat_sc_mtch_r14_e_");
}
uint16_t sc_mtch_info_br_r14_s::mpdcch_num_repeat_sc_mtch_r14_opts::to_number() const
{
  static const uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
  return map_enum_number(options, 9, value, "sc_mtch_info_br_r14_s::mpdcch_num_repeat_sc_mtch_r14_e_");
}

void sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::destroy_() {}
void sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::mpdcch_start_sf_sc_mtch_r14_c_(
    const sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::fdd_r14:
      c.init(other.c.get<fdd_r14_e_>());
      break;
    case types::tdd_r14:
      c.init(other.c.get<tdd_r14_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_");
  }
}
sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_& sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::operator=(
    const sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::fdd_r14:
      c.set(other.c.get<fdd_r14_e_>());
      break;
    case types::tdd_r14:
      c.set(other.c.get<tdd_r14_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_");
  }

  return *this;
}
sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::fdd_r14_e_&
sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::set_fdd_r14()
{
  set(types::fdd_r14);
  return c.get<fdd_r14_e_>();
}
sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::tdd_r14_e_&
sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::set_tdd_r14()
{
  set(types::tdd_r14);
  return c.get<tdd_r14_e_>();
}
void sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::fdd_r14:
      j.write_str("fdd-r14", c.get<fdd_r14_e_>().to_string());
      break;
    case types::tdd_r14:
      j.write_str("tdd-r14", c.get<tdd_r14_e_>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_");
  }
  j.end_obj();
}
SRSASN_CODE sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::fdd_r14:
      HANDLE_CODE(c.get<fdd_r14_e_>().pack(bref));
      break;
    case types::tdd_r14:
      HANDLE_CODE(c.get<tdd_r14_e_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::fdd_r14:
      HANDLE_CODE(c.get<fdd_r14_e_>().unpack(bref));
      break;
    case types::tdd_r14:
      HANDLE_CODE(c.get<tdd_r14_e_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::fdd_r14_opts::to_string() const
{
  static const char* options[] = {"v1", "v1dot5", "v2", "v2dot5", "v4", "v5", "v8", "v10"};
  return convert_enum_idx(options, 8, value, "sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::fdd_r14_e_");
}
float sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::fdd_r14_opts::to_number() const
{
  static const float options[] = {1.0, 1.5, 2.0, 2.5, 4.0, 5.0, 8.0, 10.0};
  return map_enum_number(options, 8, value, "sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::fdd_r14_e_");
}
const char* sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::fdd_r14_opts::to_number_string() const
{
  static const char* options[] = {"1", "1.5", "2", "2.5", "4", "5", "8", "10"};
  return convert_enum_idx(options, 8, value, "sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::fdd_r14_e_");
}

const char* sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::tdd_r14_opts::to_string() const
{
  static const char* options[] = {"v1", "v2", "v4", "v5", "v8", "v10", "v20"};
  return convert_enum_idx(options, 7, value, "sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::tdd_r14_e_");
}
uint8_t sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::tdd_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 5, 8, 10, 20};
  return map_enum_number(options, 7, value, "sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::tdd_r14_e_");
}

const char* sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::types_opts::to_string() const
{
  static const char* options[] = {"fdd-r14", "tdd-r14"};
  return convert_enum_idx(options, 2, value, "sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::types");
}

const char* sc_mtch_info_br_r14_s::mpdcch_pdsch_hop_cfg_sc_mtch_r14_opts::to_string() const
{
  static const char* options[] = {"on", "off"};
  return convert_enum_idx(options, 2, value, "sc_mtch_info_br_r14_s::mpdcch_pdsch_hop_cfg_sc_mtch_r14_e_");
}

const char* sc_mtch_info_br_r14_s::mpdcch_pdsch_cemode_cfg_sc_mtch_r14_opts::to_string() const
{
  static const char* options[] = {"ce-ModeA", "ce-ModeB"};
  return convert_enum_idx(options, 2, value, "sc_mtch_info_br_r14_s::mpdcch_pdsch_cemode_cfg_sc_mtch_r14_e_");
}

const char* sc_mtch_info_br_r14_s::mpdcch_pdsch_max_bw_sc_mtch_r14_opts::to_string() const
{
  static const char* options[] = {"bw1dot4", "bw5"};
  return convert_enum_idx(options, 2, value, "sc_mtch_info_br_r14_s::mpdcch_pdsch_max_bw_sc_mtch_r14_e_");
}
float sc_mtch_info_br_r14_s::mpdcch_pdsch_max_bw_sc_mtch_r14_opts::to_number() const
{
  static const float options[] = {1.4, 5.0};
  return map_enum_number(options, 2, value, "sc_mtch_info_br_r14_s::mpdcch_pdsch_max_bw_sc_mtch_r14_e_");
}
const char* sc_mtch_info_br_r14_s::mpdcch_pdsch_max_bw_sc_mtch_r14_opts::to_number_string() const
{
  static const char* options[] = {"1.4", "5"};
  return convert_enum_idx(options, 2, value, "sc_mtch_info_br_r14_s::mpdcch_pdsch_max_bw_sc_mtch_r14_e_");
}

const char* sc_mtch_info_br_r14_s::mpdcch_offset_sc_mtch_r14_opts::to_string() const
{
  static const char* options[] = {
      "zero", "oneEighth", "oneQuarter", "threeEighth", "oneHalf", "fiveEighth", "threeQuarter", "sevenEighth"};
  return convert_enum_idx(options, 8, value, "sc_mtch_info_br_r14_s::mpdcch_offset_sc_mtch_r14_e_");
}
float sc_mtch_info_br_r14_s::mpdcch_offset_sc_mtch_r14_opts::to_number() const
{
  static const float options[] = {0.0, 0.125, 0.25, 0.375, 0.5, 0.625, 0.75, 0.875};
  return map_enum_number(options, 8, value, "sc_mtch_info_br_r14_s::mpdcch_offset_sc_mtch_r14_e_");
}
const char* sc_mtch_info_br_r14_s::mpdcch_offset_sc_mtch_r14_opts::to_number_string() const
{
  static const char* options[] = {"0", "1/8", "1/4", "3/8", "1/2", "5/8", "3/4", "7/8"};
  return convert_enum_idx(options, 8, value, "sc_mtch_info_br_r14_s::mpdcch_offset_sc_mtch_r14_e_");
}

const char* sc_mtch_info_br_r14_s::p_a_r14_opts::to_string() const
{
  static const char* options[] = {"dB-6", "dB-4dot77", "dB-3", "dB-1dot77", "dB0", "dB1", "dB2", "dB3"};
  return convert_enum_idx(options, 8, value, "sc_mtch_info_br_r14_s::p_a_r14_e_");
}
float sc_mtch_info_br_r14_s::p_a_r14_opts::to_number() const
{
  static const float options[] = {-6.0, -4.77, -3.0, -1.77, 0.0, 1.0, 2.0, 3.0};
  return map_enum_number(options, 8, value, "sc_mtch_info_br_r14_s::p_a_r14_e_");
}
const char* sc_mtch_info_br_r14_s::p_a_r14_opts::to_number_string() const
{
  static const char* options[] = {"-6", "-4.77", "-3", "-1.77", "0", "1", "2", "3"};
  return convert_enum_idx(options, 8, value, "sc_mtch_info_br_r14_s::p_a_r14_e_");
}

// SC-MTCH-Info-r13 ::= SEQUENCE
SRSASN_CODE sc_mtch_info_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sc_mtch_sched_info_r13_present, 1));
  HANDLE_CODE(bref.pack(sc_mtch_neighbour_cell_r13_present, 1));

  HANDLE_CODE(mbms_session_info_r13.pack(bref));
  HANDLE_CODE(g_rnti_r13.pack(bref));
  if (sc_mtch_sched_info_r13_present) {
    HANDLE_CODE(sc_mtch_sched_info_r13.pack(bref));
  }
  if (sc_mtch_neighbour_cell_r13_present) {
    HANDLE_CODE(sc_mtch_neighbour_cell_r13.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= p_a_r13_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(p_a_r13_present, 1));
      if (p_a_r13_present) {
        HANDLE_CODE(p_a_r13.pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sc_mtch_info_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sc_mtch_sched_info_r13_present, 1));
  HANDLE_CODE(bref.unpack(sc_mtch_neighbour_cell_r13_present, 1));

  HANDLE_CODE(mbms_session_info_r13.unpack(bref));
  HANDLE_CODE(g_rnti_r13.unpack(bref));
  if (sc_mtch_sched_info_r13_present) {
    HANDLE_CODE(sc_mtch_sched_info_r13.unpack(bref));
  }
  if (sc_mtch_neighbour_cell_r13_present) {
    HANDLE_CODE(sc_mtch_neighbour_cell_r13.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(p_a_r13_present, 1));
      if (p_a_r13_present) {
        HANDLE_CODE(p_a_r13.unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sc_mtch_info_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("mbmsSessionInfo-r13");
  mbms_session_info_r13.to_json(j);
  j.write_str("g-RNTI-r13", g_rnti_r13.to_string());
  if (sc_mtch_sched_info_r13_present) {
    j.write_fieldname("sc-mtch-schedulingInfo-r13");
    sc_mtch_sched_info_r13.to_json(j);
  }
  if (sc_mtch_neighbour_cell_r13_present) {
    j.write_str("sc-mtch-neighbourCell-r13", sc_mtch_neighbour_cell_r13.to_string());
  }
  if (ext) {
    if (p_a_r13_present) {
      j.write_str("p-a-r13", p_a_r13.to_string());
    }
  }
  j.end_obj();
}

const char* sc_mtch_info_r13_s::p_a_r13_opts::to_string() const
{
  static const char* options[] = {"dB-6", "dB-4dot77", "dB-3", "dB-1dot77", "dB0", "dB1", "dB2", "dB3"};
  return convert_enum_idx(options, 8, value, "sc_mtch_info_r13_s::p_a_r13_e_");
}
float sc_mtch_info_r13_s::p_a_r13_opts::to_number() const
{
  static const float options[] = {-6.0, -4.77, -3.0, -1.77, 0.0, 1.0, 2.0, 3.0};
  return map_enum_number(options, 8, value, "sc_mtch_info_r13_s::p_a_r13_e_");
}
const char* sc_mtch_info_r13_s::p_a_r13_opts::to_number_string() const
{
  static const char* options[] = {"-6", "-4.77", "-3", "-1.77", "0", "1", "2", "3"};
  return convert_enum_idx(options, 8, value, "sc_mtch_info_r13_s::p_a_r13_e_");
}

// SCPTMConfiguration-v1340 ::= SEQUENCE
SRSASN_CODE scptm_cfg_v1340_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(p_b_r13_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (p_b_r13_present) {
    HANDLE_CODE(pack_integer(bref, p_b_r13, (uint8_t)0u, (uint8_t)3u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scptm_cfg_v1340_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(p_b_r13_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (p_b_r13_present) {
    HANDLE_CODE(unpack_integer(p_b_r13, bref, (uint8_t)0u, (uint8_t)3u));
  }

  return SRSASN_SUCCESS;
}
void scptm_cfg_v1340_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (p_b_r13_present) {
    j.write_int("p-b-r13", p_b_r13);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// SCPTMConfiguration-BR-r14 ::= SEQUENCE
SRSASN_CODE scptm_cfg_br_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(scptm_neighbour_cell_list_r14_present, 1));
  HANDLE_CODE(bref.pack(p_b_r14_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, sc_mtch_info_list_r14, 0, 128));
  if (scptm_neighbour_cell_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scptm_neighbour_cell_list_r14, 1, 8));
  }
  if (p_b_r14_present) {
    HANDLE_CODE(pack_integer(bref, p_b_r14, (uint8_t)0u, (uint8_t)3u));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scptm_cfg_br_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(scptm_neighbour_cell_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(p_b_r14_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(sc_mtch_info_list_r14, bref, 0, 128));
  if (scptm_neighbour_cell_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scptm_neighbour_cell_list_r14, bref, 1, 8));
  }
  if (p_b_r14_present) {
    HANDLE_CODE(unpack_integer(p_b_r14, bref, (uint8_t)0u, (uint8_t)3u));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void scptm_cfg_br_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("sc-mtch-InfoList-r14");
  for (const auto& e1 : sc_mtch_info_list_r14) {
    e1.to_json(j);
  }
  j.end_array();
  if (scptm_neighbour_cell_list_r14_present) {
    j.start_array("scptm-NeighbourCellList-r14");
    for (const auto& e1 : scptm_neighbour_cell_list_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (p_b_r14_present) {
    j.write_int("p-b-r14", p_b_r14);
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// SCPTMConfiguration-r13 ::= SEQUENCE
SRSASN_CODE scptm_cfg_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(scptm_neighbour_cell_list_r13_present, 1));
  HANDLE_CODE(bref.pack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, sc_mtch_info_list_r13, 0, 1023));
  if (scptm_neighbour_cell_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scptm_neighbour_cell_list_r13, 1, 8));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scptm_cfg_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(scptm_neighbour_cell_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(late_non_crit_ext_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(sc_mtch_info_list_r13, bref, 0, 1023));
  if (scptm_neighbour_cell_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scptm_neighbour_cell_list_r13, bref, 1, 8));
  }
  if (late_non_crit_ext_present) {
    HANDLE_CODE(late_non_crit_ext.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void scptm_cfg_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("sc-mtch-InfoList-r13");
  for (const auto& e1 : sc_mtch_info_list_r13) {
    e1.to_json(j);
  }
  j.end_array();
  if (scptm_neighbour_cell_list_r13_present) {
    j.start_array("scptm-NeighbourCellList-r13");
    for (const auto& e1 : scptm_neighbour_cell_list_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (late_non_crit_ext_present) {
    j.write_str("lateNonCriticalExtension", late_non_crit_ext.to_string());
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SC-MCCH-MessageType-r13 ::= CHOICE
void sc_mcch_msg_type_r13_c::destroy_()
{
  switch (type_) {
    case types::c1:
      c.destroy<c1_c_>();
      break;
    case types::msg_class_ext:
      c.destroy<msg_class_ext_c_>();
      break;
    default:
      break;
  }
}
void sc_mcch_msg_type_r13_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::c1:
      c.init<c1_c_>();
      break;
    case types::msg_class_ext:
      c.init<msg_class_ext_c_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mcch_msg_type_r13_c");
  }
}
sc_mcch_msg_type_r13_c::sc_mcch_msg_type_r13_c(const sc_mcch_msg_type_r13_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::c1:
      c.init(other.c.get<c1_c_>());
      break;
    case types::msg_class_ext:
      c.init(other.c.get<msg_class_ext_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mcch_msg_type_r13_c");
  }
}
sc_mcch_msg_type_r13_c& sc_mcch_msg_type_r13_c::operator=(const sc_mcch_msg_type_r13_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::c1:
      c.set(other.c.get<c1_c_>());
      break;
    case types::msg_class_ext:
      c.set(other.c.get<msg_class_ext_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mcch_msg_type_r13_c");
  }

  return *this;
}
sc_mcch_msg_type_r13_c::c1_c_& sc_mcch_msg_type_r13_c::set_c1()
{
  set(types::c1);
  return c.get<c1_c_>();
}
sc_mcch_msg_type_r13_c::msg_class_ext_c_& sc_mcch_msg_type_r13_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
  return c.get<msg_class_ext_c_>();
}
void sc_mcch_msg_type_r13_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.get<c1_c_>().to_json(j);
      break;
    case types::msg_class_ext:
      j.write_fieldname("messageClassExtension");
      c.get<msg_class_ext_c_>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "sc_mcch_msg_type_r13_c");
  }
  j.end_obj();
}
SRSASN_CODE sc_mcch_msg_type_r13_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().pack(bref));
      break;
    case types::msg_class_ext:
      HANDLE_CODE(c.get<msg_class_ext_c_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sc_mcch_msg_type_r13_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sc_mcch_msg_type_r13_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().unpack(bref));
      break;
    case types::msg_class_ext:
      HANDLE_CODE(c.get<msg_class_ext_c_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "sc_mcch_msg_type_r13_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void sc_mcch_msg_type_r13_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("scptmConfiguration-r13");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE sc_mcch_msg_type_r13_c::c1_c_::pack(bit_ref& bref) const
{
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE sc_mcch_msg_type_r13_c::c1_c_::unpack(cbit_ref& bref)
{
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* sc_mcch_msg_type_r13_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"scptmConfiguration-r13"};
  return convert_enum_idx(options, 1, value, "sc_mcch_msg_type_r13_c::c1_c_::types");
}

void sc_mcch_msg_type_r13_c::msg_class_ext_c_::set(types::options e)
{
  type_ = e;
}
sc_mcch_msg_type_r13_c::msg_class_ext_c_::c2_c_& sc_mcch_msg_type_r13_c::msg_class_ext_c_::set_c2()
{
  set(types::c2);
  return c;
}
void sc_mcch_msg_type_r13_c::msg_class_ext_c_::set_msg_class_ext_future_r14()
{
  set(types::msg_class_ext_future_r14);
}
void sc_mcch_msg_type_r13_c::msg_class_ext_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c2:
      j.write_fieldname("c2");
      c.to_json(j);
      break;
    case types::msg_class_ext_future_r14:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mcch_msg_type_r13_c::msg_class_ext_c_");
  }
  j.end_obj();
}
SRSASN_CODE sc_mcch_msg_type_r13_c::msg_class_ext_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c2:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::msg_class_ext_future_r14:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mcch_msg_type_r13_c::msg_class_ext_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sc_mcch_msg_type_r13_c::msg_class_ext_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c2:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::msg_class_ext_future_r14:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mcch_msg_type_r13_c::msg_class_ext_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void sc_mcch_msg_type_r13_c::msg_class_ext_c_::c2_c_::set(types::options e)
{
  type_ = e;
}
scptm_cfg_br_r14_s& sc_mcch_msg_type_r13_c::msg_class_ext_c_::c2_c_::set_scptm_cfg_br_r14()
{
  set(types::scptm_cfg_br_r14);
  return c;
}
void sc_mcch_msg_type_r13_c::msg_class_ext_c_::c2_c_::set_spare()
{
  set(types::spare);
}
void sc_mcch_msg_type_r13_c::msg_class_ext_c_::c2_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::scptm_cfg_br_r14:
      j.write_fieldname("scptmConfiguration-BR-r14");
      c.to_json(j);
      break;
    case types::spare:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mcch_msg_type_r13_c::msg_class_ext_c_::c2_c_");
  }
  j.end_obj();
}
SRSASN_CODE sc_mcch_msg_type_r13_c::msg_class_ext_c_::c2_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::scptm_cfg_br_r14:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mcch_msg_type_r13_c::msg_class_ext_c_::c2_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sc_mcch_msg_type_r13_c::msg_class_ext_c_::c2_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::scptm_cfg_br_r14:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare:
      break;
    default:
      log_invalid_choice_id(type_, "sc_mcch_msg_type_r13_c::msg_class_ext_c_::c2_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* sc_mcch_msg_type_r13_c::msg_class_ext_c_::c2_c_::types_opts::to_string() const
{
  static const char* options[] = {"scptmConfiguration-BR-r14", "spare"};
  return convert_enum_idx(options, 2, value, "sc_mcch_msg_type_r13_c::msg_class_ext_c_::c2_c_::types");
}

const char* sc_mcch_msg_type_r13_c::msg_class_ext_c_::types_opts::to_string() const
{
  static const char* options[] = {"c2", "messageClassExtensionFuture-r14"};
  return convert_enum_idx(options, 2, value, "sc_mcch_msg_type_r13_c::msg_class_ext_c_::types");
}
uint8_t sc_mcch_msg_type_r13_c::msg_class_ext_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {2};
  return map_enum_number(options, 1, value, "sc_mcch_msg_type_r13_c::msg_class_ext_c_::types");
}

const char* sc_mcch_msg_type_r13_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "sc_mcch_msg_type_r13_c::types");
}
uint8_t sc_mcch_msg_type_r13_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "sc_mcch_msg_type_r13_c::types");
}

// SC-MCCH-Message-r13 ::= SEQUENCE
SRSASN_CODE sc_mcch_msg_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE sc_mcch_msg_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void sc_mcch_msg_r13_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("SC-MCCH-Message-r13");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

const char* init_ue_id_c::types_opts::to_string() const
{
  static const char* options[] = {"s-TMSI", "randomValue"};
  return convert_enum_idx(options, 2, value, "init_ue_id_c::types");
}

const char* init_ue_id_minus5_gc_c::types_opts::to_string() const
{
  static const char* options[] = {"ng-5G-S-TMSI-Part1", "randomValue"};
  return convert_enum_idx(options, 2, value, "init_ue_id_minus5_gc_c::types");
}
int8_t init_ue_id_minus5_gc_c::types_opts::to_number() const
{
  static const int8_t options[] = {-5};
  return map_enum_number(options, 1, value, "init_ue_id_minus5_gc_c::types");
}

const char* rrc_conn_resume_request_minus5_gc_r15_ies_s::resume_id_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"fullI-RNTI-r15", "shortI-RNTI-r15"};
  return convert_enum_idx(options, 2, value, "rrc_conn_resume_request_minus5_gc_r15_ies_s::resume_id_r15_c_::types");
}

const char* rrc_conn_resume_request_r13_ies_s::resume_id_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"resumeID-r13", "truncatedResumeID-r13"};
  return convert_enum_idx(options, 2, value, "rrc_conn_resume_request_r13_ies_s::resume_id_r13_c_::types");
}

const char* rrc_conn_reest_request_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionReestablishmentRequest-r8", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_conn_reest_request_s::crit_exts_c_::types");
}

const char* rrc_conn_request_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionRequest-r8", "rrcConnectionRequest-r15"};
  return convert_enum_idx(options, 2, value, "rrc_conn_request_s::crit_exts_c_::types");
}

const char* rrc_conn_resume_request_r13_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionResumeRequest-r13", "rrcConnectionResumeRequest-r15"};
  return convert_enum_idx(options, 2, value, "rrc_conn_resume_request_r13_s::crit_exts_c_::types");
}

const char* rrc_early_data_request_r15_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcEarlyDataRequest-r15", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_early_data_request_r15_s::crit_exts_c_::types");
}

const char* ul_ccch_msg_type_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionReestablishmentRequest", "rrcConnectionRequest"};
  return convert_enum_idx(options, 2, value, "ul_ccch_msg_type_c::c1_c_::types");
}

const char* ul_ccch_msg_type_c::msg_class_ext_c_::c2_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionResumeRequest-r13"};
  return convert_enum_idx(options, 1, value, "ul_ccch_msg_type_c::msg_class_ext_c_::c2_c_::types");
}

const char* ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::c3_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcEarlyDataRequest-r15", "spare3", "spare2", "spare1"};
  return convert_enum_idx(
      options, 4, value, "ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::c3_c_::types");
}

const char* ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"c3", "messageClassExtensionFuture-r15"};
  return convert_enum_idx(
      options, 2, value, "ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::types");
}
uint8_t ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {3};
  return map_enum_number(options, 1, value, "ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::types");
}

const char* ul_ccch_msg_type_c::msg_class_ext_c_::types_opts::to_string() const
{
  static const char* options[] = {"c2", "messageClassExtensionFuture-r13"};
  return convert_enum_idx(options, 2, value, "ul_ccch_msg_type_c::msg_class_ext_c_::types");
}
uint8_t ul_ccch_msg_type_c::msg_class_ext_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {2};
  return map_enum_number(options, 1, value, "ul_ccch_msg_type_c::msg_class_ext_c_::types");
}

const char* ul_ccch_msg_type_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "ul_ccch_msg_type_c::types");
}
uint8_t ul_ccch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "ul_ccch_msg_type_c::types");
}

const char* cell_global_id_cdma2000_c::types_opts::to_string() const
{
  static const char* options[] = {"cellGlobalId1XRTT", "cellGlobalIdHRPD"};
  return convert_enum_idx(options, 2, value, "cell_global_id_cdma2000_c::types");
}
uint8_t cell_global_id_cdma2000_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "cell_global_id_cdma2000_c::types");
}

const char* meas_result_utra_s::pci_c_::types_opts::to_string() const
{
  static const char* options[] = {"fdd", "tdd"};
  return convert_enum_idx(options, 2, value, "meas_result_utra_s::pci_c_::types");
}

const char* s_nssai_r15_c::types_opts::to_string() const
{
  static const char* options[] = {"sst", "sst-SD"};
  return convert_enum_idx(options, 2, value, "s_nssai_r15_c::types");
}

const char* location_info_r10_s::location_coordinates_r10_c_::types_opts::to_string() const
{
  static const char* options[] = {"ellipsoid-Point-r10",
                                  "ellipsoidPointWithAltitude-r10",
                                  "ellipsoidPointWithUncertaintyCircle-r11",
                                  "ellipsoidPointWithUncertaintyEllipse-r11",
                                  "ellipsoidPointWithAltitudeAndUncertaintyEllipsoid-r11",
                                  "ellipsoidArc-r11",
                                  "polygon-r11"};
  return convert_enum_idx(options, 7, value, "location_info_r10_s::location_coordinates_r10_c_::types");
}

const char* location_info_r10_s::vertical_velocity_info_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"verticalVelocity-r15", "verticalVelocityAndUncertainty-r15"};
  return convert_enum_idx(options, 2, value, "location_info_r10_s::vertical_velocity_info_r15_c_::types");
}

const char* rrc_conn_setup_complete_v1530_ies_s::ng_minus5_g_s_tmsi_bits_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"ng-5G-S-TMSI-r15", "ng-5G-S-TMSI-Part2-r15"};
  return convert_enum_idx(
      options, 2, value, "rrc_conn_setup_complete_v1530_ies_s::ng_minus5_g_s_tmsi_bits_r15_c_::types");
}

const char* meas_result_idle_r15_s::meas_result_neigh_cells_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"measResultIdleListEUTRA-r15"};
  return convert_enum_idx(options, 1, value, "meas_result_idle_r15_s::meas_result_neigh_cells_r15_c_::types");
}

const char* visited_cell_info_r12_s::visited_cell_id_r12_c_::types_opts::to_string() const
{
  static const char* options[] = {"cellGlobalId-r12", "pci-arfcn-r12"};
  return convert_enum_idx(options, 2, value, "visited_cell_info_r12_s::visited_cell_id_r12_c_::types");
}

const char* idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_::types_opts::to_string() const
{
  static const char* options[] = {"subframeConfig0-r11", "subframeConfig1-5-r11", "subframeConfig6-r11"};
  return convert_enum_idx(options, 3, value, "idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_::types");
}
uint8_t idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 6};
  return map_enum_number(options, 3, value, "idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_::types");
}

const char* idc_sf_pattern_r11_c::types_opts::to_string() const
{
  static const char* options[] = {"subframePatternFDD-r11", "subframePatternTDD-r11"};
  return convert_enum_idx(options, 2, value, "idc_sf_pattern_r11_c::types");
}

const char* delay_budget_report_r14_c::types_opts::to_string() const
{
  static const char* options[] = {"type1", "type2"};
  return convert_enum_idx(options, 2, value, "delay_budget_report_r14_c::types");
}
uint8_t delay_budget_report_r14_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "delay_budget_report_r14_c::types");
}

const char* rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"rstd0-r15",  "rstd1-r15",  "rstd2-r15",  "rstd3-r15",  "rstd4-r15",  "rstd5-r15",
                                  "rstd6-r15",  "rstd7-r15",  "rstd8-r15",  "rstd9-r15",  "rstd10-r15", "rstd11-r15",
                                  "rstd12-r15", "rstd13-r15", "rstd14-r15", "rstd15-r15", "rstd16-r15", "rstd17-r15",
                                  "rstd18-r15", "rstd19-r15", "rstd20-r15"};
  return convert_enum_idx(options, 21, value, "rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::types");
}
uint8_t rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
  return map_enum_number(options, 21, value, "rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::types");
}

const char* meas_results_s::meas_result_neigh_cells_c_::types_opts::to_string() const
{
  static const char* options[] = {"measResultListEUTRA",
                                  "measResultListUTRA",
                                  "measResultListGERAN",
                                  "measResultsCDMA2000",
                                  "measResultNeighCellListNR-r15"};
  return convert_enum_idx(options, 5, value, "meas_results_s::meas_result_neigh_cells_c_::types");
}
uint16_t meas_results_s::meas_result_neigh_cells_c_::types_opts::to_number() const
{
  if (value == meas_results_cdma2000) {
    return 2000;
  }
  invalid_enum_number(value, "meas_results_s::meas_result_neigh_cells_c_::types");
  return 0;
}

const char* rlf_report_r9_s::failed_pcell_id_r10_c_::types_opts::to_string() const
{
  static const char* options[] = {"cellGlobalId-r10", "pci-arfcn-r10"};
  return convert_enum_idx(options, 2, value, "rlf_report_r9_s::failed_pcell_id_r10_c_::types");
}

const char* rlf_report_r9_s::prev_utra_cell_id_r11_s_::pci_r11_c_::types_opts::to_string() const
{
  static const char* options[] = {"fdd-r11", "tdd-r11"};
  return convert_enum_idx(options, 2, value, "rlf_report_r9_s::prev_utra_cell_id_r11_s_::pci_r11_c_::types");
}

const char* rlf_report_r9_s::sel_utra_cell_id_r11_s_::pci_r11_c_::types_opts::to_string() const
{
  static const char* options[] = {"fdd-r11", "tdd-r11"};
  return convert_enum_idx(options, 2, value, "rlf_report_r9_s::sel_utra_cell_id_r11_s_::pci_r11_c_::types");
}

const char* tdm_assist_info_r11_c::types_opts::to_string() const
{
  static const char* options[] = {"drx-AssistanceInfo-r11", "idc-SubframePatternList-r11"};
  return convert_enum_idx(options, 2, value, "tdm_assist_info_r11_c::types");
}

const char* inter_freq_rstd_meas_ind_r10_ies_s::rstd_inter_freq_ind_r10_c_::types_opts::to_string() const
{
  static const char* options[] = {"start", "stop"};
  return convert_enum_idx(options, 2, value, "inter_freq_rstd_meas_ind_r10_ies_s::rstd_inter_freq_ind_r10_c_::types");
}

const char* proximity_ind_r9_ies_s::carrier_freq_r9_c_::types_opts::to_string() const
{
  static const char* options[] = {"eutra-r9", "utra-r9", "eutra2-v9e0"};
  return convert_enum_idx(options, 3, value, "proximity_ind_r9_ies_s::carrier_freq_r9_c_::types");
}
uint8_t proximity_ind_r9_ies_s::carrier_freq_r9_c_::types_opts::to_number() const
{
  if (value == eutra2_v9e0) {
    return 2;
  }
  invalid_enum_number(value, "proximity_ind_r9_ies_s::carrier_freq_r9_c_::types");
  return 0;
}

const char* ul_info_transfer_r8_ies_s::ded_info_type_c_::types_opts::to_string() const
{
  static const char* options[] = {"dedicatedInfoNAS", "dedicatedInfoCDMA2000-1XRTT", "dedicatedInfoCDMA2000-HRPD"};
  return convert_enum_idx(options, 3, value, "ul_info_transfer_r8_ies_s::ded_info_type_c_::types");
}

const char* csfb_params_request_cdma2000_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"csfbParametersRequestCDMA2000-r8", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "csfb_params_request_cdma2000_s::crit_exts_c_::types");
}
uint16_t csfb_params_request_cdma2000_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint16_t options[] = {2000};
  return map_enum_number(options, 1, value, "csfb_params_request_cdma2000_s::crit_exts_c_::types");
}

const char* counter_check_resp_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"counterCheckResponse-r8", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "counter_check_resp_s::crit_exts_c_::types");
}

const char* in_dev_coex_ind_r11_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"inDeviceCoexIndication-r11", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "in_dev_coex_ind_r11_s::crit_exts_c_::c1_c_::types");
}

const char* inter_freq_rstd_meas_ind_r10_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"interFreqRSTDMeasurementIndication-r10", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "inter_freq_rstd_meas_ind_r10_s::crit_exts_c_::c1_c_::types");
}

const char* mbms_count_resp_r10_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"countingResponse-r10", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "mbms_count_resp_r10_s::crit_exts_c_::c1_c_::types");
}

const char* mbms_interest_ind_r11_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"interestIndication-r11", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "mbms_interest_ind_r11_s::crit_exts_c_::c1_c_::types");
}

const char* meas_report_app_layer_r15_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"measReportAppLayer-r15", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "meas_report_app_layer_r15_s::crit_exts_c_::types");
}

const char* meas_report_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "measurementReport-r8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "meas_report_s::crit_exts_c_::c1_c_::types");
}

const char* proximity_ind_r9_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"proximityIndication-r9", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "proximity_ind_r9_s::crit_exts_c_::c1_c_::types");
}

const char* rn_recfg_complete_r10_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"rnReconfigurationComplete-r10", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "rn_recfg_complete_r10_s::crit_exts_c_::c1_c_::types");
}

const char* rrc_conn_recfg_complete_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionReconfigurationComplete-r8", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_conn_recfg_complete_s::crit_exts_c_::types");
}

const char* rrc_conn_reest_complete_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionReestablishmentComplete-r8", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_conn_reest_complete_s::crit_exts_c_::types");
}

const char* rrc_conn_resume_complete_r13_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionResumeComplete-r13", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_conn_resume_complete_r13_s::crit_exts_c_::types");
}

const char* rrc_conn_setup_complete_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionSetupComplete-r8", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "rrc_conn_setup_complete_s::crit_exts_c_::c1_c_::types");
}

const char* scg_fail_info_r12_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"scgFailureInformation-r12", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "scg_fail_info_r12_s::crit_exts_c_::c1_c_::types");
}

const char* scg_fail_info_nr_r15_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"scgFailureInformationNR-r15", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "scg_fail_info_nr_r15_s::crit_exts_c_::c1_c_::types");
}

const char* security_mode_complete_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"securityModeComplete-r8", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "security_mode_complete_s::crit_exts_c_::types");
}

const char* security_mode_fail_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"securityModeFailure-r8", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "security_mode_fail_s::crit_exts_c_::types");
}

const char* sidelink_ue_info_r12_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"sidelinkUEInformation-r12", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "sidelink_ue_info_r12_s::crit_exts_c_::c1_c_::types");
}

const char* ueassist_info_r11_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"ueAssistanceInformation-r11", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ueassist_info_r11_s::crit_exts_c_::c1_c_::types");
}

const char* ue_cap_info_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "ueCapabilityInformation-r8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "ue_cap_info_s::crit_exts_c_::c1_c_::types");
}

const char* ue_info_resp_r9_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"ueInformationResponse-r9", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ue_info_resp_r9_s::crit_exts_c_::c1_c_::types");
}

const char* ul_ho_prep_transfer_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"ulHandoverPreparationTransfer-r8", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ul_ho_prep_transfer_s::crit_exts_c_::c1_c_::types");
}

const char* ul_info_transfer_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"ulInformationTransfer-r8", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ul_info_transfer_s::crit_exts_c_::c1_c_::types");
}

const char* ul_info_transfer_mrdc_r15_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"ulInformationTransferMRDC-r15", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ul_info_transfer_mrdc_r15_s::crit_exts_c_::c1_c_::types");
}

const char* wlan_conn_status_report_r13_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"wlanConnectionStatusReport-r13", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "wlan_conn_status_report_r13_s::crit_exts_c_::c1_c_::types");
}

const char* ul_dcch_msg_type_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"csfbParametersRequestCDMA2000",
                                  "measurementReport",
                                  "rrcConnectionReconfigurationComplete",
                                  "rrcConnectionReestablishmentComplete",
                                  "rrcConnectionSetupComplete",
                                  "securityModeComplete",
                                  "securityModeFailure",
                                  "ueCapabilityInformation",
                                  "ulHandoverPreparationTransfer",
                                  "ulInformationTransfer",
                                  "counterCheckResponse",
                                  "ueInformationResponse-r9",
                                  "proximityIndication-r9",
                                  "rnReconfigurationComplete-r10",
                                  "mbmsCountingResponse-r10",
                                  "interFreqRSTDMeasurementIndication-r10"};
  return convert_enum_idx(options, 16, value, "ul_dcch_msg_type_c::c1_c_::types");
}
uint16_t ul_dcch_msg_type_c::c1_c_::types_opts::to_number() const
{
  static const uint16_t options[] = {2000};
  return map_enum_number(options, 1, value, "ul_dcch_msg_type_c::c1_c_::types");
}

const char* ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::types_opts::to_string() const
{
  static const char* options[] = {"ueAssistanceInformation-r11",
                                  "inDeviceCoexIndication-r11",
                                  "mbmsInterestIndication-r11",
                                  "scgFailureInformation-r12",
                                  "sidelinkUEInformation-r12",
                                  "wlanConnectionStatusReport-r13",
                                  "rrcConnectionResumeComplete-r13",
                                  "ulInformationTransferMRDC-r15",
                                  "scgFailureInformationNR-r15",
                                  "measReportAppLayer-r15",
                                  "failureInformation-r15",
                                  "spare5",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::types");
}

const char* ul_dcch_msg_type_c::msg_class_ext_c_::types_opts::to_string() const
{
  static const char* options[] = {"c2", "messageClassExtensionFuture-r11"};
  return convert_enum_idx(options, 2, value, "ul_dcch_msg_type_c::msg_class_ext_c_::types");
}
uint8_t ul_dcch_msg_type_c::msg_class_ext_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {2};
  return map_enum_number(options, 1, value, "ul_dcch_msg_type_c::msg_class_ext_c_::types");
}

const char* ul_dcch_msg_type_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "ul_dcch_msg_type_c::types");
}
uint8_t ul_dcch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "ul_dcch_msg_type_c::types");
}

const char*
band_combination_params_r13_s::dc_support_r13_s_::supported_cell_grouping_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"threeEntries-r13", "fourEntries-r13", "fiveEntries-r13"};
  return convert_enum_idx(
      options, 3, value, "band_combination_params_r13_s::dc_support_r13_s_::supported_cell_grouping_r13_c_::types");
}
uint8_t band_combination_params_r13_s::dc_support_r13_s_::supported_cell_grouping_r13_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {3, 4, 5};
  return map_enum_number(
      options, 3, value, "band_combination_params_r13_s::dc_support_r13_s_::supported_cell_grouping_r13_c_::types");
}

const char*
band_combination_params_v1250_s::dc_support_r12_s_::supported_cell_grouping_r12_c_::types_opts::to_string() const
{
  static const char* options[] = {"threeEntries-r12", "fourEntries-r12", "fiveEntries-r12"};
  return convert_enum_idx(
      options, 3, value, "band_combination_params_v1250_s::dc_support_r12_s_::supported_cell_grouping_r12_c_::types");
}
uint8_t
band_combination_params_v1250_s::dc_support_r12_s_::supported_cell_grouping_r12_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {3, 4, 5};
  return map_enum_number(
      options, 3, value, "band_combination_params_v1250_s::dc_support_r12_s_::supported_cell_grouping_r12_c_::types");
}

// FailureReportSCG-v12d0 ::= SEQUENCE
SRSASN_CODE fail_report_scg_v12d0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_result_neigh_cells_v12d0_present, 1));

  if (meas_result_neigh_cells_v12d0_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_neigh_cells_v12d0, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE fail_report_scg_v12d0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_result_neigh_cells_v12d0_present, 1));

  if (meas_result_neigh_cells_v12d0_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_neigh_cells_v12d0, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void fail_report_scg_v12d0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_result_neigh_cells_v12d0_present) {
    j.start_array("measResultNeighCells-v12d0");
    for (const auto& e1 : meas_result_neigh_cells_v12d0) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// MIMO-WeightedLayersCapabilities-r13 ::= SEQUENCE
SRSASN_CODE mimo_weighted_layers_cap_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rel_weight_four_layers_r13_present, 1));
  HANDLE_CODE(bref.pack(rel_weight_eight_layers_r13_present, 1));

  HANDLE_CODE(rel_weight_two_layers_r13.pack(bref));
  if (rel_weight_four_layers_r13_present) {
    HANDLE_CODE(rel_weight_four_layers_r13.pack(bref));
  }
  if (rel_weight_eight_layers_r13_present) {
    HANDLE_CODE(rel_weight_eight_layers_r13.pack(bref));
  }
  HANDLE_CODE(pack_integer(bref, total_weighted_layers_r13, (uint8_t)2u, (uint8_t)128u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mimo_weighted_layers_cap_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rel_weight_four_layers_r13_present, 1));
  HANDLE_CODE(bref.unpack(rel_weight_eight_layers_r13_present, 1));

  HANDLE_CODE(rel_weight_two_layers_r13.unpack(bref));
  if (rel_weight_four_layers_r13_present) {
    HANDLE_CODE(rel_weight_four_layers_r13.unpack(bref));
  }
  if (rel_weight_eight_layers_r13_present) {
    HANDLE_CODE(rel_weight_eight_layers_r13.unpack(bref));
  }
  HANDLE_CODE(unpack_integer(total_weighted_layers_r13, bref, (uint8_t)2u, (uint8_t)128u));

  return SRSASN_SUCCESS;
}
void mimo_weighted_layers_cap_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("relWeightTwoLayers-r13", rel_weight_two_layers_r13.to_string());
  if (rel_weight_four_layers_r13_present) {
    j.write_str("relWeightFourLayers-r13", rel_weight_four_layers_r13.to_string());
  }
  if (rel_weight_eight_layers_r13_present) {
    j.write_str("relWeightEightLayers-r13", rel_weight_eight_layers_r13.to_string());
  }
  j.write_int("totalWeightedLayers-r13", total_weighted_layers_r13);
  j.end_obj();
}

const char* mimo_weighted_layers_cap_r13_s::rel_weight_two_layers_r13_opts::to_string() const
{
  static const char* options[] = {"v1", "v1dot25", "v1dot5", "v1dot75", "v2", "v2dot5", "v3", "v4"};
  return convert_enum_idx(options, 8, value, "mimo_weighted_layers_cap_r13_s::rel_weight_two_layers_r13_e_");
}
float mimo_weighted_layers_cap_r13_s::rel_weight_two_layers_r13_opts::to_number() const
{
  static const float options[] = {1.0, 1.25, 1.5, 1.75, 2.0, 2.5, 3.0, 4.0};
  return map_enum_number(options, 8, value, "mimo_weighted_layers_cap_r13_s::rel_weight_two_layers_r13_e_");
}
const char* mimo_weighted_layers_cap_r13_s::rel_weight_two_layers_r13_opts::to_number_string() const
{
  static const char* options[] = {"1", "1.25", "1.5", "1.75", "2", "2.5", "3", "4"};
  return convert_enum_idx(options, 8, value, "mimo_weighted_layers_cap_r13_s::rel_weight_two_layers_r13_e_");
}

const char* mimo_weighted_layers_cap_r13_s::rel_weight_four_layers_r13_opts::to_string() const
{
  static const char* options[] = {"v1", "v1dot25", "v1dot5", "v1dot75", "v2", "v2dot5", "v3", "v4"};
  return convert_enum_idx(options, 8, value, "mimo_weighted_layers_cap_r13_s::rel_weight_four_layers_r13_e_");
}
float mimo_weighted_layers_cap_r13_s::rel_weight_four_layers_r13_opts::to_number() const
{
  static const float options[] = {1.0, 1.25, 1.5, 1.75, 2.0, 2.5, 3.0, 4.0};
  return map_enum_number(options, 8, value, "mimo_weighted_layers_cap_r13_s::rel_weight_four_layers_r13_e_");
}
const char* mimo_weighted_layers_cap_r13_s::rel_weight_four_layers_r13_opts::to_number_string() const
{
  static const char* options[] = {"1", "1.25", "1.5", "1.75", "2", "2.5", "3", "4"};
  return convert_enum_idx(options, 8, value, "mimo_weighted_layers_cap_r13_s::rel_weight_four_layers_r13_e_");
}

const char* mimo_weighted_layers_cap_r13_s::rel_weight_eight_layers_r13_opts::to_string() const
{
  static const char* options[] = {"v1", "v1dot25", "v1dot5", "v1dot75", "v2", "v2dot5", "v3", "v4"};
  return convert_enum_idx(options, 8, value, "mimo_weighted_layers_cap_r13_s::rel_weight_eight_layers_r13_e_");
}
float mimo_weighted_layers_cap_r13_s::rel_weight_eight_layers_r13_opts::to_number() const
{
  static const float options[] = {1.0, 1.25, 1.5, 1.75, 2.0, 2.5, 3.0, 4.0};
  return map_enum_number(options, 8, value, "mimo_weighted_layers_cap_r13_s::rel_weight_eight_layers_r13_e_");
}
const char* mimo_weighted_layers_cap_r13_s::rel_weight_eight_layers_r13_opts::to_number_string() const
{
  static const char* options[] = {"1", "1.25", "1.5", "1.75", "2", "2.5", "3", "4"};
  return convert_enum_idx(options, 8, value, "mimo_weighted_layers_cap_r13_s::rel_weight_eight_layers_r13_e_");
}

// MIMO-UE-Parameters-v13e0 ::= SEQUENCE
SRSASN_CODE mimo_ue_params_v13e0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mimo_weighted_layers_cap_r13_present, 1));

  if (mimo_weighted_layers_cap_r13_present) {
    HANDLE_CODE(mimo_weighted_layers_cap_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE mimo_ue_params_v13e0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mimo_weighted_layers_cap_r13_present, 1));

  if (mimo_weighted_layers_cap_r13_present) {
    HANDLE_CODE(mimo_weighted_layers_cap_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void mimo_ue_params_v13e0_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mimo_weighted_layers_cap_r13_present) {
    j.write_fieldname("mimo-WeightedLayersCapabilities-r13");
    mimo_weighted_layers_cap_r13.to_json(j);
  }
  j.end_obj();
}

// MeasResult3EUTRA-r15 ::= SEQUENCE
SRSASN_CODE meas_result3_eutra_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_result_serving_cell_r15_present, 1));
  HANDLE_CODE(bref.pack(meas_result_neigh_cell_list_r15_present, 1));

  HANDLE_CODE(pack_integer(bref, carrier_freq_r15, (uint32_t)0u, (uint32_t)262143u));
  if (meas_result_serving_cell_r15_present) {
    HANDLE_CODE(meas_result_serving_cell_r15.pack(bref));
  }
  if (meas_result_neigh_cell_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_neigh_cell_list_r15, 1, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result3_eutra_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(meas_result_serving_cell_r15_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_neigh_cell_list_r15_present, 1));

  HANDLE_CODE(unpack_integer(carrier_freq_r15, bref, (uint32_t)0u, (uint32_t)262143u));
  if (meas_result_serving_cell_r15_present) {
    HANDLE_CODE(meas_result_serving_cell_r15.unpack(bref));
  }
  if (meas_result_neigh_cell_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_neigh_cell_list_r15, bref, 1, 8));
  }

  return SRSASN_SUCCESS;
}
void meas_result3_eutra_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("carrierFreq-r15", carrier_freq_r15);
  if (meas_result_serving_cell_r15_present) {
    j.write_fieldname("measResultServingCell-r15");
    meas_result_serving_cell_r15.to_json(j);
  }
  if (meas_result_neigh_cell_list_r15_present) {
    j.start_array("measResultNeighCellList-r15");
    for (const auto& e1 : meas_result_neigh_cell_list_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// MeasResultSCG-FailureMRDC-r15 ::= SEQUENCE
SRSASN_CODE meas_result_scg_fail_mrdc_r15_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_freq_list_eutra_r15, 1, 8));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_scg_fail_mrdc_r15_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_dyn_seq_of(meas_result_freq_list_eutra_r15, bref, 1, 8));

  return SRSASN_SUCCESS;
}
void meas_result_scg_fail_mrdc_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("measResultFreqListEUTRA-r15");
  for (const auto& e1 : meas_result_freq_list_eutra_r15) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// PhyLayerParameters-v13e0 ::= SEQUENCE
SRSASN_CODE phy_layer_params_v13e0_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(mimo_ue_params_v13e0.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE phy_layer_params_v13e0_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(mimo_ue_params_v13e0.unpack(bref));

  return SRSASN_SUCCESS;
}
void phy_layer_params_v13e0_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("mimo-UE-Parameters-v13e0");
  mimo_ue_params_v13e0.to_json(j);
  j.end_obj();
}

// SCGFailureInformation-v12d0b-IEs ::= SEQUENCE
SRSASN_CODE scg_fail_info_v12d0b_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(fail_report_scg_v12d0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (fail_report_scg_v12d0_present) {
    HANDLE_CODE(fail_report_scg_v12d0.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_fail_info_v12d0b_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(fail_report_scg_v12d0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (fail_report_scg_v12d0_present) {
    HANDLE_CODE(fail_report_scg_v12d0.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void scg_fail_info_v12d0b_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (fail_report_scg_v12d0_present) {
    j.write_fieldname("failureReportSCG-v12d0");
    fail_report_scg_v12d0.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

const char* mbms_params_v1470_s::mbms_max_bw_r14_c_::types_opts::to_string() const
{
  static const char* options[] = {"implicitValue", "explicitValue"};
  return convert_enum_idx(options, 2, value, "mbms_params_v1470_s::mbms_max_bw_r14_c_::types");
}

// UE-EUTRA-Capability-v13e0b-IEs ::= SEQUENCE
SRSASN_CODE ue_eutra_cap_v13e0b_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(phy_layer_params_v13e0.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_eutra_cap_v13e0b_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(phy_layer_params_v13e0.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_eutra_cap_v13e0b_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("phyLayerParameters-v13e0");
  phy_layer_params_v13e0.to_json(j);
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

const char* scg_cfg_r12_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "scg-Config-r12", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "scg_cfg_r12_s::crit_exts_c_::c1_c_::types");
}

void cells_triggered_list_item_c_::destroy_()
{
  switch (type_) {
    case types::pci_utra:
      c.destroy<pci_utra_c_>();
      break;
    case types::pci_geran:
      c.destroy<pci_geran_s_>();
      break;
    case types::wlan_ids_r13:
      c.destroy<wlan_ids_r12_s>();
      break;
    case types::pci_nr_r15:
      c.destroy<pci_nr_r15_s_>();
      break;
    default:
      break;
  }
}
void cells_triggered_list_item_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::pci_eutra:
      break;
    case types::pci_utra:
      c.init<pci_utra_c_>();
      break;
    case types::pci_geran:
      c.init<pci_geran_s_>();
      break;
    case types::pci_cdma2000:
      break;
    case types::wlan_ids_r13:
      c.init<wlan_ids_r12_s>();
      break;
    case types::pci_nr_r15:
      c.init<pci_nr_r15_s_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cells_triggered_list_item_c_");
  }
}
cells_triggered_list_item_c_::cells_triggered_list_item_c_(const cells_triggered_list_item_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::pci_eutra:
      c.init(other.c.get<uint16_t>());
      break;
    case types::pci_utra:
      c.init(other.c.get<pci_utra_c_>());
      break;
    case types::pci_geran:
      c.init(other.c.get<pci_geran_s_>());
      break;
    case types::pci_cdma2000:
      c.init(other.c.get<uint16_t>());
      break;
    case types::wlan_ids_r13:
      c.init(other.c.get<wlan_ids_r12_s>());
      break;
    case types::pci_nr_r15:
      c.init(other.c.get<pci_nr_r15_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cells_triggered_list_item_c_");
  }
}
cells_triggered_list_item_c_& cells_triggered_list_item_c_::operator=(const cells_triggered_list_item_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::pci_eutra:
      c.set(other.c.get<uint16_t>());
      break;
    case types::pci_utra:
      c.set(other.c.get<pci_utra_c_>());
      break;
    case types::pci_geran:
      c.set(other.c.get<pci_geran_s_>());
      break;
    case types::pci_cdma2000:
      c.set(other.c.get<uint16_t>());
      break;
    case types::wlan_ids_r13:
      c.set(other.c.get<wlan_ids_r12_s>());
      break;
    case types::pci_nr_r15:
      c.set(other.c.get<pci_nr_r15_s_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cells_triggered_list_item_c_");
  }

  return *this;
}
uint16_t& cells_triggered_list_item_c_::set_pci_eutra()
{
  set(types::pci_eutra);
  return c.get<uint16_t>();
}
cells_triggered_list_item_c_::pci_utra_c_& cells_triggered_list_item_c_::set_pci_utra()
{
  set(types::pci_utra);
  return c.get<pci_utra_c_>();
}
cells_triggered_list_item_c_::pci_geran_s_& cells_triggered_list_item_c_::set_pci_geran()
{
  set(types::pci_geran);
  return c.get<pci_geran_s_>();
}
uint16_t& cells_triggered_list_item_c_::set_pci_cdma2000()
{
  set(types::pci_cdma2000);
  return c.get<uint16_t>();
}
wlan_ids_r12_s& cells_triggered_list_item_c_::set_wlan_ids_r13()
{
  set(types::wlan_ids_r13);
  return c.get<wlan_ids_r12_s>();
}
cells_triggered_list_item_c_::pci_nr_r15_s_& cells_triggered_list_item_c_::set_pci_nr_r15()
{
  set(types::pci_nr_r15);
  return c.get<pci_nr_r15_s_>();
}
void cells_triggered_list_item_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::pci_eutra:
      j.write_int("physCellIdEUTRA", c.get<uint16_t>());
      break;
    case types::pci_utra:
      j.write_fieldname("physCellIdUTRA");
      c.get<pci_utra_c_>().to_json(j);
      break;
    case types::pci_geran:
      j.write_fieldname("physCellIdGERAN");
      j.start_obj();
      j.write_fieldname("carrierFreq");
      c.get<pci_geran_s_>().carrier_freq.to_json(j);
      j.write_fieldname("physCellId");
      c.get<pci_geran_s_>().pci.to_json(j);
      j.end_obj();
      break;
    case types::pci_cdma2000:
      j.write_int("physCellIdCDMA2000", c.get<uint16_t>());
      break;
    case types::wlan_ids_r13:
      j.write_fieldname("wlan-Identifiers-r13");
      c.get<wlan_ids_r12_s>().to_json(j);
      break;
    case types::pci_nr_r15:
      j.write_fieldname("physCellIdNR-r15");
      j.start_obj();
      j.write_int("carrierFreq", c.get<pci_nr_r15_s_>().carrier_freq);
      j.write_int("physCellId", c.get<pci_nr_r15_s_>().pci);
      if (c.get<pci_nr_r15_s_>().rs_idx_list_r15_present) {
        j.start_array("rs-IndexList-r15");
        for (const auto& e1 : c.get<pci_nr_r15_s_>().rs_idx_list_r15) {
          j.write_int(e1);
        }
        j.end_array();
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "cells_triggered_list_item_c_");
  }
  j.end_obj();
}
SRSASN_CODE cells_triggered_list_item_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::pci_eutra:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)503u));
      break;
    case types::pci_utra:
      HANDLE_CODE(c.get<pci_utra_c_>().pack(bref));
      break;
    case types::pci_geran:
      HANDLE_CODE(c.get<pci_geran_s_>().carrier_freq.pack(bref));
      HANDLE_CODE(c.get<pci_geran_s_>().pci.pack(bref));
      break;
    case types::pci_cdma2000:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)511u));
      break;
    case types::wlan_ids_r13:
      HANDLE_CODE(c.get<wlan_ids_r12_s>().pack(bref));
      break;
    case types::pci_nr_r15:
      HANDLE_CODE(bref.pack(c.get<pci_nr_r15_s_>().rs_idx_list_r15_present, 1));
      HANDLE_CODE(pack_integer(bref, c.get<pci_nr_r15_s_>().carrier_freq, (uint32_t)0u, (uint32_t)3279165u));
      HANDLE_CODE(pack_integer(bref, c.get<pci_nr_r15_s_>().pci, (uint16_t)0u, (uint16_t)1007u));
      if (c.get<pci_nr_r15_s_>().rs_idx_list_r15_present) {
        HANDLE_CODE(
            pack_dyn_seq_of(bref, c.get<pci_nr_r15_s_>().rs_idx_list_r15, 1, 64, integer_packer<uint8_t>(0, 63)));
      }
      break;
    default:
      log_invalid_choice_id(type_, "cells_triggered_list_item_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cells_triggered_list_item_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::pci_eutra:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)503u));
      break;
    case types::pci_utra:
      HANDLE_CODE(c.get<pci_utra_c_>().unpack(bref));
      break;
    case types::pci_geran:
      HANDLE_CODE(c.get<pci_geran_s_>().carrier_freq.unpack(bref));
      HANDLE_CODE(c.get<pci_geran_s_>().pci.unpack(bref));
      break;
    case types::pci_cdma2000:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)511u));
      break;
    case types::wlan_ids_r13:
      HANDLE_CODE(c.get<wlan_ids_r12_s>().unpack(bref));
      break;
    case types::pci_nr_r15:
      HANDLE_CODE(bref.unpack(c.get<pci_nr_r15_s_>().rs_idx_list_r15_present, 1));
      HANDLE_CODE(unpack_integer(c.get<pci_nr_r15_s_>().carrier_freq, bref, (uint32_t)0u, (uint32_t)3279165u));
      HANDLE_CODE(unpack_integer(c.get<pci_nr_r15_s_>().pci, bref, (uint16_t)0u, (uint16_t)1007u));
      if (c.get<pci_nr_r15_s_>().rs_idx_list_r15_present) {
        HANDLE_CODE(
            unpack_dyn_seq_of(c.get<pci_nr_r15_s_>().rs_idx_list_r15, bref, 1, 64, integer_packer<uint8_t>(0, 63)));
      }
      break;
    default:
      log_invalid_choice_id(type_, "cells_triggered_list_item_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void cells_triggered_list_item_c_::pci_utra_c_::destroy_() {}
void cells_triggered_list_item_c_::pci_utra_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
cells_triggered_list_item_c_::pci_utra_c_::pci_utra_c_(const cells_triggered_list_item_c_::pci_utra_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::fdd:
      c.init(other.c.get<uint16_t>());
      break;
    case types::tdd:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cells_triggered_list_item_c_::pci_utra_c_");
  }
}
cells_triggered_list_item_c_::pci_utra_c_&
cells_triggered_list_item_c_::pci_utra_c_::operator=(const cells_triggered_list_item_c_::pci_utra_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::fdd:
      c.set(other.c.get<uint16_t>());
      break;
    case types::tdd:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cells_triggered_list_item_c_::pci_utra_c_");
  }

  return *this;
}
uint16_t& cells_triggered_list_item_c_::pci_utra_c_::set_fdd()
{
  set(types::fdd);
  return c.get<uint16_t>();
}
uint8_t& cells_triggered_list_item_c_::pci_utra_c_::set_tdd()
{
  set(types::tdd);
  return c.get<uint8_t>();
}
void cells_triggered_list_item_c_::pci_utra_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::fdd:
      j.write_int("fdd", c.get<uint16_t>());
      break;
    case types::tdd:
      j.write_int("tdd", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "cells_triggered_list_item_c_::pci_utra_c_");
  }
  j.end_obj();
}
SRSASN_CODE cells_triggered_list_item_c_::pci_utra_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::fdd:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)511u));
      break;
    case types::tdd:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)0u, (uint8_t)127u));
      break;
    default:
      log_invalid_choice_id(type_, "cells_triggered_list_item_c_::pci_utra_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cells_triggered_list_item_c_::pci_utra_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::fdd:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)511u));
      break;
    case types::tdd:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)0u, (uint8_t)127u));
      break;
    default:
      log_invalid_choice_id(type_, "cells_triggered_list_item_c_::pci_utra_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* cells_triggered_list_item_c_::pci_utra_c_::types_opts::to_string() const
{
  static const char* options[] = {"fdd", "tdd"};
  return convert_enum_idx(options, 2, value, "cells_triggered_list_item_c_::pci_utra_c_::types");
}

const char* cells_triggered_list_item_c_::types_opts::to_string() const
{
  static const char* options[] = {"physCellIdEUTRA",
                                  "physCellIdUTRA",
                                  "physCellIdGERAN",
                                  "physCellIdCDMA2000",
                                  "wlan-Identifiers-r13",
                                  "physCellIdNR-r15"};
  return convert_enum_idx(options, 6, value, "cells_triggered_list_item_c_::types");
}
uint16_t cells_triggered_list_item_c_::types_opts::to_number() const
{
  if (value == pci_cdma2000) {
    return 2000;
  }
  invalid_enum_number(value, "cells_triggered_list_item_c_::types");
  return 0;
}

// DRB-InfoSCG-r12 ::= SEQUENCE
SRSASN_CODE drb_info_scg_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(eps_bearer_id_r12_present, 1));
  HANDLE_CODE(bref.pack(drb_type_r12_present, 1));

  if (eps_bearer_id_r12_present) {
    HANDLE_CODE(pack_integer(bref, eps_bearer_id_r12, (uint8_t)0u, (uint8_t)15u));
  }
  HANDLE_CODE(pack_integer(bref, drb_id_r12, (uint8_t)1u, (uint8_t)32u));
  if (drb_type_r12_present) {
    HANDLE_CODE(drb_type_r12.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE drb_info_scg_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(eps_bearer_id_r12_present, 1));
  HANDLE_CODE(bref.unpack(drb_type_r12_present, 1));

  if (eps_bearer_id_r12_present) {
    HANDLE_CODE(unpack_integer(eps_bearer_id_r12, bref, (uint8_t)0u, (uint8_t)15u));
  }
  HANDLE_CODE(unpack_integer(drb_id_r12, bref, (uint8_t)1u, (uint8_t)32u));
  if (drb_type_r12_present) {
    HANDLE_CODE(drb_type_r12.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void drb_info_scg_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (eps_bearer_id_r12_present) {
    j.write_int("eps-BearerIdentity-r12", eps_bearer_id_r12);
  }
  j.write_int("drb-Identity-r12", drb_id_r12);
  if (drb_type_r12_present) {
    j.write_str("drb-Type-r12", drb_type_r12.to_string());
  }
  j.end_obj();
}

const char* drb_info_scg_r12_s::drb_type_r12_opts::to_string() const
{
  static const char* options[] = {"split", "scg"};
  return convert_enum_idx(options, 2, value, "drb_info_scg_r12_s::drb_type_r12_e_");
}

const char* ho_cmd_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "handoverCommand-r8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "ho_cmd_s::crit_exts_c_::c1_c_::types");
}

const char* ho_prep_info_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "handoverPreparationInformation-r8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "ho_prep_info_s::crit_exts_c_::c1_c_::types");
}

// TDD-ConfigSL-r12 ::= SEQUENCE
SRSASN_CODE tdd_cfg_sl_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sf_assign_sl_r12.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE tdd_cfg_sl_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sf_assign_sl_r12.unpack(bref));

  return SRSASN_SUCCESS;
}
void tdd_cfg_sl_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("subframeAssignmentSL-r12", sf_assign_sl_r12.to_string());
  j.end_obj();
}

const char* tdd_cfg_sl_r12_s::sf_assign_sl_r12_opts::to_string() const
{
  static const char* options[] = {"none", "sa0", "sa1", "sa2", "sa3", "sa4", "sa5", "sa6"};
  return convert_enum_idx(options, 8, value, "tdd_cfg_sl_r12_s::sf_assign_sl_r12_e_");
}

// MasterInformationBlock-SL ::= SEQUENCE
SRSASN_CODE mib_sl_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sl_bw_r12.pack(bref));
  HANDLE_CODE(tdd_cfg_sl_r12.pack(bref));
  HANDLE_CODE(direct_frame_num_r12.pack(bref));
  HANDLE_CODE(pack_integer(bref, direct_sf_num_r12, (uint8_t)0u, (uint8_t)9u));
  HANDLE_CODE(bref.pack(in_coverage_r12, 1));
  HANDLE_CODE(reserved_r12.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mib_sl_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sl_bw_r12.unpack(bref));
  HANDLE_CODE(tdd_cfg_sl_r12.unpack(bref));
  HANDLE_CODE(direct_frame_num_r12.unpack(bref));
  HANDLE_CODE(unpack_integer(direct_sf_num_r12, bref, (uint8_t)0u, (uint8_t)9u));
  HANDLE_CODE(bref.unpack(in_coverage_r12, 1));
  HANDLE_CODE(reserved_r12.unpack(bref));

  return SRSASN_SUCCESS;
}
void mib_sl_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sl-Bandwidth-r12", sl_bw_r12.to_string());
  j.write_fieldname("tdd-ConfigSL-r12");
  tdd_cfg_sl_r12.to_json(j);
  j.write_str("directFrameNumber-r12", direct_frame_num_r12.to_string());
  j.write_int("directSubframeNumber-r12", direct_sf_num_r12);
  j.write_bool("inCoverage-r12", in_coverage_r12);
  j.write_str("reserved-r12", reserved_r12.to_string());
  j.end_obj();
}

const char* mib_sl_s::sl_bw_r12_opts::to_string() const
{
  static const char* options[] = {"n6", "n15", "n25", "n50", "n75", "n100"};
  return convert_enum_idx(options, 6, value, "mib_sl_s::sl_bw_r12_e_");
}
uint8_t mib_sl_s::sl_bw_r12_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(options, 6, value, "mib_sl_s::sl_bw_r12_e_");
}

// MasterInformationBlock-SL-V2X-r14 ::= SEQUENCE
SRSASN_CODE mib_sl_v2x_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sl_bw_r14.pack(bref));
  HANDLE_CODE(tdd_cfg_sl_r14.pack(bref));
  HANDLE_CODE(direct_frame_num_r14.pack(bref));
  HANDLE_CODE(pack_integer(bref, direct_sf_num_r14, (uint8_t)0u, (uint8_t)9u));
  HANDLE_CODE(bref.pack(in_coverage_r14, 1));
  HANDLE_CODE(reserved_r14.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mib_sl_v2x_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sl_bw_r14.unpack(bref));
  HANDLE_CODE(tdd_cfg_sl_r14.unpack(bref));
  HANDLE_CODE(direct_frame_num_r14.unpack(bref));
  HANDLE_CODE(unpack_integer(direct_sf_num_r14, bref, (uint8_t)0u, (uint8_t)9u));
  HANDLE_CODE(bref.unpack(in_coverage_r14, 1));
  HANDLE_CODE(reserved_r14.unpack(bref));

  return SRSASN_SUCCESS;
}
void mib_sl_v2x_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sl-Bandwidth-r14", sl_bw_r14.to_string());
  j.write_fieldname("tdd-ConfigSL-r14");
  tdd_cfg_sl_r14.to_json(j);
  j.write_str("directFrameNumber-r14", direct_frame_num_r14.to_string());
  j.write_int("directSubframeNumber-r14", direct_sf_num_r14);
  j.write_bool("inCoverage-r14", in_coverage_r14);
  j.write_str("reserved-r14", reserved_r14.to_string());
  j.end_obj();
}

const char* mib_sl_v2x_r14_s::sl_bw_r14_opts::to_string() const
{
  static const char* options[] = {"n6", "n15", "n25", "n50", "n75", "n100"};
  return convert_enum_idx(options, 6, value, "mib_sl_v2x_r14_s::sl_bw_r14_e_");
}
uint8_t mib_sl_v2x_r14_s::sl_bw_r14_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(options, 6, value, "mib_sl_v2x_r14_s::sl_bw_r14_e_");
}

// MeasResultRSSI-SCG-r13 ::= SEQUENCE
SRSASN_CODE meas_result_rssi_scg_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, serv_cell_id_r13, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(meas_result_for_rssi_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_rssi_scg_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(serv_cell_id_r13, bref, (uint8_t)0u, (uint8_t)31u));
  HANDLE_CODE(meas_result_for_rssi_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void meas_result_rssi_scg_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servCellId-r13", serv_cell_id_r13);
  j.write_fieldname("measResultForRSSI-r13");
  meas_result_for_rssi_r13.to_json(j);
  j.end_obj();
}

// MeasResultServCellSCG-r12 ::= SEQUENCE
SRSASN_CODE meas_result_serv_cell_scg_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, serv_cell_id_r12, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(pack_integer(bref, meas_result_scell_r12.rsrp_result_scell_r12, (uint8_t)0u, (uint8_t)97u));
  HANDLE_CODE(pack_integer(bref, meas_result_scell_r12.rsrq_result_scell_r12, (uint8_t)0u, (uint8_t)34u));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= serv_cell_id_r13_present;
    group_flags[0] |= meas_result_scell_v1310.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(serv_cell_id_r13_present, 1));
      HANDLE_CODE(bref.pack(meas_result_scell_v1310.is_present(), 1));
      if (serv_cell_id_r13_present) {
        HANDLE_CODE(pack_integer(bref, serv_cell_id_r13, (uint8_t)0u, (uint8_t)31u));
      }
      if (meas_result_scell_v1310.is_present()) {
        HANDLE_CODE(pack_integer(bref, meas_result_scell_v1310->rs_sinr_result_scell_r13, (uint8_t)0u, (uint8_t)127u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_result_serv_cell_scg_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(serv_cell_id_r12, bref, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(unpack_integer(meas_result_scell_r12.rsrp_result_scell_r12, bref, (uint8_t)0u, (uint8_t)97u));
  HANDLE_CODE(unpack_integer(meas_result_scell_r12.rsrq_result_scell_r12, bref, (uint8_t)0u, (uint8_t)34u));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(serv_cell_id_r13_present, 1));
      bool meas_result_scell_v1310_present;
      HANDLE_CODE(bref.unpack(meas_result_scell_v1310_present, 1));
      meas_result_scell_v1310.set_present(meas_result_scell_v1310_present);
      if (serv_cell_id_r13_present) {
        HANDLE_CODE(unpack_integer(serv_cell_id_r13, bref, (uint8_t)0u, (uint8_t)31u));
      }
      if (meas_result_scell_v1310.is_present()) {
        HANDLE_CODE(
            unpack_integer(meas_result_scell_v1310->rs_sinr_result_scell_r13, bref, (uint8_t)0u, (uint8_t)127u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void meas_result_serv_cell_scg_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("servCellId-r12", serv_cell_id_r12);
  j.write_fieldname("measResultSCell-r12");
  j.start_obj();
  j.write_int("rsrpResultSCell-r12", meas_result_scell_r12.rsrp_result_scell_r12);
  j.write_int("rsrqResultSCell-r12", meas_result_scell_r12.rsrq_result_scell_r12);
  j.end_obj();
  if (ext) {
    if (serv_cell_id_r13_present) {
      j.write_int("servCellId-r13", serv_cell_id_r13);
    }
    if (meas_result_scell_v1310.is_present()) {
      j.write_fieldname("measResultSCell-v1310");
      j.start_obj();
      j.write_int("rs-sinr-ResultSCell-r13", meas_result_scell_v1310->rs_sinr_result_scell_r13);
      j.end_obj();
    }
  }
  j.end_obj();
}

// SBCCH-SL-BCH-Message ::= SEQUENCE
SRSASN_CODE sbcch_sl_bch_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE sbcch_sl_bch_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void sbcch_sl_bch_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("SBCCH-SL-BCH-Message");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// SBCCH-SL-BCH-Message-V2X-r14 ::= SEQUENCE
SRSASN_CODE sbcch_sl_bch_msg_v2x_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE sbcch_sl_bch_msg_v2x_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void sbcch_sl_bch_msg_v2x_r14_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("SBCCH-SL-BCH-Message-V2X-r14");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// SCG-Config-v12i0b-IEs ::= SEQUENCE
SRSASN_CODE scg_cfg_v12i0b_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(scg_radio_cfg_v12i0_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (scg_radio_cfg_v12i0_present) {
    HANDLE_CODE(scg_radio_cfg_v12i0.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_cfg_v12i0b_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(scg_radio_cfg_v12i0_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (scg_radio_cfg_v12i0_present) {
    HANDLE_CODE(scg_radio_cfg_v12i0.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void scg_cfg_v12i0b_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (scg_radio_cfg_v12i0_present) {
    j.write_fieldname("scg-RadioConfig-v12i0");
    scg_radio_cfg_v12i0.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// SCG-ConfigInfo-v1530-IEs ::= SEQUENCE
SRSASN_CODE scg_cfg_info_v1530_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(drb_to_add_mod_list_scg_r15_present, 1));
  HANDLE_CODE(bref.pack(drb_to_release_list_scg_r15_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (drb_to_add_mod_list_scg_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_to_add_mod_list_scg_r15, 1, 15));
  }
  if (drb_to_release_list_scg_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_to_release_list_scg_r15, 1, 15, integer_packer<uint8_t>(1, 32)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_cfg_info_v1530_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(drb_to_add_mod_list_scg_r15_present, 1));
  HANDLE_CODE(bref.unpack(drb_to_release_list_scg_r15_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (drb_to_add_mod_list_scg_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drb_to_add_mod_list_scg_r15, bref, 1, 15));
  }
  if (drb_to_release_list_scg_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drb_to_release_list_scg_r15, bref, 1, 15, integer_packer<uint8_t>(1, 32)));
  }

  return SRSASN_SUCCESS;
}
void scg_cfg_info_v1530_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (drb_to_add_mod_list_scg_r15_present) {
    j.start_array("drb-ToAddModListSCG-r15");
    for (const auto& e1 : drb_to_add_mod_list_scg_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drb_to_release_list_scg_r15_present) {
    j.start_array("drb-ToReleaseListSCG-r15");
    for (const auto& e1 : drb_to_release_list_scg_r15) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

// SCG-ConfigInfo-v1430-IEs ::= SEQUENCE
SRSASN_CODE scg_cfg_info_v1430_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(make_before_break_scg_req_r14_present, 1));
  HANDLE_CODE(bref.pack(meas_gap_cfg_per_cc_list_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (meas_gap_cfg_per_cc_list_present) {
    HANDLE_CODE(meas_gap_cfg_per_cc_list.pack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_cfg_info_v1430_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(make_before_break_scg_req_r14_present, 1));
  HANDLE_CODE(bref.unpack(meas_gap_cfg_per_cc_list_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (meas_gap_cfg_per_cc_list_present) {
    HANDLE_CODE(meas_gap_cfg_per_cc_list.unpack(bref));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void scg_cfg_info_v1430_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (make_before_break_scg_req_r14_present) {
    j.write_str("makeBeforeBreakSCG-Req-r14", "true");
  }
  if (meas_gap_cfg_per_cc_list_present) {
    j.write_fieldname("measGapConfigPerCC-List");
    meas_gap_cfg_per_cc_list.to_json(j);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SCG-ConfigInfo-v1330-IEs ::= SEQUENCE
SRSASN_CODE scg_cfg_info_v1330_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_result_list_rssi_scg_r13_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (meas_result_list_rssi_scg_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_list_rssi_scg_r13, 1, 32));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_cfg_info_v1330_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_result_list_rssi_scg_r13_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (meas_result_list_rssi_scg_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_list_rssi_scg_r13, bref, 1, 32));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void scg_cfg_info_v1330_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_result_list_rssi_scg_r13_present) {
    j.start_array("measResultListRSSI-SCG-r13");
    for (const auto& e1 : meas_result_list_rssi_scg_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SCG-ConfigInfo-v1310-IEs ::= SEQUENCE
SRSASN_CODE scg_cfg_info_v1310_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_result_sstd_r13_present, 1));
  HANDLE_CODE(bref.pack(scell_to_add_mod_list_mcg_ext_r13_present, 1));
  HANDLE_CODE(bref.pack(meas_result_serv_cell_list_scg_ext_r13_present, 1));
  HANDLE_CODE(bref.pack(scell_to_add_mod_list_scg_ext_r13_present, 1));
  HANDLE_CODE(bref.pack(scell_to_release_list_scg_ext_r13_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (meas_result_sstd_r13_present) {
    HANDLE_CODE(meas_result_sstd_r13.pack(bref));
  }
  if (scell_to_add_mod_list_mcg_ext_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_add_mod_list_mcg_ext_r13, 1, 31));
  }
  if (meas_result_serv_cell_list_scg_ext_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_serv_cell_list_scg_ext_r13, 1, 32));
  }
  if (scell_to_add_mod_list_scg_ext_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_add_mod_list_scg_ext_r13, 1, 31));
  }
  if (scell_to_release_list_scg_ext_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_release_list_scg_ext_r13, 1, 31, integer_packer<uint8_t>(1, 31)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_cfg_info_v1310_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_result_sstd_r13_present, 1));
  HANDLE_CODE(bref.unpack(scell_to_add_mod_list_mcg_ext_r13_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_serv_cell_list_scg_ext_r13_present, 1));
  HANDLE_CODE(bref.unpack(scell_to_add_mod_list_scg_ext_r13_present, 1));
  HANDLE_CODE(bref.unpack(scell_to_release_list_scg_ext_r13_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (meas_result_sstd_r13_present) {
    HANDLE_CODE(meas_result_sstd_r13.unpack(bref));
  }
  if (scell_to_add_mod_list_mcg_ext_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_to_add_mod_list_mcg_ext_r13, bref, 1, 31));
  }
  if (meas_result_serv_cell_list_scg_ext_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_serv_cell_list_scg_ext_r13, bref, 1, 32));
  }
  if (scell_to_add_mod_list_scg_ext_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_to_add_mod_list_scg_ext_r13, bref, 1, 31));
  }
  if (scell_to_release_list_scg_ext_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_to_release_list_scg_ext_r13, bref, 1, 31, integer_packer<uint8_t>(1, 31)));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void scg_cfg_info_v1310_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_result_sstd_r13_present) {
    j.write_fieldname("measResultSSTD-r13");
    meas_result_sstd_r13.to_json(j);
  }
  if (scell_to_add_mod_list_mcg_ext_r13_present) {
    j.start_array("sCellToAddModListMCG-Ext-r13");
    for (const auto& e1 : scell_to_add_mod_list_mcg_ext_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_result_serv_cell_list_scg_ext_r13_present) {
    j.start_array("measResultServCellListSCG-Ext-r13");
    for (const auto& e1 : meas_result_serv_cell_list_scg_ext_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (scell_to_add_mod_list_scg_ext_r13_present) {
    j.start_array("sCellToAddModListSCG-Ext-r13");
    for (const auto& e1 : scell_to_add_mod_list_scg_ext_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (scell_to_release_list_scg_ext_r13_present) {
    j.start_array("sCellToReleaseListSCG-Ext-r13");
    for (const auto& e1 : scell_to_release_list_scg_ext_r13) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SCG-ConfigRestrictInfo-r12 ::= SEQUENCE
SRSASN_CODE scg_cfg_restrict_info_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, max_sch_tb_bits_dl_r12, (uint8_t)1u, (uint8_t)100u));
  HANDLE_CODE(pack_integer(bref, max_sch_tb_bits_ul_r12, (uint8_t)1u, (uint8_t)100u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_cfg_restrict_info_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(max_sch_tb_bits_dl_r12, bref, (uint8_t)1u, (uint8_t)100u));
  HANDLE_CODE(unpack_integer(max_sch_tb_bits_ul_r12, bref, (uint8_t)1u, (uint8_t)100u));

  return SRSASN_SUCCESS;
}
void scg_cfg_restrict_info_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("maxSCH-TB-BitsDL-r12", max_sch_tb_bits_dl_r12);
  j.write_int("maxSCH-TB-BitsUL-r12", max_sch_tb_bits_ul_r12);
  j.end_obj();
}

// SCG-ConfigInfo-r12-IEs ::= SEQUENCE
SRSASN_CODE scg_cfg_info_r12_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rr_cfg_ded_mcg_r12_present, 1));
  HANDLE_CODE(bref.pack(scell_to_add_mod_list_mcg_r12_present, 1));
  HANDLE_CODE(bref.pack(meas_gap_cfg_r12_present, 1));
  HANDLE_CODE(bref.pack(pwr_coordination_info_r12_present, 1));
  HANDLE_CODE(bref.pack(scg_radio_cfg_r12_present, 1));
  HANDLE_CODE(bref.pack(eutra_cap_info_r12_present, 1));
  HANDLE_CODE(bref.pack(scg_cfg_restrict_info_r12_present, 1));
  HANDLE_CODE(bref.pack(mbms_interest_ind_r12_present, 1));
  HANDLE_CODE(bref.pack(meas_result_serv_cell_list_scg_r12_present, 1));
  HANDLE_CODE(bref.pack(drb_to_add_mod_list_scg_r12_present, 1));
  HANDLE_CODE(bref.pack(drb_to_release_list_scg_r12_present, 1));
  HANDLE_CODE(bref.pack(scell_to_add_mod_list_scg_r12_present, 1));
  HANDLE_CODE(bref.pack(scell_to_release_list_scg_r12_present, 1));
  HANDLE_CODE(bref.pack(p_max_r12_present, 1));
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  if (rr_cfg_ded_mcg_r12_present) {
    HANDLE_CODE(rr_cfg_ded_mcg_r12.pack(bref));
  }
  if (scell_to_add_mod_list_mcg_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_add_mod_list_mcg_r12, 1, 4));
  }
  if (meas_gap_cfg_r12_present) {
    HANDLE_CODE(meas_gap_cfg_r12.pack(bref));
  }
  if (pwr_coordination_info_r12_present) {
    HANDLE_CODE(pwr_coordination_info_r12.pack(bref));
  }
  if (scg_radio_cfg_r12_present) {
    HANDLE_CODE(scg_radio_cfg_r12.pack(bref));
  }
  if (eutra_cap_info_r12_present) {
    HANDLE_CODE(eutra_cap_info_r12.pack(bref));
  }
  if (scg_cfg_restrict_info_r12_present) {
    HANDLE_CODE(scg_cfg_restrict_info_r12.pack(bref));
  }
  if (mbms_interest_ind_r12_present) {
    HANDLE_CODE(mbms_interest_ind_r12.pack(bref));
  }
  if (meas_result_serv_cell_list_scg_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_result_serv_cell_list_scg_r12, 1, 5));
  }
  if (drb_to_add_mod_list_scg_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_to_add_mod_list_scg_r12, 1, 11));
  }
  if (drb_to_release_list_scg_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, drb_to_release_list_scg_r12, 1, 11, integer_packer<uint8_t>(1, 32)));
  }
  if (scell_to_add_mod_list_scg_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_add_mod_list_scg_r12, 1, 4));
  }
  if (scell_to_release_list_scg_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, scell_to_release_list_scg_r12, 1, 4, integer_packer<uint8_t>(1, 7)));
  }
  if (p_max_r12_present) {
    HANDLE_CODE(pack_integer(bref, p_max_r12, (int8_t)-30, (int8_t)33));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_cfg_info_r12_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rr_cfg_ded_mcg_r12_present, 1));
  HANDLE_CODE(bref.unpack(scell_to_add_mod_list_mcg_r12_present, 1));
  HANDLE_CODE(bref.unpack(meas_gap_cfg_r12_present, 1));
  HANDLE_CODE(bref.unpack(pwr_coordination_info_r12_present, 1));
  HANDLE_CODE(bref.unpack(scg_radio_cfg_r12_present, 1));
  HANDLE_CODE(bref.unpack(eutra_cap_info_r12_present, 1));
  HANDLE_CODE(bref.unpack(scg_cfg_restrict_info_r12_present, 1));
  HANDLE_CODE(bref.unpack(mbms_interest_ind_r12_present, 1));
  HANDLE_CODE(bref.unpack(meas_result_serv_cell_list_scg_r12_present, 1));
  HANDLE_CODE(bref.unpack(drb_to_add_mod_list_scg_r12_present, 1));
  HANDLE_CODE(bref.unpack(drb_to_release_list_scg_r12_present, 1));
  HANDLE_CODE(bref.unpack(scell_to_add_mod_list_scg_r12_present, 1));
  HANDLE_CODE(bref.unpack(scell_to_release_list_scg_r12_present, 1));
  HANDLE_CODE(bref.unpack(p_max_r12_present, 1));
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  if (rr_cfg_ded_mcg_r12_present) {
    HANDLE_CODE(rr_cfg_ded_mcg_r12.unpack(bref));
  }
  if (scell_to_add_mod_list_mcg_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_to_add_mod_list_mcg_r12, bref, 1, 4));
  }
  if (meas_gap_cfg_r12_present) {
    HANDLE_CODE(meas_gap_cfg_r12.unpack(bref));
  }
  if (pwr_coordination_info_r12_present) {
    HANDLE_CODE(pwr_coordination_info_r12.unpack(bref));
  }
  if (scg_radio_cfg_r12_present) {
    HANDLE_CODE(scg_radio_cfg_r12.unpack(bref));
  }
  if (eutra_cap_info_r12_present) {
    HANDLE_CODE(eutra_cap_info_r12.unpack(bref));
  }
  if (scg_cfg_restrict_info_r12_present) {
    HANDLE_CODE(scg_cfg_restrict_info_r12.unpack(bref));
  }
  if (mbms_interest_ind_r12_present) {
    HANDLE_CODE(mbms_interest_ind_r12.unpack(bref));
  }
  if (meas_result_serv_cell_list_scg_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_result_serv_cell_list_scg_r12, bref, 1, 5));
  }
  if (drb_to_add_mod_list_scg_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drb_to_add_mod_list_scg_r12, bref, 1, 11));
  }
  if (drb_to_release_list_scg_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(drb_to_release_list_scg_r12, bref, 1, 11, integer_packer<uint8_t>(1, 32)));
  }
  if (scell_to_add_mod_list_scg_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_to_add_mod_list_scg_r12, bref, 1, 4));
  }
  if (scell_to_release_list_scg_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(scell_to_release_list_scg_r12, bref, 1, 4, integer_packer<uint8_t>(1, 7)));
  }
  if (p_max_r12_present) {
    HANDLE_CODE(unpack_integer(p_max_r12, bref, (int8_t)-30, (int8_t)33));
  }
  if (non_crit_ext_present) {
    HANDLE_CODE(non_crit_ext.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void scg_cfg_info_r12_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rr_cfg_ded_mcg_r12_present) {
    j.write_fieldname("radioResourceConfigDedMCG-r12");
    rr_cfg_ded_mcg_r12.to_json(j);
  }
  if (scell_to_add_mod_list_mcg_r12_present) {
    j.start_array("sCellToAddModListMCG-r12");
    for (const auto& e1 : scell_to_add_mod_list_mcg_r12) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (meas_gap_cfg_r12_present) {
    j.write_fieldname("measGapConfig-r12");
    meas_gap_cfg_r12.to_json(j);
  }
  if (pwr_coordination_info_r12_present) {
    j.write_fieldname("powerCoordinationInfo-r12");
    pwr_coordination_info_r12.to_json(j);
  }
  if (scg_radio_cfg_r12_present) {
    j.write_fieldname("scg-RadioConfig-r12");
    scg_radio_cfg_r12.to_json(j);
  }
  if (eutra_cap_info_r12_present) {
    j.write_str("eutra-CapabilityInfo-r12", eutra_cap_info_r12.to_string());
  }
  if (scg_cfg_restrict_info_r12_present) {
    j.write_fieldname("scg-ConfigRestrictInfo-r12");
    scg_cfg_restrict_info_r12.to_json(j);
  }
  if (mbms_interest_ind_r12_present) {
    j.write_str("mbmsInterestIndication-r12", mbms_interest_ind_r12.to_string());
  }
  if (meas_result_serv_cell_list_scg_r12_present) {
    j.start_array("measResultServCellListSCG-r12");
    for (const auto& e1 : meas_result_serv_cell_list_scg_r12) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drb_to_add_mod_list_scg_r12_present) {
    j.start_array("drb-ToAddModListSCG-r12");
    for (const auto& e1 : drb_to_add_mod_list_scg_r12) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (drb_to_release_list_scg_r12_present) {
    j.start_array("drb-ToReleaseListSCG-r12");
    for (const auto& e1 : drb_to_release_list_scg_r12) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (scell_to_add_mod_list_scg_r12_present) {
    j.start_array("sCellToAddModListSCG-r12");
    for (const auto& e1 : scell_to_add_mod_list_scg_r12) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (scell_to_release_list_scg_r12_present) {
    j.start_array("sCellToReleaseListSCG-r12");
    for (const auto& e1 : scell_to_release_list_scg_r12) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (p_max_r12_present) {
    j.write_int("p-Max-r12", p_max_r12);
  }
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    non_crit_ext.to_json(j);
  }
  j.end_obj();
}

// SCG-ConfigInfo-r12 ::= SEQUENCE
SRSASN_CODE scg_cfg_info_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_cfg_info_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void scg_cfg_info_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void scg_cfg_info_r12_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
scg_cfg_info_r12_s::crit_exts_c_::c1_c_& scg_cfg_info_r12_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void scg_cfg_info_r12_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
void scg_cfg_info_r12_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "scg_cfg_info_r12_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE scg_cfg_info_r12_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "scg_cfg_info_r12_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_cfg_info_r12_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "scg_cfg_info_r12_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void scg_cfg_info_r12_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
scg_cfg_info_r12_ies_s& scg_cfg_info_r12_s::crit_exts_c_::c1_c_::set_scg_cfg_info_r12()
{
  set(types::scg_cfg_info_r12);
  return c;
}
void scg_cfg_info_r12_s::crit_exts_c_::c1_c_::set_spare7()
{
  set(types::spare7);
}
void scg_cfg_info_r12_s::crit_exts_c_::c1_c_::set_spare6()
{
  set(types::spare6);
}
void scg_cfg_info_r12_s::crit_exts_c_::c1_c_::set_spare5()
{
  set(types::spare5);
}
void scg_cfg_info_r12_s::crit_exts_c_::c1_c_::set_spare4()
{
  set(types::spare4);
}
void scg_cfg_info_r12_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void scg_cfg_info_r12_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void scg_cfg_info_r12_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
void scg_cfg_info_r12_s::crit_exts_c_::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::scg_cfg_info_r12:
      j.write_fieldname("scg-ConfigInfo-r12");
      c.to_json(j);
      break;
    case types::spare7:
      break;
    case types::spare6:
      break;
    case types::spare5:
      break;
    case types::spare4:
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "scg_cfg_info_r12_s::crit_exts_c_::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE scg_cfg_info_r12_s::crit_exts_c_::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::scg_cfg_info_r12:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare7:
      break;
    case types::spare6:
      break;
    case types::spare5:
      break;
    case types::spare4:
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "scg_cfg_info_r12_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE scg_cfg_info_r12_s::crit_exts_c_::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::scg_cfg_info_r12:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare7:
      break;
    case types::spare6:
      break;
    case types::spare5:
      break;
    case types::spare4:
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "scg_cfg_info_r12_s::crit_exts_c_::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* scg_cfg_info_r12_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "scg-ConfigInfo-r12", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "scg_cfg_info_r12_s::crit_exts_c_::c1_c_::types");
}

// SL-PPPP-TxPreconfigIndex-r14 ::= SEQUENCE
SRSASN_CODE sl_pppp_tx_precfg_idx_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, prio_thres_r14, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(pack_integer(bref, default_tx_cfg_idx_r14, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(pack_integer(bref, cbr_cfg_idx_r14, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(pack_dyn_seq_of(bref, tx_cfg_idx_list_r14, 1, 16, integer_packer<uint8_t>(0, 127)));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_pppp_tx_precfg_idx_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(prio_thres_r14, bref, (uint8_t)1u, (uint8_t)8u));
  HANDLE_CODE(unpack_integer(default_tx_cfg_idx_r14, bref, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(unpack_integer(cbr_cfg_idx_r14, bref, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(unpack_dyn_seq_of(tx_cfg_idx_list_r14, bref, 1, 16, integer_packer<uint8_t>(0, 127)));

  return SRSASN_SUCCESS;
}
void sl_pppp_tx_precfg_idx_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("priorityThreshold-r14", prio_thres_r14);
  j.write_int("defaultTxConfigIndex-r14", default_tx_cfg_idx_r14);
  j.write_int("cbr-ConfigIndex-r14", cbr_cfg_idx_r14);
  j.start_array("tx-ConfigIndexList-r14");
  for (const auto& e1 : tx_cfg_idx_list_r14) {
    j.write_int(e1);
  }
  j.end_array();
  j.end_obj();
}

// SL-PPPP-TxPreconfigIndex-v1530 ::= SEQUENCE
SRSASN_CODE sl_pppp_tx_precfg_idx_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mcs_pssch_range_r15_present, 1));

  if (mcs_pssch_range_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, mcs_pssch_range_r15, 1, 16));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_pppp_tx_precfg_idx_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mcs_pssch_range_r15_present, 1));

  if (mcs_pssch_range_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(mcs_pssch_range_r15, bref, 1, 16));
  }

  return SRSASN_SUCCESS;
}
void sl_pppp_tx_precfg_idx_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mcs_pssch_range_r15_present) {
    j.start_array("mcs-PSSCH-Range-r15");
    for (const auto& e1 : mcs_pssch_range_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// SL-CBR-PreconfigTxConfigList-r14 ::= SEQUENCE
SRSASN_CODE sl_cbr_precfg_tx_cfg_list_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref,
                              cbr_range_common_cfg_list_r14,
                              1,
                              8,
                              SeqOfPacker<integer_packer<uint8_t> >(1, 16, integer_packer<uint8_t>(0, 100))));
  HANDLE_CODE(pack_dyn_seq_of(bref, sl_cbr_pssch_tx_cfg_list_r14, 1, 128));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_cbr_precfg_tx_cfg_list_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(cbr_range_common_cfg_list_r14,
                                bref,
                                1,
                                8,
                                SeqOfPacker<integer_packer<uint8_t> >(1, 16, integer_packer<uint8_t>(0, 100))));
  HANDLE_CODE(unpack_dyn_seq_of(sl_cbr_pssch_tx_cfg_list_r14, bref, 1, 128));

  return SRSASN_SUCCESS;
}
void sl_cbr_precfg_tx_cfg_list_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("cbr-RangeCommonConfigList-r14");
  for (const auto& e1 : cbr_range_common_cfg_list_r14) {
    j.start_array();
    for (const auto& e2 : e1) {
      j.write_int(e2);
    }
    j.end_array();
  }
  j.end_array();
  j.start_array("sl-CBR-PSSCH-TxConfigList-r14");
  for (const auto& e1 : sl_cbr_pssch_tx_cfg_list_r14) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// SL-PreconfigCommPool-r12 ::= SEQUENCE
SRSASN_CODE sl_precfg_comm_pool_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(sc_cp_len_r12.pack(bref));
  HANDLE_CODE(sc_period_r12.pack(bref));
  HANDLE_CODE(sc_tf_res_cfg_r12.pack(bref));
  HANDLE_CODE(pack_integer(bref, sc_tx_params_r12, (int8_t)-126, (int8_t)31));
  HANDLE_CODE(data_cp_len_r12.pack(bref));
  HANDLE_CODE(data_tf_res_cfg_r12.pack(bref));
  HANDLE_CODE(data_hop_cfg_r12.pack(bref));
  HANDLE_CODE(pack_integer(bref, data_tx_params_r12, (int8_t)-126, (int8_t)31));
  HANDLE_CODE(trpt_subset_r12.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= prio_list_r13.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(prio_list_r13.is_present(), 1));
      if (prio_list_r13.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *prio_list_r13, 1, 8, integer_packer<uint8_t>(1, 8)));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_precfg_comm_pool_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(sc_cp_len_r12.unpack(bref));
  HANDLE_CODE(sc_period_r12.unpack(bref));
  HANDLE_CODE(sc_tf_res_cfg_r12.unpack(bref));
  HANDLE_CODE(unpack_integer(sc_tx_params_r12, bref, (int8_t)-126, (int8_t)31));
  HANDLE_CODE(data_cp_len_r12.unpack(bref));
  HANDLE_CODE(data_tf_res_cfg_r12.unpack(bref));
  HANDLE_CODE(data_hop_cfg_r12.unpack(bref));
  HANDLE_CODE(unpack_integer(data_tx_params_r12, bref, (int8_t)-126, (int8_t)31));
  HANDLE_CODE(trpt_subset_r12.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool prio_list_r13_present;
      HANDLE_CODE(bref.unpack(prio_list_r13_present, 1));
      prio_list_r13.set_present(prio_list_r13_present);
      if (prio_list_r13.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*prio_list_r13, bref, 1, 8, integer_packer<uint8_t>(1, 8)));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sl_precfg_comm_pool_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sc-CP-Len-r12", sc_cp_len_r12.to_string());
  j.write_str("sc-Period-r12", sc_period_r12.to_string());
  j.write_fieldname("sc-TF-ResourceConfig-r12");
  sc_tf_res_cfg_r12.to_json(j);
  j.write_int("sc-TxParameters-r12", sc_tx_params_r12);
  j.write_str("data-CP-Len-r12", data_cp_len_r12.to_string());
  j.write_fieldname("data-TF-ResourceConfig-r12");
  data_tf_res_cfg_r12.to_json(j);
  j.write_fieldname("dataHoppingConfig-r12");
  data_hop_cfg_r12.to_json(j);
  j.write_int("dataTxParameters-r12", data_tx_params_r12);
  j.write_str("trpt-Subset-r12", trpt_subset_r12.to_string());
  if (ext) {
    if (prio_list_r13.is_present()) {
      j.start_array("priorityList-r13");
      for (const auto& e1 : *prio_list_r13) {
        j.write_int(e1);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// SL-PreconfigDiscPool-r13 ::= SEQUENCE
SRSASN_CODE sl_precfg_disc_pool_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(tx_params_r13_present, 1));

  HANDLE_CODE(cp_len_r13.pack(bref));
  HANDLE_CODE(disc_period_r13.pack(bref));
  HANDLE_CODE(pack_integer(bref, num_retx_r13, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(pack_integer(bref, num_repeat_r13, (uint8_t)1u, (uint8_t)50u));
  HANDLE_CODE(tf_res_cfg_r13.pack(bref));
  if (tx_params_r13_present) {
    HANDLE_CODE(pack_integer(bref, tx_params_r13.tx_params_general_r13, (int8_t)-126, (int8_t)31));
    HANDLE_CODE(tx_params_r13.tx_probability_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_precfg_disc_pool_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(tx_params_r13_present, 1));

  HANDLE_CODE(cp_len_r13.unpack(bref));
  HANDLE_CODE(disc_period_r13.unpack(bref));
  HANDLE_CODE(unpack_integer(num_retx_r13, bref, (uint8_t)0u, (uint8_t)3u));
  HANDLE_CODE(unpack_integer(num_repeat_r13, bref, (uint8_t)1u, (uint8_t)50u));
  HANDLE_CODE(tf_res_cfg_r13.unpack(bref));
  if (tx_params_r13_present) {
    HANDLE_CODE(unpack_integer(tx_params_r13.tx_params_general_r13, bref, (int8_t)-126, (int8_t)31));
    HANDLE_CODE(tx_params_r13.tx_probability_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void sl_precfg_disc_pool_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("cp-Len-r13", cp_len_r13.to_string());
  j.write_str("discPeriod-r13", disc_period_r13.to_string());
  j.write_int("numRetx-r13", num_retx_r13);
  j.write_int("numRepetition-r13", num_repeat_r13);
  j.write_fieldname("tf-ResourceConfig-r13");
  tf_res_cfg_r13.to_json(j);
  if (tx_params_r13_present) {
    j.write_fieldname("txParameters-r13");
    j.start_obj();
    j.write_int("txParametersGeneral-r13", tx_params_r13.tx_params_general_r13);
    j.write_str("txProbability-r13", tx_params_r13.tx_probability_r13.to_string());
    j.end_obj();
  }
  j.end_obj();
}

const char* sl_precfg_disc_pool_r13_s::disc_period_r13_opts::to_string() const
{
  static const char* options[] = {"rf4",
                                  "rf6",
                                  "rf7",
                                  "rf8",
                                  "rf12",
                                  "rf14",
                                  "rf16",
                                  "rf24",
                                  "rf28",
                                  "rf32",
                                  "rf64",
                                  "rf128",
                                  "rf256",
                                  "rf512",
                                  "rf1024",
                                  "spare"};
  return convert_enum_idx(options, 16, value, "sl_precfg_disc_pool_r13_s::disc_period_r13_e_");
}
uint16_t sl_precfg_disc_pool_r13_s::disc_period_r13_opts::to_number() const
{
  static const uint16_t options[] = {4, 6, 7, 8, 12, 14, 16, 24, 28, 32, 64, 128, 256, 512, 1024};
  return map_enum_number(options, 15, value, "sl_precfg_disc_pool_r13_s::disc_period_r13_e_");
}

const char* sl_precfg_disc_pool_r13_s::tx_params_r13_s_::tx_probability_r13_opts::to_string() const
{
  static const char* options[] = {"p25", "p50", "p75", "p100"};
  return convert_enum_idx(options, 4, value, "sl_precfg_disc_pool_r13_s::tx_params_r13_s_::tx_probability_r13_e_");
}
uint8_t sl_precfg_disc_pool_r13_s::tx_params_r13_s_::tx_probability_r13_opts::to_number() const
{
  static const uint8_t options[] = {25, 50, 75, 100};
  return map_enum_number(options, 4, value, "sl_precfg_disc_pool_r13_s::tx_params_r13_s_::tx_probability_r13_e_");
}

// SL-PreconfigGeneral-r12 ::= SEQUENCE
SRSASN_CODE sl_precfg_general_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(rohc_profiles_r12.profile0x0001_r12, 1));
  HANDLE_CODE(bref.pack(rohc_profiles_r12.profile0x0002_r12, 1));
  HANDLE_CODE(bref.pack(rohc_profiles_r12.profile0x0004_r12, 1));
  HANDLE_CODE(bref.pack(rohc_profiles_r12.profile0x0006_r12, 1));
  HANDLE_CODE(bref.pack(rohc_profiles_r12.profile0x0101_r12, 1));
  HANDLE_CODE(bref.pack(rohc_profiles_r12.profile0x0102_r12, 1));
  HANDLE_CODE(bref.pack(rohc_profiles_r12.profile0x0104_r12, 1));
  HANDLE_CODE(pack_integer(bref, carrier_freq_r12, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(pack_integer(bref, max_tx_pwr_r12, (int8_t)-30, (int8_t)33));
  HANDLE_CODE(pack_integer(bref, add_spec_emission_r12, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(sl_bw_r12.pack(bref));
  HANDLE_CODE(tdd_cfg_sl_r12.pack(bref));
  HANDLE_CODE(reserved_r12.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= add_spec_emission_v1440_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(add_spec_emission_v1440_present, 1));
      if (add_spec_emission_v1440_present) {
        HANDLE_CODE(pack_integer(bref, add_spec_emission_v1440, (uint16_t)33u, (uint16_t)288u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_precfg_general_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(rohc_profiles_r12.profile0x0001_r12, 1));
  HANDLE_CODE(bref.unpack(rohc_profiles_r12.profile0x0002_r12, 1));
  HANDLE_CODE(bref.unpack(rohc_profiles_r12.profile0x0004_r12, 1));
  HANDLE_CODE(bref.unpack(rohc_profiles_r12.profile0x0006_r12, 1));
  HANDLE_CODE(bref.unpack(rohc_profiles_r12.profile0x0101_r12, 1));
  HANDLE_CODE(bref.unpack(rohc_profiles_r12.profile0x0102_r12, 1));
  HANDLE_CODE(bref.unpack(rohc_profiles_r12.profile0x0104_r12, 1));
  HANDLE_CODE(unpack_integer(carrier_freq_r12, bref, (uint32_t)0u, (uint32_t)262143u));
  HANDLE_CODE(unpack_integer(max_tx_pwr_r12, bref, (int8_t)-30, (int8_t)33));
  HANDLE_CODE(unpack_integer(add_spec_emission_r12, bref, (uint8_t)1u, (uint8_t)32u));
  HANDLE_CODE(sl_bw_r12.unpack(bref));
  HANDLE_CODE(tdd_cfg_sl_r12.unpack(bref));
  HANDLE_CODE(reserved_r12.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(add_spec_emission_v1440_present, 1));
      if (add_spec_emission_v1440_present) {
        HANDLE_CODE(unpack_integer(add_spec_emission_v1440, bref, (uint16_t)33u, (uint16_t)288u));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sl_precfg_general_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("rohc-Profiles-r12");
  j.start_obj();
  j.write_bool("profile0x0001-r12", rohc_profiles_r12.profile0x0001_r12);
  j.write_bool("profile0x0002-r12", rohc_profiles_r12.profile0x0002_r12);
  j.write_bool("profile0x0004-r12", rohc_profiles_r12.profile0x0004_r12);
  j.write_bool("profile0x0006-r12", rohc_profiles_r12.profile0x0006_r12);
  j.write_bool("profile0x0101-r12", rohc_profiles_r12.profile0x0101_r12);
  j.write_bool("profile0x0102-r12", rohc_profiles_r12.profile0x0102_r12);
  j.write_bool("profile0x0104-r12", rohc_profiles_r12.profile0x0104_r12);
  j.end_obj();
  j.write_int("carrierFreq-r12", carrier_freq_r12);
  j.write_int("maxTxPower-r12", max_tx_pwr_r12);
  j.write_int("additionalSpectrumEmission-r12", add_spec_emission_r12);
  j.write_str("sl-bandwidth-r12", sl_bw_r12.to_string());
  j.write_fieldname("tdd-ConfigSL-r12");
  tdd_cfg_sl_r12.to_json(j);
  j.write_str("reserved-r12", reserved_r12.to_string());
  if (ext) {
    if (add_spec_emission_v1440_present) {
      j.write_int("additionalSpectrumEmission-v1440", add_spec_emission_v1440);
    }
  }
  j.end_obj();
}

const char* sl_precfg_general_r12_s::sl_bw_r12_opts::to_string() const
{
  static const char* options[] = {"n6", "n15", "n25", "n50", "n75", "n100"};
  return convert_enum_idx(options, 6, value, "sl_precfg_general_r12_s::sl_bw_r12_e_");
}
uint8_t sl_precfg_general_r12_s::sl_bw_r12_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(options, 6, value, "sl_precfg_general_r12_s::sl_bw_r12_e_");
}

// SL-PreconfigRelay-r13 ::= SEQUENCE
SRSASN_CODE sl_precfg_relay_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(resel_info_oo_c_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_precfg_relay_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(resel_info_oo_c_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void sl_precfg_relay_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("reselectionInfoOoC-r13");
  resel_info_oo_c_r13.to_json(j);
  j.end_obj();
}

// SL-PreconfigSync-r12 ::= SEQUENCE
SRSASN_CODE sl_precfg_sync_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(sync_cp_len_r12.pack(bref));
  HANDLE_CODE(pack_integer(bref, sync_offset_ind1_r12, (uint8_t)0u, (uint8_t)39u));
  HANDLE_CODE(pack_integer(bref, sync_offset_ind2_r12, (uint8_t)0u, (uint8_t)39u));
  HANDLE_CODE(pack_integer(bref, sync_tx_params_r12, (int8_t)-126, (int8_t)31));
  HANDLE_CODE(pack_integer(bref, sync_tx_thresh_oo_c_r12, (uint8_t)0u, (uint8_t)11u));
  HANDLE_CODE(filt_coef_r12.pack(bref));
  HANDLE_CODE(sync_ref_min_hyst_r12.pack(bref));
  HANDLE_CODE(sync_ref_diff_hyst_r12.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= sync_tx_periodic_r13_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sync_tx_periodic_r13_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_precfg_sync_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(sync_cp_len_r12.unpack(bref));
  HANDLE_CODE(unpack_integer(sync_offset_ind1_r12, bref, (uint8_t)0u, (uint8_t)39u));
  HANDLE_CODE(unpack_integer(sync_offset_ind2_r12, bref, (uint8_t)0u, (uint8_t)39u));
  HANDLE_CODE(unpack_integer(sync_tx_params_r12, bref, (int8_t)-126, (int8_t)31));
  HANDLE_CODE(unpack_integer(sync_tx_thresh_oo_c_r12, bref, (uint8_t)0u, (uint8_t)11u));
  HANDLE_CODE(filt_coef_r12.unpack(bref));
  HANDLE_CODE(sync_ref_min_hyst_r12.unpack(bref));
  HANDLE_CODE(sync_ref_diff_hyst_r12.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(sync_tx_periodic_r13_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void sl_precfg_sync_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("syncCP-Len-r12", sync_cp_len_r12.to_string());
  j.write_int("syncOffsetIndicator1-r12", sync_offset_ind1_r12);
  j.write_int("syncOffsetIndicator2-r12", sync_offset_ind2_r12);
  j.write_int("syncTxParameters-r12", sync_tx_params_r12);
  j.write_int("syncTxThreshOoC-r12", sync_tx_thresh_oo_c_r12);
  j.write_str("filterCoefficient-r12", filt_coef_r12.to_string());
  j.write_str("syncRefMinHyst-r12", sync_ref_min_hyst_r12.to_string());
  j.write_str("syncRefDiffHyst-r12", sync_ref_diff_hyst_r12.to_string());
  if (ext) {
    if (sync_tx_periodic_r13_present) {
      j.write_str("syncTxPeriodic-r13", "true");
    }
  }
  j.end_obj();
}

const char* sl_precfg_sync_r12_s::sync_ref_min_hyst_r12_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB3", "dB6", "dB9", "dB12"};
  return convert_enum_idx(options, 5, value, "sl_precfg_sync_r12_s::sync_ref_min_hyst_r12_e_");
}
uint8_t sl_precfg_sync_r12_s::sync_ref_min_hyst_r12_opts::to_number() const
{
  static const uint8_t options[] = {0, 3, 6, 9, 12};
  return map_enum_number(options, 5, value, "sl_precfg_sync_r12_s::sync_ref_min_hyst_r12_e_");
}

const char* sl_precfg_sync_r12_s::sync_ref_diff_hyst_r12_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB3", "dB6", "dB9", "dB12", "dBinf"};
  return convert_enum_idx(options, 6, value, "sl_precfg_sync_r12_s::sync_ref_diff_hyst_r12_e_");
}
uint8_t sl_precfg_sync_r12_s::sync_ref_diff_hyst_r12_opts::to_number() const
{
  static const uint8_t options[] = {0, 3, 6, 9, 12};
  return map_enum_number(options, 5, value, "sl_precfg_sync_r12_s::sync_ref_diff_hyst_r12_e_");
}

// SL-V2X-PreconfigCommPool-r14 ::= SEQUENCE
SRSASN_CODE sl_v2x_precfg_comm_pool_r14_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sl_offset_ind_r14_present, 1));
  HANDLE_CODE(bref.pack(start_rb_pscch_pool_r14_present, 1));
  HANDLE_CODE(bref.pack(zone_id_r14_present, 1));
  HANDLE_CODE(bref.pack(thresh_s_rssi_cbr_r14_present, 1));
  HANDLE_CODE(bref.pack(cbr_pssch_tx_cfg_list_r14_present, 1));
  HANDLE_CODE(bref.pack(res_sel_cfg_p2_x_r14_present, 1));
  HANDLE_CODE(bref.pack(sync_allowed_r14_present, 1));
  HANDLE_CODE(bref.pack(restrict_res_reserv_period_r14_present, 1));

  if (sl_offset_ind_r14_present) {
    HANDLE_CODE(sl_offset_ind_r14.pack(bref));
  }
  HANDLE_CODE(sl_sf_r14.pack(bref));
  HANDLE_CODE(bref.pack(adjacency_pscch_pssch_r14, 1));
  HANDLE_CODE(size_subch_r14.pack(bref));
  HANDLE_CODE(num_subch_r14.pack(bref));
  HANDLE_CODE(pack_integer(bref, start_rb_subch_r14, (uint8_t)0u, (uint8_t)99u));
  if (start_rb_pscch_pool_r14_present) {
    HANDLE_CODE(pack_integer(bref, start_rb_pscch_pool_r14, (uint8_t)0u, (uint8_t)99u));
  }
  HANDLE_CODE(pack_integer(bref, data_tx_params_r14, (int8_t)-126, (int8_t)31));
  if (zone_id_r14_present) {
    HANDLE_CODE(pack_integer(bref, zone_id_r14, (uint8_t)0u, (uint8_t)7u));
  }
  if (thresh_s_rssi_cbr_r14_present) {
    HANDLE_CODE(pack_integer(bref, thresh_s_rssi_cbr_r14, (uint8_t)0u, (uint8_t)45u));
  }
  if (cbr_pssch_tx_cfg_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cbr_pssch_tx_cfg_list_r14, 1, 8));
  }
  if (res_sel_cfg_p2_x_r14_present) {
    HANDLE_CODE(res_sel_cfg_p2_x_r14.pack(bref));
  }
  if (sync_allowed_r14_present) {
    HANDLE_CODE(sync_allowed_r14.pack(bref));
  }
  if (restrict_res_reserv_period_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, restrict_res_reserv_period_r14, 1, 16));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= sl_min_t2_value_list_r15.is_present();
    group_flags[0] |= cbr_pssch_tx_cfg_list_v1530.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(sl_min_t2_value_list_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(cbr_pssch_tx_cfg_list_v1530.is_present(), 1));
      if (sl_min_t2_value_list_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *sl_min_t2_value_list_r15, 1, 8));
      }
      if (cbr_pssch_tx_cfg_list_v1530.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *cbr_pssch_tx_cfg_list_v1530, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_v2x_precfg_comm_pool_r14_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sl_offset_ind_r14_present, 1));
  HANDLE_CODE(bref.unpack(start_rb_pscch_pool_r14_present, 1));
  HANDLE_CODE(bref.unpack(zone_id_r14_present, 1));
  HANDLE_CODE(bref.unpack(thresh_s_rssi_cbr_r14_present, 1));
  HANDLE_CODE(bref.unpack(cbr_pssch_tx_cfg_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(res_sel_cfg_p2_x_r14_present, 1));
  HANDLE_CODE(bref.unpack(sync_allowed_r14_present, 1));
  HANDLE_CODE(bref.unpack(restrict_res_reserv_period_r14_present, 1));

  if (sl_offset_ind_r14_present) {
    HANDLE_CODE(sl_offset_ind_r14.unpack(bref));
  }
  HANDLE_CODE(sl_sf_r14.unpack(bref));
  HANDLE_CODE(bref.unpack(adjacency_pscch_pssch_r14, 1));
  HANDLE_CODE(size_subch_r14.unpack(bref));
  HANDLE_CODE(num_subch_r14.unpack(bref));
  HANDLE_CODE(unpack_integer(start_rb_subch_r14, bref, (uint8_t)0u, (uint8_t)99u));
  if (start_rb_pscch_pool_r14_present) {
    HANDLE_CODE(unpack_integer(start_rb_pscch_pool_r14, bref, (uint8_t)0u, (uint8_t)99u));
  }
  HANDLE_CODE(unpack_integer(data_tx_params_r14, bref, (int8_t)-126, (int8_t)31));
  if (zone_id_r14_present) {
    HANDLE_CODE(unpack_integer(zone_id_r14, bref, (uint8_t)0u, (uint8_t)7u));
  }
  if (thresh_s_rssi_cbr_r14_present) {
    HANDLE_CODE(unpack_integer(thresh_s_rssi_cbr_r14, bref, (uint8_t)0u, (uint8_t)45u));
  }
  if (cbr_pssch_tx_cfg_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cbr_pssch_tx_cfg_list_r14, bref, 1, 8));
  }
  if (res_sel_cfg_p2_x_r14_present) {
    HANDLE_CODE(res_sel_cfg_p2_x_r14.unpack(bref));
  }
  if (sync_allowed_r14_present) {
    HANDLE_CODE(sync_allowed_r14.unpack(bref));
  }
  if (restrict_res_reserv_period_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(restrict_res_reserv_period_r14, bref, 1, 16));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool sl_min_t2_value_list_r15_present;
      HANDLE_CODE(bref.unpack(sl_min_t2_value_list_r15_present, 1));
      sl_min_t2_value_list_r15.set_present(sl_min_t2_value_list_r15_present);
      bool cbr_pssch_tx_cfg_list_v1530_present;
      HANDLE_CODE(bref.unpack(cbr_pssch_tx_cfg_list_v1530_present, 1));
      cbr_pssch_tx_cfg_list_v1530.set_present(cbr_pssch_tx_cfg_list_v1530_present);
      if (sl_min_t2_value_list_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*sl_min_t2_value_list_r15, bref, 1, 8));
      }
      if (cbr_pssch_tx_cfg_list_v1530.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*cbr_pssch_tx_cfg_list_v1530, bref, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sl_v2x_precfg_comm_pool_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (sl_offset_ind_r14_present) {
    j.write_fieldname("sl-OffsetIndicator-r14");
    sl_offset_ind_r14.to_json(j);
  }
  j.write_fieldname("sl-Subframe-r14");
  sl_sf_r14.to_json(j);
  j.write_bool("adjacencyPSCCH-PSSCH-r14", adjacency_pscch_pssch_r14);
  j.write_str("sizeSubchannel-r14", size_subch_r14.to_string());
  j.write_str("numSubchannel-r14", num_subch_r14.to_string());
  j.write_int("startRB-Subchannel-r14", start_rb_subch_r14);
  if (start_rb_pscch_pool_r14_present) {
    j.write_int("startRB-PSCCH-Pool-r14", start_rb_pscch_pool_r14);
  }
  j.write_int("dataTxParameters-r14", data_tx_params_r14);
  if (zone_id_r14_present) {
    j.write_int("zoneID-r14", zone_id_r14);
  }
  if (thresh_s_rssi_cbr_r14_present) {
    j.write_int("threshS-RSSI-CBR-r14", thresh_s_rssi_cbr_r14);
  }
  if (cbr_pssch_tx_cfg_list_r14_present) {
    j.start_array("cbr-pssch-TxConfigList-r14");
    for (const auto& e1 : cbr_pssch_tx_cfg_list_r14) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (res_sel_cfg_p2_x_r14_present) {
    j.write_fieldname("resourceSelectionConfigP2X-r14");
    res_sel_cfg_p2_x_r14.to_json(j);
  }
  if (sync_allowed_r14_present) {
    j.write_fieldname("syncAllowed-r14");
    sync_allowed_r14.to_json(j);
  }
  if (restrict_res_reserv_period_r14_present) {
    j.start_array("restrictResourceReservationPeriod-r14");
    for (const auto& e1 : restrict_res_reserv_period_r14) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  if (ext) {
    if (sl_min_t2_value_list_r15.is_present()) {
      j.start_array("sl-MinT2ValueList-r15");
      for (const auto& e1 : *sl_min_t2_value_list_r15) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (cbr_pssch_tx_cfg_list_v1530.is_present()) {
      j.start_array("cbr-pssch-TxConfigList-v1530");
      for (const auto& e1 : *cbr_pssch_tx_cfg_list_v1530) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

const char* sl_v2x_precfg_comm_pool_r14_s::size_subch_r14_opts::to_string() const
{
  static const char* options[] = {"n4",     "n5",     "n6",     "n8",      "n9",      "n10",     "n12",     "n15",
                                  "n16",    "n18",    "n20",    "n25",     "n30",     "n48",     "n50",     "n72",
                                  "n75",    "n96",    "n100",   "spare13", "spare12", "spare11", "spare10", "spare9",
                                  "spare8", "spare7", "spare6", "spare5",  "spare4",  "spare3",  "spare2",  "spare1"};
  return convert_enum_idx(options, 32, value, "sl_v2x_precfg_comm_pool_r14_s::size_subch_r14_e_");
}
uint8_t sl_v2x_precfg_comm_pool_r14_s::size_subch_r14_opts::to_number() const
{
  static const uint8_t options[] = {4, 5, 6, 8, 9, 10, 12, 15, 16, 18, 20, 25, 30, 48, 50, 72, 75, 96, 100};
  return map_enum_number(options, 19, value, "sl_v2x_precfg_comm_pool_r14_s::size_subch_r14_e_");
}

const char* sl_v2x_precfg_comm_pool_r14_s::num_subch_r14_opts::to_string() const
{
  static const char* options[] = {"n1", "n3", "n5", "n8", "n10", "n15", "n20", "spare1"};
  return convert_enum_idx(options, 8, value, "sl_v2x_precfg_comm_pool_r14_s::num_subch_r14_e_");
}
uint8_t sl_v2x_precfg_comm_pool_r14_s::num_subch_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 3, 5, 8, 10, 15, 20};
  return map_enum_number(options, 7, value, "sl_v2x_precfg_comm_pool_r14_s::num_subch_r14_e_");
}

// SL-V2X-SyncOffsetIndicators-r14 ::= SEQUENCE
SRSASN_CODE sl_v2x_sync_offset_inds_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(sync_offset_ind3_r14_present, 1));

  HANDLE_CODE(pack_integer(bref, sync_offset_ind1_r14, (uint8_t)0u, (uint8_t)159u));
  HANDLE_CODE(pack_integer(bref, sync_offset_ind2_r14, (uint8_t)0u, (uint8_t)159u));
  if (sync_offset_ind3_r14_present) {
    HANDLE_CODE(pack_integer(bref, sync_offset_ind3_r14, (uint8_t)0u, (uint8_t)159u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_v2x_sync_offset_inds_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(sync_offset_ind3_r14_present, 1));

  HANDLE_CODE(unpack_integer(sync_offset_ind1_r14, bref, (uint8_t)0u, (uint8_t)159u));
  HANDLE_CODE(unpack_integer(sync_offset_ind2_r14, bref, (uint8_t)0u, (uint8_t)159u));
  if (sync_offset_ind3_r14_present) {
    HANDLE_CODE(unpack_integer(sync_offset_ind3_r14, bref, (uint8_t)0u, (uint8_t)159u));
  }

  return SRSASN_SUCCESS;
}
void sl_v2x_sync_offset_inds_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("syncOffsetIndicator1-r14", sync_offset_ind1_r14);
  j.write_int("syncOffsetIndicator2-r14", sync_offset_ind2_r14);
  if (sync_offset_ind3_r14_present) {
    j.write_int("syncOffsetIndicator3-r14", sync_offset_ind3_r14);
  }
  j.end_obj();
}

// SL-PreconfigV2X-Sync-r14 ::= SEQUENCE
SRSASN_CODE sl_precfg_v2x_sync_r14_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(sync_offset_inds_r14.pack(bref));
  HANDLE_CODE(pack_integer(bref, sync_tx_params_r14, (int8_t)-126, (int8_t)31));
  HANDLE_CODE(pack_integer(bref, sync_tx_thresh_oo_c_r14, (uint8_t)0u, (uint8_t)11u));
  HANDLE_CODE(filt_coef_r14.pack(bref));
  HANDLE_CODE(sync_ref_min_hyst_r14.pack(bref));
  HANDLE_CODE(sync_ref_diff_hyst_r14.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= slss_tx_disabled_r15_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(slss_tx_disabled_r15_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_precfg_v2x_sync_r14_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(sync_offset_inds_r14.unpack(bref));
  HANDLE_CODE(unpack_integer(sync_tx_params_r14, bref, (int8_t)-126, (int8_t)31));
  HANDLE_CODE(unpack_integer(sync_tx_thresh_oo_c_r14, bref, (uint8_t)0u, (uint8_t)11u));
  HANDLE_CODE(filt_coef_r14.unpack(bref));
  HANDLE_CODE(sync_ref_min_hyst_r14.unpack(bref));
  HANDLE_CODE(sync_ref_diff_hyst_r14.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(slss_tx_disabled_r15_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void sl_precfg_v2x_sync_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("syncOffsetIndicators-r14");
  sync_offset_inds_r14.to_json(j);
  j.write_int("syncTxParameters-r14", sync_tx_params_r14);
  j.write_int("syncTxThreshOoC-r14", sync_tx_thresh_oo_c_r14);
  j.write_str("filterCoefficient-r14", filt_coef_r14.to_string());
  j.write_str("syncRefMinHyst-r14", sync_ref_min_hyst_r14.to_string());
  j.write_str("syncRefDiffHyst-r14", sync_ref_diff_hyst_r14.to_string());
  if (ext) {
    if (slss_tx_disabled_r15_present) {
      j.write_str("slss-TxDisabled-r15", "true");
    }
  }
  j.end_obj();
}

const char* sl_precfg_v2x_sync_r14_s::sync_ref_min_hyst_r14_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB3", "dB6", "dB9", "dB12"};
  return convert_enum_idx(options, 5, value, "sl_precfg_v2x_sync_r14_s::sync_ref_min_hyst_r14_e_");
}
uint8_t sl_precfg_v2x_sync_r14_s::sync_ref_min_hyst_r14_opts::to_number() const
{
  static const uint8_t options[] = {0, 3, 6, 9, 12};
  return map_enum_number(options, 5, value, "sl_precfg_v2x_sync_r14_s::sync_ref_min_hyst_r14_e_");
}

const char* sl_precfg_v2x_sync_r14_s::sync_ref_diff_hyst_r14_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB3", "dB6", "dB9", "dB12", "dBinf"};
  return convert_enum_idx(options, 6, value, "sl_precfg_v2x_sync_r14_s::sync_ref_diff_hyst_r14_e_");
}
uint8_t sl_precfg_v2x_sync_r14_s::sync_ref_diff_hyst_r14_opts::to_number() const
{
  static const uint8_t options[] = {0, 3, 6, 9, 12};
  return map_enum_number(options, 5, value, "sl_precfg_v2x_sync_r14_s::sync_ref_diff_hyst_r14_e_");
}

// SL-Preconfiguration-r12 ::= SEQUENCE
SRSASN_CODE sl_precfg_r12_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(precfg_general_r12.pack(bref));
  HANDLE_CODE(precfg_sync_r12.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, precfg_comm_r12, 1, 4));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= precfg_comm_v1310.is_present();
    group_flags[0] |= precfg_disc_r13.is_present();
    group_flags[0] |= precfg_relay_r13.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(precfg_comm_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(precfg_disc_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(precfg_relay_r13.is_present(), 1));
      if (precfg_comm_v1310.is_present()) {
        HANDLE_CODE(bref.pack(precfg_comm_v1310->comm_tx_pool_list_r13_present, 1));
        HANDLE_CODE(pack_dyn_seq_of(bref, precfg_comm_v1310->comm_rx_pool_list_r13, 1, 12));
        if (precfg_comm_v1310->comm_tx_pool_list_r13_present) {
          HANDLE_CODE(pack_dyn_seq_of(bref, precfg_comm_v1310->comm_tx_pool_list_r13, 1, 7));
        }
      }
      if (precfg_disc_r13.is_present()) {
        HANDLE_CODE(bref.pack(precfg_disc_r13->disc_tx_pool_list_r13_present, 1));
        HANDLE_CODE(pack_dyn_seq_of(bref, precfg_disc_r13->disc_rx_pool_list_r13, 1, 16));
        if (precfg_disc_r13->disc_tx_pool_list_r13_present) {
          HANDLE_CODE(pack_dyn_seq_of(bref, precfg_disc_r13->disc_tx_pool_list_r13, 1, 4));
        }
      }
      if (precfg_relay_r13.is_present()) {
        HANDLE_CODE(precfg_relay_r13->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_precfg_r12_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(precfg_general_r12.unpack(bref));
  HANDLE_CODE(precfg_sync_r12.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(precfg_comm_r12, bref, 1, 4));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool precfg_comm_v1310_present;
      HANDLE_CODE(bref.unpack(precfg_comm_v1310_present, 1));
      precfg_comm_v1310.set_present(precfg_comm_v1310_present);
      bool precfg_disc_r13_present;
      HANDLE_CODE(bref.unpack(precfg_disc_r13_present, 1));
      precfg_disc_r13.set_present(precfg_disc_r13_present);
      bool precfg_relay_r13_present;
      HANDLE_CODE(bref.unpack(precfg_relay_r13_present, 1));
      precfg_relay_r13.set_present(precfg_relay_r13_present);
      if (precfg_comm_v1310.is_present()) {
        HANDLE_CODE(bref.unpack(precfg_comm_v1310->comm_tx_pool_list_r13_present, 1));
        HANDLE_CODE(unpack_dyn_seq_of(precfg_comm_v1310->comm_rx_pool_list_r13, bref, 1, 12));
        if (precfg_comm_v1310->comm_tx_pool_list_r13_present) {
          HANDLE_CODE(unpack_dyn_seq_of(precfg_comm_v1310->comm_tx_pool_list_r13, bref, 1, 7));
        }
      }
      if (precfg_disc_r13.is_present()) {
        HANDLE_CODE(bref.unpack(precfg_disc_r13->disc_tx_pool_list_r13_present, 1));
        HANDLE_CODE(unpack_dyn_seq_of(precfg_disc_r13->disc_rx_pool_list_r13, bref, 1, 16));
        if (precfg_disc_r13->disc_tx_pool_list_r13_present) {
          HANDLE_CODE(unpack_dyn_seq_of(precfg_disc_r13->disc_tx_pool_list_r13, bref, 1, 4));
        }
      }
      if (precfg_relay_r13.is_present()) {
        HANDLE_CODE(precfg_relay_r13->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sl_precfg_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("preconfigGeneral-r12");
  precfg_general_r12.to_json(j);
  j.write_fieldname("preconfigSync-r12");
  precfg_sync_r12.to_json(j);
  j.start_array("preconfigComm-r12");
  for (const auto& e1 : precfg_comm_r12) {
    e1.to_json(j);
  }
  j.end_array();
  if (ext) {
    if (precfg_comm_v1310.is_present()) {
      j.write_fieldname("preconfigComm-v1310");
      j.start_obj();
      j.start_array("commRxPoolList-r13");
      for (const auto& e1 : precfg_comm_v1310->comm_rx_pool_list_r13) {
        e1.to_json(j);
      }
      j.end_array();
      if (precfg_comm_v1310->comm_tx_pool_list_r13_present) {
        j.start_array("commTxPoolList-r13");
        for (const auto& e1 : precfg_comm_v1310->comm_tx_pool_list_r13) {
          e1.to_json(j);
        }
        j.end_array();
      }
      j.end_obj();
    }
    if (precfg_disc_r13.is_present()) {
      j.write_fieldname("preconfigDisc-r13");
      j.start_obj();
      j.start_array("discRxPoolList-r13");
      for (const auto& e1 : precfg_disc_r13->disc_rx_pool_list_r13) {
        e1.to_json(j);
      }
      j.end_array();
      if (precfg_disc_r13->disc_tx_pool_list_r13_present) {
        j.start_array("discTxPoolList-r13");
        for (const auto& e1 : precfg_disc_r13->disc_tx_pool_list_r13) {
          e1.to_json(j);
        }
        j.end_array();
      }
      j.end_obj();
    }
    if (precfg_relay_r13.is_present()) {
      j.write_fieldname("preconfigRelay-r13");
      precfg_relay_r13->to_json(j);
    }
  }
  j.end_obj();
}

// SL-V2X-PreconfigFreqInfo-r14 ::= SEQUENCE
SRSASN_CODE sl_v2x_precfg_freq_info_r14_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(v2x_comm_precfg_sync_r14_present, 1));
  HANDLE_CODE(bref.pack(v2x_res_sel_cfg_r14_present, 1));
  HANDLE_CODE(bref.pack(zone_cfg_r14_present, 1));
  HANDLE_CODE(bref.pack(thres_sl_tx_prioritization_r14_present, 1));
  HANDLE_CODE(bref.pack(offset_dfn_r14_present, 1));

  HANDLE_CODE(v2x_comm_precfg_general_r14.pack(bref));
  if (v2x_comm_precfg_sync_r14_present) {
    HANDLE_CODE(v2x_comm_precfg_sync_r14.pack(bref));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, v2x_comm_rx_pool_list_r14, 1, 16));
  HANDLE_CODE(pack_dyn_seq_of(bref, v2x_comm_tx_pool_list_r14, 1, 8));
  HANDLE_CODE(pack_dyn_seq_of(bref, p2x_comm_tx_pool_list_r14, 1, 8));
  if (v2x_res_sel_cfg_r14_present) {
    HANDLE_CODE(v2x_res_sel_cfg_r14.pack(bref));
  }
  if (zone_cfg_r14_present) {
    HANDLE_CODE(zone_cfg_r14.pack(bref));
  }
  HANDLE_CODE(sync_prio_r14.pack(bref));
  if (thres_sl_tx_prioritization_r14_present) {
    HANDLE_CODE(pack_integer(bref, thres_sl_tx_prioritization_r14, (uint8_t)1u, (uint8_t)8u));
  }
  if (offset_dfn_r14_present) {
    HANDLE_CODE(pack_integer(bref, offset_dfn_r14, (uint16_t)0u, (uint16_t)1000u));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= v2x_freq_sel_cfg_list_r15.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(v2x_freq_sel_cfg_list_r15.is_present(), 1));
      if (v2x_freq_sel_cfg_list_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *v2x_freq_sel_cfg_list_r15, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_v2x_precfg_freq_info_r14_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(v2x_comm_precfg_sync_r14_present, 1));
  HANDLE_CODE(bref.unpack(v2x_res_sel_cfg_r14_present, 1));
  HANDLE_CODE(bref.unpack(zone_cfg_r14_present, 1));
  HANDLE_CODE(bref.unpack(thres_sl_tx_prioritization_r14_present, 1));
  HANDLE_CODE(bref.unpack(offset_dfn_r14_present, 1));

  HANDLE_CODE(v2x_comm_precfg_general_r14.unpack(bref));
  if (v2x_comm_precfg_sync_r14_present) {
    HANDLE_CODE(v2x_comm_precfg_sync_r14.unpack(bref));
  }
  HANDLE_CODE(unpack_dyn_seq_of(v2x_comm_rx_pool_list_r14, bref, 1, 16));
  HANDLE_CODE(unpack_dyn_seq_of(v2x_comm_tx_pool_list_r14, bref, 1, 8));
  HANDLE_CODE(unpack_dyn_seq_of(p2x_comm_tx_pool_list_r14, bref, 1, 8));
  if (v2x_res_sel_cfg_r14_present) {
    HANDLE_CODE(v2x_res_sel_cfg_r14.unpack(bref));
  }
  if (zone_cfg_r14_present) {
    HANDLE_CODE(zone_cfg_r14.unpack(bref));
  }
  HANDLE_CODE(sync_prio_r14.unpack(bref));
  if (thres_sl_tx_prioritization_r14_present) {
    HANDLE_CODE(unpack_integer(thres_sl_tx_prioritization_r14, bref, (uint8_t)1u, (uint8_t)8u));
  }
  if (offset_dfn_r14_present) {
    HANDLE_CODE(unpack_integer(offset_dfn_r14, bref, (uint16_t)0u, (uint16_t)1000u));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool v2x_freq_sel_cfg_list_r15_present;
      HANDLE_CODE(bref.unpack(v2x_freq_sel_cfg_list_r15_present, 1));
      v2x_freq_sel_cfg_list_r15.set_present(v2x_freq_sel_cfg_list_r15_present);
      if (v2x_freq_sel_cfg_list_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*v2x_freq_sel_cfg_list_r15, bref, 1, 8));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sl_v2x_precfg_freq_info_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("v2x-CommPreconfigGeneral-r14");
  v2x_comm_precfg_general_r14.to_json(j);
  if (v2x_comm_precfg_sync_r14_present) {
    j.write_fieldname("v2x-CommPreconfigSync-r14");
    v2x_comm_precfg_sync_r14.to_json(j);
  }
  j.start_array("v2x-CommRxPoolList-r14");
  for (const auto& e1 : v2x_comm_rx_pool_list_r14) {
    e1.to_json(j);
  }
  j.end_array();
  j.start_array("v2x-CommTxPoolList-r14");
  for (const auto& e1 : v2x_comm_tx_pool_list_r14) {
    e1.to_json(j);
  }
  j.end_array();
  j.start_array("p2x-CommTxPoolList-r14");
  for (const auto& e1 : p2x_comm_tx_pool_list_r14) {
    e1.to_json(j);
  }
  j.end_array();
  if (v2x_res_sel_cfg_r14_present) {
    j.write_fieldname("v2x-ResourceSelectionConfig-r14");
    v2x_res_sel_cfg_r14.to_json(j);
  }
  if (zone_cfg_r14_present) {
    j.write_fieldname("zoneConfig-r14");
    zone_cfg_r14.to_json(j);
  }
  j.write_str("syncPriority-r14", sync_prio_r14.to_string());
  if (thres_sl_tx_prioritization_r14_present) {
    j.write_int("thresSL-TxPrioritization-r14", thres_sl_tx_prioritization_r14);
  }
  if (offset_dfn_r14_present) {
    j.write_int("offsetDFN-r14", offset_dfn_r14);
  }
  if (ext) {
    if (v2x_freq_sel_cfg_list_r15.is_present()) {
      j.start_array("v2x-FreqSelectionConfigList-r15");
      for (const auto& e1 : *v2x_freq_sel_cfg_list_r15) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

const char* sl_v2x_precfg_freq_info_r14_s::sync_prio_r14_opts::to_string() const
{
  static const char* options[] = {"gnss", "enb"};
  return convert_enum_idx(options, 2, value, "sl_v2x_precfg_freq_info_r14_s::sync_prio_r14_e_");
}

// SL-V2X-TxProfile-r15 ::= ENUMERATED
const char* sl_v2x_tx_profile_r15_opts::to_string() const
{
  static const char* options[] = {"rel14", "rel15", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "sl_v2x_tx_profile_r15_e");
}
uint8_t sl_v2x_tx_profile_r15_opts::to_number() const
{
  static const uint8_t options[] = {14, 15};
  return map_enum_number(options, 2, value, "sl_v2x_tx_profile_r15_e");
}

// SL-V2X-Preconfiguration-r14 ::= SEQUENCE
SRSASN_CODE sl_v2x_precfg_r14_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(anchor_carrier_freq_list_r14_present, 1));
  HANDLE_CODE(bref.pack(cbr_precfg_list_r14_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, v2x_precfg_freq_list_r14, 1, 8));
  if (anchor_carrier_freq_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, anchor_carrier_freq_list_r14, 1, 8, integer_packer<uint32_t>(0, 262143)));
  }
  if (cbr_precfg_list_r14_present) {
    HANDLE_CODE(cbr_precfg_list_r14.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= v2x_packet_dupl_cfg_r15.is_present();
    group_flags[0] |= sync_freq_list_r15.is_present();
    group_flags[0] |= slss_tx_multi_freq_r15_present;
    group_flags[0] |= v2x_tx_profile_list_r15.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(v2x_packet_dupl_cfg_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(sync_freq_list_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(slss_tx_multi_freq_r15_present, 1));
      HANDLE_CODE(bref.pack(v2x_tx_profile_list_r15.is_present(), 1));
      if (v2x_packet_dupl_cfg_r15.is_present()) {
        HANDLE_CODE(v2x_packet_dupl_cfg_r15->pack(bref));
      }
      if (sync_freq_list_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *sync_freq_list_r15, 1, 8, integer_packer<uint32_t>(0, 262143)));
      }
      if (v2x_tx_profile_list_r15.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *v2x_tx_profile_list_r15, 1, 256));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE sl_v2x_precfg_r14_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(anchor_carrier_freq_list_r14_present, 1));
  HANDLE_CODE(bref.unpack(cbr_precfg_list_r14_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(v2x_precfg_freq_list_r14, bref, 1, 8));
  if (anchor_carrier_freq_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(anchor_carrier_freq_list_r14, bref, 1, 8, integer_packer<uint32_t>(0, 262143)));
  }
  if (cbr_precfg_list_r14_present) {
    HANDLE_CODE(cbr_precfg_list_r14.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool v2x_packet_dupl_cfg_r15_present;
      HANDLE_CODE(bref.unpack(v2x_packet_dupl_cfg_r15_present, 1));
      v2x_packet_dupl_cfg_r15.set_present(v2x_packet_dupl_cfg_r15_present);
      bool sync_freq_list_r15_present;
      HANDLE_CODE(bref.unpack(sync_freq_list_r15_present, 1));
      sync_freq_list_r15.set_present(sync_freq_list_r15_present);
      HANDLE_CODE(bref.unpack(slss_tx_multi_freq_r15_present, 1));
      bool v2x_tx_profile_list_r15_present;
      HANDLE_CODE(bref.unpack(v2x_tx_profile_list_r15_present, 1));
      v2x_tx_profile_list_r15.set_present(v2x_tx_profile_list_r15_present);
      if (v2x_packet_dupl_cfg_r15.is_present()) {
        HANDLE_CODE(v2x_packet_dupl_cfg_r15->unpack(bref));
      }
      if (sync_freq_list_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*sync_freq_list_r15, bref, 1, 8, integer_packer<uint32_t>(0, 262143)));
      }
      if (v2x_tx_profile_list_r15.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*v2x_tx_profile_list_r15, bref, 1, 256));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void sl_v2x_precfg_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("v2x-PreconfigFreqList-r14");
  for (const auto& e1 : v2x_precfg_freq_list_r14) {
    e1.to_json(j);
  }
  j.end_array();
  if (anchor_carrier_freq_list_r14_present) {
    j.start_array("anchorCarrierFreqList-r14");
    for (const auto& e1 : anchor_carrier_freq_list_r14) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (cbr_precfg_list_r14_present) {
    j.write_fieldname("cbr-PreconfigList-r14");
    cbr_precfg_list_r14.to_json(j);
  }
  if (ext) {
    if (v2x_packet_dupl_cfg_r15.is_present()) {
      j.write_fieldname("v2x-PacketDuplicationConfig-r15");
      v2x_packet_dupl_cfg_r15->to_json(j);
    }
    if (sync_freq_list_r15.is_present()) {
      j.start_array("syncFreqList-r15");
      for (const auto& e1 : *sync_freq_list_r15) {
        j.write_int(e1);
      }
      j.end_array();
    }
    if (slss_tx_multi_freq_r15_present) {
      j.write_str("slss-TxMultiFreq-r15", "true");
    }
    if (v2x_tx_profile_list_r15.is_present()) {
      j.start_array("v2x-TxProfileList-r15");
      for (const auto& e1 : *v2x_tx_profile_list_r15) {
        j.write_str(e1.to_string());
      }
      j.end_array();
    }
  }
  j.end_obj();
}

const char* ue_paging_coverage_info_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "uePagingCoverageInformation-r13", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "ue_paging_coverage_info_s::crit_exts_c_::c1_c_::types");
}

const char* ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "ueRadioAccessCapabilityInformation-r8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::types");
}

const char* ue_radio_paging_info_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "ueRadioPagingInformation-r12", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "ue_radio_paging_info_s::crit_exts_c_::c1_c_::types");
}

// VarConnEstFailReport-r11 ::= SEQUENCE
SRSASN_CODE var_conn_est_fail_report_r11_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(conn_est_fail_report_r11.pack(bref));
  HANDLE_CODE(plmn_id_r11.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_conn_est_fail_report_r11_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(conn_est_fail_report_r11.unpack(bref));
  HANDLE_CODE(plmn_id_r11.unpack(bref));

  return SRSASN_SUCCESS;
}
void var_conn_est_fail_report_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("connEstFailReport-r11");
  conn_est_fail_report_r11.to_json(j);
  j.write_fieldname("plmn-Identity-r11");
  plmn_id_r11.to_json(j);
  j.end_obj();
}

// VarLogMeasConfig-r10 ::= SEQUENCE
SRSASN_CODE var_log_meas_cfg_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(area_cfg_r10_present, 1));

  if (area_cfg_r10_present) {
    HANDLE_CODE(area_cfg_r10.pack(bref));
  }
  HANDLE_CODE(logging_dur_r10.pack(bref));
  HANDLE_CODE(logging_interv_r10.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_log_meas_cfg_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(area_cfg_r10_present, 1));

  if (area_cfg_r10_present) {
    HANDLE_CODE(area_cfg_r10.unpack(bref));
  }
  HANDLE_CODE(logging_dur_r10.unpack(bref));
  HANDLE_CODE(logging_interv_r10.unpack(bref));

  return SRSASN_SUCCESS;
}
void var_log_meas_cfg_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (area_cfg_r10_present) {
    j.write_fieldname("areaConfiguration-r10");
    area_cfg_r10.to_json(j);
  }
  j.write_str("loggingDuration-r10", logging_dur_r10.to_string());
  j.write_str("loggingInterval-r10", logging_interv_r10.to_string());
  j.end_obj();
}

// VarLogMeasConfig-r11 ::= SEQUENCE
SRSASN_CODE var_log_meas_cfg_r11_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(area_cfg_r10_present, 1));
  HANDLE_CODE(bref.pack(area_cfg_v1130_present, 1));

  if (area_cfg_r10_present) {
    HANDLE_CODE(area_cfg_r10.pack(bref));
  }
  if (area_cfg_v1130_present) {
    HANDLE_CODE(area_cfg_v1130.pack(bref));
  }
  HANDLE_CODE(logging_dur_r10.pack(bref));
  HANDLE_CODE(logging_interv_r10.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_log_meas_cfg_r11_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(area_cfg_r10_present, 1));
  HANDLE_CODE(bref.unpack(area_cfg_v1130_present, 1));

  if (area_cfg_r10_present) {
    HANDLE_CODE(area_cfg_r10.unpack(bref));
  }
  if (area_cfg_v1130_present) {
    HANDLE_CODE(area_cfg_v1130.unpack(bref));
  }
  HANDLE_CODE(logging_dur_r10.unpack(bref));
  HANDLE_CODE(logging_interv_r10.unpack(bref));

  return SRSASN_SUCCESS;
}
void var_log_meas_cfg_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (area_cfg_r10_present) {
    j.write_fieldname("areaConfiguration-r10");
    area_cfg_r10.to_json(j);
  }
  if (area_cfg_v1130_present) {
    j.write_fieldname("areaConfiguration-v1130");
    area_cfg_v1130.to_json(j);
  }
  j.write_str("loggingDuration-r10", logging_dur_r10.to_string());
  j.write_str("loggingInterval-r10", logging_interv_r10.to_string());
  j.end_obj();
}

// VarLogMeasConfig-r12 ::= SEQUENCE
SRSASN_CODE var_log_meas_cfg_r12_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(area_cfg_r10_present, 1));
  HANDLE_CODE(bref.pack(area_cfg_v1130_present, 1));
  HANDLE_CODE(bref.pack(target_mbsfn_area_list_r12_present, 1));

  if (area_cfg_r10_present) {
    HANDLE_CODE(area_cfg_r10.pack(bref));
  }
  if (area_cfg_v1130_present) {
    HANDLE_CODE(area_cfg_v1130.pack(bref));
  }
  HANDLE_CODE(logging_dur_r10.pack(bref));
  HANDLE_CODE(logging_interv_r10.pack(bref));
  if (target_mbsfn_area_list_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, target_mbsfn_area_list_r12, 0, 8));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_log_meas_cfg_r12_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(area_cfg_r10_present, 1));
  HANDLE_CODE(bref.unpack(area_cfg_v1130_present, 1));
  HANDLE_CODE(bref.unpack(target_mbsfn_area_list_r12_present, 1));

  if (area_cfg_r10_present) {
    HANDLE_CODE(area_cfg_r10.unpack(bref));
  }
  if (area_cfg_v1130_present) {
    HANDLE_CODE(area_cfg_v1130.unpack(bref));
  }
  HANDLE_CODE(logging_dur_r10.unpack(bref));
  HANDLE_CODE(logging_interv_r10.unpack(bref));
  if (target_mbsfn_area_list_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(target_mbsfn_area_list_r12, bref, 0, 8));
  }

  return SRSASN_SUCCESS;
}
void var_log_meas_cfg_r12_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (area_cfg_r10_present) {
    j.write_fieldname("areaConfiguration-r10");
    area_cfg_r10.to_json(j);
  }
  if (area_cfg_v1130_present) {
    j.write_fieldname("areaConfiguration-v1130");
    area_cfg_v1130.to_json(j);
  }
  j.write_str("loggingDuration-r10", logging_dur_r10.to_string());
  j.write_str("loggingInterval-r10", logging_interv_r10.to_string());
  if (target_mbsfn_area_list_r12_present) {
    j.start_array("targetMBSFN-AreaList-r12");
    for (const auto& e1 : target_mbsfn_area_list_r12) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// VarLogMeasConfig-r15 ::= SEQUENCE
SRSASN_CODE var_log_meas_cfg_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(area_cfg_r10_present, 1));
  HANDLE_CODE(bref.pack(area_cfg_v1130_present, 1));
  HANDLE_CODE(bref.pack(target_mbsfn_area_list_r12_present, 1));
  HANDLE_CODE(bref.pack(bt_name_list_r15_present, 1));
  HANDLE_CODE(bref.pack(wlan_name_list_r15_present, 1));

  if (area_cfg_r10_present) {
    HANDLE_CODE(area_cfg_r10.pack(bref));
  }
  if (area_cfg_v1130_present) {
    HANDLE_CODE(area_cfg_v1130.pack(bref));
  }
  HANDLE_CODE(logging_dur_r10.pack(bref));
  HANDLE_CODE(logging_interv_r10.pack(bref));
  if (target_mbsfn_area_list_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, target_mbsfn_area_list_r12, 0, 8));
  }
  if (bt_name_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, bt_name_list_r15, 1, 4));
  }
  if (wlan_name_list_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, wlan_name_list_r15, 1, 4));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_log_meas_cfg_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(area_cfg_r10_present, 1));
  HANDLE_CODE(bref.unpack(area_cfg_v1130_present, 1));
  HANDLE_CODE(bref.unpack(target_mbsfn_area_list_r12_present, 1));
  HANDLE_CODE(bref.unpack(bt_name_list_r15_present, 1));
  HANDLE_CODE(bref.unpack(wlan_name_list_r15_present, 1));

  if (area_cfg_r10_present) {
    HANDLE_CODE(area_cfg_r10.unpack(bref));
  }
  if (area_cfg_v1130_present) {
    HANDLE_CODE(area_cfg_v1130.unpack(bref));
  }
  HANDLE_CODE(logging_dur_r10.unpack(bref));
  HANDLE_CODE(logging_interv_r10.unpack(bref));
  if (target_mbsfn_area_list_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(target_mbsfn_area_list_r12, bref, 0, 8));
  }
  if (bt_name_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(bt_name_list_r15, bref, 1, 4));
  }
  if (wlan_name_list_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(wlan_name_list_r15, bref, 1, 4));
  }

  return SRSASN_SUCCESS;
}
void var_log_meas_cfg_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (area_cfg_r10_present) {
    j.write_fieldname("areaConfiguration-r10");
    area_cfg_r10.to_json(j);
  }
  if (area_cfg_v1130_present) {
    j.write_fieldname("areaConfiguration-v1130");
    area_cfg_v1130.to_json(j);
  }
  j.write_str("loggingDuration-r10", logging_dur_r10.to_string());
  j.write_str("loggingInterval-r10", logging_interv_r10.to_string());
  if (target_mbsfn_area_list_r12_present) {
    j.start_array("targetMBSFN-AreaList-r12");
    for (const auto& e1 : target_mbsfn_area_list_r12) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (bt_name_list_r15_present) {
    j.start_array("bt-NameList-r15");
    for (const auto& e1 : bt_name_list_r15) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  if (wlan_name_list_r15_present) {
    j.start_array("wlan-NameList-r15");
    for (const auto& e1 : wlan_name_list_r15) {
      j.write_str(e1.to_string());
    }
    j.end_array();
  }
  j.end_obj();
}

// VarLogMeasReport-r10 ::= SEQUENCE
SRSASN_CODE var_log_meas_report_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(trace_ref_r10.pack(bref));
  HANDLE_CODE(trace_recording_session_ref_r10.pack(bref));
  HANDLE_CODE(tce_id_r10.pack(bref));
  HANDLE_CODE(plmn_id_r10.pack(bref));
  HANDLE_CODE(absolute_time_info_r10.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, log_meas_info_list_r10, 1, 4060));

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_log_meas_report_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(trace_ref_r10.unpack(bref));
  HANDLE_CODE(trace_recording_session_ref_r10.unpack(bref));
  HANDLE_CODE(tce_id_r10.unpack(bref));
  HANDLE_CODE(plmn_id_r10.unpack(bref));
  HANDLE_CODE(absolute_time_info_r10.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(log_meas_info_list_r10, bref, 1, 4060));

  return SRSASN_SUCCESS;
}
void var_log_meas_report_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("traceReference-r10");
  trace_ref_r10.to_json(j);
  j.write_str("traceRecordingSessionRef-r10", trace_recording_session_ref_r10.to_string());
  j.write_str("tce-Id-r10", tce_id_r10.to_string());
  j.write_fieldname("plmn-Identity-r10");
  plmn_id_r10.to_json(j);
  j.write_str("absoluteTimeInfo-r10", absolute_time_info_r10.to_string());
  j.start_array("logMeasInfoList-r10");
  for (const auto& e1 : log_meas_info_list_r10) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// VarLogMeasReport-r11 ::= SEQUENCE
SRSASN_CODE var_log_meas_report_r11_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(trace_ref_r10.pack(bref));
  HANDLE_CODE(trace_recording_session_ref_r10.pack(bref));
  HANDLE_CODE(tce_id_r10.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, plmn_id_list_r11, 1, 16));
  HANDLE_CODE(absolute_time_info_r10.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, log_meas_info_list_r10, 1, 4060));

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_log_meas_report_r11_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(trace_ref_r10.unpack(bref));
  HANDLE_CODE(trace_recording_session_ref_r10.unpack(bref));
  HANDLE_CODE(tce_id_r10.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(plmn_id_list_r11, bref, 1, 16));
  HANDLE_CODE(absolute_time_info_r10.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(log_meas_info_list_r10, bref, 1, 4060));

  return SRSASN_SUCCESS;
}
void var_log_meas_report_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("traceReference-r10");
  trace_ref_r10.to_json(j);
  j.write_str("traceRecordingSessionRef-r10", trace_recording_session_ref_r10.to_string());
  j.write_str("tce-Id-r10", tce_id_r10.to_string());
  j.start_array("plmn-IdentityList-r11");
  for (const auto& e1 : plmn_id_list_r11) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_str("absoluteTimeInfo-r10", absolute_time_info_r10.to_string());
  j.start_array("logMeasInfoList-r10");
  for (const auto& e1 : log_meas_info_list_r10) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// VarMeasIdleConfig-r15 ::= SEQUENCE
SRSASN_CODE var_meas_idle_cfg_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_idle_carrier_list_eutra_r15_present, 1));

  if (meas_idle_carrier_list_eutra_r15_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_idle_carrier_list_eutra_r15, 1, 8));
  }
  HANDLE_CODE(meas_idle_dur_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_meas_idle_cfg_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_idle_carrier_list_eutra_r15_present, 1));

  if (meas_idle_carrier_list_eutra_r15_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_idle_carrier_list_eutra_r15, bref, 1, 8));
  }
  HANDLE_CODE(meas_idle_dur_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void var_meas_idle_cfg_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (meas_idle_carrier_list_eutra_r15_present) {
    j.start_array("measIdleCarrierListEUTRA-r15");
    for (const auto& e1 : meas_idle_carrier_list_eutra_r15) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_str("measIdleDuration-r15", meas_idle_dur_r15.to_string());
  j.end_obj();
}

const char* var_meas_idle_cfg_r15_s::meas_idle_dur_r15_opts::to_string() const
{
  static const char* options[] = {"sec10", "sec30", "sec60", "sec120", "sec180", "sec240", "sec300"};
  return convert_enum_idx(options, 7, value, "var_meas_idle_cfg_r15_s::meas_idle_dur_r15_e_");
}
uint16_t var_meas_idle_cfg_r15_s::meas_idle_dur_r15_opts::to_number() const
{
  static const uint16_t options[] = {10, 30, 60, 120, 180, 240, 300};
  return map_enum_number(options, 7, value, "var_meas_idle_cfg_r15_s::meas_idle_dur_r15_e_");
}

// VarMeasIdleReport-r15 ::= SEQUENCE
SRSASN_CODE var_meas_idle_report_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, meas_report_idle_r15, 1, 3));

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_meas_idle_report_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(meas_report_idle_r15, bref, 1, 3));

  return SRSASN_SUCCESS;
}
void var_meas_idle_report_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("measReportIdle-r15");
  for (const auto& e1 : meas_report_idle_r15) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// VarMeasReport ::= SEQUENCE
SRSASN_CODE var_meas_report_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(meas_id_v1250_present, 1));
  HANDLE_CODE(bref.pack(cells_triggered_list_present, 1));
  HANDLE_CODE(bref.pack(csi_rs_triggered_list_r12_present, 1));
  HANDLE_CODE(bref.pack(pools_triggered_list_r14_present, 1));

  HANDLE_CODE(pack_integer(bref, meas_id, (uint8_t)1u, (uint8_t)32u));
  if (meas_id_v1250_present) {
    HANDLE_CODE(pack_integer(bref, meas_id_v1250, (uint8_t)33u, (uint8_t)64u));
  }
  if (cells_triggered_list_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, cells_triggered_list, 1, 32));
  }
  if (csi_rs_triggered_list_r12_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, csi_rs_triggered_list_r12, 1, 96, integer_packer<uint8_t>(1, 96)));
  }
  if (pools_triggered_list_r14_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, pools_triggered_list_r14, 1, 72, integer_packer<uint8_t>(1, 72)));
  }
  HANDLE_CODE(pack_unconstrained_integer(bref, nof_reports_sent));

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_meas_report_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(meas_id_v1250_present, 1));
  HANDLE_CODE(bref.unpack(cells_triggered_list_present, 1));
  HANDLE_CODE(bref.unpack(csi_rs_triggered_list_r12_present, 1));
  HANDLE_CODE(bref.unpack(pools_triggered_list_r14_present, 1));

  HANDLE_CODE(unpack_integer(meas_id, bref, (uint8_t)1u, (uint8_t)32u));
  if (meas_id_v1250_present) {
    HANDLE_CODE(unpack_integer(meas_id_v1250, bref, (uint8_t)33u, (uint8_t)64u));
  }
  if (cells_triggered_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(cells_triggered_list, bref, 1, 32));
  }
  if (csi_rs_triggered_list_r12_present) {
    HANDLE_CODE(unpack_dyn_seq_of(csi_rs_triggered_list_r12, bref, 1, 96, integer_packer<uint8_t>(1, 96)));
  }
  if (pools_triggered_list_r14_present) {
    HANDLE_CODE(unpack_dyn_seq_of(pools_triggered_list_r14, bref, 1, 72, integer_packer<uint8_t>(1, 72)));
  }
  HANDLE_CODE(unpack_unconstrained_integer(nof_reports_sent, bref));

  return SRSASN_SUCCESS;
}
void var_meas_report_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("measId", meas_id);
  if (meas_id_v1250_present) {
    j.write_int("measId-v1250", meas_id_v1250);
  }
  if (cells_triggered_list_present) {
    j.start_array("cellsTriggeredList");
    for (const auto& e1 : cells_triggered_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (csi_rs_triggered_list_r12_present) {
    j.start_array("csi-RS-TriggeredList-r12");
    for (const auto& e1 : csi_rs_triggered_list_r12) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (pools_triggered_list_r14_present) {
    j.start_array("poolsTriggeredList-r14");
    for (const auto& e1 : pools_triggered_list_r14) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.write_int("numberOfReportsSent", nof_reports_sent);
  j.end_obj();
}

// VarRLF-Report-r10 ::= SEQUENCE
SRSASN_CODE var_rlf_report_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(rlf_report_r10.pack(bref));
  HANDLE_CODE(plmn_id_r10.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_rlf_report_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(rlf_report_r10.unpack(bref));
  HANDLE_CODE(plmn_id_r10.unpack(bref));

  return SRSASN_SUCCESS;
}
void var_rlf_report_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("rlf-Report-r10");
  rlf_report_r10.to_json(j);
  j.write_fieldname("plmn-Identity-r10");
  plmn_id_r10.to_json(j);
  j.end_obj();
}

// VarRLF-Report-r11 ::= SEQUENCE
SRSASN_CODE var_rlf_report_r11_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(rlf_report_r10.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, plmn_id_list_r11, 1, 16));

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_rlf_report_r11_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(rlf_report_r10.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(plmn_id_list_r11, bref, 1, 16));

  return SRSASN_SUCCESS;
}
void var_rlf_report_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("rlf-Report-r10");
  rlf_report_r10.to_json(j);
  j.start_array("plmn-IdentityList-r11");
  for (const auto& e1 : plmn_id_list_r11) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// VarShortINACTIVE-MAC-Input-r15 ::= SEQUENCE
SRSASN_CODE var_short_inactive_mac_input_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(cell_id_r15.pack(bref));
  HANDLE_CODE(pack_integer(bref, pci_r15, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(c_rnti_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_short_inactive_mac_input_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(cell_id_r15.unpack(bref));
  HANDLE_CODE(unpack_integer(pci_r15, bref, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(c_rnti_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void var_short_inactive_mac_input_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("cellIdentity-r15", cell_id_r15.to_string());
  j.write_int("physCellId-r15", pci_r15);
  j.write_str("c-RNTI-r15", c_rnti_r15.to_string());
  j.end_obj();
}

// VarShortResumeMAC-Input-r13 ::= SEQUENCE
SRSASN_CODE var_short_resume_mac_input_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(cell_id_r13.pack(bref));
  HANDLE_CODE(pack_integer(bref, pci_r13, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(c_rnti_r13.pack(bref));
  HANDLE_CODE(resume_discriminator_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_short_resume_mac_input_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(cell_id_r13.unpack(bref));
  HANDLE_CODE(unpack_integer(pci_r13, bref, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(c_rnti_r13.unpack(bref));
  HANDLE_CODE(resume_discriminator_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void var_short_resume_mac_input_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("cellIdentity-r13", cell_id_r13.to_string());
  j.write_int("physCellId-r13", pci_r13);
  j.write_str("c-RNTI-r13", c_rnti_r13.to_string());
  j.write_str("resumeDiscriminator-r13", resume_discriminator_r13.to_string());
  j.end_obj();
}

// VarWLAN-MobilityConfig ::= SEQUENCE
SRSASN_CODE var_wlan_mob_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(wlan_mob_set_r13_present, 1));
  HANDLE_CODE(bref.pack(success_report_requested_present, 1));
  HANDLE_CODE(bref.pack(wlan_suspend_cfg_r14_present, 1));

  if (wlan_mob_set_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, wlan_mob_set_r13, 1, 32));
  }
  if (wlan_suspend_cfg_r14_present) {
    HANDLE_CODE(wlan_suspend_cfg_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_wlan_mob_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(wlan_mob_set_r13_present, 1));
  HANDLE_CODE(bref.unpack(success_report_requested_present, 1));
  HANDLE_CODE(bref.unpack(wlan_suspend_cfg_r14_present, 1));

  if (wlan_mob_set_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(wlan_mob_set_r13, bref, 1, 32));
  }
  if (wlan_suspend_cfg_r14_present) {
    HANDLE_CODE(wlan_suspend_cfg_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void var_wlan_mob_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (wlan_mob_set_r13_present) {
    j.start_array("wlan-MobilitySet-r13");
    for (const auto& e1 : wlan_mob_set_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (success_report_requested_present) {
    j.write_str("successReportRequested", "true");
  }
  if (wlan_suspend_cfg_r14_present) {
    j.write_fieldname("wlan-SuspendConfig-r14");
    wlan_suspend_cfg_r14.to_json(j);
  }
  j.end_obj();
}

// VarWLAN-Status-r13 ::= SEQUENCE
SRSASN_CODE var_wlan_status_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(status_r14_present, 1));

  HANDLE_CODE(status_r13.pack(bref));
  if (status_r14_present) {
    HANDLE_CODE(status_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE var_wlan_status_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(status_r14_present, 1));

  HANDLE_CODE(status_r13.unpack(bref));
  if (status_r14_present) {
    HANDLE_CODE(status_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void var_wlan_status_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("status-r13", status_r13.to_string());
  if (status_r14_present) {
    j.write_str("status-r14", status_r14.to_string());
  }
  j.end_obj();
}
