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

#include "srslte/asn1/rrc_asn1.h"
#include <sstream>

using namespace asn1;
using namespace asn1::rrc;

/*******************************************************************************
 *                              Logging Utilities
 ******************************************************************************/

static void invalid_enum_number(int value, const char* name)
{
  asn1::log_error("The provided enum value=%d of type %s cannot be translated into a number\n", value, name);
}

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

std::string phich_cfg_s::phich_dur_opts::to_string() const
{
  static const char* options[] = {"normal", "extended"};
  return convert_enum_idx(options, 2, value, "phich_cfg_s::phich_dur_e_");
}

std::string phich_cfg_s::phich_res_opts::to_string() const
{
  static const char* options[] = {"oneSixth", "half", "one", "two"};
  return convert_enum_idx(options, 4, value, "phich_cfg_s::phich_res_e_");
}
float phich_cfg_s::phich_res_opts::to_number() const
{
  static const float options[] = {0.16666666666666666, 0.5, 1.0, 2.0};
  return map_enum_number(options, 4, value, "phich_cfg_s::phich_res_e_");
}
std::string phich_cfg_s::phich_res_opts::to_number_string() const
{
  static const char* options[] = {"1/6", "0.5", "1", "2"};
  return convert_enum_idx(options, 4, value, "phich_cfg_s::phich_res_e_");
}

std::string mib_s::dl_bw_opts::to_string() const
{
  static const char* options[] = {"n6", "n15", "n25", "n50", "n75", "n100"};
  return convert_enum_idx(options, 6, value, "mib_s::dl_bw_e_");
}
uint8_t mib_s::dl_bw_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(options, 6, value, "mib_s::dl_bw_e_");
}

std::string mib_mbms_r14_s::dl_bw_mbms_r14_opts::to_string() const
{
  static const char* options[] = {"n6", "n15", "n25", "n50", "n75", "n100"};
  return convert_enum_idx(options, 6, value, "mib_mbms_r14_s::dl_bw_mbms_r14_e_");
}
uint8_t mib_mbms_r14_s::dl_bw_mbms_r14_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(options, 6, value, "mib_mbms_r14_s::dl_bw_mbms_r14_e_");
}

std::string gnss_id_r15_s::gnss_id_r15_opts::to_string() const
{
  static const char* options[] = {"gps", "sbas", "qzss", "galileo", "glonass", "bds"};
  return convert_enum_idx(options, 6, value, "gnss_id_r15_s::gnss_id_r15_e_");
}

std::string sbas_id_r15_s::sbas_id_r15_opts::to_string() const
{
  static const char* options[] = {"waas", "egnos", "msas", "gagan"};
  return convert_enum_idx(options, 4, value, "sbas_id_r15_s::sbas_id_r15_e_");
}

std::string pos_sib_type_r15_s::pos_sib_type_r15_opts::to_string() const
{
  static const char* options[] = {
      "posSibType1-1",  "posSibType1-2",  "posSibType1-3",  "posSibType1-4",  "posSibType1-5",  "posSibType1-6",
      "posSibType1-7",  "posSibType2-1",  "posSibType2-2",  "posSibType2-3",  "posSibType2-4",  "posSibType2-5",
      "posSibType2-6",  "posSibType2-7",  "posSibType2-8",  "posSibType2-9",  "posSibType2-10", "posSibType2-11",
      "posSibType2-12", "posSibType2-13", "posSibType2-14", "posSibType2-15", "posSibType2-16", "posSibType2-17",
      "posSibType2-18", "posSibType2-19", "posSibType3-1"};
  return convert_enum_idx(options, 27, value, "pos_sib_type_r15_s::pos_sib_type_r15_e_");
}

std::string plmn_id_info_r15_s::plmn_id_minus5_gc_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"plmn-Identity-r15", "plmn-Index-r15"};
  return convert_enum_idx(options, 2, value, "plmn_id_info_r15_s::plmn_id_minus5_gc_r15_c_::types");
}

std::string plmn_id_info_r15_s::cell_reserved_for_oper_r15_opts::to_string() const
{
  static const char* options[] = {"reserved", "notReserved"};
  return convert_enum_idx(options, 2, value, "plmn_id_info_r15_s::cell_reserved_for_oper_r15_e_");
}

std::string plmn_id_info_r15_s::cell_reserved_for_oper_crs_r15_opts::to_string() const
{
  static const char* options[] = {"reserved", "notReserved"};
  return convert_enum_idx(options, 2, value, "plmn_id_info_r15_s::cell_reserved_for_oper_crs_r15_e_");
}

std::string cell_id_minus5_gc_r15_c::types_opts::to_string() const
{
  static const char* options[] = {"cellIdentity-r15", "cellId-Index-r15"};
  return convert_enum_idx(options, 2, value, "cell_id_minus5_gc_r15_c::types");
}

std::string plmn_id_info_v1530_s::cell_reserved_for_oper_crs_r15_opts::to_string() const
{
  static const char* options[] = {"reserved", "notReserved"};
  return convert_enum_idx(options, 2, value, "plmn_id_info_v1530_s::cell_reserved_for_oper_crs_r15_e_");
}

std::string pos_sched_info_r15_s::pos_si_periodicity_r15_opts::to_string() const
{
  static const char* options[] = {"rf8", "rf16", "rf32", "rf64", "rf128", "rf256", "rf512"};
  return convert_enum_idx(options, 7, value, "pos_sched_info_r15_s::pos_si_periodicity_r15_e_");
}
uint16_t pos_sched_info_r15_s::pos_si_periodicity_r15_opts::to_number() const
{
  static const uint16_t options[] = {8, 16, 32, 64, 128, 256, 512};
  return map_enum_number(options, 7, value, "pos_sched_info_r15_s::pos_si_periodicity_r15_e_");
}

std::string cell_sel_info_ce_v1530_s::pwr_class14dbm_offset_r15_opts::to_string() const
{
  static const char* options[] = {"dB-6", "dB-3", "dB3", "dB6", "dB9", "dB12"};
  return convert_enum_idx(options, 6, value, "cell_sel_info_ce_v1530_s::pwr_class14dbm_offset_r15_e_");
}
int8_t cell_sel_info_ce_v1530_s::pwr_class14dbm_offset_r15_opts::to_number() const
{
  static const int8_t options[] = {-6, -3, 3, 6, 9, 12};
  return map_enum_number(options, 6, value, "cell_sel_info_ce_v1530_s::pwr_class14dbm_offset_r15_e_");
}

std::string plmn_id_info_s::cell_reserved_for_oper_opts::to_string() const
{
  static const char* options[] = {"reserved", "notReserved"};
  return convert_enum_idx(options, 2, value, "plmn_id_info_s::cell_reserved_for_oper_e_");
}

std::string sl_tx_pwr_r14_c::types_opts::to_string() const
{
  static const char* options[] = {"minusinfinity-r14", "txPower-r14"};
  return convert_enum_idx(options, 2, value, "sl_tx_pwr_r14_c::types");
}
int8_t sl_tx_pwr_r14_c::types_opts::to_number() const
{
  static const int8_t options[] = {-1};
  return map_enum_number(options, 1, value, "sl_tx_pwr_r14_c::types");
}

// Alpha-r12 ::= ENUMERATED
std::string alpha_r12_opts::to_string() const
{
  static const char* options[] = {"al0", "al04", "al05", "al06", "al07", "al08", "al09", "al1"};
  return convert_enum_idx(options, 8, value, "alpha_r12_e");
}
float alpha_r12_opts::to_number() const
{
  static const float options[] = {0.0, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
  return map_enum_number(options, 8, value, "alpha_r12_e");
}
std::string alpha_r12_opts::to_number_string() const
{
  static const char* options[] = {"0", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9", "1"};
  return convert_enum_idx(options, 8, value, "alpha_r12_e");
}

std::string sl_pssch_tx_params_r14_s::allowed_retx_num_pssch_r14_opts::to_string() const
{
  static const char* options[] = {"n0", "n1", "both", "spare1"};
  return convert_enum_idx(options, 4, value, "sl_pssch_tx_params_r14_s::allowed_retx_num_pssch_r14_e_");
}
uint8_t sl_pssch_tx_params_r14_s::allowed_retx_num_pssch_r14_opts::to_number() const
{
  static const uint8_t options[] = {0, 1};
  return map_enum_number(options, 2, value, "sl_pssch_tx_params_r14_s::allowed_retx_num_pssch_r14_e_");
}

// SL-RestrictResourceReservationPeriod-r14 ::= ENUMERATED
std::string sl_restrict_res_reserv_period_r14_opts::to_string() const
{
  static const char* options[] = {"v0dot2",
                                  "v0dot5",
                                  "v1",
                                  "v2",
                                  "v3",
                                  "v4",
                                  "v5",
                                  "v6",
                                  "v7",
                                  "v8",
                                  "v9",
                                  "v10",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "sl_restrict_res_reserv_period_r14_e");
}
float sl_restrict_res_reserv_period_r14_opts::to_number() const
{
  static const float options[] = {0.2, 0.5, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
  return map_enum_number(options, 12, value, "sl_restrict_res_reserv_period_r14_e");
}
std::string sl_restrict_res_reserv_period_r14_opts::to_number_string() const
{
  static const char* options[] = {"0.2", "0.5", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
  return convert_enum_idx(options, 16, value, "sl_restrict_res_reserv_period_r14_e");
}

// SL-TypeTxSync-r14 ::= ENUMERATED
std::string sl_type_tx_sync_r14_opts::to_string() const
{
  static const char* options[] = {"gnss", "enb", "ue"};
  return convert_enum_idx(options, 3, value, "sl_type_tx_sync_r14_e");
}

std::string sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::crs_intf_mitig_num_prbs_r15_opts::to_string() const
{
  static const char* options[] = {"n6", "n24"};
  return convert_enum_idx(
      options, 2, value, "sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::crs_intf_mitig_num_prbs_r15_e_");
}
uint8_t sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::crs_intf_mitig_num_prbs_r15_opts::to_number() const
{
  static const uint8_t options[] = {6, 24};
  return map_enum_number(
      options, 2, value, "sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::crs_intf_mitig_num_prbs_r15_e_");
}

std::string sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"crs-IntfMitigEnabled-15", "crs-IntfMitigNumPRBs-r15"};
  return convert_enum_idx(options, 2, value, "sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::types");
}
int8_t sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::types_opts::to_number() const
{
  static const int8_t options[] = {-15};
  return map_enum_number(options, 1, value, "sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::types");
}

std::string sib_type1_v1530_ies_s::cell_barred_crs_r15_opts::to_string() const
{
  static const char* options[] = {"barred", "notBarred"};
  return convert_enum_idx(options, 2, value, "sib_type1_v1530_ies_s::cell_barred_crs_r15_e_");
}

std::string
sib_type1_v1530_ies_s::cell_access_related_info_minus5_gc_r15_s_::cell_barred_minus5_gc_r15_opts::to_string() const
{
  static const char* options[] = {"barred", "notBarred"};
  return convert_enum_idx(
      options,
      2,
      value,
      "sib_type1_v1530_ies_s::cell_access_related_info_minus5_gc_r15_s_::cell_barred_minus5_gc_r15_e_");
}

std::string
sib_type1_v1530_ies_s::cell_access_related_info_minus5_gc_r15_s_::cell_barred_minus5_gc_crs_r15_opts::to_string() const
{
  static const char* options[] = {"barred", "notBarred"};
  return convert_enum_idx(
      options,
      2,
      value,
      "sib_type1_v1530_ies_s::cell_access_related_info_minus5_gc_r15_s_::cell_barred_minus5_gc_crs_r15_e_");
}

// SL-CP-Len-r12 ::= ENUMERATED
std::string sl_cp_len_r12_opts::to_string() const
{
  static const char* options[] = {"normal", "extended"};
  return convert_enum_idx(options, 2, value, "sl_cp_len_r12_e");
}

std::string sl_offset_ind_r12_c::types_opts::to_string() const
{
  static const char* options[] = {"small-r12", "large-r12"};
  return convert_enum_idx(options, 2, value, "sl_offset_ind_r12_c::types");
}

std::string sl_pssch_tx_cfg_r14_s::thres_ue_speed_r14_opts::to_string() const
{
  static const char* options[] = {"kmph60", "kmph80", "kmph100", "kmph120", "kmph140", "kmph160", "kmph180", "kmph200"};
  return convert_enum_idx(options, 8, value, "sl_pssch_tx_cfg_r14_s::thres_ue_speed_r14_e_");
}
uint8_t sl_pssch_tx_cfg_r14_s::thres_ue_speed_r14_opts::to_number() const
{
  static const uint8_t options[] = {60, 80, 100, 120, 140, 160, 180, 200};
  return map_enum_number(options, 8, value, "sl_pssch_tx_cfg_r14_s::thres_ue_speed_r14_e_");
}

std::string sf_bitmap_sl_r12_c::types_opts::to_string() const
{
  static const char* options[] = {"bs4-r12", "bs8-r12", "bs12-r12", "bs16-r12", "bs30-r12", "bs40-r12", "bs42-r12"};
  return convert_enum_idx(options, 7, value, "sf_bitmap_sl_r12_c::types");
}
uint8_t sf_bitmap_sl_r12_c::types_opts::to_number() const
{
  static const uint8_t options[] = {4, 8, 12, 16, 30, 40, 42};
  return map_enum_number(options, 7, value, "sf_bitmap_sl_r12_c::types");
}

std::string sf_bitmap_sl_r14_c::types_opts::to_string() const
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

std::string tdd_cfg_s::sf_assign_opts::to_string() const
{
  static const char* options[] = {"sa0", "sa1", "sa2", "sa3", "sa4", "sa5", "sa6"};
  return convert_enum_idx(options, 7, value, "tdd_cfg_s::sf_assign_e_");
}
uint8_t tdd_cfg_s::sf_assign_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6};
  return map_enum_number(options, 7, value, "tdd_cfg_s::sf_assign_e_");
}

std::string tdd_cfg_s::special_sf_patterns_opts::to_string() const
{
  static const char* options[] = {"ssp0", "ssp1", "ssp2", "ssp3", "ssp4", "ssp5", "ssp6", "ssp7", "ssp8"};
  return convert_enum_idx(options, 9, value, "tdd_cfg_s::special_sf_patterns_e_");
}
uint8_t tdd_cfg_s::special_sf_patterns_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
  return map_enum_number(options, 9, value, "tdd_cfg_s::special_sf_patterns_e_");
}

// BandclassCDMA2000 ::= ENUMERATED
std::string bandclass_cdma2000_opts::to_string() const
{
  static const char* options[] = {"bc0",       "bc1",     "bc2",    "bc3",    "bc4",       "bc5",       "bc6",
                                  "bc7",       "bc8",     "bc9",    "bc10",   "bc11",      "bc12",      "bc13",
                                  "bc14",      "bc15",    "bc16",   "bc17",   "bc18-v9a0", "bc19-v9a0", "bc20-v9a0",
                                  "bc21-v9a0", "spare10", "spare9", "spare8", "spare7",    "spare6",    "spare5",
                                  "spare4",    "spare3",  "spare2", "spare1"};
  return convert_enum_idx(options, 32, value, "bandclass_cdma2000_e");
}
uint8_t bandclass_cdma2000_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
  return map_enum_number(options, 22, value, "bandclass_cdma2000_e");
}

std::string sl_comm_res_pool_v2x_r14_s::size_subch_r14_opts::to_string() const
{
  static const char* options[] = {"n4",     "n5",     "n6",     "n8",      "n9",      "n10",     "n12",     "n15",
                                  "n16",    "n18",    "n20",    "n25",     "n30",     "n48",     "n50",     "n72",
                                  "n75",    "n96",    "n100",   "spare13", "spare12", "spare11", "spare10", "spare9",
                                  "spare8", "spare7", "spare6", "spare5",  "spare4",  "spare3",  "spare2",  "spare1"};
  return convert_enum_idx(options, 32, value, "sl_comm_res_pool_v2x_r14_s::size_subch_r14_e_");
}
uint8_t sl_comm_res_pool_v2x_r14_s::size_subch_r14_opts::to_number() const
{
  static const uint8_t options[] = {4, 5, 6, 8, 9, 10, 12, 15, 16, 18, 20, 25, 30, 48, 50, 72, 75, 96, 100};
  return map_enum_number(options, 19, value, "sl_comm_res_pool_v2x_r14_s::size_subch_r14_e_");
}

std::string sl_comm_res_pool_v2x_r14_s::num_subch_r14_opts::to_string() const
{
  static const char* options[] = {"n1", "n3", "n5", "n8", "n10", "n15", "n20", "spare1"};
  return convert_enum_idx(options, 8, value, "sl_comm_res_pool_v2x_r14_s::num_subch_r14_e_");
}
uint8_t sl_comm_res_pool_v2x_r14_s::num_subch_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 3, 5, 8, 10, 15, 20};
  return map_enum_number(options, 7, value, "sl_comm_res_pool_v2x_r14_s::num_subch_r14_e_");
}

std::string sl_sync_cfg_nfreq_r13_s::rx_params_r13_s_::disc_sync_win_r13_opts::to_string() const
{
  static const char* options[] = {"w1", "w2"};
  return convert_enum_idx(options, 2, value, "sl_sync_cfg_nfreq_r13_s::rx_params_r13_s_::disc_sync_win_r13_e_");
}
uint8_t sl_sync_cfg_nfreq_r13_s::rx_params_r13_s_::disc_sync_win_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "sl_sync_cfg_nfreq_r13_s::rx_params_r13_s_::disc_sync_win_r13_e_");
}

std::string tdd_cfg_v1130_s::special_sf_patterns_v1130_opts::to_string() const
{
  static const char* options[] = {"ssp7", "ssp9"};
  return convert_enum_idx(options, 2, value, "tdd_cfg_v1130_s::special_sf_patterns_v1130_e_");
}
uint8_t tdd_cfg_v1130_s::special_sf_patterns_v1130_opts::to_number() const
{
  static const uint8_t options[] = {7, 9};
  return map_enum_number(options, 2, value, "tdd_cfg_v1130_s::special_sf_patterns_v1130_e_");
}

std::string sl_comm_tx_pool_sensing_cfg_r14_s::prob_res_keep_r14_opts::to_string() const
{
  static const char* options[] = {"v0", "v0dot2", "v0dot4", "v0dot6", "v0dot8", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "sl_comm_tx_pool_sensing_cfg_r14_s::prob_res_keep_r14_e_");
}
float sl_comm_tx_pool_sensing_cfg_r14_s::prob_res_keep_r14_opts::to_number() const
{
  static const float options[] = {0.0, 0.2, 0.4, 0.6, 0.8};
  return map_enum_number(options, 5, value, "sl_comm_tx_pool_sensing_cfg_r14_s::prob_res_keep_r14_e_");
}
std::string sl_comm_tx_pool_sensing_cfg_r14_s::prob_res_keep_r14_opts::to_number_string() const
{
  static const char* options[] = {"0", "0.2", "0.4", "0.6", "0.8"};
  return convert_enum_idx(options, 8, value, "sl_comm_tx_pool_sensing_cfg_r14_s::prob_res_keep_r14_e_");
}

std::string sl_comm_tx_pool_sensing_cfg_r14_s::sl_reselect_after_r14_opts::to_string() const
{
  static const char* options[] = {"n1",
                                  "n2",
                                  "n3",
                                  "n4",
                                  "n5",
                                  "n6",
                                  "n7",
                                  "n8",
                                  "n9",
                                  "spare7",
                                  "spare6",
                                  "spare5",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "sl_comm_tx_pool_sensing_cfg_r14_s::sl_reselect_after_r14_e_");
}
uint8_t sl_comm_tx_pool_sensing_cfg_r14_s::sl_reselect_after_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  return map_enum_number(options, 9, value, "sl_comm_tx_pool_sensing_cfg_r14_s::sl_reselect_after_r14_e_");
}

std::string setup_opts::to_string() const
{
  static const char* options[] = {"release", "setup"};
  return convert_enum_idx(options, 2, value, "setup_e");
}

std::string sl_disc_res_pool_r12_s::disc_period_r12_opts::to_string() const
{
  static const char* options[] = {"rf32", "rf64", "rf128", "rf256", "rf512", "rf1024", "rf16-v1310", "spare"};
  return convert_enum_idx(options, 8, value, "sl_disc_res_pool_r12_s::disc_period_r12_e_");
}
uint16_t sl_disc_res_pool_r12_s::disc_period_r12_opts::to_number() const
{
  static const uint16_t options[] = {32, 64, 128, 256, 512, 1024, 16};
  return map_enum_number(options, 7, value, "sl_disc_res_pool_r12_s::disc_period_r12_e_");
}

std::string
sl_disc_res_pool_r12_s::tx_params_r12_s_::ue_sel_res_cfg_r12_s_::pool_sel_r12_c_::types_opts::to_string() const
{
  static const char* options[] = {"rsrpBased-r12", "random-r12"};
  return convert_enum_idx(
      options, 2, value, "sl_disc_res_pool_r12_s::tx_params_r12_s_::ue_sel_res_cfg_r12_s_::pool_sel_r12_c_::types");
}

std::string sl_disc_res_pool_r12_s::tx_params_r12_s_::ue_sel_res_cfg_r12_s_::tx_probability_r12_opts::to_string() const
{
  static const char* options[] = {"p25", "p50", "p75", "p100"};
  return convert_enum_idx(
      options, 4, value, "sl_disc_res_pool_r12_s::tx_params_r12_s_::ue_sel_res_cfg_r12_s_::tx_probability_r12_e_");
}
uint8_t sl_disc_res_pool_r12_s::tx_params_r12_s_::ue_sel_res_cfg_r12_s_::tx_probability_r12_opts::to_number() const
{
  static const uint8_t options[] = {25, 50, 75, 100};
  return map_enum_number(
      options, 4, value, "sl_disc_res_pool_r12_s::tx_params_r12_s_::ue_sel_res_cfg_r12_s_::tx_probability_r12_e_");
}

std::string sl_disc_res_pool_r12_s::disc_period_v1310_c_::setup_opts::to_string() const
{
  static const char* options[] = {"rf4", "rf6", "rf7", "rf8", "rf12", "rf14", "rf24", "rf28"};
  return convert_enum_idx(options, 8, value, "sl_disc_res_pool_r12_s::disc_period_v1310_c_::setup_e_");
}
uint8_t sl_disc_res_pool_r12_s::disc_period_v1310_c_::setup_opts::to_number() const
{
  static const uint8_t options[] = {4, 6, 7, 8, 12, 14, 24, 28};
  return map_enum_number(options, 8, value, "sl_disc_res_pool_r12_s::disc_period_v1310_c_::setup_e_");
}

std::string
sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_r13_c_::setup_s_::freq_info_s_::ul_bw_opts::to_string() const
{
  static const char* options[] = {"n6", "n15", "n25", "n50", "n75", "n100"};
  return convert_enum_idx(
      options, 6, value, "sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_r13_c_::setup_s_::freq_info_s_::ul_bw_e_");
}
uint8_t sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_r13_c_::setup_s_::freq_info_s_::ul_bw_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(
      options, 6, value, "sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_r13_c_::setup_s_::freq_info_s_::ul_bw_e_");
}

std::string sl_zone_cfg_r14_s::zone_len_r14_opts::to_string() const
{
  static const char* options[] = {"m5", "m10", "m20", "m50", "m100", "m200", "m500", "spare1"};
  return convert_enum_idx(options, 8, value, "sl_zone_cfg_r14_s::zone_len_r14_e_");
}
uint16_t sl_zone_cfg_r14_s::zone_len_r14_opts::to_number() const
{
  static const uint16_t options[] = {5, 10, 20, 50, 100, 200, 500};
  return map_enum_number(options, 7, value, "sl_zone_cfg_r14_s::zone_len_r14_e_");
}

std::string sl_zone_cfg_r14_s::zone_width_r14_opts::to_string() const
{
  static const char* options[] = {"m5", "m10", "m20", "m50", "m100", "m200", "m500", "spare1"};
  return convert_enum_idx(options, 8, value, "sl_zone_cfg_r14_s::zone_width_r14_e_");
}
uint16_t sl_zone_cfg_r14_s::zone_width_r14_opts::to_number() const
{
  static const uint16_t options[] = {5, 10, 20, 50, 100, 200, 500};
  return map_enum_number(options, 7, value, "sl_zone_cfg_r14_s::zone_width_r14_e_");
}

std::string pci_range_s::range_opts::to_string() const
{
  static const char* options[] = {"n4",
                                  "n8",
                                  "n12",
                                  "n16",
                                  "n24",
                                  "n32",
                                  "n48",
                                  "n64",
                                  "n84",
                                  "n96",
                                  "n128",
                                  "n168",
                                  "n252",
                                  "n504",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "pci_range_s::range_e_");
}
uint16_t pci_range_s::range_opts::to_number() const
{
  static const uint16_t options[] = {4, 8, 12, 16, 24, 32, 48, 64, 84, 96, 128, 168, 252, 504};
  return map_enum_number(options, 14, value, "pci_range_s::range_e_");
}

// Q-OffsetRange ::= ENUMERATED
std::string q_offset_range_opts::to_string() const
{
  static const char* options[] = {"dB-24", "dB-22", "dB-20", "dB-18", "dB-16", "dB-14", "dB-12", "dB-10",
                                  "dB-8",  "dB-6",  "dB-5",  "dB-4",  "dB-3",  "dB-2",  "dB-1",  "dB0",
                                  "dB1",   "dB2",   "dB3",   "dB4",   "dB5",   "dB6",   "dB8",   "dB10",
                                  "dB12",  "dB14",  "dB16",  "dB18",  "dB20",  "dB22",  "dB24"};
  return convert_enum_idx(options, 31, value, "q_offset_range_e");
}
int8_t q_offset_range_opts::to_number() const
{
  static const int8_t options[] = {-24, -22, -20, -18, -16, -14, -12, -10, -8, -6, -5, -4, -3, -2, -1, 0,
                                   1,   2,   3,   4,   5,   6,   8,   10,  12, 14, 16, 18, 20, 22, 24};
  return map_enum_number(options, 31, value, "q_offset_range_e");
}

std::string sched_info_br_r13_s::si_tbs_r13_opts::to_string() const
{
  static const char* options[] = {"b152", "b208", "b256", "b328", "b408", "b504", "b600", "b712", "b808", "b936"};
  return convert_enum_idx(options, 10, value, "sched_info_br_r13_s::si_tbs_r13_e_");
}
uint16_t sched_info_br_r13_s::si_tbs_r13_opts::to_number() const
{
  static const uint16_t options[] = {152, 208, 256, 328, 408, 504, 600, 712, 808, 936};
  return map_enum_number(options, 10, value, "sched_info_br_r13_s::si_tbs_r13_e_");
}

std::string speed_state_scale_factors_s::sf_medium_opts::to_string() const
{
  static const char* options[] = {"oDot25", "oDot5", "oDot75", "lDot0"};
  return convert_enum_idx(options, 4, value, "speed_state_scale_factors_s::sf_medium_e_");
}
float speed_state_scale_factors_s::sf_medium_opts::to_number() const
{
  static const float options[] = {0.25, 0.5, 0.75, 1.0};
  return map_enum_number(options, 4, value, "speed_state_scale_factors_s::sf_medium_e_");
}
std::string speed_state_scale_factors_s::sf_medium_opts::to_number_string() const
{
  static const char* options[] = {"0.25", "0.5", "0.75", "1.0"};
  return convert_enum_idx(options, 4, value, "speed_state_scale_factors_s::sf_medium_e_");
}

std::string speed_state_scale_factors_s::sf_high_opts::to_string() const
{
  static const char* options[] = {"oDot25", "oDot5", "oDot75", "lDot0"};
  return convert_enum_idx(options, 4, value, "speed_state_scale_factors_s::sf_high_e_");
}
float speed_state_scale_factors_s::sf_high_opts::to_number() const
{
  static const float options[] = {0.25, 0.5, 0.75, 1.0};
  return map_enum_number(options, 4, value, "speed_state_scale_factors_s::sf_high_e_");
}
std::string speed_state_scale_factors_s::sf_high_opts::to_number_string() const
{
  static const char* options[] = {"0.25", "0.5", "0.75", "1.0"};
  return convert_enum_idx(options, 4, value, "speed_state_scale_factors_s::sf_high_e_");
}

// AllowedMeasBandwidth ::= ENUMERATED
std::string allowed_meas_bw_opts::to_string() const
{
  static const char* options[] = {"mbw6", "mbw15", "mbw25", "mbw50", "mbw75", "mbw100"};
  return convert_enum_idx(options, 6, value, "allowed_meas_bw_e");
}
uint8_t allowed_meas_bw_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(options, 6, value, "allowed_meas_bw_e");
}

// BandIndicatorGERAN ::= ENUMERATED
std::string band_ind_geran_opts::to_string() const
{
  static const char* options[] = {"dcs1800", "pcs1900"};
  return convert_enum_idx(options, 2, value, "band_ind_geran_e");
}
uint16_t band_ind_geran_opts::to_number() const
{
  static const uint16_t options[] = {1800, 1900};
  return map_enum_number(options, 2, value, "band_ind_geran_e");
}

std::string barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_factor_r13_opts::to_string() const
{
  static const char* options[] = {
      "p00", "p05", "p10", "p15", "p20", "p25", "p30", "p40", "p50", "p60", "p70", "p75", "p80", "p85", "p90", "p95"};
  return convert_enum_idx(
      options, 16, value, "barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_factor_r13_e_");
}
float barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_factor_r13_opts::to_number() const
{
  static const float options[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 6.0, 7.0, 7.5, 8.0, 8.5, 9.0, 9.5};
  return map_enum_number(
      options, 16, value, "barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_factor_r13_e_");
}
std::string barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_factor_r13_opts::to_number_string() const
{
  static const char* options[] = {
      "0.0", "0.5", "1.0", "1.5", "2.0", "2.5", "3.0", "4.0", "5.0", "6.0", "7.0", "7.5", "8.0", "8.5", "9.0", "9.5"};
  return convert_enum_idx(
      options, 16, value, "barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_factor_r13_e_");
}

std::string barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_time_r13_opts::to_string() const
{
  static const char* options[] = {"s4", "s8", "s16", "s32", "s64", "s128", "s256", "s512"};
  return convert_enum_idx(options, 8, value, "barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_time_r13_e_");
}
uint16_t barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_time_r13_opts::to_number() const
{
  static const uint16_t options[] = {4, 8, 16, 32, 64, 128, 256, 512};
  return map_enum_number(options, 8, value, "barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_time_r13_e_");
}

std::string cell_sel_info_nfreq_r13_s::q_hyst_r13_opts::to_string() const
{
  static const char* options[] = {"dB0",
                                  "dB1",
                                  "dB2",
                                  "dB3",
                                  "dB4",
                                  "dB5",
                                  "dB6",
                                  "dB8",
                                  "dB10",
                                  "dB12",
                                  "dB14",
                                  "dB16",
                                  "dB18",
                                  "dB20",
                                  "dB22",
                                  "dB24"};
  return convert_enum_idx(options, 16, value, "cell_sel_info_nfreq_r13_s::q_hyst_r13_e_");
}
uint8_t cell_sel_info_nfreq_r13_s::q_hyst_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24};
  return map_enum_number(options, 16, value, "cell_sel_info_nfreq_r13_s::q_hyst_r13_e_");
}

std::string plmn_id_info2_r12_c::types_opts::to_string() const
{
  static const char* options[] = {"plmn-Index-r12", "plmnIdentity-r12"};
  return convert_enum_idx(options, 2, value, "plmn_id_info2_r12_c::types");
}

std::string prach_params_ce_r13_s::prach_start_sf_r13_opts::to_string() const
{
  static const char* options[] = {"sf2", "sf4", "sf8", "sf16", "sf32", "sf64", "sf128", "sf256"};
  return convert_enum_idx(options, 8, value, "prach_params_ce_r13_s::prach_start_sf_r13_e_");
}
uint16_t prach_params_ce_r13_s::prach_start_sf_r13_opts::to_number() const
{
  static const uint16_t options[] = {2, 4, 8, 16, 32, 64, 128, 256};
  return map_enum_number(options, 8, value, "prach_params_ce_r13_s::prach_start_sf_r13_e_");
}

std::string prach_params_ce_r13_s::max_num_preamb_attempt_ce_r13_opts::to_string() const
{
  static const char* options[] = {"n3", "n4", "n5", "n6", "n7", "n8", "n10"};
  return convert_enum_idx(options, 7, value, "prach_params_ce_r13_s::max_num_preamb_attempt_ce_r13_e_");
}
uint8_t prach_params_ce_r13_s::max_num_preamb_attempt_ce_r13_opts::to_number() const
{
  static const uint8_t options[] = {3, 4, 5, 6, 7, 8, 10};
  return map_enum_number(options, 7, value, "prach_params_ce_r13_s::max_num_preamb_attempt_ce_r13_e_");
}

std::string prach_params_ce_r13_s::num_repeat_per_preamb_attempt_r13_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n4", "n8", "n16", "n32", "n64", "n128"};
  return convert_enum_idx(options, 8, value, "prach_params_ce_r13_s::num_repeat_per_preamb_attempt_r13_e_");
}
uint8_t prach_params_ce_r13_s::num_repeat_per_preamb_attempt_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8, 16, 32, 64, 128};
  return map_enum_number(options, 8, value, "prach_params_ce_r13_s::num_repeat_per_preamb_attempt_r13_e_");
}

std::string prach_params_ce_r13_s::mpdcch_num_repeat_ra_r13_opts::to_string() const
{
  static const char* options[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "r128", "r256"};
  return convert_enum_idx(options, 9, value, "prach_params_ce_r13_s::mpdcch_num_repeat_ra_r13_e_");
}
uint16_t prach_params_ce_r13_s::mpdcch_num_repeat_ra_r13_opts::to_number() const
{
  static const uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
  return map_enum_number(options, 9, value, "prach_params_ce_r13_s::mpdcch_num_repeat_ra_r13_e_");
}

std::string prach_params_ce_r13_s::prach_hop_cfg_r13_opts::to_string() const
{
  static const char* options[] = {"on", "off"};
  return convert_enum_idx(options, 2, value, "prach_params_ce_r13_s::prach_hop_cfg_r13_e_");
}

std::string rach_ce_level_info_r13_s::ra_resp_win_size_r13_opts::to_string() const
{
  static const char* options[] = {"sf20", "sf50", "sf80", "sf120", "sf180", "sf240", "sf320", "sf400"};
  return convert_enum_idx(options, 8, value, "rach_ce_level_info_r13_s::ra_resp_win_size_r13_e_");
}
uint16_t rach_ce_level_info_r13_s::ra_resp_win_size_r13_opts::to_number() const
{
  static const uint16_t options[] = {20, 50, 80, 120, 180, 240, 320, 400};
  return map_enum_number(options, 8, value, "rach_ce_level_info_r13_s::ra_resp_win_size_r13_e_");
}

std::string rach_ce_level_info_r13_s::mac_contention_resolution_timer_r13_opts::to_string() const
{
  static const char* options[] = {"sf80", "sf100", "sf120", "sf160", "sf200", "sf240", "sf480", "sf960"};
  return convert_enum_idx(options, 8, value, "rach_ce_level_info_r13_s::mac_contention_resolution_timer_r13_e_");
}
uint16_t rach_ce_level_info_r13_s::mac_contention_resolution_timer_r13_opts::to_number() const
{
  static const uint16_t options[] = {80, 100, 120, 160, 200, 240, 480, 960};
  return map_enum_number(options, 8, value, "rach_ce_level_info_r13_s::mac_contention_resolution_timer_r13_e_");
}

std::string rach_ce_level_info_r13_s::rar_hop_cfg_r13_opts::to_string() const
{
  static const char* options[] = {"on", "off"};
  return convert_enum_idx(options, 2, value, "rach_ce_level_info_r13_s::rar_hop_cfg_r13_e_");
}

std::string rach_ce_level_info_r13_s::edt_params_r15_s_::edt_tbs_r15_opts::to_string() const
{
  static const char* options[] = {"b328", "b408", "b504", "b600", "b712", "b808", "b936", "b1000or456"};
  return convert_enum_idx(options, 8, value, "rach_ce_level_info_r13_s::edt_params_r15_s_::edt_tbs_r15_e_");
}
uint16_t rach_ce_level_info_r13_s::edt_params_r15_s_::edt_tbs_r15_opts::to_number() const
{
  static const uint16_t options[] = {328, 408, 504, 600, 712, 808, 936, 1000};
  return map_enum_number(options, 8, value, "rach_ce_level_info_r13_s::edt_params_r15_s_::edt_tbs_r15_e_");
}

std::string rach_ce_level_info_r13_s::edt_params_r15_s_::mac_contention_resolution_timer_r15_opts::to_string() const
{
  static const char* options[] = {"sf240", "sf480", "sf960", "sf1920", "sf3840", "sf5760", "sf7680", "sf10240"};
  return convert_enum_idx(
      options, 8, value, "rach_ce_level_info_r13_s::edt_params_r15_s_::mac_contention_resolution_timer_r15_e_");
}
uint16_t rach_ce_level_info_r13_s::edt_params_r15_s_::mac_contention_resolution_timer_r15_opts::to_number() const
{
  static const uint16_t options[] = {240, 480, 960, 1920, 3840, 5760, 7680, 10240};
  return map_enum_number(
      options, 8, value, "rach_ce_level_info_r13_s::edt_params_r15_s_::mac_contention_resolution_timer_r15_e_");
}

std::string sl_disc_tx_res_inter_freq_r13_c::types_opts::to_string() const
{
  static const char* options[] = {
      "acquireSI-FromCarrier-r13", "discTxPoolCommon-r13", "requestDedicated-r13", "noTxOnCarrier-r13"};
  return convert_enum_idx(options, 4, value, "sl_disc_tx_res_inter_freq_r13_c::types");
}

std::string sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::mpdcch_pdsch_hop_nb_r13_opts::to_string() const
{
  static const char* options[] = {"nb2", "nb4"};
  return convert_enum_idx(
      options, 2, value, "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::mpdcch_pdsch_hop_nb_r13_e_");
}
uint8_t sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::mpdcch_pdsch_hop_nb_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(
      options, 2, value, "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::mpdcch_pdsch_hop_nb_r13_e_");
}

std::string sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::
    interv_fdd_r13_opts::to_string() const
{
  static const char* options[] = {"int1", "int2", "int4", "int8"};
  return convert_enum_idx(
      options,
      4,
      value,
      "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::interv_fdd_r13_e_");
}
uint8_t sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::interv_fdd_r13_opts::
    to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8};
  return map_enum_number(
      options,
      4,
      value,
      "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::interv_fdd_r13_e_");
}

std::string sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::
    interv_tdd_r13_opts::to_string() const
{
  static const char* options[] = {"int1", "int5", "int10", "int20"};
  return convert_enum_idx(
      options,
      4,
      value,
      "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::interv_tdd_r13_e_");
}
uint8_t sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::interv_tdd_r13_opts::
    to_number() const
{
  static const uint8_t options[] = {1, 5, 10, 20};
  return map_enum_number(
      options,
      4,
      value,
      "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::interv_tdd_r13_e_");
}

std::string
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"interval-FDD-r13", "interval-TDD-r13"};
  return convert_enum_idx(
      options,
      2,
      value,
      "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::types");
}

std::string sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::
    interv_fdd_r13_opts::to_string() const
{
  static const char* options[] = {"int2", "int4", "int8", "int16"};
  return convert_enum_idx(
      options,
      4,
      value,
      "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::interv_fdd_r13_e_");
}
uint8_t sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::interv_fdd_r13_opts::
    to_number() const
{
  static const uint8_t options[] = {2, 4, 8, 16};
  return map_enum_number(
      options,
      4,
      value,
      "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::interv_fdd_r13_e_");
}

std::string sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::
    interv_tdd_r13_opts::to_string() const
{
  static const char* options[] = {"int5", "int10", "int20", "int40"};
  return convert_enum_idx(
      options,
      4,
      value,
      "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::interv_tdd_r13_e_");
}
uint8_t sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::interv_tdd_r13_opts::
    to_number() const
{
  static const uint8_t options[] = {5, 10, 20, 40};
  return map_enum_number(
      options,
      4,
      value,
      "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::interv_tdd_r13_e_");
}

std::string
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"interval-FDD-r13", "interval-TDD-r13"};
  return convert_enum_idx(
      options,
      2,
      value,
      "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::types");
}

std::string sys_time_info_cdma2000_s::cdma_sys_time_c_::types_opts::to_string() const
{
  static const char* options[] = {"synchronousSystemTime", "asynchronousSystemTime"};
  return convert_enum_idx(options, 2, value, "sys_time_info_cdma2000_s::cdma_sys_time_c_::types");
}

std::string ac_barr_cfg_s::ac_barr_factor_opts::to_string() const
{
  static const char* options[] = {
      "p00", "p05", "p10", "p15", "p20", "p25", "p30", "p40", "p50", "p60", "p70", "p75", "p80", "p85", "p90", "p95"};
  return convert_enum_idx(options, 16, value, "ac_barr_cfg_s::ac_barr_factor_e_");
}
float ac_barr_cfg_s::ac_barr_factor_opts::to_number() const
{
  static const float options[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 6.0, 7.0, 7.5, 8.0, 8.5, 9.0, 9.5};
  return map_enum_number(options, 16, value, "ac_barr_cfg_s::ac_barr_factor_e_");
}
std::string ac_barr_cfg_s::ac_barr_factor_opts::to_number_string() const
{
  static const char* options[] = {
      "0.0", "0.5", "1.0", "1.5", "2.0", "2.5", "3.0", "4.0", "5.0", "6.0", "7.0", "7.5", "8.0", "8.5", "9.0", "9.5"};
  return convert_enum_idx(options, 16, value, "ac_barr_cfg_s::ac_barr_factor_e_");
}

std::string ac_barr_cfg_s::ac_barr_time_opts::to_string() const
{
  static const char* options[] = {"s4", "s8", "s16", "s32", "s64", "s128", "s256", "s512"};
  return convert_enum_idx(options, 8, value, "ac_barr_cfg_s::ac_barr_time_e_");
}
uint16_t ac_barr_cfg_s::ac_barr_time_opts::to_number() const
{
  static const uint16_t options[] = {4, 8, 16, 32, 64, 128, 256, 512};
  return map_enum_number(options, 8, value, "ac_barr_cfg_s::ac_barr_time_e_");
}

std::string carrier_freqs_geran_s::following_arfcns_c_::types_opts::to_string() const
{
  static const char* options[] = {"explicitListOfARFCNs", "equallySpacedARFCNs", "variableBitMapOfARFCNs"};
  return convert_enum_idx(options, 3, value, "carrier_freqs_geran_s::following_arfcns_c_::types");
}

// CellReselectionSubPriority-r13 ::= ENUMERATED
std::string cell_resel_sub_prio_r13_opts::to_string() const
{
  static const char* options[] = {"oDot2", "oDot4", "oDot6", "oDot8"};
  return convert_enum_idx(options, 4, value, "cell_resel_sub_prio_r13_e");
}
float cell_resel_sub_prio_r13_opts::to_number() const
{
  static const float options[] = {0.2, 0.4, 0.6, 0.8};
  return map_enum_number(options, 4, value, "cell_resel_sub_prio_r13_e");
}
std::string cell_resel_sub_prio_r13_opts::to_number_string() const
{
  static const char* options[] = {"0.2", "0.4", "0.6", "0.8"};
  return convert_enum_idx(options, 4, value, "cell_resel_sub_prio_r13_e");
}

std::string delta_flist_pucch_s::delta_f_pucch_format1_opts::to_string() const
{
  static const char* options[] = {"deltaF-2", "deltaF0", "deltaF2"};
  return convert_enum_idx(options, 3, value, "delta_flist_pucch_s::delta_f_pucch_format1_e_");
}
int8_t delta_flist_pucch_s::delta_f_pucch_format1_opts::to_number() const
{
  static const int8_t options[] = {-2, 0, 2};
  return map_enum_number(options, 3, value, "delta_flist_pucch_s::delta_f_pucch_format1_e_");
}

std::string delta_flist_pucch_s::delta_f_pucch_format1b_opts::to_string() const
{
  static const char* options[] = {"deltaF1", "deltaF3", "deltaF5"};
  return convert_enum_idx(options, 3, value, "delta_flist_pucch_s::delta_f_pucch_format1b_e_");
}
uint8_t delta_flist_pucch_s::delta_f_pucch_format1b_opts::to_number() const
{
  static const uint8_t options[] = {1, 3, 5};
  return map_enum_number(options, 3, value, "delta_flist_pucch_s::delta_f_pucch_format1b_e_");
}

std::string delta_flist_pucch_s::delta_f_pucch_format2_opts::to_string() const
{
  static const char* options[] = {"deltaF-2", "deltaF0", "deltaF1", "deltaF2"};
  return convert_enum_idx(options, 4, value, "delta_flist_pucch_s::delta_f_pucch_format2_e_");
}
int8_t delta_flist_pucch_s::delta_f_pucch_format2_opts::to_number() const
{
  static const int8_t options[] = {-2, 0, 1, 2};
  return map_enum_number(options, 4, value, "delta_flist_pucch_s::delta_f_pucch_format2_e_");
}

std::string delta_flist_pucch_s::delta_f_pucch_format2a_opts::to_string() const
{
  static const char* options[] = {"deltaF-2", "deltaF0", "deltaF2"};
  return convert_enum_idx(options, 3, value, "delta_flist_pucch_s::delta_f_pucch_format2a_e_");
}
int8_t delta_flist_pucch_s::delta_f_pucch_format2a_opts::to_number() const
{
  static const int8_t options[] = {-2, 0, 2};
  return map_enum_number(options, 3, value, "delta_flist_pucch_s::delta_f_pucch_format2a_e_");
}

std::string delta_flist_pucch_s::delta_f_pucch_format2b_opts::to_string() const
{
  static const char* options[] = {"deltaF-2", "deltaF0", "deltaF2"};
  return convert_enum_idx(options, 3, value, "delta_flist_pucch_s::delta_f_pucch_format2b_e_");
}
int8_t delta_flist_pucch_s::delta_f_pucch_format2b_opts::to_number() const
{
  static const int8_t options[] = {-2, 0, 2};
  return map_enum_number(options, 3, value, "delta_flist_pucch_s::delta_f_pucch_format2b_e_");
}

std::string edt_prach_params_ce_r15_s::edt_prach_params_ce_r15_s_::prach_start_sf_r15_opts::to_string() const
{
  static const char* options[] = {"sf2", "sf4", "sf8", "sf16", "sf32", "sf64", "sf128", "sf256"};
  return convert_enum_idx(
      options, 8, value, "edt_prach_params_ce_r15_s::edt_prach_params_ce_r15_s_::prach_start_sf_r15_e_");
}
uint16_t edt_prach_params_ce_r15_s::edt_prach_params_ce_r15_s_::prach_start_sf_r15_opts::to_number() const
{
  static const uint16_t options[] = {2, 4, 8, 16, 32, 64, 128, 256};
  return map_enum_number(
      options, 8, value, "edt_prach_params_ce_r15_s::edt_prach_params_ce_r15_s_::prach_start_sf_r15_e_");
}

// FilterCoefficient ::= ENUMERATED
std::string filt_coef_opts::to_string() const
{
  static const char* options[] = {"fc0",
                                  "fc1",
                                  "fc2",
                                  "fc3",
                                  "fc4",
                                  "fc5",
                                  "fc6",
                                  "fc7",
                                  "fc8",
                                  "fc9",
                                  "fc11",
                                  "fc13",
                                  "fc15",
                                  "fc17",
                                  "fc19",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "filt_coef_e");
}
uint8_t filt_coef_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 13, 15, 17, 19};
  return map_enum_number(options, 15, value, "filt_coef_e");
}

std::string mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"sf5-r15", "sf10-r15", "sf20-r15", "sf40-r15", "sf80-r15", "sf160-r15"};
  return convert_enum_idx(options, 6, value, "mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_::types");
}
uint8_t mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {5, 10, 20, 40, 80, 160};
  return map_enum_number(options, 6, value, "mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_::types");
}

std::string mtc_ssb_nr_r15_s::ssb_dur_r15_opts::to_string() const
{
  static const char* options[] = {"sf1", "sf2", "sf3", "sf4", "sf5"};
  return convert_enum_idx(options, 5, value, "mtc_ssb_nr_r15_s::ssb_dur_r15_e_");
}
uint8_t mtc_ssb_nr_r15_s::ssb_dur_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5};
  return map_enum_number(options, 5, value, "mtc_ssb_nr_r15_s::ssb_dur_r15_e_");
}

std::string meas_idle_carrier_eutra_r15_s::report_quantities_opts::to_string() const
{
  static const char* options[] = {"rsrp", "rsrq", "both"};
  return convert_enum_idx(options, 3, value, "meas_idle_carrier_eutra_r15_s::report_quantities_e_");
}

std::string params_cdma2000_r11_s::sys_time_info_r11_c_::types_opts::to_string() const
{
  static const char* options[] = {"explicitValue", "defaultValue"};
  return convert_enum_idx(options, 2, value, "params_cdma2000_r11_s::sys_time_info_r11_c_::types");
}

std::string pwr_ramp_params_s::pwr_ramp_step_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB2", "dB4", "dB6"};
  return convert_enum_idx(options, 4, value, "pwr_ramp_params_s::pwr_ramp_step_e_");
}
uint8_t pwr_ramp_params_s::pwr_ramp_step_opts::to_number() const
{
  static const uint8_t options[] = {0, 2, 4, 6};
  return map_enum_number(options, 4, value, "pwr_ramp_params_s::pwr_ramp_step_e_");
}

std::string pwr_ramp_params_s::preamb_init_rx_target_pwr_opts::to_string() const
{
  static const char* options[] = {"dBm-120",
                                  "dBm-118",
                                  "dBm-116",
                                  "dBm-114",
                                  "dBm-112",
                                  "dBm-110",
                                  "dBm-108",
                                  "dBm-106",
                                  "dBm-104",
                                  "dBm-102",
                                  "dBm-100",
                                  "dBm-98",
                                  "dBm-96",
                                  "dBm-94",
                                  "dBm-92",
                                  "dBm-90"};
  return convert_enum_idx(options, 16, value, "pwr_ramp_params_s::preamb_init_rx_target_pwr_e_");
}
int8_t pwr_ramp_params_s::preamb_init_rx_target_pwr_opts::to_number() const
{
  static const int8_t options[] = {
      -120, -118, -116, -114, -112, -110, -108, -106, -104, -102, -100, -98, -96, -94, -92, -90};
  return map_enum_number(options, 16, value, "pwr_ramp_params_s::preamb_init_rx_target_pwr_e_");
}

// PreambleTransMax ::= ENUMERATED
std::string preamb_trans_max_opts::to_string() const
{
  static const char* options[] = {"n3", "n4", "n5", "n6", "n7", "n8", "n10", "n20", "n50", "n100", "n200"};
  return convert_enum_idx(options, 11, value, "preamb_trans_max_e");
}
uint8_t preamb_trans_max_opts::to_number() const
{
  static const uint8_t options[] = {3, 4, 5, 6, 7, 8, 10, 20, 50, 100, 200};
  return map_enum_number(options, 11, value, "preamb_trans_max_e");
}

std::string sl_hop_cfg_comm_r12_s::num_subbands_r12_opts::to_string() const
{
  static const char* options[] = {"ns1", "ns2", "ns4"};
  return convert_enum_idx(options, 3, value, "sl_hop_cfg_comm_r12_s::num_subbands_r12_e_");
}
uint8_t sl_hop_cfg_comm_r12_s::num_subbands_r12_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4};
  return map_enum_number(options, 3, value, "sl_hop_cfg_comm_r12_s::num_subbands_r12_e_");
}

std::string sl_inter_freq_info_v2x_r14_s::sl_bw_r14_opts::to_string() const
{
  static const char* options[] = {"n6", "n15", "n25", "n50", "n75", "n100"};
  return convert_enum_idx(options, 6, value, "sl_inter_freq_info_v2x_r14_s::sl_bw_r14_e_");
}
uint8_t sl_inter_freq_info_v2x_r14_s::sl_bw_r14_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(options, 6, value, "sl_inter_freq_info_v2x_r14_s::sl_bw_r14_e_");
}

std::string sl_inter_freq_info_v2x_r14_s::add_spec_emission_v2x_r14_c_::types_opts::to_string() const
{
  static const char* options[] = {"additionalSpectrumEmission-r14", "additionalSpectrumEmission-v1440"};
  return convert_enum_idx(options, 2, value, "sl_inter_freq_info_v2x_r14_s::add_spec_emission_v2x_r14_c_::types");
}

// SL-PeriodComm-r12 ::= ENUMERATED
std::string sl_period_comm_r12_opts::to_string() const
{
  static const char* options[] = {"sf40",
                                  "sf60",
                                  "sf70",
                                  "sf80",
                                  "sf120",
                                  "sf140",
                                  "sf160",
                                  "sf240",
                                  "sf280",
                                  "sf320",
                                  "spare6",
                                  "spare5",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare"};
  return convert_enum_idx(options, 16, value, "sl_period_comm_r12_e");
}
uint16_t sl_period_comm_r12_opts::to_number() const
{
  static const uint16_t options[] = {40, 60, 70, 80, 120, 140, 160, 240, 280, 320};
  return map_enum_number(options, 10, value, "sl_period_comm_r12_e");
}

std::string sl_sync_cfg_r12_s::rx_params_ncell_r12_s_::disc_sync_win_r12_opts::to_string() const
{
  static const char* options[] = {"w1", "w2"};
  return convert_enum_idx(options, 2, value, "sl_sync_cfg_r12_s::rx_params_ncell_r12_s_::disc_sync_win_r12_e_");
}
uint8_t sl_sync_cfg_r12_s::rx_params_ncell_r12_s_::disc_sync_win_r12_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "sl_sync_cfg_r12_s::rx_params_ncell_r12_s_::disc_sync_win_r12_e_");
}

std::string sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_win_len_br_r13_opts::to_string() const
{
  static const char* options[] = {"ms20", "ms40", "ms60", "ms80", "ms120", "ms160", "ms200", "spare"};
  return convert_enum_idx(
      options, 8, value, "sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_win_len_br_r13_e_");
}
uint8_t sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_win_len_br_r13_opts::to_number() const
{
  static const uint8_t options[] = {20, 40, 60, 80, 120, 160, 200};
  return map_enum_number(
      options, 7, value, "sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_win_len_br_r13_e_");
}

std::string sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_repeat_pattern_r13_opts::to_string() const
{
  static const char* options[] = {"everyRF", "every2ndRF", "every4thRF", "every8thRF"};
  return convert_enum_idx(
      options, 4, value, "sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_repeat_pattern_r13_e_");
}
uint8_t sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_repeat_pattern_r13_opts::to_number() const
{
  switch (value) {
    case every2nd_rf:
      return 2;
    case every4th_rf:
      return 4;
    case every8th_rf:
      return 8;
    default:
      invalid_enum_number(value,
                          "sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_repeat_pattern_r13_e_");
  }
  return 0;
}

std::string
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

std::string sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_hop_cfg_common_r13_opts::to_string() const
{
  static const char* options[] = {"on", "off"};
  return convert_enum_idx(
      options, 2, value, "sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_hop_cfg_common_r13_e_");
}

std::string udt_restricting_r13_s::udt_restricting_time_r13_opts::to_string() const
{
  static const char* options[] = {"s4", "s8", "s16", "s32", "s64", "s128", "s256", "s512"};
  return convert_enum_idx(options, 8, value, "udt_restricting_r13_s::udt_restricting_time_r13_e_");
}
uint16_t udt_restricting_r13_s::udt_restricting_time_r13_opts::to_number() const
{
  static const uint16_t options[] = {4, 8, 16, 32, 64, 128, 256, 512};
  return map_enum_number(options, 8, value, "udt_restricting_r13_s::udt_restricting_time_r13_e_");
}

// WLAN-backhaulRate-r12 ::= ENUMERATED
std::string wlan_backhaul_rate_r12_opts::to_string() const
{
  static const char* options[] = {
      "r0",        "r4",        "r8",         "r16",        "r32",        "r64",         "r128",        "r256",
      "r512",      "r1024",     "r2048",      "r4096",      "r8192",      "r16384",      "r32768",      "r65536",
      "r131072",   "r262144",   "r524288",    "r1048576",   "r2097152",   "r4194304",    "r8388608",    "r16777216",
      "r33554432", "r67108864", "r134217728", "r268435456", "r536870912", "r1073741824", "r2147483648", "r4294967296"};
  return convert_enum_idx(options, 32, value, "wlan_backhaul_rate_r12_e");
}
uint64_t wlan_backhaul_rate_r12_opts::to_number() const
{
  static const uint64_t options[] = {0,         4,          8,          16,        32,       64,        128,
                                     256,       512,        1024,       2048,      4096,     8192,      16384,
                                     32768,     65536,      131072,     262144,    524288,   1048576,   2097152,
                                     4194304,   8388608,    16777216,   33554432,  67108864, 134217728, 268435456,
                                     536870912, 1073741824, 2147483648, 4294967296};
  return map_enum_number(options, 32, value, "wlan_backhaul_rate_r12_e");
}

std::string bcch_cfg_s::mod_period_coeff_opts::to_string() const
{
  static const char* options[] = {"n2", "n4", "n8", "n16"};
  return convert_enum_idx(options, 4, value, "bcch_cfg_s::mod_period_coeff_e_");
}
uint8_t bcch_cfg_s::mod_period_coeff_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 8, 16};
  return map_enum_number(options, 4, value, "bcch_cfg_s::mod_period_coeff_e_");
}

std::string carrier_freq_nr_r15_s::subcarrier_spacing_ssb_r15_opts::to_string() const
{
  static const char* options[] = {"kHz15", "kHz30", "kHz120", "kHz240"};
  return convert_enum_idx(options, 4, value, "carrier_freq_nr_r15_s::subcarrier_spacing_ssb_r15_e_");
}
uint8_t carrier_freq_nr_r15_s::subcarrier_spacing_ssb_r15_opts::to_number() const
{
  static const uint8_t options[] = {15, 30, 120, 240};
  return map_enum_number(options, 4, value, "carrier_freq_nr_r15_s::subcarrier_spacing_ssb_r15_e_");
}

std::string eab_cfg_r11_s::eab_category_r11_opts::to_string() const
{
  static const char* options[] = {"a", "b", "c"};
  return convert_enum_idx(options, 3, value, "eab_cfg_r11_s::eab_category_r11_e_");
}

std::string freq_hop_params_r13_s::dummy_opts::to_string() const
{
  static const char* options[] = {"nb2", "nb4"};
  return convert_enum_idx(options, 2, value, "freq_hop_params_r13_s::dummy_e_");
}
uint8_t freq_hop_params_r13_s::dummy_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "freq_hop_params_r13_s::dummy_e_");
}

std::string freq_hop_params_r13_s::dummy2_c_::interv_fdd_r13_opts::to_string() const
{
  static const char* options[] = {"int1", "int2", "int4", "int8"};
  return convert_enum_idx(options, 4, value, "freq_hop_params_r13_s::dummy2_c_::interv_fdd_r13_e_");
}
uint8_t freq_hop_params_r13_s::dummy2_c_::interv_fdd_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8};
  return map_enum_number(options, 4, value, "freq_hop_params_r13_s::dummy2_c_::interv_fdd_r13_e_");
}

std::string freq_hop_params_r13_s::dummy2_c_::interv_tdd_r13_opts::to_string() const
{
  static const char* options[] = {"int1", "int5", "int10", "int20"};
  return convert_enum_idx(options, 4, value, "freq_hop_params_r13_s::dummy2_c_::interv_tdd_r13_e_");
}
uint8_t freq_hop_params_r13_s::dummy2_c_::interv_tdd_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 5, 10, 20};
  return map_enum_number(options, 4, value, "freq_hop_params_r13_s::dummy2_c_::interv_tdd_r13_e_");
}

std::string freq_hop_params_r13_s::dummy2_c_::types_opts::to_string() const
{
  static const char* options[] = {"interval-FDD-r13", "interval-TDD-r13"};
  return convert_enum_idx(options, 2, value, "freq_hop_params_r13_s::dummy2_c_::types");
}

std::string freq_hop_params_r13_s::dummy3_c_::interv_fdd_r13_opts::to_string() const
{
  static const char* options[] = {"int2", "int4", "int8", "int16"};
  return convert_enum_idx(options, 4, value, "freq_hop_params_r13_s::dummy3_c_::interv_fdd_r13_e_");
}
uint8_t freq_hop_params_r13_s::dummy3_c_::interv_fdd_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 8, 16};
  return map_enum_number(options, 4, value, "freq_hop_params_r13_s::dummy3_c_::interv_fdd_r13_e_");
}

std::string freq_hop_params_r13_s::dummy3_c_::interv_tdd_r13_opts::to_string() const
{
  static const char* options[] = {"int5", "int10", "int20", "int40"};
  return convert_enum_idx(options, 4, value, "freq_hop_params_r13_s::dummy3_c_::interv_tdd_r13_e_");
}
uint8_t freq_hop_params_r13_s::dummy3_c_::interv_tdd_r13_opts::to_number() const
{
  static const uint8_t options[] = {5, 10, 20, 40};
  return map_enum_number(options, 4, value, "freq_hop_params_r13_s::dummy3_c_::interv_tdd_r13_e_");
}

std::string freq_hop_params_r13_s::dummy3_c_::types_opts::to_string() const
{
  static const char* options[] = {"interval-FDD-r13", "interval-TDD-r13"};
  return convert_enum_idx(options, 2, value, "freq_hop_params_r13_s::dummy3_c_::types");
}

std::string freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::interv_fdd_r13_opts::to_string() const
{
  static const char* options[] = {"int1", "int2", "int4", "int8"};
  return convert_enum_idx(
      options, 4, value, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::interv_fdd_r13_e_");
}
uint8_t freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::interv_fdd_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8};
  return map_enum_number(
      options, 4, value, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::interv_fdd_r13_e_");
}

std::string freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::interv_tdd_r13_opts::to_string() const
{
  static const char* options[] = {"int1", "int5", "int10", "int20"};
  return convert_enum_idx(
      options, 4, value, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::interv_tdd_r13_e_");
}
uint8_t freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::interv_tdd_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 5, 10, 20};
  return map_enum_number(
      options, 4, value, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::interv_tdd_r13_e_");
}

std::string freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"interval-FDD-r13", "interval-TDD-r13"};
  return convert_enum_idx(options, 2, value, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::types");
}

std::string freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::interv_fdd_r13_opts::to_string() const
{
  static const char* options[] = {"int2", "int4", "int8", "int16"};
  return convert_enum_idx(
      options, 4, value, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::interv_fdd_r13_e_");
}
uint8_t freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::interv_fdd_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 8, 16};
  return map_enum_number(
      options, 4, value, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::interv_fdd_r13_e_");
}

std::string freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::interv_tdd_r13_opts::to_string() const
{
  static const char* options[] = {"int5", "int10", "int20", "int40"};
  return convert_enum_idx(
      options, 4, value, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::interv_tdd_r13_e_");
}
uint8_t freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::interv_tdd_r13_opts::to_number() const
{
  static const uint8_t options[] = {5, 10, 20, 40};
  return map_enum_number(
      options, 4, value, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::interv_tdd_r13_e_");
}

std::string freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"interval-FDD-r13", "interval-TDD-r13"};
  return convert_enum_idx(options, 2, value, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::types");
}

std::string mbms_carrier_type_r14_s::carrier_type_r14_opts::to_string() const
{
  static const char* options[] = {"mbms", "fembmsMixed", "fembmsDedicated"};
  return convert_enum_idx(options, 3, value, "mbms_carrier_type_r14_s::carrier_type_r14_e_");
}

std::string mbsfn_area_info_r9_s::non_mbsfn_region_len_opts::to_string() const
{
  static const char* options[] = {"s1", "s2"};
  return convert_enum_idx(options, 2, value, "mbsfn_area_info_r9_s::non_mbsfn_region_len_e_");
}
uint8_t mbsfn_area_info_r9_s::non_mbsfn_region_len_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "mbsfn_area_info_r9_s::non_mbsfn_region_len_e_");
}

std::string mbsfn_area_info_r9_s::mcch_cfg_r9_s_::mcch_repeat_period_r9_opts::to_string() const
{
  static const char* options[] = {"rf32", "rf64", "rf128", "rf256"};
  return convert_enum_idx(options, 4, value, "mbsfn_area_info_r9_s::mcch_cfg_r9_s_::mcch_repeat_period_r9_e_");
}
uint16_t mbsfn_area_info_r9_s::mcch_cfg_r9_s_::mcch_repeat_period_r9_opts::to_number() const
{
  static const uint16_t options[] = {32, 64, 128, 256};
  return map_enum_number(options, 4, value, "mbsfn_area_info_r9_s::mcch_cfg_r9_s_::mcch_repeat_period_r9_e_");
}

std::string mbsfn_area_info_r9_s::mcch_cfg_r9_s_::mcch_mod_period_r9_opts::to_string() const
{
  static const char* options[] = {"rf512", "rf1024"};
  return convert_enum_idx(options, 2, value, "mbsfn_area_info_r9_s::mcch_cfg_r9_s_::mcch_mod_period_r9_e_");
}
uint16_t mbsfn_area_info_r9_s::mcch_cfg_r9_s_::mcch_mod_period_r9_opts::to_number() const
{
  static const uint16_t options[] = {512, 1024};
  return map_enum_number(options, 2, value, "mbsfn_area_info_r9_s::mcch_cfg_r9_s_::mcch_mod_period_r9_e_");
}

std::string mbsfn_area_info_r9_s::mcch_cfg_r9_s_::sig_mcs_r9_opts::to_string() const
{
  static const char* options[] = {"n2", "n7", "n13", "n19"};
  return convert_enum_idx(options, 4, value, "mbsfn_area_info_r9_s::mcch_cfg_r9_s_::sig_mcs_r9_e_");
}
uint8_t mbsfn_area_info_r9_s::mcch_cfg_r9_s_::sig_mcs_r9_opts::to_number() const
{
  static const uint8_t options[] = {2, 7, 13, 19};
  return map_enum_number(options, 4, value, "mbsfn_area_info_r9_s::mcch_cfg_r9_s_::sig_mcs_r9_e_");
}

std::string mbsfn_area_info_r9_s::mcch_cfg_r14_s_::mcch_repeat_period_v1430_opts::to_string() const
{
  static const char* options[] = {"rf1", "rf2", "rf4", "rf8", "rf16"};
  return convert_enum_idx(options, 5, value, "mbsfn_area_info_r9_s::mcch_cfg_r14_s_::mcch_repeat_period_v1430_e_");
}
uint8_t mbsfn_area_info_r9_s::mcch_cfg_r14_s_::mcch_repeat_period_v1430_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8, 16};
  return map_enum_number(options, 5, value, "mbsfn_area_info_r9_s::mcch_cfg_r14_s_::mcch_repeat_period_v1430_e_");
}

std::string mbsfn_area_info_r9_s::mcch_cfg_r14_s_::mcch_mod_period_v1430_opts::to_string() const
{
  static const char* options[] = {"rf1", "rf2", "rf4", "rf8", "rf16", "rf32", "rf64", "rf128", "rf256", "spare7"};
  return convert_enum_idx(options, 10, value, "mbsfn_area_info_r9_s::mcch_cfg_r14_s_::mcch_mod_period_v1430_e_");
}
uint16_t mbsfn_area_info_r9_s::mcch_cfg_r14_s_::mcch_mod_period_v1430_opts::to_number() const
{
  static const uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
  return map_enum_number(options, 9, value, "mbsfn_area_info_r9_s::mcch_cfg_r14_s_::mcch_mod_period_v1430_e_");
}

std::string mbsfn_area_info_r9_s::subcarrier_spacing_mbms_r14_opts::to_string() const
{
  static const char* options[] = {"khz-7dot5", "khz-1dot25"};
  return convert_enum_idx(options, 2, value, "mbsfn_area_info_r9_s::subcarrier_spacing_mbms_r14_e_");
}
float mbsfn_area_info_r9_s::subcarrier_spacing_mbms_r14_opts::to_number() const
{
  static const float options[] = {-7.5, -1.25};
  return map_enum_number(options, 2, value, "mbsfn_area_info_r9_s::subcarrier_spacing_mbms_r14_e_");
}
std::string mbsfn_area_info_r9_s::subcarrier_spacing_mbms_r14_opts::to_number_string() const
{
  static const char* options[] = {"-7.5", "-1.25"};
  return convert_enum_idx(options, 2, value, "mbsfn_area_info_r9_s::subcarrier_spacing_mbms_r14_e_");
}

std::string mbsfn_sf_cfg_s::radioframe_alloc_period_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n4", "n8", "n16", "n32"};
  return convert_enum_idx(options, 6, value, "mbsfn_sf_cfg_s::radioframe_alloc_period_e_");
}
uint8_t mbsfn_sf_cfg_s::radioframe_alloc_period_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8, 16, 32};
  return map_enum_number(options, 6, value, "mbsfn_sf_cfg_s::radioframe_alloc_period_e_");
}

std::string mbsfn_sf_cfg_s::sf_alloc_c_::types_opts::to_string() const
{
  static const char* options[] = {"oneFrame", "fourFrames"};
  return convert_enum_idx(options, 2, value, "mbsfn_sf_cfg_s::sf_alloc_c_::types");
}
uint8_t mbsfn_sf_cfg_s::sf_alloc_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1, 4};
  return map_enum_number(options, 2, value, "mbsfn_sf_cfg_s::sf_alloc_c_::types");
}

std::string mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_::types_opts::to_string() const
{
  static const char* options[] = {"oneFrame-v1430", "fourFrames-v1430"};
  return convert_enum_idx(options, 2, value, "mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_::types");
}
uint8_t mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1, 4};
  return map_enum_number(options, 2, value, "mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_::types");
}

std::string pcch_cfg_s::default_paging_cycle_opts::to_string() const
{
  static const char* options[] = {"rf32", "rf64", "rf128", "rf256"};
  return convert_enum_idx(options, 4, value, "pcch_cfg_s::default_paging_cycle_e_");
}
uint16_t pcch_cfg_s::default_paging_cycle_opts::to_number() const
{
  static const uint16_t options[] = {32, 64, 128, 256};
  return map_enum_number(options, 4, value, "pcch_cfg_s::default_paging_cycle_e_");
}

std::string pcch_cfg_s::nb_opts::to_string() const
{
  static const char* options[] = {
      "fourT", "twoT", "oneT", "halfT", "quarterT", "oneEighthT", "oneSixteenthT", "oneThirtySecondT"};
  return convert_enum_idx(options, 8, value, "pcch_cfg_s::nb_e_");
}
float pcch_cfg_s::nb_opts::to_number() const
{
  static const float options[] = {4.0, 2.0, 1.0, 0.5, 0.25, 0.125, 0.0625, 0.03125};
  return map_enum_number(options, 8, value, "pcch_cfg_s::nb_e_");
}
std::string pcch_cfg_s::nb_opts::to_number_string() const
{
  static const char* options[] = {"4", "2", "1", "0.5", "0.25", "1/8", "1/16", "1/32"};
  return convert_enum_idx(options, 8, value, "pcch_cfg_s::nb_e_");
}

std::string pcch_cfg_v1310_s::mpdcch_num_repeat_paging_r13_opts::to_string() const
{
  static const char* options[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "r128", "r256"};
  return convert_enum_idx(options, 9, value, "pcch_cfg_v1310_s::mpdcch_num_repeat_paging_r13_e_");
}
uint16_t pcch_cfg_v1310_s::mpdcch_num_repeat_paging_r13_opts::to_number() const
{
  static const uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
  return map_enum_number(options, 9, value, "pcch_cfg_v1310_s::mpdcch_num_repeat_paging_r13_e_");
}

std::string pcch_cfg_v1310_s::nb_v1310_opts::to_string() const
{
  static const char* options[] = {"one64thT", "one128thT", "one256thT"};
  return convert_enum_idx(options, 3, value, "pcch_cfg_v1310_s::nb_v1310_e_");
}
uint16_t pcch_cfg_v1310_s::nb_v1310_opts::to_number() const
{
  static const uint16_t options[] = {64, 128, 256};
  return map_enum_number(options, 3, value, "pcch_cfg_v1310_s::nb_v1310_e_");
}

std::string pdsch_cfg_common_v1310_s::pdsch_max_num_repeat_cemode_a_r13_opts::to_string() const
{
  static const char* options[] = {"r16", "r32"};
  return convert_enum_idx(options, 2, value, "pdsch_cfg_common_v1310_s::pdsch_max_num_repeat_cemode_a_r13_e_");
}
uint8_t pdsch_cfg_common_v1310_s::pdsch_max_num_repeat_cemode_a_r13_opts::to_number() const
{
  static const uint8_t options[] = {16, 32};
  return map_enum_number(options, 2, value, "pdsch_cfg_common_v1310_s::pdsch_max_num_repeat_cemode_a_r13_e_");
}

std::string pdsch_cfg_common_v1310_s::pdsch_max_num_repeat_cemode_b_r13_opts::to_string() const
{
  static const char* options[] = {"r192", "r256", "r384", "r512", "r768", "r1024", "r1536", "r2048"};
  return convert_enum_idx(options, 8, value, "pdsch_cfg_common_v1310_s::pdsch_max_num_repeat_cemode_b_r13_e_");
}
uint16_t pdsch_cfg_common_v1310_s::pdsch_max_num_repeat_cemode_b_r13_opts::to_number() const
{
  static const uint16_t options[] = {192, 256, 384, 512, 768, 1024, 1536, 2048};
  return map_enum_number(options, 8, value, "pdsch_cfg_common_v1310_s::pdsch_max_num_repeat_cemode_b_r13_e_");
}

std::string prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_opts::to_string() const
{
  static const char* options[] = {"v1", "v1dot5", "v2", "v2dot5", "v4", "v5", "v8", "v10"};
  return convert_enum_idx(options, 8, value, "prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_e_");
}
float prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_opts::to_number() const
{
  static const float options[] = {1.0, 1.5, 2.0, 2.5, 4.0, 5.0, 8.0, 10.0};
  return map_enum_number(options, 8, value, "prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_e_");
}
std::string prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_opts::to_number_string() const
{
  static const char* options[] = {"1", "1.5", "2", "2.5", "4", "5", "8", "10"};
  return convert_enum_idx(options, 8, value, "prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_e_");
}

std::string prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::tdd_r13_opts::to_string() const
{
  static const char* options[] = {"v1", "v2", "v4", "v5", "v8", "v10", "v20", "spare"};
  return convert_enum_idx(options, 8, value, "prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::tdd_r13_e_");
}
uint8_t prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::tdd_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 5, 8, 10, 20};
  return map_enum_number(options, 7, value, "prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::tdd_r13_e_");
}

std::string prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"fdd-r13", "tdd-r13"};
  return convert_enum_idx(options, 2, value, "prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::types");
}

std::string pucch_cfg_common_s::delta_pucch_shift_opts::to_string() const
{
  static const char* options[] = {"ds1", "ds2", "ds3"};
  return convert_enum_idx(options, 3, value, "pucch_cfg_common_s::delta_pucch_shift_e_");
}
uint8_t pucch_cfg_common_s::delta_pucch_shift_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3};
  return map_enum_number(options, 3, value, "pucch_cfg_common_s::delta_pucch_shift_e_");
}

std::string pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level0_r13_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n4", "n8"};
  return convert_enum_idx(options, 4, value, "pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level0_r13_e_");
}
uint8_t pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level0_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8};
  return map_enum_number(options, 4, value, "pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level0_r13_e_");
}

std::string pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level1_r13_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n4", "n8"};
  return convert_enum_idx(options, 4, value, "pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level1_r13_e_");
}
uint8_t pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level1_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8};
  return map_enum_number(options, 4, value, "pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level1_r13_e_");
}

std::string pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level2_r13_opts::to_string() const
{
  static const char* options[] = {"n4", "n8", "n16", "n32"};
  return convert_enum_idx(options, 4, value, "pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level2_r13_e_");
}
uint8_t pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level2_r13_opts::to_number() const
{
  static const uint8_t options[] = {4, 8, 16, 32};
  return map_enum_number(options, 4, value, "pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level2_r13_e_");
}

std::string pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level3_r13_opts::to_string() const
{
  static const char* options[] = {"n4", "n8", "n16", "n32"};
  return convert_enum_idx(options, 4, value, "pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level3_r13_e_");
}
uint8_t pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level3_r13_opts::to_number() const
{
  static const uint8_t options[] = {4, 8, 16, 32};
  return map_enum_number(options, 4, value, "pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level3_r13_e_");
}

std::string pucch_cfg_common_v1430_s::pucch_num_repeat_ce_msg4_level3_r14_opts::to_string() const
{
  static const char* options[] = {"n64", "n128"};
  return convert_enum_idx(options, 2, value, "pucch_cfg_common_v1430_s::pucch_num_repeat_ce_msg4_level3_r14_e_");
}
uint8_t pucch_cfg_common_v1430_s::pucch_num_repeat_ce_msg4_level3_r14_opts::to_number() const
{
  static const uint8_t options[] = {64, 128};
  return map_enum_number(options, 2, value, "pucch_cfg_common_v1430_s::pucch_num_repeat_ce_msg4_level3_r14_e_");
}

std::string pusch_cfg_common_s::pusch_cfg_basic_s_::hop_mode_opts::to_string() const
{
  static const char* options[] = {"interSubFrame", "intraAndInterSubFrame"};
  return convert_enum_idx(options, 2, value, "pusch_cfg_common_s::pusch_cfg_basic_s_::hop_mode_e_");
}

std::string pusch_cfg_common_v1310_s::pusch_max_num_repeat_cemode_a_r13_opts::to_string() const
{
  static const char* options[] = {"r8", "r16", "r32"};
  return convert_enum_idx(options, 3, value, "pusch_cfg_common_v1310_s::pusch_max_num_repeat_cemode_a_r13_e_");
}
uint8_t pusch_cfg_common_v1310_s::pusch_max_num_repeat_cemode_a_r13_opts::to_number() const
{
  static const uint8_t options[] = {8, 16, 32};
  return map_enum_number(options, 3, value, "pusch_cfg_common_v1310_s::pusch_max_num_repeat_cemode_a_r13_e_");
}

std::string pusch_cfg_common_v1310_s::pusch_max_num_repeat_cemode_b_r13_opts::to_string() const
{
  static const char* options[] = {"r192", "r256", "r384", "r512", "r768", "r1024", "r1536", "r2048"};
  return convert_enum_idx(options, 8, value, "pusch_cfg_common_v1310_s::pusch_max_num_repeat_cemode_b_r13_e_");
}
uint16_t pusch_cfg_common_v1310_s::pusch_max_num_repeat_cemode_b_r13_opts::to_number() const
{
  static const uint16_t options[] = {192, 256, 384, 512, 768, 1024, 1536, 2048};
  return map_enum_number(options, 8, value, "pusch_cfg_common_v1310_s::pusch_max_num_repeat_cemode_b_r13_e_");
}

std::string rach_cfg_common_s::preamb_info_s_::nof_ra_preambs_opts::to_string() const
{
  static const char* options[] = {
      "n4", "n8", "n12", "n16", "n20", "n24", "n28", "n32", "n36", "n40", "n44", "n48", "n52", "n56", "n60", "n64"};
  return convert_enum_idx(options, 16, value, "rach_cfg_common_s::preamb_info_s_::nof_ra_preambs_e_");
}
uint8_t rach_cfg_common_s::preamb_info_s_::nof_ra_preambs_opts::to_number() const
{
  static const uint8_t options[] = {4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64};
  return map_enum_number(options, 16, value, "rach_cfg_common_s::preamb_info_s_::nof_ra_preambs_e_");
}

std::string
rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::size_of_ra_preambs_group_a_opts::to_string() const
{
  static const char* options[] = {
      "n4", "n8", "n12", "n16", "n20", "n24", "n28", "n32", "n36", "n40", "n44", "n48", "n52", "n56", "n60"};
  return convert_enum_idx(
      options, 15, value, "rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::size_of_ra_preambs_group_a_e_");
}
uint8_t rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::size_of_ra_preambs_group_a_opts::to_number() const
{
  static const uint8_t options[] = {4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60};
  return map_enum_number(
      options, 15, value, "rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::size_of_ra_preambs_group_a_e_");
}

std::string rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::msg_size_group_a_opts::to_string() const
{
  static const char* options[] = {"b56", "b144", "b208", "b256"};
  return convert_enum_idx(
      options, 4, value, "rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::msg_size_group_a_e_");
}
uint16_t rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::msg_size_group_a_opts::to_number() const
{
  static const uint16_t options[] = {56, 144, 208, 256};
  return map_enum_number(
      options, 4, value, "rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::msg_size_group_a_e_");
}

std::string rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::msg_pwr_offset_group_b_opts::to_string() const
{
  static const char* options[] = {"minusinfinity", "dB0", "dB5", "dB8", "dB10", "dB12", "dB15", "dB18"};
  return convert_enum_idx(
      options, 8, value, "rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::msg_pwr_offset_group_b_e_");
}
int8_t rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::msg_pwr_offset_group_b_opts::to_number() const
{
  static const int8_t options[] = {-1, 0, 5, 8, 10, 12, 15, 18};
  return map_enum_number(
      options, 8, value, "rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::msg_pwr_offset_group_b_e_");
}

std::string rach_cfg_common_s::ra_supervision_info_s_::ra_resp_win_size_opts::to_string() const
{
  static const char* options[] = {"sf2", "sf3", "sf4", "sf5", "sf6", "sf7", "sf8", "sf10"};
  return convert_enum_idx(options, 8, value, "rach_cfg_common_s::ra_supervision_info_s_::ra_resp_win_size_e_");
}
uint8_t rach_cfg_common_s::ra_supervision_info_s_::ra_resp_win_size_opts::to_number() const
{
  static const uint8_t options[] = {2, 3, 4, 5, 6, 7, 8, 10};
  return map_enum_number(options, 8, value, "rach_cfg_common_s::ra_supervision_info_s_::ra_resp_win_size_e_");
}

std::string rach_cfg_common_s::ra_supervision_info_s_::mac_contention_resolution_timer_opts::to_string() const
{
  static const char* options[] = {"sf8", "sf16", "sf24", "sf32", "sf40", "sf48", "sf56", "sf64"};
  return convert_enum_idx(
      options, 8, value, "rach_cfg_common_s::ra_supervision_info_s_::mac_contention_resolution_timer_e_");
}
uint8_t rach_cfg_common_s::ra_supervision_info_s_::mac_contention_resolution_timer_opts::to_number() const
{
  static const uint8_t options[] = {8, 16, 24, 32, 40, 48, 56, 64};
  return map_enum_number(
      options, 8, value, "rach_cfg_common_s::ra_supervision_info_s_::mac_contention_resolution_timer_e_");
}

std::string rach_cfg_common_v1250_s::tx_fail_params_r12_s_::conn_est_fail_count_r12_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4"};
  return convert_enum_idx(
      options, 4, value, "rach_cfg_common_v1250_s::tx_fail_params_r12_s_::conn_est_fail_count_r12_e_");
}
uint8_t rach_cfg_common_v1250_s::tx_fail_params_r12_s_::conn_est_fail_count_r12_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4};
  return map_enum_number(
      options, 4, value, "rach_cfg_common_v1250_s::tx_fail_params_r12_s_::conn_est_fail_count_r12_e_");
}

std::string rach_cfg_common_v1250_s::tx_fail_params_r12_s_::conn_est_fail_offset_validity_r12_opts::to_string() const
{
  static const char* options[] = {"s30", "s60", "s120", "s240", "s300", "s420", "s600", "s900"};
  return convert_enum_idx(
      options, 8, value, "rach_cfg_common_v1250_s::tx_fail_params_r12_s_::conn_est_fail_offset_validity_r12_e_");
}
uint16_t rach_cfg_common_v1250_s::tx_fail_params_r12_s_::conn_est_fail_offset_validity_r12_opts::to_number() const
{
  static const uint16_t options[] = {30, 60, 120, 240, 300, 420, 600, 900};
  return map_enum_number(
      options, 8, value, "rach_cfg_common_v1250_s::tx_fail_params_r12_s_::conn_est_fail_offset_validity_r12_e_");
}

std::string rss_cfg_r15_s::dur_r15_opts::to_string() const
{
  static const char* options[] = {"sf8", "sf16", "sf32", "sf40"};
  return convert_enum_idx(options, 4, value, "rss_cfg_r15_s::dur_r15_e_");
}
uint8_t rss_cfg_r15_s::dur_r15_opts::to_number() const
{
  static const uint8_t options[] = {8, 16, 32, 40};
  return map_enum_number(options, 4, value, "rss_cfg_r15_s::dur_r15_e_");
}

std::string rss_cfg_r15_s::periodicity_r15_opts::to_string() const
{
  static const char* options[] = {"ms160", "ms320", "ms640", "ms1280"};
  return convert_enum_idx(options, 4, value, "rss_cfg_r15_s::periodicity_r15_e_");
}
uint16_t rss_cfg_r15_s::periodicity_r15_opts::to_number() const
{
  static const uint16_t options[] = {160, 320, 640, 1280};
  return map_enum_number(options, 4, value, "rss_cfg_r15_s::periodicity_r15_e_");
}

std::string rss_cfg_r15_s::pwr_boost_r15_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB3", "dB4dot8", "dB6"};
  return convert_enum_idx(options, 4, value, "rss_cfg_r15_s::pwr_boost_r15_e_");
}
float rss_cfg_r15_s::pwr_boost_r15_opts::to_number() const
{
  static const float options[] = {0.0, 3.0, 4.8, 6.0};
  return map_enum_number(options, 4, value, "rss_cfg_r15_s::pwr_boost_r15_e_");
}
std::string rss_cfg_r15_s::pwr_boost_r15_opts::to_number_string() const
{
  static const char* options[] = {"0", "3", "4.8", "6"};
  return convert_enum_idx(options, 4, value, "rss_cfg_r15_s::pwr_boost_r15_e_");
}

std::string resel_info_relay_r13_s::min_hyst_r13_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB3", "dB6", "dB9", "dB12", "dBinf"};
  return convert_enum_idx(options, 6, value, "resel_info_relay_r13_s::min_hyst_r13_e_");
}
uint8_t resel_info_relay_r13_s::min_hyst_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 3, 6, 9, 12};
  return map_enum_number(options, 5, value, "resel_info_relay_r13_s::min_hyst_r13_e_");
}

// SIB-Type ::= ENUMERATED
std::string sib_type_opts::to_string() const
{
  static const char* options[] = {
      "sibType3",        "sibType4",        "sibType5",        "sibType6",        "sibType7",        "sibType8",
      "sibType9",        "sibType10",       "sibType11",       "sibType12-v920",  "sibType13-v920",  "sibType14-v1130",
      "sibType15-v1130", "sibType16-v1130", "sibType17-v1250", "sibType18-v1250", "sibType19-v1250", "sibType20-v1310",
      "sibType21-v1430", "sibType24-v1530", "sibType25-v1530", "sibType26-v1530"};
  return convert_enum_idx(options, 22, value, "sib_type_e");
}
uint8_t sib_type_opts::to_number() const
{
  static const uint8_t options[] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 24, 25, 26};
  return map_enum_number(options, 22, value, "sib_type_e");
}

std::string sib8_per_plmn_r11_s::params_cdma2000_r11_c_::types_opts::to_string() const
{
  static const char* options[] = {"explicitValue", "defaultValue"};
  return convert_enum_idx(options, 2, value, "sib8_per_plmn_r11_s::params_cdma2000_r11_c_::types");
}

std::string srs_ul_cfg_common_c::setup_s_::srs_bw_cfg_opts::to_string() const
{
  static const char* options[] = {"bw0", "bw1", "bw2", "bw3", "bw4", "bw5", "bw6", "bw7"};
  return convert_enum_idx(options, 8, value, "srs_ul_cfg_common_c::setup_s_::srs_bw_cfg_e_");
}
uint8_t srs_ul_cfg_common_c::setup_s_::srs_bw_cfg_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7};
  return map_enum_number(options, 8, value, "srs_ul_cfg_common_c::setup_s_::srs_bw_cfg_e_");
}

std::string srs_ul_cfg_common_c::setup_s_::srs_sf_cfg_opts::to_string() const
{
  static const char* options[] = {"sc0",
                                  "sc1",
                                  "sc2",
                                  "sc3",
                                  "sc4",
                                  "sc5",
                                  "sc6",
                                  "sc7",
                                  "sc8",
                                  "sc9",
                                  "sc10",
                                  "sc11",
                                  "sc12",
                                  "sc13",
                                  "sc14",
                                  "sc15"};
  return convert_enum_idx(options, 16, value, "srs_ul_cfg_common_c::setup_s_::srs_sf_cfg_e_");
}
uint8_t srs_ul_cfg_common_c::setup_s_::srs_sf_cfg_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  return map_enum_number(options, 16, value, "srs_ul_cfg_common_c::setup_s_::srs_sf_cfg_e_");
}

std::string uac_barr_info_set_r15_s::uac_barr_factor_r15_opts::to_string() const
{
  static const char* options[] = {
      "p00", "p05", "p10", "p15", "p20", "p25", "p30", "p40", "p50", "p60", "p70", "p75", "p80", "p85", "p90", "p95"};
  return convert_enum_idx(options, 16, value, "uac_barr_info_set_r15_s::uac_barr_factor_r15_e_");
}
float uac_barr_info_set_r15_s::uac_barr_factor_r15_opts::to_number() const
{
  static const float options[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 6.0, 7.0, 7.5, 8.0, 8.5, 9.0, 9.5};
  return map_enum_number(options, 16, value, "uac_barr_info_set_r15_s::uac_barr_factor_r15_e_");
}
std::string uac_barr_info_set_r15_s::uac_barr_factor_r15_opts::to_number_string() const
{
  static const char* options[] = {
      "0.0", "0.5", "1.0", "1.5", "2.0", "2.5", "3.0", "4.0", "5.0", "6.0", "7.0", "7.5", "8.0", "8.5", "9.0", "9.5"};
  return convert_enum_idx(options, 16, value, "uac_barr_info_set_r15_s::uac_barr_factor_r15_e_");
}

std::string uac_barr_info_set_r15_s::uac_barr_time_r15_opts::to_string() const
{
  static const char* options[] = {"s4", "s8", "s16", "s32", "s64", "s128", "s256", "s512"};
  return convert_enum_idx(options, 8, value, "uac_barr_info_set_r15_s::uac_barr_time_r15_e_");
}
uint16_t uac_barr_info_set_r15_s::uac_barr_time_r15_opts::to_number() const
{
  static const uint16_t options[] = {4, 8, 16, 32, 64, 128, 256, 512};
  return map_enum_number(options, 8, value, "uac_barr_info_set_r15_s::uac_barr_time_r15_e_");
}

std::string uac_barr_per_plmn_r15_s::uac_ac_barr_list_type_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"uac-ImplicitAC-BarringList-r15", "uac-ExplicitAC-BarringList-r15"};
  return convert_enum_idx(options, 2, value, "uac_barr_per_plmn_r15_s::uac_ac_barr_list_type_r15_c_::types");
}

// UL-CyclicPrefixLength ::= ENUMERATED
std::string ul_cp_len_opts::to_string() const
{
  static const char* options[] = {"len1", "len2"};
  return convert_enum_idx(options, 2, value, "ul_cp_len_e");
}
uint8_t ul_cp_len_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "ul_cp_len_e");
}

std::string ul_pwr_ctrl_common_v1020_s::delta_f_pucch_format3_r10_opts::to_string() const
{
  static const char* options[] = {
      "deltaF-1", "deltaF0", "deltaF1", "deltaF2", "deltaF3", "deltaF4", "deltaF5", "deltaF6"};
  return convert_enum_idx(options, 8, value, "ul_pwr_ctrl_common_v1020_s::delta_f_pucch_format3_r10_e_");
}
int8_t ul_pwr_ctrl_common_v1020_s::delta_f_pucch_format3_r10_opts::to_number() const
{
  static const int8_t options[] = {-1, 0, 1, 2, 3, 4, 5, 6};
  return map_enum_number(options, 8, value, "ul_pwr_ctrl_common_v1020_s::delta_f_pucch_format3_r10_e_");
}

std::string ul_pwr_ctrl_common_v1020_s::delta_f_pucch_format1b_cs_r10_opts::to_string() const
{
  static const char* options[] = {"deltaF1", "deltaF2", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ul_pwr_ctrl_common_v1020_s::delta_f_pucch_format1b_cs_r10_e_");
}
uint8_t ul_pwr_ctrl_common_v1020_s::delta_f_pucch_format1b_cs_r10_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "ul_pwr_ctrl_common_v1020_s::delta_f_pucch_format1b_cs_r10_e_");
}

std::string wus_cfg_r15_s::max_dur_factor_r15_opts::to_string() const
{
  static const char* options[] = {"one32th", "one16th", "one8th", "one4th"};
  return convert_enum_idx(options, 4, value, "wus_cfg_r15_s::max_dur_factor_r15_e_");
}
uint8_t wus_cfg_r15_s::max_dur_factor_r15_opts::to_number() const
{
  static const uint8_t options[] = {32, 16, 8, 4};
  return map_enum_number(options, 4, value, "wus_cfg_r15_s::max_dur_factor_r15_e_");
}

std::string wus_cfg_r15_s::num_pos_r15_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n4", "spare1"};
  return convert_enum_idx(options, 4, value, "wus_cfg_r15_s::num_pos_r15_e_");
}
uint8_t wus_cfg_r15_s::num_pos_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4};
  return map_enum_number(options, 3, value, "wus_cfg_r15_s::num_pos_r15_e_");
}

std::string wus_cfg_r15_s::freq_location_r15_opts::to_string() const
{
  static const char* options[] = {"n0", "n2", "n4", "spare1"};
  return convert_enum_idx(options, 4, value, "wus_cfg_r15_s::freq_location_r15_e_");
}
uint8_t wus_cfg_r15_s::freq_location_r15_opts::to_number() const
{
  static const uint8_t options[] = {0, 2, 4};
  return map_enum_number(options, 3, value, "wus_cfg_r15_s::freq_location_r15_e_");
}

std::string wus_cfg_r15_s::time_offset_drx_r15_opts::to_string() const
{
  static const char* options[] = {"ms40", "ms80", "ms160", "ms240"};
  return convert_enum_idx(options, 4, value, "wus_cfg_r15_s::time_offset_drx_r15_e_");
}
uint8_t wus_cfg_r15_s::time_offset_drx_r15_opts::to_number() const
{
  static const uint8_t options[] = {40, 80, 160, 240};
  return map_enum_number(options, 4, value, "wus_cfg_r15_s::time_offset_drx_r15_e_");
}

std::string wus_cfg_r15_s::time_offset_e_drx_short_r15_opts::to_string() const
{
  static const char* options[] = {"ms40", "ms80", "ms160", "ms240"};
  return convert_enum_idx(options, 4, value, "wus_cfg_r15_s::time_offset_e_drx_short_r15_e_");
}
uint8_t wus_cfg_r15_s::time_offset_e_drx_short_r15_opts::to_number() const
{
  static const uint8_t options[] = {40, 80, 160, 240};
  return map_enum_number(options, 4, value, "wus_cfg_r15_s::time_offset_e_drx_short_r15_e_");
}

std::string wus_cfg_r15_s::time_offset_e_drx_long_r15_opts::to_string() const
{
  static const char* options[] = {"ms1000", "ms2000"};
  return convert_enum_idx(options, 2, value, "wus_cfg_r15_s::time_offset_e_drx_long_r15_e_");
}
uint16_t wus_cfg_r15_s::time_offset_e_drx_long_r15_opts::to_number() const
{
  static const uint16_t options[] = {1000, 2000};
  return map_enum_number(options, 2, value, "wus_cfg_r15_s::time_offset_e_drx_long_r15_e_");
}

std::string cell_resel_info_common_v1460_s::s_search_delta_p_r14_opts::to_string() const
{
  static const char* options[] = {"dB6", "dB9", "dB12", "dB15"};
  return convert_enum_idx(options, 4, value, "cell_resel_info_common_v1460_s::s_search_delta_p_r14_e_");
}
uint8_t cell_resel_info_common_v1460_s::s_search_delta_p_r14_opts::to_number() const
{
  static const uint8_t options[] = {6, 9, 12, 15};
  return map_enum_number(options, 4, value, "cell_resel_info_common_v1460_s::s_search_delta_p_r14_e_");
}

std::string mbms_notif_cfg_r9_s::notif_repeat_coeff_r9_opts::to_string() const
{
  static const char* options[] = {"n2", "n4"};
  return convert_enum_idx(options, 2, value, "mbms_notif_cfg_r9_s::notif_repeat_coeff_r9_e_");
}
uint8_t mbms_notif_cfg_r9_s::notif_repeat_coeff_r9_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "mbms_notif_cfg_r9_s::notif_repeat_coeff_r9_e_");
}

std::string mob_state_params_s::t_eval_opts::to_string() const
{
  static const char* options[] = {"s30", "s60", "s120", "s180", "s240", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "mob_state_params_s::t_eval_e_");
}
uint8_t mob_state_params_s::t_eval_opts::to_number() const
{
  static const uint8_t options[] = {30, 60, 120, 180, 240};
  return map_enum_number(options, 5, value, "mob_state_params_s::t_eval_e_");
}

std::string mob_state_params_s::t_hyst_normal_opts::to_string() const
{
  static const char* options[] = {"s30", "s60", "s120", "s180", "s240", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "mob_state_params_s::t_hyst_normal_e_");
}
uint8_t mob_state_params_s::t_hyst_normal_opts::to_number() const
{
  static const uint8_t options[] = {30, 60, 120, 180, 240};
  return map_enum_number(options, 5, value, "mob_state_params_s::t_hyst_normal_e_");
}

std::string redist_serving_info_r13_s::t360_r13_opts::to_string() const
{
  static const char* options[] = {"min4", "min8", "min16", "min32", "infinity", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "redist_serving_info_r13_s::t360_r13_e_");
}
int8_t redist_serving_info_r13_s::t360_r13_opts::to_number() const
{
  static const int8_t options[] = {4, 8, 16, 32, -1};
  return map_enum_number(options, 5, value, "redist_serving_info_r13_s::t360_r13_e_");
}

std::string sc_mcch_sched_info_r14_s::on_dur_timer_scptm_r14_opts::to_string() const
{
  static const char* options[] = {"psf10", "psf20", "psf100", "psf300", "psf500", "psf1000", "psf1200", "psf1600"};
  return convert_enum_idx(options, 8, value, "sc_mcch_sched_info_r14_s::on_dur_timer_scptm_r14_e_");
}
uint16_t sc_mcch_sched_info_r14_s::on_dur_timer_scptm_r14_opts::to_number() const
{
  static const uint16_t options[] = {10, 20, 100, 300, 500, 1000, 1200, 1600};
  return map_enum_number(options, 8, value, "sc_mcch_sched_info_r14_s::on_dur_timer_scptm_r14_e_");
}

std::string sc_mcch_sched_info_r14_s::drx_inactivity_timer_scptm_r14_opts::to_string() const
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
  return convert_enum_idx(options, 16, value, "sc_mcch_sched_info_r14_s::drx_inactivity_timer_scptm_r14_e_");
}
uint16_t sc_mcch_sched_info_r14_s::drx_inactivity_timer_scptm_r14_opts::to_number() const
{
  static const uint16_t options[] = {0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384};
  return map_enum_number(options, 16, value, "sc_mcch_sched_info_r14_s::drx_inactivity_timer_scptm_r14_e_");
}

std::string sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::types_opts::to_string() const
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

std::string sl_disc_cfg_relay_ue_r13_s::hyst_max_r13_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB3", "dB6", "dB9", "dB12", "dBinf"};
  return convert_enum_idx(options, 6, value, "sl_disc_cfg_relay_ue_r13_s::hyst_max_r13_e_");
}
uint8_t sl_disc_cfg_relay_ue_r13_s::hyst_max_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 3, 6, 9, 12};
  return map_enum_number(options, 5, value, "sl_disc_cfg_relay_ue_r13_s::hyst_max_r13_e_");
}

std::string sl_disc_cfg_relay_ue_r13_s::hyst_min_r13_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB3", "dB6", "dB9", "dB12"};
  return convert_enum_idx(options, 5, value, "sl_disc_cfg_relay_ue_r13_s::hyst_min_r13_e_");
}
uint8_t sl_disc_cfg_relay_ue_r13_s::hyst_min_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 3, 6, 9, 12};
  return map_enum_number(options, 5, value, "sl_disc_cfg_relay_ue_r13_s::hyst_min_r13_e_");
}

std::string sl_disc_cfg_remote_ue_r13_s::hyst_max_r13_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB3", "dB6", "dB9", "dB12"};
  return convert_enum_idx(options, 5, value, "sl_disc_cfg_remote_ue_r13_s::hyst_max_r13_e_");
}
uint8_t sl_disc_cfg_remote_ue_r13_s::hyst_max_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 3, 6, 9, 12};
  return map_enum_number(options, 5, value, "sl_disc_cfg_remote_ue_r13_s::hyst_max_r13_e_");
}

// TimeAlignmentTimer ::= ENUMERATED
std::string time_align_timer_opts::to_string() const
{
  static const char* options[] = {"sf500", "sf750", "sf1280", "sf1920", "sf2560", "sf5120", "sf10240", "infinity"};
  return convert_enum_idx(options, 8, value, "time_align_timer_e");
}
int16_t time_align_timer_opts::to_number() const
{
  static const int16_t options[] = {500, 750, 1280, 1920, 2560, 5120, 10240, -1};
  return map_enum_number(options, 8, value, "time_align_timer_e");
}

// UAC-AC1-SelectAssistInfo-r15 ::= ENUMERATED
std::string uac_ac1_select_assist_info_r15_opts::to_string() const
{
  static const char* options[] = {"a", "b", "c"};
  return convert_enum_idx(options, 3, value, "uac_ac1_select_assist_info_r15_e");
}

std::string ue_timers_and_consts_s::t300_opts::to_string() const
{
  static const char* options[] = {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
  return convert_enum_idx(options, 8, value, "ue_timers_and_consts_s::t300_e_");
}
uint16_t ue_timers_and_consts_s::t300_opts::to_number() const
{
  static const uint16_t options[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(options, 8, value, "ue_timers_and_consts_s::t300_e_");
}

std::string ue_timers_and_consts_s::t301_opts::to_string() const
{
  static const char* options[] = {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
  return convert_enum_idx(options, 8, value, "ue_timers_and_consts_s::t301_e_");
}
uint16_t ue_timers_and_consts_s::t301_opts::to_number() const
{
  static const uint16_t options[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(options, 8, value, "ue_timers_and_consts_s::t301_e_");
}

std::string ue_timers_and_consts_s::t310_opts::to_string() const
{
  static const char* options[] = {"ms0", "ms50", "ms100", "ms200", "ms500", "ms1000", "ms2000"};
  return convert_enum_idx(options, 7, value, "ue_timers_and_consts_s::t310_e_");
}
uint16_t ue_timers_and_consts_s::t310_opts::to_number() const
{
  static const uint16_t options[] = {0, 50, 100, 200, 500, 1000, 2000};
  return map_enum_number(options, 7, value, "ue_timers_and_consts_s::t310_e_");
}

std::string ue_timers_and_consts_s::n310_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4", "n6", "n8", "n10", "n20"};
  return convert_enum_idx(options, 8, value, "ue_timers_and_consts_s::n310_e_");
}
uint8_t ue_timers_and_consts_s::n310_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 6, 8, 10, 20};
  return map_enum_number(options, 8, value, "ue_timers_and_consts_s::n310_e_");
}

std::string ue_timers_and_consts_s::t311_opts::to_string() const
{
  static const char* options[] = {"ms1000", "ms3000", "ms5000", "ms10000", "ms15000", "ms20000", "ms30000"};
  return convert_enum_idx(options, 7, value, "ue_timers_and_consts_s::t311_e_");
}
uint16_t ue_timers_and_consts_s::t311_opts::to_number() const
{
  static const uint16_t options[] = {1000, 3000, 5000, 10000, 15000, 20000, 30000};
  return map_enum_number(options, 7, value, "ue_timers_and_consts_s::t311_e_");
}

std::string ue_timers_and_consts_s::n311_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4", "n5", "n6", "n8", "n10"};
  return convert_enum_idx(options, 8, value, "ue_timers_and_consts_s::n311_e_");
}
uint8_t ue_timers_and_consts_s::n311_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5, 6, 8, 10};
  return map_enum_number(options, 8, value, "ue_timers_and_consts_s::n311_e_");
}

std::string ue_timers_and_consts_s::t300_v1310_opts::to_string() const
{
  static const char* options[] = {"ms2500", "ms3000", "ms3500", "ms4000", "ms5000", "ms6000", "ms8000", "ms10000"};
  return convert_enum_idx(options, 8, value, "ue_timers_and_consts_s::t300_v1310_e_");
}
uint16_t ue_timers_and_consts_s::t300_v1310_opts::to_number() const
{
  static const uint16_t options[] = {2500, 3000, 3500, 4000, 5000, 6000, 8000, 10000};
  return map_enum_number(options, 8, value, "ue_timers_and_consts_s::t300_v1310_e_");
}

std::string ue_timers_and_consts_s::t301_v1310_opts::to_string() const
{
  static const char* options[] = {"ms2500", "ms3000", "ms3500", "ms4000", "ms5000", "ms6000", "ms8000", "ms10000"};
  return convert_enum_idx(options, 8, value, "ue_timers_and_consts_s::t301_v1310_e_");
}
uint16_t ue_timers_and_consts_s::t301_v1310_opts::to_number() const
{
  static const uint16_t options[] = {2500, 3000, 3500, 4000, 5000, 6000, 8000, 10000};
  return map_enum_number(options, 8, value, "ue_timers_and_consts_s::t301_v1310_e_");
}

std::string ue_timers_and_consts_s::t310_v1330_opts::to_string() const
{
  static const char* options[] = {"ms4000", "ms6000"};
  return convert_enum_idx(options, 2, value, "ue_timers_and_consts_s::t310_v1330_e_");
}
uint16_t ue_timers_and_consts_s::t310_v1330_opts::to_number() const
{
  static const uint16_t options[] = {4000, 6000};
  return map_enum_number(options, 2, value, "ue_timers_and_consts_s::t310_v1330_e_");
}

std::string ue_timers_and_consts_s::t300_r15_opts::to_string() const
{
  static const char* options[] = {"ms4000", "ms6000", "ms8000", "ms10000", "ms15000", "ms25000", "ms40000", "ms60000"};
  return convert_enum_idx(options, 8, value, "ue_timers_and_consts_s::t300_r15_e_");
}
uint16_t ue_timers_and_consts_s::t300_r15_opts::to_number() const
{
  static const uint16_t options[] = {4000, 6000, 8000, 10000, 15000, 25000, 40000, 60000};
  return map_enum_number(options, 8, value, "ue_timers_and_consts_s::t300_r15_e_");
}

std::string sched_info_s::si_periodicity_opts::to_string() const
{
  static const char* options[] = {"rf8", "rf16", "rf32", "rf64", "rf128", "rf256", "rf512"};
  return convert_enum_idx(options, 7, value, "sched_info_s::si_periodicity_e_");
}
uint16_t sched_info_s::si_periodicity_opts::to_number() const
{
  static const uint16_t options[] = {8, 16, 32, 64, 128, 256, 512};
  return map_enum_number(options, 7, value, "sched_info_s::si_periodicity_e_");
}

std::string sib_type11_s::warning_msg_segment_type_opts::to_string() const
{
  static const char* options[] = {"notLastSegment", "lastSegment"};
  return convert_enum_idx(options, 2, value, "sib_type11_s::warning_msg_segment_type_e_");
}

std::string sib_type12_r9_s::warning_msg_segment_type_r9_opts::to_string() const
{
  static const char* options[] = {"notLastSegment", "lastSegment"};
  return convert_enum_idx(options, 2, value, "sib_type12_r9_s::warning_msg_segment_type_r9_e_");
}

std::string sib_type14_r11_s::eab_param_r11_c_::types_opts::to_string() const
{
  static const char* options[] = {"eab-Common-r11", "eab-PerPLMN-List-r11"};
  return convert_enum_idx(options, 2, value, "sib_type14_r11_s::eab_param_r11_c_::types");
}

std::string sib_type14_r11_s::eab_per_rsrp_r15_opts::to_string() const
{
  static const char* options[] = {"thresh0", "thresh1", "thresh2", "thresh3"};
  return convert_enum_idx(options, 4, value, "sib_type14_r11_s::eab_per_rsrp_r15_e_");
}
uint8_t sib_type14_r11_s::eab_per_rsrp_r15_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3};
  return map_enum_number(options, 4, value, "sib_type14_r11_s::eab_per_rsrp_r15_e_");
}

std::string sib_type2_s::freq_info_s_::ul_bw_opts::to_string() const
{
  static const char* options[] = {"n6", "n15", "n25", "n50", "n75", "n100"};
  return convert_enum_idx(options, 6, value, "sib_type2_s::freq_info_s_::ul_bw_e_");
}
uint8_t sib_type2_s::freq_info_s_::ul_bw_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(options, 6, value, "sib_type2_s::freq_info_s_::ul_bw_e_");
}

std::string sib_type20_r13_s::sc_mcch_repeat_period_r13_opts::to_string() const
{
  static const char* options[] = {"rf2", "rf4", "rf8", "rf16", "rf32", "rf64", "rf128", "rf256"};
  return convert_enum_idx(options, 8, value, "sib_type20_r13_s::sc_mcch_repeat_period_r13_e_");
}
uint16_t sib_type20_r13_s::sc_mcch_repeat_period_r13_opts::to_number() const
{
  static const uint16_t options[] = {2, 4, 8, 16, 32, 64, 128, 256};
  return map_enum_number(options, 8, value, "sib_type20_r13_s::sc_mcch_repeat_period_r13_e_");
}

std::string sib_type20_r13_s::sc_mcch_mod_period_r13_opts::to_string() const
{
  static const char* options[] = {"rf2",
                                  "rf4",
                                  "rf8",
                                  "rf16",
                                  "rf32",
                                  "rf64",
                                  "rf128",
                                  "rf256",
                                  "rf512",
                                  "rf1024",
                                  "r2048",
                                  "rf4096",
                                  "rf8192",
                                  "rf16384",
                                  "rf32768",
                                  "rf65536"};
  return convert_enum_idx(options, 16, value, "sib_type20_r13_s::sc_mcch_mod_period_r13_e_");
}
uint32_t sib_type20_r13_s::sc_mcch_mod_period_r13_opts::to_number() const
{
  static const uint32_t options[] = {2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536};
  return map_enum_number(options, 16, value, "sib_type20_r13_s::sc_mcch_mod_period_r13_e_");
}

std::string sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_num_repeat_sc_mcch_r14_opts::to_string() const
{
  static const char* options[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "r128", "r256"};
  return convert_enum_idx(options, 9, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_num_repeat_sc_mcch_r14_e_");
}
uint16_t sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_num_repeat_sc_mcch_r14_opts::to_number() const
{
  static const uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
  return map_enum_number(options, 9, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_num_repeat_sc_mcch_r14_e_");
}

std::string sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::fdd_r14_opts::to_string() const
{
  static const char* options[] = {"v1", "v1dot5", "v2", "v2dot5", "v4", "v5", "v8", "v10"};
  return convert_enum_idx(
      options, 8, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::fdd_r14_e_");
}
float sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::fdd_r14_opts::to_number() const
{
  static const float options[] = {1.0, 1.5, 2.0, 2.5, 4.0, 5.0, 8.0, 10.0};
  return map_enum_number(
      options, 8, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::fdd_r14_e_");
}
std::string sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::fdd_r14_opts::to_number_string() const
{
  static const char* options[] = {"1", "1.5", "2", "2.5", "4", "5", "8", "10"};
  return convert_enum_idx(
      options, 8, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::fdd_r14_e_");
}

std::string sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::tdd_r14_opts::to_string() const
{
  static const char* options[] = {"v1", "v2", "v4", "v5", "v8", "v10", "v20"};
  return convert_enum_idx(
      options, 7, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::tdd_r14_e_");
}
uint8_t sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::tdd_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 5, 8, 10, 20};
  return map_enum_number(
      options, 7, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::tdd_r14_e_");
}

std::string sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::types_opts::to_string() const
{
  static const char* options[] = {"fdd-r14", "tdd-r14"};
  return convert_enum_idx(
      options, 2, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::types");
}

std::string sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_pdsch_hop_cfg_sc_mcch_r14_opts::to_string() const
{
  static const char* options[] = {"off", "ce-ModeA", "ce-ModeB"};
  return convert_enum_idx(
      options, 3, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_pdsch_hop_cfg_sc_mcch_r14_e_");
}

std::string sib_type20_r13_s::br_bcch_cfg_r14_s_::sc_mcch_repeat_period_br_r14_opts::to_string() const
{
  static const char* options[] = {"rf32", "rf128", "rf512", "rf1024", "rf2048", "rf4096", "rf8192", "rf16384"};
  return convert_enum_idx(options, 8, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::sc_mcch_repeat_period_br_r14_e_");
}
uint16_t sib_type20_r13_s::br_bcch_cfg_r14_s_::sc_mcch_repeat_period_br_r14_opts::to_number() const
{
  static const uint16_t options[] = {32, 128, 512, 1024, 2048, 4096, 8192, 16384};
  return map_enum_number(options, 8, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::sc_mcch_repeat_period_br_r14_e_");
}

std::string sib_type20_r13_s::br_bcch_cfg_r14_s_::sc_mcch_mod_period_br_r14_opts::to_string() const
{
  static const char* options[] = {"rf32",
                                  "rf128",
                                  "rf256",
                                  "rf512",
                                  "rf1024",
                                  "rf2048",
                                  "rf4096",
                                  "rf8192",
                                  "rf16384",
                                  "rf32768",
                                  "rf65536",
                                  "rf131072",
                                  "rf262144",
                                  "rf524288",
                                  "rf1048576"};
  return convert_enum_idx(options, 15, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::sc_mcch_mod_period_br_r14_e_");
}
uint32_t sib_type20_r13_s::br_bcch_cfg_r14_s_::sc_mcch_mod_period_br_r14_opts::to_number() const
{
  static const uint32_t options[] = {
      32, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576};
  return map_enum_number(options, 15, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::sc_mcch_mod_period_br_r14_e_");
}

std::string sib_type20_r13_s::pdsch_max_num_repeat_cemode_a_sc_mtch_r14_opts::to_string() const
{
  static const char* options[] = {"r16", "r32"};
  return convert_enum_idx(options, 2, value, "sib_type20_r13_s::pdsch_max_num_repeat_cemode_a_sc_mtch_r14_e_");
}
uint8_t sib_type20_r13_s::pdsch_max_num_repeat_cemode_a_sc_mtch_r14_opts::to_number() const
{
  static const uint8_t options[] = {16, 32};
  return map_enum_number(options, 2, value, "sib_type20_r13_s::pdsch_max_num_repeat_cemode_a_sc_mtch_r14_e_");
}

std::string sib_type20_r13_s::pdsch_max_num_repeat_cemode_b_sc_mtch_r14_opts::to_string() const
{
  static const char* options[] = {"r192", "r256", "r384", "r512", "r768", "r1024", "r1536", "r2048"};
  return convert_enum_idx(options, 8, value, "sib_type20_r13_s::pdsch_max_num_repeat_cemode_b_sc_mtch_r14_e_");
}
uint16_t sib_type20_r13_s::pdsch_max_num_repeat_cemode_b_sc_mtch_r14_opts::to_number() const
{
  static const uint16_t options[] = {192, 256, 384, 512, 768, 1024, 1536, 2048};
  return map_enum_number(options, 8, value, "sib_type20_r13_s::pdsch_max_num_repeat_cemode_b_sc_mtch_r14_e_");
}

std::string sib_type25_r15_s::uac_ac1_select_assist_info_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"plmnCommon-r15", "individualPLMNList-r15"};
  return convert_enum_idx(options, 2, value, "sib_type25_r15_s::uac_ac1_select_assist_info_r15_c_::types");
}

std::string sib_type3_s::cell_resel_info_common_s_::q_hyst_opts::to_string() const
{
  static const char* options[] = {"dB0",
                                  "dB1",
                                  "dB2",
                                  "dB3",
                                  "dB4",
                                  "dB5",
                                  "dB6",
                                  "dB8",
                                  "dB10",
                                  "dB12",
                                  "dB14",
                                  "dB16",
                                  "dB18",
                                  "dB20",
                                  "dB22",
                                  "dB24"};
  return convert_enum_idx(options, 16, value, "sib_type3_s::cell_resel_info_common_s_::q_hyst_e_");
}
uint8_t sib_type3_s::cell_resel_info_common_s_::q_hyst_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24};
  return map_enum_number(options, 16, value, "sib_type3_s::cell_resel_info_common_s_::q_hyst_e_");
}

std::string
sib_type3_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_opts::to_string() const
{
  static const char* options[] = {"dB-6", "dB-4", "dB-2", "dB0"};
  return convert_enum_idx(
      options,
      4,
      value,
      "sib_type3_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_e_");
}
int8_t
sib_type3_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_opts::to_number() const
{
  static const int8_t options[] = {-6, -4, -2, 0};
  return map_enum_number(
      options,
      4,
      value,
      "sib_type3_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_e_");
}

std::string
sib_type3_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_opts::to_string() const
{
  static const char* options[] = {"dB-6", "dB-4", "dB-2", "dB0"};
  return convert_enum_idx(
      options, 4, value, "sib_type3_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_e_");
}
int8_t sib_type3_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_opts::to_number() const
{
  static const int8_t options[] = {-6, -4, -2, 0};
  return map_enum_number(
      options, 4, value, "sib_type3_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_e_");
}

std::string pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "posSib1-1-r15",  "posSib1-2-r15",  "posSib1-3-r15",  "posSib1-4-r15",  "posSib1-5-r15",  "posSib1-6-r15",
      "posSib1-7-r15",  "posSib2-1-r15",  "posSib2-2-r15",  "posSib2-3-r15",  "posSib2-4-r15",  "posSib2-5-r15",
      "posSib2-6-r15",  "posSib2-7-r15",  "posSib2-8-r15",  "posSib2-9-r15",  "posSib2-10-r15", "posSib2-11-r15",
      "posSib2-12-r15", "posSib2-13-r15", "posSib2-14-r15", "posSib2-15-r15", "posSib2-16-r15", "posSib2-17-r15",
      "posSib2-18-r15", "posSib2-19-r15", "posSib3-1-r15"};
  return convert_enum_idx(options, 27, value, "pos_sys_info_r15_ies_s::pos_sib_type_and_info_r15_item_c_::types");
}

std::string sys_info_r8_ies_s::sib_type_and_info_item_c_::types_opts::to_string() const
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

std::string sys_info_s::crit_exts_c_::crit_exts_future_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"posSystemInformation-r15", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "sys_info_s::crit_exts_c_::crit_exts_future_r15_c_::types");
}

std::string sys_info_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"systemInformation-r8", "criticalExtensionsFuture-r15"};
  return convert_enum_idx(options, 2, value, "sys_info_s::crit_exts_c_::types");
}

std::string sib_type1_s::cell_access_related_info_s_::cell_barred_opts::to_string() const
{
  static const char* options[] = {"barred", "notBarred"};
  return convert_enum_idx(options, 2, value, "sib_type1_s::cell_access_related_info_s_::cell_barred_e_");
}

std::string sib_type1_s::cell_access_related_info_s_::intra_freq_resel_opts::to_string() const
{
  static const char* options[] = {"allowed", "notAllowed"};
  return convert_enum_idx(options, 2, value, "sib_type1_s::cell_access_related_info_s_::intra_freq_resel_e_");
}

std::string sib_type1_s::si_win_len_opts::to_string() const
{
  static const char* options[] = {"ms1", "ms2", "ms5", "ms10", "ms15", "ms20", "ms40"};
  return convert_enum_idx(options, 7, value, "sib_type1_s::si_win_len_e_");
}
uint8_t sib_type1_s::si_win_len_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 5, 10, 15, 20, 40};
  return map_enum_number(options, 7, value, "sib_type1_s::si_win_len_e_");
}

std::string bcch_dl_sch_msg_type_c::c1_c_::types_opts::to_string() const
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

std::string bcch_dl_sch_msg_type_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "bcch_dl_sch_msg_type_c::types");
}
uint8_t bcch_dl_sch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "bcch_dl_sch_msg_type_c::types");
}

std::string bcch_dl_sch_msg_type_br_r13_c::c1_c_::types_opts::to_string() const
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

std::string bcch_dl_sch_msg_type_br_r13_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "bcch_dl_sch_msg_type_br_r13_c::types");
}
uint8_t bcch_dl_sch_msg_type_br_r13_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "bcch_dl_sch_msg_type_br_r13_c::types");
}

// SIB-Type-MBMS-r14 ::= ENUMERATED
std::string sib_type_mbms_r14_opts::to_string() const
{
  static const char* options[] = {
      "sibType10", "sibType11", "sibType12-v920", "sibType13-v920", "sibType15-v1130", "sibType16-v1130"};
  return convert_enum_idx(options, 6, value, "sib_type_mbms_r14_e");
}
uint8_t sib_type_mbms_r14_opts::to_number() const
{
  static const uint8_t options[] = {10, 11, 12, 13, 15, 16};
  return map_enum_number(options, 6, value, "sib_type_mbms_r14_e");
}

std::string sched_info_mbms_r14_s::si_periodicity_r14_opts::to_string() const
{
  static const char* options[] = {"rf16", "rf32", "rf64", "rf128", "rf256", "rf512"};
  return convert_enum_idx(options, 6, value, "sched_info_mbms_r14_s::si_periodicity_r14_e_");
}
uint16_t sched_info_mbms_r14_s::si_periodicity_r14_opts::to_number() const
{
  static const uint16_t options[] = {16, 32, 64, 128, 256, 512};
  return map_enum_number(options, 6, value, "sched_info_mbms_r14_s::si_periodicity_r14_e_");
}

std::string non_mbsfn_sf_cfg_r14_s::radio_frame_alloc_period_r14_opts::to_string() const
{
  static const char* options[] = {"rf4", "rf8", "rf16", "rf32", "rf64", "rf128", "rf512"};
  return convert_enum_idx(options, 7, value, "non_mbsfn_sf_cfg_r14_s::radio_frame_alloc_period_r14_e_");
}
uint16_t non_mbsfn_sf_cfg_r14_s::radio_frame_alloc_period_r14_opts::to_number() const
{
  static const uint16_t options[] = {4, 8, 16, 32, 64, 128, 512};
  return map_enum_number(options, 7, value, "non_mbsfn_sf_cfg_r14_s::radio_frame_alloc_period_r14_e_");
}

std::string sib_type1_mbms_r14_s::si_win_len_r14_opts::to_string() const
{
  static const char* options[] = {"ms1", "ms2", "ms5", "ms10", "ms15", "ms20", "ms40", "ms80"};
  return convert_enum_idx(options, 8, value, "sib_type1_mbms_r14_s::si_win_len_r14_e_");
}
uint8_t sib_type1_mbms_r14_s::si_win_len_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 5, 10, 15, 20, 40, 80};
  return map_enum_number(options, 8, value, "sib_type1_mbms_r14_s::si_win_len_r14_e_");
}

std::string bcch_dl_sch_msg_type_mbms_r14_c::c1_c_::types_opts::to_string() const
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

std::string bcch_dl_sch_msg_type_mbms_r14_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "bcch_dl_sch_msg_type_mbms_r14_c::types");
}
uint8_t bcch_dl_sch_msg_type_mbms_r14_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "bcch_dl_sch_msg_type_mbms_r14_c::types");
}

// NZP-FrequencyDensity-r14 ::= ENUMERATED
std::string nzp_freq_density_r14_opts::to_string() const
{
  static const char* options[] = {"d1", "d2", "d3"};
  return convert_enum_idx(options, 3, value, "nzp_freq_density_r14_e");
}
uint8_t nzp_freq_density_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3};
  return map_enum_number(options, 3, value, "nzp_freq_density_r14_e");
}

std::string p_c_and_cbsr_r13_s::cbsr_sel_r13_c_::types_opts::to_string() const
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

std::string csi_rs_cfg_nzp_activation_r14_s::csi_rs_nzp_mode_r14_opts::to_string() const
{
  static const char* options[] = {"semiPersistent", "aperiodic"};
  return convert_enum_idx(options, 2, value, "csi_rs_cfg_nzp_activation_r14_s::csi_rs_nzp_mode_r14_e_");
}

std::string csi_rs_cfg_nzp_r11_s::ant_ports_count_r11_opts::to_string() const
{
  static const char* options[] = {"an1", "an2", "an4", "an8"};
  return convert_enum_idx(options, 4, value, "csi_rs_cfg_nzp_r11_s::ant_ports_count_r11_e_");
}
uint8_t csi_rs_cfg_nzp_r11_s::ant_ports_count_r11_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8};
  return map_enum_number(options, 4, value, "csi_rs_cfg_nzp_r11_s::ant_ports_count_r11_e_");
}

std::string csi_rs_cfg_nzp_r11_s::qcl_crs_info_r11_s_::crs_ports_count_r11_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n4", "spare1"};
  return convert_enum_idx(options, 4, value, "csi_rs_cfg_nzp_r11_s::qcl_crs_info_r11_s_::crs_ports_count_r11_e_");
}
uint8_t csi_rs_cfg_nzp_r11_s::qcl_crs_info_r11_s_::crs_ports_count_r11_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4};
  return map_enum_number(options, 3, value, "csi_rs_cfg_nzp_r11_s::qcl_crs_info_r11_s_::crs_ports_count_r11_e_");
}

std::string csi_rs_cfg_nzp_emimo_r13_c::setup_s_::cdm_type_r13_opts::to_string() const
{
  static const char* options[] = {"cdm2", "cdm4"};
  return convert_enum_idx(options, 2, value, "csi_rs_cfg_nzp_emimo_r13_c::setup_s_::cdm_type_r13_e_");
}
uint8_t csi_rs_cfg_nzp_emimo_r13_c::setup_s_::cdm_type_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "csi_rs_cfg_nzp_emimo_r13_c::setup_s_::cdm_type_r13_e_");
}

// CQI-ReportModeAperiodic ::= ENUMERATED
std::string cqi_report_mode_aperiodic_opts::to_string() const
{
  static const char* options[] = {"rm12", "rm20", "rm22", "rm30", "rm31", "rm32-v1250", "rm10-v1310", "rm11-v1310"};
  return convert_enum_idx(options, 8, value, "cqi_report_mode_aperiodic_e");
}
uint8_t cqi_report_mode_aperiodic_opts::to_number() const
{
  static const uint8_t options[] = {12, 20, 22, 30, 31, 32, 10, 11};
  return map_enum_number(options, 8, value, "cqi_report_mode_aperiodic_e");
}

std::string csi_rs_cfg_non_precoded_r13_s::codebook_cfg_n1_r13_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4", "n8"};
  return convert_enum_idx(options, 5, value, "csi_rs_cfg_non_precoded_r13_s::codebook_cfg_n1_r13_e_");
}
uint8_t csi_rs_cfg_non_precoded_r13_s::codebook_cfg_n1_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 8};
  return map_enum_number(options, 5, value, "csi_rs_cfg_non_precoded_r13_s::codebook_cfg_n1_r13_e_");
}

std::string csi_rs_cfg_non_precoded_r13_s::codebook_cfg_n2_r13_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4", "n8"};
  return convert_enum_idx(options, 5, value, "csi_rs_cfg_non_precoded_r13_s::codebook_cfg_n2_r13_e_");
}
uint8_t csi_rs_cfg_non_precoded_r13_s::codebook_cfg_n2_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 8};
  return map_enum_number(options, 5, value, "csi_rs_cfg_non_precoded_r13_s::codebook_cfg_n2_r13_e_");
}

std::string csi_rs_cfg_non_precoded_r13_s::codebook_over_sampling_rate_cfg_o1_r13_opts::to_string() const
{
  static const char* options[] = {"n4", "n8"};
  return convert_enum_idx(
      options, 2, value, "csi_rs_cfg_non_precoded_r13_s::codebook_over_sampling_rate_cfg_o1_r13_e_");
}
uint8_t csi_rs_cfg_non_precoded_r13_s::codebook_over_sampling_rate_cfg_o1_r13_opts::to_number() const
{
  static const uint8_t options[] = {4, 8};
  return map_enum_number(options, 2, value, "csi_rs_cfg_non_precoded_r13_s::codebook_over_sampling_rate_cfg_o1_r13_e_");
}

std::string csi_rs_cfg_non_precoded_r13_s::codebook_over_sampling_rate_cfg_o2_r13_opts::to_string() const
{
  static const char* options[] = {"n4", "n8"};
  return convert_enum_idx(
      options, 2, value, "csi_rs_cfg_non_precoded_r13_s::codebook_over_sampling_rate_cfg_o2_r13_e_");
}
uint8_t csi_rs_cfg_non_precoded_r13_s::codebook_over_sampling_rate_cfg_o2_r13_opts::to_number() const
{
  static const uint8_t options[] = {4, 8};
  return map_enum_number(options, 2, value, "csi_rs_cfg_non_precoded_r13_s::codebook_over_sampling_rate_cfg_o2_r13_e_");
}

std::string csi_rs_cfg_non_precoded_v1430_s::codebook_cfg_n1_v1430_opts::to_string() const
{
  static const char* options[] = {"n5", "n6", "n7", "n10", "n12", "n14", "n16"};
  return convert_enum_idx(options, 7, value, "csi_rs_cfg_non_precoded_v1430_s::codebook_cfg_n1_v1430_e_");
}
uint8_t csi_rs_cfg_non_precoded_v1430_s::codebook_cfg_n1_v1430_opts::to_number() const
{
  static const uint8_t options[] = {5, 6, 7, 10, 12, 14, 16};
  return map_enum_number(options, 7, value, "csi_rs_cfg_non_precoded_v1430_s::codebook_cfg_n1_v1430_e_");
}

std::string csi_rs_cfg_non_precoded_v1430_s::codebook_cfg_n2_v1430_opts::to_string() const
{
  static const char* options[] = {"n5", "n6", "n7"};
  return convert_enum_idx(options, 3, value, "csi_rs_cfg_non_precoded_v1430_s::codebook_cfg_n2_v1430_e_");
}
uint8_t csi_rs_cfg_non_precoded_v1430_s::codebook_cfg_n2_v1430_opts::to_number() const
{
  static const uint8_t options[] = {5, 6, 7};
  return map_enum_number(options, 3, value, "csi_rs_cfg_non_precoded_v1430_s::codebook_cfg_n2_v1430_e_");
}

std::string csi_rs_cfg_non_precoded_v1480_s::codebook_cfg_n1_v1480_opts::to_string() const
{
  static const char* options[] = {"n5", "n6", "n7", "n10", "n12", "n14", "n16"};
  return convert_enum_idx(options, 7, value, "csi_rs_cfg_non_precoded_v1480_s::codebook_cfg_n1_v1480_e_");
}
uint8_t csi_rs_cfg_non_precoded_v1480_s::codebook_cfg_n1_v1480_opts::to_number() const
{
  static const uint8_t options[] = {5, 6, 7, 10, 12, 14, 16};
  return map_enum_number(options, 7, value, "csi_rs_cfg_non_precoded_v1480_s::codebook_cfg_n1_v1480_e_");
}

std::string csi_rs_cfg_non_precoded_v1480_s::codebook_cfg_n2_r1480_opts::to_string() const
{
  static const char* options[] = {"n5", "n6", "n7"};
  return convert_enum_idx(options, 3, value, "csi_rs_cfg_non_precoded_v1480_s::codebook_cfg_n2_r1480_e_");
}
uint8_t csi_rs_cfg_non_precoded_v1480_s::codebook_cfg_n2_r1480_opts::to_number() const
{
  static const uint8_t options[] = {5, 6, 7};
  return map_enum_number(options, 3, value, "csi_rs_cfg_non_precoded_v1480_s::codebook_cfg_n2_r1480_e_");
}

std::string csi_rs_cfg_emimo_r13_c::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"nonPrecoded-r13", "beamformed-r13"};
  return convert_enum_idx(options, 2, value, "csi_rs_cfg_emimo_r13_c::setup_c_::types");
}

std::string csi_rs_cfg_emimo_v1430_c::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"nonPrecoded-v1430", "beamformed-v1430"};
  return convert_enum_idx(options, 2, value, "csi_rs_cfg_emimo_v1430_c::setup_c_::types");
}

std::string csi_rs_cfg_emimo_v1480_c::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"nonPrecoded-v1480", "beamformed-v1480"};
  return convert_enum_idx(options, 2, value, "csi_rs_cfg_emimo_v1480_c::setup_c_::types");
}

std::string csi_rs_cfg_emimo_v1530_c::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"nonPrecoded-v1530"};
  return convert_enum_idx(options, 1, value, "csi_rs_cfg_emimo_v1530_c::setup_c_::types");
}

std::string cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::wideband_cqi_r11_s_::
    csi_report_mode_r11_opts::to_string() const
{
  static const char* options[] = {"submode1", "submode2"};
  return convert_enum_idx(options,
                          2,
                          value,
                          "cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::wideband_cqi_r11_s_::"
                          "csi_report_mode_r11_e_");
}
uint8_t cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::wideband_cqi_r11_s_::
    csi_report_mode_r11_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options,
                         2,
                         value,
                         "cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::wideband_cqi_r11_s_::csi_"
                         "report_mode_r11_e_");
}

std::string cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::subband_cqi_r11_s_::
    periodicity_factor_r11_opts::to_string() const
{
  static const char* options[] = {"n2", "n4"};
  return convert_enum_idx(options,
                          2,
                          value,
                          "cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::subband_cqi_r11_s_::"
                          "periodicity_factor_r11_e_");
}
uint8_t cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::subband_cqi_r11_s_::
    periodicity_factor_r11_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options,
                         2,
                         value,
                         "cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::subband_cqi_r11_s_::"
                         "periodicity_factor_r11_e_");
}

std::string cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::types_opts::to_string() const
{
  static const char* options[] = {"widebandCQI-r11", "subbandCQI-r11"};
  return convert_enum_idx(
      options, 2, value, "cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::types");
}

std::string cqi_report_periodic_proc_ext_r11_s::periodicity_factor_wb_r13_opts::to_string() const
{
  static const char* options[] = {"n2", "n4"};
  return convert_enum_idx(options, 2, value, "cqi_report_periodic_proc_ext_r11_s::periodicity_factor_wb_r13_e_");
}
uint8_t cqi_report_periodic_proc_ext_r11_s::periodicity_factor_wb_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "cqi_report_periodic_proc_ext_r11_s::periodicity_factor_wb_r13_e_");
}

// PollByte-r14 ::= ENUMERATED
std::string poll_byte_r14_opts::to_string() const
{
  static const char* options[] = {"kB1",     "kB2",     "kB5",     "kB8",     "kB10",    "kB15",    "kB3500",
                                  "kB4000",  "kB4500",  "kB5000",  "kB5500",  "kB6000",  "kB6500",  "kB7000",
                                  "kB7500",  "kB8000",  "kB9000",  "kB10000", "kB11000", "kB12000", "kB13000",
                                  "kB14000", "kB15000", "kB16000", "kB17000", "kB18000", "kB19000", "kB20000",
                                  "kB25000", "kB30000", "kB35000", "kB40000"};
  return convert_enum_idx(options, 32, value, "poll_byte_r14_e");
}
uint16_t poll_byte_r14_opts::to_number() const
{
  static const uint16_t options[] = {1,     2,     5,     8,     10,    15,    3500,  4000,  4500,  5000,  5500,
                                     6000,  6500,  7000,  7500,  8000,  9000,  10000, 11000, 12000, 13000, 14000,
                                     15000, 16000, 17000, 18000, 19000, 20000, 25000, 30000, 35000, 40000};
  return map_enum_number(options, 32, value, "poll_byte_r14_e");
}

// PollPDU-r15 ::= ENUMERATED
std::string poll_pdu_r15_opts::to_string() const
{
  static const char* options[] = {"p4",
                                  "p8",
                                  "p16",
                                  "p32",
                                  "p64",
                                  "p128",
                                  "p256",
                                  "p512",
                                  "p1024",
                                  "p2048-r15",
                                  "p4096-r15",
                                  "p6144-r15",
                                  "p8192-r15",
                                  "p12288-r15",
                                  "p16384-r15",
                                  "pInfinity"};
  return convert_enum_idx(options, 16, value, "poll_pdu_r15_e");
}
int16_t poll_pdu_r15_opts::to_number() const
{
  static const int16_t options[] = {4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 6144, 8192, 12288, 16384, -1};
  return map_enum_number(options, 16, value, "poll_pdu_r15_e");
}

// SN-FieldLength ::= ENUMERATED
std::string sn_field_len_opts::to_string() const
{
  static const char* options[] = {"size5", "size10"};
  return convert_enum_idx(options, 2, value, "sn_field_len_e");
}
uint8_t sn_field_len_opts::to_number() const
{
  static const uint8_t options[] = {5, 10};
  return map_enum_number(options, 2, value, "sn_field_len_e");
}

// SN-FieldLength-r15 ::= ENUMERATED
std::string sn_field_len_r15_opts::to_string() const
{
  static const char* options[] = {"size5", "size10", "size16-r15"};
  return convert_enum_idx(options, 3, value, "sn_field_len_r15_e");
}
uint8_t sn_field_len_r15_opts::to_number() const
{
  static const uint8_t options[] = {5, 10, 16};
  return map_enum_number(options, 3, value, "sn_field_len_r15_e");
}

// T-PollRetransmit ::= ENUMERATED
std::string t_poll_retx_opts::to_string() const
{
  static const char* options[] = {
      "ms5",          "ms10",         "ms15",         "ms20",   "ms25",   "ms30",   "ms35",   "ms40",
      "ms45",         "ms50",         "ms55",         "ms60",   "ms65",   "ms70",   "ms75",   "ms80",
      "ms85",         "ms90",         "ms95",         "ms100",  "ms105",  "ms110",  "ms115",  "ms120",
      "ms125",        "ms130",        "ms135",        "ms140",  "ms145",  "ms150",  "ms155",  "ms160",
      "ms165",        "ms170",        "ms175",        "ms180",  "ms185",  "ms190",  "ms195",  "ms200",
      "ms205",        "ms210",        "ms215",        "ms220",  "ms225",  "ms230",  "ms235",  "ms240",
      "ms245",        "ms250",        "ms300",        "ms350",  "ms400",  "ms450",  "ms500",  "ms800-v1310",
      "ms1000-v1310", "ms2000-v1310", "ms4000-v1310", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 64, value, "t_poll_retx_e");
}
uint16_t t_poll_retx_opts::to_number() const
{
  static const uint16_t options[] = {5,   10,  15,  20,  25,  30,  35,  40,  45,  50,  55,  60,   65,   70,  75,
                                     80,  85,  90,  95,  100, 105, 110, 115, 120, 125, 130, 135,  140,  145, 150,
                                     155, 160, 165, 170, 175, 180, 185, 190, 195, 200, 205, 210,  215,  220, 225,
                                     230, 235, 240, 245, 250, 300, 350, 400, 450, 500, 800, 1000, 2000, 4000};
  return map_enum_number(options, 59, value, "t_poll_retx_e");
}

// T-Reordering ::= ENUMERATED
std::string t_reordering_opts::to_string() const
{
  static const char* options[] = {"ms0",   "ms5",   "ms10",  "ms15",  "ms20",  "ms25",  "ms30",  "ms35",
                                  "ms40",  "ms45",  "ms50",  "ms55",  "ms60",  "ms65",  "ms70",  "ms75",
                                  "ms80",  "ms85",  "ms90",  "ms95",  "ms100", "ms110", "ms120", "ms130",
                                  "ms140", "ms150", "ms160", "ms170", "ms180", "ms190", "ms200", "ms1600-v1310"};
  return convert_enum_idx(options, 32, value, "t_reordering_e");
}
uint16_t t_reordering_opts::to_number() const
{
  static const uint16_t options[] = {0,  5,  10, 15, 20,  25,  30,  35,  40,  45,  50,  55,  60,  65,  70,  75,
                                     80, 85, 90, 95, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200, 1600};
  return map_enum_number(options, 32, value, "t_reordering_e");
}

// T-StatusProhibit ::= ENUMERATED
std::string t_status_prohibit_opts::to_string() const
{
  static const char* options[] = {
      "ms0",         "ms5",          "ms10",         "ms15",         "ms20",         "ms25",         "ms30",
      "ms35",        "ms40",         "ms45",         "ms50",         "ms55",         "ms60",         "ms65",
      "ms70",        "ms75",         "ms80",         "ms85",         "ms90",         "ms95",         "ms100",
      "ms105",       "ms110",        "ms115",        "ms120",        "ms125",        "ms130",        "ms135",
      "ms140",       "ms145",        "ms150",        "ms155",        "ms160",        "ms165",        "ms170",
      "ms175",       "ms180",        "ms185",        "ms190",        "ms195",        "ms200",        "ms205",
      "ms210",       "ms215",        "ms220",        "ms225",        "ms230",        "ms235",        "ms240",
      "ms245",       "ms250",        "ms300",        "ms350",        "ms400",        "ms450",        "ms500",
      "ms800-v1310", "ms1000-v1310", "ms1200-v1310", "ms1600-v1310", "ms2000-v1310", "ms2400-v1310", "spare2",
      "spare1"};
  return convert_enum_idx(options, 64, value, "t_status_prohibit_e");
}
uint16_t t_status_prohibit_opts::to_number() const
{
  static const uint16_t options[] = {0,   5,   10,  15,  20,  25,  30,  35,  40,  45,   50,   55,   60,   65,  70,  75,
                                     80,  85,  90,  95,  100, 105, 110, 115, 120, 125,  130,  135,  140,  145, 150, 155,
                                     160, 165, 170, 175, 180, 185, 190, 195, 200, 205,  210,  215,  220,  225, 230, 235,
                                     240, 245, 250, 300, 350, 400, 450, 500, 800, 1000, 1200, 1600, 2000, 2400};
  return map_enum_number(options, 62, value, "t_status_prohibit_e");
}

std::string
cqi_report_aperiodic_v1250_c::setup_s_::aperiodic_csi_trigger_v1250_s_::trigger_sf_set_ind_r12_opts::to_string() const
{
  static const char* options[] = {"s1", "s2"};
  return convert_enum_idx(
      options,
      2,
      value,
      "cqi_report_aperiodic_v1250_c::setup_s_::aperiodic_csi_trigger_v1250_s_::trigger_sf_set_ind_r12_e_");
}
uint8_t
cqi_report_aperiodic_v1250_c::setup_s_::aperiodic_csi_trigger_v1250_s_::trigger_sf_set_ind_r12_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(
      options,
      2,
      value,
      "cqi_report_aperiodic_v1250_c::setup_s_::aperiodic_csi_trigger_v1250_s_::trigger_sf_set_ind_r12_e_");
}

std::string cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_::types_opts::to_string() const
{
  static const char* options[] = {"oneBit-r14", "twoBit-r14", "threeBit-r14"};
  return convert_enum_idx(options, 3, value, "cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_::types");
}
uint8_t cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3};
  return map_enum_number(options, 3, value, "cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_::types");
}

std::string cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::wideband_cqi_r10_s_::
    csi_report_mode_r10_opts::to_string() const
{
  static const char* options[] = {"submode1", "submode2"};
  return convert_enum_idx(options,
                          2,
                          value,
                          "cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::wideband_cqi_r10_s_::"
                          "csi_report_mode_r10_e_");
}
uint8_t cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::wideband_cqi_r10_s_::
    csi_report_mode_r10_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options,
                         2,
                         value,
                         "cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::wideband_cqi_r10_s_::"
                         "csi_report_mode_r10_e_");
}

std::string cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::subband_cqi_r10_s_::
    periodicity_factor_r10_opts::to_string() const
{
  static const char* options[] = {"n2", "n4"};
  return convert_enum_idx(options,
                          2,
                          value,
                          "cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::subband_cqi_r10_s_::"
                          "periodicity_factor_r10_e_");
}
uint8_t cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::subband_cqi_r10_s_::
    periodicity_factor_r10_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options,
                         2,
                         value,
                         "cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::subband_cqi_r10_s_::"
                         "periodicity_factor_r10_e_");
}

std::string cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::types_opts::to_string() const
{
  static const char* options[] = {"widebandCQI-r10", "subbandCQI-r10"};
  return convert_enum_idx(
      options, 2, value, "cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::types");
}

std::string cqi_report_periodic_v1320_s::periodicity_factor_wb_r13_opts::to_string() const
{
  static const char* options[] = {"n2", "n4"};
  return convert_enum_idx(options, 2, value, "cqi_report_periodic_v1320_s::periodicity_factor_wb_r13_e_");
}
uint8_t cqi_report_periodic_v1320_s::periodicity_factor_wb_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "cqi_report_periodic_v1320_s::periodicity_factor_wb_r13_e_");
}

std::string meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_::types_opts::to_string() const
{
  static const char* options[] = {"subframeConfig1-5-r10", "subframeConfig0-r10", "subframeConfig6-r10"};
  return convert_enum_idx(options, 3, value, "meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_::types");
}
uint8_t meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1, 0, 6};
  return map_enum_number(options, 3, value, "meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_::types");
}

std::string meas_sf_pattern_r10_c::types_opts::to_string() const
{
  static const char* options[] = {"subframePatternFDD-r10", "subframePatternTDD-r10"};
  return convert_enum_idx(options, 2, value, "meas_sf_pattern_r10_c::types");
}

// PollByte ::= ENUMERATED
std::string poll_byte_opts::to_string() const
{
  static const char* options[] = {"kB25",
                                  "kB50",
                                  "kB75",
                                  "kB100",
                                  "kB125",
                                  "kB250",
                                  "kB375",
                                  "kB500",
                                  "kB750",
                                  "kB1000",
                                  "kB1250",
                                  "kB1500",
                                  "kB2000",
                                  "kB3000",
                                  "kBinfinity",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "poll_byte_e");
}
int16_t poll_byte_opts::to_number() const
{
  static const int16_t options[] = {25, 50, 75, 100, 125, 250, 375, 500, 750, 1000, 1250, 1500, 2000, 3000, -1};
  return map_enum_number(options, 15, value, "poll_byte_e");
}

// PollPDU ::= ENUMERATED
std::string poll_pdu_opts::to_string() const
{
  static const char* options[] = {"p4", "p8", "p16", "p32", "p64", "p128", "p256", "pInfinity"};
  return convert_enum_idx(options, 8, value, "poll_pdu_e");
}
int16_t poll_pdu_opts::to_number() const
{
  static const int16_t options[] = {4, 8, 16, 32, 64, 128, 256, -1};
  return map_enum_number(options, 8, value, "poll_pdu_e");
}

std::string spdcch_elems_r15_c::setup_s_::spdcch_set_ref_sig_r15_opts::to_string() const
{
  static const char* options[] = {"crs", "dmrs"};
  return convert_enum_idx(options, 2, value, "spdcch_elems_r15_c::setup_s_::spdcch_set_ref_sig_r15_e_");
}

std::string spdcch_elems_r15_c::setup_s_::tx_type_r15_opts::to_string() const
{
  static const char* options[] = {"localised", "distributed"};
  return convert_enum_idx(options, 2, value, "spdcch_elems_r15_c::setup_s_::tx_type_r15_e_");
}

std::string spdcch_elems_r15_c::setup_s_::sf_type_r15_opts::to_string() const
{
  static const char* options[] = {"mbsfn", "nonmbsfn", "all"};
  return convert_enum_idx(options, 3, value, "spdcch_elems_r15_c::setup_s_::sf_type_r15_e_");
}

std::string spdcch_elems_r15_c::setup_s_::rate_matching_mode_r15_opts::to_string() const
{
  static const char* options[] = {"m1", "m2", "m3", "m4"};
  return convert_enum_idx(options, 4, value, "spdcch_elems_r15_c::setup_s_::rate_matching_mode_r15_e_");
}
uint8_t spdcch_elems_r15_c::setup_s_::rate_matching_mode_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4};
  return map_enum_number(options, 4, value, "spdcch_elems_r15_c::setup_s_::rate_matching_mode_r15_e_");
}

std::string tpc_idx_c::types_opts::to_string() const
{
  static const char* options[] = {"indexOfFormat3", "indexOfFormat3A"};
  return convert_enum_idx(options, 2, value, "tpc_idx_c::types");
}

std::string ul_am_rlc_r15_s::max_retx_thres_r15_opts::to_string() const
{
  static const char* options[] = {"t1", "t2", "t3", "t4", "t6", "t8", "t16", "t32"};
  return convert_enum_idx(options, 8, value, "ul_am_rlc_r15_s::max_retx_thres_r15_e_");
}
uint8_t ul_am_rlc_r15_s::max_retx_thres_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 6, 8, 16, 32};
  return map_enum_number(options, 8, value, "ul_am_rlc_r15_s::max_retx_thres_r15_e_");
}

std::string cqi_report_cfg_v1250_s::alt_cqi_table_r12_opts::to_string() const
{
  static const char* options[] = {"allSubframes", "csi-SubframeSet1", "csi-SubframeSet2", "spare1"};
  return convert_enum_idx(options, 4, value, "cqi_report_cfg_v1250_s::alt_cqi_table_r12_e_");
}
uint8_t cqi_report_cfg_v1250_s::alt_cqi_table_r12_opts::to_number() const
{
  switch (value) {
    case csi_sf_set1:
      return 1;
    case csi_sf_set2:
      return 2;
    default:
      invalid_enum_number(value, "cqi_report_cfg_v1250_s::alt_cqi_table_r12_e_");
  }
  return 0;
}

std::string csi_rs_cfg_r10_s::csi_rs_r10_c_::setup_s_::ant_ports_count_r10_opts::to_string() const
{
  static const char* options[] = {"an1", "an2", "an4", "an8"};
  return convert_enum_idx(options, 4, value, "csi_rs_cfg_r10_s::csi_rs_r10_c_::setup_s_::ant_ports_count_r10_e_");
}
uint8_t csi_rs_cfg_r10_s::csi_rs_r10_c_::setup_s_::ant_ports_count_r10_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8};
  return map_enum_number(options, 4, value, "csi_rs_cfg_r10_s::csi_rs_r10_c_::setup_s_::ant_ports_count_r10_e_");
}

std::string delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1_r15_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB-2"};
  return convert_enum_idx(
      options, 2, value, "delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1_r15_e_");
}
int8_t delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1_r15_opts::to_number() const
{
  static const int8_t options[] = {0, -2};
  return map_enum_number(
      options, 2, value, "delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1_r15_e_");
}

std::string delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1a_r15_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB-2"};
  return convert_enum_idx(
      options, 2, value, "delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1a_r15_e_");
}
int8_t delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1a_r15_opts::to_number() const
{
  static const int8_t options[] = {0, -2};
  return map_enum_number(
      options, 2, value, "delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1a_r15_e_");
}

std::string delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1b_r15_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB-2"};
  return convert_enum_idx(
      options, 2, value, "delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1b_r15_e_");
}
int8_t delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1b_r15_opts::to_number() const
{
  static const int8_t options[] = {0, -2};
  return map_enum_number(
      options, 2, value, "delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1b_r15_e_");
}

std::string delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format3_r15_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB-2"};
  return convert_enum_idx(
      options, 2, value, "delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format3_r15_e_");
}
int8_t delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format3_r15_opts::to_number() const
{
  static const int8_t options[] = {0, -2};
  return map_enum_number(
      options, 2, value, "delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format3_r15_e_");
}

std::string epdcch_set_cfg_r11_s::tx_type_r11_opts::to_string() const
{
  static const char* options[] = {"localised", "distributed"};
  return convert_enum_idx(options, 2, value, "epdcch_set_cfg_r11_s::tx_type_r11_e_");
}

std::string epdcch_set_cfg_r11_s::res_block_assign_r11_s_::num_prb_pairs_r11_opts::to_string() const
{
  static const char* options[] = {"n2", "n4", "n8"};
  return convert_enum_idx(options, 3, value, "epdcch_set_cfg_r11_s::res_block_assign_r11_s_::num_prb_pairs_r11_e_");
}
uint8_t epdcch_set_cfg_r11_s::res_block_assign_r11_s_::num_prb_pairs_r11_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 8};
  return map_enum_number(options, 3, value, "epdcch_set_cfg_r11_s::res_block_assign_r11_s_::num_prb_pairs_r11_e_");
}

std::string epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::csi_num_repeat_ce_r13_opts::to_string() const
{
  static const char* options[] = {"sf1", "sf2", "sf4", "sf8", "sf16", "sf32"};
  return convert_enum_idx(
      options, 6, value, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::csi_num_repeat_ce_r13_e_");
}
uint8_t epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::csi_num_repeat_ce_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8, 16, 32};
  return map_enum_number(
      options, 6, value, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::csi_num_repeat_ce_r13_e_");
}

std::string epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_pdsch_hop_cfg_r13_opts::to_string() const
{
  static const char* options[] = {"on", "off"};
  return convert_enum_idx(
      options, 2, value, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_pdsch_hop_cfg_r13_e_");
}

std::string
epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::fdd_r13_opts::to_string() const
{
  static const char* options[] = {"v1", "v1dot5", "v2", "v2dot5", "v4", "v5", "v8", "v10"};
  return convert_enum_idx(
      options, 8, value, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::fdd_r13_e_");
}
float epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::fdd_r13_opts::to_number() const
{
  static const float options[] = {1.0, 1.5, 2.0, 2.5, 4.0, 5.0, 8.0, 10.0};
  return map_enum_number(
      options, 8, value, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::fdd_r13_e_");
}
std::string
epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::fdd_r13_opts::to_number_string() const
{
  static const char* options[] = {"1", "1.5", "2", "2.5", "4", "5", "8", "10"};
  return convert_enum_idx(
      options, 8, value, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::fdd_r13_e_");
}

std::string
epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::tdd_r13_opts::to_string() const
{
  static const char* options[] = {"v1", "v2", "v4", "v5", "v8", "v10", "v20", "spare1"};
  return convert_enum_idx(
      options, 8, value, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::tdd_r13_e_");
}
uint8_t epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::tdd_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 5, 8, 10, 20};
  return map_enum_number(
      options, 7, value, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::tdd_r13_e_");
}

std::string
epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"fdd-r13", "tdd-r13"};
  return convert_enum_idx(
      options, 2, value, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::types");
}

std::string epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_num_repeat_r13_opts::to_string() const
{
  static const char* options[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "r128", "r256"};
  return convert_enum_idx(
      options, 9, value, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_num_repeat_r13_e_");
}
uint16_t epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_num_repeat_r13_opts::to_number() const
{
  static const uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
  return map_enum_number(
      options, 9, value, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_num_repeat_r13_e_");
}

std::string enable256_qam_r14_c::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"tpc-SubframeSet-Configured-r14", "tpc-SubframeSet-NotConfigured-r14"};
  return convert_enum_idx(options, 2, value, "enable256_qam_r14_c::setup_c_::types");
}

std::string lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_opts::to_string() const
{
  static const char* options[] = {"kBps0",
                                  "kBps8",
                                  "kBps16",
                                  "kBps32",
                                  "kBps64",
                                  "kBps128",
                                  "kBps256",
                                  "infinity",
                                  "kBps512-v1020",
                                  "kBps1024-v1020",
                                  "kBps2048-v1020",
                                  "spare5",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_e_");
}
int16_t lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_opts::to_number() const
{
  static const int16_t options[] = {0, 8, 16, 32, 64, 128, 256, -1, 512, 1024, 2048};
  return map_enum_number(options, 11, value, "lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_e_");
}

std::string lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_opts::to_string() const
{
  static const char* options[] = {"ms50", "ms100", "ms150", "ms300", "ms500", "ms1000", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_e_");
}
uint16_t lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_opts::to_number() const
{
  static const uint16_t options[] = {50, 100, 150, 300, 500, 1000};
  return map_enum_number(options, 6, value, "lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_e_");
}

std::string lc_ch_cfg_s::bit_rate_query_prohibit_timer_r14_opts::to_string() const
{
  static const char* options[] = {"s0", "s0dot4", "s0dot8", "s1dot6", "s3", "s6", "s12", "s30"};
  return convert_enum_idx(options, 8, value, "lc_ch_cfg_s::bit_rate_query_prohibit_timer_r14_e_");
}
float lc_ch_cfg_s::bit_rate_query_prohibit_timer_r14_opts::to_number() const
{
  static const float options[] = {0.0, 0.4, 0.8, 1.6, 3.0, 6.0, 12.0, 30.0};
  return map_enum_number(options, 8, value, "lc_ch_cfg_s::bit_rate_query_prohibit_timer_r14_e_");
}
std::string lc_ch_cfg_s::bit_rate_query_prohibit_timer_r14_opts::to_number_string() const
{
  static const char* options[] = {"0", "0.4", "0.8", "1.6", "3", "6", "12", "30"};
  return convert_enum_idx(options, 8, value, "lc_ch_cfg_s::bit_rate_query_prohibit_timer_r14_e_");
}

std::string lc_ch_cfg_s::lc_ch_sr_restrict_r15_c_::setup_opts::to_string() const
{
  static const char* options[] = {"spucch", "pucch"};
  return convert_enum_idx(options, 2, value, "lc_ch_cfg_s::lc_ch_sr_restrict_r15_c_::setup_e_");
}

// P-a ::= ENUMERATED
std::string p_a_opts::to_string() const
{
  static const char* options[] = {"dB-6", "dB-4dot77", "dB-3", "dB-1dot77", "dB0", "dB1", "dB2", "dB3"};
  return convert_enum_idx(options, 8, value, "p_a_e");
}
float p_a_opts::to_number() const
{
  static const float options[] = {-6.0, -4.77, -3.0, -1.77, 0.0, 1.0, 2.0, 3.0};
  return map_enum_number(options, 8, value, "p_a_e");
}
std::string p_a_opts::to_number_string() const
{
  static const char* options[] = {"-6", "-4.77", "-3", "-1.77", "0", "1", "2", "3"};
  return convert_enum_idx(options, 8, value, "p_a_e");
}

std::string pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::crs_ports_count_r11_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n4", "spare1"};
  return convert_enum_idx(
      options, 4, value, "pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::crs_ports_count_r11_e_");
}
uint8_t pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::crs_ports_count_r11_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4};
  return map_enum_number(
      options, 3, value, "pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::crs_ports_count_r11_e_");
}

std::string pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::pdsch_start_r11_opts::to_string() const
{
  static const char* options[] = {"reserved", "n1", "n2", "n3", "n4", "assigned"};
  return convert_enum_idx(
      options, 6, value, "pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::pdsch_start_r11_e_");
}
uint8_t pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::pdsch_start_r11_opts::to_number() const
{
  switch (value) {
    case n1:
      return 1;
    case n2:
      return 2;
    case n3:
      return 3;
    case n4:
      return 4;
    default:
      invalid_enum_number(value, "pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::pdsch_start_r11_e_");
  }
  return 0;
}

std::string
pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::setup_s_::crs_ports_count_v1530_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n4", "spare1"};
  return convert_enum_idx(
      options, 4, value, "pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::setup_s_::crs_ports_count_v1530_e_");
}
uint8_t pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::setup_s_::crs_ports_count_v1530_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4};
  return map_enum_number(
      options, 3, value, "pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::setup_s_::crs_ports_count_v1530_e_");
}

std::string pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::setup_s_::pdsch_start_v1530_opts::to_string() const
{
  static const char* options[] = {"reserved", "n1", "n2", "n3", "n4", "assigned"};
  return convert_enum_idx(
      options, 6, value, "pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::setup_s_::pdsch_start_v1530_e_");
}
uint8_t pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::setup_s_::pdsch_start_v1530_opts::to_number() const
{
  switch (value) {
    case n1:
      return 1;
    case n2:
      return 2;
    case n3:
      return 3;
    case n4:
      return 4;
    default:
      invalid_enum_number(value,
                          "pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::setup_s_::pdsch_start_v1530_e_");
  }
  return 0;
}

// PollPDU-v1310 ::= ENUMERATED
std::string poll_pdu_v1310_opts::to_string() const
{
  static const char* options[] = {"p512", "p1024", "p2048", "p4096", "p6144", "p8192", "p12288", "p16384"};
  return convert_enum_idx(options, 8, value, "poll_pdu_v1310_e");
}
uint16_t poll_pdu_v1310_opts::to_number() const
{
  static const uint16_t options[] = {512, 1024, 2048, 4096, 6144, 8192, 12288, 16384};
  return map_enum_number(options, 8, value, "poll_pdu_v1310_e");
}

std::string rlc_cfg_r15_s::mode_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "am-r15", "um-Bi-Directional-r15", "um-Uni-Directional-UL-r15", "um-Uni-Directional-DL-r15"};
  return convert_enum_idx(options, 4, value, "rlc_cfg_r15_s::mode_r15_c_::types");
}

// SRS-AntennaPort ::= ENUMERATED
std::string srs_ant_port_opts::to_string() const
{
  static const char* options[] = {"an1", "an2", "an4", "spare1"};
  return convert_enum_idx(options, 4, value, "srs_ant_port_e");
}
uint8_t srs_ant_port_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4};
  return map_enum_number(options, 3, value, "srs_ant_port_e");
}

// ShortTTI-Length-r15 ::= ENUMERATED
std::string short_tti_len_r15_opts::to_string() const
{
  static const char* options[] = {"slot", "subslot"};
  return convert_enum_idx(options, 2, value, "short_tti_len_r15_e");
}

std::string ul_am_rlc_s::max_retx_thres_opts::to_string() const
{
  static const char* options[] = {"t1", "t2", "t3", "t4", "t6", "t8", "t16", "t32"};
  return convert_enum_idx(options, 8, value, "ul_am_rlc_s::max_retx_thres_e_");
}
uint8_t ul_am_rlc_s::max_retx_thres_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 6, 8, 16, 32};
  return map_enum_number(options, 8, value, "ul_am_rlc_s::max_retx_thres_e_");
}

std::string ant_info_ded_stti_r15_c::setup_s_::tx_mode_dl_mbsfn_r15_opts::to_string() const
{
  static const char* options[] = {"tm9", "tm10"};
  return convert_enum_idx(options, 2, value, "ant_info_ded_stti_r15_c::setup_s_::tx_mode_dl_mbsfn_r15_e_");
}
uint8_t ant_info_ded_stti_r15_c::setup_s_::tx_mode_dl_mbsfn_r15_opts::to_number() const
{
  static const uint8_t options[] = {9, 10};
  return map_enum_number(options, 2, value, "ant_info_ded_stti_r15_c::setup_s_::tx_mode_dl_mbsfn_r15_e_");
}

std::string ant_info_ded_stti_r15_c::setup_s_::tx_mode_dl_non_mbsfn_r15_opts::to_string() const
{
  static const char* options[] = {"tm1", "tm2", "tm3", "tm4", "tm6", "tm8", "tm9", "tm10"};
  return convert_enum_idx(options, 8, value, "ant_info_ded_stti_r15_c::setup_s_::tx_mode_dl_non_mbsfn_r15_e_");
}
uint8_t ant_info_ded_stti_r15_c::setup_s_::tx_mode_dl_non_mbsfn_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 6, 8, 9, 10};
  return map_enum_number(options, 8, value, "ant_info_ded_stti_r15_c::setup_s_::tx_mode_dl_non_mbsfn_r15_e_");
}

std::string ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_::types_opts::to_string() const
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

std::string ant_info_ded_stti_r15_c::setup_s_::max_layers_mimo_stti_r15_opts::to_string() const
{
  static const char* options[] = {"twoLayers", "fourLayers"};
  return convert_enum_idx(options, 2, value, "ant_info_ded_stti_r15_c::setup_s_::max_layers_mimo_stti_r15_e_");
}
uint8_t ant_info_ded_stti_r15_c::setup_s_::max_layers_mimo_stti_r15_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "ant_info_ded_stti_r15_c::setup_s_::max_layers_mimo_stti_r15_e_");
}

std::string ant_info_ul_stti_r15_s::tx_mode_ul_stti_r15_opts::to_string() const
{
  static const char* options[] = {"tm1", "tm2"};
  return convert_enum_idx(options, 2, value, "ant_info_ul_stti_r15_s::tx_mode_ul_stti_r15_e_");
}
uint8_t ant_info_ul_stti_r15_s::tx_mode_ul_stti_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "ant_info_ul_stti_r15_s::tx_mode_ul_stti_r15_e_");
}

std::string cqi_report_cfg_r15_c::setup_s_::alt_cqi_table_minus1024_qam_r15_opts::to_string() const
{
  static const char* options[] = {"allSubframes", "csi-SubframeSet1", "csi-SubframeSet2", "spare1"};
  return convert_enum_idx(options, 4, value, "cqi_report_cfg_r15_c::setup_s_::alt_cqi_table_minus1024_qam_r15_e_");
}
uint8_t cqi_report_cfg_r15_c::setup_s_::alt_cqi_table_minus1024_qam_r15_opts::to_number() const
{
  switch (value) {
    case csi_sf_set1:
      return 1;
    case csi_sf_set2:
      return 2;
    default:
      invalid_enum_number(value, "cqi_report_cfg_r15_c::setup_s_::alt_cqi_table_minus1024_qam_r15_e_");
  }
  return 0;
}

std::string cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_::types_opts::to_string() const
{
  static const char* options[] = {"widebandCQI", "subbandCQI"};
  return convert_enum_idx(options, 2, value, "cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_::types");
}

std::string crs_assist_info_r11_s::ant_ports_count_r11_opts::to_string() const
{
  static const char* options[] = {"an1", "an2", "an4", "spare1"};
  return convert_enum_idx(options, 4, value, "crs_assist_info_r11_s::ant_ports_count_r11_e_");
}
uint8_t crs_assist_info_r11_s::ant_ports_count_r11_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4};
  return map_enum_number(options, 3, value, "crs_assist_info_r11_s::ant_ports_count_r11_e_");
}

std::string crs_assist_info_r13_s::ant_ports_count_r13_opts::to_string() const
{
  static const char* options[] = {"an1", "an2", "an4", "spare1"};
  return convert_enum_idx(options, 4, value, "crs_assist_info_r13_s::ant_ports_count_r13_e_");
}
uint8_t crs_assist_info_r13_s::ant_ports_count_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4};
  return map_enum_number(options, 3, value, "crs_assist_info_r13_s::ant_ports_count_r13_e_");
}

std::string delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format1_r10_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB-2"};
  return convert_enum_idx(
      options, 2, value, "delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format1_r10_e_");
}
int8_t delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format1_r10_opts::to_number() const
{
  static const int8_t options[] = {0, -2};
  return map_enum_number(
      options, 2, value, "delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format1_r10_e_");
}

std::string delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format1a1b_r10_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB-2"};
  return convert_enum_idx(
      options, 2, value, "delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format1a1b_r10_e_");
}
int8_t delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format1a1b_r10_opts::to_number() const
{
  static const int8_t options[] = {0, -2};
  return map_enum_number(
      options, 2, value, "delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format1a1b_r10_e_");
}

std::string delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format22a2b_r10_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB-2"};
  return convert_enum_idx(
      options, 2, value, "delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format22a2b_r10_e_");
}
int8_t delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format22a2b_r10_opts::to_number() const
{
  static const int8_t options[] = {0, -2};
  return map_enum_number(
      options, 2, value, "delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format22a2b_r10_e_");
}

std::string delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format3_r10_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB-2"};
  return convert_enum_idx(
      options, 2, value, "delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format3_r10_e_");
}
int8_t delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format3_r10_opts::to_number() const
{
  static const int8_t options[] = {0, -2};
  return map_enum_number(
      options, 2, value, "delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format3_r10_e_");
}

std::string delta_tx_d_offset_list_pucch_v1130_s::delta_tx_d_offset_pucch_format1b_cs_r11_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB-1"};
  return convert_enum_idx(
      options, 2, value, "delta_tx_d_offset_list_pucch_v1130_s::delta_tx_d_offset_pucch_format1b_cs_r11_e_");
}
int8_t delta_tx_d_offset_list_pucch_v1130_s::delta_tx_d_offset_pucch_format1b_cs_r11_opts::to_number() const
{
  static const int8_t options[] = {0, -1};
  return map_enum_number(
      options, 2, value, "delta_tx_d_offset_list_pucch_v1130_s::delta_tx_d_offset_pucch_format1b_cs_r11_e_");
}

std::string eimta_main_cfg_r12_c::setup_s_::eimta_cmd_periodicity_r12_opts::to_string() const
{
  static const char* options[] = {"sf10", "sf20", "sf40", "sf80"};
  return convert_enum_idx(options, 4, value, "eimta_main_cfg_r12_c::setup_s_::eimta_cmd_periodicity_r12_e_");
}
uint8_t eimta_main_cfg_r12_c::setup_s_::eimta_cmd_periodicity_r12_opts::to_number() const
{
  static const uint8_t options[] = {10, 20, 40, 80};
  return map_enum_number(options, 4, value, "eimta_main_cfg_r12_c::setup_s_::eimta_cmd_periodicity_r12_e_");
}

std::string eimta_main_cfg_serv_cell_r12_c::setup_s_::eimta_harq_ref_cfg_r12_opts::to_string() const
{
  static const char* options[] = {"sa2", "sa4", "sa5"};
  return convert_enum_idx(options, 3, value, "eimta_main_cfg_serv_cell_r12_c::setup_s_::eimta_harq_ref_cfg_r12_e_");
}
uint8_t eimta_main_cfg_serv_cell_r12_c::setup_s_::eimta_harq_ref_cfg_r12_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 5};
  return map_enum_number(options, 3, value, "eimta_main_cfg_serv_cell_r12_c::setup_s_::eimta_harq_ref_cfg_r12_e_");
}

std::string neigh_cells_info_r12_s::crs_ports_count_r12_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n4", "spare"};
  return convert_enum_idx(options, 4, value, "neigh_cells_info_r12_s::crs_ports_count_r12_e_");
}
uint8_t neigh_cells_info_r12_s::crs_ports_count_r12_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4};
  return map_enum_number(options, 3, value, "neigh_cells_info_r12_s::crs_ports_count_r12_e_");
}

// PDCCH-CandidateReductionValue-r13 ::= ENUMERATED
std::string pdcch_candidate_reduction_value_r13_opts::to_string() const
{
  static const char* options[] = {"n0", "n33", "n66", "n100"};
  return convert_enum_idx(options, 4, value, "pdcch_candidate_reduction_value_r13_e");
}
uint8_t pdcch_candidate_reduction_value_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 33, 66, 100};
  return map_enum_number(options, 4, value, "pdcch_candidate_reduction_value_r13_e");
}

std::string pdcp_cfg_s::discard_timer_opts::to_string() const
{
  static const char* options[] = {"ms50", "ms100", "ms150", "ms300", "ms500", "ms750", "ms1500", "infinity"};
  return convert_enum_idx(options, 8, value, "pdcp_cfg_s::discard_timer_e_");
}
int16_t pdcp_cfg_s::discard_timer_opts::to_number() const
{
  static const int16_t options[] = {50, 100, 150, 300, 500, 750, 1500, -1};
  return map_enum_number(options, 8, value, "pdcp_cfg_s::discard_timer_e_");
}

std::string pdcp_cfg_s::rlc_um_s_::pdcp_sn_size_opts::to_string() const
{
  static const char* options[] = {"len7bits", "len12bits"};
  return convert_enum_idx(options, 2, value, "pdcp_cfg_s::rlc_um_s_::pdcp_sn_size_e_");
}
uint8_t pdcp_cfg_s::rlc_um_s_::pdcp_sn_size_opts::to_number() const
{
  static const uint8_t options[] = {7, 12};
  return map_enum_number(options, 2, value, "pdcp_cfg_s::rlc_um_s_::pdcp_sn_size_e_");
}

std::string pdcp_cfg_s::hdr_compress_c_::types_opts::to_string() const
{
  static const char* options[] = {"notUsed", "rohc"};
  return convert_enum_idx(options, 2, value, "pdcp_cfg_s::hdr_compress_c_::types");
}

std::string pdcp_cfg_s::t_reordering_r12_opts::to_string() const
{
  static const char* options[] = {"ms0",    "ms20",   "ms40",    "ms60",    "ms80",    "ms100",   "ms120",   "ms140",
                                  "ms160",  "ms180",  "ms200",   "ms220",   "ms240",   "ms260",   "ms280",   "ms300",
                                  "ms500",  "ms750",  "spare14", "spare13", "spare12", "spare11", "spare10", "spare9",
                                  "spare8", "spare7", "spare6",  "spare5",  "spare4",  "spare3",  "spare2",  "spare1"};
  return convert_enum_idx(options, 32, value, "pdcp_cfg_s::t_reordering_r12_e_");
}
uint16_t pdcp_cfg_s::t_reordering_r12_opts::to_number() const
{
  static const uint16_t options[] = {
      0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300, 500, 750};
  return map_enum_number(options, 18, value, "pdcp_cfg_s::t_reordering_r12_e_");
}

std::string pdcp_cfg_s::ul_data_split_thres_r13_c_::setup_opts::to_string() const
{
  static const char* options[] = {"b0",
                                  "b100",
                                  "b200",
                                  "b400",
                                  "b800",
                                  "b1600",
                                  "b3200",
                                  "b6400",
                                  "b12800",
                                  "b25600",
                                  "b51200",
                                  "b102400",
                                  "b204800",
                                  "b409600",
                                  "b819200",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "pdcp_cfg_s::ul_data_split_thres_r13_c_::setup_e_");
}
uint32_t pdcp_cfg_s::ul_data_split_thres_r13_c_::setup_opts::to_number() const
{
  static const uint32_t options[] = {
      0, 100, 200, 400, 800, 1600, 3200, 6400, 12800, 25600, 51200, 102400, 204800, 409600, 819200};
  return map_enum_number(options, 15, value, "pdcp_cfg_s::ul_data_split_thres_r13_c_::setup_e_");
}

std::string pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_type_for_polling_r13_opts::to_string() const
{
  static const char* options[] = {"type1", "type2"};
  return convert_enum_idx(
      options, 2, value, "pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_type_for_polling_r13_e_");
}
uint8_t pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_type_for_polling_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(
      options, 2, value, "pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_type_for_polling_r13_e_");
}

std::string pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_type1_r13_opts::to_string() const
{
  static const char* options[] = {"ms5",   "ms10",   "ms20",   "ms30",   "ms40",    "ms50",    "ms60",
                                  "ms70",  "ms80",   "ms90",   "ms100",  "ms150",   "ms200",   "ms300",
                                  "ms500", "ms1000", "ms2000", "ms5000", "ms10000", "ms20000", "ms50000"};
  return convert_enum_idx(
      options, 21, value, "pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_type1_r13_e_");
}
uint16_t pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_type1_r13_opts::to_number() const
{
  static const uint16_t options[] = {5,   10,  20,  30,  40,   50,   60,   70,    80,    90,   100,
                                     150, 200, 300, 500, 1000, 2000, 5000, 10000, 20000, 50000};
  return map_enum_number(
      options, 21, value, "pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_type1_r13_e_");
}

std::string pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_type2_r13_opts::to_string() const
{
  static const char* options[] = {"ms5",   "ms10",   "ms20",   "ms30",   "ms40",    "ms50",    "ms60",
                                  "ms70",  "ms80",   "ms90",   "ms100",  "ms150",   "ms200",   "ms300",
                                  "ms500", "ms1000", "ms2000", "ms5000", "ms10000", "ms20000", "ms50000"};
  return convert_enum_idx(
      options, 21, value, "pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_type2_r13_e_");
}
uint16_t pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_type2_r13_opts::to_number() const
{
  static const uint16_t options[] = {5,   10,  20,  30,  40,   50,   60,   70,    80,    90,   100,
                                     150, 200, 300, 500, 1000, 2000, 5000, 10000, 20000, 50000};
  return map_enum_number(
      options, 21, value, "pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_type2_r13_e_");
}

std::string pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_offset_r13_opts::to_string() const
{
  static const char* options[] = {
      "ms1", "ms2", "ms5", "ms10", "ms25", "ms50", "ms100", "ms250", "ms500", "ms2500", "ms5000", "ms25000"};
  return convert_enum_idx(
      options, 12, value, "pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_offset_r13_e_");
}
uint16_t pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_offset_r13_opts::to_number() const
{
  static const uint16_t options[] = {1, 2, 5, 10, 25, 50, 100, 250, 500, 2500, 5000, 25000};
  return map_enum_number(
      options, 12, value, "pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_offset_r13_e_");
}

std::string pdcp_cfg_s::ul_lwa_cfg_r14_c_::setup_s_::ul_lwa_data_split_thres_r14_opts::to_string() const
{
  static const char* options[] = {"b0",
                                  "b100",
                                  "b200",
                                  "b400",
                                  "b800",
                                  "b1600",
                                  "b3200",
                                  "b6400",
                                  "b12800",
                                  "b25600",
                                  "b51200",
                                  "b102400",
                                  "b204800",
                                  "b409600",
                                  "b819200"};
  return convert_enum_idx(
      options, 15, value, "pdcp_cfg_s::ul_lwa_cfg_r14_c_::setup_s_::ul_lwa_data_split_thres_r14_e_");
}
uint32_t pdcp_cfg_s::ul_lwa_cfg_r14_c_::setup_s_::ul_lwa_data_split_thres_r14_opts::to_number() const
{
  static const uint32_t options[] = {
      0, 100, 200, 400, 800, 1600, 3200, 6400, 12800, 25600, 51200, 102400, 204800, 409600, 819200};
  return map_enum_number(options, 15, value, "pdcp_cfg_s::ul_lwa_cfg_r14_c_::setup_s_::ul_lwa_data_split_thres_r14_e_");
}

std::string pdcp_cfg_s::ul_only_hdr_compress_r14_c_::types_opts::to_string() const
{
  static const char* options[] = {"notUsed-r14", "rohc-r14"};
  return convert_enum_idx(options, 2, value, "pdcp_cfg_s::ul_only_hdr_compress_r14_c_::types");
}

std::string pdcp_cfg_s::ul_data_compress_r15_s_::buffer_size_r15_opts::to_string() const
{
  static const char* options[] = {"kbyte2", "kbyte4", "kbyte8", "spare1"};
  return convert_enum_idx(options, 4, value, "pdcp_cfg_s::ul_data_compress_r15_s_::buffer_size_r15_e_");
}
uint8_t pdcp_cfg_s::ul_data_compress_r15_s_::buffer_size_r15_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 8};
  return map_enum_number(options, 3, value, "pdcp_cfg_s::ul_data_compress_r15_s_::buffer_size_r15_e_");
}

std::string pdcp_cfg_s::ul_data_compress_r15_s_::dictionary_r15_opts::to_string() const
{
  static const char* options[] = {"sip-SDP", "operator"};
  return convert_enum_idx(options, 2, value, "pdcp_cfg_s::ul_data_compress_r15_s_::dictionary_r15_e_");
}

std::string pdcp_cfg_s::pdcp_dupl_cfg_r15_c_::setup_s_::pdcp_dupl_r15_opts::to_string() const
{
  static const char* options[] = {"configured", "activated"};
  return convert_enum_idx(options, 2, value, "pdcp_cfg_s::pdcp_dupl_cfg_r15_c_::setup_s_::pdcp_dupl_r15_e_");
}

std::string pucch_cfg_ded_v1530_s::codebooksize_determination_stti_r15_opts::to_string() const
{
  static const char* options[] = {"dai", "cc"};
  return convert_enum_idx(options, 2, value, "pucch_cfg_ded_v1530_s::codebooksize_determination_stti_r15_e_");
}

std::string rlc_bearer_cfg_r15_c::setup_s_::lc_ch_id_cfg_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"logicalChannelIdentity-r15", "logicalChannelIdentityExt-r15"};
  return convert_enum_idx(options, 2, value, "rlc_bearer_cfg_r15_c::setup_s_::lc_ch_id_cfg_r15_c_::types");
}

std::string rlc_cfg_c::types_opts::to_string() const
{
  static const char* options[] = {"am", "um-Bi-Directional", "um-Uni-Directional-UL", "um-Uni-Directional-DL"};
  return convert_enum_idx(options, 4, value, "rlc_cfg_c::types");
}

std::string spdcch_cfg_r15_c::setup_s_::spdcch_l1_reuse_ind_r15_opts::to_string() const
{
  static const char* options[] = {"n0", "n1", "n2"};
  return convert_enum_idx(options, 3, value, "spdcch_cfg_r15_c::setup_s_::spdcch_l1_reuse_ind_r15_e_");
}
uint8_t spdcch_cfg_r15_c::setup_s_::spdcch_l1_reuse_ind_r15_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2};
  return map_enum_number(options, 3, value, "spdcch_cfg_r15_c::setup_s_::spdcch_l1_reuse_ind_r15_e_");
}

std::string sps_cfg_sl_r14_s::semi_persist_sched_interv_sl_r14_opts::to_string() const
{
  static const char* options[] = {"sf20",
                                  "sf50",
                                  "sf100",
                                  "sf200",
                                  "sf300",
                                  "sf400",
                                  "sf500",
                                  "sf600",
                                  "sf700",
                                  "sf800",
                                  "sf900",
                                  "sf1000",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "sps_cfg_sl_r14_s::semi_persist_sched_interv_sl_r14_e_");
}
uint16_t sps_cfg_sl_r14_s::semi_persist_sched_interv_sl_r14_opts::to_number() const
{
  static const uint16_t options[] = {20, 50, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
  return map_enum_number(options, 12, value, "sps_cfg_sl_r14_s::semi_persist_sched_interv_sl_r14_e_");
}

std::string sps_cfg_ul_c::setup_s_::semi_persist_sched_interv_ul_opts::to_string() const
{
  static const char* options[] = {"sf10",
                                  "sf20",
                                  "sf32",
                                  "sf40",
                                  "sf64",
                                  "sf80",
                                  "sf128",
                                  "sf160",
                                  "sf320",
                                  "sf640",
                                  "sf1-v1430",
                                  "sf2-v1430",
                                  "sf3-v1430",
                                  "sf4-v1430",
                                  "sf5-v1430",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "sps_cfg_ul_c::setup_s_::semi_persist_sched_interv_ul_e_");
}
uint16_t sps_cfg_ul_c::setup_s_::semi_persist_sched_interv_ul_opts::to_number() const
{
  static const uint16_t options[] = {10, 20, 32, 40, 64, 80, 128, 160, 320, 640, 1, 2, 3, 4, 5};
  return map_enum_number(options, 15, value, "sps_cfg_ul_c::setup_s_::semi_persist_sched_interv_ul_e_");
}

std::string sps_cfg_ul_c::setup_s_::implicit_release_after_opts::to_string() const
{
  static const char* options[] = {"e2", "e3", "e4", "e8"};
  return convert_enum_idx(options, 4, value, "sps_cfg_ul_c::setup_s_::implicit_release_after_e_");
}
uint8_t sps_cfg_ul_c::setup_s_::implicit_release_after_opts::to_number() const
{
  static const uint8_t options[] = {2, 3, 4, 8};
  return map_enum_number(options, 4, value, "sps_cfg_ul_c::setup_s_::implicit_release_after_e_");
}

std::string sps_cfg_ul_c::setup_s_::semi_persist_sched_interv_ul_v1430_opts::to_string() const
{
  static const char* options[] = {"sf50",
                                  "sf100",
                                  "sf200",
                                  "sf300",
                                  "sf400",
                                  "sf500",
                                  "sf600",
                                  "sf700",
                                  "sf800",
                                  "sf900",
                                  "sf1000",
                                  "spare5",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "sps_cfg_ul_c::setup_s_::semi_persist_sched_interv_ul_v1430_e_");
}
uint16_t sps_cfg_ul_c::setup_s_::semi_persist_sched_interv_ul_v1430_opts::to_number() const
{
  static const uint16_t options[] = {50, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
  return map_enum_number(options, 11, value, "sps_cfg_ul_c::setup_s_::semi_persist_sched_interv_ul_v1430_e_");
}

std::string sps_cfg_ul_c::setup_s_::cyclic_shift_sps_r15_opts::to_string() const
{
  static const char* options[] = {"cs0", "cs1", "cs2", "cs3", "cs4", "cs5", "cs6", "cs7"};
  return convert_enum_idx(options, 8, value, "sps_cfg_ul_c::setup_s_::cyclic_shift_sps_r15_e_");
}
uint8_t sps_cfg_ul_c::setup_s_::cyclic_shift_sps_r15_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7};
  return map_enum_number(options, 8, value, "sps_cfg_ul_c::setup_s_::cyclic_shift_sps_r15_e_");
}

std::string sps_cfg_ul_c::setup_s_::rv_sps_ul_repeats_r15_opts::to_string() const
{
  static const char* options[] = {"ulrvseq1", "ulrvseq2", "ulrvseq3"};
  return convert_enum_idx(options, 3, value, "sps_cfg_ul_c::setup_s_::rv_sps_ul_repeats_r15_e_");
}
uint8_t sps_cfg_ul_c::setup_s_::rv_sps_ul_repeats_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3};
  return map_enum_number(options, 3, value, "sps_cfg_ul_c::setup_s_::rv_sps_ul_repeats_r15_e_");
}

std::string sps_cfg_ul_c::setup_s_::total_num_pusch_sps_ul_repeats_r15_opts::to_string() const
{
  static const char* options[] = {"n2", "n3", "n4", "n6"};
  return convert_enum_idx(options, 4, value, "sps_cfg_ul_c::setup_s_::total_num_pusch_sps_ul_repeats_r15_e_");
}
uint8_t sps_cfg_ul_c::setup_s_::total_num_pusch_sps_ul_repeats_r15_opts::to_number() const
{
  static const uint8_t options[] = {2, 3, 4, 6};
  return map_enum_number(options, 4, value, "sps_cfg_ul_c::setup_s_::total_num_pusch_sps_ul_repeats_r15_e_");
}

std::string sps_cfg_ul_stti_r15_c::setup_s_::semi_persist_sched_interv_ul_stti_r15_opts::to_string() const
{
  static const char* options[] = {"sTTI1",
                                  "sTTI2",
                                  "sTTI3",
                                  "sTTI4",
                                  "sTTI6",
                                  "sTTI8",
                                  "sTTI12",
                                  "sTTI16",
                                  "sTTI20",
                                  "sTTI40",
                                  "sTTI60",
                                  "sTTI80",
                                  "sTTI120",
                                  "sTTI240",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(
      options, 16, value, "sps_cfg_ul_stti_r15_c::setup_s_::semi_persist_sched_interv_ul_stti_r15_e_");
}
uint8_t sps_cfg_ul_stti_r15_c::setup_s_::semi_persist_sched_interv_ul_stti_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 6, 8, 12, 16, 20, 40, 60, 80, 120, 240};
  return map_enum_number(
      options, 14, value, "sps_cfg_ul_stti_r15_c::setup_s_::semi_persist_sched_interv_ul_stti_r15_e_");
}

std::string sps_cfg_ul_stti_r15_c::setup_s_::implicit_release_after_opts::to_string() const
{
  static const char* options[] = {"e2", "e3", "e4", "e8"};
  return convert_enum_idx(options, 4, value, "sps_cfg_ul_stti_r15_c::setup_s_::implicit_release_after_e_");
}
uint8_t sps_cfg_ul_stti_r15_c::setup_s_::implicit_release_after_opts::to_number() const
{
  static const uint8_t options[] = {2, 3, 4, 8};
  return map_enum_number(options, 4, value, "sps_cfg_ul_stti_r15_c::setup_s_::implicit_release_after_e_");
}

std::string sps_cfg_ul_stti_r15_c::setup_s_::cyclic_shift_sps_s_tti_r15_opts::to_string() const
{
  static const char* options[] = {"cs0", "cs1", "cs2", "cs3", "cs4", "cs5", "cs6", "cs7"};
  return convert_enum_idx(options, 8, value, "sps_cfg_ul_stti_r15_c::setup_s_::cyclic_shift_sps_s_tti_r15_e_");
}
uint8_t sps_cfg_ul_stti_r15_c::setup_s_::cyclic_shift_sps_s_tti_r15_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7};
  return map_enum_number(options, 8, value, "sps_cfg_ul_stti_r15_c::setup_s_::cyclic_shift_sps_s_tti_r15_e_");
}

std::string sps_cfg_ul_stti_r15_c::setup_s_::rv_sps_stti_ul_repeats_r15_opts::to_string() const
{
  static const char* options[] = {"ulrvseq1", "ulrvseq2", "ulrvseq3"};
  return convert_enum_idx(options, 3, value, "sps_cfg_ul_stti_r15_c::setup_s_::rv_sps_stti_ul_repeats_r15_e_");
}
uint8_t sps_cfg_ul_stti_r15_c::setup_s_::rv_sps_stti_ul_repeats_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3};
  return map_enum_number(options, 3, value, "sps_cfg_ul_stti_r15_c::setup_s_::rv_sps_stti_ul_repeats_r15_e_");
}

std::string sps_cfg_ul_stti_r15_c::setup_s_::tbs_scaling_factor_subslot_sps_ul_repeats_r15_opts::to_string() const
{
  static const char* options[] = {"n6", "n12"};
  return convert_enum_idx(
      options, 2, value, "sps_cfg_ul_stti_r15_c::setup_s_::tbs_scaling_factor_subslot_sps_ul_repeats_r15_e_");
}
uint8_t sps_cfg_ul_stti_r15_c::setup_s_::tbs_scaling_factor_subslot_sps_ul_repeats_r15_opts::to_number() const
{
  static const uint8_t options[] = {6, 12};
  return map_enum_number(
      options, 2, value, "sps_cfg_ul_stti_r15_c::setup_s_::tbs_scaling_factor_subslot_sps_ul_repeats_r15_e_");
}

std::string sps_cfg_ul_stti_r15_c::setup_s_::total_num_pusch_sps_stti_ul_repeats_r15_opts::to_string() const
{
  static const char* options[] = {"n2", "n3", "n4", "n6"};
  return convert_enum_idx(
      options, 4, value, "sps_cfg_ul_stti_r15_c::setup_s_::total_num_pusch_sps_stti_ul_repeats_r15_e_");
}
uint8_t sps_cfg_ul_stti_r15_c::setup_s_::total_num_pusch_sps_stti_ul_repeats_r15_opts::to_number() const
{
  static const uint8_t options[] = {2, 3, 4, 6};
  return map_enum_number(
      options, 4, value, "sps_cfg_ul_stti_r15_c::setup_s_::total_num_pusch_sps_stti_ul_repeats_r15_e_");
}

std::string srs_cfg_ap_r10_s::srs_bw_ap_r10_opts::to_string() const
{
  static const char* options[] = {"bw0", "bw1", "bw2", "bw3"};
  return convert_enum_idx(options, 4, value, "srs_cfg_ap_r10_s::srs_bw_ap_r10_e_");
}
uint8_t srs_cfg_ap_r10_s::srs_bw_ap_r10_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3};
  return map_enum_number(options, 4, value, "srs_cfg_ap_r10_s::srs_bw_ap_r10_e_");
}

std::string srs_cfg_ap_r10_s::cyclic_shift_ap_r10_opts::to_string() const
{
  static const char* options[] = {"cs0", "cs1", "cs2", "cs3", "cs4", "cs5", "cs6", "cs7"};
  return convert_enum_idx(options, 8, value, "srs_cfg_ap_r10_s::cyclic_shift_ap_r10_e_");
}
uint8_t srs_cfg_ap_r10_s::cyclic_shift_ap_r10_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7};
  return map_enum_number(options, 8, value, "srs_cfg_ap_r10_s::cyclic_shift_ap_r10_e_");
}

std::string srs_cfg_ap_r13_s::srs_bw_ap_r13_opts::to_string() const
{
  static const char* options[] = {"bw0", "bw1", "bw2", "bw3"};
  return convert_enum_idx(options, 4, value, "srs_cfg_ap_r13_s::srs_bw_ap_r13_e_");
}
uint8_t srs_cfg_ap_r13_s::srs_bw_ap_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3};
  return map_enum_number(options, 4, value, "srs_cfg_ap_r13_s::srs_bw_ap_r13_e_");
}

std::string srs_cfg_ap_r13_s::cyclic_shift_ap_r13_opts::to_string() const
{
  static const char* options[] = {"cs0", "cs1", "cs2", "cs3", "cs4", "cs5", "cs6", "cs7", "cs8", "cs9", "cs10", "cs11"};
  return convert_enum_idx(options, 12, value, "srs_cfg_ap_r13_s::cyclic_shift_ap_r13_e_");
}
uint8_t srs_cfg_ap_r13_s::cyclic_shift_ap_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  return map_enum_number(options, 12, value, "srs_cfg_ap_r13_s::cyclic_shift_ap_r13_e_");
}

std::string srs_cfg_ap_r13_s::tx_comb_num_r13_opts::to_string() const
{
  static const char* options[] = {"n2", "n4"};
  return convert_enum_idx(options, 2, value, "srs_cfg_ap_r13_s::tx_comb_num_r13_e_");
}
uint8_t srs_cfg_ap_r13_s::tx_comb_num_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "srs_cfg_ap_r13_s::tx_comb_num_r13_e_");
}

std::string srs_cfg_ap_v1310_s::cyclic_shift_ap_v1310_opts::to_string() const
{
  static const char* options[] = {"cs8", "cs9", "cs10", "cs11"};
  return convert_enum_idx(options, 4, value, "srs_cfg_ap_v1310_s::cyclic_shift_ap_v1310_e_");
}
uint8_t srs_cfg_ap_v1310_s::cyclic_shift_ap_v1310_opts::to_number() const
{
  static const uint8_t options[] = {8, 9, 10, 11};
  return map_enum_number(options, 4, value, "srs_cfg_ap_v1310_s::cyclic_shift_ap_v1310_e_");
}

std::string srs_cfg_ap_v1310_s::tx_comb_num_r13_opts::to_string() const
{
  static const char* options[] = {"n2", "n4"};
  return convert_enum_idx(options, 2, value, "srs_cfg_ap_v1310_s::tx_comb_num_r13_e_");
}
uint8_t srs_cfg_ap_v1310_s::tx_comb_num_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "srs_cfg_ap_v1310_s::tx_comb_num_r13_e_");
}

std::string sched_request_cfg_v1530_c::setup_s_::dssr_trans_max_r15_opts::to_string() const
{
  static const char* options[] = {"n4", "n8", "n16", "n32", "n64", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "sched_request_cfg_v1530_c::setup_s_::dssr_trans_max_r15_e_");
}
uint8_t sched_request_cfg_v1530_c::setup_s_::dssr_trans_max_r15_opts::to_number() const
{
  static const uint8_t options[] = {4, 8, 16, 32, 64};
  return map_enum_number(options, 5, value, "sched_request_cfg_v1530_c::setup_s_::dssr_trans_max_r15_e_");
}

std::string slot_or_subslot_pdsch_cfg_r15_c::setup_s_::alt_cqi_table_stti_r15_opts::to_string() const
{
  static const char* options[] = {"allSubframes", "csi-SubframeSet1", "csi-SubframeSet2", "spare1"};
  return convert_enum_idx(options, 4, value, "slot_or_subslot_pdsch_cfg_r15_c::setup_s_::alt_cqi_table_stti_r15_e_");
}
uint8_t slot_or_subslot_pdsch_cfg_r15_c::setup_s_::alt_cqi_table_stti_r15_opts::to_number() const
{
  switch (value) {
    case csi_sf_set1:
      return 1;
    case csi_sf_set2:
      return 2;
    default:
      invalid_enum_number(value, "slot_or_subslot_pdsch_cfg_r15_c::setup_s_::alt_cqi_table_stti_r15_e_");
  }
  return 0;
}

std::string slot_or_subslot_pdsch_cfg_r15_c::setup_s_::alt_cqi_table1024_qam_stti_r15_opts::to_string() const
{
  static const char* options[] = {"allSubframes", "csi-SubframeSet1", "csi-SubframeSet2", "spare1"};
  return convert_enum_idx(
      options, 4, value, "slot_or_subslot_pdsch_cfg_r15_c::setup_s_::alt_cqi_table1024_qam_stti_r15_e_");
}
uint8_t slot_or_subslot_pdsch_cfg_r15_c::setup_s_::alt_cqi_table1024_qam_stti_r15_opts::to_number() const
{
  switch (value) {
    case csi_sf_set1:
      return 1;
    case csi_sf_set2:
      return 2;
    default:
      invalid_enum_number(value, "slot_or_subslot_pdsch_cfg_r15_c::setup_s_::alt_cqi_table1024_qam_stti_r15_e_");
  }
  return 0;
}

std::string slot_or_subslot_pdsch_cfg_r15_c::setup_s_::res_alloc_r15_opts::to_string() const
{
  static const char* options[] = {"resourceAllocationType0", "resourceAllocationType2"};
  return convert_enum_idx(options, 2, value, "slot_or_subslot_pdsch_cfg_r15_c::setup_s_::res_alloc_r15_e_");
}
uint8_t slot_or_subslot_pdsch_cfg_r15_c::setup_s_::res_alloc_r15_opts::to_number() const
{
  static const uint8_t options[] = {0, 2};
  return map_enum_number(options, 2, value, "slot_or_subslot_pdsch_cfg_r15_c::setup_s_::res_alloc_r15_e_");
}

std::string tdd_pusch_up_pts_r14_c::setup_s_::sym_pusch_up_pts_r14_opts::to_string() const
{
  static const char* options[] = {"sym1", "sym2", "sym3", "sym4", "sym5", "sym6"};
  return convert_enum_idx(options, 6, value, "tdd_pusch_up_pts_r14_c::setup_s_::sym_pusch_up_pts_r14_e_");
}
uint8_t tdd_pusch_up_pts_r14_c::setup_s_::sym_pusch_up_pts_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5, 6};
  return map_enum_number(options, 6, value, "tdd_pusch_up_pts_r14_c::setup_s_::sym_pusch_up_pts_r14_e_");
}

std::string ant_info_ded_s::tx_mode_opts::to_string() const
{
  static const char* options[] = {"tm1", "tm2", "tm3", "tm4", "tm5", "tm6", "tm7", "tm8-v920"};
  return convert_enum_idx(options, 8, value, "ant_info_ded_s::tx_mode_e_");
}
uint8_t ant_info_ded_s::tx_mode_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5, 6, 7, 8};
  return map_enum_number(options, 8, value, "ant_info_ded_s::tx_mode_e_");
}

std::string ant_info_ded_s::codebook_subset_restrict_c_::types_opts::to_string() const
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

std::string ant_info_ded_s::ue_tx_ant_sel_c_::setup_opts::to_string() const
{
  static const char* options[] = {"closedLoop", "openLoop"};
  return convert_enum_idx(options, 2, value, "ant_info_ded_s::ue_tx_ant_sel_c_::setup_e_");
}

std::string ant_info_ded_r10_s::tx_mode_r10_opts::to_string() const
{
  static const char* options[] = {"tm1",
                                  "tm2",
                                  "tm3",
                                  "tm4",
                                  "tm5",
                                  "tm6",
                                  "tm7",
                                  "tm8-v920",
                                  "tm9-v1020",
                                  "tm10-v1130",
                                  "spare6",
                                  "spare5",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "ant_info_ded_r10_s::tx_mode_r10_e_");
}
uint8_t ant_info_ded_r10_s::tx_mode_r10_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  return map_enum_number(options, 10, value, "ant_info_ded_r10_s::tx_mode_r10_e_");
}

std::string ant_info_ded_r10_s::ue_tx_ant_sel_c_::setup_opts::to_string() const
{
  static const char* options[] = {"closedLoop", "openLoop"};
  return convert_enum_idx(options, 2, value, "ant_info_ded_r10_s::ue_tx_ant_sel_c_::setup_e_");
}

std::string ant_info_ded_v1530_c::setup_c_::ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15_opts::to_string() const
{
  static const char* options[] = {"two", "three"};
  return convert_enum_idx(
      options, 2, value, "ant_info_ded_v1530_c::setup_c_::ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15_e_");
}
uint8_t ant_info_ded_v1530_c::setup_c_::ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15_opts::to_number() const
{
  static const uint8_t options[] = {2, 3};
  return map_enum_number(
      options, 2, value, "ant_info_ded_v1530_c::setup_c_::ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15_e_");
}

std::string ant_info_ded_v1530_c::setup_c_::types_opts::to_string() const
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

std::string ant_info_ded_v920_s::codebook_subset_restrict_v920_c_::types_opts::to_string() const
{
  static const char* options[] = {"n2TxAntenna-tm8-r9", "n4TxAntenna-tm8-r9"};
  return convert_enum_idx(options, 2, value, "ant_info_ded_v920_s::codebook_subset_restrict_v920_c_::types");
}
uint8_t ant_info_ded_v920_s::codebook_subset_restrict_v920_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "ant_info_ded_v920_s::codebook_subset_restrict_v920_c_::types");
}

std::string ant_info_ul_r10_s::tx_mode_ul_r10_opts::to_string() const
{
  static const char* options[] = {"tm1", "tm2", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "ant_info_ul_r10_s::tx_mode_ul_r10_e_");
}
uint8_t ant_info_ul_r10_s::tx_mode_ul_r10_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "ant_info_ul_r10_s::tx_mode_ul_r10_e_");
}

std::string cqi_report_cfg_v1530_s::alt_cqi_table_minus1024_qam_r15_opts::to_string() const
{
  static const char* options[] = {"allSubframes", "csi-SubframeSet1", "csi-SubframeSet2", "spare1"};
  return convert_enum_idx(options, 4, value, "cqi_report_cfg_v1530_s::alt_cqi_table_minus1024_qam_r15_e_");
}
uint8_t cqi_report_cfg_v1530_s::alt_cqi_table_minus1024_qam_r15_opts::to_number() const
{
  switch (value) {
    case csi_sf_set1:
      return 1;
    case csi_sf_set2:
      return 2;
    default:
      invalid_enum_number(value, "cqi_report_cfg_v1530_s::alt_cqi_table_minus1024_qam_r15_e_");
  }
  return 0;
}

std::string drb_to_add_mod_s::drb_type_lwip_r13_opts::to_string() const
{
  static const char* options[] = {"lwip", "lwip-DL-only", "lwip-UL-only", "eutran"};
  return convert_enum_idx(options, 4, value, "drb_to_add_mod_s::drb_type_lwip_r13_e_");
}

std::string drb_to_add_mod_s::lwa_wlan_ac_r14_opts::to_string() const
{
  static const char* options[] = {"ac-bk", "ac-be", "ac-vi", "ac-vo"};
  return convert_enum_idx(options, 4, value, "drb_to_add_mod_s::lwa_wlan_ac_r14_e_");
}

std::string drx_cfg_c::setup_s_::on_dur_timer_opts::to_string() const
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
  return convert_enum_idx(options, 16, value, "drx_cfg_c::setup_s_::on_dur_timer_e_");
}
uint8_t drx_cfg_c::setup_s_::on_dur_timer_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5, 6, 8, 10, 20, 30, 40, 50, 60, 80, 100, 200};
  return map_enum_number(options, 16, value, "drx_cfg_c::setup_s_::on_dur_timer_e_");
}

std::string drx_cfg_c::setup_s_::drx_inactivity_timer_opts::to_string() const
{
  static const char* options[] = {"psf1",    "psf2",       "psf3",   "psf4",   "psf5",   "psf6",    "psf8",
                                  "psf10",   "psf20",      "psf30",  "psf40",  "psf50",  "psf60",   "psf80",
                                  "psf100",  "psf200",     "psf300", "psf500", "psf750", "psf1280", "psf1920",
                                  "psf2560", "psf0-v1020", "spare9", "spare8", "spare7", "spare6",  "spare5",
                                  "spare4",  "spare3",     "spare2", "spare1"};
  return convert_enum_idx(options, 32, value, "drx_cfg_c::setup_s_::drx_inactivity_timer_e_");
}
uint16_t drx_cfg_c::setup_s_::drx_inactivity_timer_opts::to_number() const
{
  static const uint16_t options[] = {1,  2,  3,   4,   5,   6,   8,   10,   20,   30,   40, 50,
                                     60, 80, 100, 200, 300, 500, 750, 1280, 1920, 2560, 0};
  return map_enum_number(options, 23, value, "drx_cfg_c::setup_s_::drx_inactivity_timer_e_");
}

std::string drx_cfg_c::setup_s_::drx_retx_timer_opts::to_string() const
{
  static const char* options[] = {"psf1", "psf2", "psf4", "psf6", "psf8", "psf16", "psf24", "psf33"};
  return convert_enum_idx(options, 8, value, "drx_cfg_c::setup_s_::drx_retx_timer_e_");
}
uint8_t drx_cfg_c::setup_s_::drx_retx_timer_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 6, 8, 16, 24, 33};
  return map_enum_number(options, 8, value, "drx_cfg_c::setup_s_::drx_retx_timer_e_");
}

std::string drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::types_opts::to_string() const
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

std::string drx_cfg_c::setup_s_::short_drx_s_::short_drx_cycle_opts::to_string() const
{
  static const char* options[] = {"sf2",
                                  "sf5",
                                  "sf8",
                                  "sf10",
                                  "sf16",
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
                                  "sf640"};
  return convert_enum_idx(options, 16, value, "drx_cfg_c::setup_s_::short_drx_s_::short_drx_cycle_e_");
}
uint16_t drx_cfg_c::setup_s_::short_drx_s_::short_drx_cycle_opts::to_number() const
{
  static const uint16_t options[] = {2, 5, 8, 10, 16, 20, 32, 40, 64, 80, 128, 160, 256, 320, 512, 640};
  return map_enum_number(options, 16, value, "drx_cfg_c::setup_s_::short_drx_s_::short_drx_cycle_e_");
}

std::string drx_cfg_r13_s::on_dur_timer_v1310_opts::to_string() const
{
  static const char* options[] = {"psf300", "psf400", "psf500", "psf600", "psf800", "psf1000", "psf1200", "psf1600"};
  return convert_enum_idx(options, 8, value, "drx_cfg_r13_s::on_dur_timer_v1310_e_");
}
uint16_t drx_cfg_r13_s::on_dur_timer_v1310_opts::to_number() const
{
  static const uint16_t options[] = {300, 400, 500, 600, 800, 1000, 1200, 1600};
  return map_enum_number(options, 8, value, "drx_cfg_r13_s::on_dur_timer_v1310_e_");
}

std::string drx_cfg_r13_s::drx_retx_timer_v1310_opts::to_string() const
{
  static const char* options[] = {"psf40", "psf64", "psf80", "psf96", "psf112", "psf128", "psf160", "psf320"};
  return convert_enum_idx(options, 8, value, "drx_cfg_r13_s::drx_retx_timer_v1310_e_");
}
uint16_t drx_cfg_r13_s::drx_retx_timer_v1310_opts::to_number() const
{
  static const uint16_t options[] = {40, 64, 80, 96, 112, 128, 160, 320};
  return map_enum_number(options, 8, value, "drx_cfg_r13_s::drx_retx_timer_v1310_e_");
}

std::string drx_cfg_r13_s::drx_ul_retx_timer_r13_opts::to_string() const
{
  static const char* options[] = {"psf0",
                                  "psf1",
                                  "psf2",
                                  "psf4",
                                  "psf6",
                                  "psf8",
                                  "psf16",
                                  "psf24",
                                  "psf33",
                                  "psf40",
                                  "psf64",
                                  "psf80",
                                  "psf96",
                                  "psf112",
                                  "psf128",
                                  "psf160",
                                  "psf320"};
  return convert_enum_idx(options, 17, value, "drx_cfg_r13_s::drx_ul_retx_timer_r13_e_");
}
uint16_t drx_cfg_r13_s::drx_ul_retx_timer_r13_opts::to_number() const
{
  static const uint16_t options[] = {0, 1, 2, 4, 6, 8, 16, 24, 33, 40, 64, 80, 96, 112, 128, 160, 320};
  return map_enum_number(options, 17, value, "drx_cfg_r13_s::drx_ul_retx_timer_r13_e_");
}

std::string drx_cfg_r15_s::drx_retx_timer_short_tti_r15_opts::to_string() const
{
  static const char* options[] = {
      "tti10", "tti20", "tti40", "tti64", "tti80", "tti96", "tti112", "tti128", "tti160", "tti320"};
  return convert_enum_idx(options, 10, value, "drx_cfg_r15_s::drx_retx_timer_short_tti_r15_e_");
}
uint16_t drx_cfg_r15_s::drx_retx_timer_short_tti_r15_opts::to_number() const
{
  static const uint16_t options[] = {10, 20, 40, 64, 80, 96, 112, 128, 160, 320};
  return map_enum_number(options, 10, value, "drx_cfg_r15_s::drx_retx_timer_short_tti_r15_e_");
}

std::string drx_cfg_r15_s::drx_ul_retx_timer_short_tti_r15_opts::to_string() const
{
  static const char* options[] = {"tti0",
                                  "tti1",
                                  "tti2",
                                  "tti4",
                                  "tti6",
                                  "tti8",
                                  "tti16",
                                  "tti24",
                                  "tti33",
                                  "tti40",
                                  "tti64",
                                  "tti80",
                                  "tti96",
                                  "tti112",
                                  "tti128",
                                  "tti160",
                                  "tti320"};
  return convert_enum_idx(options, 17, value, "drx_cfg_r15_s::drx_ul_retx_timer_short_tti_r15_e_");
}
uint16_t drx_cfg_r15_s::drx_ul_retx_timer_short_tti_r15_opts::to_number() const
{
  static const uint16_t options[] = {0, 1, 2, 4, 6, 8, 16, 24, 33, 40, 64, 80, 96, 112, 128, 160, 320};
  return map_enum_number(options, 17, value, "drx_cfg_r15_s::drx_ul_retx_timer_short_tti_r15_e_");
}

std::string drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_::types_opts::to_string() const
{
  static const char* options[] = {"sf60-v1130", "sf70-v1130"};
  return convert_enum_idx(options, 2, value, "drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_::types");
}
uint8_t drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {60, 70};
  return map_enum_number(options, 2, value, "drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_::types");
}

// DataInactivityTimer-r14 ::= ENUMERATED
std::string data_inactivity_timer_r14_opts::to_string() const
{
  static const char* options[] = {
      "s1", "s2", "s3", "s5", "s7", "s10", "s15", "s20", "s40", "s50", "s60", "s80", "s100", "s120", "s150", "s180"};
  return convert_enum_idx(options, 16, value, "data_inactivity_timer_r14_e");
}
uint8_t data_inactivity_timer_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 5, 7, 10, 15, 20, 40, 50, 60, 80, 100, 120, 150, 180};
  return map_enum_number(options, 16, value, "data_inactivity_timer_r14_e");
}

std::string pdsch_cfg_ded_s::p_a_opts::to_string() const
{
  static const char* options[] = {"dB-6", "dB-4dot77", "dB-3", "dB-1dot77", "dB0", "dB1", "dB2", "dB3"};
  return convert_enum_idx(options, 8, value, "pdsch_cfg_ded_s::p_a_e_");
}
float pdsch_cfg_ded_s::p_a_opts::to_number() const
{
  static const float options[] = {-6.0, -4.77, -3.0, -1.77, 0.0, 1.0, 2.0, 3.0};
  return map_enum_number(options, 8, value, "pdsch_cfg_ded_s::p_a_e_");
}
std::string pdsch_cfg_ded_s::p_a_opts::to_number_string() const
{
  static const char* options[] = {"-6", "-4.77", "-3", "-1.77", "0", "1", "2", "3"};
  return convert_enum_idx(options, 8, value, "pdsch_cfg_ded_s::p_a_e_");
}

std::string pdsch_cfg_ded_v1130_s::qcl_operation_opts::to_string() const
{
  static const char* options[] = {"typeA", "typeB"};
  return convert_enum_idx(options, 2, value, "pdsch_cfg_ded_v1130_s::qcl_operation_e_");
}

std::string pdsch_cfg_ded_v1280_s::tbs_idx_alt_r12_opts::to_string() const
{
  static const char* options[] = {"a26", "a33"};
  return convert_enum_idx(options, 2, value, "pdsch_cfg_ded_v1280_s::tbs_idx_alt_r12_e_");
}
uint8_t pdsch_cfg_ded_v1280_s::tbs_idx_alt_r12_opts::to_number() const
{
  static const uint8_t options[] = {26, 33};
  return map_enum_number(options, 2, value, "pdsch_cfg_ded_v1280_s::tbs_idx_alt_r12_e_");
}

std::string pdsch_cfg_ded_v1430_s::ce_pdsch_max_bw_r14_opts::to_string() const
{
  static const char* options[] = {"bw5", "bw20"};
  return convert_enum_idx(options, 2, value, "pdsch_cfg_ded_v1430_s::ce_pdsch_max_bw_r14_e_");
}
uint8_t pdsch_cfg_ded_v1430_s::ce_pdsch_max_bw_r14_opts::to_number() const
{
  static const uint8_t options[] = {5, 20};
  return map_enum_number(options, 2, value, "pdsch_cfg_ded_v1430_s::ce_pdsch_max_bw_r14_e_");
}

std::string pdsch_cfg_ded_v1430_s::ce_sched_enhancement_r14_opts::to_string() const
{
  static const char* options[] = {"range1", "range2"};
  return convert_enum_idx(options, 2, value, "pdsch_cfg_ded_v1430_s::ce_sched_enhancement_r14_e_");
}
uint8_t pdsch_cfg_ded_v1430_s::ce_sched_enhancement_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "pdsch_cfg_ded_v1430_s::ce_sched_enhancement_r14_e_");
}

std::string pdsch_cfg_ded_v1530_s::alt_mcs_table_scaling_cfg_r15_opts::to_string() const
{
  static const char* options[] = {"oDot5", "oDot625", "oDot75", "oDot875"};
  return convert_enum_idx(options, 4, value, "pdsch_cfg_ded_v1530_s::alt_mcs_table_scaling_cfg_r15_e_");
}
float pdsch_cfg_ded_v1530_s::alt_mcs_table_scaling_cfg_r15_opts::to_number() const
{
  static const float options[] = {0.5, 0.625, 0.75, 0.875};
  return map_enum_number(options, 4, value, "pdsch_cfg_ded_v1530_s::alt_mcs_table_scaling_cfg_r15_e_");
}
std::string pdsch_cfg_ded_v1530_s::alt_mcs_table_scaling_cfg_r15_opts::to_number_string() const
{
  static const char* options[] = {"0.5", "0.625", "0.75", "0.875"};
  return convert_enum_idx(options, 4, value, "pdsch_cfg_ded_v1530_s::alt_mcs_table_scaling_cfg_r15_e_");
}

std::string pucch_cfg_ded_s::ack_nack_repeat_c_::setup_s_::repeat_factor_opts::to_string() const
{
  static const char* options[] = {"n2", "n4", "n6", "spare1"};
  return convert_enum_idx(options, 4, value, "pucch_cfg_ded_s::ack_nack_repeat_c_::setup_s_::repeat_factor_e_");
}
uint8_t pucch_cfg_ded_s::ack_nack_repeat_c_::setup_s_::repeat_factor_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 6};
  return map_enum_number(options, 3, value, "pucch_cfg_ded_s::ack_nack_repeat_c_::setup_s_::repeat_factor_e_");
}

std::string pucch_cfg_ded_s::tdd_ack_nack_feedback_mode_opts::to_string() const
{
  static const char* options[] = {"bundling", "multiplexing"};
  return convert_enum_idx(options, 2, value, "pucch_cfg_ded_s::tdd_ack_nack_feedback_mode_e_");
}

std::string pucch_cfg_ded_r13_s::ack_nack_repeat_r13_c_::setup_s_::repeat_factor_r13_opts::to_string() const
{
  static const char* options[] = {"n2", "n4", "n6", "spare1"};
  return convert_enum_idx(
      options, 4, value, "pucch_cfg_ded_r13_s::ack_nack_repeat_r13_c_::setup_s_::repeat_factor_r13_e_");
}
uint8_t pucch_cfg_ded_r13_s::ack_nack_repeat_r13_c_::setup_s_::repeat_factor_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 6};
  return map_enum_number(
      options, 3, value, "pucch_cfg_ded_r13_s::ack_nack_repeat_r13_c_::setup_s_::repeat_factor_r13_e_");
}

std::string pucch_cfg_ded_r13_s::tdd_ack_nack_feedback_mode_r13_opts::to_string() const
{
  static const char* options[] = {"bundling", "multiplexing"};
  return convert_enum_idx(options, 2, value, "pucch_cfg_ded_r13_s::tdd_ack_nack_feedback_mode_r13_e_");
}

std::string pucch_cfg_ded_r13_s::pucch_format_r13_c_::types_opts::to_string() const
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

std::string pucch_cfg_ded_r13_s::codebooksize_determination_r13_opts::to_string() const
{
  static const char* options[] = {"dai", "cc"};
  return convert_enum_idx(options, 2, value, "pucch_cfg_ded_r13_s::codebooksize_determination_r13_e_");
}

std::string
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_a_s_::pucch_num_repeat_ce_format1_r13_opts::to_string()
    const
{
  static const char* options[] = {"r1", "r2", "r4", "r8"};
  return convert_enum_idx(
      options,
      4,
      value,
      "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_a_s_::pucch_num_repeat_ce_format1_r13_e_");
}
uint8_t
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_a_s_::pucch_num_repeat_ce_format1_r13_opts::to_number()
    const
{
  static const uint8_t options[] = {1, 2, 4, 8};
  return map_enum_number(
      options,
      4,
      value,
      "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_a_s_::pucch_num_repeat_ce_format1_r13_e_");
}

std::string
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_a_s_::pucch_num_repeat_ce_format2_r13_opts::to_string()
    const
{
  static const char* options[] = {"r1", "r2", "r4", "r8"};
  return convert_enum_idx(
      options,
      4,
      value,
      "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_a_s_::pucch_num_repeat_ce_format2_r13_e_");
}
uint8_t
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_a_s_::pucch_num_repeat_ce_format2_r13_opts::to_number()
    const
{
  static const uint8_t options[] = {1, 2, 4, 8};
  return map_enum_number(
      options,
      4,
      value,
      "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_a_s_::pucch_num_repeat_ce_format2_r13_e_");
}

std::string
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_b_s_::pucch_num_repeat_ce_format1_r13_opts::to_string()
    const
{
  static const char* options[] = {"r4", "r8", "r16", "r32"};
  return convert_enum_idx(
      options,
      4,
      value,
      "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_b_s_::pucch_num_repeat_ce_format1_r13_e_");
}
uint8_t
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_b_s_::pucch_num_repeat_ce_format1_r13_opts::to_number()
    const
{
  static const uint8_t options[] = {4, 8, 16, 32};
  return map_enum_number(
      options,
      4,
      value,
      "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_b_s_::pucch_num_repeat_ce_format1_r13_e_");
}

std::string
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_b_s_::pucch_num_repeat_ce_format2_r13_opts::to_string()
    const
{
  static const char* options[] = {"r4", "r8", "r16", "r32"};
  return convert_enum_idx(
      options,
      4,
      value,
      "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_b_s_::pucch_num_repeat_ce_format2_r13_e_");
}
uint8_t
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_b_s_::pucch_num_repeat_ce_format2_r13_opts::to_number()
    const
{
  static const uint8_t options[] = {4, 8, 16, 32};
  return map_enum_number(
      options,
      4,
      value,
      "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_b_s_::pucch_num_repeat_ce_format2_r13_e_");
}

std::string pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"modeA", "modeB"};
  return convert_enum_idx(options, 2, value, "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::types");
}

std::string pucch_cfg_ded_v1020_s::pucch_format_r10_c_::types_opts::to_string() const
{
  static const char* options[] = {"format3-r10", "channelSelection-r10"};
  return convert_enum_idx(options, 2, value, "pucch_cfg_ded_v1020_s::pucch_format_r10_c_::types");
}
uint8_t pucch_cfg_ded_v1020_s::pucch_format_r10_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {3};
  return map_enum_number(options, 1, value, "pucch_cfg_ded_v1020_s::pucch_format_r10_c_::types");
}

std::string pucch_cfg_ded_v1430_s::pucch_num_repeat_ce_format1_r14_opts::to_string() const
{
  static const char* options[] = {"r64", "r128"};
  return convert_enum_idx(options, 2, value, "pucch_cfg_ded_v1430_s::pucch_num_repeat_ce_format1_r14_e_");
}
uint8_t pucch_cfg_ded_v1430_s::pucch_num_repeat_ce_format1_r14_opts::to_number() const
{
  static const uint8_t options[] = {64, 128};
  return map_enum_number(options, 2, value, "pucch_cfg_ded_v1430_s::pucch_num_repeat_ce_format1_r14_e_");
}

std::string
pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::interv_fdd_pusch_enh_r14_opts::to_string() const
{
  static const char* options[] = {"int1", "int2", "int4", "int8"};
  return convert_enum_idx(
      options,
      4,
      value,
      "pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::interv_fdd_pusch_enh_r14_e_");
}
uint8_t
pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::interv_fdd_pusch_enh_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8};
  return map_enum_number(
      options,
      4,
      value,
      "pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::interv_fdd_pusch_enh_r14_e_");
}

std::string
pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::interv_tdd_pusch_enh_r14_opts::to_string() const
{
  static const char* options[] = {"int1", "int5", "int10", "int20"};
  return convert_enum_idx(
      options,
      4,
      value,
      "pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::interv_tdd_pusch_enh_r14_e_");
}
uint8_t
pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::interv_tdd_pusch_enh_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 5, 10, 20};
  return map_enum_number(
      options,
      4,
      value,
      "pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::interv_tdd_pusch_enh_r14_e_");
}

std::string pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::types_opts::to_string() const
{
  static const char* options[] = {"interval-FDD-PUSCH-Enh-r14", "interval-TDD-PUSCH-Enh-r14"};
  return convert_enum_idx(
      options, 2, value, "pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::types");
}

// PeriodicBSR-Timer-r12 ::= ENUMERATED
std::string periodic_bsr_timer_r12_opts::to_string() const
{
  static const char* options[] = {"sf5",
                                  "sf10",
                                  "sf16",
                                  "sf20",
                                  "sf32",
                                  "sf40",
                                  "sf64",
                                  "sf80",
                                  "sf128",
                                  "sf160",
                                  "sf320",
                                  "sf640",
                                  "sf1280",
                                  "sf2560",
                                  "infinity",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "periodic_bsr_timer_r12_e");
}
int16_t periodic_bsr_timer_r12_opts::to_number() const
{
  static const int16_t options[] = {5, 10, 16, 20, 32, 40, 64, 80, 128, 160, 320, 640, 1280, 2560, -1};
  return map_enum_number(options, 15, value, "periodic_bsr_timer_r12_e");
}

std::string rrc_conn_reject_v1130_ies_s::depriorit_req_r11_s_::depriorit_type_r11_opts::to_string() const
{
  static const char* options[] = {"frequency", "e-utra"};
  return convert_enum_idx(
      options, 2, value, "rrc_conn_reject_v1130_ies_s::depriorit_req_r11_s_::depriorit_type_r11_e_");
}

std::string rrc_conn_reject_v1130_ies_s::depriorit_req_r11_s_::depriorit_timer_r11_opts::to_string() const
{
  static const char* options[] = {"min5", "min10", "min15", "min30"};
  return convert_enum_idx(
      options, 4, value, "rrc_conn_reject_v1130_ies_s::depriorit_req_r11_s_::depriorit_timer_r11_e_");
}
uint8_t rrc_conn_reject_v1130_ies_s::depriorit_req_r11_s_::depriorit_timer_r11_opts::to_number() const
{
  static const uint8_t options[] = {5, 10, 15, 30};
  return map_enum_number(
      options, 4, value, "rrc_conn_reject_v1130_ies_s::depriorit_req_r11_s_::depriorit_timer_r11_e_");
}

// RetxBSR-Timer-r12 ::= ENUMERATED
std::string retx_bsr_timer_r12_opts::to_string() const
{
  static const char* options[] = {"sf320", "sf640", "sf1280", "sf2560", "sf5120", "sf10240", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "retx_bsr_timer_r12_e");
}
uint16_t retx_bsr_timer_r12_opts::to_number() const
{
  static const uint16_t options[] = {320, 640, 1280, 2560, 5120, 10240};
  return map_enum_number(options, 6, value, "retx_bsr_timer_r12_e");
}

std::string sps_cfg_dl_c::setup_s_::semi_persist_sched_interv_dl_opts::to_string() const
{
  static const char* options[] = {"sf10",
                                  "sf20",
                                  "sf32",
                                  "sf40",
                                  "sf64",
                                  "sf80",
                                  "sf128",
                                  "sf160",
                                  "sf320",
                                  "sf640",
                                  "spare6",
                                  "spare5",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "sps_cfg_dl_c::setup_s_::semi_persist_sched_interv_dl_e_");
}
uint16_t sps_cfg_dl_c::setup_s_::semi_persist_sched_interv_dl_opts::to_number() const
{
  static const uint16_t options[] = {10, 20, 32, 40, 64, 80, 128, 160, 320, 640};
  return map_enum_number(options, 10, value, "sps_cfg_dl_c::setup_s_::semi_persist_sched_interv_dl_e_");
}

std::string srb_to_add_mod_s::rlc_cfg_c_::types_opts::to_string() const
{
  static const char* options[] = {"explicitValue", "defaultValue"};
  return convert_enum_idx(options, 2, value, "srb_to_add_mod_s::rlc_cfg_c_::types");
}

std::string srb_to_add_mod_s::lc_ch_cfg_c_::types_opts::to_string() const
{
  static const char* options[] = {"explicitValue", "defaultValue"};
  return convert_enum_idx(options, 2, value, "srb_to_add_mod_s::lc_ch_cfg_c_::types");
}

std::string sched_request_cfg_c::setup_s_::dsr_trans_max_opts::to_string() const
{
  static const char* options[] = {"n4", "n8", "n16", "n32", "n64", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "sched_request_cfg_c::setup_s_::dsr_trans_max_e_");
}
uint8_t sched_request_cfg_c::setup_s_::dsr_trans_max_opts::to_number() const
{
  static const uint8_t options[] = {4, 8, 16, 32, 64};
  return map_enum_number(options, 5, value, "sched_request_cfg_c::setup_s_::dsr_trans_max_e_");
}

std::string srs_ul_cfg_ded_c::setup_s_::srs_bw_opts::to_string() const
{
  static const char* options[] = {"bw0", "bw1", "bw2", "bw3"};
  return convert_enum_idx(options, 4, value, "srs_ul_cfg_ded_c::setup_s_::srs_bw_e_");
}
uint8_t srs_ul_cfg_ded_c::setup_s_::srs_bw_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3};
  return map_enum_number(options, 4, value, "srs_ul_cfg_ded_c::setup_s_::srs_bw_e_");
}

std::string srs_ul_cfg_ded_c::setup_s_::srs_hop_bw_opts::to_string() const
{
  static const char* options[] = {"hbw0", "hbw1", "hbw2", "hbw3"};
  return convert_enum_idx(options, 4, value, "srs_ul_cfg_ded_c::setup_s_::srs_hop_bw_e_");
}
uint8_t srs_ul_cfg_ded_c::setup_s_::srs_hop_bw_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3};
  return map_enum_number(options, 4, value, "srs_ul_cfg_ded_c::setup_s_::srs_hop_bw_e_");
}

std::string srs_ul_cfg_ded_c::setup_s_::cyclic_shift_opts::to_string() const
{
  static const char* options[] = {"cs0", "cs1", "cs2", "cs3", "cs4", "cs5", "cs6", "cs7"};
  return convert_enum_idx(options, 8, value, "srs_ul_cfg_ded_c::setup_s_::cyclic_shift_e_");
}
uint8_t srs_ul_cfg_ded_c::setup_s_::cyclic_shift_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7};
  return map_enum_number(options, 8, value, "srs_ul_cfg_ded_c::setup_s_::cyclic_shift_e_");
}

std::string srs_ul_cfg_ded_v1310_c::setup_s_::cyclic_shift_v1310_opts::to_string() const
{
  static const char* options[] = {"cs8", "cs9", "cs10", "cs11"};
  return convert_enum_idx(options, 4, value, "srs_ul_cfg_ded_v1310_c::setup_s_::cyclic_shift_v1310_e_");
}
uint8_t srs_ul_cfg_ded_v1310_c::setup_s_::cyclic_shift_v1310_opts::to_number() const
{
  static const uint8_t options[] = {8, 9, 10, 11};
  return map_enum_number(options, 4, value, "srs_ul_cfg_ded_v1310_c::setup_s_::cyclic_shift_v1310_e_");
}

std::string srs_ul_cfg_ded_v1310_c::setup_s_::tx_comb_num_r13_opts::to_string() const
{
  static const char* options[] = {"n2", "n4"};
  return convert_enum_idx(options, 2, value, "srs_ul_cfg_ded_v1310_c::setup_s_::tx_comb_num_r13_e_");
}
uint8_t srs_ul_cfg_ded_v1310_c::setup_s_::tx_comb_num_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "srs_ul_cfg_ded_v1310_c::setup_s_::tx_comb_num_r13_e_");
}

std::string srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::setup_s_::srs_up_pts_add_r13_opts::to_string() const
{
  static const char* options[] = {"sym2", "sym4"};
  return convert_enum_idx(
      options, 2, value, "srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::setup_s_::srs_up_pts_add_r13_e_");
}
uint8_t srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::setup_s_::srs_up_pts_add_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(
      options, 2, value, "srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::setup_s_::srs_up_pts_add_r13_e_");
}

std::string srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_up_pts_add_r13_opts::to_string() const
{
  static const char* options[] = {"sym2", "sym4"};
  return convert_enum_idx(options, 2, value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_up_pts_add_r13_e_");
}
uint8_t srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_up_pts_add_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_up_pts_add_r13_e_");
}

std::string srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_bw_r13_opts::to_string() const
{
  static const char* options[] = {"bw0", "bw1", "bw2", "bw3"};
  return convert_enum_idx(options, 4, value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_bw_r13_e_");
}
uint8_t srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_bw_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3};
  return map_enum_number(options, 4, value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_bw_r13_e_");
}

std::string srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_hop_bw_r13_opts::to_string() const
{
  static const char* options[] = {"hbw0", "hbw1", "hbw2", "hbw3"};
  return convert_enum_idx(options, 4, value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_hop_bw_r13_e_");
}
uint8_t srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_hop_bw_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3};
  return map_enum_number(options, 4, value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_hop_bw_r13_e_");
}

std::string srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::cyclic_shift_r13_opts::to_string() const
{
  static const char* options[] = {"cs0", "cs1", "cs2", "cs3", "cs4", "cs5", "cs6", "cs7", "cs8", "cs9", "cs10", "cs11"};
  return convert_enum_idx(options, 12, value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::cyclic_shift_r13_e_");
}
uint8_t srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::cyclic_shift_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  return map_enum_number(options, 12, value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::cyclic_shift_r13_e_");
}

std::string srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::tx_comb_num_r13_opts::to_string() const
{
  static const char* options[] = {"n2", "n4"};
  return convert_enum_idx(options, 2, value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::tx_comb_num_r13_e_");
}
uint8_t srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::tx_comb_num_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::tx_comb_num_r13_e_");
}

std::string ul_pwr_ctrl_ded_s::delta_mcs_enabled_opts::to_string() const
{
  static const char* options[] = {"en0", "en1"};
  return convert_enum_idx(options, 2, value, "ul_pwr_ctrl_ded_s::delta_mcs_enabled_e_");
}
uint8_t ul_pwr_ctrl_ded_s::delta_mcs_enabled_opts::to_number() const
{
  static const uint8_t options[] = {0, 1};
  return map_enum_number(options, 2, value, "ul_pwr_ctrl_ded_s::delta_mcs_enabled_e_");
}

std::string mac_main_cfg_s::ul_sch_cfg_s_::max_harq_tx_opts::to_string() const
{
  static const char* options[] = {
      "n1", "n2", "n3", "n4", "n5", "n6", "n7", "n8", "n10", "n12", "n16", "n20", "n24", "n28", "spare2", "spare1"};
  return convert_enum_idx(options, 16, value, "mac_main_cfg_s::ul_sch_cfg_s_::max_harq_tx_e_");
}
uint8_t mac_main_cfg_s::ul_sch_cfg_s_::max_harq_tx_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 16, 20, 24, 28};
  return map_enum_number(options, 14, value, "mac_main_cfg_s::ul_sch_cfg_s_::max_harq_tx_e_");
}

std::string mac_main_cfg_s::phr_cfg_c_::setup_s_::periodic_phr_timer_opts::to_string() const
{
  static const char* options[] = {"sf10", "sf20", "sf50", "sf100", "sf200", "sf500", "sf1000", "infinity"};
  return convert_enum_idx(options, 8, value, "mac_main_cfg_s::phr_cfg_c_::setup_s_::periodic_phr_timer_e_");
}
int16_t mac_main_cfg_s::phr_cfg_c_::setup_s_::periodic_phr_timer_opts::to_number() const
{
  static const int16_t options[] = {10, 20, 50, 100, 200, 500, 1000, -1};
  return map_enum_number(options, 8, value, "mac_main_cfg_s::phr_cfg_c_::setup_s_::periodic_phr_timer_e_");
}

std::string mac_main_cfg_s::phr_cfg_c_::setup_s_::prohibit_phr_timer_opts::to_string() const
{
  static const char* options[] = {"sf0", "sf10", "sf20", "sf50", "sf100", "sf200", "sf500", "sf1000"};
  return convert_enum_idx(options, 8, value, "mac_main_cfg_s::phr_cfg_c_::setup_s_::prohibit_phr_timer_e_");
}
uint16_t mac_main_cfg_s::phr_cfg_c_::setup_s_::prohibit_phr_timer_opts::to_number() const
{
  static const uint16_t options[] = {0, 10, 20, 50, 100, 200, 500, 1000};
  return map_enum_number(options, 8, value, "mac_main_cfg_s::phr_cfg_c_::setup_s_::prohibit_phr_timer_e_");
}

std::string mac_main_cfg_s::phr_cfg_c_::setup_s_::dl_pathloss_change_opts::to_string() const
{
  static const char* options[] = {"dB1", "dB3", "dB6", "infinity"};
  return convert_enum_idx(options, 4, value, "mac_main_cfg_s::phr_cfg_c_::setup_s_::dl_pathloss_change_e_");
}
int8_t mac_main_cfg_s::phr_cfg_c_::setup_s_::dl_pathloss_change_opts::to_number() const
{
  static const int8_t options[] = {1, 3, 6, -1};
  return map_enum_number(options, 4, value, "mac_main_cfg_s::phr_cfg_c_::setup_s_::dl_pathloss_change_e_");
}

std::string mac_main_cfg_s::mac_main_cfg_v1020_s_::scell_deactivation_timer_r10_opts::to_string() const
{
  static const char* options[] = {"rf2", "rf4", "rf8", "rf16", "rf32", "rf64", "rf128", "spare"};
  return convert_enum_idx(options, 8, value, "mac_main_cfg_s::mac_main_cfg_v1020_s_::scell_deactivation_timer_r10_e_");
}
uint8_t mac_main_cfg_s::mac_main_cfg_v1020_s_::scell_deactivation_timer_r10_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 8, 16, 32, 64, 128};
  return map_enum_number(options, 7, value, "mac_main_cfg_s::mac_main_cfg_v1020_s_::scell_deactivation_timer_r10_e_");
}

std::string mac_main_cfg_s::dual_connect_phr_c_::setup_s_::phr_mode_other_cg_r12_opts::to_string() const
{
  static const char* options[] = {"real", "virtual"};
  return convert_enum_idx(options, 2, value, "mac_main_cfg_s::dual_connect_phr_c_::setup_s_::phr_mode_other_cg_r12_e_");
}

std::string mac_main_cfg_s::lc_ch_sr_cfg_r12_c_::setup_s_::lc_ch_sr_prohibit_timer_r12_opts::to_string() const
{
  static const char* options[] = {"sf20", "sf40", "sf64", "sf128", "sf512", "sf1024", "sf2560", "spare1"};
  return convert_enum_idx(
      options, 8, value, "mac_main_cfg_s::lc_ch_sr_cfg_r12_c_::setup_s_::lc_ch_sr_prohibit_timer_r12_e_");
}
uint16_t mac_main_cfg_s::lc_ch_sr_cfg_r12_c_::setup_s_::lc_ch_sr_prohibit_timer_r12_opts::to_number() const
{
  static const uint16_t options[] = {20, 40, 64, 128, 512, 1024, 2560};
  return map_enum_number(
      options, 7, value, "mac_main_cfg_s::lc_ch_sr_cfg_r12_c_::setup_s_::lc_ch_sr_prohibit_timer_r12_e_");
}

std::string mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"sf5120", "sf10240"};
  return convert_enum_idx(options, 2, value, "mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::setup_c_::types");
}
uint16_t mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::setup_c_::types_opts::to_number() const
{
  static const uint16_t options[] = {5120, 10240};
  return map_enum_number(options, 2, value, "mac_main_cfg_s::edrx_cfg_cycle_start_offset_r13_c_::setup_c_::types");
}

std::string mac_main_cfg_s::short_tti_and_spt_r15_c_::setup_s_::periodic_bsr_timer_r15_opts::to_string() const
{
  static const char* options[] = {"sf1",
                                  "sf5",
                                  "sf10",
                                  "sf16",
                                  "sf20",
                                  "sf32",
                                  "sf40",
                                  "sf64",
                                  "sf80",
                                  "sf128",
                                  "sf160",
                                  "sf320",
                                  "sf640",
                                  "sf1280",
                                  "sf2560",
                                  "infinity"};
  return convert_enum_idx(
      options, 16, value, "mac_main_cfg_s::short_tti_and_spt_r15_c_::setup_s_::periodic_bsr_timer_r15_e_");
}
int16_t mac_main_cfg_s::short_tti_and_spt_r15_c_::setup_s_::periodic_bsr_timer_r15_opts::to_number() const
{
  static const int16_t options[] = {1, 5, 10, 16, 20, 32, 40, 64, 80, 128, 160, 320, 640, 1280, 2560, -1};
  return map_enum_number(
      options, 16, value, "mac_main_cfg_s::short_tti_and_spt_r15_c_::setup_s_::periodic_bsr_timer_r15_e_");
}

std::string mac_main_cfg_s::short_tti_and_spt_r15_c_::setup_s_::proc_timeline_r15_opts::to_string() const
{
  static const char* options[] = {"nplus4set1", "nplus6set1", "nplus6set2", "nplus8set2"};
  return convert_enum_idx(
      options, 4, value, "mac_main_cfg_s::short_tti_and_spt_r15_c_::setup_s_::proc_timeline_r15_e_");
}

std::string mac_main_cfg_s::dormant_state_timers_r15_c_::setup_s_::scell_hibernation_timer_r15_opts::to_string() const
{
  static const char* options[] = {"rf2", "rf4", "rf8", "rf16", "rf32", "rf64", "rf128", "spare"};
  return convert_enum_idx(
      options, 8, value, "mac_main_cfg_s::dormant_state_timers_r15_c_::setup_s_::scell_hibernation_timer_r15_e_");
}
uint8_t mac_main_cfg_s::dormant_state_timers_r15_c_::setup_s_::scell_hibernation_timer_r15_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 8, 16, 32, 64, 128};
  return map_enum_number(
      options, 7, value, "mac_main_cfg_s::dormant_state_timers_r15_c_::setup_s_::scell_hibernation_timer_r15_e_");
}

std::string
mac_main_cfg_s::dormant_state_timers_r15_c_::setup_s_::dormant_scell_deactivation_timer_r15_opts::to_string() const
{
  static const char* options[] = {"rf2",
                                  "rf4",
                                  "rf8",
                                  "rf16",
                                  "rf32",
                                  "rf64",
                                  "rf128",
                                  "rf320",
                                  "rf640",
                                  "rf1280",
                                  "rf2560",
                                  "rf5120",
                                  "rf10240",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(
      options,
      16,
      value,
      "mac_main_cfg_s::dormant_state_timers_r15_c_::setup_s_::dormant_scell_deactivation_timer_r15_e_");
}
uint16_t
mac_main_cfg_s::dormant_state_timers_r15_c_::setup_s_::dormant_scell_deactivation_timer_r15_opts::to_number() const
{
  static const uint16_t options[] = {2, 4, 8, 16, 32, 64, 128, 320, 640, 1280, 2560, 5120, 10240};
  return map_enum_number(
      options,
      13,
      value,
      "mac_main_cfg_s::dormant_state_timers_r15_c_::setup_s_::dormant_scell_deactivation_timer_r15_e_");
}

std::string phys_cfg_ded_s::ant_info_c_::types_opts::to_string() const
{
  static const char* options[] = {"explicitValue", "defaultValue"};
  return convert_enum_idx(options, 2, value, "phys_cfg_ded_s::ant_info_c_::types");
}

std::string phys_cfg_ded_s::ant_info_r10_c_::types_opts::to_string() const
{
  static const char* options[] = {"explicitValue-r10", "defaultValue"};
  return convert_enum_idx(options, 2, value, "phys_cfg_ded_s::ant_info_r10_c_::types");
}

std::string phys_cfg_ded_s::ce_mode_r13_c_::setup_opts::to_string() const
{
  static const char* options[] = {"ce-ModeA", "ce-ModeB"};
  return convert_enum_idx(options, 2, value, "phys_cfg_ded_s::ce_mode_r13_c_::setup_e_");
}

std::string phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::k_max_r14_opts::to_string() const
{
  static const char* options[] = {"l1", "l3"};
  return convert_enum_idx(options, 2, value, "phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::k_max_r14_e_");
}
uint8_t phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::k_max_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 3};
  return map_enum_number(options, 2, value, "phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::k_max_r14_e_");
}

std::string phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_opts::to_string() const
{
  static const char* options[] = {"dB-6", "dB-4dot77", "dB-3", "dB-1dot77", "dB0", "dB1", "dB2", "dB3"};
  return convert_enum_idx(options, 8, value, "phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_e_");
}
float phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_opts::to_number() const
{
  static const float options[] = {-6.0, -4.77, -3.0, -1.77, 0.0, 1.0, 2.0, 3.0};
  return map_enum_number(options, 8, value, "phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_e_");
}
std::string phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_opts::to_number_string() const
{
  static const char* options[] = {"-6", "-4.77", "-3", "-1.77", "0", "1", "2", "3"};
  return convert_enum_idx(options, 8, value, "phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_e_");
}

std::string phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"cfi-Config-r15", "cfi-PatternConfig-r15"};
  return convert_enum_idx(options, 2, value, "phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::setup_c_::types");
}

std::string
phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_sf_pdsch_repeats_r15_opts::to_string() const
{
  static const char* options[] = {"n4", "n6"};
  return convert_enum_idx(
      options, 2, value, "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_sf_pdsch_repeats_r15_e_");
}
uint8_t phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_sf_pdsch_repeats_r15_opts::to_number() const
{
  static const uint8_t options[] = {4, 6};
  return map_enum_number(
      options, 2, value, "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_sf_pdsch_repeats_r15_e_");
}

std::string
phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_slot_subslot_pdsch_repeats_r15_opts::to_string() const
{
  static const char* options[] = {"n4", "n6"};
  return convert_enum_idx(
      options,
      2,
      value,
      "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_slot_subslot_pdsch_repeats_r15_e_");
}
uint8_t
phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_slot_subslot_pdsch_repeats_r15_opts::to_number() const
{
  static const uint8_t options[] = {4, 6};
  return map_enum_number(
      options,
      2,
      value,
      "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_slot_subslot_pdsch_repeats_r15_e_");
}

std::string phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_sf_pdsch_repeats_r15_opts::to_string() const
{
  static const char* options[] = {"dlrvseq1", "dlrvseq2"};
  return convert_enum_idx(
      options, 2, value, "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_sf_pdsch_repeats_r15_e_");
}
uint8_t phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_sf_pdsch_repeats_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(
      options, 2, value, "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_sf_pdsch_repeats_r15_e_");
}

std::string
phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_slotsublot_pdsch_repeats_r15_opts::to_string() const
{
  static const char* options[] = {"dlrvseq1", "dlrvseq2"};
  return convert_enum_idx(
      options, 2, value, "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_slotsublot_pdsch_repeats_r15_e_");
}
uint8_t phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_slotsublot_pdsch_repeats_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(
      options, 2, value, "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_slotsublot_pdsch_repeats_r15_e_");
}

std::string
phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_sf_pdsch_repeats_r15_opts::to_string() const
{
  static const char* options[] = {"n0", "n1"};
  return convert_enum_idx(
      options,
      2,
      value,
      "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_sf_pdsch_repeats_r15_e_");
}
uint8_t
phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_sf_pdsch_repeats_r15_opts::to_number() const
{
  static const uint8_t options[] = {0, 1};
  return map_enum_number(
      options,
      2,
      value,
      "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_sf_pdsch_repeats_r15_e_");
}

std::string
phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_slot_subslot_pdsch_repeats_r15_opts::to_string()
    const
{
  static const char* options[] = {"n0", "n1"};
  return convert_enum_idx(
      options,
      2,
      value,
      "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_slot_subslot_pdsch_repeats_r15_e_");
}
uint8_t
phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_slot_subslot_pdsch_repeats_r15_opts::to_number()
    const
{
  static const uint8_t options[] = {0, 1};
  return map_enum_number(
      options,
      2,
      value,
      "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_slot_subslot_pdsch_repeats_r15_e_");
}

std::string rlf_timers_and_consts_r13_c::setup_s_::t301_v1310_opts::to_string() const
{
  static const char* options[] = {"ms2500", "ms3000", "ms3500", "ms4000", "ms5000", "ms6000", "ms8000", "ms10000"};
  return convert_enum_idx(options, 8, value, "rlf_timers_and_consts_r13_c::setup_s_::t301_v1310_e_");
}
uint16_t rlf_timers_and_consts_r13_c::setup_s_::t301_v1310_opts::to_number() const
{
  static const uint16_t options[] = {2500, 3000, 3500, 4000, 5000, 6000, 8000, 10000};
  return map_enum_number(options, 8, value, "rlf_timers_and_consts_r13_c::setup_s_::t301_v1310_e_");
}

std::string rlf_timers_and_consts_r13_c::setup_s_::t310_v1330_opts::to_string() const
{
  static const char* options[] = {"ms4000", "ms6000"};
  return convert_enum_idx(options, 2, value, "rlf_timers_and_consts_r13_c::setup_s_::t310_v1330_e_");
}
uint16_t rlf_timers_and_consts_r13_c::setup_s_::t310_v1330_opts::to_number() const
{
  static const uint16_t options[] = {4000, 6000};
  return map_enum_number(options, 2, value, "rlf_timers_and_consts_r13_c::setup_s_::t310_v1330_e_");
}

std::string rlf_timers_and_consts_r9_c::setup_s_::t301_r9_opts::to_string() const
{
  static const char* options[] = {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
  return convert_enum_idx(options, 8, value, "rlf_timers_and_consts_r9_c::setup_s_::t301_r9_e_");
}
uint16_t rlf_timers_and_consts_r9_c::setup_s_::t301_r9_opts::to_number() const
{
  static const uint16_t options[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(options, 8, value, "rlf_timers_and_consts_r9_c::setup_s_::t301_r9_e_");
}

std::string rlf_timers_and_consts_r9_c::setup_s_::t310_r9_opts::to_string() const
{
  static const char* options[] = {"ms0", "ms50", "ms100", "ms200", "ms500", "ms1000", "ms2000"};
  return convert_enum_idx(options, 7, value, "rlf_timers_and_consts_r9_c::setup_s_::t310_r9_e_");
}
uint16_t rlf_timers_and_consts_r9_c::setup_s_::t310_r9_opts::to_number() const
{
  static const uint16_t options[] = {0, 50, 100, 200, 500, 1000, 2000};
  return map_enum_number(options, 7, value, "rlf_timers_and_consts_r9_c::setup_s_::t310_r9_e_");
}

std::string rlf_timers_and_consts_r9_c::setup_s_::n310_r9_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4", "n6", "n8", "n10", "n20"};
  return convert_enum_idx(options, 8, value, "rlf_timers_and_consts_r9_c::setup_s_::n310_r9_e_");
}
uint8_t rlf_timers_and_consts_r9_c::setup_s_::n310_r9_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 6, 8, 10, 20};
  return map_enum_number(options, 8, value, "rlf_timers_and_consts_r9_c::setup_s_::n310_r9_e_");
}

std::string rlf_timers_and_consts_r9_c::setup_s_::t311_r9_opts::to_string() const
{
  static const char* options[] = {"ms1000", "ms3000", "ms5000", "ms10000", "ms15000", "ms20000", "ms30000"};
  return convert_enum_idx(options, 7, value, "rlf_timers_and_consts_r9_c::setup_s_::t311_r9_e_");
}
uint16_t rlf_timers_and_consts_r9_c::setup_s_::t311_r9_opts::to_number() const
{
  static const uint16_t options[] = {1000, 3000, 5000, 10000, 15000, 20000, 30000};
  return map_enum_number(options, 7, value, "rlf_timers_and_consts_r9_c::setup_s_::t311_r9_e_");
}

std::string rlf_timers_and_consts_r9_c::setup_s_::n311_r9_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4", "n5", "n6", "n8", "n10"};
  return convert_enum_idx(options, 8, value, "rlf_timers_and_consts_r9_c::setup_s_::n311_r9_e_");
}
uint8_t rlf_timers_and_consts_r9_c::setup_s_::n311_r9_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5, 6, 8, 10};
  return map_enum_number(options, 8, value, "rlf_timers_and_consts_r9_c::setup_s_::n311_r9_e_");
}

std::string idle_mode_mob_ctrl_info_s::t320_opts::to_string() const
{
  static const char* options[] = {"min5", "min10", "min20", "min30", "min60", "min120", "min180", "spare1"};
  return convert_enum_idx(options, 8, value, "idle_mode_mob_ctrl_info_s::t320_e_");
}
uint8_t idle_mode_mob_ctrl_info_s::t320_opts::to_number() const
{
  static const uint8_t options[] = {5, 10, 20, 30, 60, 120, 180};
  return map_enum_number(options, 7, value, "idle_mode_mob_ctrl_info_s::t320_e_");
}

std::string rr_cfg_ded_s::mac_main_cfg_c_::types_opts::to_string() const
{
  static const char* options[] = {"explicitValue", "defaultValue"};
  return convert_enum_idx(options, 2, value, "rr_cfg_ded_s::mac_main_cfg_c_::types");
}

std::string rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::crs_intf_mitig_num_prbs_r15_opts::to_string() const
{
  static const char* options[] = {"n6", "n24"};
  return convert_enum_idx(
      options, 2, value, "rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::crs_intf_mitig_num_prbs_r15_e_");
}
uint8_t rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::crs_intf_mitig_num_prbs_r15_opts::to_number() const
{
  static const uint8_t options[] = {6, 24};
  return map_enum_number(
      options, 2, value, "rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::crs_intf_mitig_num_prbs_r15_e_");
}

std::string rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"crs-IntfMitigEnabled-15", "crs-IntfMitigNumPRBs-r15"};
  return convert_enum_idx(options, 2, value, "rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::types");
}
int8_t rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::types_opts::to_number() const
{
  static const int8_t options[] = {-15};
  return map_enum_number(options, 1, value, "rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::types");
}

std::string redirected_carrier_info_r15_ies_c::types_opts::to_string() const
{
  static const char* options[] = {
      "eutra-r15", "geran-r15", "utra-FDD-r15", "cdma2000-HRPD-r15", "cdma2000-1xRTT-r15", "utra-TDD-r15"};
  return convert_enum_idx(options, 6, value, "redirected_carrier_info_r15_ies_c::types");
}

std::string c1_or_crit_ext_opts::to_string() const
{
  static const char* options[] = {"c1", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "c1_or_crit_ext_e");
}
uint8_t c1_or_crit_ext_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "c1_or_crit_ext_e");
}

std::string rrc_conn_reest_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "rrcConnectionReestablishment-r8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "rrc_conn_reest_s::crit_exts_c_::c1_c_::types");
}

std::string rrc_conn_reest_reject_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionReestablishmentReject-r8", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_conn_reest_reject_s::crit_exts_c_::types");
}

std::string rrc_conn_reject_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionReject-r8", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "rrc_conn_reject_s::crit_exts_c_::c1_c_::types");
}

std::string rrc_conn_setup_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "rrcConnectionSetup-r8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "rrc_conn_setup_s::crit_exts_c_::c1_c_::types");
}

std::string rrc_early_data_complete_r15_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcEarlyDataComplete-r15", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_early_data_complete_r15_s::crit_exts_c_::types");
}

std::string dl_ccch_msg_type_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionReestablishment",
                                  "rrcConnectionReestablishmentReject",
                                  "rrcConnectionReject",
                                  "rrcConnectionSetup"};
  return convert_enum_idx(options, 4, value, "dl_ccch_msg_type_c::c1_c_::types");
}

std::string dl_ccch_msg_type_c::msg_class_ext_c_::c2_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcEarlyDataComplete-r15", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "dl_ccch_msg_type_c::msg_class_ext_c_::c2_c_::types");
}

std::string dl_ccch_msg_type_c::msg_class_ext_c_::types_opts::to_string() const
{
  static const char* options[] = {"c2", "messageClassExtensionFuture-r15"};
  return convert_enum_idx(options, 2, value, "dl_ccch_msg_type_c::msg_class_ext_c_::types");
}
uint8_t dl_ccch_msg_type_c::msg_class_ext_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {2};
  return map_enum_number(options, 1, value, "dl_ccch_msg_type_c::msg_class_ext_c_::types");
}

std::string dl_ccch_msg_type_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "dl_ccch_msg_type_c::types");
}
uint8_t dl_ccch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "dl_ccch_msg_type_c::types");
}

// PDCCH-CandidateReductionValue-r14 ::= ENUMERATED
std::string pdcch_candidate_reduction_value_r14_opts::to_string() const
{
  static const char* options[] = {"n0", "n50", "n100", "n150"};
  return convert_enum_idx(options, 4, value, "pdcch_candidate_reduction_value_r14_e");
}
uint8_t pdcch_candidate_reduction_value_r14_opts::to_number() const
{
  static const uint8_t options[] = {0, 50, 100, 150};
  return map_enum_number(options, 4, value, "pdcch_candidate_reduction_value_r14_e");
}

std::string aul_cfg_r15_c::setup_s_::tx_mode_ul_aul_r15_opts::to_string() const
{
  static const char* options[] = {"tm1", "tm2"};
  return convert_enum_idx(options, 2, value, "aul_cfg_r15_c::setup_s_::tx_mode_ul_aul_r15_e_");
}
uint8_t aul_cfg_r15_c::setup_s_::tx_mode_ul_aul_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "aul_cfg_r15_c::setup_s_::tx_mode_ul_aul_r15_e_");
}

std::string aul_cfg_r15_c::setup_s_::aul_start_partial_bw_inside_mcot_r15_opts::to_string() const
{
  static const char* options[] = {"o34", "o43", "o52", "o61", "oOS1"};
  return convert_enum_idx(options, 5, value, "aul_cfg_r15_c::setup_s_::aul_start_partial_bw_inside_mcot_r15_e_");
}
uint8_t aul_cfg_r15_c::setup_s_::aul_start_partial_bw_inside_mcot_r15_opts::to_number() const
{
  static const uint8_t options[] = {34, 43, 52, 61, 1};
  return map_enum_number(options, 5, value, "aul_cfg_r15_c::setup_s_::aul_start_partial_bw_inside_mcot_r15_e_");
}

std::string aul_cfg_r15_c::setup_s_::aul_start_partial_bw_outside_mcot_r15_opts::to_string() const
{
  static const char* options[] = {"o16", "o25", "o34", "o43", "o52", "o61", "oOS1"};
  return convert_enum_idx(options, 7, value, "aul_cfg_r15_c::setup_s_::aul_start_partial_bw_outside_mcot_r15_e_");
}
uint8_t aul_cfg_r15_c::setup_s_::aul_start_partial_bw_outside_mcot_r15_opts::to_number() const
{
  static const uint8_t options[] = {16, 25, 34, 43, 52, 61, 1};
  return map_enum_number(options, 7, value, "aul_cfg_r15_c::setup_s_::aul_start_partial_bw_outside_mcot_r15_e_");
}

std::string aul_cfg_r15_c::setup_s_::aul_retx_timer_r15_opts::to_string() const
{
  static const char* options[] = {"psf4",
                                  "psf5",
                                  "psf6",
                                  "psf8",
                                  "psf10",
                                  "psf12",
                                  "psf20",
                                  "psf28",
                                  "psf37",
                                  "psf44",
                                  "psf68",
                                  "psf84",
                                  "psf100",
                                  "psf116",
                                  "psf132",
                                  "psf164",
                                  "psf324"};
  return convert_enum_idx(options, 17, value, "aul_cfg_r15_c::setup_s_::aul_retx_timer_r15_e_");
}
uint16_t aul_cfg_r15_c::setup_s_::aul_retx_timer_r15_opts::to_number() const
{
  static const uint16_t options[] = {4, 5, 6, 8, 10, 12, 20, 28, 37, 44, 68, 84, 100, 116, 132, 164, 324};
  return map_enum_number(options, 17, value, "aul_cfg_r15_c::setup_s_::aul_retx_timer_r15_e_");
}

std::string aul_cfg_r15_c::setup_s_::contention_win_size_timer_r15_opts::to_string() const
{
  static const char* options[] = {"n0", "n5", "n10"};
  return convert_enum_idx(options, 3, value, "aul_cfg_r15_c::setup_s_::contention_win_size_timer_r15_e_");
}
uint8_t aul_cfg_r15_c::setup_s_::contention_win_size_timer_r15_opts::to_number() const
{
  static const uint8_t options[] = {0, 5, 10};
  return map_enum_number(options, 3, value, "aul_cfg_r15_c::setup_s_::contention_win_size_timer_r15_e_");
}

std::string cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::wideband_cqi_r15_s_::
    csi_report_mode_r15_opts::to_string() const
{
  static const char* options[] = {"submode1", "submode2"};
  return convert_enum_idx(options,
                          2,
                          value,
                          "cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::wideband_cqi_r15_"
                          "s_::csi_report_mode_r15_e_");
}
uint8_t cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::wideband_cqi_r15_s_::
    csi_report_mode_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options,
                         2,
                         value,
                         "cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::wideband_cqi_r15_s_"
                         "::csi_report_mode_r15_e_");
}

std::string cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::subband_cqi_r15_s_::
    periodicity_factor_r15_opts::to_string() const
{
  static const char* options[] = {"n2", "n4"};
  return convert_enum_idx(options,
                          2,
                          value,
                          "cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::subband_cqi_r15_s_"
                          "::periodicity_factor_r15_e_");
}
uint8_t cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::subband_cqi_r15_s_::
    periodicity_factor_r15_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options,
                         2,
                         value,
                         "cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::subband_cqi_r15_s_:"
                         ":periodicity_factor_r15_e_");
}

std::string cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"widebandCQI-r15", "subbandCQI-r15"};
  return convert_enum_idx(
      options, 2, value, "cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::types");
}

std::string lbt_cfg_r14_c::types_opts::to_string() const
{
  static const char* options[] = {"maxEnergyDetectionThreshold-r14", "energyDetectionThresholdOffset-r14"};
  return convert_enum_idx(options, 2, value, "lbt_cfg_r14_c::types");
}

std::string pdcch_cfg_laa_r14_s::max_nof_sched_sfs_format0_b_r14_opts::to_string() const
{
  static const char* options[] = {"sf2", "sf3", "sf4"};
  return convert_enum_idx(options, 3, value, "pdcch_cfg_laa_r14_s::max_nof_sched_sfs_format0_b_r14_e_");
}
uint8_t pdcch_cfg_laa_r14_s::max_nof_sched_sfs_format0_b_r14_opts::to_number() const
{
  static const uint8_t options[] = {2, 3, 4};
  return map_enum_number(options, 3, value, "pdcch_cfg_laa_r14_s::max_nof_sched_sfs_format0_b_r14_e_");
}

std::string pdcch_cfg_laa_r14_s::max_nof_sched_sfs_format4_b_r14_opts::to_string() const
{
  static const char* options[] = {"sf2", "sf3", "sf4"};
  return convert_enum_idx(options, 3, value, "pdcch_cfg_laa_r14_s::max_nof_sched_sfs_format4_b_r14_e_");
}
uint8_t pdcch_cfg_laa_r14_s::max_nof_sched_sfs_format4_b_r14_opts::to_number() const
{
  static const uint8_t options[] = {2, 3, 4};
  return map_enum_number(options, 3, value, "pdcch_cfg_laa_r14_s::max_nof_sched_sfs_format4_b_r14_e_");
}

std::string cqi_report_cfg_scell_r15_s::alt_cqi_table_minus1024_qam_r15_opts::to_string() const
{
  static const char* options[] = {"allSubframes", "csi-SubframeSet1", "csi-SubframeSet2", "spare1"};
  return convert_enum_idx(options, 4, value, "cqi_report_cfg_scell_r15_s::alt_cqi_table_minus1024_qam_r15_e_");
}
uint8_t cqi_report_cfg_scell_r15_s::alt_cqi_table_minus1024_qam_r15_opts::to_number() const
{
  switch (value) {
    case csi_sf_set1:
      return 1;
    case csi_sf_set2:
      return 2;
    default:
      invalid_enum_number(value, "cqi_report_cfg_scell_r15_s::alt_cqi_table_minus1024_qam_r15_e_");
  }
  return 0;
}

std::string cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::wideband_cqi_short_r15_s_::
    csi_report_mode_short_r15_opts::to_string() const
{
  static const char* options[] = {"submode1", "submode2"};
  return convert_enum_idx(options,
                          2,
                          value,
                          "cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::wideband_cqi_short_r15_s_:"
                          ":csi_report_mode_short_r15_e_");
}
uint8_t cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::wideband_cqi_short_r15_s_::
    csi_report_mode_short_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options,
                         2,
                         value,
                         "cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::wideband_cqi_short_r15_s_::"
                         "csi_report_mode_short_r15_e_");
}

std::string cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::subband_cqi_short_r15_s_::
    periodicity_factor_r15_opts::to_string() const
{
  static const char* options[] = {"n2", "n4"};
  return convert_enum_idx(options,
                          2,
                          value,
                          "cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::subband_cqi_short_r15_s_::"
                          "periodicity_factor_r15_e_");
}
uint8_t cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::subband_cqi_short_r15_s_::
    periodicity_factor_r15_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options,
                         2,
                         value,
                         "cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::subband_cqi_short_r15_s_::"
                         "periodicity_factor_r15_e_");
}

std::string cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"widebandCQI-Short-r15", "subbandCQI-Short-r15"};
  return convert_enum_idx(options, 2, value, "cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::types");
}

std::string cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_::types_opts::to_string() const
{
  static const char* options[] = {"own-r10", "other-r10"};
  return convert_enum_idx(options, 2, value, "cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_::types");
}

std::string cross_carrier_sched_cfg_r13_s::sched_cell_info_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"own-r13", "other-r13"};
  return convert_enum_idx(options, 2, value, "cross_carrier_sched_cfg_r13_s::sched_cell_info_r13_c_::types");
}

std::string delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1_r15_opts::to_string() const
{
  static const char* options[] = {
      "deltaF-1", "deltaF0", "deltaF1", "deltaF2", "deltaF3", "deltaF4", "deltaF5", "deltaF6"};
  return convert_enum_idx(options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1_r15_e_");
}
int8_t delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1_r15_opts::to_number() const
{
  static const int8_t options[] = {-1, 0, 1, 2, 3, 4, 5, 6};
  return map_enum_number(options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1_r15_e_");
}

std::string delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1a_r15_opts::to_string() const
{
  static const char* options[] = {
      "deltaF1", "deltaF2", "deltaF3", "deltaF4", "deltaF5", "deltaF6", "deltaF7", "deltaF8"};
  return convert_enum_idx(options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1a_r15_e_");
}
uint8_t delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1a_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5, 6, 7, 8};
  return map_enum_number(options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1a_r15_e_");
}

std::string delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1b_r15_opts::to_string() const
{
  static const char* options[] = {
      "deltaF3", "deltaF4", "deltaF5", "deltaF6", "deltaF7", "deltaF8", "deltaF9", "deltaF10"};
  return convert_enum_idx(options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1b_r15_e_");
}
uint8_t delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1b_r15_opts::to_number() const
{
  static const uint8_t options[] = {3, 4, 5, 6, 7, 8, 9, 10};
  return map_enum_number(options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1b_r15_e_");
}

std::string delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format3_r15_opts::to_string() const
{
  static const char* options[] = {
      "deltaF4", "deltaF5", "deltaF6", "deltaF7", "deltaF8", "deltaF9", "deltaF10", "deltaF11"};
  return convert_enum_idx(options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format3_r15_e_");
}
uint8_t delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format3_r15_opts::to_number() const
{
  static const uint8_t options[] = {4, 5, 6, 7, 8, 9, 10, 11};
  return map_enum_number(options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format3_r15_e_");
}

std::string delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_rm_format4_r15_opts::to_string() const
{
  static const char* options[] = {
      "deltaF13", "deltaF14", "deltaF15", "deltaF16", "deltaF17", "deltaF18", "deltaF19", "deltaF20"};
  return convert_enum_idx(
      options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_rm_format4_r15_e_");
}
uint8_t delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_rm_format4_r15_opts::to_number() const
{
  static const uint8_t options[] = {13, 14, 15, 16, 17, 18, 19, 20};
  return map_enum_number(
      options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_rm_format4_r15_e_");
}

std::string delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_tbcc_format4_r15_opts::to_string() const
{
  static const char* options[] = {
      "deltaF10", "deltaF11", "deltaF12", "deltaF13", "deltaF14", "deltaF15", "deltaF16", "deltaF17"};
  return convert_enum_idx(
      options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_tbcc_format4_r15_e_");
}
uint8_t delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_tbcc_format4_r15_opts::to_number() const
{
  static const uint8_t options[] = {10, 11, 12, 13, 14, 15, 16, 17};
  return map_enum_number(
      options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_tbcc_format4_r15_e_");
}

std::string delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_format1and1a_r15_opts::to_string() const
{
  static const char* options[] = {
      "deltaF5", "deltaF6", "deltaF7", "deltaF8", "deltaF9", "deltaF10", "deltaF11", "deltaF12"};
  return convert_enum_idx(
      options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_format1and1a_r15_e_");
}
uint8_t delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_format1and1a_r15_opts::to_number() const
{
  static const uint8_t options[] = {5, 6, 7, 8, 9, 10, 11, 12};
  return map_enum_number(
      options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_format1and1a_r15_e_");
}

std::string delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_format1b_r15_opts::to_string() const
{
  static const char* options[] = {
      "deltaF6", "deltaF7", "deltaF8", "deltaF9", "deltaF10", "deltaF11", "deltaF12", "deltaF13"};
  return convert_enum_idx(
      options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_format1b_r15_e_");
}
uint8_t delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_format1b_r15_opts::to_number() const
{
  static const uint8_t options[] = {6, 7, 8, 9, 10, 11, 12, 13};
  return map_enum_number(
      options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_format1b_r15_e_");
}

std::string delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_rm_format4_r15_opts::to_string() const
{
  static const char* options[] = {
      "deltaF15", "deltaF16", "deltaF17", "deltaF18", "deltaF19", "deltaF20", "deltaF21", "deltaF22"};
  return convert_enum_idx(
      options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_rm_format4_r15_e_");
}
uint8_t delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_rm_format4_r15_opts::to_number() const
{
  static const uint8_t options[] = {15, 16, 17, 18, 19, 20, 21, 22};
  return map_enum_number(
      options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_rm_format4_r15_e_");
}

std::string delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_tbcc_format4_r15_opts::to_string() const
{
  static const char* options[] = {
      "deltaF10", "deltaF11", "deltaF12", "deltaF13", "deltaF14", "deltaF15", "deltaF16", "deltaF17"};
  return convert_enum_idx(
      options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_tbcc_format4_r15_e_");
}
uint8_t delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_tbcc_format4_r15_opts::to_number() const
{
  static const uint8_t options[] = {10, 11, 12, 13, 14, 15, 16, 17};
  return map_enum_number(
      options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_tbcc_format4_r15_e_");
}

std::string laa_scell_cfg_r13_s::sf_start_position_r13_opts::to_string() const
{
  static const char* options[] = {"s0", "s07"};
  return convert_enum_idx(options, 2, value, "laa_scell_cfg_r13_s::sf_start_position_r13_e_");
}
float laa_scell_cfg_r13_s::sf_start_position_r13_opts::to_number() const
{
  static const float options[] = {0.0, 0.7};
  return map_enum_number(options, 2, value, "laa_scell_cfg_r13_s::sf_start_position_r13_e_");
}
std::string laa_scell_cfg_r13_s::sf_start_position_r13_opts::to_number_string() const
{
  static const char* options[] = {"0", "0.7"};
  return convert_enum_idx(options, 2, value, "laa_scell_cfg_r13_s::sf_start_position_r13_e_");
}

std::string sched_request_cfg_scell_r13_c::setup_s_::dsr_trans_max_r13_opts::to_string() const
{
  static const char* options[] = {"n4", "n8", "n16", "n32", "n64", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "sched_request_cfg_scell_r13_c::setup_s_::dsr_trans_max_r13_e_");
}
uint8_t sched_request_cfg_scell_r13_c::setup_s_::dsr_trans_max_r13_opts::to_number() const
{
  static const uint8_t options[] = {4, 8, 16, 32, 64};
  return map_enum_number(options, 5, value, "sched_request_cfg_scell_r13_c::setup_s_::dsr_trans_max_r13_e_");
}

std::string ul_pwr_ctrl_ded_scell_r10_s::delta_mcs_enabled_r10_opts::to_string() const
{
  static const char* options[] = {"en0", "en1"};
  return convert_enum_idx(options, 2, value, "ul_pwr_ctrl_ded_scell_r10_s::delta_mcs_enabled_r10_e_");
}
uint8_t ul_pwr_ctrl_ded_scell_r10_s::delta_mcs_enabled_r10_opts::to_number() const
{
  static const uint8_t options[] = {0, 1};
  return map_enum_number(options, 2, value, "ul_pwr_ctrl_ded_scell_r10_s::delta_mcs_enabled_r10_e_");
}

std::string ul_pwr_ctrl_ded_scell_r10_s::pathloss_ref_linking_r10_opts::to_string() const
{
  static const char* options[] = {"pCell", "sCell"};
  return convert_enum_idx(options, 2, value, "ul_pwr_ctrl_ded_scell_r10_s::pathloss_ref_linking_r10_e_");
}

std::string ant_info_common_s::ant_ports_count_opts::to_string() const
{
  static const char* options[] = {"an1", "an2", "an4", "spare1"};
  return convert_enum_idx(options, 4, value, "ant_info_common_s::ant_ports_count_e_");
}
uint8_t ant_info_common_s::ant_ports_count_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4};
  return map_enum_number(options, 3, value, "ant_info_common_s::ant_ports_count_e_");
}

std::string phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::k_max_r14_opts::to_string() const
{
  static const char* options[] = {"l1", "l3"};
  return convert_enum_idx(options, 2, value, "phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::k_max_r14_e_");
}
uint8_t phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::k_max_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 3};
  return map_enum_number(options, 2, value, "phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::k_max_r14_e_");
}

std::string phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_opts::to_string() const
{
  static const char* options[] = {"dB-6", "dB-4dot77", "dB-3", "dB-1dot77", "dB0", "dB1", "dB2", "dB3"};
  return convert_enum_idx(options, 8, value, "phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_e_");
}
float phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_opts::to_number() const
{
  static const float options[] = {-6.0, -4.77, -3.0, -1.77, 0.0, 1.0, 2.0, 3.0};
  return map_enum_number(options, 8, value, "phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_e_");
}
std::string phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_opts::to_number_string() const
{
  static const char* options[] = {"-6", "-4.77", "-3", "-1.77", "0", "1", "2", "3"};
  return convert_enum_idx(options, 8, value, "phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_e_");
}

std::string phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"cfi-Config-r15", "cfi-PatternConfig-r15"};
  return convert_enum_idx(options, 2, value, "phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::setup_c_::types");
}

std::string
phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_sf_pdsch_repeats_r15_opts::to_string() const
{
  static const char* options[] = {"n4", "n6"};
  return convert_enum_idx(
      options,
      2,
      value,
      "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_sf_pdsch_repeats_r15_e_");
}
uint8_t
phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_sf_pdsch_repeats_r15_opts::to_number() const
{
  static const uint8_t options[] = {4, 6};
  return map_enum_number(
      options,
      2,
      value,
      "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_sf_pdsch_repeats_r15_e_");
}

std::string phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::
    max_num_slot_subslot_pdsch_repeats_r15_opts::to_string() const
{
  static const char* options[] = {"n4", "n6"};
  return convert_enum_idx(
      options,
      2,
      value,
      "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_slot_subslot_pdsch_repeats_r15_e_");
}
uint8_t phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_slot_subslot_pdsch_repeats_r15_opts::
    to_number() const
{
  static const uint8_t options[] = {4, 6};
  return map_enum_number(
      options,
      2,
      value,
      "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_slot_subslot_pdsch_repeats_r15_e_");
}

std::string
phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_sf_pdsch_repeats_r15_opts::to_string() const
{
  static const char* options[] = {"dlrvseq1", "dlrvseq2"};
  return convert_enum_idx(
      options,
      2,
      value,
      "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_sf_pdsch_repeats_r15_e_");
}
uint8_t
phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_sf_pdsch_repeats_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(
      options,
      2,
      value,
      "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_sf_pdsch_repeats_r15_e_");
}

std::string
phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_slotsublot_pdsch_repeats_r15_opts::to_string()
    const
{
  static const char* options[] = {"dlrvseq1", "dlrvseq2"};
  return convert_enum_idx(
      options,
      2,
      value,
      "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_slotsublot_pdsch_repeats_r15_e_");
}
uint8_t
phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_slotsublot_pdsch_repeats_r15_opts::to_number()
    const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(
      options,
      2,
      value,
      "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_slotsublot_pdsch_repeats_r15_e_");
}

std::string
phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_sf_pdsch_repeats_r15_opts::to_string()
    const
{
  static const char* options[] = {"n0", "n1"};
  return convert_enum_idx(
      options,
      2,
      value,
      "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_sf_pdsch_repeats_r15_e_");
}
uint8_t
phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_sf_pdsch_repeats_r15_opts::to_number()
    const
{
  static const uint8_t options[] = {0, 1};
  return map_enum_number(
      options,
      2,
      value,
      "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_sf_pdsch_repeats_r15_e_");
}

std::string phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::
    mcs_restrict_slot_subslot_pdsch_repeats_r15_opts::to_string() const
{
  static const char* options[] = {"n0", "n1"};
  return convert_enum_idx(options,
                          2,
                          value,
                          "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_slot_"
                          "subslot_pdsch_repeats_r15_e_");
}
uint8_t phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::
    mcs_restrict_slot_subslot_pdsch_repeats_r15_opts::to_number() const
{
  static const uint8_t options[] = {0, 1};
  return map_enum_number(options,
                         2,
                         value,
                         "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_slot_subslot_"
                         "pdsch_repeats_r15_e_");
}

std::string ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format3_r12_opts::to_string() const
{
  static const char* options[] = {
      "deltaF-1", "deltaF0", "deltaF1", "deltaF2", "deltaF3", "deltaF4", "deltaF5", "deltaF6"};
  return convert_enum_idx(options, 8, value, "ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format3_r12_e_");
}
int8_t ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format3_r12_opts::to_number() const
{
  static const int8_t options[] = {-1, 0, 1, 2, 3, 4, 5, 6};
  return map_enum_number(options, 8, value, "ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format3_r12_e_");
}

std::string ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format1b_cs_r12_opts::to_string() const
{
  static const char* options[] = {"deltaF1", "deltaF2", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format1b_cs_r12_e_");
}
uint8_t ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format1b_cs_r12_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format1b_cs_r12_e_");
}

std::string ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format4_r13_opts::to_string() const
{
  static const char* options[] = {
      "deltaF16", "deltaF15", "deltaF14", "deltaF13", "deltaF12", "deltaF11", "deltaF10", "spare1"};
  return convert_enum_idx(options, 8, value, "ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format4_r13_e_");
}
uint8_t ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format4_r13_opts::to_number() const
{
  static const uint8_t options[] = {16, 15, 14, 13, 12, 11, 10};
  return map_enum_number(options, 7, value, "ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format4_r13_e_");
}

std::string ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format5_minus13_opts::to_string() const
{
  static const char* options[] = {
      "deltaF13", "deltaF12", "deltaF11", "deltaF10", "deltaF9", "deltaF8", "deltaF7", "spare1"};
  return convert_enum_idx(options, 8, value, "ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format5_minus13_e_");
}
uint8_t ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format5_minus13_opts::to_number() const
{
  static const uint8_t options[] = {13, 12, 11, 10, 9, 8, 7};
  return map_enum_number(options, 7, value, "ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format5_minus13_e_");
}

std::string ant_info_ded_v10i0_s::max_layers_mimo_r10_opts::to_string() const
{
  static const char* options[] = {"twoLayers", "fourLayers", "eightLayers"};
  return convert_enum_idx(options, 3, value, "ant_info_ded_v10i0_s::max_layers_mimo_r10_e_");
}
uint8_t ant_info_ded_v10i0_s::max_layers_mimo_r10_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 8};
  return map_enum_number(options, 3, value, "ant_info_ded_v10i0_s::max_layers_mimo_r10_e_");
}

std::string rr_cfg_common_scell_r10_s::non_ul_cfg_r10_s_::dl_bw_r10_opts::to_string() const
{
  static const char* options[] = {"n6", "n15", "n25", "n50", "n75", "n100"};
  return convert_enum_idx(options, 6, value, "rr_cfg_common_scell_r10_s::non_ul_cfg_r10_s_::dl_bw_r10_e_");
}
uint8_t rr_cfg_common_scell_r10_s::non_ul_cfg_r10_s_::dl_bw_r10_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(options, 6, value, "rr_cfg_common_scell_r10_s::non_ul_cfg_r10_s_::dl_bw_r10_e_");
}

std::string rr_cfg_common_scell_r10_s::ul_cfg_r10_s_::ul_freq_info_r10_s_::ul_bw_r10_opts::to_string() const
{
  static const char* options[] = {"n6", "n15", "n25", "n50", "n75", "n100"};
  return convert_enum_idx(
      options, 6, value, "rr_cfg_common_scell_r10_s::ul_cfg_r10_s_::ul_freq_info_r10_s_::ul_bw_r10_e_");
}
uint8_t rr_cfg_common_scell_r10_s::ul_cfg_r10_s_::ul_freq_info_r10_s_::ul_bw_r10_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(
      options, 6, value, "rr_cfg_common_scell_r10_s::ul_cfg_r10_s_::ul_freq_info_r10_s_::ul_bw_r10_e_");
}

std::string rr_cfg_common_scell_r10_s::ul_cfg_r14_s_::ul_freq_info_r14_s_::ul_bw_r14_opts::to_string() const
{
  static const char* options[] = {"n6", "n15", "n25", "n50", "n75", "n100"};
  return convert_enum_idx(
      options, 6, value, "rr_cfg_common_scell_r10_s::ul_cfg_r14_s_::ul_freq_info_r14_s_::ul_bw_r14_e_");
}
uint8_t rr_cfg_common_scell_r10_s::ul_cfg_r14_s_::ul_freq_info_r14_s_::ul_bw_r14_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(
      options, 6, value, "rr_cfg_common_scell_r10_s::ul_cfg_r14_s_::ul_freq_info_r14_s_::ul_bw_r14_e_");
}

std::string rr_cfg_common_scell_r10_s::harq_ref_cfg_r14_opts::to_string() const
{
  static const char* options[] = {"sa2", "sa4", "sa5"};
  return convert_enum_idx(options, 3, value, "rr_cfg_common_scell_r10_s::harq_ref_cfg_r14_e_");
}
uint8_t rr_cfg_common_scell_r10_s::harq_ref_cfg_r14_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 5};
  return map_enum_number(options, 3, value, "rr_cfg_common_scell_r10_s::harq_ref_cfg_r14_e_");
}

// CipheringAlgorithm-r12 ::= ENUMERATED
std::string ciphering_algorithm_r12_opts::to_string() const
{
  static const char* options[] = {"eea0", "eea1", "eea2", "eea3-v1130", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "ciphering_algorithm_r12_e");
}
uint8_t ciphering_algorithm_r12_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3};
  return map_enum_number(options, 4, value, "ciphering_algorithm_r12_e");
}

std::string sl_hop_cfg_disc_r12_s::c_r12_opts::to_string() const
{
  static const char* options[] = {"n1", "n5"};
  return convert_enum_idx(options, 2, value, "sl_hop_cfg_disc_r12_s::c_r12_e_");
}
uint8_t sl_hop_cfg_disc_r12_s::c_r12_opts::to_number() const
{
  static const uint8_t options[] = {1, 5};
  return map_enum_number(options, 2, value, "sl_hop_cfg_disc_r12_s::c_r12_e_");
}

std::string security_algorithm_cfg_s::integrity_prot_algorithm_opts::to_string() const
{
  static const char* options[] = {"eia0-v920", "eia1", "eia2", "eia3-v1130", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "security_algorithm_cfg_s::integrity_prot_algorithm_e_");
}
uint8_t security_algorithm_cfg_s::integrity_prot_algorithm_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3};
  return map_enum_number(options, 4, value, "security_algorithm_cfg_s::integrity_prot_algorithm_e_");
}

std::string drb_to_add_mod_scg_r12_s::drb_type_r12_c_::types_opts::to_string() const
{
  static const char* options[] = {"split-r12", "scg-r12"};
  return convert_enum_idx(options, 2, value, "drb_to_add_mod_scg_r12_s::drb_type_r12_c_::types");
}

std::string ip_address_r13_c::types_opts::to_string() const
{
  static const char* options[] = {"ipv4-r13", "ipv6-r13"};
  return convert_enum_idx(options, 2, value, "ip_address_r13_c::types");
}
uint8_t ip_address_r13_c::types_opts::to_number() const
{
  static const uint8_t options[] = {4, 6};
  return map_enum_number(options, 2, value, "ip_address_r13_c::types");
}

std::string security_cfg_ho_v1530_s::handov_type_v1530_c_::types_opts::to_string() const
{
  static const char* options[] = {"intra5GC-r15", "ngc-ToEPC-r15", "epc-ToNGC-r15"};
  return convert_enum_idx(options, 3, value, "security_cfg_ho_v1530_s::handov_type_v1530_c_::types");
}
uint8_t security_cfg_ho_v1530_s::handov_type_v1530_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {5};
  return map_enum_number(options, 1, value, "security_cfg_ho_v1530_s::handov_type_v1530_c_::types");
}

std::string ul_pwr_ctrl_common_v1310_s::delta_f_pucch_format4_r13_opts::to_string() const
{
  static const char* options[] = {
      "deltaF16", "deltaF15", "deltaF14", "deltaF13", "deltaF12", "deltaF11", "deltaF10", "spare1"};
  return convert_enum_idx(options, 8, value, "ul_pwr_ctrl_common_v1310_s::delta_f_pucch_format4_r13_e_");
}
uint8_t ul_pwr_ctrl_common_v1310_s::delta_f_pucch_format4_r13_opts::to_number() const
{
  static const uint8_t options[] = {16, 15, 14, 13, 12, 11, 10};
  return map_enum_number(options, 7, value, "ul_pwr_ctrl_common_v1310_s::delta_f_pucch_format4_r13_e_");
}

std::string ul_pwr_ctrl_common_v1310_s::delta_f_pucch_format5_minus13_opts::to_string() const
{
  static const char* options[] = {
      "deltaF13", "deltaF12", "deltaF11", "deltaF10", "deltaF9", "deltaF8", "deltaF7", "spare1"};
  return convert_enum_idx(options, 8, value, "ul_pwr_ctrl_common_v1310_s::delta_f_pucch_format5_minus13_e_");
}
uint8_t ul_pwr_ctrl_common_v1310_s::delta_f_pucch_format5_minus13_opts::to_number() const
{
  static const uint8_t options[] = {13, 12, 11, 10, 9, 8, 7};
  return map_enum_number(options, 7, value, "ul_pwr_ctrl_common_v1310_s::delta_f_pucch_format5_minus13_e_");
}

std::string ul_pwr_ctrl_common_ps_cell_r12_s::delta_f_pucch_format3_r12_opts::to_string() const
{
  static const char* options[] = {
      "deltaF-1", "deltaF0", "deltaF1", "deltaF2", "deltaF3", "deltaF4", "deltaF5", "deltaF6"};
  return convert_enum_idx(options, 8, value, "ul_pwr_ctrl_common_ps_cell_r12_s::delta_f_pucch_format3_r12_e_");
}
int8_t ul_pwr_ctrl_common_ps_cell_r12_s::delta_f_pucch_format3_r12_opts::to_number() const
{
  static const int8_t options[] = {-1, 0, 1, 2, 3, 4, 5, 6};
  return map_enum_number(options, 8, value, "ul_pwr_ctrl_common_ps_cell_r12_s::delta_f_pucch_format3_r12_e_");
}

std::string ul_pwr_ctrl_common_ps_cell_r12_s::delta_f_pucch_format1b_cs_r12_opts::to_string() const
{
  static const char* options[] = {"deltaF1", "deltaF2", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ul_pwr_ctrl_common_ps_cell_r12_s::delta_f_pucch_format1b_cs_r12_e_");
}
uint8_t ul_pwr_ctrl_common_ps_cell_r12_s::delta_f_pucch_format1b_cs_r12_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "ul_pwr_ctrl_common_ps_cell_r12_s::delta_f_pucch_format1b_cs_r12_e_");
}

std::string rach_skip_r14_s::target_ta_r14_c_::types_opts::to_string() const
{
  static const char* options[] = {"ta0-r14", "mcg-PTAG-r14", "scg-PTAG-r14", "mcg-STAG-r14", "scg-STAG-r14"};
  return convert_enum_idx(options, 5, value, "rach_skip_r14_s::target_ta_r14_c_::types");
}
uint8_t rach_skip_r14_s::target_ta_r14_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {0};
  return map_enum_number(options, 1, value, "rach_skip_r14_s::target_ta_r14_c_::types");
}

std::string rach_skip_r14_s::ul_cfg_info_r14_s_::ul_sched_interv_r14_opts::to_string() const
{
  static const char* options[] = {"sf2", "sf5", "sf10"};
  return convert_enum_idx(options, 3, value, "rach_skip_r14_s::ul_cfg_info_r14_s_::ul_sched_interv_r14_e_");
}
uint8_t rach_skip_r14_s::ul_cfg_info_r14_s_::ul_sched_interv_r14_opts::to_number() const
{
  static const uint8_t options[] = {2, 5, 10};
  return map_enum_number(options, 3, value, "rach_skip_r14_s::ul_cfg_info_r14_s_::ul_sched_interv_r14_e_");
}

std::string rlf_timers_and_consts_scg_r12_c::setup_s_::t313_r12_opts::to_string() const
{
  static const char* options[] = {"ms0", "ms50", "ms100", "ms200", "ms500", "ms1000", "ms2000"};
  return convert_enum_idx(options, 7, value, "rlf_timers_and_consts_scg_r12_c::setup_s_::t313_r12_e_");
}
uint16_t rlf_timers_and_consts_scg_r12_c::setup_s_::t313_r12_opts::to_number() const
{
  static const uint16_t options[] = {0, 50, 100, 200, 500, 1000, 2000};
  return map_enum_number(options, 7, value, "rlf_timers_and_consts_scg_r12_c::setup_s_::t313_r12_e_");
}

std::string rlf_timers_and_consts_scg_r12_c::setup_s_::n313_r12_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4", "n6", "n8", "n10", "n20"};
  return convert_enum_idx(options, 8, value, "rlf_timers_and_consts_scg_r12_c::setup_s_::n313_r12_e_");
}
uint8_t rlf_timers_and_consts_scg_r12_c::setup_s_::n313_r12_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 6, 8, 10, 20};
  return map_enum_number(options, 8, value, "rlf_timers_and_consts_scg_r12_c::setup_s_::n313_r12_e_");
}

std::string rlf_timers_and_consts_scg_r12_c::setup_s_::n314_r12_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4", "n5", "n6", "n8", "n10"};
  return convert_enum_idx(options, 8, value, "rlf_timers_and_consts_scg_r12_c::setup_s_::n314_r12_e_");
}
uint8_t rlf_timers_and_consts_scg_r12_c::setup_s_::n314_r12_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5, 6, 8, 10};
  return map_enum_number(options, 8, value, "rlf_timers_and_consts_scg_r12_c::setup_s_::n314_r12_e_");
}

std::string scell_to_add_mod_r10_s::scell_state_r15_opts::to_string() const
{
  static const char* options[] = {"activated", "dormant"};
  return convert_enum_idx(options, 2, value, "scell_to_add_mod_r10_s::scell_state_r15_e_");
}

std::string scell_to_add_mod_ext_v1430_s::scell_state_r15_opts::to_string() const
{
  static const char* options[] = {"activated", "dormant"};
  return convert_enum_idx(options, 2, value, "scell_to_add_mod_ext_v1430_s::scell_state_r15_e_");
}

std::string sl_disc_tx_ref_carrier_ded_r13_c::types_opts::to_string() const
{
  static const char* options[] = {"pCell", "sCell"};
  return convert_enum_idx(options, 2, value, "sl_disc_tx_ref_carrier_ded_r13_c::types");
}

std::string sl_disc_tx_res_r13_c::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"scheduled-r13", "ue-Selected-r13"};
  return convert_enum_idx(options, 2, value, "sl_disc_tx_res_r13_c::setup_c_::types");
}

std::string sl_gap_pattern_r13_s::gap_period_r13_opts::to_string() const
{
  static const char* options[] = {"sf40",
                                  "sf60",
                                  "sf70",
                                  "sf80",
                                  "sf120",
                                  "sf140",
                                  "sf160",
                                  "sf240",
                                  "sf280",
                                  "sf320",
                                  "sf640",
                                  "sf1280",
                                  "sf2560",
                                  "sf5120",
                                  "sf10240"};
  return convert_enum_idx(options, 15, value, "sl_gap_pattern_r13_s::gap_period_r13_e_");
}
uint16_t sl_gap_pattern_r13_s::gap_period_r13_opts::to_number() const
{
  static const uint16_t options[] = {40, 60, 70, 80, 120, 140, 160, 240, 280, 320, 640, 1280, 2560, 5120, 10240};
  return map_enum_number(options, 15, value, "sl_gap_pattern_r13_s::gap_period_r13_e_");
}

// SubframeAssignment-r15 ::= ENUMERATED
std::string sf_assign_r15_opts::to_string() const
{
  static const char* options[] = {"sa0", "sa1", "sa2", "sa3", "sa4", "sa5", "sa6"};
  return convert_enum_idx(options, 7, value, "sf_assign_r15_e");
}
uint8_t sf_assign_r15_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6};
  return map_enum_number(options, 7, value, "sf_assign_r15_e");
}

std::string wlan_mob_cfg_r13_s::assoc_timer_r13_opts::to_string() const
{
  static const char* options[] = {"s10", "s30", "s60", "s120", "s240"};
  return convert_enum_idx(options, 5, value, "wlan_mob_cfg_r13_s::assoc_timer_r13_e_");
}
uint8_t wlan_mob_cfg_r13_s::assoc_timer_r13_opts::to_number() const
{
  static const uint8_t options[] = {10, 30, 60, 120, 240};
  return map_enum_number(options, 5, value, "wlan_mob_cfg_r13_s::assoc_timer_r13_e_");
}

// CA-BandwidthClass-r10 ::= ENUMERATED
std::string ca_bw_class_r10_opts::to_string() const
{
  static const char* options[] = {"a", "b", "c", "d", "e", "f"};
  return convert_enum_idx(options, 6, value, "ca_bw_class_r10_e");
}

std::string mob_ctrl_info_scg_r12_s::t307_r12_opts::to_string() const
{
  static const char* options[] = {"ms50", "ms100", "ms150", "ms200", "ms500", "ms1000", "ms2000", "spare1"};
  return convert_enum_idx(options, 8, value, "mob_ctrl_info_scg_r12_s::t307_r12_e_");
}
uint16_t mob_ctrl_info_scg_r12_s::t307_r12_opts::to_number() const
{
  static const uint16_t options[] = {50, 100, 150, 200, 500, 1000, 2000};
  return map_enum_number(options, 7, value, "mob_ctrl_info_scg_r12_s::t307_r12_e_");
}

std::string rclwi_cfg_r13_s::cmd_c_::types_opts::to_string() const
{
  static const char* options[] = {"steerToWLAN-r13", "steerToLTE-r13"};
  return convert_enum_idx(options, 2, value, "rclwi_cfg_r13_s::cmd_c_::types");
}

std::string sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"scheduled-r14", "ue-Selected-r14"};
  return convert_enum_idx(options, 2, value, "sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::setup_c_::types");
}

std::string sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"scheduled-v1530", "ue-Selected-v1530"};
  return convert_enum_idx(options, 2, value, "sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::setup_c_::types");
}

std::string ran_notif_area_info_r15_c::types_opts::to_string() const
{
  static const char* options[] = {"cellList-r15", "ran-AreaConfigList-r15"};
  return convert_enum_idx(options, 2, value, "ran_notif_area_info_r15_c::types");
}

std::string meas_idle_cfg_ded_r15_s::meas_idle_dur_r15_opts::to_string() const
{
  static const char* options[] = {"sec10", "sec30", "sec60", "sec120", "sec180", "sec240", "sec300", "spare"};
  return convert_enum_idx(options, 8, value, "meas_idle_cfg_ded_r15_s::meas_idle_dur_r15_e_");
}
uint16_t meas_idle_cfg_ded_r15_s::meas_idle_dur_r15_opts::to_number() const
{
  static const uint16_t options[] = {10, 30, 60, 120, 180, 240, 300};
  return map_enum_number(options, 7, value, "meas_idle_cfg_ded_r15_s::meas_idle_dur_r15_e_");
}

std::string rrc_inactive_cfg_r15_s::ran_paging_cycle_r15_opts::to_string() const
{
  static const char* options[] = {"rf32", "rf64", "rf128", "rf256"};
  return convert_enum_idx(options, 4, value, "rrc_inactive_cfg_r15_s::ran_paging_cycle_r15_e_");
}
uint16_t rrc_inactive_cfg_r15_s::ran_paging_cycle_r15_opts::to_number() const
{
  static const uint16_t options[] = {32, 64, 128, 256};
  return map_enum_number(options, 4, value, "rrc_inactive_cfg_r15_s::ran_paging_cycle_r15_e_");
}

std::string rrc_inactive_cfg_r15_s::periodic_rnau_timer_r15_opts::to_string() const
{
  static const char* options[] = {"min5", "min10", "min20", "min30", "min60", "min120", "min360", "min720"};
  return convert_enum_idx(options, 8, value, "rrc_inactive_cfg_r15_s::periodic_rnau_timer_r15_e_");
}
uint16_t rrc_inactive_cfg_r15_s::periodic_rnau_timer_r15_opts::to_number() const
{
  static const uint16_t options[] = {5, 10, 20, 30, 60, 120, 360, 720};
  return map_enum_number(options, 8, value, "rrc_inactive_cfg_r15_s::periodic_rnau_timer_r15_e_");
}

std::string sl_comm_cfg_r12_s::comm_tx_res_r12_c_::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"scheduled-r12", "ue-Selected-r12"};
  return convert_enum_idx(options, 2, value, "sl_comm_cfg_r12_s::comm_tx_res_r12_c_::setup_c_::types");
}

std::string sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"scheduled-v1310", "ue-Selected-v1310"};
  return convert_enum_idx(options, 2, value, "sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::setup_c_::types");
}

std::string sl_disc_cfg_r12_s::disc_tx_res_r12_c_::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"scheduled-r12", "ue-Selected-r12"};
  return convert_enum_idx(options, 2, value, "sl_disc_cfg_r12_s::disc_tx_res_r12_c_::setup_c_::types");
}

std::string sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::setup_c_::types_opts::to_string() const
{
  static const char* options[] = {"scheduled-r13", "ue-Selected-r13"};
  return convert_enum_idx(options, 2, value, "sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::setup_c_::types");
}

std::string sl_sync_tx_ctrl_r12_s::network_ctrl_sync_tx_r12_opts::to_string() const
{
  static const char* options[] = {"on", "off"};
  return convert_enum_idx(options, 2, value, "sl_sync_tx_ctrl_r12_s::network_ctrl_sync_tx_r12_e_");
}

// CDMA2000-Type ::= ENUMERATED
std::string cdma2000_type_opts::to_string() const
{
  static const char* options[] = {"type1XRTT", "typeHRPD"};
  return convert_enum_idx(options, 2, value, "cdma2000_type_e");
}
uint8_t cdma2000_type_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "cdma2000_type_e");
}

// MeasCycleSCell-r10 ::= ENUMERATED
std::string meas_cycle_scell_r10_opts::to_string() const
{
  static const char* options[] = {"sf160", "sf256", "sf320", "sf512", "sf640", "sf1024", "sf1280", "spare1"};
  return convert_enum_idx(options, 8, value, "meas_cycle_scell_r10_e");
}
uint16_t meas_cycle_scell_r10_opts::to_number() const
{
  static const uint16_t options[] = {160, 256, 320, 512, 640, 1024, 1280};
  return map_enum_number(options, 7, value, "meas_cycle_scell_r10_e");
}

std::string meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_::types_opts::to_string() const
{
  static const char* options[] = {"ms40-r12", "ms80-r12", "ms160-r12"};
  return convert_enum_idx(options, 3, value, "meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_::types");
}
uint8_t meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {40, 80, 160};
  return map_enum_number(options, 3, value, "meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_::types");
}

std::string meas_ds_cfg_r12_c::setup_s_::ds_occasion_dur_r12_c_::types_opts::to_string() const
{
  static const char* options[] = {"durationFDD-r12", "durationTDD-r12"};
  return convert_enum_idx(options, 2, value, "meas_ds_cfg_r12_c::setup_s_::ds_occasion_dur_r12_c_::types");
}

std::string meas_gap_cfg_c::setup_s_::gap_offset_c_::types_opts::to_string() const
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

std::string meas_sensing_cfg_r15_s::sensing_periodicity_r15_opts::to_string() const
{
  static const char* options[] = {
      "ms20", "ms50", "ms100", "ms200", "ms300", "ms400", "ms500", "ms600", "ms700", "ms800", "ms900", "ms1000"};
  return convert_enum_idx(options, 12, value, "meas_sensing_cfg_r15_s::sensing_periodicity_r15_e_");
}
uint16_t meas_sensing_cfg_r15_s::sensing_periodicity_r15_opts::to_number() const
{
  static const uint16_t options[] = {20, 50, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
  return map_enum_number(options, 12, value, "meas_sensing_cfg_r15_s::sensing_periodicity_r15_e_");
}

std::string rmtc_cfg_r13_c::setup_s_::rmtc_period_r13_opts::to_string() const
{
  static const char* options[] = {"ms40", "ms80", "ms160", "ms320", "ms640"};
  return convert_enum_idx(options, 5, value, "rmtc_cfg_r13_c::setup_s_::rmtc_period_r13_e_");
}
uint16_t rmtc_cfg_r13_c::setup_s_::rmtc_period_r13_opts::to_number() const
{
  static const uint16_t options[] = {40, 80, 160, 320, 640};
  return map_enum_number(options, 5, value, "rmtc_cfg_r13_c::setup_s_::rmtc_period_r13_e_");
}

std::string rmtc_cfg_r13_c::setup_s_::meas_dur_r13_opts::to_string() const
{
  static const char* options[] = {"sym1", "sym14", "sym28", "sym42", "sym70"};
  return convert_enum_idx(options, 5, value, "rmtc_cfg_r13_c::setup_s_::meas_dur_r13_e_");
}
uint8_t rmtc_cfg_r13_c::setup_s_::meas_dur_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 14, 28, 42, 70};
  return map_enum_number(options, 5, value, "rmtc_cfg_r13_c::setup_s_::meas_dur_r13_e_");
}

std::string rrc_conn_recfg_v1250_ies_s::wlan_offload_info_r12_c_::setup_s_::t350_r12_opts::to_string() const
{
  static const char* options[] = {"min5", "min10", "min20", "min30", "min60", "min120", "min180", "spare1"};
  return convert_enum_idx(
      options, 8, value, "rrc_conn_recfg_v1250_ies_s::wlan_offload_info_r12_c_::setup_s_::t350_r12_e_");
}
uint8_t rrc_conn_recfg_v1250_ies_s::wlan_offload_info_r12_c_::setup_s_::t350_r12_opts::to_number() const
{
  static const uint8_t options[] = {5, 10, 20, 30, 60, 120, 180};
  return map_enum_number(
      options, 7, value, "rrc_conn_recfg_v1250_ies_s::wlan_offload_info_r12_c_::setup_s_::t350_r12_e_");
}

std::string rrc_conn_release_v1530_ies_s::cn_type_r15_opts::to_string() const
{
  static const char* options[] = {"epc", "fivegc"};
  return convert_enum_idx(options, 2, value, "rrc_conn_release_v1530_ies_s::cn_type_r15_e_");
}
uint8_t rrc_conn_release_v1530_ies_s::cn_type_r15_opts::to_number() const
{
  if (value == fivegc) {
    return 5;
  }
  invalid_enum_number(value, "rrc_conn_release_v1530_ies_s::cn_type_r15_e_");
  return 0;
}

std::string rs_cfg_ssb_nr_r15_s::subcarrier_spacing_ssb_r15_opts::to_string() const
{
  static const char* options[] = {"kHz15", "kHz30", "kHz120", "kHz240"};
  return convert_enum_idx(options, 4, value, "rs_cfg_ssb_nr_r15_s::subcarrier_spacing_ssb_r15_e_");
}
uint8_t rs_cfg_ssb_nr_r15_s::subcarrier_spacing_ssb_r15_opts::to_number() const
{
  static const uint8_t options[] = {15, 30, 120, 240};
  return map_enum_number(options, 4, value, "rs_cfg_ssb_nr_r15_s::subcarrier_spacing_ssb_r15_e_");
}

// ReportInterval ::= ENUMERATED
std::string report_interv_opts::to_string() const
{
  static const char* options[] = {"ms120",
                                  "ms240",
                                  "ms480",
                                  "ms640",
                                  "ms1024",
                                  "ms2048",
                                  "ms5120",
                                  "ms10240",
                                  "min1",
                                  "min6",
                                  "min12",
                                  "min30",
                                  "min60",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "report_interv_e");
}
uint16_t report_interv_opts::to_number() const
{
  static const uint16_t options[] = {120, 240, 480, 640, 1024, 2048, 5120, 10240, 1, 6, 12, 30, 60};
  return map_enum_number(options, 13, value, "report_interv_e");
}

std::string thres_eutra_c::types_opts::to_string() const
{
  static const char* options[] = {"threshold-RSRP", "threshold-RSRQ"};
  return convert_enum_idx(options, 2, value, "thres_eutra_c::types");
}

std::string thres_nr_r15_c::types_opts::to_string() const
{
  static const char* options[] = {"nr-RSRP-r15", "nr-RSRQ-r15", "nr-SINR-r15"};
  return convert_enum_idx(options, 3, value, "thres_nr_r15_c::types");
}

std::string thres_utra_c::types_opts::to_string() const
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

// TimeToTrigger ::= ENUMERATED
std::string time_to_trigger_opts::to_string() const
{
  static const char* options[] = {"ms0",
                                  "ms40",
                                  "ms64",
                                  "ms80",
                                  "ms100",
                                  "ms128",
                                  "ms160",
                                  "ms256",
                                  "ms320",
                                  "ms480",
                                  "ms512",
                                  "ms640",
                                  "ms1024",
                                  "ms1280",
                                  "ms2560",
                                  "ms5120"};
  return convert_enum_idx(options, 16, value, "time_to_trigger_e");
}
uint16_t time_to_trigger_opts::to_number() const
{
  static const uint16_t options[] = {0, 40, 64, 80, 100, 128, 160, 256, 320, 480, 512, 640, 1024, 1280, 2560, 5120};
  return map_enum_number(options, 16, value, "time_to_trigger_e");
}

std::string ul_delay_cfg_r13_c::setup_s_::delay_thres_r13_opts::to_string() const
{
  static const char* options[] = {"ms30",
                                  "ms40",
                                  "ms50",
                                  "ms60",
                                  "ms70",
                                  "ms80",
                                  "ms90",
                                  "ms100",
                                  "ms150",
                                  "ms300",
                                  "ms500",
                                  "ms750",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "ul_delay_cfg_r13_c::setup_s_::delay_thres_r13_e_");
}
uint16_t ul_delay_cfg_r13_c::setup_s_::delay_thres_r13_opts::to_number() const
{
  static const uint16_t options[] = {30, 40, 50, 60, 70, 80, 90, 100, 150, 300, 500, 750};
  return map_enum_number(options, 12, value, "ul_delay_cfg_r13_c::setup_s_::delay_thres_r13_e_");
}

// WLAN-BandIndicator-r13 ::= ENUMERATED
std::string wlan_band_ind_r13_opts::to_string() const
{
  static const char* options[] = {
      "band2dot4", "band5", "band60-v1430", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "wlan_band_ind_r13_e");
}
float wlan_band_ind_r13_opts::to_number() const
{
  static const float options[] = {2.4, 5.0, 60.0};
  return map_enum_number(options, 3, value, "wlan_band_ind_r13_e");
}
std::string wlan_band_ind_r13_opts::to_number_string() const
{
  static const char* options[] = {"2.4", "5", "60"};
  return convert_enum_idx(options, 8, value, "wlan_band_ind_r13_e");
}

std::string wlan_carrier_info_r13_s::country_code_r13_opts::to_string() const
{
  static const char* options[] = {"unitedStates", "europe", "japan", "global"};
  return convert_enum_idx(options, 4, value, "wlan_carrier_info_r13_s::country_code_r13_e_");
}

std::string idc_cfg_r11_s::autonomous_denial_params_r11_s_::autonomous_denial_sfs_r11_opts::to_string() const
{
  static const char* options[] = {"n2", "n5", "n10", "n15", "n20", "n30", "spare2", "spare1"};
  return convert_enum_idx(
      options, 8, value, "idc_cfg_r11_s::autonomous_denial_params_r11_s_::autonomous_denial_sfs_r11_e_");
}
uint8_t idc_cfg_r11_s::autonomous_denial_params_r11_s_::autonomous_denial_sfs_r11_opts::to_number() const
{
  static const uint8_t options[] = {2, 5, 10, 15, 20, 30};
  return map_enum_number(
      options, 6, value, "idc_cfg_r11_s::autonomous_denial_params_r11_s_::autonomous_denial_sfs_r11_e_");
}

std::string idc_cfg_r11_s::autonomous_denial_params_r11_s_::autonomous_denial_validity_r11_opts::to_string() const
{
  static const char* options[] = {"sf200", "sf500", "sf1000", "sf2000", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(
      options, 8, value, "idc_cfg_r11_s::autonomous_denial_params_r11_s_::autonomous_denial_validity_r11_e_");
}
uint16_t idc_cfg_r11_s::autonomous_denial_params_r11_s_::autonomous_denial_validity_r11_opts::to_number() const
{
  static const uint16_t options[] = {200, 500, 1000, 2000};
  return map_enum_number(
      options, 4, value, "idc_cfg_r11_s::autonomous_denial_params_r11_s_::autonomous_denial_validity_r11_e_");
}

std::string meas_obj_eutra_s::t312_r12_c_::setup_opts::to_string() const
{
  static const char* options[] = {"ms0", "ms50", "ms100", "ms200", "ms300", "ms400", "ms500", "ms1000"};
  return convert_enum_idx(options, 8, value, "meas_obj_eutra_s::t312_r12_c_::setup_e_");
}
uint16_t meas_obj_eutra_s::t312_r12_c_::setup_opts::to_number() const
{
  static const uint16_t options[] = {0, 50, 100, 200, 300, 400, 500, 1000};
  return map_enum_number(options, 8, value, "meas_obj_eutra_s::t312_r12_c_::setup_e_");
}

std::string meas_obj_utra_s::cells_to_add_mod_list_c_::types_opts::to_string() const
{
  static const char* options[] = {"cellsToAddModListUTRA-FDD", "cellsToAddModListUTRA-TDD"};
  return convert_enum_idx(options, 2, value, "meas_obj_utra_s::cells_to_add_mod_list_c_::types");
}

std::string meas_obj_utra_s::cell_for_which_to_report_cgi_c_::types_opts::to_string() const
{
  static const char* options[] = {"utra-FDD", "utra-TDD"};
  return convert_enum_idx(options, 2, value, "meas_obj_utra_s::cell_for_which_to_report_cgi_c_::types");
}

std::string meas_obj_wlan_r13_s::carrier_freq_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"bandIndicatorListWLAN-r13", "carrierInfoListWLAN-r13"};
  return convert_enum_idx(options, 2, value, "meas_obj_wlan_r13_s::carrier_freq_r13_c_::types");
}

std::string pwr_pref_ind_cfg_r11_c::setup_s_::pwr_pref_ind_timer_r11_opts::to_string() const
{
  static const char* options[] = {"s0",
                                  "s0dot5",
                                  "s1",
                                  "s2",
                                  "s5",
                                  "s10",
                                  "s20",
                                  "s30",
                                  "s60",
                                  "s90",
                                  "s120",
                                  "s300",
                                  "s600",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "pwr_pref_ind_cfg_r11_c::setup_s_::pwr_pref_ind_timer_r11_e_");
}
float pwr_pref_ind_cfg_r11_c::setup_s_::pwr_pref_ind_timer_r11_opts::to_number() const
{
  static const float options[] = {0.0, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 30.0, 60.0, 90.0, 120.0, 300.0, 600.0};
  return map_enum_number(options, 13, value, "pwr_pref_ind_cfg_r11_c::setup_s_::pwr_pref_ind_timer_r11_e_");
}
std::string pwr_pref_ind_cfg_r11_c::setup_s_::pwr_pref_ind_timer_r11_opts::to_number_string() const
{
  static const char* options[] = {"0", "0.5", "1", "2", "5", "10", "20", "30", "60", "90", "120", "300", "600"};
  return convert_enum_idx(options, 16, value, "pwr_pref_ind_cfg_r11_c::setup_s_::pwr_pref_ind_timer_r11_e_");
}

std::string eutra_event_s::event_id_c_::types_opts::to_string() const
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

std::string report_cfg_eutra_s::trigger_type_c_::periodical_s_::purpose_opts::to_string() const
{
  static const char* options[] = {"reportStrongestCells", "reportCGI"};
  return convert_enum_idx(options, 2, value, "report_cfg_eutra_s::trigger_type_c_::periodical_s_::purpose_e_");
}

std::string report_cfg_eutra_s::trigger_type_c_::types_opts::to_string() const
{
  static const char* options[] = {"event", "periodical"};
  return convert_enum_idx(options, 2, value, "report_cfg_eutra_s::trigger_type_c_::types");
}

std::string report_cfg_eutra_s::trigger_quant_opts::to_string() const
{
  static const char* options[] = {"rsrp", "rsrq"};
  return convert_enum_idx(options, 2, value, "report_cfg_eutra_s::trigger_quant_e_");
}

std::string report_cfg_eutra_s::report_quant_opts::to_string() const
{
  static const char* options[] = {"sameAsTriggerQuantity", "both"};
  return convert_enum_idx(options, 2, value, "report_cfg_eutra_s::report_quant_e_");
}

std::string report_cfg_eutra_s::report_amount_opts::to_string() const
{
  static const char* options[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "infinity"};
  return convert_enum_idx(options, 8, value, "report_cfg_eutra_s::report_amount_e_");
}
int8_t report_cfg_eutra_s::report_amount_opts::to_number() const
{
  static const int8_t options[] = {1, 2, 4, 8, 16, 32, 64, -1};
  return map_enum_number(options, 8, value, "report_cfg_eutra_s::report_amount_e_");
}

std::string report_cfg_eutra_s::rs_sinr_cfg_r13_c_::setup_s_::report_quant_v1310_opts::to_string() const
{
  static const char* options[] = {"rsrpANDsinr", "rsrqANDsinr", "all"};
  return convert_enum_idx(options, 3, value, "report_cfg_eutra_s::rs_sinr_cfg_r13_c_::setup_s_::report_quant_v1310_e_");
}

std::string report_cfg_eutra_s::purpose_v1430_opts::to_string() const
{
  static const char* options[] = {"reportLocation", "sidelink", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "report_cfg_eutra_s::purpose_v1430_e_");
}

std::string
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b1_s_::b1_thres_c_::types_opts::to_string() const
{
  static const char* options[] = {"b1-ThresholdUTRA", "b1-ThresholdGERAN", "b1-ThresholdCDMA2000"};
  return convert_enum_idx(
      options,
      3,
      value,
      "report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b1_s_::b1_thres_c_::types");
}

std::string
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b2_s_::b2_thres2_c_::types_opts::to_string() const
{
  static const char* options[] = {"b2-Threshold2UTRA", "b2-Threshold2GERAN", "b2-Threshold2CDMA2000"};
  return convert_enum_idx(
      options,
      3,
      value,
      "report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b2_s_::b2_thres2_c_::types");
}

std::string report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "eventB1", "eventB2", "eventW1-r13", "eventW2-r13", "eventW3-r13", "eventB1-NR-r15", "eventB2-NR-r15"};
  return convert_enum_idx(options, 7, value, "report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::types");
}

std::string report_cfg_inter_rat_s::trigger_type_c_::periodical_s_::purpose_opts::to_string() const
{
  static const char* options[] = {"reportStrongestCells", "reportStrongestCellsForSON", "reportCGI"};
  return convert_enum_idx(options, 3, value, "report_cfg_inter_rat_s::trigger_type_c_::periodical_s_::purpose_e_");
}

std::string report_cfg_inter_rat_s::trigger_type_c_::types_opts::to_string() const
{
  static const char* options[] = {"event", "periodical"};
  return convert_enum_idx(options, 2, value, "report_cfg_inter_rat_s::trigger_type_c_::types");
}

std::string report_cfg_inter_rat_s::report_amount_opts::to_string() const
{
  static const char* options[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "infinity"};
  return convert_enum_idx(options, 8, value, "report_cfg_inter_rat_s::report_amount_e_");
}
int8_t report_cfg_inter_rat_s::report_amount_opts::to_number() const
{
  static const int8_t options[] = {1, 2, 4, 8, 16, 32, 64, -1};
  return map_enum_number(options, 8, value, "report_cfg_inter_rat_s::report_amount_e_");
}

std::string report_cfg_inter_rat_s::report_sftd_meas_r15_opts::to_string() const
{
  static const char* options[] = {"pSCell", "neighborCells"};
  return convert_enum_idx(options, 2, value, "report_cfg_inter_rat_s::report_sftd_meas_r15_e_");
}

std::string meas_obj_to_add_mod_s::meas_obj_c_::types_opts::to_string() const
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

std::string meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_::types_opts::to_string() const
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

std::string other_cfg_r9_s::bw_pref_ind_timer_r14_opts::to_string() const
{
  static const char* options[] = {"s0",
                                  "s0dot5",
                                  "s1",
                                  "s2",
                                  "s5",
                                  "s10",
                                  "s20",
                                  "s30",
                                  "s60",
                                  "s90",
                                  "s120",
                                  "s300",
                                  "s600",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "other_cfg_r9_s::bw_pref_ind_timer_r14_e_");
}
float other_cfg_r9_s::bw_pref_ind_timer_r14_opts::to_number() const
{
  static const float options[] = {0.0, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 30.0, 60.0, 90.0, 120.0, 300.0, 600.0};
  return map_enum_number(options, 13, value, "other_cfg_r9_s::bw_pref_ind_timer_r14_e_");
}
std::string other_cfg_r9_s::bw_pref_ind_timer_r14_opts::to_number_string() const
{
  static const char* options[] = {"0", "0.5", "1", "2", "5", "10", "20", "30", "60", "90", "120", "300", "600"};
  return convert_enum_idx(options, 16, value, "other_cfg_r9_s::bw_pref_ind_timer_r14_e_");
}

std::string
other_cfg_r9_s::delay_budget_report_cfg_r14_c_::setup_s_::delay_budget_report_prohibit_timer_r14_opts::to_string() const
{
  static const char* options[] = {"s0", "s0dot4", "s0dot8", "s1dot6", "s3", "s6", "s12", "s30"};
  return convert_enum_idx(
      options,
      8,
      value,
      "other_cfg_r9_s::delay_budget_report_cfg_r14_c_::setup_s_::delay_budget_report_prohibit_timer_r14_e_");
}
float other_cfg_r9_s::delay_budget_report_cfg_r14_c_::setup_s_::delay_budget_report_prohibit_timer_r14_opts::to_number()
    const
{
  static const float options[] = {0.0, 0.4, 0.8, 1.6, 3.0, 6.0, 12.0, 30.0};
  return map_enum_number(
      options,
      8,
      value,
      "other_cfg_r9_s::delay_budget_report_cfg_r14_c_::setup_s_::delay_budget_report_prohibit_timer_r14_e_");
}
std::string other_cfg_r9_s::delay_budget_report_cfg_r14_c_::setup_s_::delay_budget_report_prohibit_timer_r14_opts::
    to_number_string() const
{
  static const char* options[] = {"0", "0.4", "0.8", "1.6", "3", "6", "12", "30"};
  return convert_enum_idx(
      options,
      8,
      value,
      "other_cfg_r9_s::delay_budget_report_cfg_r14_c_::setup_s_::delay_budget_report_prohibit_timer_r14_e_");
}

std::string other_cfg_r9_s::rlm_report_cfg_r14_c_::setup_s_::rlm_report_timer_r14_opts::to_string() const
{
  static const char* options[] = {"s0",
                                  "s0dot5",
                                  "s1",
                                  "s2",
                                  "s5",
                                  "s10",
                                  "s20",
                                  "s30",
                                  "s60",
                                  "s90",
                                  "s120",
                                  "s300",
                                  "s600",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(
      options, 16, value, "other_cfg_r9_s::rlm_report_cfg_r14_c_::setup_s_::rlm_report_timer_r14_e_");
}
float other_cfg_r9_s::rlm_report_cfg_r14_c_::setup_s_::rlm_report_timer_r14_opts::to_number() const
{
  static const float options[] = {0.0, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 30.0, 60.0, 90.0, 120.0, 300.0, 600.0};
  return map_enum_number(
      options, 13, value, "other_cfg_r9_s::rlm_report_cfg_r14_c_::setup_s_::rlm_report_timer_r14_e_");
}
std::string other_cfg_r9_s::rlm_report_cfg_r14_c_::setup_s_::rlm_report_timer_r14_opts::to_number_string() const
{
  static const char* options[] = {"0", "0.5", "1", "2", "5", "10", "20", "30", "60", "90", "120", "300", "600"};
  return convert_enum_idx(
      options, 16, value, "other_cfg_r9_s::rlm_report_cfg_r14_c_::setup_s_::rlm_report_timer_r14_e_");
}

std::string
other_cfg_r9_s::overheat_assist_cfg_r14_c_::setup_s_::overheat_ind_prohibit_timer_r14_opts::to_string() const
{
  static const char* options[] = {"s0",
                                  "s0dot5",
                                  "s1",
                                  "s2",
                                  "s5",
                                  "s10",
                                  "s20",
                                  "s30",
                                  "s60",
                                  "s90",
                                  "s120",
                                  "s300",
                                  "s600",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(
      options, 16, value, "other_cfg_r9_s::overheat_assist_cfg_r14_c_::setup_s_::overheat_ind_prohibit_timer_r14_e_");
}
float other_cfg_r9_s::overheat_assist_cfg_r14_c_::setup_s_::overheat_ind_prohibit_timer_r14_opts::to_number() const
{
  static const float options[] = {0.0, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 30.0, 60.0, 90.0, 120.0, 300.0, 600.0};
  return map_enum_number(
      options, 13, value, "other_cfg_r9_s::overheat_assist_cfg_r14_c_::setup_s_::overheat_ind_prohibit_timer_r14_e_");
}
std::string
other_cfg_r9_s::overheat_assist_cfg_r14_c_::setup_s_::overheat_ind_prohibit_timer_r14_opts::to_number_string() const
{
  static const char* options[] = {"0", "0.5", "1", "2", "5", "10", "20", "30", "60", "90", "120", "300", "600"};
  return convert_enum_idx(
      options, 16, value, "other_cfg_r9_s::overheat_assist_cfg_r14_c_::setup_s_::overheat_ind_prohibit_timer_r14_e_");
}

std::string other_cfg_r9_s::meas_cfg_app_layer_r15_c_::setup_s_::service_type_opts::to_string() const
{
  static const char* options[] = {"qoe", "qoemtsi", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "other_cfg_r9_s::meas_cfg_app_layer_r15_c_::setup_s_::service_type_e_");
}

std::string prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_opts::to_string() const
{
  static const char* options[] = {"v1", "v1dot5", "v2", "v2dot5", "v4", "v5", "v8", "v10"};
  return convert_enum_idx(options, 8, value, "prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_e_");
}
float prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_opts::to_number() const
{
  static const float options[] = {1.0, 1.5, 2.0, 2.5, 4.0, 5.0, 8.0, 10.0};
  return map_enum_number(options, 8, value, "prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_e_");
}
std::string prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_opts::to_number_string() const
{
  static const char* options[] = {"1", "1.5", "2", "2.5", "4", "5", "8", "10"};
  return convert_enum_idx(options, 8, value, "prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_e_");
}

std::string prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::tdd_r13_opts::to_string() const
{
  static const char* options[] = {"v1", "v2", "v4", "v5", "v8", "v10", "v20", "spare"};
  return convert_enum_idx(options, 8, value, "prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::tdd_r13_e_");
}
uint8_t prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::tdd_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 5, 8, 10, 20};
  return map_enum_number(options, 7, value, "prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::tdd_r13_e_");
}

std::string prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"fdd-r13", "tdd-r13"};
  return convert_enum_idx(options, 2, value, "prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::types");
}

std::string quant_cfg_cdma2000_s::meas_quant_cdma2000_opts::to_string() const
{
  static const char* options[] = {"pilotStrength", "pilotPnPhaseAndPilotStrength"};
  return convert_enum_idx(options, 2, value, "quant_cfg_cdma2000_s::meas_quant_cdma2000_e_");
}

std::string quant_cfg_utra_s::meas_quant_utra_fdd_opts::to_string() const
{
  static const char* options[] = {"cpich-RSCP", "cpich-EcN0"};
  return convert_enum_idx(options, 2, value, "quant_cfg_utra_s::meas_quant_utra_fdd_e_");
}
uint8_t quant_cfg_utra_s::meas_quant_utra_fdd_opts::to_number() const
{
  if (value == cpich_ec_n0) {
    return 0;
  }
  invalid_enum_number(value, "quant_cfg_utra_s::meas_quant_utra_fdd_e_");
  return 0;
}

std::string report_cfg_to_add_mod_s::report_cfg_c_::types_opts::to_string() const
{
  static const char* options[] = {"reportConfigEUTRA", "reportConfigInterRAT"};
  return convert_enum_idx(options, 2, value, "report_cfg_to_add_mod_s::report_cfg_c_::types");
}

std::string carrier_bw_eutra_s::dl_bw_opts::to_string() const
{
  static const char* options[] = {"n6",
                                  "n15",
                                  "n25",
                                  "n50",
                                  "n75",
                                  "n100",
                                  "spare10",
                                  "spare9",
                                  "spare8",
                                  "spare7",
                                  "spare6",
                                  "spare5",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "carrier_bw_eutra_s::dl_bw_e_");
}
uint8_t carrier_bw_eutra_s::dl_bw_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(options, 6, value, "carrier_bw_eutra_s::dl_bw_e_");
}

std::string carrier_bw_eutra_s::ul_bw_opts::to_string() const
{
  static const char* options[] = {"n6",
                                  "n15",
                                  "n25",
                                  "n50",
                                  "n75",
                                  "n100",
                                  "spare10",
                                  "spare9",
                                  "spare8",
                                  "spare7",
                                  "spare6",
                                  "spare5",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "carrier_bw_eutra_s::ul_bw_e_");
}
uint8_t carrier_bw_eutra_s::ul_bw_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(options, 6, value, "carrier_bw_eutra_s::ul_bw_e_");
}

std::string carrier_info_nr_r15_s::subcarrier_spacing_ssb_r15_opts::to_string() const
{
  static const char* options[] = {"kHz15", "kHz30", "kHz120", "kHz240"};
  return convert_enum_idx(options, 4, value, "carrier_info_nr_r15_s::subcarrier_spacing_ssb_r15_e_");
}
uint8_t carrier_info_nr_r15_s::subcarrier_spacing_ssb_r15_opts::to_number() const
{
  static const uint8_t options[] = {15, 30, 120, 240};
  return map_enum_number(options, 4, value, "carrier_info_nr_r15_s::subcarrier_spacing_ssb_r15_e_");
}

std::string meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::types_opts::to_string() const
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

std::string meas_gap_sharing_cfg_r14_c::setup_s_::meas_gap_sharing_scheme_r14_opts::to_string() const
{
  static const char* options[] = {"scheme00", "scheme01", "scheme10", "scheme11"};
  return convert_enum_idx(options, 4, value, "meas_gap_sharing_cfg_r14_c::setup_s_::meas_gap_sharing_scheme_r14_e_");
}
float meas_gap_sharing_cfg_r14_c::setup_s_::meas_gap_sharing_scheme_r14_opts::to_number() const
{
  static const float options[] = {0.0, 0.1, 1.0, 1.1};
  return map_enum_number(options, 4, value, "meas_gap_sharing_cfg_r14_c::setup_s_::meas_gap_sharing_scheme_r14_e_");
}
std::string meas_gap_sharing_cfg_r14_c::setup_s_::meas_gap_sharing_scheme_r14_opts::to_number_string() const
{
  static const char* options[] = {"0.0", "0.1", "1.0", "1.1"};
  return convert_enum_idx(options, 4, value, "meas_gap_sharing_cfg_r14_c::setup_s_::meas_gap_sharing_scheme_r14_e_");
}

// MeasScaleFactor-r12 ::= ENUMERATED
std::string meas_scale_factor_r12_opts::to_string() const
{
  static const char* options[] = {"sf-EUTRA-cf1", "sf-EUTRA-cf2"};
  return convert_enum_idx(options, 2, value, "meas_scale_factor_r12_e");
}
uint8_t meas_scale_factor_r12_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "meas_scale_factor_r12_e");
}

// RAT-Type ::= ENUMERATED
std::string rat_type_opts::to_string() const
{
  static const char* options[] = {
      "eutra", "utra", "geran-cs", "geran-ps", "cdma2000-1XRTT", "nr", "eutra-nr", "spare1"};
  return convert_enum_idx(options, 8, value, "rat_type_e");
}
uint16_t rat_type_opts::to_number() const
{
  if (value == cdma2000_minus1_xrtt) {
    return 2000;
  }
  invalid_enum_number(value, "rat_type_e");
  return 0;
}

std::string rrc_conn_release_v920_ies_s::cell_info_list_r9_c_::types_opts::to_string() const
{
  static const char* options[] = {"geran-r9", "utra-FDD-r9", "utra-TDD-r9", "utra-TDD-r10"};
  return convert_enum_idx(options, 4, value, "rrc_conn_release_v920_ies_s::cell_info_list_r9_c_::types");
}

std::string si_or_psi_geran_c::types_opts::to_string() const
{
  static const char* options[] = {"si", "psi"};
  return convert_enum_idx(options, 2, value, "si_or_psi_geran_c::types");
}

std::string area_cfg_r10_c::types_opts::to_string() const
{
  static const char* options[] = {"cellGlobalIdList-r10", "trackingAreaCodeList-r10"};
  return convert_enum_idx(options, 2, value, "area_cfg_r10_c::types");
}

std::string cell_change_order_s::t304_opts::to_string() const
{
  static const char* options[] = {"ms100", "ms200", "ms500", "ms1000", "ms2000", "ms4000", "ms8000", "ms10000-v1310"};
  return convert_enum_idx(options, 8, value, "cell_change_order_s::t304_e_");
}
uint16_t cell_change_order_s::t304_opts::to_number() const
{
  static const uint16_t options[] = {100, 200, 500, 1000, 2000, 4000, 8000, 10000};
  return map_enum_number(options, 8, value, "cell_change_order_s::t304_e_");
}

std::string cell_change_order_s::target_rat_type_c_::types_opts::to_string() const
{
  static const char* options[] = {"geran"};
  return convert_enum_idx(options, 1, value, "cell_change_order_s::target_rat_type_c_::types");
}

std::string e_csfb_r9_s::mob_cdma2000_hrpd_r9_opts::to_string() const
{
  static const char* options[] = {"handover", "redirection"};
  return convert_enum_idx(options, 2, value, "e_csfb_r9_s::mob_cdma2000_hrpd_r9_e_");
}

std::string ho_s::target_rat_type_opts::to_string() const
{
  static const char* options[] = {
      "utra", "geran", "cdma2000-1XRTT", "cdma2000-HRPD", "nr", "eutra", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "ho_s::target_rat_type_e_");
}

// LoggingDuration-r10 ::= ENUMERATED
std::string logging_dur_r10_opts::to_string() const
{
  static const char* options[] = {"min10", "min20", "min40", "min60", "min90", "min120", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "logging_dur_r10_e");
}
uint8_t logging_dur_r10_opts::to_number() const
{
  static const uint8_t options[] = {10, 20, 40, 60, 90, 120};
  return map_enum_number(options, 6, value, "logging_dur_r10_e");
}

// LoggingInterval-r10 ::= ENUMERATED
std::string logging_interv_r10_opts::to_string() const
{
  static const char* options[] = {"ms1280", "ms2560", "ms5120", "ms10240", "ms20480", "ms30720", "ms40960", "ms61440"};
  return convert_enum_idx(options, 8, value, "logging_interv_r10_e");
}
uint16_t logging_interv_r10_opts::to_number() const
{
  static const uint16_t options[] = {1280, 2560, 5120, 10240, 20480, 30720, 40960, 61440};
  return map_enum_number(options, 8, value, "logging_interv_r10_e");
}

std::string mob_ctrl_info_s::t304_opts::to_string() const
{
  static const char* options[] = {"ms50", "ms100", "ms150", "ms200", "ms500", "ms1000", "ms2000", "ms10000-v1310"};
  return convert_enum_idx(options, 8, value, "mob_ctrl_info_s::t304_e_");
}
uint16_t mob_ctrl_info_s::t304_opts::to_number() const
{
  static const uint16_t options[] = {50, 100, 150, 200, 500, 1000, 2000, 10000};
  return map_enum_number(options, 8, value, "mob_ctrl_info_s::t304_e_");
}

std::string mob_ctrl_info_s::ho_without_wt_change_r14_opts::to_string() const
{
  static const char* options[] = {"keepLWA-Config", "sendEndMarker"};
  return convert_enum_idx(options, 2, value, "mob_ctrl_info_s::ho_without_wt_change_r14_e_");
}

std::string rn_sf_cfg_r10_s::sf_cfg_pattern_r10_c_::types_opts::to_string() const
{
  static const char* options[] = {"subframeConfigPatternFDD-r10", "subframeConfigPatternTDD-r10"};
  return convert_enum_idx(options, 2, value, "rn_sf_cfg_r10_s::sf_cfg_pattern_r10_c_::types");
}

std::string rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_alloc_type_r10_opts::to_string() const
{
  static const char* options[] = {
      "type0", "type1", "type2Localized", "type2Distributed", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_alloc_type_r10_e_");
}

std::string rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_::types_opts::to_string() const
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

std::string rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_::types_opts::to_string() const
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

std::string rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::types_opts::to_string() const
{
  static const char* options[] = {"type01-r10", "type2-r10"};
  return convert_enum_idx(options, 2, value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::types");
}
float rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::types_opts::to_number() const
{
  static const float options[] = {0.1, 2.0};
  return map_enum_number(options, 2, value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::types");
}
std::string rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::types_opts::to_number_string() const
{
  static const char* options[] = {"0.1", "2"};
  return convert_enum_idx(options, 2, value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::types");
}

std::string rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::demod_rs_r10_c_::no_interleaving_r10_opts::to_string() const
{
  static const char* options[] = {"crs", "dmrs"};
  return convert_enum_idx(
      options, 2, value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::demod_rs_r10_c_::no_interleaving_r10_e_");
}

std::string rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::demod_rs_r10_c_::types_opts::to_string() const
{
  static const char* options[] = {"interleaving-r10", "noInterleaving-r10"};
  return convert_enum_idx(options, 2, value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::demod_rs_r10_c_::types");
}

std::string rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_::types_opts::to_string() const
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

std::string rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::types_opts::to_string() const
{
  static const char* options[] = {"tdd", "fdd"};
  return convert_enum_idx(options, 2, value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::types");
}

std::string redirected_carrier_info_c::types_opts::to_string() const
{
  static const char* options[] = {
      "eutra", "geran", "utra-FDD", "utra-TDD", "cdma2000-HRPD", "cdma2000-1xRTT", "utra-TDD-r10", "nr-r15"};
  return convert_enum_idx(options, 8, value, "redirected_carrier_info_c::types");
}

// ReleaseCause ::= ENUMERATED
std::string release_cause_opts::to_string() const
{
  static const char* options[] = {
      "loadBalancingTAUrequired", "other", "cs-FallbackHighPriority-v1020", "rrc-Suspend-v1320"};
  return convert_enum_idx(options, 4, value, "release_cause_e");
}

std::string security_cfg_ho_s::handov_type_c_::types_opts::to_string() const
{
  static const char* options[] = {"intraLTE", "interRAT"};
  return convert_enum_idx(options, 2, value, "security_cfg_ho_s::handov_type_c_::types");
}

std::string dl_info_transfer_r15_ies_s::ded_info_type_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "dedicatedInfoNAS-r15", "dedicatedInfoCDMA2000-1XRTT-r15", "dedicatedInfoCDMA2000-HRPD-r15"};
  return convert_enum_idx(options, 3, value, "dl_info_transfer_r15_ies_s::ded_info_type_r15_c_::types");
}

std::string dl_info_transfer_r8_ies_s::ded_info_type_c_::types_opts::to_string() const
{
  static const char* options[] = {"dedicatedInfoNAS", "dedicatedInfoCDMA2000-1XRTT", "dedicatedInfoCDMA2000-HRPD"};
  return convert_enum_idx(options, 3, value, "dl_info_transfer_r8_ies_s::ded_info_type_c_::types");
}

std::string mob_from_eutra_cmd_r8_ies_s::purpose_c_::types_opts::to_string() const
{
  static const char* options[] = {"handover", "cellChangeOrder"};
  return convert_enum_idx(options, 2, value, "mob_from_eutra_cmd_r8_ies_s::purpose_c_::types");
}

std::string mob_from_eutra_cmd_r9_ies_s::purpose_c_::types_opts::to_string() const
{
  static const char* options[] = {"handover", "cellChangeOrder", "e-CSFB-r9"};
  return convert_enum_idx(options, 3, value, "mob_from_eutra_cmd_r9_ies_s::purpose_c_::types");
}

std::string csfb_params_resp_cdma2000_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"csfbParametersResponseCDMA2000-r8", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "csfb_params_resp_cdma2000_s::crit_exts_c_::types");
}
uint16_t csfb_params_resp_cdma2000_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint16_t options[] = {2000};
  return map_enum_number(options, 1, value, "csfb_params_resp_cdma2000_s::crit_exts_c_::types");
}

std::string counter_check_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"counterCheck-r8", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "counter_check_s::crit_exts_c_::c1_c_::types");
}

std::string dl_info_transfer_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"dlInformationTransfer-r8", "dlInformationTransfer-r15", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "dl_info_transfer_s::crit_exts_c_::c1_c_::types");
}

std::string ho_from_eutra_prep_request_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"handoverFromEUTRAPreparationRequest-r8", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ho_from_eutra_prep_request_s::crit_exts_c_::c1_c_::types");
}

std::string logged_meas_cfg_r10_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"loggedMeasurementConfiguration-r10", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "logged_meas_cfg_r10_s::crit_exts_c_::c1_c_::types");
}

std::string mob_from_eutra_cmd_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"mobilityFromEUTRACommand-r8", "mobilityFromEUTRACommand-r9", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "mob_from_eutra_cmd_s::crit_exts_c_::c1_c_::types");
}

std::string rn_recfg_r10_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"rnReconfiguration-r10", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "rn_recfg_r10_s::crit_exts_c_::c1_c_::types");
}

std::string rrc_conn_recfg_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "rrcConnectionReconfiguration-r8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "rrc_conn_recfg_s::crit_exts_c_::c1_c_::types");
}

std::string rrc_conn_release_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionRelease-r8", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "rrc_conn_release_s::crit_exts_c_::c1_c_::types");
}

std::string rrc_conn_resume_r13_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionResume-r13", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "rrc_conn_resume_r13_s::crit_exts_c_::c1_c_::types");
}

std::string security_mode_cmd_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"securityModeCommand-r8", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "security_mode_cmd_s::crit_exts_c_::c1_c_::types");
}

std::string ue_cap_enquiry_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"ueCapabilityEnquiry-r8", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ue_cap_enquiry_s::crit_exts_c_::c1_c_::types");
}

std::string ue_info_request_r9_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"ueInformationRequest-r9", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ue_info_request_r9_s::crit_exts_c_::c1_c_::types");
}

std::string dl_dcch_msg_type_c::c1_c_::types_opts::to_string() const
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

std::string dl_dcch_msg_type_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "dl_dcch_msg_type_c::types");
}
uint8_t dl_dcch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "dl_dcch_msg_type_c::types");
}

std::string tmgi_r9_s::plmn_id_r9_c_::types_opts::to_string() const
{
  static const char* options[] = {"plmn-Index-r9", "explicitValue-r9"};
  return convert_enum_idx(options, 2, value, "tmgi_r9_s::plmn_id_r9_c_::types");
}

std::string pmch_cfg_r12_s::data_mcs_r12_c_::types_opts::to_string() const
{
  static const char* options[] = {"normal-r12", "higerOrder-r12"};
  return convert_enum_idx(options, 2, value, "pmch_cfg_r12_s::data_mcs_r12_c_::types");
}

std::string pmch_cfg_r12_s::mch_sched_period_r12_opts::to_string() const
{
  static const char* options[] = {"rf4", "rf8", "rf16", "rf32", "rf64", "rf128", "rf256", "rf512", "rf1024"};
  return convert_enum_idx(options, 9, value, "pmch_cfg_r12_s::mch_sched_period_r12_e_");
}
uint16_t pmch_cfg_r12_s::mch_sched_period_r12_opts::to_number() const
{
  static const uint16_t options[] = {4, 8, 16, 32, 64, 128, 256, 512, 1024};
  return map_enum_number(options, 9, value, "pmch_cfg_r12_s::mch_sched_period_r12_e_");
}

std::string pmch_cfg_r12_s::mch_sched_period_v1430_opts::to_string() const
{
  static const char* options[] = {"rf1", "rf2"};
  return convert_enum_idx(options, 2, value, "pmch_cfg_r12_s::mch_sched_period_v1430_e_");
}
uint8_t pmch_cfg_r12_s::mch_sched_period_v1430_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "pmch_cfg_r12_s::mch_sched_period_v1430_e_");
}

std::string pmch_cfg_r9_s::mch_sched_period_r9_opts::to_string() const
{
  static const char* options[] = {"rf8", "rf16", "rf32", "rf64", "rf128", "rf256", "rf512", "rf1024"};
  return convert_enum_idx(options, 8, value, "pmch_cfg_r9_s::mch_sched_period_r9_e_");
}
uint16_t pmch_cfg_r9_s::mch_sched_period_r9_opts::to_number() const
{
  static const uint16_t options[] = {8, 16, 32, 64, 128, 256, 512, 1024};
  return map_enum_number(options, 8, value, "pmch_cfg_r9_s::mch_sched_period_r9_e_");
}

std::string mbsfn_area_cfg_r9_s::common_sf_alloc_period_r9_opts::to_string() const
{
  static const char* options[] = {"rf4", "rf8", "rf16", "rf32", "rf64", "rf128", "rf256"};
  return convert_enum_idx(options, 7, value, "mbsfn_area_cfg_r9_s::common_sf_alloc_period_r9_e_");
}
uint16_t mbsfn_area_cfg_r9_s::common_sf_alloc_period_r9_opts::to_number() const
{
  static const uint16_t options[] = {4, 8, 16, 32, 64, 128, 256};
  return map_enum_number(options, 7, value, "mbsfn_area_cfg_r9_s::common_sf_alloc_period_r9_e_");
}

std::string mcch_msg_type_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"mbsfnAreaConfiguration-r9"};
  return convert_enum_idx(options, 1, value, "mcch_msg_type_c::c1_c_::types");
}

std::string mcch_msg_type_c::later_c_::c2_c_::types_opts::to_string() const
{
  static const char* options[] = {"mbmsCountingRequest-r10"};
  return convert_enum_idx(options, 1, value, "mcch_msg_type_c::later_c_::c2_c_::types");
}

std::string mcch_msg_type_c::later_c_::types_opts::to_string() const
{
  static const char* options[] = {"c2", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "mcch_msg_type_c::later_c_::types");
}
uint8_t mcch_msg_type_c::later_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {2};
  return map_enum_number(options, 1, value, "mcch_msg_type_c::later_c_::types");
}

std::string mcch_msg_type_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "later"};
  return convert_enum_idx(options, 2, value, "mcch_msg_type_c::types");
}
uint8_t mcch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "mcch_msg_type_c::types");
}

std::string paging_ue_id_c::types_opts::to_string() const
{
  static const char* options[] = {"s-TMSI", "imsi", "ng-5G-S-TMSI-r15", "i-RNTI-r15"};
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

std::string paging_record_s::cn_domain_opts::to_string() const
{
  static const char* options[] = {"ps", "cs"};
  return convert_enum_idx(options, 2, value, "paging_record_s::cn_domain_e_");
}

std::string pcch_msg_type_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"paging"};
  return convert_enum_idx(options, 1, value, "pcch_msg_type_c::c1_c_::types");
}

std::string pcch_msg_type_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "pcch_msg_type_c::types");
}
uint8_t pcch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "pcch_msg_type_c::types");
}

std::string sc_mtch_sched_info_br_r14_s::on_dur_timer_scptm_r14_opts::to_string() const
{
  static const char* options[] = {"psf300", "psf400", "psf500", "psf600", "psf800", "psf1000", "psf1200", "psf1600"};
  return convert_enum_idx(options, 8, value, "sc_mtch_sched_info_br_r14_s::on_dur_timer_scptm_r14_e_");
}
uint16_t sc_mtch_sched_info_br_r14_s::on_dur_timer_scptm_r14_opts::to_number() const
{
  static const uint16_t options[] = {300, 400, 500, 600, 800, 1000, 1200, 1600};
  return map_enum_number(options, 8, value, "sc_mtch_sched_info_br_r14_s::on_dur_timer_scptm_r14_e_");
}

std::string sc_mtch_sched_info_br_r14_s::drx_inactivity_timer_scptm_r14_opts::to_string() const
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

std::string sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::types_opts::to_string() const
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

std::string sc_mtch_sched_info_r13_s::on_dur_timer_scptm_r13_opts::to_string() const
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

std::string sc_mtch_sched_info_r13_s::drx_inactivity_timer_scptm_r13_opts::to_string() const
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

std::string sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::types_opts::to_string() const
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

std::string sc_mtch_info_br_r14_s::mpdcch_num_repeat_sc_mtch_r14_opts::to_string() const
{
  static const char* options[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "r128", "r256"};
  return convert_enum_idx(options, 9, value, "sc_mtch_info_br_r14_s::mpdcch_num_repeat_sc_mtch_r14_e_");
}
uint16_t sc_mtch_info_br_r14_s::mpdcch_num_repeat_sc_mtch_r14_opts::to_number() const
{
  static const uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
  return map_enum_number(options, 9, value, "sc_mtch_info_br_r14_s::mpdcch_num_repeat_sc_mtch_r14_e_");
}

std::string sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::fdd_r14_opts::to_string() const
{
  static const char* options[] = {"v1", "v1dot5", "v2", "v2dot5", "v4", "v5", "v8", "v10"};
  return convert_enum_idx(options, 8, value, "sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::fdd_r14_e_");
}
float sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::fdd_r14_opts::to_number() const
{
  static const float options[] = {1.0, 1.5, 2.0, 2.5, 4.0, 5.0, 8.0, 10.0};
  return map_enum_number(options, 8, value, "sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::fdd_r14_e_");
}
std::string sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::fdd_r14_opts::to_number_string() const
{
  static const char* options[] = {"1", "1.5", "2", "2.5", "4", "5", "8", "10"};
  return convert_enum_idx(options, 8, value, "sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::fdd_r14_e_");
}

std::string sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::tdd_r14_opts::to_string() const
{
  static const char* options[] = {"v1", "v2", "v4", "v5", "v8", "v10", "v20"};
  return convert_enum_idx(options, 7, value, "sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::tdd_r14_e_");
}
uint8_t sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::tdd_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 5, 8, 10, 20};
  return map_enum_number(options, 7, value, "sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::tdd_r14_e_");
}

std::string sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::types_opts::to_string() const
{
  static const char* options[] = {"fdd-r14", "tdd-r14"};
  return convert_enum_idx(options, 2, value, "sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::types");
}

std::string sc_mtch_info_br_r14_s::mpdcch_pdsch_hop_cfg_sc_mtch_r14_opts::to_string() const
{
  static const char* options[] = {"on", "off"};
  return convert_enum_idx(options, 2, value, "sc_mtch_info_br_r14_s::mpdcch_pdsch_hop_cfg_sc_mtch_r14_e_");
}

std::string sc_mtch_info_br_r14_s::mpdcch_pdsch_cemode_cfg_sc_mtch_r14_opts::to_string() const
{
  static const char* options[] = {"ce-ModeA", "ce-ModeB"};
  return convert_enum_idx(options, 2, value, "sc_mtch_info_br_r14_s::mpdcch_pdsch_cemode_cfg_sc_mtch_r14_e_");
}

std::string sc_mtch_info_br_r14_s::mpdcch_pdsch_max_bw_sc_mtch_r14_opts::to_string() const
{
  static const char* options[] = {"bw1dot4", "bw5"};
  return convert_enum_idx(options, 2, value, "sc_mtch_info_br_r14_s::mpdcch_pdsch_max_bw_sc_mtch_r14_e_");
}
float sc_mtch_info_br_r14_s::mpdcch_pdsch_max_bw_sc_mtch_r14_opts::to_number() const
{
  static const float options[] = {1.4, 5.0};
  return map_enum_number(options, 2, value, "sc_mtch_info_br_r14_s::mpdcch_pdsch_max_bw_sc_mtch_r14_e_");
}
std::string sc_mtch_info_br_r14_s::mpdcch_pdsch_max_bw_sc_mtch_r14_opts::to_number_string() const
{
  static const char* options[] = {"1.4", "5"};
  return convert_enum_idx(options, 2, value, "sc_mtch_info_br_r14_s::mpdcch_pdsch_max_bw_sc_mtch_r14_e_");
}

std::string sc_mtch_info_br_r14_s::mpdcch_offset_sc_mtch_r14_opts::to_string() const
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
std::string sc_mtch_info_br_r14_s::mpdcch_offset_sc_mtch_r14_opts::to_number_string() const
{
  static const char* options[] = {"0", "1/8", "1/4", "3/8", "1/2", "5/8", "3/4", "7/8"};
  return convert_enum_idx(options, 8, value, "sc_mtch_info_br_r14_s::mpdcch_offset_sc_mtch_r14_e_");
}

std::string sc_mtch_info_br_r14_s::p_a_r14_opts::to_string() const
{
  static const char* options[] = {"dB-6", "dB-4dot77", "dB-3", "dB-1dot77", "dB0", "dB1", "dB2", "dB3"};
  return convert_enum_idx(options, 8, value, "sc_mtch_info_br_r14_s::p_a_r14_e_");
}
float sc_mtch_info_br_r14_s::p_a_r14_opts::to_number() const
{
  static const float options[] = {-6.0, -4.77, -3.0, -1.77, 0.0, 1.0, 2.0, 3.0};
  return map_enum_number(options, 8, value, "sc_mtch_info_br_r14_s::p_a_r14_e_");
}
std::string sc_mtch_info_br_r14_s::p_a_r14_opts::to_number_string() const
{
  static const char* options[] = {"-6", "-4.77", "-3", "-1.77", "0", "1", "2", "3"};
  return convert_enum_idx(options, 8, value, "sc_mtch_info_br_r14_s::p_a_r14_e_");
}

std::string sc_mtch_info_r13_s::p_a_r13_opts::to_string() const
{
  static const char* options[] = {"dB-6", "dB-4dot77", "dB-3", "dB-1dot77", "dB0", "dB1", "dB2", "dB3"};
  return convert_enum_idx(options, 8, value, "sc_mtch_info_r13_s::p_a_r13_e_");
}
float sc_mtch_info_r13_s::p_a_r13_opts::to_number() const
{
  static const float options[] = {-6.0, -4.77, -3.0, -1.77, 0.0, 1.0, 2.0, 3.0};
  return map_enum_number(options, 8, value, "sc_mtch_info_r13_s::p_a_r13_e_");
}
std::string sc_mtch_info_r13_s::p_a_r13_opts::to_number_string() const
{
  static const char* options[] = {"-6", "-4.77", "-3", "-1.77", "0", "1", "2", "3"};
  return convert_enum_idx(options, 8, value, "sc_mtch_info_r13_s::p_a_r13_e_");
}

std::string sc_mcch_msg_type_r13_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"scptmConfiguration-r13"};
  return convert_enum_idx(options, 1, value, "sc_mcch_msg_type_r13_c::c1_c_::types");
}

std::string sc_mcch_msg_type_r13_c::msg_class_ext_c_::c2_c_::types_opts::to_string() const
{
  static const char* options[] = {"scptmConfiguration-BR-r14", "spare"};
  return convert_enum_idx(options, 2, value, "sc_mcch_msg_type_r13_c::msg_class_ext_c_::c2_c_::types");
}

std::string sc_mcch_msg_type_r13_c::msg_class_ext_c_::types_opts::to_string() const
{
  static const char* options[] = {"c2", "messageClassExtensionFuture-r14"};
  return convert_enum_idx(options, 2, value, "sc_mcch_msg_type_r13_c::msg_class_ext_c_::types");
}
uint8_t sc_mcch_msg_type_r13_c::msg_class_ext_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {2};
  return map_enum_number(options, 1, value, "sc_mcch_msg_type_r13_c::msg_class_ext_c_::types");
}

std::string sc_mcch_msg_type_r13_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "sc_mcch_msg_type_r13_c::types");
}
uint8_t sc_mcch_msg_type_r13_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "sc_mcch_msg_type_r13_c::types");
}

// EstablishmentCause ::= ENUMERATED
std::string establishment_cause_opts::to_string() const
{
  static const char* options[] = {"emergency",
                                  "highPriorityAccess",
                                  "mt-Access",
                                  "mo-Signalling",
                                  "mo-Data",
                                  "delayTolerantAccess-v1020",
                                  "mo-VoiceCall-v1280",
                                  "spare1"};
  return convert_enum_idx(options, 8, value, "establishment_cause_e");
}

std::string init_ue_id_c::types_opts::to_string() const
{
  static const char* options[] = {"s-TMSI", "randomValue"};
  return convert_enum_idx(options, 2, value, "init_ue_id_c::types");
}

// ReestablishmentCause ::= ENUMERATED
std::string reest_cause_opts::to_string() const
{
  static const char* options[] = {"reconfigurationFailure", "handoverFailure", "otherFailure", "spare1"};
  return convert_enum_idx(options, 4, value, "reest_cause_e");
}

// ResumeCause ::= ENUMERATED
std::string resume_cause_opts::to_string() const
{
  static const char* options[] = {"emergency",
                                  "highPriorityAccess",
                                  "mt-Access",
                                  "mo-Signalling",
                                  "mo-Data",
                                  "delayTolerantAccess-v1020",
                                  "mo-VoiceCall-v1280",
                                  "spare1"};
  return convert_enum_idx(options, 8, value, "resume_cause_e");
}

// ResumeCause-r15 ::= ENUMERATED
std::string resume_cause_r15_opts::to_string() const
{
  static const char* options[] = {"emergency",
                                  "highPriorityAccess",
                                  "mt-Access",
                                  "mo-Signalling",
                                  "mo-Data",
                                  "rna-Update",
                                  "mo-VoiceCall",
                                  "spare1"};
  return convert_enum_idx(options, 8, value, "resume_cause_r15_e");
}

std::string rrc_conn_resume_request_minus5_gc_r15_ies_s::resume_id_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"fullI-RNTI-r15", "shortI-RNTI-r15"};
  return convert_enum_idx(options, 2, value, "rrc_conn_resume_request_minus5_gc_r15_ies_s::resume_id_r15_c_::types");
}

std::string rrc_conn_resume_request_r13_ies_s::resume_id_r13_c_::types_opts::to_string() const
{
  static const char* options[] = {"resumeID-r13", "truncatedResumeID-r13"};
  return convert_enum_idx(options, 2, value, "rrc_conn_resume_request_r13_ies_s::resume_id_r13_c_::types");
}

std::string rrc_early_data_request_r15_ies_s::establishment_cause_r15_opts::to_string() const
{
  static const char* options[] = {"mo-Data-r15", "delayTolerantAccess-r15"};
  return convert_enum_idx(options, 2, value, "rrc_early_data_request_r15_ies_s::establishment_cause_r15_e_");
}

std::string rrc_conn_reest_request_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionReestablishmentRequest-r8", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_conn_reest_request_s::crit_exts_c_::types");
}

std::string rrc_conn_request_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionRequest-r8", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_conn_request_s::crit_exts_c_::types");
}

std::string rrc_conn_resume_request_r13_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionResumeRequest-r13", "rrcConnectionResumeRequest-r15"};
  return convert_enum_idx(options, 2, value, "rrc_conn_resume_request_r13_s::crit_exts_c_::types");
}

std::string rrc_early_data_request_r15_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcEarlyDataRequest-r15", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_early_data_request_r15_s::crit_exts_c_::types");
}

std::string ul_ccch_msg_type_c::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionReestablishmentRequest", "rrcConnectionRequest"};
  return convert_enum_idx(options, 2, value, "ul_ccch_msg_type_c::c1_c_::types");
}

std::string ul_ccch_msg_type_c::msg_class_ext_c_::c2_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionResumeRequest-r13"};
  return convert_enum_idx(options, 1, value, "ul_ccch_msg_type_c::msg_class_ext_c_::c2_c_::types");
}

std::string ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::c3_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcEarlyDataRequest-r15", "spare3", "spare2", "spare1"};
  return convert_enum_idx(
      options, 4, value, "ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::c3_c_::types");
}

std::string ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::types_opts::to_string() const
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

std::string ul_ccch_msg_type_c::msg_class_ext_c_::types_opts::to_string() const
{
  static const char* options[] = {"c2", "messageClassExtensionFuture-r13"};
  return convert_enum_idx(options, 2, value, "ul_ccch_msg_type_c::msg_class_ext_c_::types");
}
uint8_t ul_ccch_msg_type_c::msg_class_ext_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {2};
  return map_enum_number(options, 1, value, "ul_ccch_msg_type_c::msg_class_ext_c_::types");
}

std::string ul_ccch_msg_type_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "ul_ccch_msg_type_c::types");
}
uint8_t ul_ccch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "ul_ccch_msg_type_c::types");
}

std::string cell_global_id_cdma2000_c::types_opts::to_string() const
{
  static const char* options[] = {"cellGlobalId1XRTT", "cellGlobalIdHRPD"};
  return convert_enum_idx(options, 2, value, "cell_global_id_cdma2000_c::types");
}
uint8_t cell_global_id_cdma2000_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "cell_global_id_cdma2000_c::types");
}

std::string meas_result_utra_s::pci_c_::types_opts::to_string() const
{
  static const char* options[] = {"fdd", "tdd"};
  return convert_enum_idx(options, 2, value, "meas_result_utra_s::pci_c_::types");
}

std::string s_nssai_r15_c::types_opts::to_string() const
{
  static const char* options[] = {"sst", "sst-SD"};
  return convert_enum_idx(options, 2, value, "s_nssai_r15_c::types");
}

std::string location_info_r10_s::location_coordinates_r10_c_::types_opts::to_string() const
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

std::string location_info_r10_s::vertical_velocity_info_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"verticalVelocity-r15", "verticalVelocityAndUncertainty-r15"};
  return convert_enum_idx(options, 2, value, "location_info_r10_s::vertical_velocity_info_r15_c_::types");
}

std::string rrc_conn_setup_complete_v1530_ies_s::ng_minus5_g_s_tmsi_bits_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"ng-5G-S-TMSI-r15", "ng-5G-S-TMSI-Part2-r15"};
  return convert_enum_idx(
      options, 2, value, "rrc_conn_setup_complete_v1530_ies_s::ng_minus5_g_s_tmsi_bits_r15_c_::types");
}

std::string wlan_rtt_r15_s::rtt_units_r15_opts::to_string() const
{
  static const char* options[] = {
      "microseconds", "hundredsofnanoseconds", "tensofnanoseconds", "nanoseconds", "tenthsofnanoseconds"};
  return convert_enum_idx(options, 5, value, "wlan_rtt_r15_s::rtt_units_r15_e_");
}

std::string meas_result_idle_r15_s::meas_result_neigh_cells_r15_c_::types_opts::to_string() const
{
  static const char* options[] = {"measResultIdleListEUTRA-r15"};
  return convert_enum_idx(options, 1, value, "meas_result_idle_r15_s::meas_result_neigh_cells_r15_c_::types");
}

std::string per_cc_gap_ind_r14_s::gap_ind_r14_opts::to_string() const
{
  static const char* options[] = {"gap", "ncsg", "nogap-noNcsg"};
  return convert_enum_idx(options, 3, value, "per_cc_gap_ind_r14_s::gap_ind_r14_e_");
}

std::string visited_cell_info_r12_s::visited_cell_id_r12_c_::types_opts::to_string() const
{
  static const char* options[] = {"cellGlobalId-r12", "pci-arfcn-r12"};
  return convert_enum_idx(options, 2, value, "visited_cell_info_r12_s::visited_cell_id_r12_c_::types");
}

std::string affected_carrier_freq_comb_info_mrdc_r15_s::interference_direction_mrdc_r15_opts::to_string() const
{
  static const char* options[] = {
      "eutra-nr", "nr", "other", "eutra-nr-other", "nr-other", "spare3", "spare2", "spare1"};
  return convert_enum_idx(
      options, 8, value, "affected_carrier_freq_comb_info_mrdc_r15_s::interference_direction_mrdc_r15_e_");
}

std::string rrc_conn_setup_complete_v1250_ies_s::mob_state_r12_opts::to_string() const
{
  static const char* options[] = {"normal", "medium", "high", "spare"};
  return convert_enum_idx(options, 4, value, "rrc_conn_setup_complete_v1250_ies_s::mob_state_r12_e_");
}

std::string idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_::types_opts::to_string() const
{
  static const char* options[] = {"subframeConfig0-r11", "subframeConfig1-5-r11", "subframeConfig6-r11"};
  return convert_enum_idx(options, 3, value, "idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_::types");
}
uint8_t idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 6};
  return map_enum_number(options, 3, value, "idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_::types");
}

std::string idc_sf_pattern_r11_c::types_opts::to_string() const
{
  static const char* options[] = {"subframePatternFDD-r11", "subframePatternTDD-r11"};
  return convert_enum_idx(options, 2, value, "idc_sf_pattern_r11_c::types");
}

std::string sl_disc_sys_info_report_r13_s::cell_resel_info_r13_s_::q_hyst_r13_opts::to_string() const
{
  static const char* options[] = {"dB0",
                                  "dB1",
                                  "dB2",
                                  "dB3",
                                  "dB4",
                                  "dB5",
                                  "dB6",
                                  "dB8",
                                  "dB10",
                                  "dB12",
                                  "dB14",
                                  "dB16",
                                  "dB18",
                                  "dB20",
                                  "dB22",
                                  "dB24"};
  return convert_enum_idx(options, 16, value, "sl_disc_sys_info_report_r13_s::cell_resel_info_r13_s_::q_hyst_r13_e_");
}
uint8_t sl_disc_sys_info_report_r13_s::cell_resel_info_r13_s_::q_hyst_r13_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24};
  return map_enum_number(options, 16, value, "sl_disc_sys_info_report_r13_s::cell_resel_info_r13_s_::q_hyst_r13_e_");
}

std::string sl_disc_sys_info_report_r13_s::freq_info_r13_s_::ul_bw_r13_opts::to_string() const
{
  static const char* options[] = {"n6", "n15", "n25", "n50", "n75", "n100"};
  return convert_enum_idx(options, 6, value, "sl_disc_sys_info_report_r13_s::freq_info_r13_s_::ul_bw_r13_e_");
}
uint8_t sl_disc_sys_info_report_r13_s::freq_info_r13_s_::ul_bw_r13_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(options, 6, value, "sl_disc_sys_info_report_r13_s::freq_info_r13_s_::ul_bw_r13_e_");
}

std::string traffic_pattern_info_r14_s::traffic_periodicity_r14_opts::to_string() const
{
  static const char* options[] = {
      "sf20", "sf50", "sf100", "sf200", "sf300", "sf400", "sf500", "sf600", "sf700", "sf800", "sf900", "sf1000"};
  return convert_enum_idx(options, 12, value, "traffic_pattern_info_r14_s::traffic_periodicity_r14_e_");
}
uint16_t traffic_pattern_info_r14_s::traffic_periodicity_r14_opts::to_number() const
{
  static const uint16_t options[] = {20, 50, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
  return map_enum_number(options, 12, value, "traffic_pattern_info_r14_s::traffic_periodicity_r14_e_");
}

std::string ue_radio_paging_info_r12_s::wake_up_signal_min_gap_e_drx_r15_opts::to_string() const
{
  static const char* options[] = {"ms40", "ms240", "ms1000", "ms2000"};
  return convert_enum_idx(options, 4, value, "ue_radio_paging_info_r12_s::wake_up_signal_min_gap_e_drx_r15_e_");
}
uint16_t ue_radio_paging_info_r12_s::wake_up_signal_min_gap_e_drx_r15_opts::to_number() const
{
  static const uint16_t options[] = {40, 240, 1000, 2000};
  return map_enum_number(options, 4, value, "ue_radio_paging_info_r12_s::wake_up_signal_min_gap_e_drx_r15_e_");
}

std::string ue_radio_paging_info_r12_s::wake_up_signal_min_gap_e_drx_tdd_r15_opts::to_string() const
{
  static const char* options[] = {"ms40", "ms240", "ms1000", "ms2000"};
  return convert_enum_idx(options, 4, value, "ue_radio_paging_info_r12_s::wake_up_signal_min_gap_e_drx_tdd_r15_e_");
}
uint16_t ue_radio_paging_info_r12_s::wake_up_signal_min_gap_e_drx_tdd_r15_opts::to_number() const
{
  static const uint16_t options[] = {40, 240, 1000, 2000};
  return map_enum_number(options, 4, value, "ue_radio_paging_info_r12_s::wake_up_signal_min_gap_e_drx_tdd_r15_e_");
}

std::string ul_pdcp_delay_result_r13_s::qci_id_r13_opts::to_string() const
{
  static const char* options[] = {"qci1", "qci2", "qci3", "qci4", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "ul_pdcp_delay_result_r13_s::qci_id_r13_e_");
}
uint8_t ul_pdcp_delay_result_r13_s::qci_id_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4};
  return map_enum_number(options, 4, value, "ul_pdcp_delay_result_r13_s::qci_id_r13_e_");
}

std::string affected_carrier_freq_r11_s::interference_direction_r11_opts::to_string() const
{
  static const char* options[] = {"eutra", "other", "both", "spare"};
  return convert_enum_idx(options, 4, value, "affected_carrier_freq_r11_s::interference_direction_r11_e_");
}

std::string bw_pref_r14_s::dl_pref_r14_opts::to_string() const
{
  static const char* options[] = {"mhz1dot4", "mhz5", "mhz20"};
  return convert_enum_idx(options, 3, value, "bw_pref_r14_s::dl_pref_r14_e_");
}
float bw_pref_r14_s::dl_pref_r14_opts::to_number() const
{
  static const float options[] = {1.4, 5.0, 20.0};
  return map_enum_number(options, 3, value, "bw_pref_r14_s::dl_pref_r14_e_");
}
std::string bw_pref_r14_s::dl_pref_r14_opts::to_number_string() const
{
  static const char* options[] = {"1.4", "5", "20"};
  return convert_enum_idx(options, 3, value, "bw_pref_r14_s::dl_pref_r14_e_");
}

std::string bw_pref_r14_s::ul_pref_r14_opts::to_string() const
{
  static const char* options[] = {"mhz1dot4", "mhz5"};
  return convert_enum_idx(options, 2, value, "bw_pref_r14_s::ul_pref_r14_e_");
}
float bw_pref_r14_s::ul_pref_r14_opts::to_number() const
{
  static const float options[] = {1.4, 5.0};
  return map_enum_number(options, 2, value, "bw_pref_r14_s::ul_pref_r14_e_");
}
std::string bw_pref_r14_s::ul_pref_r14_opts::to_number_string() const
{
  static const char* options[] = {"1.4", "5"};
  return convert_enum_idx(options, 2, value, "bw_pref_r14_s::ul_pref_r14_e_");
}

std::string delay_budget_report_r14_c::type1_opts::to_string() const
{
  static const char* options[] = {"msMinus1280",
                                  "msMinus640",
                                  "msMinus320",
                                  "msMinus160",
                                  "msMinus80",
                                  "msMinus60",
                                  "msMinus40",
                                  "msMinus20",
                                  "ms0",
                                  "ms20",
                                  "ms40",
                                  "ms60",
                                  "ms80",
                                  "ms160",
                                  "ms320",
                                  "ms640",
                                  "ms1280"};
  return convert_enum_idx(options, 17, value, "delay_budget_report_r14_c::type1_e_");
}
int16_t delay_budget_report_r14_c::type1_opts::to_number() const
{
  static const int16_t options[] = {
      -1280, -640, -320, -160, -80, -60, -40, -20, 0, 20, 40, 60, 80, 160, 320, 640, 1280};
  return map_enum_number(options, 17, value, "delay_budget_report_r14_c::type1_e_");
}

std::string delay_budget_report_r14_c::type2_opts::to_string() const
{
  static const char* options[] = {"msMinus192",
                                  "msMinus168",
                                  "msMinus144",
                                  "msMinus120",
                                  "msMinus96",
                                  "msMinus72",
                                  "msMinus48",
                                  "msMinus24",
                                  "ms0",
                                  "ms24",
                                  "ms48",
                                  "ms72",
                                  "ms96",
                                  "ms120",
                                  "ms144",
                                  "ms168",
                                  "ms192"};
  return convert_enum_idx(options, 17, value, "delay_budget_report_r14_c::type2_e_");
}
int16_t delay_budget_report_r14_c::type2_opts::to_number() const
{
  static const int16_t options[] = {-192, -168, -144, -120, -96, -72, -48, -24, 0, 24, 48, 72, 96, 120, 144, 168, 192};
  return map_enum_number(options, 17, value, "delay_budget_report_r14_c::type2_e_");
}

std::string delay_budget_report_r14_c::types_opts::to_string() const
{
  static const char* options[] = {"type1", "type2"};
  return convert_enum_idx(options, 2, value, "delay_budget_report_r14_c::types");
}
uint8_t delay_budget_report_r14_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "delay_budget_report_r14_c::types");
}

std::string rrc_conn_setup_complete_v1020_ies_s::gummei_type_r10_opts::to_string() const
{
  static const char* options[] = {"native", "mapped"};
  return convert_enum_idx(options, 2, value, "rrc_conn_setup_complete_v1020_ies_s::gummei_type_r10_e_");
}

std::string rrc_conn_setup_complete_v1020_ies_s::rn_sf_cfg_req_r10_opts::to_string() const
{
  static const char* options[] = {"required", "notRequired"};
  return convert_enum_idx(options, 2, value, "rrc_conn_setup_complete_v1020_ies_s::rn_sf_cfg_req_r10_e_");
}

std::string rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::types_opts::to_string() const
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

// WLAN-Status-v1430 ::= ENUMERATED
std::string wlan_status_v1430_opts::to_string() const
{
  static const char* options[] = {"suspended", "resumed"};
  return convert_enum_idx(options, 2, value, "wlan_status_v1430_e");
}

std::string fail_report_scg_nr_r15_s::fail_type_r15_opts::to_string() const
{
  static const char* options[] = {"t310-Expiry",
                                  "randomAccessProblem",
                                  "rlc-MaxNumRetx",
                                  "scg-ChangeFailure",
                                  "scg-reconfigFailure",
                                  "srb3-IntegrityFailure"};
  return convert_enum_idx(options, 6, value, "fail_report_scg_nr_r15_s::fail_type_r15_e_");
}
uint16_t fail_report_scg_nr_r15_s::fail_type_r15_opts::to_number() const
{
  switch (value) {
    case t310_expiry:
      return 310;
    case srb3_integrity_fail:
      return 3;
    default:
      invalid_enum_number(value, "fail_report_scg_nr_r15_s::fail_type_r15_e_");
  }
  return 0;
}

std::string fail_report_scg_r12_s::fail_type_r12_opts::to_string() const
{
  static const char* options[] = {"t313-Expiry", "randomAccessProblem", "rlc-MaxNumRetx", "scg-ChangeFailure"};
  return convert_enum_idx(options, 4, value, "fail_report_scg_r12_s::fail_type_r12_e_");
}
uint16_t fail_report_scg_r12_s::fail_type_r12_opts::to_number() const
{
  static const uint16_t options[] = {313};
  return map_enum_number(options, 1, value, "fail_report_scg_r12_s::fail_type_r12_e_");
}

std::string meas_results_s::meas_result_neigh_cells_c_::types_opts::to_string() const
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

std::string rlf_report_r9_s::failed_pcell_id_r10_c_::types_opts::to_string() const
{
  static const char* options[] = {"cellGlobalId-r10", "pci-arfcn-r10"};
  return convert_enum_idx(options, 2, value, "rlf_report_r9_s::failed_pcell_id_r10_c_::types");
}

std::string rlf_report_r9_s::conn_fail_type_r10_opts::to_string() const
{
  static const char* options[] = {"rlf", "hof"};
  return convert_enum_idx(options, 2, value, "rlf_report_r9_s::conn_fail_type_r10_e_");
}

std::string rlf_report_r9_s::basic_fields_r11_s_::rlf_cause_r11_opts::to_string() const
{
  static const char* options[] = {"t310-Expiry", "randomAccessProblem", "rlc-MaxNumRetx", "t312-Expiry-r12"};
  return convert_enum_idx(options, 4, value, "rlf_report_r9_s::basic_fields_r11_s_::rlf_cause_r11_e_");
}
uint16_t rlf_report_r9_s::basic_fields_r11_s_::rlf_cause_r11_opts::to_number() const
{
  switch (value) {
    case t310_expiry:
      return 310;
    case t312_expiry_r12:
      return 312;
    default:
      invalid_enum_number(value, "rlf_report_r9_s::basic_fields_r11_s_::rlf_cause_r11_e_");
  }
  return 0;
}

std::string rlf_report_r9_s::prev_utra_cell_id_r11_s_::pci_r11_c_::types_opts::to_string() const
{
  static const char* options[] = {"fdd-r11", "tdd-r11"};
  return convert_enum_idx(options, 2, value, "rlf_report_r9_s::prev_utra_cell_id_r11_s_::pci_r11_c_::types");
}

std::string rlf_report_r9_s::sel_utra_cell_id_r11_s_::pci_r11_c_::types_opts::to_string() const
{
  static const char* options[] = {"fdd-r11", "tdd-r11"};
  return convert_enum_idx(options, 2, value, "rlf_report_r9_s::sel_utra_cell_id_r11_s_::pci_r11_c_::types");
}

std::string sidelink_ue_info_v1310_ies_s::comm_tx_res_info_req_relay_r13_s_::ue_type_r13_opts::to_string() const
{
  static const char* options[] = {"relayUE", "remoteUE"};
  return convert_enum_idx(
      options, 2, value, "sidelink_ue_info_v1310_ies_s::comm_tx_res_info_req_relay_r13_s_::ue_type_r13_e_");
}

std::string tdm_assist_info_r11_c::drx_assist_info_r11_s_::drx_cycle_len_r11_opts::to_string() const
{
  static const char* options[] = {"sf40", "sf64", "sf80", "sf128", "sf160", "sf256", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "tdm_assist_info_r11_c::drx_assist_info_r11_s_::drx_cycle_len_r11_e_");
}
uint16_t tdm_assist_info_r11_c::drx_assist_info_r11_s_::drx_cycle_len_r11_opts::to_number() const
{
  static const uint16_t options[] = {40, 64, 80, 128, 160, 256};
  return map_enum_number(options, 6, value, "tdm_assist_info_r11_c::drx_assist_info_r11_s_::drx_cycle_len_r11_e_");
}

std::string tdm_assist_info_r11_c::drx_assist_info_r11_s_::drx_active_time_r11_opts::to_string() const
{
  static const char* options[] = {"sf20", "sf30", "sf40", "sf60", "sf80", "sf100", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "tdm_assist_info_r11_c::drx_assist_info_r11_s_::drx_active_time_r11_e_");
}
uint8_t tdm_assist_info_r11_c::drx_assist_info_r11_s_::drx_active_time_r11_opts::to_number() const
{
  static const uint8_t options[] = {20, 30, 40, 60, 80, 100};
  return map_enum_number(options, 6, value, "tdm_assist_info_r11_c::drx_assist_info_r11_s_::drx_active_time_r11_e_");
}

std::string tdm_assist_info_r11_c::types_opts::to_string() const
{
  static const char* options[] = {"drx-AssistanceInfo-r11", "idc-SubframePatternList-r11"};
  return convert_enum_idx(options, 2, value, "tdm_assist_info_r11_c::types");
}

std::string ueassist_info_v1430_ies_s::rlm_report_r14_s_::rlm_event_r14_opts::to_string() const
{
  static const char* options[] = {"earlyOutOfSync", "earlyInSync"};
  return convert_enum_idx(options, 2, value, "ueassist_info_v1430_ies_s::rlm_report_r14_s_::rlm_event_r14_e_");
}

std::string ueassist_info_v1430_ies_s::rlm_report_r14_s_::excess_rep_mpdcch_r14_opts::to_string() const
{
  static const char* options[] = {"excessRep1", "excessRep2"};
  return convert_enum_idx(options, 2, value, "ueassist_info_v1430_ies_s::rlm_report_r14_s_::excess_rep_mpdcch_r14_e_");
}
uint8_t ueassist_info_v1430_ies_s::rlm_report_r14_s_::excess_rep_mpdcch_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "ueassist_info_v1430_ies_s::rlm_report_r14_s_::excess_rep_mpdcch_r14_e_");
}

// WLAN-Status-r13 ::= ENUMERATED
std::string wlan_status_r13_opts::to_string() const
{
  static const char* options[] = {
      "successfulAssociation", "failureWlanRadioLink", "failureWlanUnavailable", "failureTimeout"};
  return convert_enum_idx(options, 4, value, "wlan_status_r13_e");
}

std::string failed_lc_ch_info_r15_s::failed_lc_ch_id_r15_s_::cell_group_ind_r15_opts::to_string() const
{
  static const char* options[] = {"mn", "sn"};
  return convert_enum_idx(options, 2, value, "failed_lc_ch_info_r15_s::failed_lc_ch_id_r15_s_::cell_group_ind_r15_e_");
}

std::string failed_lc_ch_info_r15_s::fail_type_opts::to_string() const
{
  static const char* options[] = {"duplication", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "failed_lc_ch_info_r15_s::fail_type_e_");
}

std::string inter_freq_rstd_meas_ind_r10_ies_s::rstd_inter_freq_ind_r10_c_::types_opts::to_string() const
{
  static const char* options[] = {"start", "stop"};
  return convert_enum_idx(options, 2, value, "inter_freq_rstd_meas_ind_r10_ies_s::rstd_inter_freq_ind_r10_c_::types");
}

std::string meas_report_app_layer_r15_ies_s::service_type_opts::to_string() const
{
  static const char* options[] = {"qoe", "qoemtsi", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "meas_report_app_layer_r15_ies_s::service_type_e_");
}

std::string proximity_ind_r9_ies_s::type_r9_opts::to_string() const
{
  static const char* options[] = {"entering", "leaving"};
  return convert_enum_idx(options, 2, value, "proximity_ind_r9_ies_s::type_r9_e_");
}

std::string proximity_ind_r9_ies_s::carrier_freq_r9_c_::types_opts::to_string() const
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

std::string rrc_conn_resume_complete_r13_ies_s::mob_state_r13_opts::to_string() const
{
  static const char* options[] = {"normal", "medium", "high", "spare"};
  return convert_enum_idx(options, 4, value, "rrc_conn_resume_complete_r13_ies_s::mob_state_r13_e_");
}

std::string ueassist_info_r11_ies_s::pwr_pref_ind_r11_opts::to_string() const
{
  static const char* options[] = {"normal", "lowPowerConsumption"};
  return convert_enum_idx(options, 2, value, "ueassist_info_r11_ies_s::pwr_pref_ind_r11_e_");
}

std::string ul_info_transfer_r8_ies_s::ded_info_type_c_::types_opts::to_string() const
{
  static const char* options[] = {"dedicatedInfoNAS", "dedicatedInfoCDMA2000-1XRTT", "dedicatedInfoCDMA2000-HRPD"};
  return convert_enum_idx(options, 3, value, "ul_info_transfer_r8_ies_s::ded_info_type_c_::types");
}

std::string csfb_params_request_cdma2000_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"csfbParametersRequestCDMA2000-r8", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "csfb_params_request_cdma2000_s::crit_exts_c_::types");
}
uint16_t csfb_params_request_cdma2000_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint16_t options[] = {2000};
  return map_enum_number(options, 1, value, "csfb_params_request_cdma2000_s::crit_exts_c_::types");
}

std::string counter_check_resp_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"counterCheckResponse-r8", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "counter_check_resp_s::crit_exts_c_::types");
}

std::string in_dev_coex_ind_r11_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"inDeviceCoexIndication-r11", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "in_dev_coex_ind_r11_s::crit_exts_c_::c1_c_::types");
}

std::string inter_freq_rstd_meas_ind_r10_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"interFreqRSTDMeasurementIndication-r10", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "inter_freq_rstd_meas_ind_r10_s::crit_exts_c_::c1_c_::types");
}

std::string mbms_count_resp_r10_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"countingResponse-r10", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "mbms_count_resp_r10_s::crit_exts_c_::c1_c_::types");
}

std::string mbms_interest_ind_r11_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"interestIndication-r11", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "mbms_interest_ind_r11_s::crit_exts_c_::c1_c_::types");
}

std::string meas_report_app_layer_r15_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"measReportAppLayer-r15", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "meas_report_app_layer_r15_s::crit_exts_c_::types");
}

std::string meas_report_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "measurementReport-r8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "meas_report_s::crit_exts_c_::c1_c_::types");
}

std::string proximity_ind_r9_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"proximityIndication-r9", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "proximity_ind_r9_s::crit_exts_c_::c1_c_::types");
}

std::string rn_recfg_complete_r10_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"rnReconfigurationComplete-r10", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "rn_recfg_complete_r10_s::crit_exts_c_::c1_c_::types");
}

std::string rrc_conn_recfg_complete_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionReconfigurationComplete-r8", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_conn_recfg_complete_s::crit_exts_c_::types");
}

std::string rrc_conn_reest_complete_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionReestablishmentComplete-r8", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_conn_reest_complete_s::crit_exts_c_::types");
}

std::string rrc_conn_resume_complete_r13_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionResumeComplete-r13", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "rrc_conn_resume_complete_r13_s::crit_exts_c_::types");
}

std::string rrc_conn_setup_complete_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"rrcConnectionSetupComplete-r8", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "rrc_conn_setup_complete_s::crit_exts_c_::c1_c_::types");
}

std::string scg_fail_info_r12_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"scgFailureInformation-r12", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "scg_fail_info_r12_s::crit_exts_c_::c1_c_::types");
}

std::string scg_fail_info_nr_r15_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"scgFailureInformationNR-r15", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "scg_fail_info_nr_r15_s::crit_exts_c_::c1_c_::types");
}

std::string security_mode_complete_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"securityModeComplete-r8", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "security_mode_complete_s::crit_exts_c_::types");
}

std::string security_mode_fail_s::crit_exts_c_::types_opts::to_string() const
{
  static const char* options[] = {"securityModeFailure-r8", "criticalExtensionsFuture"};
  return convert_enum_idx(options, 2, value, "security_mode_fail_s::crit_exts_c_::types");
}

std::string sidelink_ue_info_r12_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"sidelinkUEInformation-r12", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "sidelink_ue_info_r12_s::crit_exts_c_::c1_c_::types");
}

std::string ueassist_info_r11_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"ueAssistanceInformation-r11", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ueassist_info_r11_s::crit_exts_c_::c1_c_::types");
}

std::string ue_cap_info_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "ueCapabilityInformation-r8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "ue_cap_info_s::crit_exts_c_::c1_c_::types");
}

std::string ue_info_resp_r9_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"ueInformationResponse-r9", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ue_info_resp_r9_s::crit_exts_c_::c1_c_::types");
}

std::string ul_ho_prep_transfer_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"ulHandoverPreparationTransfer-r8", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ul_ho_prep_transfer_s::crit_exts_c_::c1_c_::types");
}

std::string ul_info_transfer_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"ulInformationTransfer-r8", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ul_info_transfer_s::crit_exts_c_::c1_c_::types");
}

std::string ul_info_transfer_mrdc_r15_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"ulInformationTransferMRDC-r15", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ul_info_transfer_mrdc_r15_s::crit_exts_c_::c1_c_::types");
}

std::string wlan_conn_status_report_r13_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {"wlanConnectionStatusReport-r13", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "wlan_conn_status_report_r13_s::crit_exts_c_::c1_c_::types");
}

std::string ul_dcch_msg_type_c::c1_c_::types_opts::to_string() const
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

std::string ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::types_opts::to_string() const
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

std::string ul_dcch_msg_type_c::msg_class_ext_c_::types_opts::to_string() const
{
  static const char* options[] = {"c2", "messageClassExtensionFuture-r11"};
  return convert_enum_idx(options, 2, value, "ul_dcch_msg_type_c::msg_class_ext_c_::types");
}
uint8_t ul_dcch_msg_type_c::msg_class_ext_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {2};
  return map_enum_number(options, 1, value, "ul_dcch_msg_type_c::msg_class_ext_c_::types");
}

std::string ul_dcch_msg_type_c::types_opts::to_string() const
{
  static const char* options[] = {"c1", "messageClassExtension"};
  return convert_enum_idx(options, 2, value, "ul_dcch_msg_type_c::types");
}
uint8_t ul_dcch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "ul_dcch_msg_type_c::types");
}

// MIMO-CapabilityDL-r10 ::= ENUMERATED
std::string mimo_cap_dl_r10_opts::to_string() const
{
  static const char* options[] = {"twoLayers", "fourLayers", "eightLayers"};
  return convert_enum_idx(options, 3, value, "mimo_cap_dl_r10_e");
}
uint8_t mimo_cap_dl_r10_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 8};
  return map_enum_number(options, 3, value, "mimo_cap_dl_r10_e");
}

// MIMO-CapabilityUL-r10 ::= ENUMERATED
std::string mimo_cap_ul_r10_opts::to_string() const
{
  static const char* options[] = {"twoLayers", "fourLayers"};
  return convert_enum_idx(options, 2, value, "mimo_cap_ul_r10_e");
}
uint8_t mimo_cap_ul_r10_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "mimo_cap_ul_r10_e");
}

std::string band_params_r11_s::supported_csi_proc_r11_opts::to_string() const
{
  static const char* options[] = {"n1", "n3", "n4"};
  return convert_enum_idx(options, 3, value, "band_params_r11_s::supported_csi_proc_r11_e_");
}
uint8_t band_params_r11_s::supported_csi_proc_r11_opts::to_number() const
{
  static const uint8_t options[] = {1, 3, 4};
  return map_enum_number(options, 3, value, "band_params_r11_s::supported_csi_proc_r11_e_");
}

std::string intra_band_contiguous_cc_info_r12_s::supported_csi_proc_r12_opts::to_string() const
{
  static const char* options[] = {"n1", "n3", "n4"};
  return convert_enum_idx(options, 3, value, "intra_band_contiguous_cc_info_r12_s::supported_csi_proc_r12_e_");
}
uint8_t intra_band_contiguous_cc_info_r12_s::supported_csi_proc_r12_opts::to_number() const
{
  static const uint8_t options[] = {1, 3, 4};
  return map_enum_number(options, 3, value, "intra_band_contiguous_cc_info_r12_s::supported_csi_proc_r12_e_");
}

std::string band_params_r13_s::supported_csi_proc_r13_opts::to_string() const
{
  static const char* options[] = {"n1", "n3", "n4"};
  return convert_enum_idx(options, 3, value, "band_params_r13_s::supported_csi_proc_r13_e_");
}
uint8_t band_params_r13_s::supported_csi_proc_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 3, 4};
  return map_enum_number(options, 3, value, "band_params_r13_s::supported_csi_proc_r13_e_");
}

std::string
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

std::string band_params_v1130_s::supported_csi_proc_r11_opts::to_string() const
{
  static const char* options[] = {"n1", "n3", "n4"};
  return convert_enum_idx(options, 3, value, "band_params_v1130_s::supported_csi_proc_r11_e_");
}
uint8_t band_params_v1130_s::supported_csi_proc_r11_opts::to_number() const
{
  static const uint8_t options[] = {1, 3, 4};
  return map_enum_number(options, 3, value, "band_params_v1130_s::supported_csi_proc_r11_e_");
}

std::string
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

std::string retuning_time_info_r14_s::retuning_info_s_::rf_retuning_time_dl_r14_opts::to_string() const
{
  static const char* options[] = {"n0",
                                  "n0dot5",
                                  "n1",
                                  "n1dot5",
                                  "n2",
                                  "n2dot5",
                                  "n3",
                                  "n3dot5",
                                  "n4",
                                  "n4dot5",
                                  "n5",
                                  "n5dot5",
                                  "n6",
                                  "n6dot5",
                                  "n7",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "retuning_time_info_r14_s::retuning_info_s_::rf_retuning_time_dl_r14_e_");
}
float retuning_time_info_r14_s::retuning_info_s_::rf_retuning_time_dl_r14_opts::to_number() const
{
  static const float options[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0};
  return map_enum_number(options, 15, value, "retuning_time_info_r14_s::retuning_info_s_::rf_retuning_time_dl_r14_e_");
}
std::string retuning_time_info_r14_s::retuning_info_s_::rf_retuning_time_dl_r14_opts::to_number_string() const
{
  static const char* options[] = {
      "0", "0.5", "1", "1.5", "2", "2.5", "3", "3.5", "4", "4.5", "5", "5.5", "6", "6.5", "7"};
  return convert_enum_idx(options, 16, value, "retuning_time_info_r14_s::retuning_info_s_::rf_retuning_time_dl_r14_e_");
}

std::string retuning_time_info_r14_s::retuning_info_s_::rf_retuning_time_ul_r14_opts::to_string() const
{
  static const char* options[] = {"n0",
                                  "n0dot5",
                                  "n1",
                                  "n1dot5",
                                  "n2",
                                  "n2dot5",
                                  "n3",
                                  "n3dot5",
                                  "n4",
                                  "n4dot5",
                                  "n5",
                                  "n5dot5",
                                  "n6",
                                  "n6dot5",
                                  "n7",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "retuning_time_info_r14_s::retuning_info_s_::rf_retuning_time_ul_r14_e_");
}
float retuning_time_info_r14_s::retuning_info_s_::rf_retuning_time_ul_r14_opts::to_number() const
{
  static const float options[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0};
  return map_enum_number(options, 15, value, "retuning_time_info_r14_s::retuning_info_s_::rf_retuning_time_ul_r14_e_");
}
std::string retuning_time_info_r14_s::retuning_info_s_::rf_retuning_time_ul_r14_opts::to_number_string() const
{
  static const char* options[] = {
      "0", "0.5", "1", "1.5", "2", "2.5", "3", "3.5", "4", "4.5", "5", "5.5", "6", "6.5", "7"};
  return convert_enum_idx(options, 16, value, "retuning_time_info_r14_s::retuning_info_s_::rf_retuning_time_ul_r14_e_");
}

std::string mimo_ca_params_per_bo_bc_per_tm_v1470_s::csi_report_advanced_max_ports_r14_opts::to_string() const
{
  static const char* options[] = {"n8", "n12", "n16", "n20", "n24", "n28"};
  return convert_enum_idx(
      options, 6, value, "mimo_ca_params_per_bo_bc_per_tm_v1470_s::csi_report_advanced_max_ports_r14_e_");
}
uint8_t mimo_ca_params_per_bo_bc_per_tm_v1470_s::csi_report_advanced_max_ports_r14_opts::to_number() const
{
  static const uint8_t options[] = {8, 12, 16, 20, 24, 28};
  return map_enum_number(
      options, 6, value, "mimo_ca_params_per_bo_bc_per_tm_v1470_s::csi_report_advanced_max_ports_r14_e_");
}

std::string stti_spt_band_params_r15_s::stti_supported_csi_proc_r15_opts::to_string() const
{
  static const char* options[] = {"n1", "n3", "n4"};
  return convert_enum_idx(options, 3, value, "stti_spt_band_params_r15_s::stti_supported_csi_proc_r15_e_");
}
uint8_t stti_spt_band_params_r15_s::stti_supported_csi_proc_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 3, 4};
  return map_enum_number(options, 3, value, "stti_spt_band_params_r15_s::stti_supported_csi_proc_r15_e_");
}

// V2X-BandwidthClass-r14 ::= ENUMERATED
std::string v2x_bw_class_r14_opts::to_string() const
{
  static const char* options[] = {"a", "b", "c", "d", "e", "f", "c1-v1530"};
  return convert_enum_idx(options, 7, value, "v2x_bw_class_r14_e");
}
uint8_t v2x_bw_class_r14_opts::to_number() const
{
  if (value == c1_v1530) {
    return 1;
  }
  invalid_enum_number(value, "v2x_bw_class_r14_e");
  return 0;
}

std::string feature_set_dl_per_cc_r15_s::supported_csi_proc_r15_opts::to_string() const
{
  static const char* options[] = {"n1", "n3", "n4"};
  return convert_enum_idx(options, 3, value, "feature_set_dl_per_cc_r15_s::supported_csi_proc_r15_e_");
}
uint8_t feature_set_dl_per_cc_r15_s::supported_csi_proc_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 3, 4};
  return map_enum_number(options, 3, value, "feature_set_dl_per_cc_r15_s::supported_csi_proc_r15_e_");
}

std::string irat_params_cdma2000_minus1_xrtt_s::tx_cfg1_xrtt_opts::to_string() const
{
  static const char* options[] = {"single", "dual"};
  return convert_enum_idx(options, 2, value, "irat_params_cdma2000_minus1_xrtt_s::tx_cfg1_xrtt_e_");
}

std::string irat_params_cdma2000_minus1_xrtt_s::rx_cfg1_xrtt_opts::to_string() const
{
  static const char* options[] = {"single", "dual"};
  return convert_enum_idx(options, 2, value, "irat_params_cdma2000_minus1_xrtt_s::rx_cfg1_xrtt_e_");
}

std::string irat_params_cdma2000_hrpd_s::tx_cfg_hrpd_opts::to_string() const
{
  static const char* options[] = {"single", "dual"};
  return convert_enum_idx(options, 2, value, "irat_params_cdma2000_hrpd_s::tx_cfg_hrpd_e_");
}

std::string irat_params_cdma2000_hrpd_s::rx_cfg_hrpd_opts::to_string() const
{
  static const char* options[] = {"single", "dual"};
  return convert_enum_idx(options, 2, value, "irat_params_cdma2000_hrpd_s::rx_cfg_hrpd_e_");
}

// SupportedBandGERAN ::= ENUMERATED
std::string supported_band_geran_opts::to_string() const
{
  static const char* options[] = {"gsm450",
                                  "gsm480",
                                  "gsm710",
                                  "gsm750",
                                  "gsm810",
                                  "gsm850",
                                  "gsm900P",
                                  "gsm900E",
                                  "gsm900R",
                                  "gsm1800",
                                  "gsm1900",
                                  "spare5",
                                  "spare4",
                                  "spare3",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "supported_band_geran_e");
}

// SupportedBandUTRA-FDD ::= ENUMERATED
std::string supported_band_utra_fdd_opts::to_string() const
{
  static const char* options[] = {"bandI",        "bandII",        "bandIII",        "bandIV",       "bandV",
                                  "bandVI",       "bandVII",       "bandVIII",       "bandIX",       "bandX",
                                  "bandXI",       "bandXII",       "bandXIII",       "bandXIV",      "bandXV",
                                  "bandXVI",      "bandXVII-8a0",  "bandXVIII-8a0",  "bandXIX-8a0",  "bandXX-8a0",
                                  "bandXXI-8a0",  "bandXXII-8a0",  "bandXXIII-8a0",  "bandXXIV-8a0", "bandXXV-8a0",
                                  "bandXXVI-8a0", "bandXXVII-8a0", "bandXXVIII-8a0", "bandXXIX-8a0", "bandXXX-8a0",
                                  "bandXXXI-8a0", "bandXXXII-8a0"};
  return convert_enum_idx(options, 32, value, "supported_band_utra_fdd_e");
}

// SupportedBandUTRA-TDD128 ::= ENUMERATED
std::string supported_band_utra_tdd128_opts::to_string() const
{
  static const char* options[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p"};
  return convert_enum_idx(options, 16, value, "supported_band_utra_tdd128_e");
}

// SupportedBandUTRA-TDD384 ::= ENUMERATED
std::string supported_band_utra_tdd384_opts::to_string() const
{
  static const char* options[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p"};
  return convert_enum_idx(options, 16, value, "supported_band_utra_tdd384_e");
}

// SupportedBandUTRA-TDD768 ::= ENUMERATED
std::string supported_band_utra_tdd768_opts::to_string() const
{
  static const char* options[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p"};
  return convert_enum_idx(options, 16, value, "supported_band_utra_tdd768_e");
}

// ProcessingTimelineSet-r15 ::= ENUMERATED
std::string processing_timeline_set_r15_opts::to_string() const
{
  static const char* options[] = {"set1", "set2"};
  return convert_enum_idx(options, 2, value, "processing_timeline_set_r15_e");
}
uint8_t processing_timeline_set_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "processing_timeline_set_r15_e");
}

std::string mimo_ue_params_per_tm_v1430_s::nzp_csi_rs_aperiodic_info_r14_s_::nmax_res_r14_opts::to_string() const
{
  static const char* options[] = {"ffs1", "ffs2", "ffs3", "ffs4"};
  return convert_enum_idx(
      options, 4, value, "mimo_ue_params_per_tm_v1430_s::nzp_csi_rs_aperiodic_info_r14_s_::nmax_res_r14_e_");
}
uint8_t mimo_ue_params_per_tm_v1430_s::nzp_csi_rs_aperiodic_info_r14_s_::nmax_res_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4};
  return map_enum_number(
      options, 4, value, "mimo_ue_params_per_tm_v1430_s::nzp_csi_rs_aperiodic_info_r14_s_::nmax_res_r14_e_");
}

std::string mimo_ue_params_per_tm_v1430_s::nzp_csi_rs_periodic_info_r14_s_::nmax_res_r14_opts::to_string() const
{
  static const char* options[] = {"ffs1", "ffs2", "ffs3", "ffs4"};
  return convert_enum_idx(
      options, 4, value, "mimo_ue_params_per_tm_v1430_s::nzp_csi_rs_periodic_info_r14_s_::nmax_res_r14_e_");
}
uint8_t mimo_ue_params_per_tm_v1430_s::nzp_csi_rs_periodic_info_r14_s_::nmax_res_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4};
  return map_enum_number(
      options, 4, value, "mimo_ue_params_per_tm_v1430_s::nzp_csi_rs_periodic_info_r14_s_::nmax_res_r14_e_");
}

std::string mimo_ue_params_per_tm_v1470_s::csi_report_advanced_max_ports_r14_opts::to_string() const
{
  static const char* options[] = {"n8", "n12", "n16", "n20", "n24", "n28"};
  return convert_enum_idx(options, 6, value, "mimo_ue_params_per_tm_v1470_s::csi_report_advanced_max_ports_r14_e_");
}
uint8_t mimo_ue_params_per_tm_v1470_s::csi_report_advanced_max_ports_r14_opts::to_number() const
{
  static const uint8_t options[] = {8, 12, 16, 20, 24, 28};
  return map_enum_number(options, 6, value, "mimo_ue_params_per_tm_v1470_s::csi_report_advanced_max_ports_r14_e_");
}

std::string naics_cap_entry_r12_s::nof_aggregated_prb_r12_opts::to_string() const
{
  static const char* options[] = {"n50",
                                  "n75",
                                  "n100",
                                  "n125",
                                  "n150",
                                  "n175",
                                  "n200",
                                  "n225",
                                  "n250",
                                  "n275",
                                  "n300",
                                  "n350",
                                  "n400",
                                  "n450",
                                  "n500",
                                  "spare"};
  return convert_enum_idx(options, 16, value, "naics_cap_entry_r12_s::nof_aggregated_prb_r12_e_");
}
uint16_t naics_cap_entry_r12_s::nof_aggregated_prb_r12_opts::to_number() const
{
  static const uint16_t options[] = {50, 75, 100, 125, 150, 175, 200, 225, 250, 275, 300, 350, 400, 450, 500};
  return map_enum_number(options, 15, value, "naics_cap_entry_r12_s::nof_aggregated_prb_r12_e_");
}

std::string pdcp_params_s::max_num_rohc_context_sessions_opts::to_string() const
{
  static const char* options[] = {"cs2",
                                  "cs4",
                                  "cs8",
                                  "cs12",
                                  "cs16",
                                  "cs24",
                                  "cs32",
                                  "cs48",
                                  "cs64",
                                  "cs128",
                                  "cs256",
                                  "cs512",
                                  "cs1024",
                                  "cs16384",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "pdcp_params_s::max_num_rohc_context_sessions_e_");
}
uint16_t pdcp_params_s::max_num_rohc_context_sessions_opts::to_number() const
{
  static const uint16_t options[] = {2, 4, 8, 12, 16, 24, 32, 48, 64, 128, 256, 512, 1024, 16384};
  return map_enum_number(options, 14, value, "pdcp_params_s::max_num_rohc_context_sessions_e_");
}

std::string pdcp_params_nr_r15_s::rohc_context_max_sessions_r15_opts::to_string() const
{
  static const char* options[] = {"cs2",
                                  "cs4",
                                  "cs8",
                                  "cs12",
                                  "cs16",
                                  "cs24",
                                  "cs32",
                                  "cs48",
                                  "cs64",
                                  "cs128",
                                  "cs256",
                                  "cs512",
                                  "cs1024",
                                  "cs16384",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "pdcp_params_nr_r15_s::rohc_context_max_sessions_r15_e_");
}
uint16_t pdcp_params_nr_r15_s::rohc_context_max_sessions_r15_opts::to_number() const
{
  static const uint16_t options[] = {2, 4, 8, 12, 16, 24, 32, 48, 64, 128, 256, 512, 1024, 16384};
  return map_enum_number(options, 14, value, "pdcp_params_nr_r15_s::rohc_context_max_sessions_r15_e_");
}

std::string phy_layer_params_v1430_s::ce_pdsch_pusch_max_bw_r14_opts::to_string() const
{
  static const char* options[] = {"bw5", "bw20"};
  return convert_enum_idx(options, 2, value, "phy_layer_params_v1430_s::ce_pdsch_pusch_max_bw_r14_e_");
}
uint8_t phy_layer_params_v1430_s::ce_pdsch_pusch_max_bw_r14_opts::to_number() const
{
  static const uint8_t options[] = {5, 20};
  return map_enum_number(options, 2, value, "phy_layer_params_v1430_s::ce_pdsch_pusch_max_bw_r14_e_");
}

std::string phy_layer_params_v1430_s::ce_retuning_symbols_r14_opts::to_string() const
{
  static const char* options[] = {"n0", "n1"};
  return convert_enum_idx(options, 2, value, "phy_layer_params_v1430_s::ce_retuning_symbols_r14_e_");
}
uint8_t phy_layer_params_v1430_s::ce_retuning_symbols_r14_opts::to_number() const
{
  static const uint8_t options[] = {0, 1};
  return map_enum_number(options, 2, value, "phy_layer_params_v1430_s::ce_retuning_symbols_r14_e_");
}

std::string phy_layer_params_v1530_s::stti_spt_cap_r15_s_::max_layers_slot_or_subslot_pusch_r15_opts::to_string() const
{
  static const char* options[] = {"oneLayer", "twoLayers", "fourLayers"};
  return convert_enum_idx(
      options, 3, value, "phy_layer_params_v1530_s::stti_spt_cap_r15_s_::max_layers_slot_or_subslot_pusch_r15_e_");
}
uint8_t phy_layer_params_v1530_s::stti_spt_cap_r15_s_::max_layers_slot_or_subslot_pusch_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4};
  return map_enum_number(
      options, 3, value, "phy_layer_params_v1530_s::stti_spt_cap_r15_s_::max_layers_slot_or_subslot_pusch_r15_e_");
}

std::string phy_layer_params_v1530_s::stti_spt_cap_r15_s_::sps_stti_r15_opts::to_string() const
{
  static const char* options[] = {"slot", "subslot", "slotAndSubslot"};
  return convert_enum_idx(options, 3, value, "phy_layer_params_v1530_s::stti_spt_cap_r15_s_::sps_stti_r15_e_");
}

std::string supported_band_eutra_v1320_s::ue_pwr_class_n_r13_opts::to_string() const
{
  static const char* options[] = {"class1", "class2", "class4"};
  return convert_enum_idx(options, 3, value, "supported_band_eutra_v1320_s::ue_pwr_class_n_r13_e_");
}
uint8_t supported_band_eutra_v1320_s::ue_pwr_class_n_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4};
  return map_enum_number(options, 3, value, "supported_band_eutra_v1320_s::ue_pwr_class_n_r13_e_");
}

std::string sl_params_r12_s::disc_supported_proc_r12_opts::to_string() const
{
  static const char* options[] = {"n50", "n400"};
  return convert_enum_idx(options, 2, value, "sl_params_r12_s::disc_supported_proc_r12_e_");
}
uint16_t sl_params_r12_s::disc_supported_proc_r12_opts::to_number() const
{
  static const uint16_t options[] = {50, 400};
  return map_enum_number(options, 2, value, "sl_params_r12_s::disc_supported_proc_r12_e_");
}

std::string sl_params_v1530_s::slss_supported_tx_freq_r15_opts::to_string() const
{
  static const char* options[] = {"single", "multiple"};
  return convert_enum_idx(options, 2, value, "sl_params_v1530_s::slss_supported_tx_freq_r15_e_");
}

std::string sps_cfg_dl_stti_r15_c::setup_s_::semi_persist_sched_interv_dl_stti_r15_opts::to_string() const
{
  static const char* options[] = {"sTTI1",
                                  "sTTI2",
                                  "sTTI3",
                                  "sTTI4",
                                  "sTTI6",
                                  "sTTI8",
                                  "sTTI12",
                                  "sTTI16",
                                  "sTTI20",
                                  "sTTI40",
                                  "sTTI60",
                                  "sTTI80",
                                  "sTTI120",
                                  "sTTI240",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(
      options, 16, value, "sps_cfg_dl_stti_r15_c::setup_s_::semi_persist_sched_interv_dl_stti_r15_e_");
}
uint8_t sps_cfg_dl_stti_r15_c::setup_s_::semi_persist_sched_interv_dl_stti_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 6, 8, 12, 16, 20, 40, 60, 80, 120, 240};
  return map_enum_number(
      options, 14, value, "sps_cfg_dl_stti_r15_c::setup_s_::semi_persist_sched_interv_dl_stti_r15_e_");
}

std::string laa_params_v1430_s::two_step_sched_timing_info_r14_opts::to_string() const
{
  static const char* options[] = {"nPlus1", "nPlus2", "nPlus3"};
  return convert_enum_idx(options, 3, value, "laa_params_v1430_s::two_step_sched_timing_info_r14_e_");
}
uint8_t laa_params_v1430_s::two_step_sched_timing_info_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3};
  return map_enum_number(options, 3, value, "laa_params_v1430_s::two_step_sched_timing_info_r14_e_");
}

std::string pdcp_params_v1430_s::max_num_rohc_context_sessions_r14_opts::to_string() const
{
  static const char* options[] = {"cs2",
                                  "cs4",
                                  "cs8",
                                  "cs12",
                                  "cs16",
                                  "cs24",
                                  "cs32",
                                  "cs48",
                                  "cs64",
                                  "cs128",
                                  "cs256",
                                  "cs512",
                                  "cs1024",
                                  "cs16384",
                                  "spare2",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "pdcp_params_v1430_s::max_num_rohc_context_sessions_r14_e_");
}
uint16_t pdcp_params_v1430_s::max_num_rohc_context_sessions_r14_opts::to_number() const
{
  static const uint16_t options[] = {2, 4, 8, 12, 16, 24, 32, 48, 64, 128, 256, 512, 1024, 16384};
  return map_enum_number(options, 14, value, "pdcp_params_v1430_s::max_num_rohc_context_sessions_r14_e_");
}

std::string mbms_params_v1470_s::mbms_max_bw_r14_c_::types_opts::to_string() const
{
  static const char* options[] = {"implicitValue", "explicitValue"};
  return convert_enum_idx(options, 2, value, "mbms_params_v1470_s::mbms_max_bw_r14_c_::types");
}

std::string mbms_params_v1470_s::mbms_scaling_factor1dot25_r14_opts::to_string() const
{
  static const char* options[] = {"n3", "n6", "n9", "n12"};
  return convert_enum_idx(options, 4, value, "mbms_params_v1470_s::mbms_scaling_factor1dot25_r14_e_");
}
uint8_t mbms_params_v1470_s::mbms_scaling_factor1dot25_r14_opts::to_number() const
{
  static const uint8_t options[] = {3, 6, 9, 12};
  return map_enum_number(options, 4, value, "mbms_params_v1470_s::mbms_scaling_factor1dot25_r14_e_");
}

std::string mbms_params_v1470_s::mbms_scaling_factor7dot5_r14_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4"};
  return convert_enum_idx(options, 4, value, "mbms_params_v1470_s::mbms_scaling_factor7dot5_r14_e_");
}
uint8_t mbms_params_v1470_s::mbms_scaling_factor7dot5_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4};
  return map_enum_number(options, 4, value, "mbms_params_v1470_s::mbms_scaling_factor7dot5_r14_e_");
}

std::string ue_eutra_cap_v1430_ies_s::ue_category_ul_v1430_opts::to_string() const
{
  static const char* options[] = {"n16", "n17", "n18", "n19", "n20", "m2"};
  return convert_enum_idx(options, 6, value, "ue_eutra_cap_v1430_ies_s::ue_category_ul_v1430_e_");
}
uint8_t ue_eutra_cap_v1430_ies_s::ue_category_ul_v1430_opts::to_number() const
{
  static const uint8_t options[] = {16, 17, 18, 19, 20, 2};
  return map_enum_number(options, 6, value, "ue_eutra_cap_v1430_ies_s::ue_category_ul_v1430_e_");
}

std::string ue_eutra_cap_v1310_ies_s::ue_category_dl_v1310_opts::to_string() const
{
  static const char* options[] = {"n17", "m1"};
  return convert_enum_idx(options, 2, value, "ue_eutra_cap_v1310_ies_s::ue_category_dl_v1310_e_");
}
uint8_t ue_eutra_cap_v1310_ies_s::ue_category_dl_v1310_opts::to_number() const
{
  static const uint8_t options[] = {17, 1};
  return map_enum_number(options, 2, value, "ue_eutra_cap_v1310_ies_s::ue_category_dl_v1310_e_");
}

std::string ue_eutra_cap_v1310_ies_s::ue_category_ul_v1310_opts::to_string() const
{
  static const char* options[] = {"n14", "m1"};
  return convert_enum_idx(options, 2, value, "ue_eutra_cap_v1310_ies_s::ue_category_ul_v1310_e_");
}
uint8_t ue_eutra_cap_v1310_ies_s::ue_category_ul_v1310_opts::to_number() const
{
  static const uint8_t options[] = {14, 1};
  return map_enum_number(options, 2, value, "ue_eutra_cap_v1310_ies_s::ue_category_ul_v1310_e_");
}

// AccessStratumRelease ::= ENUMERATED
std::string access_stratum_release_opts::to_string() const
{
  static const char* options[] = {"rel8", "rel9", "rel10", "rel11", "rel12", "rel13", "rel14", "rel15"};
  return convert_enum_idx(options, 8, value, "access_stratum_release_e");
}
uint8_t access_stratum_release_opts::to_number() const
{
  static const uint8_t options[] = {8, 9, 10, 11, 12, 13, 14, 15};
  return map_enum_number(options, 8, value, "access_stratum_release_e");
}

std::string scg_cfg_r12_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "scg-Config-r12", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "scg_cfg_r12_s::crit_exts_c_::c1_c_::types");
}

std::string cells_triggered_list_item_c_::pci_utra_c_::types_opts::to_string() const
{
  static const char* options[] = {"fdd", "tdd"};
  return convert_enum_idx(options, 2, value, "cells_triggered_list_item_c_::pci_utra_c_::types");
}

std::string cells_triggered_list_item_c_::types_opts::to_string() const
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

std::string drb_info_scg_r12_s::drb_type_r12_opts::to_string() const
{
  static const char* options[] = {"split", "scg"};
  return convert_enum_idx(options, 2, value, "drb_info_scg_r12_s::drb_type_r12_e_");
}

std::string ho_cmd_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "handoverCommand-r8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "ho_cmd_s::crit_exts_c_::c1_c_::types");
}

std::string ho_prep_info_v920_ies_s::ue_cfg_release_r9_opts::to_string() const
{
  static const char* options[] = {
      "rel9", "rel10", "rel11", "rel12", "v10j0", "v11e0", "v1280", "rel13", "rel14", "rel15"};
  return convert_enum_idx(options, 10, value, "ho_prep_info_v920_ies_s::ue_cfg_release_r9_e_");
}

std::string rrm_cfg_s::ue_inactive_time_opts::to_string() const
{
  static const char* options[] = {
      "s1",    "s2",       "s3",    "s5",           "s7",      "s10",   "s15",     "s20",      "s25",     "s30",
      "s40",   "s50",      "min1",  "min1s20c",     "min1s40", "min2",  "min2s30", "min3",     "min3s30", "min4",
      "min5",  "min6",     "min7",  "min8",         "min9",    "min10", "min12",   "min14",    "min17",   "min20",
      "min24", "min28",    "min33", "min38",        "min44",   "min50", "hr1",     "hr1min30", "hr2",     "hr2min30",
      "hr3",   "hr3min30", "hr4",   "hr5",          "hr6",     "hr8",   "hr10",    "hr13",     "hr16",    "hr20",
      "day1",  "day1hr12", "day2",  "day2hr12",     "day3",    "day4",  "day5",    "day7",     "day10",   "day14",
      "day19", "day24",    "day30", "dayMoreThan30"};
  return convert_enum_idx(options, 64, value, "rrm_cfg_s::ue_inactive_time_e_");
}

std::string ho_prep_info_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "handoverPreparationInformation-r8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "ho_prep_info_s::crit_exts_c_::c1_c_::types");
}

std::string tdd_cfg_sl_r12_s::sf_assign_sl_r12_opts::to_string() const
{
  static const char* options[] = {"none", "sa0", "sa1", "sa2", "sa3", "sa4", "sa5", "sa6"};
  return convert_enum_idx(options, 8, value, "tdd_cfg_sl_r12_s::sf_assign_sl_r12_e_");
}

std::string mib_sl_s::sl_bw_r12_opts::to_string() const
{
  static const char* options[] = {"n6", "n15", "n25", "n50", "n75", "n100"};
  return convert_enum_idx(options, 6, value, "mib_sl_s::sl_bw_r12_e_");
}
uint8_t mib_sl_s::sl_bw_r12_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(options, 6, value, "mib_sl_s::sl_bw_r12_e_");
}

std::string mib_sl_v2x_r14_s::sl_bw_r14_opts::to_string() const
{
  static const char* options[] = {"n6", "n15", "n25", "n50", "n75", "n100"};
  return convert_enum_idx(options, 6, value, "mib_sl_v2x_r14_s::sl_bw_r14_e_");
}
uint8_t mib_sl_v2x_r14_s::sl_bw_r14_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(options, 6, value, "mib_sl_v2x_r14_s::sl_bw_r14_e_");
}

std::string scg_cfg_info_r12_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "scg-ConfigInfo-r12", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "scg_cfg_info_r12_s::crit_exts_c_::c1_c_::types");
}

std::string sl_precfg_disc_pool_r13_s::disc_period_r13_opts::to_string() const
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

std::string sl_precfg_disc_pool_r13_s::tx_params_r13_s_::tx_probability_r13_opts::to_string() const
{
  static const char* options[] = {"p25", "p50", "p75", "p100"};
  return convert_enum_idx(options, 4, value, "sl_precfg_disc_pool_r13_s::tx_params_r13_s_::tx_probability_r13_e_");
}
uint8_t sl_precfg_disc_pool_r13_s::tx_params_r13_s_::tx_probability_r13_opts::to_number() const
{
  static const uint8_t options[] = {25, 50, 75, 100};
  return map_enum_number(options, 4, value, "sl_precfg_disc_pool_r13_s::tx_params_r13_s_::tx_probability_r13_e_");
}

std::string sl_precfg_general_r12_s::sl_bw_r12_opts::to_string() const
{
  static const char* options[] = {"n6", "n15", "n25", "n50", "n75", "n100"};
  return convert_enum_idx(options, 6, value, "sl_precfg_general_r12_s::sl_bw_r12_e_");
}
uint8_t sl_precfg_general_r12_s::sl_bw_r12_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(options, 6, value, "sl_precfg_general_r12_s::sl_bw_r12_e_");
}

std::string sl_precfg_sync_r12_s::sync_ref_min_hyst_r12_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB3", "dB6", "dB9", "dB12"};
  return convert_enum_idx(options, 5, value, "sl_precfg_sync_r12_s::sync_ref_min_hyst_r12_e_");
}
uint8_t sl_precfg_sync_r12_s::sync_ref_min_hyst_r12_opts::to_number() const
{
  static const uint8_t options[] = {0, 3, 6, 9, 12};
  return map_enum_number(options, 5, value, "sl_precfg_sync_r12_s::sync_ref_min_hyst_r12_e_");
}

std::string sl_precfg_sync_r12_s::sync_ref_diff_hyst_r12_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB3", "dB6", "dB9", "dB12", "dBinf"};
  return convert_enum_idx(options, 6, value, "sl_precfg_sync_r12_s::sync_ref_diff_hyst_r12_e_");
}
uint8_t sl_precfg_sync_r12_s::sync_ref_diff_hyst_r12_opts::to_number() const
{
  static const uint8_t options[] = {0, 3, 6, 9, 12};
  return map_enum_number(options, 5, value, "sl_precfg_sync_r12_s::sync_ref_diff_hyst_r12_e_");
}

std::string sl_v2x_precfg_comm_pool_r14_s::size_subch_r14_opts::to_string() const
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

std::string sl_v2x_precfg_comm_pool_r14_s::num_subch_r14_opts::to_string() const
{
  static const char* options[] = {"n1", "n3", "n5", "n8", "n10", "n15", "n20", "spare1"};
  return convert_enum_idx(options, 8, value, "sl_v2x_precfg_comm_pool_r14_s::num_subch_r14_e_");
}
uint8_t sl_v2x_precfg_comm_pool_r14_s::num_subch_r14_opts::to_number() const
{
  static const uint8_t options[] = {1, 3, 5, 8, 10, 15, 20};
  return map_enum_number(options, 7, value, "sl_v2x_precfg_comm_pool_r14_s::num_subch_r14_e_");
}

std::string sl_precfg_v2x_sync_r14_s::sync_ref_min_hyst_r14_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB3", "dB6", "dB9", "dB12"};
  return convert_enum_idx(options, 5, value, "sl_precfg_v2x_sync_r14_s::sync_ref_min_hyst_r14_e_");
}
uint8_t sl_precfg_v2x_sync_r14_s::sync_ref_min_hyst_r14_opts::to_number() const
{
  static const uint8_t options[] = {0, 3, 6, 9, 12};
  return map_enum_number(options, 5, value, "sl_precfg_v2x_sync_r14_s::sync_ref_min_hyst_r14_e_");
}

std::string sl_precfg_v2x_sync_r14_s::sync_ref_diff_hyst_r14_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB3", "dB6", "dB9", "dB12", "dBinf"};
  return convert_enum_idx(options, 6, value, "sl_precfg_v2x_sync_r14_s::sync_ref_diff_hyst_r14_e_");
}
uint8_t sl_precfg_v2x_sync_r14_s::sync_ref_diff_hyst_r14_opts::to_number() const
{
  static const uint8_t options[] = {0, 3, 6, 9, 12};
  return map_enum_number(options, 5, value, "sl_precfg_v2x_sync_r14_s::sync_ref_diff_hyst_r14_e_");
}

std::string sl_v2x_precfg_freq_info_r14_s::sync_prio_r14_opts::to_string() const
{
  static const char* options[] = {"gnss", "enb"};
  return convert_enum_idx(options, 2, value, "sl_v2x_precfg_freq_info_r14_s::sync_prio_r14_e_");
}

// SL-V2X-TxProfile-r15 ::= ENUMERATED
std::string sl_v2x_tx_profile_r15_opts::to_string() const
{
  static const char* options[] = {"rel14", "rel15", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "sl_v2x_tx_profile_r15_e");
}
uint8_t sl_v2x_tx_profile_r15_opts::to_number() const
{
  static const uint8_t options[] = {14, 15};
  return map_enum_number(options, 2, value, "sl_v2x_tx_profile_r15_e");
}

std::string ue_paging_coverage_info_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "uePagingCoverageInformation-r13", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "ue_paging_coverage_info_s::crit_exts_c_::c1_c_::types");
}

std::string ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "ueRadioAccessCapabilityInformation-r8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::types");
}

std::string ue_radio_paging_info_s::crit_exts_c_::c1_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "ueRadioPagingInformation-r12", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "ue_radio_paging_info_s::crit_exts_c_::c1_c_::types");
}

std::string var_meas_idle_cfg_r15_s::meas_idle_dur_r15_opts::to_string() const
{
  static const char* options[] = {"sec10", "sec30", "sec60", "sec120", "sec180", "sec240", "sec300"};
  return convert_enum_idx(options, 7, value, "var_meas_idle_cfg_r15_s::meas_idle_dur_r15_e_");
}
uint16_t var_meas_idle_cfg_r15_s::meas_idle_dur_r15_opts::to_number() const
{
  static const uint16_t options[] = {10, 30, 60, 120, 180, 240, 300};
  return map_enum_number(options, 7, value, "var_meas_idle_cfg_r15_s::meas_idle_dur_r15_e_");
}
