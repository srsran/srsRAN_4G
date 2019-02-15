/*
Copyright 2013-2017 Software Radio Systems Limited

This file is part of srsLTE

srsASN1 is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

srsASN1 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

A copy of the GNU Affero General Public License can be found in
the LICENSE file in the top-level directory of this distribution
and at http://www.gnu.org/licenses/.
*/

#include "srslte/asn1/rrc_asn1.h"
#include <sstream>

using namespace asn1;
using namespace asn1::rrc;

static void invalid_enum_value(int value, const char* name)
{
  rrc_log_print(LOG_LEVEL_ERROR, "The provided enum value=%d of type %s is not recognized\n", value, name);
}

static void invalid_enum_number(int value, const char* name)
{
  rrc_log_print(LOG_LEVEL_ERROR, "The provided enum value=%d of type %s cannot be translated into a number\n", value,
                name);
}

/*******************************************************************************
/*                                Struct Methods
/******************************************************************************/

std::string phich_cfg_s::phich_dur_e_::to_string() const
{
  switch (value) {
    case normal:
      return "normal";
    case extended:
      return "extended";
    default:
      invalid_enum_value(value, "phich_cfg_s::phich_dur_e_");
  }
  return "";
}

std::string phich_cfg_s::phich_res_e_::to_string() const
{
  switch (value) {
    case one_sixth:
      return "oneSixth";
    case half:
      return "half";
    case one:
      return "one";
    case two:
      return "two";
    default:
      invalid_enum_value(value, "phich_cfg_s::phich_res_e_");
  }
  return "";
}
float phich_cfg_s::phich_res_e_::to_number() const
{
  const static float options[] = {0.16666666666666666, 0.5, 1.0, 2.0};
  return get_enum_number(options, 4, value, "phich_cfg_s::phich_res_e_");
}
std::string phich_cfg_s::phich_res_e_::to_number_string() const
{
  switch (value) {
    case one_sixth:
      return "1/6";
    case half:
      return "0.5";
    case one:
      return "1";
    case two:
      return "2";
    default:
      invalid_enum_number(value, "phich_cfg_s::phich_res_e_");
  }
  return "";
}

std::string mib_s::dl_bw_e_::to_string() const
{
  switch (value) {
    case n6:
      return "n6";
    case n15:
      return "n15";
    case n25:
      return "n25";
    case n50:
      return "n50";
    case n75:
      return "n75";
    case n100:
      return "n100";
    default:
      invalid_enum_value(value, "mib_s::dl_bw_e_");
  }
  return "";
}
uint8_t mib_s::dl_bw_e_::to_number() const
{
  const static uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return get_enum_number(options, 6, value, "mib_s::dl_bw_e_");
}

std::string mib_mbms_r14_s::dl_bw_mbms_r14_e_::to_string() const
{
  switch (value) {
    case n6:
      return "n6";
    case n15:
      return "n15";
    case n25:
      return "n25";
    case n50:
      return "n50";
    case n75:
      return "n75";
    case n100:
      return "n100";
    default:
      invalid_enum_value(value, "mib_mbms_r14_s::dl_bw_mbms_r14_e_");
  }
  return "";
}
uint8_t mib_mbms_r14_s::dl_bw_mbms_r14_e_::to_number() const
{
  const static uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return get_enum_number(options, 6, value, "mib_mbms_r14_s::dl_bw_mbms_r14_e_");
}

std::string gnss_id_r15_s::gnss_id_r15_e_::to_string() const
{
  switch (value) {
    case gps:
      return "gps";
    case sbas:
      return "sbas";
    case qzss:
      return "qzss";
    case galileo:
      return "galileo";
    case glonass:
      return "glonass";
    case bds:
      return "bds";
    default:
      invalid_enum_value(value, "gnss_id_r15_s::gnss_id_r15_e_");
  }
  return "";
}

std::string sbas_id_r15_s::sbas_id_r15_e_::to_string() const
{
  switch (value) {
    case waas:
      return "waas";
    case egnos:
      return "egnos";
    case msas:
      return "msas";
    case gagan:
      return "gagan";
    default:
      invalid_enum_value(value, "sbas_id_r15_s::sbas_id_r15_e_");
  }
  return "";
}

std::string pos_sib_type_r15_s::pos_sib_type_r15_e_::to_string() const
{
  switch (value) {
    case pos_sib_type1_minus1:
      return "posSibType1-1";
    case pos_sib_type1_minus2:
      return "posSibType1-2";
    case pos_sib_type1_minus3:
      return "posSibType1-3";
    case pos_sib_type1_minus4:
      return "posSibType1-4";
    case pos_sib_type1_minus5:
      return "posSibType1-5";
    case pos_sib_type1_minus6:
      return "posSibType1-6";
    case pos_sib_type1_minus7:
      return "posSibType1-7";
    case pos_sib_type2_minus1:
      return "posSibType2-1";
    case pos_sib_type2_minus2:
      return "posSibType2-2";
    case pos_sib_type2_minus3:
      return "posSibType2-3";
    case pos_sib_type2_minus4:
      return "posSibType2-4";
    case pos_sib_type2_minus5:
      return "posSibType2-5";
    case pos_sib_type2_minus6:
      return "posSibType2-6";
    case pos_sib_type2_minus7:
      return "posSibType2-7";
    case pos_sib_type2_minus8:
      return "posSibType2-8";
    case pos_sib_type2_minus9:
      return "posSibType2-9";
    case pos_sib_type2_minus10:
      return "posSibType2-10";
    case pos_sib_type2_minus11:
      return "posSibType2-11";
    case pos_sib_type2_minus12:
      return "posSibType2-12";
    case pos_sib_type2_minus13:
      return "posSibType2-13";
    case pos_sib_type2_minus14:
      return "posSibType2-14";
    case pos_sib_type2_minus15:
      return "posSibType2-15";
    case pos_sib_type2_minus16:
      return "posSibType2-16";
    case pos_sib_type2_minus17:
      return "posSibType2-17";
    case pos_sib_type2_minus18:
      return "posSibType2-18";
    case pos_sib_type2_minus19:
      return "posSibType2-19";
    case pos_sib_type3_minus1:
      return "posSibType3-1";
    default:
      invalid_enum_value(value, "pos_sib_type_r15_s::pos_sib_type_r15_e_");
  }
  return "";
}

std::string plmn_id_info_r15_s::plmn_id_minus5_gc_r15_c_::types::to_string() const
{
  switch (value) {
    case plmn_id_r15:
      return "plmn-Identity-r15";
    case plmn_idx_r15:
      return "plmn-Index-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "plmn_id_info_r15_s::plmn_id_minus5_gc_r15_c_::types");
  }
  return "";
}

std::string plmn_id_info_r15_s::cell_reserved_for_oper_r15_e_::to_string() const
{
  switch (value) {
    case reserved:
      return "reserved";
    case not_reserved:
      return "notReserved";
    default:
      invalid_enum_value(value, "plmn_id_info_r15_s::cell_reserved_for_oper_r15_e_");
  }
  return "";
}

std::string plmn_id_info_r15_s::cell_reserved_for_oper_crs_r15_e_::to_string() const
{
  switch (value) {
    case reserved:
      return "reserved";
    case not_reserved:
      return "notReserved";
    default:
      invalid_enum_value(value, "plmn_id_info_r15_s::cell_reserved_for_oper_crs_r15_e_");
  }
  return "";
}

std::string cell_id_minus5_gc_r15_c::types::to_string() const
{
  switch (value) {
    case cell_id_r15:
      return "cellIdentity-r15";
    case cell_id_idx_r15:
      return "cellId-Index-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "cell_id_minus5_gc_r15_c::types");
  }
  return "";
}

std::string plmn_id_info_v1530_s::cell_reserved_for_oper_crs_r15_e_::to_string() const
{
  switch (value) {
    case reserved:
      return "reserved";
    case not_reserved:
      return "notReserved";
    default:
      invalid_enum_value(value, "plmn_id_info_v1530_s::cell_reserved_for_oper_crs_r15_e_");
  }
  return "";
}

std::string pos_sched_info_r15_s::pos_si_periodicity_r15_e_::to_string() const
{
  switch (value) {
    case rf8:
      return "rf8";
    case rf16:
      return "rf16";
    case rf32:
      return "rf32";
    case rf64:
      return "rf64";
    case rf128:
      return "rf128";
    case rf256:
      return "rf256";
    case rf512:
      return "rf512";
    default:
      invalid_enum_value(value, "pos_sched_info_r15_s::pos_si_periodicity_r15_e_");
  }
  return "";
}
uint16_t pos_sched_info_r15_s::pos_si_periodicity_r15_e_::to_number() const
{
  const static uint16_t options[] = {8, 16, 32, 64, 128, 256, 512};
  return get_enum_number(options, 7, value, "pos_sched_info_r15_s::pos_si_periodicity_r15_e_");
}

std::string cell_sel_info_ce_v1530_s::pwr_class14dbm_offset_r15_e_::to_string() const
{
  switch (value) {
    case db_minus6:
      return "dB-6";
    case db_minus3:
      return "dB-3";
    case db3:
      return "dB3";
    case db6:
      return "dB6";
    case db9:
      return "dB9";
    case db12:
      return "dB12";
    default:
      invalid_enum_value(value, "cell_sel_info_ce_v1530_s::pwr_class14dbm_offset_r15_e_");
  }
  return "";
}
int8_t cell_sel_info_ce_v1530_s::pwr_class14dbm_offset_r15_e_::to_number() const
{
  const static int8_t options[] = {-6, -3, 3, 6, 9, 12};
  return get_enum_number(options, 6, value, "cell_sel_info_ce_v1530_s::pwr_class14dbm_offset_r15_e_");
}

std::string plmn_id_info_s::cell_reserved_for_oper_e_::to_string() const
{
  switch (value) {
    case reserved:
      return "reserved";
    case not_reserved:
      return "notReserved";
    default:
      invalid_enum_value(value, "plmn_id_info_s::cell_reserved_for_oper_e_");
  }
  return "";
}

std::string sl_tx_pwr_r14_c::types::to_string() const
{
  switch (value) {
    case minusinfinity_r14:
      return "minusinfinity-r14";
    case tx_pwr_r14:
      return "txPower-r14";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sl_tx_pwr_r14_c::types");
  }
  return "";
}
int8_t sl_tx_pwr_r14_c::types::to_number() const
{
  const static int8_t options[] = {-1};
  return get_enum_number(options, 1, value, "sl_tx_pwr_r14_c::types");
}

// Alpha-r12 ::= ENUMERATED
std::string alpha_r12_e::to_string() const
{
  switch (value) {
    case al0:
      return "al0";
    case al04:
      return "al04";
    case al05:
      return "al05";
    case al06:
      return "al06";
    case al07:
      return "al07";
    case al08:
      return "al08";
    case al09:
      return "al09";
    case al1:
      return "al1";
    default:
      invalid_enum_value(value, "alpha_r12_e");
  }
  return "";
}
float alpha_r12_e::to_number() const
{
  const static float options[] = {0.0, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
  return get_enum_number(options, 8, value, "alpha_r12_e");
}
std::string alpha_r12_e::to_number_string() const
{
  switch (value) {
    case al0:
      return "0";
    case al04:
      return "0.4";
    case al05:
      return "0.5";
    case al06:
      return "0.6";
    case al07:
      return "0.7";
    case al08:
      return "0.8";
    case al09:
      return "0.9";
    case al1:
      return "1";
    default:
      invalid_enum_number(value, "alpha_r12_e");
  }
  return "";
}

std::string sl_pssch_tx_params_r14_s::allowed_retx_num_pssch_r14_e_::to_string() const
{
  switch (value) {
    case n0:
      return "n0";
    case n1:
      return "n1";
    case both:
      return "both";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "sl_pssch_tx_params_r14_s::allowed_retx_num_pssch_r14_e_");
  }
  return "";
}
uint8_t sl_pssch_tx_params_r14_s::allowed_retx_num_pssch_r14_e_::to_number() const
{
  const static uint8_t options[] = {0, 1};
  return get_enum_number(options, 2, value, "sl_pssch_tx_params_r14_s::allowed_retx_num_pssch_r14_e_");
}

// SL-RestrictResourceReservationPeriod-r14 ::= ENUMERATED
std::string sl_restrict_res_reserv_period_r14_e::to_string() const
{
  switch (value) {
    case v0dot2:
      return "v0dot2";
    case v0dot5:
      return "v0dot5";
    case v1:
      return "v1";
    case v2:
      return "v2";
    case v3:
      return "v3";
    case v4:
      return "v4";
    case v5:
      return "v5";
    case v6:
      return "v6";
    case v7:
      return "v7";
    case v8:
      return "v8";
    case v9:
      return "v9";
    case v10:
      return "v10";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "sl_restrict_res_reserv_period_r14_e");
  }
  return "";
}
float sl_restrict_res_reserv_period_r14_e::to_number() const
{
  const static float options[] = {0.2, 0.5, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
  return get_enum_number(options, 12, value, "sl_restrict_res_reserv_period_r14_e");
}
std::string sl_restrict_res_reserv_period_r14_e::to_number_string() const
{
  switch (value) {
    case v0dot2:
      return "0.2";
    case v0dot5:
      return "0.5";
    case v1:
      return "1";
    case v2:
      return "2";
    case v3:
      return "3";
    case v4:
      return "4";
    case v5:
      return "5";
    case v6:
      return "6";
    case v7:
      return "7";
    case v8:
      return "8";
    case v9:
      return "9";
    case v10:
      return "10";
    default:
      invalid_enum_number(value, "sl_restrict_res_reserv_period_r14_e");
  }
  return "";
}

// SL-TypeTxSync-r14 ::= ENUMERATED
std::string sl_type_tx_sync_r14_e::to_string() const
{
  switch (value) {
    case gnss:
      return "gnss";
    case enb:
      return "enb";
    case ue:
      return "ue";
    default:
      invalid_enum_value(value, "sl_type_tx_sync_r14_e");
  }
  return "";
}

std::string sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::crs_intf_mitig_num_prbs_r15_e_::to_string() const
{
  switch (value) {
    case n6:
      return "n6";
    case n24:
      return "n24";
    default:
      invalid_enum_value(value, "sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::crs_intf_mitig_num_prbs_r15_e_");
  }
  return "";
}
uint8_t sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::crs_intf_mitig_num_prbs_r15_e_::to_number() const
{
  const static uint8_t options[] = {6, 24};
  return get_enum_number(options, 2, value,
                         "sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::crs_intf_mitig_num_prbs_r15_e_");
}

std::string sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::types::to_string() const
{
  switch (value) {
    case crs_intf_mitig_enabled_minus15:
      return "crs-IntfMitigEnabled-15";
    case crs_intf_mitig_num_prbs_r15:
      return "crs-IntfMitigNumPRBs-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::types");
  }
  return "";
}
int8_t sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::types::to_number() const
{
  const static int8_t options[] = {-15};
  return get_enum_number(options, 1, value, "sib_type1_v1530_ies_s::crs_intf_mitig_cfg_r15_c_::types");
}

std::string sib_type1_v1530_ies_s::cell_barred_crs_r15_e_::to_string() const
{
  switch (value) {
    case barred:
      return "barred";
    case not_barred:
      return "notBarred";
    default:
      invalid_enum_value(value, "sib_type1_v1530_ies_s::cell_barred_crs_r15_e_");
  }
  return "";
}

std::string
sib_type1_v1530_ies_s::cell_access_related_info_minus5_gc_r15_s_::cell_barred_minus5_gc_r15_e_::to_string() const
{
  switch (value) {
    case barred:
      return "barred";
    case not_barred:
      return "notBarred";
    default:
      invalid_enum_value(
          value, "sib_type1_v1530_ies_s::cell_access_related_info_minus5_gc_r15_s_::cell_barred_minus5_gc_r15_e_");
  }
  return "";
}

std::string
sib_type1_v1530_ies_s::cell_access_related_info_minus5_gc_r15_s_::cell_barred_minus5_gc_crs_r15_e_::to_string() const
{
  switch (value) {
    case barred:
      return "barred";
    case not_barred:
      return "notBarred";
    default:
      invalid_enum_value(
          value, "sib_type1_v1530_ies_s::cell_access_related_info_minus5_gc_r15_s_::cell_barred_minus5_gc_crs_r15_e_");
  }
  return "";
}

// SL-CP-Len-r12 ::= ENUMERATED
std::string sl_cp_len_r12_e::to_string() const
{
  switch (value) {
    case normal:
      return "normal";
    case extended:
      return "extended";
    default:
      invalid_enum_value(value, "sl_cp_len_r12_e");
  }
  return "";
}

std::string sl_offset_ind_r12_c::types::to_string() const
{
  switch (value) {
    case small_r12:
      return "small-r12";
    case large_r12:
      return "large-r12";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sl_offset_ind_r12_c::types");
  }
  return "";
}

std::string sl_pssch_tx_cfg_r14_s::thres_ue_speed_r14_e_::to_string() const
{
  switch (value) {
    case kmph60:
      return "kmph60";
    case kmph80:
      return "kmph80";
    case kmph100:
      return "kmph100";
    case kmph120:
      return "kmph120";
    case kmph140:
      return "kmph140";
    case kmph160:
      return "kmph160";
    case kmph180:
      return "kmph180";
    case kmph200:
      return "kmph200";
    default:
      invalid_enum_value(value, "sl_pssch_tx_cfg_r14_s::thres_ue_speed_r14_e_");
  }
  return "";
}
uint8_t sl_pssch_tx_cfg_r14_s::thres_ue_speed_r14_e_::to_number() const
{
  const static uint8_t options[] = {60, 80, 100, 120, 140, 160, 180, 200};
  return get_enum_number(options, 8, value, "sl_pssch_tx_cfg_r14_s::thres_ue_speed_r14_e_");
}

std::string sf_bitmap_sl_r12_c::types::to_string() const
{
  switch (value) {
    case bs4_r12:
      return "bs4-r12";
    case bs8_r12:
      return "bs8-r12";
    case bs12_r12:
      return "bs12-r12";
    case bs16_r12:
      return "bs16-r12";
    case bs30_r12:
      return "bs30-r12";
    case bs40_r12:
      return "bs40-r12";
    case bs42_r12:
      return "bs42-r12";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sf_bitmap_sl_r12_c::types");
  }
  return "";
}
uint8_t sf_bitmap_sl_r12_c::types::to_number() const
{
  const static uint8_t options[] = {4, 8, 12, 16, 30, 40, 42};
  return get_enum_number(options, 7, value, "sf_bitmap_sl_r12_c::types");
}

std::string sf_bitmap_sl_r14_c::types::to_string() const
{
  switch (value) {
    case bs10_r14:
      return "bs10-r14";
    case bs16_r14:
      return "bs16-r14";
    case bs20_r14:
      return "bs20-r14";
    case bs30_r14:
      return "bs30-r14";
    case bs40_r14:
      return "bs40-r14";
    case bs50_r14:
      return "bs50-r14";
    case bs60_r14:
      return "bs60-r14";
    case bs100_r14:
      return "bs100-r14";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sf_bitmap_sl_r14_c::types");
  }
  return "";
}
uint8_t sf_bitmap_sl_r14_c::types::to_number() const
{
  const static uint8_t options[] = {10, 16, 20, 30, 40, 50, 60, 100};
  return get_enum_number(options, 8, value, "sf_bitmap_sl_r14_c::types");
}

std::string tdd_cfg_s::sf_assign_e_::to_string() const
{
  switch (value) {
    case sa0:
      return "sa0";
    case sa1:
      return "sa1";
    case sa2:
      return "sa2";
    case sa3:
      return "sa3";
    case sa4:
      return "sa4";
    case sa5:
      return "sa5";
    case sa6:
      return "sa6";
    default:
      invalid_enum_value(value, "tdd_cfg_s::sf_assign_e_");
  }
  return "";
}
uint8_t tdd_cfg_s::sf_assign_e_::to_number() const
{
  const static uint8_t options[] = {0, 1, 2, 3, 4, 5, 6};
  return get_enum_number(options, 7, value, "tdd_cfg_s::sf_assign_e_");
}

std::string tdd_cfg_s::special_sf_patterns_e_::to_string() const
{
  switch (value) {
    case ssp0:
      return "ssp0";
    case ssp1:
      return "ssp1";
    case ssp2:
      return "ssp2";
    case ssp3:
      return "ssp3";
    case ssp4:
      return "ssp4";
    case ssp5:
      return "ssp5";
    case ssp6:
      return "ssp6";
    case ssp7:
      return "ssp7";
    case ssp8:
      return "ssp8";
    default:
      invalid_enum_value(value, "tdd_cfg_s::special_sf_patterns_e_");
  }
  return "";
}
uint8_t tdd_cfg_s::special_sf_patterns_e_::to_number() const
{
  const static uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
  return get_enum_number(options, 9, value, "tdd_cfg_s::special_sf_patterns_e_");
}

// BandclassCDMA2000 ::= ENUMERATED
std::string bandclass_cdma2000_e::to_string() const
{
  switch (value) {
    case bc0:
      return "bc0";
    case bc1:
      return "bc1";
    case bc2:
      return "bc2";
    case bc3:
      return "bc3";
    case bc4:
      return "bc4";
    case bc5:
      return "bc5";
    case bc6:
      return "bc6";
    case bc7:
      return "bc7";
    case bc8:
      return "bc8";
    case bc9:
      return "bc9";
    case bc10:
      return "bc10";
    case bc11:
      return "bc11";
    case bc12:
      return "bc12";
    case bc13:
      return "bc13";
    case bc14:
      return "bc14";
    case bc15:
      return "bc15";
    case bc16:
      return "bc16";
    case bc17:
      return "bc17";
    case bc18_v9a0:
      return "bc18-v9a0";
    case bc19_v9a0:
      return "bc19-v9a0";
    case bc20_v9a0:
      return "bc20-v9a0";
    case bc21_v9a0:
      return "bc21-v9a0";
    case spare10:
      return "spare10";
    case spare9:
      return "spare9";
    case spare8:
      return "spare8";
    case spare7:
      return "spare7";
    case spare6:
      return "spare6";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "bandclass_cdma2000_e");
  }
  return "";
}
uint8_t bandclass_cdma2000_e::to_number() const
{
  const static uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
  return get_enum_number(options, 22, value, "bandclass_cdma2000_e");
}

std::string sl_comm_res_pool_v2x_r14_s::size_subch_r14_e_::to_string() const
{
  switch (value) {
    case n4:
      return "n4";
    case n5:
      return "n5";
    case n6:
      return "n6";
    case n8:
      return "n8";
    case n9:
      return "n9";
    case n10:
      return "n10";
    case n12:
      return "n12";
    case n15:
      return "n15";
    case n16:
      return "n16";
    case n18:
      return "n18";
    case n20:
      return "n20";
    case n25:
      return "n25";
    case n30:
      return "n30";
    case n48:
      return "n48";
    case n50:
      return "n50";
    case n72:
      return "n72";
    case n75:
      return "n75";
    case n96:
      return "n96";
    case n100:
      return "n100";
    case spare13:
      return "spare13";
    case spare12:
      return "spare12";
    case spare11:
      return "spare11";
    case spare10:
      return "spare10";
    case spare9:
      return "spare9";
    case spare8:
      return "spare8";
    case spare7:
      return "spare7";
    case spare6:
      return "spare6";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "sl_comm_res_pool_v2x_r14_s::size_subch_r14_e_");
  }
  return "";
}
uint8_t sl_comm_res_pool_v2x_r14_s::size_subch_r14_e_::to_number() const
{
  const static uint8_t options[] = {4, 5, 6, 8, 9, 10, 12, 15, 16, 18, 20, 25, 30, 48, 50, 72, 75, 96, 100};
  return get_enum_number(options, 19, value, "sl_comm_res_pool_v2x_r14_s::size_subch_r14_e_");
}

std::string sl_comm_res_pool_v2x_r14_s::num_subch_r14_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n3:
      return "n3";
    case n5:
      return "n5";
    case n8:
      return "n8";
    case n10:
      return "n10";
    case n15:
      return "n15";
    case n20:
      return "n20";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "sl_comm_res_pool_v2x_r14_s::num_subch_r14_e_");
  }
  return "";
}
uint8_t sl_comm_res_pool_v2x_r14_s::num_subch_r14_e_::to_number() const
{
  const static uint8_t options[] = {1, 3, 5, 8, 10, 15, 20};
  return get_enum_number(options, 7, value, "sl_comm_res_pool_v2x_r14_s::num_subch_r14_e_");
}

std::string sl_sync_cfg_nfreq_r13_s::rx_params_r13_s_::disc_sync_win_r13_e_::to_string() const
{
  switch (value) {
    case w1:
      return "w1";
    case w2:
      return "w2";
    default:
      invalid_enum_value(value, "sl_sync_cfg_nfreq_r13_s::rx_params_r13_s_::disc_sync_win_r13_e_");
  }
  return "";
}
uint8_t sl_sync_cfg_nfreq_r13_s::rx_params_r13_s_::disc_sync_win_r13_e_::to_number() const
{
  const static uint8_t options[] = {1, 2};
  return get_enum_number(options, 2, value, "sl_sync_cfg_nfreq_r13_s::rx_params_r13_s_::disc_sync_win_r13_e_");
}

std::string tdd_cfg_v1130_s::special_sf_patterns_v1130_e_::to_string() const
{
  switch (value) {
    case ssp7:
      return "ssp7";
    case ssp9:
      return "ssp9";
    default:
      invalid_enum_value(value, "tdd_cfg_v1130_s::special_sf_patterns_v1130_e_");
  }
  return "";
}
uint8_t tdd_cfg_v1130_s::special_sf_patterns_v1130_e_::to_number() const
{
  const static uint8_t options[] = {7, 9};
  return get_enum_number(options, 2, value, "tdd_cfg_v1130_s::special_sf_patterns_v1130_e_");
}

std::string sl_comm_tx_pool_sensing_cfg_r14_s::prob_res_keep_r14_e_::to_string() const
{
  switch (value) {
    case v0:
      return "v0";
    case v0dot2:
      return "v0dot2";
    case v0dot4:
      return "v0dot4";
    case v0dot6:
      return "v0dot6";
    case v0dot8:
      return "v0dot8";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "sl_comm_tx_pool_sensing_cfg_r14_s::prob_res_keep_r14_e_");
  }
  return "";
}
float sl_comm_tx_pool_sensing_cfg_r14_s::prob_res_keep_r14_e_::to_number() const
{
  const static float options[] = {0.0, 0.2, 0.4, 0.6, 0.8};
  return get_enum_number(options, 5, value, "sl_comm_tx_pool_sensing_cfg_r14_s::prob_res_keep_r14_e_");
}
std::string sl_comm_tx_pool_sensing_cfg_r14_s::prob_res_keep_r14_e_::to_number_string() const
{
  switch (value) {
    case v0:
      return "0";
    case v0dot2:
      return "0.2";
    case v0dot4:
      return "0.4";
    case v0dot6:
      return "0.6";
    case v0dot8:
      return "0.8";
    default:
      invalid_enum_number(value, "sl_comm_tx_pool_sensing_cfg_r14_s::prob_res_keep_r14_e_");
  }
  return "";
}

std::string sl_comm_tx_pool_sensing_cfg_r14_s::sl_reselect_after_r14_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n2:
      return "n2";
    case n3:
      return "n3";
    case n4:
      return "n4";
    case n5:
      return "n5";
    case n6:
      return "n6";
    case n7:
      return "n7";
    case n8:
      return "n8";
    case n9:
      return "n9";
    case spare7:
      return "spare7";
    case spare6:
      return "spare6";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "sl_comm_tx_pool_sensing_cfg_r14_s::sl_reselect_after_r14_e_");
  }
  return "";
}
uint8_t sl_comm_tx_pool_sensing_cfg_r14_s::sl_reselect_after_r14_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  return get_enum_number(options, 9, value, "sl_comm_tx_pool_sensing_cfg_r14_s::sl_reselect_after_r14_e_");
}

std::string setup_e::to_string() const
{
  switch (value) {
    case release:
      return "release";
    case setup:
      return "setup";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "setup_e");
  }
  return "";
}

std::string sl_disc_res_pool_r12_s::disc_period_r12_e_::to_string() const
{
  switch (value) {
    case rf32:
      return "rf32";
    case rf64:
      return "rf64";
    case rf128:
      return "rf128";
    case rf256:
      return "rf256";
    case rf512:
      return "rf512";
    case rf1024:
      return "rf1024";
    case rf16_v1310:
      return "rf16-v1310";
    case spare:
      return "spare";
    default:
      invalid_enum_value(value, "sl_disc_res_pool_r12_s::disc_period_r12_e_");
  }
  return "";
}
uint16_t sl_disc_res_pool_r12_s::disc_period_r12_e_::to_number() const
{
  const static uint16_t options[] = {32, 64, 128, 256, 512, 1024, 16};
  return get_enum_number(options, 7, value, "sl_disc_res_pool_r12_s::disc_period_r12_e_");
}

std::string sl_disc_res_pool_r12_s::tx_params_r12_s_::ue_sel_res_cfg_r12_s_::pool_sel_r12_c_::types::to_string() const
{
  switch (value) {
    case rsrp_based_r12:
      return "rsrpBased-r12";
    case random_r12:
      return "random-r12";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value,
                         "sl_disc_res_pool_r12_s::tx_params_r12_s_::ue_sel_res_cfg_r12_s_::pool_sel_r12_c_::types");
  }
  return "";
}

std::string sl_disc_res_pool_r12_s::tx_params_r12_s_::ue_sel_res_cfg_r12_s_::tx_probability_r12_e_::to_string() const
{
  switch (value) {
    case p25:
      return "p25";
    case p50:
      return "p50";
    case p75:
      return "p75";
    case p100:
      return "p100";
    default:
      invalid_enum_value(value,
                         "sl_disc_res_pool_r12_s::tx_params_r12_s_::ue_sel_res_cfg_r12_s_::tx_probability_r12_e_");
  }
  return "";
}
uint8_t sl_disc_res_pool_r12_s::tx_params_r12_s_::ue_sel_res_cfg_r12_s_::tx_probability_r12_e_::to_number() const
{
  const static uint8_t options[] = {25, 50, 75, 100};
  return get_enum_number(options, 4, value,
                         "sl_disc_res_pool_r12_s::tx_params_r12_s_::ue_sel_res_cfg_r12_s_::tx_probability_r12_e_");
}

std::string sl_disc_res_pool_r12_s::disc_period_v1310_c_::setup_e_::to_string() const
{
  switch (value) {
    case rf4:
      return "rf4";
    case rf6:
      return "rf6";
    case rf7:
      return "rf7";
    case rf8:
      return "rf8";
    case rf12:
      return "rf12";
    case rf14:
      return "rf14";
    case rf24:
      return "rf24";
    case rf28:
      return "rf28";
    default:
      invalid_enum_value(value, "sl_disc_res_pool_r12_s::disc_period_v1310_c_::setup_e_");
  }
  return "";
}
uint8_t sl_disc_res_pool_r12_s::disc_period_v1310_c_::setup_e_::to_number() const
{
  const static uint8_t options[] = {4, 6, 7, 8, 12, 14, 24, 28};
  return get_enum_number(options, 8, value, "sl_disc_res_pool_r12_s::disc_period_v1310_c_::setup_e_");
}

std::string sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_r13_c_::setup_s_::freq_info_s_::ul_bw_e_::to_string() const
{
  switch (value) {
    case n6:
      return "n6";
    case n15:
      return "n15";
    case n25:
      return "n25";
    case n50:
      return "n50";
    case n75:
      return "n75";
    case n100:
      return "n100";
    default:
      invalid_enum_value(value,
                         "sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_r13_c_::setup_s_::freq_info_s_::ul_bw_e_");
  }
  return "";
}
uint8_t sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_r13_c_::setup_s_::freq_info_s_::ul_bw_e_::to_number() const
{
  const static uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return get_enum_number(options, 6, value,
                         "sl_disc_res_pool_r12_s::tx_params_add_neigh_freq_r13_c_::setup_s_::freq_info_s_::ul_bw_e_");
}

std::string sl_zone_cfg_r14_s::zone_len_r14_e_::to_string() const
{
  switch (value) {
    case m5:
      return "m5";
    case m10:
      return "m10";
    case m20:
      return "m20";
    case m50:
      return "m50";
    case m100:
      return "m100";
    case m200:
      return "m200";
    case m500:
      return "m500";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "sl_zone_cfg_r14_s::zone_len_r14_e_");
  }
  return "";
}
uint16_t sl_zone_cfg_r14_s::zone_len_r14_e_::to_number() const
{
  const static uint16_t options[] = {5, 10, 20, 50, 100, 200, 500};
  return get_enum_number(options, 7, value, "sl_zone_cfg_r14_s::zone_len_r14_e_");
}

std::string sl_zone_cfg_r14_s::zone_width_r14_e_::to_string() const
{
  switch (value) {
    case m5:
      return "m5";
    case m10:
      return "m10";
    case m20:
      return "m20";
    case m50:
      return "m50";
    case m100:
      return "m100";
    case m200:
      return "m200";
    case m500:
      return "m500";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "sl_zone_cfg_r14_s::zone_width_r14_e_");
  }
  return "";
}
uint16_t sl_zone_cfg_r14_s::zone_width_r14_e_::to_number() const
{
  const static uint16_t options[] = {5, 10, 20, 50, 100, 200, 500};
  return get_enum_number(options, 7, value, "sl_zone_cfg_r14_s::zone_width_r14_e_");
}

std::string pci_range_s::range_e_::to_string() const
{
  switch (value) {
    case n4:
      return "n4";
    case n8:
      return "n8";
    case n12:
      return "n12";
    case n16:
      return "n16";
    case n24:
      return "n24";
    case n32:
      return "n32";
    case n48:
      return "n48";
    case n64:
      return "n64";
    case n84:
      return "n84";
    case n96:
      return "n96";
    case n128:
      return "n128";
    case n168:
      return "n168";
    case n252:
      return "n252";
    case n504:
      return "n504";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "pci_range_s::range_e_");
  }
  return "";
}
uint16_t pci_range_s::range_e_::to_number() const
{
  const static uint16_t options[] = {4, 8, 12, 16, 24, 32, 48, 64, 84, 96, 128, 168, 252, 504};
  return get_enum_number(options, 14, value, "pci_range_s::range_e_");
}

// Q-OffsetRange ::= ENUMERATED
std::string q_offset_range_e::to_string() const
{
  switch (value) {
    case db_minus24:
      return "dB-24";
    case db_minus22:
      return "dB-22";
    case db_minus20:
      return "dB-20";
    case db_minus18:
      return "dB-18";
    case db_minus16:
      return "dB-16";
    case db_minus14:
      return "dB-14";
    case db_minus12:
      return "dB-12";
    case db_minus10:
      return "dB-10";
    case db_minus8:
      return "dB-8";
    case db_minus6:
      return "dB-6";
    case db_minus5:
      return "dB-5";
    case db_minus4:
      return "dB-4";
    case db_minus3:
      return "dB-3";
    case db_minus2:
      return "dB-2";
    case db_minus1:
      return "dB-1";
    case db0:
      return "dB0";
    case db1:
      return "dB1";
    case db2:
      return "dB2";
    case db3:
      return "dB3";
    case db4:
      return "dB4";
    case db5:
      return "dB5";
    case db6:
      return "dB6";
    case db8:
      return "dB8";
    case db10:
      return "dB10";
    case db12:
      return "dB12";
    case db14:
      return "dB14";
    case db16:
      return "dB16";
    case db18:
      return "dB18";
    case db20:
      return "dB20";
    case db22:
      return "dB22";
    case db24:
      return "dB24";
    default:
      invalid_enum_value(value, "q_offset_range_e");
  }
  return "";
}
int8_t q_offset_range_e::to_number() const
{
  const static int8_t options[] = {-24, -22, -20, -18, -16, -14, -12, -10, -8, -6, -5, -4, -3, -2, -1, 0,
                                   1,   2,   3,   4,   5,   6,   8,   10,  12, 14, 16, 18, 20, 22, 24};
  return get_enum_number(options, 31, value, "q_offset_range_e");
}

std::string sched_info_br_r13_s::si_tbs_r13_e_::to_string() const
{
  switch (value) {
    case b152:
      return "b152";
    case b208:
      return "b208";
    case b256:
      return "b256";
    case b328:
      return "b328";
    case b408:
      return "b408";
    case b504:
      return "b504";
    case b600:
      return "b600";
    case b712:
      return "b712";
    case b808:
      return "b808";
    case b936:
      return "b936";
    default:
      invalid_enum_value(value, "sched_info_br_r13_s::si_tbs_r13_e_");
  }
  return "";
}
uint16_t sched_info_br_r13_s::si_tbs_r13_e_::to_number() const
{
  const static uint16_t options[] = {152, 208, 256, 328, 408, 504, 600, 712, 808, 936};
  return get_enum_number(options, 10, value, "sched_info_br_r13_s::si_tbs_r13_e_");
}

std::string speed_state_scale_factors_s::sf_medium_e_::to_string() const
{
  switch (value) {
    case o_dot25:
      return "oDot25";
    case o_dot5:
      return "oDot5";
    case o_dot75:
      return "oDot75";
    case l_dot0:
      return "lDot0";
    default:
      invalid_enum_value(value, "speed_state_scale_factors_s::sf_medium_e_");
  }
  return "";
}
float speed_state_scale_factors_s::sf_medium_e_::to_number() const
{
  const static float options[] = {0.25, 0.5, 0.75, 1.0};
  return get_enum_number(options, 4, value, "speed_state_scale_factors_s::sf_medium_e_");
}
std::string speed_state_scale_factors_s::sf_medium_e_::to_number_string() const
{
  switch (value) {
    case o_dot25:
      return "0.25";
    case o_dot5:
      return "0.5";
    case o_dot75:
      return "0.75";
    case l_dot0:
      return "1.0";
    default:
      invalid_enum_number(value, "speed_state_scale_factors_s::sf_medium_e_");
  }
  return "";
}

std::string speed_state_scale_factors_s::sf_high_e_::to_string() const
{
  switch (value) {
    case o_dot25:
      return "oDot25";
    case o_dot5:
      return "oDot5";
    case o_dot75:
      return "oDot75";
    case l_dot0:
      return "lDot0";
    default:
      invalid_enum_value(value, "speed_state_scale_factors_s::sf_high_e_");
  }
  return "";
}
float speed_state_scale_factors_s::sf_high_e_::to_number() const
{
  const static float options[] = {0.25, 0.5, 0.75, 1.0};
  return get_enum_number(options, 4, value, "speed_state_scale_factors_s::sf_high_e_");
}
std::string speed_state_scale_factors_s::sf_high_e_::to_number_string() const
{
  switch (value) {
    case o_dot25:
      return "0.25";
    case o_dot5:
      return "0.5";
    case o_dot75:
      return "0.75";
    case l_dot0:
      return "1.0";
    default:
      invalid_enum_number(value, "speed_state_scale_factors_s::sf_high_e_");
  }
  return "";
}

// AllowedMeasBandwidth ::= ENUMERATED
std::string allowed_meas_bw_e::to_string() const
{
  switch (value) {
    case mbw6:
      return "mbw6";
    case mbw15:
      return "mbw15";
    case mbw25:
      return "mbw25";
    case mbw50:
      return "mbw50";
    case mbw75:
      return "mbw75";
    case mbw100:
      return "mbw100";
    default:
      invalid_enum_value(value, "allowed_meas_bw_e");
  }
  return "";
}
uint8_t allowed_meas_bw_e::to_number() const
{
  const static uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return get_enum_number(options, 6, value, "allowed_meas_bw_e");
}

// BandIndicatorGERAN ::= ENUMERATED
std::string band_ind_geran_e::to_string() const
{
  switch (value) {
    case dcs1800:
      return "dcs1800";
    case pcs1900:
      return "pcs1900";
    default:
      invalid_enum_value(value, "band_ind_geran_e");
  }
  return "";
}
uint16_t band_ind_geran_e::to_number() const
{
  const static uint16_t options[] = {1800, 1900};
  return get_enum_number(options, 2, value, "band_ind_geran_e");
}

std::string barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_factor_r13_e_::to_string() const
{
  switch (value) {
    case p00:
      return "p00";
    case p05:
      return "p05";
    case p10:
      return "p10";
    case p15:
      return "p15";
    case p20:
      return "p20";
    case p25:
      return "p25";
    case p30:
      return "p30";
    case p40:
      return "p40";
    case p50:
      return "p50";
    case p60:
      return "p60";
    case p70:
      return "p70";
    case p75:
      return "p75";
    case p80:
      return "p80";
    case p85:
      return "p85";
    case p90:
      return "p90";
    case p95:
      return "p95";
    default:
      invalid_enum_value(value, "barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_factor_r13_e_");
  }
  return "";
}
float barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_factor_r13_e_::to_number() const
{
  const static float options[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 6.0, 7.0, 7.5, 8.0, 8.5, 9.0, 9.5};
  return get_enum_number(options, 16, value,
                         "barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_factor_r13_e_");
}
std::string barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_factor_r13_e_::to_number_string() const
{
  switch (value) {
    case p00:
      return "0.0";
    case p05:
      return "0.5";
    case p10:
      return "1.0";
    case p15:
      return "1.5";
    case p20:
      return "2.0";
    case p25:
      return "2.5";
    case p30:
      return "3.0";
    case p40:
      return "4.0";
    case p50:
      return "5.0";
    case p60:
      return "6.0";
    case p70:
      return "7.0";
    case p75:
      return "7.5";
    case p80:
      return "8.0";
    case p85:
      return "8.5";
    case p90:
      return "9.0";
    case p95:
      return "9.5";
    default:
      invalid_enum_number(value, "barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_factor_r13_e_");
  }
  return "";
}

std::string barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_time_r13_e_::to_string() const
{
  switch (value) {
    case s4:
      return "s4";
    case s8:
      return "s8";
    case s16:
      return "s16";
    case s32:
      return "s32";
    case s64:
      return "s64";
    case s128:
      return "s128";
    case s256:
      return "s256";
    case s512:
      return "s512";
    default:
      invalid_enum_value(value, "barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_time_r13_e_");
  }
  return "";
}
uint16_t barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_time_r13_e_::to_number() const
{
  const static uint16_t options[] = {4, 8, 16, 32, 64, 128, 256, 512};
  return get_enum_number(options, 8, value, "barr_per_acdc_category_r13_s::acdc_barr_cfg_r13_s_::ac_barr_time_r13_e_");
}

std::string cell_sel_info_nfreq_r13_s::q_hyst_r13_e_::to_string() const
{
  switch (value) {
    case db0:
      return "dB0";
    case db1:
      return "dB1";
    case db2:
      return "dB2";
    case db3:
      return "dB3";
    case db4:
      return "dB4";
    case db5:
      return "dB5";
    case db6:
      return "dB6";
    case db8:
      return "dB8";
    case db10:
      return "dB10";
    case db12:
      return "dB12";
    case db14:
      return "dB14";
    case db16:
      return "dB16";
    case db18:
      return "dB18";
    case db20:
      return "dB20";
    case db22:
      return "dB22";
    case db24:
      return "dB24";
    default:
      invalid_enum_value(value, "cell_sel_info_nfreq_r13_s::q_hyst_r13_e_");
  }
  return "";
}
uint8_t cell_sel_info_nfreq_r13_s::q_hyst_r13_e_::to_number() const
{
  const static uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24};
  return get_enum_number(options, 16, value, "cell_sel_info_nfreq_r13_s::q_hyst_r13_e_");
}

std::string plmn_id_info2_r12_c::types::to_string() const
{
  switch (value) {
    case plmn_idx_r12:
      return "plmn-Index-r12";
    case plmn_id_r12:
      return "plmnIdentity-r12";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "plmn_id_info2_r12_c::types");
  }
  return "";
}

std::string prach_params_ce_r13_s::prach_start_sf_r13_e_::to_string() const
{
  switch (value) {
    case sf2:
      return "sf2";
    case sf4:
      return "sf4";
    case sf8:
      return "sf8";
    case sf16:
      return "sf16";
    case sf32:
      return "sf32";
    case sf64:
      return "sf64";
    case sf128:
      return "sf128";
    case sf256:
      return "sf256";
    default:
      invalid_enum_value(value, "prach_params_ce_r13_s::prach_start_sf_r13_e_");
  }
  return "";
}
uint16_t prach_params_ce_r13_s::prach_start_sf_r13_e_::to_number() const
{
  const static uint16_t options[] = {2, 4, 8, 16, 32, 64, 128, 256};
  return get_enum_number(options, 8, value, "prach_params_ce_r13_s::prach_start_sf_r13_e_");
}

std::string prach_params_ce_r13_s::max_num_preamb_attempt_ce_r13_e_::to_string() const
{
  switch (value) {
    case n3:
      return "n3";
    case n4:
      return "n4";
    case n5:
      return "n5";
    case n6:
      return "n6";
    case n7:
      return "n7";
    case n8:
      return "n8";
    case n10:
      return "n10";
    default:
      invalid_enum_value(value, "prach_params_ce_r13_s::max_num_preamb_attempt_ce_r13_e_");
  }
  return "";
}
uint8_t prach_params_ce_r13_s::max_num_preamb_attempt_ce_r13_e_::to_number() const
{
  const static uint8_t options[] = {3, 4, 5, 6, 7, 8, 10};
  return get_enum_number(options, 7, value, "prach_params_ce_r13_s::max_num_preamb_attempt_ce_r13_e_");
}

std::string prach_params_ce_r13_s::num_repeat_per_preamb_attempt_r13_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n2:
      return "n2";
    case n4:
      return "n4";
    case n8:
      return "n8";
    case n16:
      return "n16";
    case n32:
      return "n32";
    case n64:
      return "n64";
    case n128:
      return "n128";
    default:
      invalid_enum_value(value, "prach_params_ce_r13_s::num_repeat_per_preamb_attempt_r13_e_");
  }
  return "";
}
uint8_t prach_params_ce_r13_s::num_repeat_per_preamb_attempt_r13_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 4, 8, 16, 32, 64, 128};
  return get_enum_number(options, 8, value, "prach_params_ce_r13_s::num_repeat_per_preamb_attempt_r13_e_");
}

std::string prach_params_ce_r13_s::mpdcch_num_repeat_ra_r13_e_::to_string() const
{
  switch (value) {
    case r1:
      return "r1";
    case r2:
      return "r2";
    case r4:
      return "r4";
    case r8:
      return "r8";
    case r16:
      return "r16";
    case r32:
      return "r32";
    case r64:
      return "r64";
    case r128:
      return "r128";
    case r256:
      return "r256";
    default:
      invalid_enum_value(value, "prach_params_ce_r13_s::mpdcch_num_repeat_ra_r13_e_");
  }
  return "";
}
uint16_t prach_params_ce_r13_s::mpdcch_num_repeat_ra_r13_e_::to_number() const
{
  const static uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
  return get_enum_number(options, 9, value, "prach_params_ce_r13_s::mpdcch_num_repeat_ra_r13_e_");
}

std::string prach_params_ce_r13_s::prach_hop_cfg_r13_e_::to_string() const
{
  switch (value) {
    case on:
      return "on";
    case off:
      return "off";
    default:
      invalid_enum_value(value, "prach_params_ce_r13_s::prach_hop_cfg_r13_e_");
  }
  return "";
}

std::string rach_ce_level_info_r13_s::ra_resp_win_size_r13_e_::to_string() const
{
  switch (value) {
    case sf20:
      return "sf20";
    case sf50:
      return "sf50";
    case sf80:
      return "sf80";
    case sf120:
      return "sf120";
    case sf180:
      return "sf180";
    case sf240:
      return "sf240";
    case sf320:
      return "sf320";
    case sf400:
      return "sf400";
    default:
      invalid_enum_value(value, "rach_ce_level_info_r13_s::ra_resp_win_size_r13_e_");
  }
  return "";
}
uint16_t rach_ce_level_info_r13_s::ra_resp_win_size_r13_e_::to_number() const
{
  const static uint16_t options[] = {20, 50, 80, 120, 180, 240, 320, 400};
  return get_enum_number(options, 8, value, "rach_ce_level_info_r13_s::ra_resp_win_size_r13_e_");
}

std::string rach_ce_level_info_r13_s::mac_contention_resolution_timer_r13_e_::to_string() const
{
  switch (value) {
    case sf80:
      return "sf80";
    case sf100:
      return "sf100";
    case sf120:
      return "sf120";
    case sf160:
      return "sf160";
    case sf200:
      return "sf200";
    case sf240:
      return "sf240";
    case sf480:
      return "sf480";
    case sf960:
      return "sf960";
    default:
      invalid_enum_value(value, "rach_ce_level_info_r13_s::mac_contention_resolution_timer_r13_e_");
  }
  return "";
}
uint16_t rach_ce_level_info_r13_s::mac_contention_resolution_timer_r13_e_::to_number() const
{
  const static uint16_t options[] = {80, 100, 120, 160, 200, 240, 480, 960};
  return get_enum_number(options, 8, value, "rach_ce_level_info_r13_s::mac_contention_resolution_timer_r13_e_");
}

std::string rach_ce_level_info_r13_s::rar_hop_cfg_r13_e_::to_string() const
{
  switch (value) {
    case on:
      return "on";
    case off:
      return "off";
    default:
      invalid_enum_value(value, "rach_ce_level_info_r13_s::rar_hop_cfg_r13_e_");
  }
  return "";
}

std::string rach_ce_level_info_r13_s::edt_params_r15_s_::edt_tbs_r15_e_::to_string() const
{
  switch (value) {
    case b328:
      return "b328";
    case b408:
      return "b408";
    case b504:
      return "b504";
    case b600:
      return "b600";
    case b712:
      return "b712";
    case b808:
      return "b808";
    case b936:
      return "b936";
    case b1000or456:
      return "b1000or456";
    default:
      invalid_enum_value(value, "rach_ce_level_info_r13_s::edt_params_r15_s_::edt_tbs_r15_e_");
  }
  return "";
}
uint16_t rach_ce_level_info_r13_s::edt_params_r15_s_::edt_tbs_r15_e_::to_number() const
{
  const static uint16_t options[] = {328, 408, 504, 600, 712, 808, 936, 1000};
  return get_enum_number(options, 8, value, "rach_ce_level_info_r13_s::edt_params_r15_s_::edt_tbs_r15_e_");
}

std::string rach_ce_level_info_r13_s::edt_params_r15_s_::mac_contention_resolution_timer_r15_e_::to_string() const
{
  switch (value) {
    case sf240:
      return "sf240";
    case sf480:
      return "sf480";
    case sf960:
      return "sf960";
    case sf1920:
      return "sf1920";
    case sf3840:
      return "sf3840";
    case sf5760:
      return "sf5760";
    case sf7680:
      return "sf7680";
    case sf10240:
      return "sf10240";
    default:
      invalid_enum_value(value, "rach_ce_level_info_r13_s::edt_params_r15_s_::mac_contention_resolution_timer_r15_e_");
  }
  return "";
}
uint16_t rach_ce_level_info_r13_s::edt_params_r15_s_::mac_contention_resolution_timer_r15_e_::to_number() const
{
  const static uint16_t options[] = {240, 480, 960, 1920, 3840, 5760, 7680, 10240};
  return get_enum_number(options, 8, value,
                         "rach_ce_level_info_r13_s::edt_params_r15_s_::mac_contention_resolution_timer_r15_e_");
}

std::string sl_disc_tx_res_inter_freq_r13_c::types::to_string() const
{
  switch (value) {
    case acquire_si_from_carrier_r13:
      return "acquireSI-FromCarrier-r13";
    case disc_tx_pool_common_r13:
      return "discTxPoolCommon-r13";
    case request_ded_r13:
      return "requestDedicated-r13";
    case no_tx_on_carrier_r13:
      return "noTxOnCarrier-r13";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sl_disc_tx_res_inter_freq_r13_c::types");
  }
  return "";
}

std::string sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::mpdcch_pdsch_hop_nb_r13_e_::to_string() const
{
  switch (value) {
    case nb2:
      return "nb2";
    case nb4:
      return "nb4";
    default:
      invalid_enum_value(value, "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::mpdcch_pdsch_hop_nb_r13_e_");
  }
  return "";
}
uint8_t sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::mpdcch_pdsch_hop_nb_r13_e_::to_number() const
{
  const static uint8_t options[] = {2, 4};
  return get_enum_number(options, 2, value,
                         "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::mpdcch_pdsch_hop_nb_r13_e_");
}

std::string
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::interv_fdd_r13_e_::to_string()
    const
{
  switch (value) {
    case int1:
      return "int1";
    case int2:
      return "int2";
    case int4:
      return "int4";
    case int8:
      return "int8";
    default:
      invalid_enum_value(value, "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_"
                                "c_::interv_fdd_r13_e_");
  }
  return "";
}
uint8_t
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::interv_fdd_r13_e_::to_number()
    const
{
  const static uint8_t options[] = {1, 2, 4, 8};
  return get_enum_number(
      options, 4, value,
      "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::interv_fdd_r13_e_");
}

std::string
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::interv_tdd_r13_e_::to_string()
    const
{
  switch (value) {
    case int1:
      return "int1";
    case int5:
      return "int5";
    case int10:
      return "int10";
    case int20:
      return "int20";
    default:
      invalid_enum_value(value, "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_"
                                "c_::interv_tdd_r13_e_");
  }
  return "";
}
uint8_t
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::interv_tdd_r13_e_::to_number()
    const
{
  const static uint8_t options[] = {1, 5, 10, 20};
  return get_enum_number(
      options, 4, value,
      "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::interv_tdd_r13_e_");
}

std::string
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::types::to_string() const
{
  switch (value) {
    case interv_fdd_r13:
      return "interval-FDD-r13";
    case interv_tdd_r13:
      return "interval-TDD-r13";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(
          value, "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_a_r13_c_::types");
  }
  return "";
}

std::string
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::interv_fdd_r13_e_::to_string()
    const
{
  switch (value) {
    case int2:
      return "int2";
    case int4:
      return "int4";
    case int8:
      return "int8";
    case int16:
      return "int16";
    default:
      invalid_enum_value(value, "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_"
                                "c_::interv_fdd_r13_e_");
  }
  return "";
}
uint8_t
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::interv_fdd_r13_e_::to_number()
    const
{
  const static uint8_t options[] = {2, 4, 8, 16};
  return get_enum_number(
      options, 4, value,
      "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::interv_fdd_r13_e_");
}

std::string
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::interv_tdd_r13_e_::to_string()
    const
{
  switch (value) {
    case int5:
      return "int5";
    case int10:
      return "int10";
    case int20:
      return "int20";
    case int40:
      return "int40";
    default:
      invalid_enum_value(value, "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_"
                                "c_::interv_tdd_r13_e_");
  }
  return "";
}
uint8_t
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::interv_tdd_r13_e_::to_number()
    const
{
  const static uint8_t options[] = {5, 10, 20, 40};
  return get_enum_number(
      options, 4, value,
      "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::interv_tdd_r13_e_");
}

std::string
sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::types::to_string() const
{
  switch (value) {
    case interv_fdd_r13:
      return "interval-FDD-r13";
    case interv_tdd_r13:
      return "interval-TDD-r13";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(
          value, "sib_type1_v1320_ies_s::freq_hop_params_dl_r13_s_::interv_dl_hop_cfg_common_mode_b_r13_c_::types");
  }
  return "";
}

std::string sys_time_info_cdma2000_s::cdma_sys_time_c_::types::to_string() const
{
  switch (value) {
    case sync_sys_time:
      return "synchronousSystemTime";
    case async_sys_time:
      return "asynchronousSystemTime";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sys_time_info_cdma2000_s::cdma_sys_time_c_::types");
  }
  return "";
}

std::string ac_barr_cfg_s::ac_barr_factor_e_::to_string() const
{
  switch (value) {
    case p00:
      return "p00";
    case p05:
      return "p05";
    case p10:
      return "p10";
    case p15:
      return "p15";
    case p20:
      return "p20";
    case p25:
      return "p25";
    case p30:
      return "p30";
    case p40:
      return "p40";
    case p50:
      return "p50";
    case p60:
      return "p60";
    case p70:
      return "p70";
    case p75:
      return "p75";
    case p80:
      return "p80";
    case p85:
      return "p85";
    case p90:
      return "p90";
    case p95:
      return "p95";
    default:
      invalid_enum_value(value, "ac_barr_cfg_s::ac_barr_factor_e_");
  }
  return "";
}
float ac_barr_cfg_s::ac_barr_factor_e_::to_number() const
{
  const static float options[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 6.0, 7.0, 7.5, 8.0, 8.5, 9.0, 9.5};
  return get_enum_number(options, 16, value, "ac_barr_cfg_s::ac_barr_factor_e_");
}
std::string ac_barr_cfg_s::ac_barr_factor_e_::to_number_string() const
{
  switch (value) {
    case p00:
      return "0.0";
    case p05:
      return "0.5";
    case p10:
      return "1.0";
    case p15:
      return "1.5";
    case p20:
      return "2.0";
    case p25:
      return "2.5";
    case p30:
      return "3.0";
    case p40:
      return "4.0";
    case p50:
      return "5.0";
    case p60:
      return "6.0";
    case p70:
      return "7.0";
    case p75:
      return "7.5";
    case p80:
      return "8.0";
    case p85:
      return "8.5";
    case p90:
      return "9.0";
    case p95:
      return "9.5";
    default:
      invalid_enum_number(value, "ac_barr_cfg_s::ac_barr_factor_e_");
  }
  return "";
}

std::string ac_barr_cfg_s::ac_barr_time_e_::to_string() const
{
  switch (value) {
    case s4:
      return "s4";
    case s8:
      return "s8";
    case s16:
      return "s16";
    case s32:
      return "s32";
    case s64:
      return "s64";
    case s128:
      return "s128";
    case s256:
      return "s256";
    case s512:
      return "s512";
    default:
      invalid_enum_value(value, "ac_barr_cfg_s::ac_barr_time_e_");
  }
  return "";
}
uint16_t ac_barr_cfg_s::ac_barr_time_e_::to_number() const
{
  const static uint16_t options[] = {4, 8, 16, 32, 64, 128, 256, 512};
  return get_enum_number(options, 8, value, "ac_barr_cfg_s::ac_barr_time_e_");
}

std::string carrier_freqs_geran_s::following_arfcns_c_::types::to_string() const
{
  switch (value) {
    case explicit_list_of_arfcns:
      return "explicitListOfARFCNs";
    case equally_spaced_arfcns:
      return "equallySpacedARFCNs";
    case variable_bit_map_of_arfcns:
      return "variableBitMapOfARFCNs";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "carrier_freqs_geran_s::following_arfcns_c_::types");
  }
  return "";
}

// CellReselectionSubPriority-r13 ::= ENUMERATED
std::string cell_resel_sub_prio_r13_e::to_string() const
{
  switch (value) {
    case o_dot2:
      return "oDot2";
    case o_dot4:
      return "oDot4";
    case o_dot6:
      return "oDot6";
    case o_dot8:
      return "oDot8";
    default:
      invalid_enum_value(value, "cell_resel_sub_prio_r13_e");
  }
  return "";
}
float cell_resel_sub_prio_r13_e::to_number() const
{
  const static float options[] = {0.2, 0.4, 0.6, 0.8};
  return get_enum_number(options, 4, value, "cell_resel_sub_prio_r13_e");
}
std::string cell_resel_sub_prio_r13_e::to_number_string() const
{
  switch (value) {
    case o_dot2:
      return "0.2";
    case o_dot4:
      return "0.4";
    case o_dot6:
      return "0.6";
    case o_dot8:
      return "0.8";
    default:
      invalid_enum_number(value, "cell_resel_sub_prio_r13_e");
  }
  return "";
}

std::string delta_flist_pucch_s::delta_f_pucch_format1_e_::to_string() const
{
  switch (value) {
    case delta_f_minus2:
      return "deltaF-2";
    case delta_f0:
      return "deltaF0";
    case delta_f2:
      return "deltaF2";
    default:
      invalid_enum_value(value, "delta_flist_pucch_s::delta_f_pucch_format1_e_");
  }
  return "";
}
int8_t delta_flist_pucch_s::delta_f_pucch_format1_e_::to_number() const
{
  const static int8_t options[] = {-2, 0, 2};
  return get_enum_number(options, 3, value, "delta_flist_pucch_s::delta_f_pucch_format1_e_");
}

std::string delta_flist_pucch_s::delta_f_pucch_format1b_e_::to_string() const
{
  switch (value) {
    case delta_f1:
      return "deltaF1";
    case delta_f3:
      return "deltaF3";
    case delta_f5:
      return "deltaF5";
    default:
      invalid_enum_value(value, "delta_flist_pucch_s::delta_f_pucch_format1b_e_");
  }
  return "";
}
uint8_t delta_flist_pucch_s::delta_f_pucch_format1b_e_::to_number() const
{
  const static uint8_t options[] = {1, 3, 5};
  return get_enum_number(options, 3, value, "delta_flist_pucch_s::delta_f_pucch_format1b_e_");
}

std::string delta_flist_pucch_s::delta_f_pucch_format2_e_::to_string() const
{
  switch (value) {
    case delta_f_minus2:
      return "deltaF-2";
    case delta_f0:
      return "deltaF0";
    case delta_f1:
      return "deltaF1";
    case delta_f2:
      return "deltaF2";
    default:
      invalid_enum_value(value, "delta_flist_pucch_s::delta_f_pucch_format2_e_");
  }
  return "";
}
int8_t delta_flist_pucch_s::delta_f_pucch_format2_e_::to_number() const
{
  const static int8_t options[] = {-2, 0, 1, 2};
  return get_enum_number(options, 4, value, "delta_flist_pucch_s::delta_f_pucch_format2_e_");
}

std::string delta_flist_pucch_s::delta_f_pucch_format2a_e_::to_string() const
{
  switch (value) {
    case delta_f_minus2:
      return "deltaF-2";
    case delta_f0:
      return "deltaF0";
    case delta_f2:
      return "deltaF2";
    default:
      invalid_enum_value(value, "delta_flist_pucch_s::delta_f_pucch_format2a_e_");
  }
  return "";
}
int8_t delta_flist_pucch_s::delta_f_pucch_format2a_e_::to_number() const
{
  const static int8_t options[] = {-2, 0, 2};
  return get_enum_number(options, 3, value, "delta_flist_pucch_s::delta_f_pucch_format2a_e_");
}

std::string delta_flist_pucch_s::delta_f_pucch_format2b_e_::to_string() const
{
  switch (value) {
    case delta_f_minus2:
      return "deltaF-2";
    case delta_f0:
      return "deltaF0";
    case delta_f2:
      return "deltaF2";
    default:
      invalid_enum_value(value, "delta_flist_pucch_s::delta_f_pucch_format2b_e_");
  }
  return "";
}
int8_t delta_flist_pucch_s::delta_f_pucch_format2b_e_::to_number() const
{
  const static int8_t options[] = {-2, 0, 2};
  return get_enum_number(options, 3, value, "delta_flist_pucch_s::delta_f_pucch_format2b_e_");
}

std::string edt_prach_params_ce_r15_s::edt_prach_params_ce_r15_s_::prach_start_sf_r15_e_::to_string() const
{
  switch (value) {
    case sf2:
      return "sf2";
    case sf4:
      return "sf4";
    case sf8:
      return "sf8";
    case sf16:
      return "sf16";
    case sf32:
      return "sf32";
    case sf64:
      return "sf64";
    case sf128:
      return "sf128";
    case sf256:
      return "sf256";
    default:
      invalid_enum_value(value, "edt_prach_params_ce_r15_s::edt_prach_params_ce_r15_s_::prach_start_sf_r15_e_");
  }
  return "";
}
uint16_t edt_prach_params_ce_r15_s::edt_prach_params_ce_r15_s_::prach_start_sf_r15_e_::to_number() const
{
  const static uint16_t options[] = {2, 4, 8, 16, 32, 64, 128, 256};
  return get_enum_number(options, 8, value,
                         "edt_prach_params_ce_r15_s::edt_prach_params_ce_r15_s_::prach_start_sf_r15_e_");
}

// FilterCoefficient ::= ENUMERATED
std::string filt_coef_e::to_string() const
{
  switch (value) {
    case fc0:
      return "fc0";
    case fc1:
      return "fc1";
    case fc2:
      return "fc2";
    case fc3:
      return "fc3";
    case fc4:
      return "fc4";
    case fc5:
      return "fc5";
    case fc6:
      return "fc6";
    case fc7:
      return "fc7";
    case fc8:
      return "fc8";
    case fc9:
      return "fc9";
    case fc11:
      return "fc11";
    case fc13:
      return "fc13";
    case fc15:
      return "fc15";
    case fc17:
      return "fc17";
    case fc19:
      return "fc19";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "filt_coef_e");
  }
  return "";
}
uint8_t filt_coef_e::to_number() const
{
  const static uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 13, 15, 17, 19};
  return get_enum_number(options, 15, value, "filt_coef_e");
}

std::string mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_::types::to_string() const
{
  switch (value) {
    case sf5_r15:
      return "sf5-r15";
    case sf10_r15:
      return "sf10-r15";
    case sf20_r15:
      return "sf20-r15";
    case sf40_r15:
      return "sf40-r15";
    case sf80_r15:
      return "sf80-r15";
    case sf160_r15:
      return "sf160-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_::types");
  }
  return "";
}
uint8_t mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_::types::to_number() const
{
  const static uint8_t options[] = {5, 10, 20, 40, 80, 160};
  return get_enum_number(options, 6, value, "mtc_ssb_nr_r15_s::periodicity_and_offset_r15_c_::types");
}

std::string mtc_ssb_nr_r15_s::ssb_dur_r15_e_::to_string() const
{
  switch (value) {
    case sf1:
      return "sf1";
    case sf2:
      return "sf2";
    case sf3:
      return "sf3";
    case sf4:
      return "sf4";
    case sf5:
      return "sf5";
    default:
      invalid_enum_value(value, "mtc_ssb_nr_r15_s::ssb_dur_r15_e_");
  }
  return "";
}
uint8_t mtc_ssb_nr_r15_s::ssb_dur_r15_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 4, 5};
  return get_enum_number(options, 5, value, "mtc_ssb_nr_r15_s::ssb_dur_r15_e_");
}

std::string meas_idle_carrier_eutra_r15_s::report_quantities_e_::to_string() const
{
  switch (value) {
    case rsrp:
      return "rsrp";
    case rsrq:
      return "rsrq";
    case both:
      return "both";
    default:
      invalid_enum_value(value, "meas_idle_carrier_eutra_r15_s::report_quantities_e_");
  }
  return "";
}

std::string params_cdma2000_r11_s::sys_time_info_r11_c_::types::to_string() const
{
  switch (value) {
    case explicit_value:
      return "explicitValue";
    case default_value:
      return "defaultValue";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "params_cdma2000_r11_s::sys_time_info_r11_c_::types");
  }
  return "";
}

std::string pwr_ramp_params_s::pwr_ramp_step_e_::to_string() const
{
  switch (value) {
    case db0:
      return "dB0";
    case db2:
      return "dB2";
    case db4:
      return "dB4";
    case db6:
      return "dB6";
    default:
      invalid_enum_value(value, "pwr_ramp_params_s::pwr_ramp_step_e_");
  }
  return "";
}
uint8_t pwr_ramp_params_s::pwr_ramp_step_e_::to_number() const
{
  const static uint8_t options[] = {0, 2, 4, 6};
  return get_enum_number(options, 4, value, "pwr_ramp_params_s::pwr_ramp_step_e_");
}

std::string pwr_ramp_params_s::preamb_init_rx_target_pwr_e_::to_string() const
{
  switch (value) {
    case dbm_minus120:
      return "dBm-120";
    case dbm_minus118:
      return "dBm-118";
    case dbm_minus116:
      return "dBm-116";
    case dbm_minus114:
      return "dBm-114";
    case dbm_minus112:
      return "dBm-112";
    case dbm_minus110:
      return "dBm-110";
    case dbm_minus108:
      return "dBm-108";
    case dbm_minus106:
      return "dBm-106";
    case dbm_minus104:
      return "dBm-104";
    case dbm_minus102:
      return "dBm-102";
    case dbm_minus100:
      return "dBm-100";
    case dbm_minus98:
      return "dBm-98";
    case dbm_minus96:
      return "dBm-96";
    case dbm_minus94:
      return "dBm-94";
    case dbm_minus92:
      return "dBm-92";
    case dbm_minus90:
      return "dBm-90";
    default:
      invalid_enum_value(value, "pwr_ramp_params_s::preamb_init_rx_target_pwr_e_");
  }
  return "";
}
int8_t pwr_ramp_params_s::preamb_init_rx_target_pwr_e_::to_number() const
{
  const static int8_t options[] = {-120, -118, -116, -114, -112, -110, -108, -106,
                                   -104, -102, -100, -98,  -96,  -94,  -92,  -90};
  return get_enum_number(options, 16, value, "pwr_ramp_params_s::preamb_init_rx_target_pwr_e_");
}

// PreambleTransMax ::= ENUMERATED
std::string preamb_trans_max_e::to_string() const
{
  switch (value) {
    case n3:
      return "n3";
    case n4:
      return "n4";
    case n5:
      return "n5";
    case n6:
      return "n6";
    case n7:
      return "n7";
    case n8:
      return "n8";
    case n10:
      return "n10";
    case n20:
      return "n20";
    case n50:
      return "n50";
    case n100:
      return "n100";
    case n200:
      return "n200";
    default:
      invalid_enum_value(value, "preamb_trans_max_e");
  }
  return "";
}
uint8_t preamb_trans_max_e::to_number() const
{
  const static uint8_t options[] = {3, 4, 5, 6, 7, 8, 10, 20, 50, 100, 200};
  return get_enum_number(options, 11, value, "preamb_trans_max_e");
}

std::string sl_hop_cfg_comm_r12_s::num_subbands_r12_e_::to_string() const
{
  switch (value) {
    case ns1:
      return "ns1";
    case ns2:
      return "ns2";
    case ns4:
      return "ns4";
    default:
      invalid_enum_value(value, "sl_hop_cfg_comm_r12_s::num_subbands_r12_e_");
  }
  return "";
}
uint8_t sl_hop_cfg_comm_r12_s::num_subbands_r12_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 4};
  return get_enum_number(options, 3, value, "sl_hop_cfg_comm_r12_s::num_subbands_r12_e_");
}

std::string sl_inter_freq_info_v2x_r14_s::sl_bw_r14_e_::to_string() const
{
  switch (value) {
    case n6:
      return "n6";
    case n15:
      return "n15";
    case n25:
      return "n25";
    case n50:
      return "n50";
    case n75:
      return "n75";
    case n100:
      return "n100";
    default:
      invalid_enum_value(value, "sl_inter_freq_info_v2x_r14_s::sl_bw_r14_e_");
  }
  return "";
}
uint8_t sl_inter_freq_info_v2x_r14_s::sl_bw_r14_e_::to_number() const
{
  const static uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return get_enum_number(options, 6, value, "sl_inter_freq_info_v2x_r14_s::sl_bw_r14_e_");
}

std::string sl_inter_freq_info_v2x_r14_s::add_spec_emission_v2x_r14_c_::types::to_string() const
{
  switch (value) {
    case add_spec_emission_r14:
      return "additionalSpectrumEmission-r14";
    case add_spec_emission_v1440:
      return "additionalSpectrumEmission-v1440";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sl_inter_freq_info_v2x_r14_s::add_spec_emission_v2x_r14_c_::types");
  }
  return "";
}

// SL-PeriodComm-r12 ::= ENUMERATED
std::string sl_period_comm_r12_e::to_string() const
{
  switch (value) {
    case sf40:
      return "sf40";
    case sf60:
      return "sf60";
    case sf70:
      return "sf70";
    case sf80:
      return "sf80";
    case sf120:
      return "sf120";
    case sf140:
      return "sf140";
    case sf160:
      return "sf160";
    case sf240:
      return "sf240";
    case sf280:
      return "sf280";
    case sf320:
      return "sf320";
    case spare6:
      return "spare6";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare:
      return "spare";
    default:
      invalid_enum_value(value, "sl_period_comm_r12_e");
  }
  return "";
}
uint16_t sl_period_comm_r12_e::to_number() const
{
  const static uint16_t options[] = {40, 60, 70, 80, 120, 140, 160, 240, 280, 320};
  return get_enum_number(options, 10, value, "sl_period_comm_r12_e");
}

std::string sl_sync_cfg_r12_s::rx_params_ncell_r12_s_::disc_sync_win_r12_e_::to_string() const
{
  switch (value) {
    case w1:
      return "w1";
    case w2:
      return "w2";
    default:
      invalid_enum_value(value, "sl_sync_cfg_r12_s::rx_params_ncell_r12_s_::disc_sync_win_r12_e_");
  }
  return "";
}
uint8_t sl_sync_cfg_r12_s::rx_params_ncell_r12_s_::disc_sync_win_r12_e_::to_number() const
{
  const static uint8_t options[] = {1, 2};
  return get_enum_number(options, 2, value, "sl_sync_cfg_r12_s::rx_params_ncell_r12_s_::disc_sync_win_r12_e_");
}

std::string sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_win_len_br_r13_e_::to_string() const
{
  switch (value) {
    case ms20:
      return "ms20";
    case ms40:
      return "ms40";
    case ms60:
      return "ms60";
    case ms80:
      return "ms80";
    case ms120:
      return "ms120";
    case ms160:
      return "ms160";
    case ms200:
      return "ms200";
    case spare:
      return "spare";
    default:
      invalid_enum_value(value, "sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_win_len_br_r13_e_");
  }
  return "";
}
uint8_t sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_win_len_br_r13_e_::to_number() const
{
  const static uint8_t options[] = {20, 40, 60, 80, 120, 160, 200};
  return get_enum_number(options, 7, value,
                         "sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_win_len_br_r13_e_");
}

std::string sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_repeat_pattern_r13_e_::to_string() const
{
  switch (value) {
    case every_rf:
      return "everyRF";
    case every2nd_rf:
      return "every2ndRF";
    case every4th_rf:
      return "every4thRF";
    case every8th_rf:
      return "every8thRF";
    default:
      invalid_enum_value(value,
                         "sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_repeat_pattern_r13_e_");
  }
  return "";
}
uint8_t sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_repeat_pattern_r13_e_::to_number() const
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
sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::fdd_dl_or_tdd_sf_bitmap_br_r13_c_::types::to_string()
    const
{
  switch (value) {
    case sf_pattern10_r13:
      return "subframePattern10-r13";
    case sf_pattern40_r13:
      return "subframePattern40-r13";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(
          value,
          "sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::fdd_dl_or_tdd_sf_bitmap_br_r13_c_::types");
  }
  return "";
}
uint8_t
sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::fdd_dl_or_tdd_sf_bitmap_br_r13_c_::types::to_number()
    const
{
  const static uint8_t options[] = {10, 40};
  return get_enum_number(
      options, 2, value,
      "sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::fdd_dl_or_tdd_sf_bitmap_br_r13_c_::types");
}

std::string sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_hop_cfg_common_r13_e_::to_string() const
{
  switch (value) {
    case on:
      return "on";
    case off:
      return "off";
    default:
      invalid_enum_value(value,
                         "sib_type1_v1310_ies_s::bw_reduced_access_related_info_r13_s_::si_hop_cfg_common_r13_e_");
  }
  return "";
}

std::string udt_restricting_r13_s::udt_restricting_time_r13_e_::to_string() const
{
  switch (value) {
    case s4:
      return "s4";
    case s8:
      return "s8";
    case s16:
      return "s16";
    case s32:
      return "s32";
    case s64:
      return "s64";
    case s128:
      return "s128";
    case s256:
      return "s256";
    case s512:
      return "s512";
    default:
      invalid_enum_value(value, "udt_restricting_r13_s::udt_restricting_time_r13_e_");
  }
  return "";
}
uint16_t udt_restricting_r13_s::udt_restricting_time_r13_e_::to_number() const
{
  const static uint16_t options[] = {4, 8, 16, 32, 64, 128, 256, 512};
  return get_enum_number(options, 8, value, "udt_restricting_r13_s::udt_restricting_time_r13_e_");
}

// WLAN-backhaulRate-r12 ::= ENUMERATED
std::string wlan_backhaul_rate_r12_e::to_string() const
{
  switch (value) {
    case r0:
      return "r0";
    case r4:
      return "r4";
    case r8:
      return "r8";
    case r16:
      return "r16";
    case r32:
      return "r32";
    case r64:
      return "r64";
    case r128:
      return "r128";
    case r256:
      return "r256";
    case r512:
      return "r512";
    case r1024:
      return "r1024";
    case r2048:
      return "r2048";
    case r4096:
      return "r4096";
    case r8192:
      return "r8192";
    case r16384:
      return "r16384";
    case r32768:
      return "r32768";
    case r65536:
      return "r65536";
    case r131072:
      return "r131072";
    case r262144:
      return "r262144";
    case r524288:
      return "r524288";
    case r1048576:
      return "r1048576";
    case r2097152:
      return "r2097152";
    case r4194304:
      return "r4194304";
    case r8388608:
      return "r8388608";
    case r16777216:
      return "r16777216";
    case r33554432:
      return "r33554432";
    case r67108864:
      return "r67108864";
    case r134217728:
      return "r134217728";
    case r268435456:
      return "r268435456";
    case r536870912:
      return "r536870912";
    case r1073741824:
      return "r1073741824";
    case r2147483648:
      return "r2147483648";
    case r4294967296:
      return "r4294967296";
    default:
      invalid_enum_value(value, "wlan_backhaul_rate_r12_e");
  }
  return "";
}
uint64_t wlan_backhaul_rate_r12_e::to_number() const
{
  const static uint64_t options[] = {0,         4,          8,          16,        32,       64,        128,
                                     256,       512,        1024,       2048,      4096,     8192,      16384,
                                     32768,     65536,      131072,     262144,    524288,   1048576,   2097152,
                                     4194304,   8388608,    16777216,   33554432,  67108864, 134217728, 268435456,
                                     536870912, 1073741824, 2147483648, 4294967296};
  return get_enum_number(options, 32, value, "wlan_backhaul_rate_r12_e");
}

std::string bcch_cfg_s::mod_period_coeff_e_::to_string() const
{
  switch (value) {
    case n2:
      return "n2";
    case n4:
      return "n4";
    case n8:
      return "n8";
    case n16:
      return "n16";
    default:
      invalid_enum_value(value, "bcch_cfg_s::mod_period_coeff_e_");
  }
  return "";
}
uint8_t bcch_cfg_s::mod_period_coeff_e_::to_number() const
{
  const static uint8_t options[] = {2, 4, 8, 16};
  return get_enum_number(options, 4, value, "bcch_cfg_s::mod_period_coeff_e_");
}

std::string carrier_freq_nr_r15_s::subcarrier_spacing_ssb_r15_e_::to_string() const
{
  switch (value) {
    case k_hz15:
      return "kHz15";
    case k_hz30:
      return "kHz30";
    case k_hz120:
      return "kHz120";
    case k_hz240:
      return "kHz240";
    default:
      invalid_enum_value(value, "carrier_freq_nr_r15_s::subcarrier_spacing_ssb_r15_e_");
  }
  return "";
}
uint8_t carrier_freq_nr_r15_s::subcarrier_spacing_ssb_r15_e_::to_number() const
{
  const static uint8_t options[] = {15, 30, 120, 240};
  return get_enum_number(options, 4, value, "carrier_freq_nr_r15_s::subcarrier_spacing_ssb_r15_e_");
}

std::string eab_cfg_r11_s::eab_category_r11_e_::to_string() const
{
  switch (value) {
    case a:
      return "a";
    case b:
      return "b";
    case c:
      return "c";
    default:
      invalid_enum_value(value, "eab_cfg_r11_s::eab_category_r11_e_");
  }
  return "";
}

std::string freq_hop_params_r13_s::dummy_e_::to_string() const
{
  switch (value) {
    case nb2:
      return "nb2";
    case nb4:
      return "nb4";
    default:
      invalid_enum_value(value, "freq_hop_params_r13_s::dummy_e_");
  }
  return "";
}
uint8_t freq_hop_params_r13_s::dummy_e_::to_number() const
{
  const static uint8_t options[] = {2, 4};
  return get_enum_number(options, 2, value, "freq_hop_params_r13_s::dummy_e_");
}

std::string freq_hop_params_r13_s::dummy2_c_::interv_fdd_r13_e_::to_string() const
{
  switch (value) {
    case int1:
      return "int1";
    case int2:
      return "int2";
    case int4:
      return "int4";
    case int8:
      return "int8";
    default:
      invalid_enum_value(value, "freq_hop_params_r13_s::dummy2_c_::interv_fdd_r13_e_");
  }
  return "";
}
uint8_t freq_hop_params_r13_s::dummy2_c_::interv_fdd_r13_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 4, 8};
  return get_enum_number(options, 4, value, "freq_hop_params_r13_s::dummy2_c_::interv_fdd_r13_e_");
}

std::string freq_hop_params_r13_s::dummy2_c_::interv_tdd_r13_e_::to_string() const
{
  switch (value) {
    case int1:
      return "int1";
    case int5:
      return "int5";
    case int10:
      return "int10";
    case int20:
      return "int20";
    default:
      invalid_enum_value(value, "freq_hop_params_r13_s::dummy2_c_::interv_tdd_r13_e_");
  }
  return "";
}
uint8_t freq_hop_params_r13_s::dummy2_c_::interv_tdd_r13_e_::to_number() const
{
  const static uint8_t options[] = {1, 5, 10, 20};
  return get_enum_number(options, 4, value, "freq_hop_params_r13_s::dummy2_c_::interv_tdd_r13_e_");
}

std::string freq_hop_params_r13_s::dummy2_c_::types::to_string() const
{
  switch (value) {
    case interv_fdd_r13:
      return "interval-FDD-r13";
    case interv_tdd_r13:
      return "interval-TDD-r13";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "freq_hop_params_r13_s::dummy2_c_::types");
  }
  return "";
}

std::string freq_hop_params_r13_s::dummy3_c_::interv_fdd_r13_e_::to_string() const
{
  switch (value) {
    case int2:
      return "int2";
    case int4:
      return "int4";
    case int8:
      return "int8";
    case int16:
      return "int16";
    default:
      invalid_enum_value(value, "freq_hop_params_r13_s::dummy3_c_::interv_fdd_r13_e_");
  }
  return "";
}
uint8_t freq_hop_params_r13_s::dummy3_c_::interv_fdd_r13_e_::to_number() const
{
  const static uint8_t options[] = {2, 4, 8, 16};
  return get_enum_number(options, 4, value, "freq_hop_params_r13_s::dummy3_c_::interv_fdd_r13_e_");
}

std::string freq_hop_params_r13_s::dummy3_c_::interv_tdd_r13_e_::to_string() const
{
  switch (value) {
    case int5:
      return "int5";
    case int10:
      return "int10";
    case int20:
      return "int20";
    case int40:
      return "int40";
    default:
      invalid_enum_value(value, "freq_hop_params_r13_s::dummy3_c_::interv_tdd_r13_e_");
  }
  return "";
}
uint8_t freq_hop_params_r13_s::dummy3_c_::interv_tdd_r13_e_::to_number() const
{
  const static uint8_t options[] = {5, 10, 20, 40};
  return get_enum_number(options, 4, value, "freq_hop_params_r13_s::dummy3_c_::interv_tdd_r13_e_");
}

std::string freq_hop_params_r13_s::dummy3_c_::types::to_string() const
{
  switch (value) {
    case interv_fdd_r13:
      return "interval-FDD-r13";
    case interv_tdd_r13:
      return "interval-TDD-r13";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "freq_hop_params_r13_s::dummy3_c_::types");
  }
  return "";
}

std::string freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::interv_fdd_r13_e_::to_string() const
{
  switch (value) {
    case int1:
      return "int1";
    case int2:
      return "int2";
    case int4:
      return "int4";
    case int8:
      return "int8";
    default:
      invalid_enum_value(value, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::interv_fdd_r13_e_");
  }
  return "";
}
uint8_t freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::interv_fdd_r13_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 4, 8};
  return get_enum_number(options, 4, value,
                         "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::interv_fdd_r13_e_");
}

std::string freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::interv_tdd_r13_e_::to_string() const
{
  switch (value) {
    case int1:
      return "int1";
    case int5:
      return "int5";
    case int10:
      return "int10";
    case int20:
      return "int20";
    default:
      invalid_enum_value(value, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::interv_tdd_r13_e_");
  }
  return "";
}
uint8_t freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::interv_tdd_r13_e_::to_number() const
{
  const static uint8_t options[] = {1, 5, 10, 20};
  return get_enum_number(options, 4, value,
                         "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::interv_tdd_r13_e_");
}

std::string freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::types::to_string() const
{
  switch (value) {
    case interv_fdd_r13:
      return "interval-FDD-r13";
    case interv_tdd_r13:
      return "interval-TDD-r13";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::types");
  }
  return "";
}

std::string freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::interv_fdd_r13_e_::to_string() const
{
  switch (value) {
    case int2:
      return "int2";
    case int4:
      return "int4";
    case int8:
      return "int8";
    case int16:
      return "int16";
    default:
      invalid_enum_value(value, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::interv_fdd_r13_e_");
  }
  return "";
}
uint8_t freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::interv_fdd_r13_e_::to_number() const
{
  const static uint8_t options[] = {2, 4, 8, 16};
  return get_enum_number(options, 4, value,
                         "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::interv_fdd_r13_e_");
}

std::string freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::interv_tdd_r13_e_::to_string() const
{
  switch (value) {
    case int5:
      return "int5";
    case int10:
      return "int10";
    case int20:
      return "int20";
    case int40:
      return "int40";
    default:
      invalid_enum_value(value, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::interv_tdd_r13_e_");
  }
  return "";
}
uint8_t freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::interv_tdd_r13_e_::to_number() const
{
  const static uint8_t options[] = {5, 10, 20, 40};
  return get_enum_number(options, 4, value,
                         "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::interv_tdd_r13_e_");
}

std::string freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::types::to_string() const
{
  switch (value) {
    case interv_fdd_r13:
      return "interval-FDD-r13";
    case interv_tdd_r13:
      return "interval-TDD-r13";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::types");
  }
  return "";
}

std::string mbms_carrier_type_r14_s::carrier_type_r14_e_::to_string() const
{
  switch (value) {
    case mbms:
      return "mbms";
    case fembms_mixed:
      return "fembmsMixed";
    case fembms_ded:
      return "fembmsDedicated";
    default:
      invalid_enum_value(value, "mbms_carrier_type_r14_s::carrier_type_r14_e_");
  }
  return "";
}

std::string mbsfn_area_info_r9_s::non_mbsfn_region_len_e_::to_string() const
{
  switch (value) {
    case s1:
      return "s1";
    case s2:
      return "s2";
    default:
      invalid_enum_value(value, "mbsfn_area_info_r9_s::non_mbsfn_region_len_e_");
  }
  return "";
}
uint8_t mbsfn_area_info_r9_s::non_mbsfn_region_len_e_::to_number() const
{
  const static uint8_t options[] = {1, 2};
  return get_enum_number(options, 2, value, "mbsfn_area_info_r9_s::non_mbsfn_region_len_e_");
}

std::string mbsfn_area_info_r9_s::mcch_cfg_r9_s_::mcch_repeat_period_r9_e_::to_string() const
{
  switch (value) {
    case rf32:
      return "rf32";
    case rf64:
      return "rf64";
    case rf128:
      return "rf128";
    case rf256:
      return "rf256";
    default:
      invalid_enum_value(value, "mbsfn_area_info_r9_s::mcch_cfg_r9_s_::mcch_repeat_period_r9_e_");
  }
  return "";
}
uint16_t mbsfn_area_info_r9_s::mcch_cfg_r9_s_::mcch_repeat_period_r9_e_::to_number() const
{
  const static uint16_t options[] = {32, 64, 128, 256};
  return get_enum_number(options, 4, value, "mbsfn_area_info_r9_s::mcch_cfg_r9_s_::mcch_repeat_period_r9_e_");
}

std::string mbsfn_area_info_r9_s::mcch_cfg_r9_s_::mcch_mod_period_r9_e_::to_string() const
{
  switch (value) {
    case rf512:
      return "rf512";
    case rf1024:
      return "rf1024";
    default:
      invalid_enum_value(value, "mbsfn_area_info_r9_s::mcch_cfg_r9_s_::mcch_mod_period_r9_e_");
  }
  return "";
}
uint16_t mbsfn_area_info_r9_s::mcch_cfg_r9_s_::mcch_mod_period_r9_e_::to_number() const
{
  const static uint16_t options[] = {512, 1024};
  return get_enum_number(options, 2, value, "mbsfn_area_info_r9_s::mcch_cfg_r9_s_::mcch_mod_period_r9_e_");
}

std::string mbsfn_area_info_r9_s::mcch_cfg_r9_s_::sig_mcs_r9_e_::to_string() const
{
  switch (value) {
    case n2:
      return "n2";
    case n7:
      return "n7";
    case n13:
      return "n13";
    case n19:
      return "n19";
    default:
      invalid_enum_value(value, "mbsfn_area_info_r9_s::mcch_cfg_r9_s_::sig_mcs_r9_e_");
  }
  return "";
}
uint8_t mbsfn_area_info_r9_s::mcch_cfg_r9_s_::sig_mcs_r9_e_::to_number() const
{
  const static uint8_t options[] = {2, 7, 13, 19};
  return get_enum_number(options, 4, value, "mbsfn_area_info_r9_s::mcch_cfg_r9_s_::sig_mcs_r9_e_");
}

std::string mbsfn_area_info_r9_s::mcch_cfg_r14_s_::mcch_repeat_period_v1430_e_::to_string() const
{
  switch (value) {
    case rf1:
      return "rf1";
    case rf2:
      return "rf2";
    case rf4:
      return "rf4";
    case rf8:
      return "rf8";
    case rf16:
      return "rf16";
    default:
      invalid_enum_value(value, "mbsfn_area_info_r9_s::mcch_cfg_r14_s_::mcch_repeat_period_v1430_e_");
  }
  return "";
}
uint8_t mbsfn_area_info_r9_s::mcch_cfg_r14_s_::mcch_repeat_period_v1430_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 4, 8, 16};
  return get_enum_number(options, 5, value, "mbsfn_area_info_r9_s::mcch_cfg_r14_s_::mcch_repeat_period_v1430_e_");
}

std::string mbsfn_area_info_r9_s::mcch_cfg_r14_s_::mcch_mod_period_v1430_e_::to_string() const
{
  switch (value) {
    case rf1:
      return "rf1";
    case rf2:
      return "rf2";
    case rf4:
      return "rf4";
    case rf8:
      return "rf8";
    case rf16:
      return "rf16";
    case rf32:
      return "rf32";
    case rf64:
      return "rf64";
    case rf128:
      return "rf128";
    case rf256:
      return "rf256";
    case spare7:
      return "spare7";
    default:
      invalid_enum_value(value, "mbsfn_area_info_r9_s::mcch_cfg_r14_s_::mcch_mod_period_v1430_e_");
  }
  return "";
}
uint16_t mbsfn_area_info_r9_s::mcch_cfg_r14_s_::mcch_mod_period_v1430_e_::to_number() const
{
  const static uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
  return get_enum_number(options, 9, value, "mbsfn_area_info_r9_s::mcch_cfg_r14_s_::mcch_mod_period_v1430_e_");
}

std::string mbsfn_area_info_r9_s::subcarrier_spacing_mbms_r14_e_::to_string() const
{
  switch (value) {
    case khz_minus7dot5:
      return "khz-7dot5";
    case khz_minus1dot25:
      return "khz-1dot25";
    default:
      invalid_enum_value(value, "mbsfn_area_info_r9_s::subcarrier_spacing_mbms_r14_e_");
  }
  return "";
}
float mbsfn_area_info_r9_s::subcarrier_spacing_mbms_r14_e_::to_number() const
{
  const static float options[] = {-7.5, -1.25};
  return get_enum_number(options, 2, value, "mbsfn_area_info_r9_s::subcarrier_spacing_mbms_r14_e_");
}
std::string mbsfn_area_info_r9_s::subcarrier_spacing_mbms_r14_e_::to_number_string() const
{
  switch (value) {
    case khz_minus7dot5:
      return "-7.5";
    case khz_minus1dot25:
      return "-1.25";
    default:
      invalid_enum_number(value, "mbsfn_area_info_r9_s::subcarrier_spacing_mbms_r14_e_");
  }
  return "";
}

std::string mbsfn_sf_cfg_s::radioframe_alloc_period_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n2:
      return "n2";
    case n4:
      return "n4";
    case n8:
      return "n8";
    case n16:
      return "n16";
    case n32:
      return "n32";
    default:
      invalid_enum_value(value, "mbsfn_sf_cfg_s::radioframe_alloc_period_e_");
  }
  return "";
}
uint8_t mbsfn_sf_cfg_s::radioframe_alloc_period_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 4, 8, 16, 32};
  return get_enum_number(options, 6, value, "mbsfn_sf_cfg_s::radioframe_alloc_period_e_");
}

std::string mbsfn_sf_cfg_s::sf_alloc_c_::types::to_string() const
{
  switch (value) {
    case one_frame:
      return "oneFrame";
    case four_frames:
      return "fourFrames";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "mbsfn_sf_cfg_s::sf_alloc_c_::types");
  }
  return "";
}
uint8_t mbsfn_sf_cfg_s::sf_alloc_c_::types::to_number() const
{
  const static uint8_t options[] = {1, 4};
  return get_enum_number(options, 2, value, "mbsfn_sf_cfg_s::sf_alloc_c_::types");
}

std::string mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_::types::to_string() const
{
  switch (value) {
    case one_frame_v1430:
      return "oneFrame-v1430";
    case four_frames_v1430:
      return "fourFrames-v1430";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_::types");
  }
  return "";
}
uint8_t mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_::types::to_number() const
{
  const static uint8_t options[] = {1, 4};
  return get_enum_number(options, 2, value, "mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_::types");
}

std::string pcch_cfg_s::default_paging_cycle_e_::to_string() const
{
  switch (value) {
    case rf32:
      return "rf32";
    case rf64:
      return "rf64";
    case rf128:
      return "rf128";
    case rf256:
      return "rf256";
    default:
      invalid_enum_value(value, "pcch_cfg_s::default_paging_cycle_e_");
  }
  return "";
}
uint16_t pcch_cfg_s::default_paging_cycle_e_::to_number() const
{
  const static uint16_t options[] = {32, 64, 128, 256};
  return get_enum_number(options, 4, value, "pcch_cfg_s::default_paging_cycle_e_");
}

std::string pcch_cfg_s::nb_e_::to_string() const
{
  switch (value) {
    case four_t:
      return "fourT";
    case two_t:
      return "twoT";
    case one_t:
      return "oneT";
    case half_t:
      return "halfT";
    case quarter_t:
      return "quarterT";
    case one_eighth_t:
      return "oneEighthT";
    case one_sixteenth_t:
      return "oneSixteenthT";
    case one_thirty_second_t:
      return "oneThirtySecondT";
    default:
      invalid_enum_value(value, "pcch_cfg_s::nb_e_");
  }
  return "";
}
float pcch_cfg_s::nb_e_::to_number() const
{
  const static float options[] = {4.0, 2.0, 1.0, 0.5, 0.25, 0.125, 0.0625, 0.03125};
  return get_enum_number(options, 8, value, "pcch_cfg_s::nb_e_");
}
std::string pcch_cfg_s::nb_e_::to_number_string() const
{
  switch (value) {
    case four_t:
      return "4";
    case two_t:
      return "2";
    case one_t:
      return "1";
    case half_t:
      return "0.5";
    case quarter_t:
      return "0.25";
    case one_eighth_t:
      return "1/8";
    case one_sixteenth_t:
      return "1/16";
    case one_thirty_second_t:
      return "1/32";
    default:
      invalid_enum_number(value, "pcch_cfg_s::nb_e_");
  }
  return "";
}

std::string pcch_cfg_v1310_s::mpdcch_num_repeat_paging_r13_e_::to_string() const
{
  switch (value) {
    case r1:
      return "r1";
    case r2:
      return "r2";
    case r4:
      return "r4";
    case r8:
      return "r8";
    case r16:
      return "r16";
    case r32:
      return "r32";
    case r64:
      return "r64";
    case r128:
      return "r128";
    case r256:
      return "r256";
    default:
      invalid_enum_value(value, "pcch_cfg_v1310_s::mpdcch_num_repeat_paging_r13_e_");
  }
  return "";
}
uint16_t pcch_cfg_v1310_s::mpdcch_num_repeat_paging_r13_e_::to_number() const
{
  const static uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
  return get_enum_number(options, 9, value, "pcch_cfg_v1310_s::mpdcch_num_repeat_paging_r13_e_");
}

std::string pcch_cfg_v1310_s::nb_v1310_e_::to_string() const
{
  switch (value) {
    case one64th_t:
      return "one64thT";
    case one128th_t:
      return "one128thT";
    case one256th_t:
      return "one256thT";
    default:
      invalid_enum_value(value, "pcch_cfg_v1310_s::nb_v1310_e_");
  }
  return "";
}
uint16_t pcch_cfg_v1310_s::nb_v1310_e_::to_number() const
{
  const static uint16_t options[] = {64, 128, 256};
  return get_enum_number(options, 3, value, "pcch_cfg_v1310_s::nb_v1310_e_");
}

std::string pdsch_cfg_common_v1310_s::pdsch_max_num_repeat_cemode_a_r13_e_::to_string() const
{
  switch (value) {
    case r16:
      return "r16";
    case r32:
      return "r32";
    default:
      invalid_enum_value(value, "pdsch_cfg_common_v1310_s::pdsch_max_num_repeat_cemode_a_r13_e_");
  }
  return "";
}
uint8_t pdsch_cfg_common_v1310_s::pdsch_max_num_repeat_cemode_a_r13_e_::to_number() const
{
  const static uint8_t options[] = {16, 32};
  return get_enum_number(options, 2, value, "pdsch_cfg_common_v1310_s::pdsch_max_num_repeat_cemode_a_r13_e_");
}

std::string pdsch_cfg_common_v1310_s::pdsch_max_num_repeat_cemode_b_r13_e_::to_string() const
{
  switch (value) {
    case r192:
      return "r192";
    case r256:
      return "r256";
    case r384:
      return "r384";
    case r512:
      return "r512";
    case r768:
      return "r768";
    case r1024:
      return "r1024";
    case r1536:
      return "r1536";
    case r2048:
      return "r2048";
    default:
      invalid_enum_value(value, "pdsch_cfg_common_v1310_s::pdsch_max_num_repeat_cemode_b_r13_e_");
  }
  return "";
}
uint16_t pdsch_cfg_common_v1310_s::pdsch_max_num_repeat_cemode_b_r13_e_::to_number() const
{
  const static uint16_t options[] = {192, 256, 384, 512, 768, 1024, 1536, 2048};
  return get_enum_number(options, 8, value, "pdsch_cfg_common_v1310_s::pdsch_max_num_repeat_cemode_b_r13_e_");
}

std::string prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_e_::to_string() const
{
  switch (value) {
    case v1:
      return "v1";
    case v1dot5:
      return "v1dot5";
    case v2:
      return "v2";
    case v2dot5:
      return "v2dot5";
    case v4:
      return "v4";
    case v5:
      return "v5";
    case v8:
      return "v8";
    case v10:
      return "v10";
    default:
      invalid_enum_value(value, "prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_e_");
  }
  return "";
}
float prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_e_::to_number() const
{
  const static float options[] = {1.0, 1.5, 2.0, 2.5, 4.0, 5.0, 8.0, 10.0};
  return get_enum_number(options, 8, value, "prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_e_");
}
std::string prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_e_::to_number_string() const
{
  switch (value) {
    case v1:
      return "1";
    case v1dot5:
      return "1.5";
    case v2:
      return "2";
    case v2dot5:
      return "2.5";
    case v4:
      return "4";
    case v5:
      return "5";
    case v8:
      return "8";
    case v10:
      return "10";
    default:
      invalid_enum_number(value, "prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_e_");
  }
  return "";
}

std::string prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::tdd_r13_e_::to_string() const
{
  switch (value) {
    case v1:
      return "v1";
    case v2:
      return "v2";
    case v4:
      return "v4";
    case v5:
      return "v5";
    case v8:
      return "v8";
    case v10:
      return "v10";
    case v20:
      return "v20";
    case spare:
      return "spare";
    default:
      invalid_enum_value(value, "prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::tdd_r13_e_");
  }
  return "";
}
uint8_t prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::tdd_r13_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 4, 5, 8, 10, 20};
  return get_enum_number(options, 7, value, "prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::tdd_r13_e_");
}

std::string prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::types::to_string() const
{
  switch (value) {
    case fdd_r13:
      return "fdd-r13";
    case tdd_r13:
      return "tdd-r13";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::types");
  }
  return "";
}

std::string pucch_cfg_common_s::delta_pucch_shift_e_::to_string() const
{
  switch (value) {
    case ds1:
      return "ds1";
    case ds2:
      return "ds2";
    case ds3:
      return "ds3";
    default:
      invalid_enum_value(value, "pucch_cfg_common_s::delta_pucch_shift_e_");
  }
  return "";
}
uint8_t pucch_cfg_common_s::delta_pucch_shift_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3};
  return get_enum_number(options, 3, value, "pucch_cfg_common_s::delta_pucch_shift_e_");
}

std::string pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level0_r13_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n2:
      return "n2";
    case n4:
      return "n4";
    case n8:
      return "n8";
    default:
      invalid_enum_value(value, "pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level0_r13_e_");
  }
  return "";
}
uint8_t pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level0_r13_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 4, 8};
  return get_enum_number(options, 4, value, "pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level0_r13_e_");
}

std::string pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level1_r13_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n2:
      return "n2";
    case n4:
      return "n4";
    case n8:
      return "n8";
    default:
      invalid_enum_value(value, "pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level1_r13_e_");
  }
  return "";
}
uint8_t pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level1_r13_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 4, 8};
  return get_enum_number(options, 4, value, "pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level1_r13_e_");
}

std::string pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level2_r13_e_::to_string() const
{
  switch (value) {
    case n4:
      return "n4";
    case n8:
      return "n8";
    case n16:
      return "n16";
    case n32:
      return "n32";
    default:
      invalid_enum_value(value, "pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level2_r13_e_");
  }
  return "";
}
uint8_t pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level2_r13_e_::to_number() const
{
  const static uint8_t options[] = {4, 8, 16, 32};
  return get_enum_number(options, 4, value, "pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level2_r13_e_");
}

std::string pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level3_r13_e_::to_string() const
{
  switch (value) {
    case n4:
      return "n4";
    case n8:
      return "n8";
    case n16:
      return "n16";
    case n32:
      return "n32";
    default:
      invalid_enum_value(value, "pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level3_r13_e_");
  }
  return "";
}
uint8_t pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level3_r13_e_::to_number() const
{
  const static uint8_t options[] = {4, 8, 16, 32};
  return get_enum_number(options, 4, value, "pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level3_r13_e_");
}

std::string pucch_cfg_common_v1430_s::pucch_num_repeat_ce_msg4_level3_r14_e_::to_string() const
{
  switch (value) {
    case n64:
      return "n64";
    case n128:
      return "n128";
    default:
      invalid_enum_value(value, "pucch_cfg_common_v1430_s::pucch_num_repeat_ce_msg4_level3_r14_e_");
  }
  return "";
}
uint8_t pucch_cfg_common_v1430_s::pucch_num_repeat_ce_msg4_level3_r14_e_::to_number() const
{
  const static uint8_t options[] = {64, 128};
  return get_enum_number(options, 2, value, "pucch_cfg_common_v1430_s::pucch_num_repeat_ce_msg4_level3_r14_e_");
}

std::string pusch_cfg_common_s::pusch_cfg_basic_s_::hop_mode_e_::to_string() const
{
  switch (value) {
    case inter_sub_frame:
      return "interSubFrame";
    case intra_and_inter_sub_frame:
      return "intraAndInterSubFrame";
    default:
      invalid_enum_value(value, "pusch_cfg_common_s::pusch_cfg_basic_s_::hop_mode_e_");
  }
  return "";
}

std::string pusch_cfg_common_v1310_s::pusch_max_num_repeat_cemode_a_r13_e_::to_string() const
{
  switch (value) {
    case r8:
      return "r8";
    case r16:
      return "r16";
    case r32:
      return "r32";
    default:
      invalid_enum_value(value, "pusch_cfg_common_v1310_s::pusch_max_num_repeat_cemode_a_r13_e_");
  }
  return "";
}
uint8_t pusch_cfg_common_v1310_s::pusch_max_num_repeat_cemode_a_r13_e_::to_number() const
{
  const static uint8_t options[] = {8, 16, 32};
  return get_enum_number(options, 3, value, "pusch_cfg_common_v1310_s::pusch_max_num_repeat_cemode_a_r13_e_");
}

std::string pusch_cfg_common_v1310_s::pusch_max_num_repeat_cemode_b_r13_e_::to_string() const
{
  switch (value) {
    case r192:
      return "r192";
    case r256:
      return "r256";
    case r384:
      return "r384";
    case r512:
      return "r512";
    case r768:
      return "r768";
    case r1024:
      return "r1024";
    case r1536:
      return "r1536";
    case r2048:
      return "r2048";
    default:
      invalid_enum_value(value, "pusch_cfg_common_v1310_s::pusch_max_num_repeat_cemode_b_r13_e_");
  }
  return "";
}
uint16_t pusch_cfg_common_v1310_s::pusch_max_num_repeat_cemode_b_r13_e_::to_number() const
{
  const static uint16_t options[] = {192, 256, 384, 512, 768, 1024, 1536, 2048};
  return get_enum_number(options, 8, value, "pusch_cfg_common_v1310_s::pusch_max_num_repeat_cemode_b_r13_e_");
}

std::string rach_cfg_common_s::preamb_info_s_::nof_ra_preambs_e_::to_string() const
{
  switch (value) {
    case n4:
      return "n4";
    case n8:
      return "n8";
    case n12:
      return "n12";
    case n16:
      return "n16";
    case n20:
      return "n20";
    case n24:
      return "n24";
    case n28:
      return "n28";
    case n32:
      return "n32";
    case n36:
      return "n36";
    case n40:
      return "n40";
    case n44:
      return "n44";
    case n48:
      return "n48";
    case n52:
      return "n52";
    case n56:
      return "n56";
    case n60:
      return "n60";
    case n64:
      return "n64";
    default:
      invalid_enum_value(value, "rach_cfg_common_s::preamb_info_s_::nof_ra_preambs_e_");
  }
  return "";
}
uint8_t rach_cfg_common_s::preamb_info_s_::nof_ra_preambs_e_::to_number() const
{
  const static uint8_t options[] = {4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64};
  return get_enum_number(options, 16, value, "rach_cfg_common_s::preamb_info_s_::nof_ra_preambs_e_");
}

std::string rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::size_of_ra_preambs_group_a_e_::to_string() const
{
  switch (value) {
    case n4:
      return "n4";
    case n8:
      return "n8";
    case n12:
      return "n12";
    case n16:
      return "n16";
    case n20:
      return "n20";
    case n24:
      return "n24";
    case n28:
      return "n28";
    case n32:
      return "n32";
    case n36:
      return "n36";
    case n40:
      return "n40";
    case n44:
      return "n44";
    case n48:
      return "n48";
    case n52:
      return "n52";
    case n56:
      return "n56";
    case n60:
      return "n60";
    default:
      invalid_enum_value(value,
                         "rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::size_of_ra_preambs_group_a_e_");
  }
  return "";
}
uint8_t rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::size_of_ra_preambs_group_a_e_::to_number() const
{
  const static uint8_t options[] = {4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60};
  return get_enum_number(options, 15, value,
                         "rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::size_of_ra_preambs_group_a_e_");
}

std::string rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::msg_size_group_a_e_::to_string() const
{
  switch (value) {
    case b56:
      return "b56";
    case b144:
      return "b144";
    case b208:
      return "b208";
    case b256:
      return "b256";
    default:
      invalid_enum_value(value, "rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::msg_size_group_a_e_");
  }
  return "";
}
uint16_t rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::msg_size_group_a_e_::to_number() const
{
  const static uint16_t options[] = {56, 144, 208, 256};
  return get_enum_number(options, 4, value,
                         "rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::msg_size_group_a_e_");
}

std::string rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::msg_pwr_offset_group_b_e_::to_string() const
{
  switch (value) {
    case minusinfinity:
      return "minusinfinity";
    case db0:
      return "dB0";
    case db5:
      return "dB5";
    case db8:
      return "dB8";
    case db10:
      return "dB10";
    case db12:
      return "dB12";
    case db15:
      return "dB15";
    case db18:
      return "dB18";
    default:
      invalid_enum_value(value, "rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::msg_pwr_offset_group_b_e_");
  }
  return "";
}
int8_t rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::msg_pwr_offset_group_b_e_::to_number() const
{
  const static int8_t options[] = {-1, 0, 5, 8, 10, 12, 15, 18};
  return get_enum_number(options, 8, value,
                         "rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::msg_pwr_offset_group_b_e_");
}

std::string rach_cfg_common_s::ra_supervision_info_s_::ra_resp_win_size_e_::to_string() const
{
  switch (value) {
    case sf2:
      return "sf2";
    case sf3:
      return "sf3";
    case sf4:
      return "sf4";
    case sf5:
      return "sf5";
    case sf6:
      return "sf6";
    case sf7:
      return "sf7";
    case sf8:
      return "sf8";
    case sf10:
      return "sf10";
    default:
      invalid_enum_value(value, "rach_cfg_common_s::ra_supervision_info_s_::ra_resp_win_size_e_");
  }
  return "";
}
uint8_t rach_cfg_common_s::ra_supervision_info_s_::ra_resp_win_size_e_::to_number() const
{
  const static uint8_t options[] = {2, 3, 4, 5, 6, 7, 8, 10};
  return get_enum_number(options, 8, value, "rach_cfg_common_s::ra_supervision_info_s_::ra_resp_win_size_e_");
}

std::string rach_cfg_common_s::ra_supervision_info_s_::mac_contention_resolution_timer_e_::to_string() const
{
  switch (value) {
    case sf8:
      return "sf8";
    case sf16:
      return "sf16";
    case sf24:
      return "sf24";
    case sf32:
      return "sf32";
    case sf40:
      return "sf40";
    case sf48:
      return "sf48";
    case sf56:
      return "sf56";
    case sf64:
      return "sf64";
    default:
      invalid_enum_value(value, "rach_cfg_common_s::ra_supervision_info_s_::mac_contention_resolution_timer_e_");
  }
  return "";
}
uint8_t rach_cfg_common_s::ra_supervision_info_s_::mac_contention_resolution_timer_e_::to_number() const
{
  const static uint8_t options[] = {8, 16, 24, 32, 40, 48, 56, 64};
  return get_enum_number(options, 8, value,
                         "rach_cfg_common_s::ra_supervision_info_s_::mac_contention_resolution_timer_e_");
}

std::string rach_cfg_common_v1250_s::tx_fail_params_r12_s_::conn_est_fail_count_r12_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n2:
      return "n2";
    case n3:
      return "n3";
    case n4:
      return "n4";
    default:
      invalid_enum_value(value, "rach_cfg_common_v1250_s::tx_fail_params_r12_s_::conn_est_fail_count_r12_e_");
  }
  return "";
}
uint8_t rach_cfg_common_v1250_s::tx_fail_params_r12_s_::conn_est_fail_count_r12_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 4};
  return get_enum_number(options, 4, value,
                         "rach_cfg_common_v1250_s::tx_fail_params_r12_s_::conn_est_fail_count_r12_e_");
}

std::string rach_cfg_common_v1250_s::tx_fail_params_r12_s_::conn_est_fail_offset_validity_r12_e_::to_string() const
{
  switch (value) {
    case s30:
      return "s30";
    case s60:
      return "s60";
    case s120:
      return "s120";
    case s240:
      return "s240";
    case s300:
      return "s300";
    case s420:
      return "s420";
    case s600:
      return "s600";
    case s900:
      return "s900";
    default:
      invalid_enum_value(value, "rach_cfg_common_v1250_s::tx_fail_params_r12_s_::conn_est_fail_offset_validity_r12_e_");
  }
  return "";
}
uint16_t rach_cfg_common_v1250_s::tx_fail_params_r12_s_::conn_est_fail_offset_validity_r12_e_::to_number() const
{
  const static uint16_t options[] = {30, 60, 120, 240, 300, 420, 600, 900};
  return get_enum_number(options, 8, value,
                         "rach_cfg_common_v1250_s::tx_fail_params_r12_s_::conn_est_fail_offset_validity_r12_e_");
}

std::string rss_cfg_r15_s::dur_r15_e_::to_string() const
{
  switch (value) {
    case sf8:
      return "sf8";
    case sf16:
      return "sf16";
    case sf32:
      return "sf32";
    case sf40:
      return "sf40";
    default:
      invalid_enum_value(value, "rss_cfg_r15_s::dur_r15_e_");
  }
  return "";
}
uint8_t rss_cfg_r15_s::dur_r15_e_::to_number() const
{
  const static uint8_t options[] = {8, 16, 32, 40};
  return get_enum_number(options, 4, value, "rss_cfg_r15_s::dur_r15_e_");
}

std::string rss_cfg_r15_s::periodicity_r15_e_::to_string() const
{
  switch (value) {
    case ms160:
      return "ms160";
    case ms320:
      return "ms320";
    case ms640:
      return "ms640";
    case ms1280:
      return "ms1280";
    default:
      invalid_enum_value(value, "rss_cfg_r15_s::periodicity_r15_e_");
  }
  return "";
}
uint16_t rss_cfg_r15_s::periodicity_r15_e_::to_number() const
{
  const static uint16_t options[] = {160, 320, 640, 1280};
  return get_enum_number(options, 4, value, "rss_cfg_r15_s::periodicity_r15_e_");
}

std::string rss_cfg_r15_s::pwr_boost_r15_e_::to_string() const
{
  switch (value) {
    case db0:
      return "dB0";
    case db3:
      return "dB3";
    case db4dot8:
      return "dB4dot8";
    case db6:
      return "dB6";
    default:
      invalid_enum_value(value, "rss_cfg_r15_s::pwr_boost_r15_e_");
  }
  return "";
}
float rss_cfg_r15_s::pwr_boost_r15_e_::to_number() const
{
  const static float options[] = {0.0, 3.0, 4.8, 6.0};
  return get_enum_number(options, 4, value, "rss_cfg_r15_s::pwr_boost_r15_e_");
}
std::string rss_cfg_r15_s::pwr_boost_r15_e_::to_number_string() const
{
  switch (value) {
    case db0:
      return "0";
    case db3:
      return "3";
    case db4dot8:
      return "4.8";
    case db6:
      return "6";
    default:
      invalid_enum_number(value, "rss_cfg_r15_s::pwr_boost_r15_e_");
  }
  return "";
}

std::string resel_info_relay_r13_s::min_hyst_r13_e_::to_string() const
{
  switch (value) {
    case db0:
      return "dB0";
    case db3:
      return "dB3";
    case db6:
      return "dB6";
    case db9:
      return "dB9";
    case db12:
      return "dB12";
    case dbinf:
      return "dBinf";
    default:
      invalid_enum_value(value, "resel_info_relay_r13_s::min_hyst_r13_e_");
  }
  return "";
}
uint8_t resel_info_relay_r13_s::min_hyst_r13_e_::to_number() const
{
  const static uint8_t options[] = {0, 3, 6, 9, 12};
  return get_enum_number(options, 5, value, "resel_info_relay_r13_s::min_hyst_r13_e_");
}

// SIB-Type ::= ENUMERATED
std::string sib_type_e::to_string() const
{
  switch (value) {
    case sib_type3:
      return "sibType3";
    case sib_type4:
      return "sibType4";
    case sib_type5:
      return "sibType5";
    case sib_type6:
      return "sibType6";
    case sib_type7:
      return "sibType7";
    case sib_type8:
      return "sibType8";
    case sib_type9:
      return "sibType9";
    case sib_type10:
      return "sibType10";
    case sib_type11:
      return "sibType11";
    case sib_type12_v920:
      return "sibType12-v920";
    case sib_type13_v920:
      return "sibType13-v920";
    case sib_type14_v1130:
      return "sibType14-v1130";
    case sib_type15_v1130:
      return "sibType15-v1130";
    case sib_type16_v1130:
      return "sibType16-v1130";
    case sib_type17_v1250:
      return "sibType17-v1250";
    case sib_type18_v1250:
      return "sibType18-v1250";
    case sib_type19_v1250:
      return "sibType19-v1250";
    case sib_type20_v1310:
      return "sibType20-v1310";
    case sib_type21_v1430:
      return "sibType21-v1430";
    case sib_type24_v1530:
      return "sibType24-v1530";
    case sib_type25_v1530:
      return "sibType25-v1530";
    case sib_type26_v1530:
      return "sibType26-v1530";
    default:
      invalid_enum_value(value, "sib_type_e");
  }
  return "";
}
uint8_t sib_type_e::to_number() const
{
  const static uint8_t options[] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 24, 25, 26};
  return get_enum_number(options, 22, value, "sib_type_e");
}

std::string sib8_per_plmn_r11_s::params_cdma2000_r11_c_::types::to_string() const
{
  switch (value) {
    case explicit_value:
      return "explicitValue";
    case default_value:
      return "defaultValue";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sib8_per_plmn_r11_s::params_cdma2000_r11_c_::types");
  }
  return "";
}

std::string srs_ul_cfg_common_c::setup_s_::srs_bw_cfg_e_::to_string() const
{
  switch (value) {
    case bw0:
      return "bw0";
    case bw1:
      return "bw1";
    case bw2:
      return "bw2";
    case bw3:
      return "bw3";
    case bw4:
      return "bw4";
    case bw5:
      return "bw5";
    case bw6:
      return "bw6";
    case bw7:
      return "bw7";
    default:
      invalid_enum_value(value, "srs_ul_cfg_common_c::setup_s_::srs_bw_cfg_e_");
  }
  return "";
}
uint8_t srs_ul_cfg_common_c::setup_s_::srs_bw_cfg_e_::to_number() const
{
  const static uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7};
  return get_enum_number(options, 8, value, "srs_ul_cfg_common_c::setup_s_::srs_bw_cfg_e_");
}

std::string srs_ul_cfg_common_c::setup_s_::srs_sf_cfg_e_::to_string() const
{
  switch (value) {
    case sc0:
      return "sc0";
    case sc1:
      return "sc1";
    case sc2:
      return "sc2";
    case sc3:
      return "sc3";
    case sc4:
      return "sc4";
    case sc5:
      return "sc5";
    case sc6:
      return "sc6";
    case sc7:
      return "sc7";
    case sc8:
      return "sc8";
    case sc9:
      return "sc9";
    case sc10:
      return "sc10";
    case sc11:
      return "sc11";
    case sc12:
      return "sc12";
    case sc13:
      return "sc13";
    case sc14:
      return "sc14";
    case sc15:
      return "sc15";
    default:
      invalid_enum_value(value, "srs_ul_cfg_common_c::setup_s_::srs_sf_cfg_e_");
  }
  return "";
}
uint8_t srs_ul_cfg_common_c::setup_s_::srs_sf_cfg_e_::to_number() const
{
  const static uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  return get_enum_number(options, 16, value, "srs_ul_cfg_common_c::setup_s_::srs_sf_cfg_e_");
}

std::string uac_barr_info_set_r15_s::uac_barr_factor_r15_e_::to_string() const
{
  switch (value) {
    case p00:
      return "p00";
    case p05:
      return "p05";
    case p10:
      return "p10";
    case p15:
      return "p15";
    case p20:
      return "p20";
    case p25:
      return "p25";
    case p30:
      return "p30";
    case p40:
      return "p40";
    case p50:
      return "p50";
    case p60:
      return "p60";
    case p70:
      return "p70";
    case p75:
      return "p75";
    case p80:
      return "p80";
    case p85:
      return "p85";
    case p90:
      return "p90";
    case p95:
      return "p95";
    default:
      invalid_enum_value(value, "uac_barr_info_set_r15_s::uac_barr_factor_r15_e_");
  }
  return "";
}
float uac_barr_info_set_r15_s::uac_barr_factor_r15_e_::to_number() const
{
  const static float options[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 6.0, 7.0, 7.5, 8.0, 8.5, 9.0, 9.5};
  return get_enum_number(options, 16, value, "uac_barr_info_set_r15_s::uac_barr_factor_r15_e_");
}
std::string uac_barr_info_set_r15_s::uac_barr_factor_r15_e_::to_number_string() const
{
  switch (value) {
    case p00:
      return "0.0";
    case p05:
      return "0.5";
    case p10:
      return "1.0";
    case p15:
      return "1.5";
    case p20:
      return "2.0";
    case p25:
      return "2.5";
    case p30:
      return "3.0";
    case p40:
      return "4.0";
    case p50:
      return "5.0";
    case p60:
      return "6.0";
    case p70:
      return "7.0";
    case p75:
      return "7.5";
    case p80:
      return "8.0";
    case p85:
      return "8.5";
    case p90:
      return "9.0";
    case p95:
      return "9.5";
    default:
      invalid_enum_number(value, "uac_barr_info_set_r15_s::uac_barr_factor_r15_e_");
  }
  return "";
}

std::string uac_barr_info_set_r15_s::uac_barr_time_r15_e_::to_string() const
{
  switch (value) {
    case s4:
      return "s4";
    case s8:
      return "s8";
    case s16:
      return "s16";
    case s32:
      return "s32";
    case s64:
      return "s64";
    case s128:
      return "s128";
    case s256:
      return "s256";
    case s512:
      return "s512";
    default:
      invalid_enum_value(value, "uac_barr_info_set_r15_s::uac_barr_time_r15_e_");
  }
  return "";
}
uint16_t uac_barr_info_set_r15_s::uac_barr_time_r15_e_::to_number() const
{
  const static uint16_t options[] = {4, 8, 16, 32, 64, 128, 256, 512};
  return get_enum_number(options, 8, value, "uac_barr_info_set_r15_s::uac_barr_time_r15_e_");
}

std::string uac_barr_per_plmn_r15_s::uac_ac_barr_list_type_r15_c_::types::to_string() const
{
  switch (value) {
    case uac_implicit_ac_barr_list_r15:
      return "uac-ImplicitAC-BarringList-r15";
    case uac_explicit_ac_barr_list_r15:
      return "uac-ExplicitAC-BarringList-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "uac_barr_per_plmn_r15_s::uac_ac_barr_list_type_r15_c_::types");
  }
  return "";
}

// UL-CyclicPrefixLength ::= ENUMERATED
std::string ul_cp_len_e::to_string() const
{
  switch (value) {
    case len1:
      return "len1";
    case len2:
      return "len2";
    default:
      invalid_enum_value(value, "ul_cp_len_e");
  }
  return "";
}
uint8_t ul_cp_len_e::to_number() const
{
  const static uint8_t options[] = {1, 2};
  return get_enum_number(options, 2, value, "ul_cp_len_e");
}

std::string ul_pwr_ctrl_common_v1020_s::delta_f_pucch_format3_r10_e_::to_string() const
{
  switch (value) {
    case delta_f_minus1:
      return "deltaF-1";
    case delta_f0:
      return "deltaF0";
    case delta_f1:
      return "deltaF1";
    case delta_f2:
      return "deltaF2";
    case delta_f3:
      return "deltaF3";
    case delta_f4:
      return "deltaF4";
    case delta_f5:
      return "deltaF5";
    case delta_f6:
      return "deltaF6";
    default:
      invalid_enum_value(value, "ul_pwr_ctrl_common_v1020_s::delta_f_pucch_format3_r10_e_");
  }
  return "";
}
int8_t ul_pwr_ctrl_common_v1020_s::delta_f_pucch_format3_r10_e_::to_number() const
{
  const static int8_t options[] = {-1, 0, 1, 2, 3, 4, 5, 6};
  return get_enum_number(options, 8, value, "ul_pwr_ctrl_common_v1020_s::delta_f_pucch_format3_r10_e_");
}

std::string ul_pwr_ctrl_common_v1020_s::delta_f_pucch_format1b_cs_r10_e_::to_string() const
{
  switch (value) {
    case delta_f1:
      return "deltaF1";
    case delta_f2:
      return "deltaF2";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "ul_pwr_ctrl_common_v1020_s::delta_f_pucch_format1b_cs_r10_e_");
  }
  return "";
}
uint8_t ul_pwr_ctrl_common_v1020_s::delta_f_pucch_format1b_cs_r10_e_::to_number() const
{
  const static uint8_t options[] = {1, 2};
  return get_enum_number(options, 2, value, "ul_pwr_ctrl_common_v1020_s::delta_f_pucch_format1b_cs_r10_e_");
}

std::string wus_cfg_r15_s::max_dur_factor_r15_e_::to_string() const
{
  switch (value) {
    case one32th:
      return "one32th";
    case one16th:
      return "one16th";
    case one8th:
      return "one8th";
    case one4th:
      return "one4th";
    default:
      invalid_enum_value(value, "wus_cfg_r15_s::max_dur_factor_r15_e_");
  }
  return "";
}
uint8_t wus_cfg_r15_s::max_dur_factor_r15_e_::to_number() const
{
  const static uint8_t options[] = {32, 16, 8, 4};
  return get_enum_number(options, 4, value, "wus_cfg_r15_s::max_dur_factor_r15_e_");
}

std::string wus_cfg_r15_s::num_pos_r15_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n2:
      return "n2";
    case n4:
      return "n4";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "wus_cfg_r15_s::num_pos_r15_e_");
  }
  return "";
}
uint8_t wus_cfg_r15_s::num_pos_r15_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 4};
  return get_enum_number(options, 3, value, "wus_cfg_r15_s::num_pos_r15_e_");
}

std::string wus_cfg_r15_s::freq_location_r15_e_::to_string() const
{
  switch (value) {
    case n0:
      return "n0";
    case n2:
      return "n2";
    case n4:
      return "n4";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "wus_cfg_r15_s::freq_location_r15_e_");
  }
  return "";
}
uint8_t wus_cfg_r15_s::freq_location_r15_e_::to_number() const
{
  const static uint8_t options[] = {0, 2, 4};
  return get_enum_number(options, 3, value, "wus_cfg_r15_s::freq_location_r15_e_");
}

std::string wus_cfg_r15_s::time_offset_drx_r15_e_::to_string() const
{
  switch (value) {
    case ms40:
      return "ms40";
    case ms80:
      return "ms80";
    case ms160:
      return "ms160";
    case ms240:
      return "ms240";
    default:
      invalid_enum_value(value, "wus_cfg_r15_s::time_offset_drx_r15_e_");
  }
  return "";
}
uint8_t wus_cfg_r15_s::time_offset_drx_r15_e_::to_number() const
{
  const static uint8_t options[] = {40, 80, 160, 240};
  return get_enum_number(options, 4, value, "wus_cfg_r15_s::time_offset_drx_r15_e_");
}

std::string wus_cfg_r15_s::time_offset_e_drx_short_r15_e_::to_string() const
{
  switch (value) {
    case ms40:
      return "ms40";
    case ms80:
      return "ms80";
    case ms160:
      return "ms160";
    case ms240:
      return "ms240";
    default:
      invalid_enum_value(value, "wus_cfg_r15_s::time_offset_e_drx_short_r15_e_");
  }
  return "";
}
uint8_t wus_cfg_r15_s::time_offset_e_drx_short_r15_e_::to_number() const
{
  const static uint8_t options[] = {40, 80, 160, 240};
  return get_enum_number(options, 4, value, "wus_cfg_r15_s::time_offset_e_drx_short_r15_e_");
}

std::string wus_cfg_r15_s::time_offset_e_drx_long_r15_e_::to_string() const
{
  switch (value) {
    case ms1000:
      return "ms1000";
    case ms2000:
      return "ms2000";
    default:
      invalid_enum_value(value, "wus_cfg_r15_s::time_offset_e_drx_long_r15_e_");
  }
  return "";
}
uint16_t wus_cfg_r15_s::time_offset_e_drx_long_r15_e_::to_number() const
{
  const static uint16_t options[] = {1000, 2000};
  return get_enum_number(options, 2, value, "wus_cfg_r15_s::time_offset_e_drx_long_r15_e_");
}

std::string cell_resel_info_common_v1460_s::s_search_delta_p_r14_e_::to_string() const
{
  switch (value) {
    case db6:
      return "dB6";
    case db9:
      return "dB9";
    case db12:
      return "dB12";
    case db15:
      return "dB15";
    default:
      invalid_enum_value(value, "cell_resel_info_common_v1460_s::s_search_delta_p_r14_e_");
  }
  return "";
}
uint8_t cell_resel_info_common_v1460_s::s_search_delta_p_r14_e_::to_number() const
{
  const static uint8_t options[] = {6, 9, 12, 15};
  return get_enum_number(options, 4, value, "cell_resel_info_common_v1460_s::s_search_delta_p_r14_e_");
}

std::string mbms_notif_cfg_r9_s::notif_repeat_coeff_r9_e_::to_string() const
{
  switch (value) {
    case n2:
      return "n2";
    case n4:
      return "n4";
    default:
      invalid_enum_value(value, "mbms_notif_cfg_r9_s::notif_repeat_coeff_r9_e_");
  }
  return "";
}
uint8_t mbms_notif_cfg_r9_s::notif_repeat_coeff_r9_e_::to_number() const
{
  const static uint8_t options[] = {2, 4};
  return get_enum_number(options, 2, value, "mbms_notif_cfg_r9_s::notif_repeat_coeff_r9_e_");
}

std::string mob_state_params_s::t_eval_e_::to_string() const
{
  switch (value) {
    case s30:
      return "s30";
    case s60:
      return "s60";
    case s120:
      return "s120";
    case s180:
      return "s180";
    case s240:
      return "s240";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "mob_state_params_s::t_eval_e_");
  }
  return "";
}
uint8_t mob_state_params_s::t_eval_e_::to_number() const
{
  const static uint8_t options[] = {30, 60, 120, 180, 240};
  return get_enum_number(options, 5, value, "mob_state_params_s::t_eval_e_");
}

std::string mob_state_params_s::t_hyst_normal_e_::to_string() const
{
  switch (value) {
    case s30:
      return "s30";
    case s60:
      return "s60";
    case s120:
      return "s120";
    case s180:
      return "s180";
    case s240:
      return "s240";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "mob_state_params_s::t_hyst_normal_e_");
  }
  return "";
}
uint8_t mob_state_params_s::t_hyst_normal_e_::to_number() const
{
  const static uint8_t options[] = {30, 60, 120, 180, 240};
  return get_enum_number(options, 5, value, "mob_state_params_s::t_hyst_normal_e_");
}

std::string redist_serving_info_r13_s::t360_r13_e_::to_string() const
{
  switch (value) {
    case min4:
      return "min4";
    case min8:
      return "min8";
    case min16:
      return "min16";
    case min32:
      return "min32";
    case infinity:
      return "infinity";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "redist_serving_info_r13_s::t360_r13_e_");
  }
  return "";
}
int8_t redist_serving_info_r13_s::t360_r13_e_::to_number() const
{
  const static int8_t options[] = {4, 8, 16, 32, -1};
  return get_enum_number(options, 5, value, "redist_serving_info_r13_s::t360_r13_e_");
}

std::string sc_mcch_sched_info_r14_s::on_dur_timer_scptm_r14_e_::to_string() const
{
  switch (value) {
    case psf10:
      return "psf10";
    case psf20:
      return "psf20";
    case psf100:
      return "psf100";
    case psf300:
      return "psf300";
    case psf500:
      return "psf500";
    case psf1000:
      return "psf1000";
    case psf1200:
      return "psf1200";
    case psf1600:
      return "psf1600";
    default:
      invalid_enum_value(value, "sc_mcch_sched_info_r14_s::on_dur_timer_scptm_r14_e_");
  }
  return "";
}
uint16_t sc_mcch_sched_info_r14_s::on_dur_timer_scptm_r14_e_::to_number() const
{
  const static uint16_t options[] = {10, 20, 100, 300, 500, 1000, 1200, 1600};
  return get_enum_number(options, 8, value, "sc_mcch_sched_info_r14_s::on_dur_timer_scptm_r14_e_");
}

std::string sc_mcch_sched_info_r14_s::drx_inactivity_timer_scptm_r14_e_::to_string() const
{
  switch (value) {
    case psf0:
      return "psf0";
    case psf1:
      return "psf1";
    case psf2:
      return "psf2";
    case psf4:
      return "psf4";
    case psf8:
      return "psf8";
    case psf16:
      return "psf16";
    case psf32:
      return "psf32";
    case psf64:
      return "psf64";
    case psf128:
      return "psf128";
    case psf256:
      return "psf256";
    case ps512:
      return "ps512";
    case psf1024:
      return "psf1024";
    case psf2048:
      return "psf2048";
    case psf4096:
      return "psf4096";
    case psf8192:
      return "psf8192";
    case psf16384:
      return "psf16384";
    default:
      invalid_enum_value(value, "sc_mcch_sched_info_r14_s::drx_inactivity_timer_scptm_r14_e_");
  }
  return "";
}
uint16_t sc_mcch_sched_info_r14_s::drx_inactivity_timer_scptm_r14_e_::to_number() const
{
  const static uint16_t options[] = {0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384};
  return get_enum_number(options, 16, value, "sc_mcch_sched_info_r14_s::drx_inactivity_timer_scptm_r14_e_");
}

std::string sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::types::to_string() const
{
  switch (value) {
    case sf10:
      return "sf10";
    case sf20:
      return "sf20";
    case sf32:
      return "sf32";
    case sf40:
      return "sf40";
    case sf64:
      return "sf64";
    case sf80:
      return "sf80";
    case sf128:
      return "sf128";
    case sf160:
      return "sf160";
    case sf256:
      return "sf256";
    case sf320:
      return "sf320";
    case sf512:
      return "sf512";
    case sf640:
      return "sf640";
    case sf1024:
      return "sf1024";
    case sf2048:
      return "sf2048";
    case sf4096:
      return "sf4096";
    case sf8192:
      return "sf8192";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::types");
  }
  return "";
}
uint16_t sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::types::to_number() const
{
  const static uint16_t options[] = {10, 20, 32, 40, 64, 80, 128, 160, 256, 320, 512, 640, 1024, 2048, 4096, 8192};
  return get_enum_number(options, 16, value, "sc_mcch_sched_info_r14_s::sched_period_start_offset_scptm_r14_c_::types");
}

std::string sl_disc_cfg_relay_ue_r13_s::hyst_max_r13_e_::to_string() const
{
  switch (value) {
    case db0:
      return "dB0";
    case db3:
      return "dB3";
    case db6:
      return "dB6";
    case db9:
      return "dB9";
    case db12:
      return "dB12";
    case dbinf:
      return "dBinf";
    default:
      invalid_enum_value(value, "sl_disc_cfg_relay_ue_r13_s::hyst_max_r13_e_");
  }
  return "";
}
uint8_t sl_disc_cfg_relay_ue_r13_s::hyst_max_r13_e_::to_number() const
{
  const static uint8_t options[] = {0, 3, 6, 9, 12};
  return get_enum_number(options, 5, value, "sl_disc_cfg_relay_ue_r13_s::hyst_max_r13_e_");
}

std::string sl_disc_cfg_relay_ue_r13_s::hyst_min_r13_e_::to_string() const
{
  switch (value) {
    case db0:
      return "dB0";
    case db3:
      return "dB3";
    case db6:
      return "dB6";
    case db9:
      return "dB9";
    case db12:
      return "dB12";
    default:
      invalid_enum_value(value, "sl_disc_cfg_relay_ue_r13_s::hyst_min_r13_e_");
  }
  return "";
}
uint8_t sl_disc_cfg_relay_ue_r13_s::hyst_min_r13_e_::to_number() const
{
  const static uint8_t options[] = {0, 3, 6, 9, 12};
  return get_enum_number(options, 5, value, "sl_disc_cfg_relay_ue_r13_s::hyst_min_r13_e_");
}

std::string sl_disc_cfg_remote_ue_r13_s::hyst_max_r13_e_::to_string() const
{
  switch (value) {
    case db0:
      return "dB0";
    case db3:
      return "dB3";
    case db6:
      return "dB6";
    case db9:
      return "dB9";
    case db12:
      return "dB12";
    default:
      invalid_enum_value(value, "sl_disc_cfg_remote_ue_r13_s::hyst_max_r13_e_");
  }
  return "";
}
uint8_t sl_disc_cfg_remote_ue_r13_s::hyst_max_r13_e_::to_number() const
{
  const static uint8_t options[] = {0, 3, 6, 9, 12};
  return get_enum_number(options, 5, value, "sl_disc_cfg_remote_ue_r13_s::hyst_max_r13_e_");
}

// TimeAlignmentTimer ::= ENUMERATED
std::string time_align_timer_e::to_string() const
{
  switch (value) {
    case sf500:
      return "sf500";
    case sf750:
      return "sf750";
    case sf1280:
      return "sf1280";
    case sf1920:
      return "sf1920";
    case sf2560:
      return "sf2560";
    case sf5120:
      return "sf5120";
    case sf10240:
      return "sf10240";
    case infinity:
      return "infinity";
    default:
      invalid_enum_value(value, "time_align_timer_e");
  }
  return "";
}
int16_t time_align_timer_e::to_number() const
{
  const static int16_t options[] = {500, 750, 1280, 1920, 2560, 5120, 10240, -1};
  return get_enum_number(options, 8, value, "time_align_timer_e");
}

// UAC-AC1-SelectAssistInfo-r15 ::= ENUMERATED
std::string uac_ac1_select_assist_info_r15_e::to_string() const
{
  switch (value) {
    case a:
      return "a";
    case b:
      return "b";
    case c:
      return "c";
    default:
      invalid_enum_value(value, "uac_ac1_select_assist_info_r15_e");
  }
  return "";
}

std::string ue_timers_and_consts_s::t300_e_::to_string() const
{
  switch (value) {
    case ms100:
      return "ms100";
    case ms200:
      return "ms200";
    case ms300:
      return "ms300";
    case ms400:
      return "ms400";
    case ms600:
      return "ms600";
    case ms1000:
      return "ms1000";
    case ms1500:
      return "ms1500";
    case ms2000:
      return "ms2000";
    default:
      invalid_enum_value(value, "ue_timers_and_consts_s::t300_e_");
  }
  return "";
}
uint16_t ue_timers_and_consts_s::t300_e_::to_number() const
{
  const static uint16_t options[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return get_enum_number(options, 8, value, "ue_timers_and_consts_s::t300_e_");
}

std::string ue_timers_and_consts_s::t301_e_::to_string() const
{
  switch (value) {
    case ms100:
      return "ms100";
    case ms200:
      return "ms200";
    case ms300:
      return "ms300";
    case ms400:
      return "ms400";
    case ms600:
      return "ms600";
    case ms1000:
      return "ms1000";
    case ms1500:
      return "ms1500";
    case ms2000:
      return "ms2000";
    default:
      invalid_enum_value(value, "ue_timers_and_consts_s::t301_e_");
  }
  return "";
}
uint16_t ue_timers_and_consts_s::t301_e_::to_number() const
{
  const static uint16_t options[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return get_enum_number(options, 8, value, "ue_timers_and_consts_s::t301_e_");
}

std::string ue_timers_and_consts_s::t310_e_::to_string() const
{
  switch (value) {
    case ms0:
      return "ms0";
    case ms50:
      return "ms50";
    case ms100:
      return "ms100";
    case ms200:
      return "ms200";
    case ms500:
      return "ms500";
    case ms1000:
      return "ms1000";
    case ms2000:
      return "ms2000";
    default:
      invalid_enum_value(value, "ue_timers_and_consts_s::t310_e_");
  }
  return "";
}
uint16_t ue_timers_and_consts_s::t310_e_::to_number() const
{
  const static uint16_t options[] = {0, 50, 100, 200, 500, 1000, 2000};
  return get_enum_number(options, 7, value, "ue_timers_and_consts_s::t310_e_");
}

std::string ue_timers_and_consts_s::n310_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n2:
      return "n2";
    case n3:
      return "n3";
    case n4:
      return "n4";
    case n6:
      return "n6";
    case n8:
      return "n8";
    case n10:
      return "n10";
    case n20:
      return "n20";
    default:
      invalid_enum_value(value, "ue_timers_and_consts_s::n310_e_");
  }
  return "";
}
uint8_t ue_timers_and_consts_s::n310_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 4, 6, 8, 10, 20};
  return get_enum_number(options, 8, value, "ue_timers_and_consts_s::n310_e_");
}

std::string ue_timers_and_consts_s::t311_e_::to_string() const
{
  switch (value) {
    case ms1000:
      return "ms1000";
    case ms3000:
      return "ms3000";
    case ms5000:
      return "ms5000";
    case ms10000:
      return "ms10000";
    case ms15000:
      return "ms15000";
    case ms20000:
      return "ms20000";
    case ms30000:
      return "ms30000";
    default:
      invalid_enum_value(value, "ue_timers_and_consts_s::t311_e_");
  }
  return "";
}
uint16_t ue_timers_and_consts_s::t311_e_::to_number() const
{
  const static uint16_t options[] = {1000, 3000, 5000, 10000, 15000, 20000, 30000};
  return get_enum_number(options, 7, value, "ue_timers_and_consts_s::t311_e_");
}

std::string ue_timers_and_consts_s::n311_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n2:
      return "n2";
    case n3:
      return "n3";
    case n4:
      return "n4";
    case n5:
      return "n5";
    case n6:
      return "n6";
    case n8:
      return "n8";
    case n10:
      return "n10";
    default:
      invalid_enum_value(value, "ue_timers_and_consts_s::n311_e_");
  }
  return "";
}
uint8_t ue_timers_and_consts_s::n311_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 4, 5, 6, 8, 10};
  return get_enum_number(options, 8, value, "ue_timers_and_consts_s::n311_e_");
}

std::string ue_timers_and_consts_s::t300_v1310_e_::to_string() const
{
  switch (value) {
    case ms2500:
      return "ms2500";
    case ms3000:
      return "ms3000";
    case ms3500:
      return "ms3500";
    case ms4000:
      return "ms4000";
    case ms5000:
      return "ms5000";
    case ms6000:
      return "ms6000";
    case ms8000:
      return "ms8000";
    case ms10000:
      return "ms10000";
    default:
      invalid_enum_value(value, "ue_timers_and_consts_s::t300_v1310_e_");
  }
  return "";
}
uint16_t ue_timers_and_consts_s::t300_v1310_e_::to_number() const
{
  const static uint16_t options[] = {2500, 3000, 3500, 4000, 5000, 6000, 8000, 10000};
  return get_enum_number(options, 8, value, "ue_timers_and_consts_s::t300_v1310_e_");
}

std::string ue_timers_and_consts_s::t301_v1310_e_::to_string() const
{
  switch (value) {
    case ms2500:
      return "ms2500";
    case ms3000:
      return "ms3000";
    case ms3500:
      return "ms3500";
    case ms4000:
      return "ms4000";
    case ms5000:
      return "ms5000";
    case ms6000:
      return "ms6000";
    case ms8000:
      return "ms8000";
    case ms10000:
      return "ms10000";
    default:
      invalid_enum_value(value, "ue_timers_and_consts_s::t301_v1310_e_");
  }
  return "";
}
uint16_t ue_timers_and_consts_s::t301_v1310_e_::to_number() const
{
  const static uint16_t options[] = {2500, 3000, 3500, 4000, 5000, 6000, 8000, 10000};
  return get_enum_number(options, 8, value, "ue_timers_and_consts_s::t301_v1310_e_");
}

std::string ue_timers_and_consts_s::t310_v1330_e_::to_string() const
{
  switch (value) {
    case ms4000:
      return "ms4000";
    case ms6000:
      return "ms6000";
    default:
      invalid_enum_value(value, "ue_timers_and_consts_s::t310_v1330_e_");
  }
  return "";
}
uint16_t ue_timers_and_consts_s::t310_v1330_e_::to_number() const
{
  const static uint16_t options[] = {4000, 6000};
  return get_enum_number(options, 2, value, "ue_timers_and_consts_s::t310_v1330_e_");
}

std::string ue_timers_and_consts_s::t300_r15_e_::to_string() const
{
  switch (value) {
    case ms4000:
      return "ms4000";
    case ms6000:
      return "ms6000";
    case ms8000:
      return "ms8000";
    case ms10000:
      return "ms10000";
    case ms15000:
      return "ms15000";
    case ms25000:
      return "ms25000";
    case ms40000:
      return "ms40000";
    case ms60000:
      return "ms60000";
    default:
      invalid_enum_value(value, "ue_timers_and_consts_s::t300_r15_e_");
  }
  return "";
}
uint16_t ue_timers_and_consts_s::t300_r15_e_::to_number() const
{
  const static uint16_t options[] = {4000, 6000, 8000, 10000, 15000, 25000, 40000, 60000};
  return get_enum_number(options, 8, value, "ue_timers_and_consts_s::t300_r15_e_");
}

std::string sched_info_s::si_periodicity_e_::to_string() const
{
  switch (value) {
    case rf8:
      return "rf8";
    case rf16:
      return "rf16";
    case rf32:
      return "rf32";
    case rf64:
      return "rf64";
    case rf128:
      return "rf128";
    case rf256:
      return "rf256";
    case rf512:
      return "rf512";
    default:
      invalid_enum_value(value, "sched_info_s::si_periodicity_e_");
  }
  return "";
}
uint16_t sched_info_s::si_periodicity_e_::to_number() const
{
  const static uint16_t options[] = {8, 16, 32, 64, 128, 256, 512};
  return get_enum_number(options, 7, value, "sched_info_s::si_periodicity_e_");
}

std::string sib_type11_s::warning_msg_segment_type_e_::to_string() const
{
  switch (value) {
    case not_last_segment:
      return "notLastSegment";
    case last_segment:
      return "lastSegment";
    default:
      invalid_enum_value(value, "sib_type11_s::warning_msg_segment_type_e_");
  }
  return "";
}

std::string sib_type12_r9_s::warning_msg_segment_type_r9_e_::to_string() const
{
  switch (value) {
    case not_last_segment:
      return "notLastSegment";
    case last_segment:
      return "lastSegment";
    default:
      invalid_enum_value(value, "sib_type12_r9_s::warning_msg_segment_type_r9_e_");
  }
  return "";
}

std::string sib_type14_r11_s::eab_param_r11_c_::types::to_string() const
{
  switch (value) {
    case eab_common_r11:
      return "eab-Common-r11";
    case eab_per_plmn_list_r11:
      return "eab-PerPLMN-List-r11";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sib_type14_r11_s::eab_param_r11_c_::types");
  }
  return "";
}

std::string sib_type14_r11_s::eab_per_rsrp_r15_e_::to_string() const
{
  switch (value) {
    case thresh0:
      return "thresh0";
    case thresh1:
      return "thresh1";
    case thresh2:
      return "thresh2";
    case thresh3:
      return "thresh3";
    default:
      invalid_enum_value(value, "sib_type14_r11_s::eab_per_rsrp_r15_e_");
  }
  return "";
}
uint8_t sib_type14_r11_s::eab_per_rsrp_r15_e_::to_number() const
{
  const static uint8_t options[] = {0, 1, 2, 3};
  return get_enum_number(options, 4, value, "sib_type14_r11_s::eab_per_rsrp_r15_e_");
}

std::string sib_type2_s::freq_info_s_::ul_bw_e_::to_string() const
{
  switch (value) {
    case n6:
      return "n6";
    case n15:
      return "n15";
    case n25:
      return "n25";
    case n50:
      return "n50";
    case n75:
      return "n75";
    case n100:
      return "n100";
    default:
      invalid_enum_value(value, "sib_type2_s::freq_info_s_::ul_bw_e_");
  }
  return "";
}
uint8_t sib_type2_s::freq_info_s_::ul_bw_e_::to_number() const
{
  const static uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return get_enum_number(options, 6, value, "sib_type2_s::freq_info_s_::ul_bw_e_");
}

std::string sib_type20_r13_s::sc_mcch_repeat_period_r13_e_::to_string() const
{
  switch (value) {
    case rf2:
      return "rf2";
    case rf4:
      return "rf4";
    case rf8:
      return "rf8";
    case rf16:
      return "rf16";
    case rf32:
      return "rf32";
    case rf64:
      return "rf64";
    case rf128:
      return "rf128";
    case rf256:
      return "rf256";
    default:
      invalid_enum_value(value, "sib_type20_r13_s::sc_mcch_repeat_period_r13_e_");
  }
  return "";
}
uint16_t sib_type20_r13_s::sc_mcch_repeat_period_r13_e_::to_number() const
{
  const static uint16_t options[] = {2, 4, 8, 16, 32, 64, 128, 256};
  return get_enum_number(options, 8, value, "sib_type20_r13_s::sc_mcch_repeat_period_r13_e_");
}

std::string sib_type20_r13_s::sc_mcch_mod_period_r13_e_::to_string() const
{
  switch (value) {
    case rf2:
      return "rf2";
    case rf4:
      return "rf4";
    case rf8:
      return "rf8";
    case rf16:
      return "rf16";
    case rf32:
      return "rf32";
    case rf64:
      return "rf64";
    case rf128:
      return "rf128";
    case rf256:
      return "rf256";
    case rf512:
      return "rf512";
    case rf1024:
      return "rf1024";
    case r2048:
      return "r2048";
    case rf4096:
      return "rf4096";
    case rf8192:
      return "rf8192";
    case rf16384:
      return "rf16384";
    case rf32768:
      return "rf32768";
    case rf65536:
      return "rf65536";
    default:
      invalid_enum_value(value, "sib_type20_r13_s::sc_mcch_mod_period_r13_e_");
  }
  return "";
}
uint32_t sib_type20_r13_s::sc_mcch_mod_period_r13_e_::to_number() const
{
  const static uint32_t options[] = {2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536};
  return get_enum_number(options, 16, value, "sib_type20_r13_s::sc_mcch_mod_period_r13_e_");
}

std::string sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_num_repeat_sc_mcch_r14_e_::to_string() const
{
  switch (value) {
    case r1:
      return "r1";
    case r2:
      return "r2";
    case r4:
      return "r4";
    case r8:
      return "r8";
    case r16:
      return "r16";
    case r32:
      return "r32";
    case r64:
      return "r64";
    case r128:
      return "r128";
    case r256:
      return "r256";
    default:
      invalid_enum_value(value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_num_repeat_sc_mcch_r14_e_");
  }
  return "";
}
uint16_t sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_num_repeat_sc_mcch_r14_e_::to_number() const
{
  const static uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
  return get_enum_number(options, 9, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_num_repeat_sc_mcch_r14_e_");
}

std::string sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::fdd_r14_e_::to_string() const
{
  switch (value) {
    case v1:
      return "v1";
    case v1dot5:
      return "v1dot5";
    case v2:
      return "v2";
    case v2dot5:
      return "v2dot5";
    case v4:
      return "v4";
    case v5:
      return "v5";
    case v8:
      return "v8";
    case v10:
      return "v10";
    default:
      invalid_enum_value(value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::fdd_r14_e_");
  }
  return "";
}
float sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::fdd_r14_e_::to_number() const
{
  const static float options[] = {1.0, 1.5, 2.0, 2.5, 4.0, 5.0, 8.0, 10.0};
  return get_enum_number(options, 8, value,
                         "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::fdd_r14_e_");
}
std::string sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::fdd_r14_e_::to_number_string() const
{
  switch (value) {
    case v1:
      return "1";
    case v1dot5:
      return "1.5";
    case v2:
      return "2";
    case v2dot5:
      return "2.5";
    case v4:
      return "4";
    case v5:
      return "5";
    case v8:
      return "8";
    case v10:
      return "10";
    default:
      invalid_enum_number(value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::fdd_r14_e_");
  }
  return "";
}

std::string sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::tdd_r14_e_::to_string() const
{
  switch (value) {
    case v1:
      return "v1";
    case v2:
      return "v2";
    case v4:
      return "v4";
    case v5:
      return "v5";
    case v8:
      return "v8";
    case v10:
      return "v10";
    case v20:
      return "v20";
    default:
      invalid_enum_value(value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::tdd_r14_e_");
  }
  return "";
}
uint8_t sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::tdd_r14_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 4, 5, 8, 10, 20};
  return get_enum_number(options, 7, value,
                         "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::tdd_r14_e_");
}

std::string sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::types::to_string() const
{
  switch (value) {
    case fdd_r14:
      return "fdd-r14";
    case tdd_r14:
      return "tdd-r14";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_start_sf_sc_mcch_r14_c_::types");
  }
  return "";
}

std::string sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_pdsch_hop_cfg_sc_mcch_r14_e_::to_string() const
{
  switch (value) {
    case off:
      return "off";
    case ce_mode_a:
      return "ce-ModeA";
    case ce_mode_b:
      return "ce-ModeB";
    default:
      invalid_enum_value(value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::mpdcch_pdsch_hop_cfg_sc_mcch_r14_e_");
  }
  return "";
}

std::string sib_type20_r13_s::br_bcch_cfg_r14_s_::sc_mcch_repeat_period_br_r14_e_::to_string() const
{
  switch (value) {
    case rf32:
      return "rf32";
    case rf128:
      return "rf128";
    case rf512:
      return "rf512";
    case rf1024:
      return "rf1024";
    case rf2048:
      return "rf2048";
    case rf4096:
      return "rf4096";
    case rf8192:
      return "rf8192";
    case rf16384:
      return "rf16384";
    default:
      invalid_enum_value(value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::sc_mcch_repeat_period_br_r14_e_");
  }
  return "";
}
uint16_t sib_type20_r13_s::br_bcch_cfg_r14_s_::sc_mcch_repeat_period_br_r14_e_::to_number() const
{
  const static uint16_t options[] = {32, 128, 512, 1024, 2048, 4096, 8192, 16384};
  return get_enum_number(options, 8, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::sc_mcch_repeat_period_br_r14_e_");
}

std::string sib_type20_r13_s::br_bcch_cfg_r14_s_::sc_mcch_mod_period_br_r14_e_::to_string() const
{
  switch (value) {
    case rf32:
      return "rf32";
    case rf128:
      return "rf128";
    case rf256:
      return "rf256";
    case rf512:
      return "rf512";
    case rf1024:
      return "rf1024";
    case rf2048:
      return "rf2048";
    case rf4096:
      return "rf4096";
    case rf8192:
      return "rf8192";
    case rf16384:
      return "rf16384";
    case rf32768:
      return "rf32768";
    case rf65536:
      return "rf65536";
    case rf131072:
      return "rf131072";
    case rf262144:
      return "rf262144";
    case rf524288:
      return "rf524288";
    case rf1048576:
      return "rf1048576";
    default:
      invalid_enum_value(value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::sc_mcch_mod_period_br_r14_e_");
  }
  return "";
}
uint32_t sib_type20_r13_s::br_bcch_cfg_r14_s_::sc_mcch_mod_period_br_r14_e_::to_number() const
{
  const static uint32_t options[] = {32,    128,   256,   512,    1024,   2048,   4096,   8192,
                                     16384, 32768, 65536, 131072, 262144, 524288, 1048576};
  return get_enum_number(options, 15, value, "sib_type20_r13_s::br_bcch_cfg_r14_s_::sc_mcch_mod_period_br_r14_e_");
}

std::string sib_type20_r13_s::pdsch_max_num_repeat_cemode_a_sc_mtch_r14_e_::to_string() const
{
  switch (value) {
    case r16:
      return "r16";
    case r32:
      return "r32";
    default:
      invalid_enum_value(value, "sib_type20_r13_s::pdsch_max_num_repeat_cemode_a_sc_mtch_r14_e_");
  }
  return "";
}
uint8_t sib_type20_r13_s::pdsch_max_num_repeat_cemode_a_sc_mtch_r14_e_::to_number() const
{
  const static uint8_t options[] = {16, 32};
  return get_enum_number(options, 2, value, "sib_type20_r13_s::pdsch_max_num_repeat_cemode_a_sc_mtch_r14_e_");
}

std::string sib_type20_r13_s::pdsch_max_num_repeat_cemode_b_sc_mtch_r14_e_::to_string() const
{
  switch (value) {
    case r192:
      return "r192";
    case r256:
      return "r256";
    case r384:
      return "r384";
    case r512:
      return "r512";
    case r768:
      return "r768";
    case r1024:
      return "r1024";
    case r1536:
      return "r1536";
    case r2048:
      return "r2048";
    default:
      invalid_enum_value(value, "sib_type20_r13_s::pdsch_max_num_repeat_cemode_b_sc_mtch_r14_e_");
  }
  return "";
}
uint16_t sib_type20_r13_s::pdsch_max_num_repeat_cemode_b_sc_mtch_r14_e_::to_number() const
{
  const static uint16_t options[] = {192, 256, 384, 512, 768, 1024, 1536, 2048};
  return get_enum_number(options, 8, value, "sib_type20_r13_s::pdsch_max_num_repeat_cemode_b_sc_mtch_r14_e_");
}

std::string sib_type25_r15_s::uac_ac1_select_assist_info_r15_c_::types::to_string() const
{
  switch (value) {
    case plmn_common_r15:
      return "plmnCommon-r15";
    case individual_plmn_list_r15:
      return "individualPLMNList-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sib_type25_r15_s::uac_ac1_select_assist_info_r15_c_::types");
  }
  return "";
}

std::string sib_type3_s::cell_resel_info_common_s_::q_hyst_e_::to_string() const
{
  switch (value) {
    case db0:
      return "dB0";
    case db1:
      return "dB1";
    case db2:
      return "dB2";
    case db3:
      return "dB3";
    case db4:
      return "dB4";
    case db5:
      return "dB5";
    case db6:
      return "dB6";
    case db8:
      return "dB8";
    case db10:
      return "dB10";
    case db12:
      return "dB12";
    case db14:
      return "dB14";
    case db16:
      return "dB16";
    case db18:
      return "dB18";
    case db20:
      return "dB20";
    case db22:
      return "dB22";
    case db24:
      return "dB24";
    default:
      invalid_enum_value(value, "sib_type3_s::cell_resel_info_common_s_::q_hyst_e_");
  }
  return "";
}
uint8_t sib_type3_s::cell_resel_info_common_s_::q_hyst_e_::to_number() const
{
  const static uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24};
  return get_enum_number(options, 16, value, "sib_type3_s::cell_resel_info_common_s_::q_hyst_e_");
}

std::string
sib_type3_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_e_::to_string() const
{
  switch (value) {
    case db_minus6:
      return "dB-6";
    case db_minus4:
      return "dB-4";
    case db_minus2:
      return "dB-2";
    case db0:
      return "dB0";
    default:
      invalid_enum_value(
          value, "sib_type3_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_e_");
  }
  return "";
}
int8_t sib_type3_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_e_::to_number() const
{
  const static int8_t options[] = {-6, -4, -2, 0};
  return get_enum_number(
      options, 4, value,
      "sib_type3_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_e_");
}

std::string
sib_type3_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_e_::to_string() const
{
  switch (value) {
    case db_minus6:
      return "dB-6";
    case db_minus4:
      return "dB-4";
    case db_minus2:
      return "dB-2";
    case db0:
      return "dB0";
    default:
      invalid_enum_value(value,
                         "sib_type3_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_e_");
  }
  return "";
}
int8_t sib_type3_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_e_::to_number() const
{
  const static int8_t options[] = {-6, -4, -2, 0};
  return get_enum_number(options, 4, value,
                         "sib_type3_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_e_");
}

std::string sib_info_item_c::types::to_string() const
{
  switch (value) {
    case sib2:
      return "sib2";
    case sib3:
      return "sib3";
    case sib4:
      return "sib4";
    case sib5:
      return "sib5";
    case sib6:
      return "sib6";
    case sib7:
      return "sib7";
    case sib8:
      return "sib8";
    case sib9:
      return "sib9";
    case sib10:
      return "sib10";
    case sib11:
      return "sib11";
    case sib12_v920:
      return "sib12-v920";
    case sib13_v920:
      return "sib13-v920";
    case sib14_v1130:
      return "sib14-v1130";
    case sib15_v1130:
      return "sib15-v1130";
    case sib16_v1130:
      return "sib16-v1130";
    case sib17_v1250:
      return "sib17-v1250";
    case sib18_v1250:
      return "sib18-v1250";
    case sib19_v1250:
      return "sib19-v1250";
    case sib20_v1310:
      return "sib20-v1310";
    case sib21_v1430:
      return "sib21-v1430";
    case sib24_v1530:
      return "sib24-v1530";
    case sib25_v1530:
      return "sib25-v1530";
    case sib26_v1530:
      return "sib26-v1530";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sib_info_item_c::types");
  }
  return "";
}
uint8_t sib_info_item_c::types::to_number() const
{
  const static uint8_t options[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 24, 25, 26};
  return get_enum_number(options, 23, value, "sib_info_item_c::types");
}

std::string sys_info_s::crit_exts_c_::crit_exts_future_r15_c_::types::to_string() const
{
  switch (value) {
    case pos_sys_info_r15:
      return "posSystemInformation-r15";
    case crit_exts_future:
      return "criticalExtensionsFuture";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sys_info_s::crit_exts_c_::crit_exts_future_r15_c_::types");
  }
  return "";
}

std::string sys_info_s::crit_exts_c_::types::to_string() const
{
  switch (value) {
    case sys_info_r8:
      return "systemInformation-r8";
    case crit_exts_future_r15:
      return "criticalExtensionsFuture-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sys_info_s::crit_exts_c_::types");
  }
  return "";
}

std::string sib_type1_s::cell_access_related_info_s_::cell_barred_e_::to_string() const
{
  switch (value) {
    case barred:
      return "barred";
    case not_barred:
      return "notBarred";
    default:
      invalid_enum_value(value, "sib_type1_s::cell_access_related_info_s_::cell_barred_e_");
  }
  return "";
}

std::string sib_type1_s::cell_access_related_info_s_::intra_freq_resel_e_::to_string() const
{
  switch (value) {
    case allowed:
      return "allowed";
    case not_allowed:
      return "notAllowed";
    default:
      invalid_enum_value(value, "sib_type1_s::cell_access_related_info_s_::intra_freq_resel_e_");
  }
  return "";
}

std::string sib_type1_s::si_win_len_e_::to_string() const
{
  switch (value) {
    case ms1:
      return "ms1";
    case ms2:
      return "ms2";
    case ms5:
      return "ms5";
    case ms10:
      return "ms10";
    case ms15:
      return "ms15";
    case ms20:
      return "ms20";
    case ms40:
      return "ms40";
    default:
      invalid_enum_value(value, "sib_type1_s::si_win_len_e_");
  }
  return "";
}
uint8_t sib_type1_s::si_win_len_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 5, 10, 15, 20, 40};
  return get_enum_number(options, 7, value, "sib_type1_s::si_win_len_e_");
}

std::string bcch_dl_sch_msg_type_c::c1_c_::types::to_string() const
{
  switch (value) {
    case sys_info:
      return "systemInformation";
    case sib_type1:
      return "systemInformationBlockType1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "bcch_dl_sch_msg_type_c::c1_c_::types");
  }
  return "";
}
uint8_t bcch_dl_sch_msg_type_c::c1_c_::types::to_number() const
{
  switch (value) {
    case sib_type1:
      return 1;
    default:
      invalid_enum_number(value, "bcch_dl_sch_msg_type_c::c1_c_::types");
  }
  return 0;
}

std::string bcch_dl_sch_msg_type_c::types::to_string() const
{
  switch (value) {
    case c1:
      return "c1";
    case msg_class_ext:
      return "messageClassExtension";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "bcch_dl_sch_msg_type_c::types");
  }
  return "";
}
uint8_t bcch_dl_sch_msg_type_c::types::to_number() const
{
  const static uint8_t options[] = {1};
  return get_enum_number(options, 1, value, "bcch_dl_sch_msg_type_c::types");
}

std::string bcch_dl_sch_msg_type_br_r13_c::c1_c_::types::to_string() const
{
  switch (value) {
    case sys_info_br_r13:
      return "systemInformation-BR-r13";
    case sib_type1_br_r13:
      return "systemInformationBlockType1-BR-r13";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "bcch_dl_sch_msg_type_br_r13_c::c1_c_::types");
  }
  return "";
}
uint8_t bcch_dl_sch_msg_type_br_r13_c::c1_c_::types::to_number() const
{
  switch (value) {
    case sib_type1_br_r13:
      return 1;
    default:
      invalid_enum_number(value, "bcch_dl_sch_msg_type_br_r13_c::c1_c_::types");
  }
  return 0;
}

std::string bcch_dl_sch_msg_type_br_r13_c::types::to_string() const
{
  switch (value) {
    case c1:
      return "c1";
    case msg_class_ext:
      return "messageClassExtension";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "bcch_dl_sch_msg_type_br_r13_c::types");
  }
  return "";
}
uint8_t bcch_dl_sch_msg_type_br_r13_c::types::to_number() const
{
  const static uint8_t options[] = {1};
  return get_enum_number(options, 1, value, "bcch_dl_sch_msg_type_br_r13_c::types");
}

// SIB-Type-MBMS-r14 ::= ENUMERATED
std::string sib_type_mbms_r14_e::to_string() const
{
  switch (value) {
    case sib_type10:
      return "sibType10";
    case sib_type11:
      return "sibType11";
    case sib_type12_v920:
      return "sibType12-v920";
    case sib_type13_v920:
      return "sibType13-v920";
    case sib_type15_v1130:
      return "sibType15-v1130";
    case sib_type16_v1130:
      return "sibType16-v1130";
    default:
      invalid_enum_value(value, "sib_type_mbms_r14_e");
  }
  return "";
}
uint8_t sib_type_mbms_r14_e::to_number() const
{
  const static uint8_t options[] = {10, 11, 12, 13, 15, 16};
  return get_enum_number(options, 6, value, "sib_type_mbms_r14_e");
}

std::string sched_info_mbms_r14_s::si_periodicity_r14_e_::to_string() const
{
  switch (value) {
    case rf16:
      return "rf16";
    case rf32:
      return "rf32";
    case rf64:
      return "rf64";
    case rf128:
      return "rf128";
    case rf256:
      return "rf256";
    case rf512:
      return "rf512";
    default:
      invalid_enum_value(value, "sched_info_mbms_r14_s::si_periodicity_r14_e_");
  }
  return "";
}
uint16_t sched_info_mbms_r14_s::si_periodicity_r14_e_::to_number() const
{
  const static uint16_t options[] = {16, 32, 64, 128, 256, 512};
  return get_enum_number(options, 6, value, "sched_info_mbms_r14_s::si_periodicity_r14_e_");
}

std::string non_mbsfn_sf_cfg_r14_s::radio_frame_alloc_period_r14_e_::to_string() const
{
  switch (value) {
    case rf4:
      return "rf4";
    case rf8:
      return "rf8";
    case rf16:
      return "rf16";
    case rf32:
      return "rf32";
    case rf64:
      return "rf64";
    case rf128:
      return "rf128";
    case rf512:
      return "rf512";
    default:
      invalid_enum_value(value, "non_mbsfn_sf_cfg_r14_s::radio_frame_alloc_period_r14_e_");
  }
  return "";
}
uint16_t non_mbsfn_sf_cfg_r14_s::radio_frame_alloc_period_r14_e_::to_number() const
{
  const static uint16_t options[] = {4, 8, 16, 32, 64, 128, 512};
  return get_enum_number(options, 7, value, "non_mbsfn_sf_cfg_r14_s::radio_frame_alloc_period_r14_e_");
}

std::string sib_type1_mbms_r14_s::si_win_len_r14_e_::to_string() const
{
  switch (value) {
    case ms1:
      return "ms1";
    case ms2:
      return "ms2";
    case ms5:
      return "ms5";
    case ms10:
      return "ms10";
    case ms15:
      return "ms15";
    case ms20:
      return "ms20";
    case ms40:
      return "ms40";
    case ms80:
      return "ms80";
    default:
      invalid_enum_value(value, "sib_type1_mbms_r14_s::si_win_len_r14_e_");
  }
  return "";
}
uint8_t sib_type1_mbms_r14_s::si_win_len_r14_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 5, 10, 15, 20, 40, 80};
  return get_enum_number(options, 8, value, "sib_type1_mbms_r14_s::si_win_len_r14_e_");
}

std::string bcch_dl_sch_msg_type_mbms_r14_c::c1_c_::types::to_string() const
{
  switch (value) {
    case sys_info_mbms_r14:
      return "systemInformation-MBMS-r14";
    case sib_type1_mbms_r14:
      return "systemInformationBlockType1-MBMS-r14";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "bcch_dl_sch_msg_type_mbms_r14_c::c1_c_::types");
  }
  return "";
}
uint8_t bcch_dl_sch_msg_type_mbms_r14_c::c1_c_::types::to_number() const
{
  switch (value) {
    case sib_type1_mbms_r14:
      return 1;
    default:
      invalid_enum_number(value, "bcch_dl_sch_msg_type_mbms_r14_c::c1_c_::types");
  }
  return 0;
}

std::string bcch_dl_sch_msg_type_mbms_r14_c::types::to_string() const
{
  switch (value) {
    case c1:
      return "c1";
    case msg_class_ext:
      return "messageClassExtension";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "bcch_dl_sch_msg_type_mbms_r14_c::types");
  }
  return "";
}
uint8_t bcch_dl_sch_msg_type_mbms_r14_c::types::to_number() const
{
  const static uint8_t options[] = {1};
  return get_enum_number(options, 1, value, "bcch_dl_sch_msg_type_mbms_r14_c::types");
}

// NZP-FrequencyDensity-r14 ::= ENUMERATED
std::string nzp_freq_density_r14_e::to_string() const
{
  switch (value) {
    case d1:
      return "d1";
    case d2:
      return "d2";
    case d3:
      return "d3";
    default:
      invalid_enum_value(value, "nzp_freq_density_r14_e");
  }
  return "";
}
uint8_t nzp_freq_density_r14_e::to_number() const
{
  const static uint8_t options[] = {1, 2, 3};
  return get_enum_number(options, 3, value, "nzp_freq_density_r14_e");
}

std::string p_c_and_cbsr_r13_s::cbsr_sel_r13_c_::types::to_string() const
{
  switch (value) {
    case non_precoded_r13:
      return "nonPrecoded-r13";
    case beamformed_k1a_r13:
      return "beamformedK1a-r13";
    case beamformed_kn_r13:
      return "beamformedKN-r13";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "p_c_and_cbsr_r13_s::cbsr_sel_r13_c_::types");
  }
  return "";
}
uint8_t p_c_and_cbsr_r13_s::cbsr_sel_r13_c_::types::to_number() const
{
  switch (value) {
    case beamformed_k1a_r13:
      return 1;
    default:
      invalid_enum_number(value, "p_c_and_cbsr_r13_s::cbsr_sel_r13_c_::types");
  }
  return 0;
}

std::string csi_rs_cfg_nzp_activation_r14_s::csi_rs_nzp_mode_r14_e_::to_string() const
{
  switch (value) {
    case semi_persistent:
      return "semiPersistent";
    case aperiodic:
      return "aperiodic";
    default:
      invalid_enum_value(value, "csi_rs_cfg_nzp_activation_r14_s::csi_rs_nzp_mode_r14_e_");
  }
  return "";
}

std::string csi_rs_cfg_nzp_r11_s::ant_ports_count_r11_e_::to_string() const
{
  switch (value) {
    case an1:
      return "an1";
    case an2:
      return "an2";
    case an4:
      return "an4";
    case an8:
      return "an8";
    default:
      invalid_enum_value(value, "csi_rs_cfg_nzp_r11_s::ant_ports_count_r11_e_");
  }
  return "";
}
uint8_t csi_rs_cfg_nzp_r11_s::ant_ports_count_r11_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 4, 8};
  return get_enum_number(options, 4, value, "csi_rs_cfg_nzp_r11_s::ant_ports_count_r11_e_");
}

std::string csi_rs_cfg_nzp_r11_s::qcl_crs_info_r11_s_::crs_ports_count_r11_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n2:
      return "n2";
    case n4:
      return "n4";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "csi_rs_cfg_nzp_r11_s::qcl_crs_info_r11_s_::crs_ports_count_r11_e_");
  }
  return "";
}
uint8_t csi_rs_cfg_nzp_r11_s::qcl_crs_info_r11_s_::crs_ports_count_r11_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 4};
  return get_enum_number(options, 3, value, "csi_rs_cfg_nzp_r11_s::qcl_crs_info_r11_s_::crs_ports_count_r11_e_");
}

std::string csi_rs_cfg_nzp_emimo_r13_c::setup_s_::cdm_type_r13_e_::to_string() const
{
  switch (value) {
    case cdm2:
      return "cdm2";
    case cdm4:
      return "cdm4";
    default:
      invalid_enum_value(value, "csi_rs_cfg_nzp_emimo_r13_c::setup_s_::cdm_type_r13_e_");
  }
  return "";
}
uint8_t csi_rs_cfg_nzp_emimo_r13_c::setup_s_::cdm_type_r13_e_::to_number() const
{
  const static uint8_t options[] = {2, 4};
  return get_enum_number(options, 2, value, "csi_rs_cfg_nzp_emimo_r13_c::setup_s_::cdm_type_r13_e_");
}

// CQI-ReportModeAperiodic ::= ENUMERATED
std::string cqi_report_mode_aperiodic_e::to_string() const
{
  switch (value) {
    case rm12:
      return "rm12";
    case rm20:
      return "rm20";
    case rm22:
      return "rm22";
    case rm30:
      return "rm30";
    case rm31:
      return "rm31";
    case rm32_v1250:
      return "rm32-v1250";
    case rm10_v1310:
      return "rm10-v1310";
    case rm11_v1310:
      return "rm11-v1310";
    default:
      invalid_enum_value(value, "cqi_report_mode_aperiodic_e");
  }
  return "";
}
uint8_t cqi_report_mode_aperiodic_e::to_number() const
{
  const static uint8_t options[] = {12, 20, 22, 30, 31, 32, 10, 11};
  return get_enum_number(options, 8, value, "cqi_report_mode_aperiodic_e");
}

std::string csi_rs_cfg_non_precoded_r13_s::codebook_cfg_n1_r13_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n2:
      return "n2";
    case n3:
      return "n3";
    case n4:
      return "n4";
    case n8:
      return "n8";
    default:
      invalid_enum_value(value, "csi_rs_cfg_non_precoded_r13_s::codebook_cfg_n1_r13_e_");
  }
  return "";
}
uint8_t csi_rs_cfg_non_precoded_r13_s::codebook_cfg_n1_r13_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 4, 8};
  return get_enum_number(options, 5, value, "csi_rs_cfg_non_precoded_r13_s::codebook_cfg_n1_r13_e_");
}

std::string csi_rs_cfg_non_precoded_r13_s::codebook_cfg_n2_r13_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n2:
      return "n2";
    case n3:
      return "n3";
    case n4:
      return "n4";
    case n8:
      return "n8";
    default:
      invalid_enum_value(value, "csi_rs_cfg_non_precoded_r13_s::codebook_cfg_n2_r13_e_");
  }
  return "";
}
uint8_t csi_rs_cfg_non_precoded_r13_s::codebook_cfg_n2_r13_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 4, 8};
  return get_enum_number(options, 5, value, "csi_rs_cfg_non_precoded_r13_s::codebook_cfg_n2_r13_e_");
}

std::string csi_rs_cfg_non_precoded_r13_s::codebook_over_sampling_rate_cfg_o1_r13_e_::to_string() const
{
  switch (value) {
    case n4:
      return "n4";
    case n8:
      return "n8";
    default:
      invalid_enum_value(value, "csi_rs_cfg_non_precoded_r13_s::codebook_over_sampling_rate_cfg_o1_r13_e_");
  }
  return "";
}
uint8_t csi_rs_cfg_non_precoded_r13_s::codebook_over_sampling_rate_cfg_o1_r13_e_::to_number() const
{
  const static uint8_t options[] = {4, 8};
  return get_enum_number(options, 2, value, "csi_rs_cfg_non_precoded_r13_s::codebook_over_sampling_rate_cfg_o1_r13_e_");
}

std::string csi_rs_cfg_non_precoded_r13_s::codebook_over_sampling_rate_cfg_o2_r13_e_::to_string() const
{
  switch (value) {
    case n4:
      return "n4";
    case n8:
      return "n8";
    default:
      invalid_enum_value(value, "csi_rs_cfg_non_precoded_r13_s::codebook_over_sampling_rate_cfg_o2_r13_e_");
  }
  return "";
}
uint8_t csi_rs_cfg_non_precoded_r13_s::codebook_over_sampling_rate_cfg_o2_r13_e_::to_number() const
{
  const static uint8_t options[] = {4, 8};
  return get_enum_number(options, 2, value, "csi_rs_cfg_non_precoded_r13_s::codebook_over_sampling_rate_cfg_o2_r13_e_");
}

std::string csi_rs_cfg_non_precoded_v1430_s::codebook_cfg_n1_v1430_e_::to_string() const
{
  switch (value) {
    case n5:
      return "n5";
    case n6:
      return "n6";
    case n7:
      return "n7";
    case n10:
      return "n10";
    case n12:
      return "n12";
    case n14:
      return "n14";
    case n16:
      return "n16";
    default:
      invalid_enum_value(value, "csi_rs_cfg_non_precoded_v1430_s::codebook_cfg_n1_v1430_e_");
  }
  return "";
}
uint8_t csi_rs_cfg_non_precoded_v1430_s::codebook_cfg_n1_v1430_e_::to_number() const
{
  const static uint8_t options[] = {5, 6, 7, 10, 12, 14, 16};
  return get_enum_number(options, 7, value, "csi_rs_cfg_non_precoded_v1430_s::codebook_cfg_n1_v1430_e_");
}

std::string csi_rs_cfg_non_precoded_v1430_s::codebook_cfg_n2_v1430_e_::to_string() const
{
  switch (value) {
    case n5:
      return "n5";
    case n6:
      return "n6";
    case n7:
      return "n7";
    default:
      invalid_enum_value(value, "csi_rs_cfg_non_precoded_v1430_s::codebook_cfg_n2_v1430_e_");
  }
  return "";
}
uint8_t csi_rs_cfg_non_precoded_v1430_s::codebook_cfg_n2_v1430_e_::to_number() const
{
  const static uint8_t options[] = {5, 6, 7};
  return get_enum_number(options, 3, value, "csi_rs_cfg_non_precoded_v1430_s::codebook_cfg_n2_v1430_e_");
}

std::string csi_rs_cfg_non_precoded_v1480_s::codebook_cfg_n1_v1480_e_::to_string() const
{
  switch (value) {
    case n5:
      return "n5";
    case n6:
      return "n6";
    case n7:
      return "n7";
    case n10:
      return "n10";
    case n12:
      return "n12";
    case n14:
      return "n14";
    case n16:
      return "n16";
    default:
      invalid_enum_value(value, "csi_rs_cfg_non_precoded_v1480_s::codebook_cfg_n1_v1480_e_");
  }
  return "";
}
uint8_t csi_rs_cfg_non_precoded_v1480_s::codebook_cfg_n1_v1480_e_::to_number() const
{
  const static uint8_t options[] = {5, 6, 7, 10, 12, 14, 16};
  return get_enum_number(options, 7, value, "csi_rs_cfg_non_precoded_v1480_s::codebook_cfg_n1_v1480_e_");
}

std::string csi_rs_cfg_non_precoded_v1480_s::codebook_cfg_n2_r1480_e_::to_string() const
{
  switch (value) {
    case n5:
      return "n5";
    case n6:
      return "n6";
    case n7:
      return "n7";
    default:
      invalid_enum_value(value, "csi_rs_cfg_non_precoded_v1480_s::codebook_cfg_n2_r1480_e_");
  }
  return "";
}
uint8_t csi_rs_cfg_non_precoded_v1480_s::codebook_cfg_n2_r1480_e_::to_number() const
{
  const static uint8_t options[] = {5, 6, 7};
  return get_enum_number(options, 3, value, "csi_rs_cfg_non_precoded_v1480_s::codebook_cfg_n2_r1480_e_");
}

std::string csi_rs_cfg_emimo_r13_c::setup_c_::types::to_string() const
{
  switch (value) {
    case non_precoded_r13:
      return "nonPrecoded-r13";
    case beamformed_r13:
      return "beamformed-r13";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "csi_rs_cfg_emimo_r13_c::setup_c_::types");
  }
  return "";
}

std::string csi_rs_cfg_emimo_v1430_c::setup_c_::types::to_string() const
{
  switch (value) {
    case non_precoded_v1430:
      return "nonPrecoded-v1430";
    case beamformed_v1430:
      return "beamformed-v1430";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "csi_rs_cfg_emimo_v1430_c::setup_c_::types");
  }
  return "";
}

std::string csi_rs_cfg_emimo_v1480_c::setup_c_::types::to_string() const
{
  switch (value) {
    case non_precoded_v1480:
      return "nonPrecoded-v1480";
    case beamformed_v1480:
      return "beamformed-v1480";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "csi_rs_cfg_emimo_v1480_c::setup_c_::types");
  }
  return "";
}

std::string csi_rs_cfg_emimo_v1530_c::setup_c_::types::to_string() const
{
  switch (value) {
    case non_precoded_v1530:
      return "nonPrecoded-v1530";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "csi_rs_cfg_emimo_v1530_c::setup_c_::types");
  }
  return "";
}

std::string cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::wideband_cqi_r11_s_::
    csi_report_mode_r11_e_::to_string() const
{
  switch (value) {
    case submode1:
      return "submode1";
    case submode2:
      return "submode2";
    default:
      invalid_enum_value(value, "cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::wideband_cqi_r11_"
                                "s_::csi_report_mode_r11_e_");
  }
  return "";
}
uint8_t cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::wideband_cqi_r11_s_::
    csi_report_mode_r11_e_::to_number() const
{
  const static uint8_t options[] = {1, 2};
  return get_enum_number(options, 2, value,
                         "cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::wideband_cqi_r11_s_::csi_"
                         "report_mode_r11_e_");
}

std::string cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::subband_cqi_r11_s_::
    periodicity_factor_r11_e_::to_string() const
{
  switch (value) {
    case n2:
      return "n2";
    case n4:
      return "n4";
    default:
      invalid_enum_value(value, "cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::subband_cqi_r11_s_"
                                "::periodicity_factor_r11_e_");
  }
  return "";
}
uint8_t cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::subband_cqi_r11_s_::
    periodicity_factor_r11_e_::to_number() const
{
  const static uint8_t options[] = {2, 4};
  return get_enum_number(options, 2, value,
                         "cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::subband_cqi_r11_s_::"
                         "periodicity_factor_r11_e_");
}

std::string cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::types::to_string() const
{
  switch (value) {
    case wideband_cqi_r11:
      return "widebandCQI-r11";
    case subband_cqi_r11:
      return "subbandCQI-r11";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "cqi_report_periodic_proc_ext_r11_s::cqi_format_ind_periodic_r11_c_::types");
  }
  return "";
}

std::string cqi_report_periodic_proc_ext_r11_s::periodicity_factor_wb_r13_e_::to_string() const
{
  switch (value) {
    case n2:
      return "n2";
    case n4:
      return "n4";
    default:
      invalid_enum_value(value, "cqi_report_periodic_proc_ext_r11_s::periodicity_factor_wb_r13_e_");
  }
  return "";
}
uint8_t cqi_report_periodic_proc_ext_r11_s::periodicity_factor_wb_r13_e_::to_number() const
{
  const static uint8_t options[] = {2, 4};
  return get_enum_number(options, 2, value, "cqi_report_periodic_proc_ext_r11_s::periodicity_factor_wb_r13_e_");
}

// PollByte-r14 ::= ENUMERATED
std::string poll_byte_r14_e::to_string() const
{
  switch (value) {
    case kb1:
      return "kB1";
    case kb2:
      return "kB2";
    case kb5:
      return "kB5";
    case kb8:
      return "kB8";
    case kb10:
      return "kB10";
    case kb15:
      return "kB15";
    case kb3500:
      return "kB3500";
    case kb4000:
      return "kB4000";
    case kb4500:
      return "kB4500";
    case kb5000:
      return "kB5000";
    case kb5500:
      return "kB5500";
    case kb6000:
      return "kB6000";
    case kb6500:
      return "kB6500";
    case kb7000:
      return "kB7000";
    case kb7500:
      return "kB7500";
    case kb8000:
      return "kB8000";
    case kb9000:
      return "kB9000";
    case kb10000:
      return "kB10000";
    case kb11000:
      return "kB11000";
    case kb12000:
      return "kB12000";
    case kb13000:
      return "kB13000";
    case kb14000:
      return "kB14000";
    case kb15000:
      return "kB15000";
    case kb16000:
      return "kB16000";
    case kb17000:
      return "kB17000";
    case kb18000:
      return "kB18000";
    case kb19000:
      return "kB19000";
    case kb20000:
      return "kB20000";
    case kb25000:
      return "kB25000";
    case kb30000:
      return "kB30000";
    case kb35000:
      return "kB35000";
    case kb40000:
      return "kB40000";
    default:
      invalid_enum_value(value, "poll_byte_r14_e");
  }
  return "";
}
uint16_t poll_byte_r14_e::to_number() const
{
  const static uint16_t options[] = {1,     2,     5,     8,     10,    15,    3500,  4000,  4500,  5000,  5500,
                                     6000,  6500,  7000,  7500,  8000,  9000,  10000, 11000, 12000, 13000, 14000,
                                     15000, 16000, 17000, 18000, 19000, 20000, 25000, 30000, 35000, 40000};
  return get_enum_number(options, 32, value, "poll_byte_r14_e");
}

// PollPDU-r15 ::= ENUMERATED
std::string poll_pdu_r15_e::to_string() const
{
  switch (value) {
    case p4:
      return "p4";
    case p8:
      return "p8";
    case p16:
      return "p16";
    case p32:
      return "p32";
    case p64:
      return "p64";
    case p128:
      return "p128";
    case p256:
      return "p256";
    case p512:
      return "p512";
    case p1024:
      return "p1024";
    case p2048_r15:
      return "p2048-r15";
    case p4096_r15:
      return "p4096-r15";
    case p6144_r15:
      return "p6144-r15";
    case p8192_r15:
      return "p8192-r15";
    case p12288_r15:
      return "p12288-r15";
    case p16384_r15:
      return "p16384-r15";
    case p_infinity:
      return "pInfinity";
    default:
      invalid_enum_value(value, "poll_pdu_r15_e");
  }
  return "";
}
int16_t poll_pdu_r15_e::to_number() const
{
  const static int16_t options[] = {4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 6144, 8192, 12288, 16384, -1};
  return get_enum_number(options, 16, value, "poll_pdu_r15_e");
}

// SN-FieldLength ::= ENUMERATED
std::string sn_field_len_e::to_string() const
{
  switch (value) {
    case size5:
      return "size5";
    case size10:
      return "size10";
    default:
      invalid_enum_value(value, "sn_field_len_e");
  }
  return "";
}
uint8_t sn_field_len_e::to_number() const
{
  const static uint8_t options[] = {5, 10};
  return get_enum_number(options, 2, value, "sn_field_len_e");
}

// SN-FieldLength-r15 ::= ENUMERATED
std::string sn_field_len_r15_e::to_string() const
{
  switch (value) {
    case size5:
      return "size5";
    case size10:
      return "size10";
    case size16_r15:
      return "size16-r15";
    default:
      invalid_enum_value(value, "sn_field_len_r15_e");
  }
  return "";
}
uint8_t sn_field_len_r15_e::to_number() const
{
  const static uint8_t options[] = {5, 10, 16};
  return get_enum_number(options, 3, value, "sn_field_len_r15_e");
}

// T-PollRetransmit ::= ENUMERATED
std::string t_poll_retx_e::to_string() const
{
  switch (value) {
    case ms5:
      return "ms5";
    case ms10:
      return "ms10";
    case ms15:
      return "ms15";
    case ms20:
      return "ms20";
    case ms25:
      return "ms25";
    case ms30:
      return "ms30";
    case ms35:
      return "ms35";
    case ms40:
      return "ms40";
    case ms45:
      return "ms45";
    case ms50:
      return "ms50";
    case ms55:
      return "ms55";
    case ms60:
      return "ms60";
    case ms65:
      return "ms65";
    case ms70:
      return "ms70";
    case ms75:
      return "ms75";
    case ms80:
      return "ms80";
    case ms85:
      return "ms85";
    case ms90:
      return "ms90";
    case ms95:
      return "ms95";
    case ms100:
      return "ms100";
    case ms105:
      return "ms105";
    case ms110:
      return "ms110";
    case ms115:
      return "ms115";
    case ms120:
      return "ms120";
    case ms125:
      return "ms125";
    case ms130:
      return "ms130";
    case ms135:
      return "ms135";
    case ms140:
      return "ms140";
    case ms145:
      return "ms145";
    case ms150:
      return "ms150";
    case ms155:
      return "ms155";
    case ms160:
      return "ms160";
    case ms165:
      return "ms165";
    case ms170:
      return "ms170";
    case ms175:
      return "ms175";
    case ms180:
      return "ms180";
    case ms185:
      return "ms185";
    case ms190:
      return "ms190";
    case ms195:
      return "ms195";
    case ms200:
      return "ms200";
    case ms205:
      return "ms205";
    case ms210:
      return "ms210";
    case ms215:
      return "ms215";
    case ms220:
      return "ms220";
    case ms225:
      return "ms225";
    case ms230:
      return "ms230";
    case ms235:
      return "ms235";
    case ms240:
      return "ms240";
    case ms245:
      return "ms245";
    case ms250:
      return "ms250";
    case ms300:
      return "ms300";
    case ms350:
      return "ms350";
    case ms400:
      return "ms400";
    case ms450:
      return "ms450";
    case ms500:
      return "ms500";
    case ms800_v1310:
      return "ms800-v1310";
    case ms1000_v1310:
      return "ms1000-v1310";
    case ms2000_v1310:
      return "ms2000-v1310";
    case ms4000_v1310:
      return "ms4000-v1310";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "t_poll_retx_e");
  }
  return "";
}
uint16_t t_poll_retx_e::to_number() const
{
  const static uint16_t options[] = {5,   10,  15,  20,  25,  30,  35,  40,  45,  50,  55,  60,   65,   70,  75,
                                     80,  85,  90,  95,  100, 105, 110, 115, 120, 125, 130, 135,  140,  145, 150,
                                     155, 160, 165, 170, 175, 180, 185, 190, 195, 200, 205, 210,  215,  220, 225,
                                     230, 235, 240, 245, 250, 300, 350, 400, 450, 500, 800, 1000, 2000, 4000};
  return get_enum_number(options, 59, value, "t_poll_retx_e");
}

// T-Reordering ::= ENUMERATED
std::string t_reordering_e::to_string() const
{
  switch (value) {
    case ms0:
      return "ms0";
    case ms5:
      return "ms5";
    case ms10:
      return "ms10";
    case ms15:
      return "ms15";
    case ms20:
      return "ms20";
    case ms25:
      return "ms25";
    case ms30:
      return "ms30";
    case ms35:
      return "ms35";
    case ms40:
      return "ms40";
    case ms45:
      return "ms45";
    case ms50:
      return "ms50";
    case ms55:
      return "ms55";
    case ms60:
      return "ms60";
    case ms65:
      return "ms65";
    case ms70:
      return "ms70";
    case ms75:
      return "ms75";
    case ms80:
      return "ms80";
    case ms85:
      return "ms85";
    case ms90:
      return "ms90";
    case ms95:
      return "ms95";
    case ms100:
      return "ms100";
    case ms110:
      return "ms110";
    case ms120:
      return "ms120";
    case ms130:
      return "ms130";
    case ms140:
      return "ms140";
    case ms150:
      return "ms150";
    case ms160:
      return "ms160";
    case ms170:
      return "ms170";
    case ms180:
      return "ms180";
    case ms190:
      return "ms190";
    case ms200:
      return "ms200";
    case ms1600_v1310:
      return "ms1600-v1310";
    default:
      invalid_enum_value(value, "t_reordering_e");
  }
  return "";
}
uint16_t t_reordering_e::to_number() const
{
  const static uint16_t options[] = {0,  5,  10, 15, 20,  25,  30,  35,  40,  45,  50,  55,  60,  65,  70,  75,
                                     80, 85, 90, 95, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200, 1600};
  return get_enum_number(options, 32, value, "t_reordering_e");
}

// T-StatusProhibit ::= ENUMERATED
std::string t_status_prohibit_e::to_string() const
{
  switch (value) {
    case ms0:
      return "ms0";
    case ms5:
      return "ms5";
    case ms10:
      return "ms10";
    case ms15:
      return "ms15";
    case ms20:
      return "ms20";
    case ms25:
      return "ms25";
    case ms30:
      return "ms30";
    case ms35:
      return "ms35";
    case ms40:
      return "ms40";
    case ms45:
      return "ms45";
    case ms50:
      return "ms50";
    case ms55:
      return "ms55";
    case ms60:
      return "ms60";
    case ms65:
      return "ms65";
    case ms70:
      return "ms70";
    case ms75:
      return "ms75";
    case ms80:
      return "ms80";
    case ms85:
      return "ms85";
    case ms90:
      return "ms90";
    case ms95:
      return "ms95";
    case ms100:
      return "ms100";
    case ms105:
      return "ms105";
    case ms110:
      return "ms110";
    case ms115:
      return "ms115";
    case ms120:
      return "ms120";
    case ms125:
      return "ms125";
    case ms130:
      return "ms130";
    case ms135:
      return "ms135";
    case ms140:
      return "ms140";
    case ms145:
      return "ms145";
    case ms150:
      return "ms150";
    case ms155:
      return "ms155";
    case ms160:
      return "ms160";
    case ms165:
      return "ms165";
    case ms170:
      return "ms170";
    case ms175:
      return "ms175";
    case ms180:
      return "ms180";
    case ms185:
      return "ms185";
    case ms190:
      return "ms190";
    case ms195:
      return "ms195";
    case ms200:
      return "ms200";
    case ms205:
      return "ms205";
    case ms210:
      return "ms210";
    case ms215:
      return "ms215";
    case ms220:
      return "ms220";
    case ms225:
      return "ms225";
    case ms230:
      return "ms230";
    case ms235:
      return "ms235";
    case ms240:
      return "ms240";
    case ms245:
      return "ms245";
    case ms250:
      return "ms250";
    case ms300:
      return "ms300";
    case ms350:
      return "ms350";
    case ms400:
      return "ms400";
    case ms450:
      return "ms450";
    case ms500:
      return "ms500";
    case ms800_v1310:
      return "ms800-v1310";
    case ms1000_v1310:
      return "ms1000-v1310";
    case ms1200_v1310:
      return "ms1200-v1310";
    case ms1600_v1310:
      return "ms1600-v1310";
    case ms2000_v1310:
      return "ms2000-v1310";
    case ms2400_v1310:
      return "ms2400-v1310";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "t_status_prohibit_e");
  }
  return "";
}
uint16_t t_status_prohibit_e::to_number() const
{
  const static uint16_t options[] = {0,   5,   10,  15,  20,  25,  30,  35,  40,  45,   50,   55,   60,   65,  70,  75,
                                     80,  85,  90,  95,  100, 105, 110, 115, 120, 125,  130,  135,  140,  145, 150, 155,
                                     160, 165, 170, 175, 180, 185, 190, 195, 200, 205,  210,  215,  220,  225, 230, 235,
                                     240, 245, 250, 300, 350, 400, 450, 500, 800, 1000, 1200, 1600, 2000, 2400};
  return get_enum_number(options, 62, value, "t_status_prohibit_e");
}

std::string
cqi_report_aperiodic_v1250_c::setup_s_::aperiodic_csi_trigger_v1250_s_::trigger_sf_set_ind_r12_e_::to_string() const
{
  switch (value) {
    case s1:
      return "s1";
    case s2:
      return "s2";
    default:
      invalid_enum_value(
          value, "cqi_report_aperiodic_v1250_c::setup_s_::aperiodic_csi_trigger_v1250_s_::trigger_sf_set_ind_r12_e_");
  }
  return "";
}
uint8_t
cqi_report_aperiodic_v1250_c::setup_s_::aperiodic_csi_trigger_v1250_s_::trigger_sf_set_ind_r12_e_::to_number() const
{
  const static uint8_t options[] = {1, 2};
  return get_enum_number(
      options, 2, value,
      "cqi_report_aperiodic_v1250_c::setup_s_::aperiodic_csi_trigger_v1250_s_::trigger_sf_set_ind_r12_e_");
}

std::string cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_::types::to_string() const
{
  switch (value) {
    case one_bit_r14:
      return "oneBit-r14";
    case two_bit_r14:
      return "twoBit-r14";
    case three_bit_r14:
      return "threeBit-r14";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_::types");
  }
  return "";
}
uint8_t cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_::types::to_number() const
{
  const static uint8_t options[] = {1, 2, 3};
  return get_enum_number(options, 3, value, "cqi_report_aperiodic_hybrid_r14_s::triggers_r14_c_::types");
}

std::string cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::wideband_cqi_r10_s_::
    csi_report_mode_r10_e_::to_string() const
{
  switch (value) {
    case submode1:
      return "submode1";
    case submode2:
      return "submode2";
    default:
      invalid_enum_value(value, "cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::wideband_cqi_r10_"
                                "s_::csi_report_mode_r10_e_");
  }
  return "";
}
uint8_t cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::wideband_cqi_r10_s_::
    csi_report_mode_r10_e_::to_number() const
{
  const static uint8_t options[] = {1, 2};
  return get_enum_number(options, 2, value,
                         "cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::wideband_cqi_r10_s_::"
                         "csi_report_mode_r10_e_");
}

std::string cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::subband_cqi_r10_s_::
    periodicity_factor_r10_e_::to_string() const
{
  switch (value) {
    case n2:
      return "n2";
    case n4:
      return "n4";
    default:
      invalid_enum_value(value, "cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::subband_cqi_r10_"
                                "s_::periodicity_factor_r10_e_");
  }
  return "";
}
uint8_t cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::subband_cqi_r10_s_::
    periodicity_factor_r10_e_::to_number() const
{
  const static uint8_t options[] = {2, 4};
  return get_enum_number(options, 2, value,
                         "cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::subband_cqi_r10_s_::"
                         "periodicity_factor_r10_e_");
}

std::string cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::types::to_string() const
{
  switch (value) {
    case wideband_cqi_r10:
      return "widebandCQI-r10";
    case subband_cqi_r10:
      return "subbandCQI-r10";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "cqi_report_periodic_r10_c::setup_s_::cqi_format_ind_periodic_r10_c_::types");
  }
  return "";
}

std::string cqi_report_periodic_v1320_s::periodicity_factor_wb_r13_e_::to_string() const
{
  switch (value) {
    case n2:
      return "n2";
    case n4:
      return "n4";
    default:
      invalid_enum_value(value, "cqi_report_periodic_v1320_s::periodicity_factor_wb_r13_e_");
  }
  return "";
}
uint8_t cqi_report_periodic_v1320_s::periodicity_factor_wb_r13_e_::to_number() const
{
  const static uint8_t options[] = {2, 4};
  return get_enum_number(options, 2, value, "cqi_report_periodic_v1320_s::periodicity_factor_wb_r13_e_");
}

std::string meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_::types::to_string() const
{
  switch (value) {
    case sf_cfg1_minus5_r10:
      return "subframeConfig1-5-r10";
    case sf_cfg0_r10:
      return "subframeConfig0-r10";
    case sf_cfg6_r10:
      return "subframeConfig6-r10";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_::types");
  }
  return "";
}
uint8_t meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_::types::to_number() const
{
  const static uint8_t options[] = {1, 0, 6};
  return get_enum_number(options, 3, value, "meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_::types");
}

std::string meas_sf_pattern_r10_c::types::to_string() const
{
  switch (value) {
    case sf_pattern_fdd_r10:
      return "subframePatternFDD-r10";
    case sf_pattern_tdd_r10:
      return "subframePatternTDD-r10";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "meas_sf_pattern_r10_c::types");
  }
  return "";
}

// PollByte ::= ENUMERATED
std::string poll_byte_e::to_string() const
{
  switch (value) {
    case kb25:
      return "kB25";
    case kb50:
      return "kB50";
    case kb75:
      return "kB75";
    case kb100:
      return "kB100";
    case kb125:
      return "kB125";
    case kb250:
      return "kB250";
    case kb375:
      return "kB375";
    case kb500:
      return "kB500";
    case kb750:
      return "kB750";
    case kb1000:
      return "kB1000";
    case kb1250:
      return "kB1250";
    case kb1500:
      return "kB1500";
    case kb2000:
      return "kB2000";
    case kb3000:
      return "kB3000";
    case kbinfinity:
      return "kBinfinity";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "poll_byte_e");
  }
  return "";
}
int16_t poll_byte_e::to_number() const
{
  const static int16_t options[] = {25, 50, 75, 100, 125, 250, 375, 500, 750, 1000, 1250, 1500, 2000, 3000, -1};
  return get_enum_number(options, 15, value, "poll_byte_e");
}

// PollPDU ::= ENUMERATED
std::string poll_pdu_e::to_string() const
{
  switch (value) {
    case p4:
      return "p4";
    case p8:
      return "p8";
    case p16:
      return "p16";
    case p32:
      return "p32";
    case p64:
      return "p64";
    case p128:
      return "p128";
    case p256:
      return "p256";
    case p_infinity:
      return "pInfinity";
    default:
      invalid_enum_value(value, "poll_pdu_e");
  }
  return "";
}
int16_t poll_pdu_e::to_number() const
{
  const static int16_t options[] = {4, 8, 16, 32, 64, 128, 256, -1};
  return get_enum_number(options, 8, value, "poll_pdu_e");
}

std::string spdcch_elems_r15_c::setup_s_::spdcch_set_ref_sig_r15_e_::to_string() const
{
  switch (value) {
    case crs:
      return "crs";
    case dmrs:
      return "dmrs";
    default:
      invalid_enum_value(value, "spdcch_elems_r15_c::setup_s_::spdcch_set_ref_sig_r15_e_");
  }
  return "";
}

std::string spdcch_elems_r15_c::setup_s_::tx_type_r15_e_::to_string() const
{
  switch (value) {
    case localised:
      return "localised";
    case distributed:
      return "distributed";
    default:
      invalid_enum_value(value, "spdcch_elems_r15_c::setup_s_::tx_type_r15_e_");
  }
  return "";
}

std::string spdcch_elems_r15_c::setup_s_::sf_type_r15_e_::to_string() const
{
  switch (value) {
    case mbsfn:
      return "mbsfn";
    case nonmbsfn:
      return "nonmbsfn";
    case all:
      return "all";
    default:
      invalid_enum_value(value, "spdcch_elems_r15_c::setup_s_::sf_type_r15_e_");
  }
  return "";
}

std::string spdcch_elems_r15_c::setup_s_::rate_matching_mode_r15_e_::to_string() const
{
  switch (value) {
    case m1:
      return "m1";
    case m2:
      return "m2";
    case m3:
      return "m3";
    case m4:
      return "m4";
    default:
      invalid_enum_value(value, "spdcch_elems_r15_c::setup_s_::rate_matching_mode_r15_e_");
  }
  return "";
}
uint8_t spdcch_elems_r15_c::setup_s_::rate_matching_mode_r15_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 4};
  return get_enum_number(options, 4, value, "spdcch_elems_r15_c::setup_s_::rate_matching_mode_r15_e_");
}

std::string tpc_idx_c::types::to_string() const
{
  switch (value) {
    case idx_of_format3:
      return "indexOfFormat3";
    case idx_of_format3_a:
      return "indexOfFormat3A";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "tpc_idx_c::types");
  }
  return "";
}

std::string ul_am_rlc_r15_s::max_retx_thres_r15_e_::to_string() const
{
  switch (value) {
    case t1:
      return "t1";
    case t2:
      return "t2";
    case t3:
      return "t3";
    case t4:
      return "t4";
    case t6:
      return "t6";
    case t8:
      return "t8";
    case t16:
      return "t16";
    case t32:
      return "t32";
    default:
      invalid_enum_value(value, "ul_am_rlc_r15_s::max_retx_thres_r15_e_");
  }
  return "";
}
uint8_t ul_am_rlc_r15_s::max_retx_thres_r15_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 4, 6, 8, 16, 32};
  return get_enum_number(options, 8, value, "ul_am_rlc_r15_s::max_retx_thres_r15_e_");
}

std::string cqi_report_cfg_v1250_s::alt_cqi_table_r12_e_::to_string() const
{
  switch (value) {
    case all_sfs:
      return "allSubframes";
    case csi_sf_set1:
      return "csi-SubframeSet1";
    case csi_sf_set2:
      return "csi-SubframeSet2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "cqi_report_cfg_v1250_s::alt_cqi_table_r12_e_");
  }
  return "";
}
uint8_t cqi_report_cfg_v1250_s::alt_cqi_table_r12_e_::to_number() const
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

std::string csi_rs_cfg_r10_s::csi_rs_r10_c_::setup_s_::ant_ports_count_r10_e_::to_string() const
{
  switch (value) {
    case an1:
      return "an1";
    case an2:
      return "an2";
    case an4:
      return "an4";
    case an8:
      return "an8";
    default:
      invalid_enum_value(value, "csi_rs_cfg_r10_s::csi_rs_r10_c_::setup_s_::ant_ports_count_r10_e_");
  }
  return "";
}
uint8_t csi_rs_cfg_r10_s::csi_rs_r10_c_::setup_s_::ant_ports_count_r10_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 4, 8};
  return get_enum_number(options, 4, value, "csi_rs_cfg_r10_s::csi_rs_r10_c_::setup_s_::ant_ports_count_r10_e_");
}

std::string delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1_r15_e_::to_string() const
{
  switch (value) {
    case db0:
      return "dB0";
    case db_minus2:
      return "dB-2";
    default:
      invalid_enum_value(value, "delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1_r15_e_");
  }
  return "";
}
int8_t delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1_r15_e_::to_number() const
{
  const static int8_t options[] = {0, -2};
  return get_enum_number(options, 2, value,
                         "delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1_r15_e_");
}

std::string delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1a_r15_e_::to_string() const
{
  switch (value) {
    case db0:
      return "dB0";
    case db_minus2:
      return "dB-2";
    default:
      invalid_enum_value(value, "delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1a_r15_e_");
  }
  return "";
}
int8_t delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1a_r15_e_::to_number() const
{
  const static int8_t options[] = {0, -2};
  return get_enum_number(options, 2, value,
                         "delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1a_r15_e_");
}

std::string delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1b_r15_e_::to_string() const
{
  switch (value) {
    case db0:
      return "dB0";
    case db_minus2:
      return "dB-2";
    default:
      invalid_enum_value(value, "delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1b_r15_e_");
  }
  return "";
}
int8_t delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1b_r15_e_::to_number() const
{
  const static int8_t options[] = {0, -2};
  return get_enum_number(options, 2, value,
                         "delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format1b_r15_e_");
}

std::string delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format3_r15_e_::to_string() const
{
  switch (value) {
    case db0:
      return "dB0";
    case db_minus2:
      return "dB-2";
    default:
      invalid_enum_value(value, "delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format3_r15_e_");
  }
  return "";
}
int8_t delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format3_r15_e_::to_number() const
{
  const static int8_t options[] = {0, -2};
  return get_enum_number(options, 2, value,
                         "delta_tx_d_offset_list_spucch_r15_s::delta_tx_d_offset_spucch_format3_r15_e_");
}

std::string epdcch_set_cfg_r11_s::tx_type_r11_e_::to_string() const
{
  switch (value) {
    case localised:
      return "localised";
    case distributed:
      return "distributed";
    default:
      invalid_enum_value(value, "epdcch_set_cfg_r11_s::tx_type_r11_e_");
  }
  return "";
}

std::string epdcch_set_cfg_r11_s::res_block_assign_r11_s_::num_prb_pairs_r11_e_::to_string() const
{
  switch (value) {
    case n2:
      return "n2";
    case n4:
      return "n4";
    case n8:
      return "n8";
    default:
      invalid_enum_value(value, "epdcch_set_cfg_r11_s::res_block_assign_r11_s_::num_prb_pairs_r11_e_");
  }
  return "";
}
uint8_t epdcch_set_cfg_r11_s::res_block_assign_r11_s_::num_prb_pairs_r11_e_::to_number() const
{
  const static uint8_t options[] = {2, 4, 8};
  return get_enum_number(options, 3, value, "epdcch_set_cfg_r11_s::res_block_assign_r11_s_::num_prb_pairs_r11_e_");
}

std::string epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::csi_num_repeat_ce_r13_e_::to_string() const
{
  switch (value) {
    case sf1:
      return "sf1";
    case sf2:
      return "sf2";
    case sf4:
      return "sf4";
    case sf8:
      return "sf8";
    case sf16:
      return "sf16";
    case sf32:
      return "sf32";
    default:
      invalid_enum_value(value, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::csi_num_repeat_ce_r13_e_");
  }
  return "";
}
uint8_t epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::csi_num_repeat_ce_r13_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 4, 8, 16, 32};
  return get_enum_number(options, 6, value,
                         "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::csi_num_repeat_ce_r13_e_");
}

std::string epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_pdsch_hop_cfg_r13_e_::to_string() const
{
  switch (value) {
    case on:
      return "on";
    case off:
      return "off";
    default:
      invalid_enum_value(value, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_pdsch_hop_cfg_r13_e_");
  }
  return "";
}

std::string
epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::fdd_r13_e_::to_string() const
{
  switch (value) {
    case v1:
      return "v1";
    case v1dot5:
      return "v1dot5";
    case v2:
      return "v2";
    case v2dot5:
      return "v2dot5";
    case v4:
      return "v4";
    case v5:
      return "v5";
    case v8:
      return "v8";
    case v10:
      return "v10";
    default:
      invalid_enum_value(value,
                         "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::fdd_r13_e_");
  }
  return "";
}
float epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::fdd_r13_e_::to_number() const
{
  const static float options[] = {1.0, 1.5, 2.0, 2.5, 4.0, 5.0, 8.0, 10.0};
  return get_enum_number(options, 8, value,
                         "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::fdd_r13_e_");
}
std::string
epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::fdd_r13_e_::to_number_string() const
{
  switch (value) {
    case v1:
      return "1";
    case v1dot5:
      return "1.5";
    case v2:
      return "2";
    case v2dot5:
      return "2.5";
    case v4:
      return "4";
    case v5:
      return "5";
    case v8:
      return "8";
    case v10:
      return "10";
    default:
      invalid_enum_number(value,
                          "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::fdd_r13_e_");
  }
  return "";
}

std::string
epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::tdd_r13_e_::to_string() const
{
  switch (value) {
    case v1:
      return "v1";
    case v2:
      return "v2";
    case v4:
      return "v4";
    case v5:
      return "v5";
    case v8:
      return "v8";
    case v10:
      return "v10";
    case v20:
      return "v20";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value,
                         "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::tdd_r13_e_");
  }
  return "";
}
uint8_t epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::tdd_r13_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 4, 5, 8, 10, 20};
  return get_enum_number(options, 7, value,
                         "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::tdd_r13_e_");
}

std::string epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::types::to_string() const
{
  switch (value) {
    case fdd_r13:
      return "fdd-r13";
    case tdd_r13:
      return "tdd-r13";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value,
                         "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_start_sf_uess_r13_c_::types");
  }
  return "";
}

std::string epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_num_repeat_r13_e_::to_string() const
{
  switch (value) {
    case r1:
      return "r1";
    case r2:
      return "r2";
    case r4:
      return "r4";
    case r8:
      return "r8";
    case r16:
      return "r16";
    case r32:
      return "r32";
    case r64:
      return "r64";
    case r128:
      return "r128";
    case r256:
      return "r256";
    default:
      invalid_enum_value(value, "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_num_repeat_r13_e_");
  }
  return "";
}
uint16_t epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_num_repeat_r13_e_::to_number() const
{
  const static uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
  return get_enum_number(options, 9, value,
                         "epdcch_set_cfg_r11_s::mpdcch_cfg_r13_c_::setup_s_::mpdcch_num_repeat_r13_e_");
}

std::string enable256_qam_r14_c::setup_c_::types::to_string() const
{
  switch (value) {
    case tpc_sf_set_cfgured_r14:
      return "tpc-SubframeSet-Configured-r14";
    case tpc_sf_set_not_cfgured_r14:
      return "tpc-SubframeSet-NotConfigured-r14";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "enable256_qam_r14_c::setup_c_::types");
  }
  return "";
}

std::string lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_e_::to_string() const
{
  switch (value) {
    case kbps0:
      return "kBps0";
    case kbps8:
      return "kBps8";
    case kbps16:
      return "kBps16";
    case kbps32:
      return "kBps32";
    case kbps64:
      return "kBps64";
    case kbps128:
      return "kBps128";
    case kbps256:
      return "kBps256";
    case infinity:
      return "infinity";
    case kbps512_v1020:
      return "kBps512-v1020";
    case kbps1024_v1020:
      return "kBps1024-v1020";
    case kbps2048_v1020:
      return "kBps2048-v1020";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_e_");
  }
  return "";
}
int16_t lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_e_::to_number() const
{
  const static int16_t options[] = {0, 8, 16, 32, 64, 128, 256, -1, 512, 1024, 2048};
  return get_enum_number(options, 11, value, "lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_e_");
}

std::string lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_e_::to_string() const
{
  switch (value) {
    case ms50:
      return "ms50";
    case ms100:
      return "ms100";
    case ms150:
      return "ms150";
    case ms300:
      return "ms300";
    case ms500:
      return "ms500";
    case ms1000:
      return "ms1000";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_e_");
  }
  return "";
}
uint16_t lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_e_::to_number() const
{
  const static uint16_t options[] = {50, 100, 150, 300, 500, 1000};
  return get_enum_number(options, 6, value, "lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_e_");
}

std::string lc_ch_cfg_s::bit_rate_query_prohibit_timer_r14_e_::to_string() const
{
  switch (value) {
    case s0:
      return "s0";
    case s0dot4:
      return "s0dot4";
    case s0dot8:
      return "s0dot8";
    case s1dot6:
      return "s1dot6";
    case s3:
      return "s3";
    case s6:
      return "s6";
    case s12:
      return "s12";
    case s30:
      return "s30";
    default:
      invalid_enum_value(value, "lc_ch_cfg_s::bit_rate_query_prohibit_timer_r14_e_");
  }
  return "";
}
float lc_ch_cfg_s::bit_rate_query_prohibit_timer_r14_e_::to_number() const
{
  const static float options[] = {0.0, 0.4, 0.8, 1.6, 3.0, 6.0, 12.0, 30.0};
  return get_enum_number(options, 8, value, "lc_ch_cfg_s::bit_rate_query_prohibit_timer_r14_e_");
}
std::string lc_ch_cfg_s::bit_rate_query_prohibit_timer_r14_e_::to_number_string() const
{
  switch (value) {
    case s0:
      return "0";
    case s0dot4:
      return "0.4";
    case s0dot8:
      return "0.8";
    case s1dot6:
      return "1.6";
    case s3:
      return "3";
    case s6:
      return "6";
    case s12:
      return "12";
    case s30:
      return "30";
    default:
      invalid_enum_number(value, "lc_ch_cfg_s::bit_rate_query_prohibit_timer_r14_e_");
  }
  return "";
}

std::string lc_ch_cfg_s::lc_ch_sr_restrict_r15_c_::setup_e_::to_string() const
{
  switch (value) {
    case spucch:
      return "spucch";
    case pucch:
      return "pucch";
    default:
      invalid_enum_value(value, "lc_ch_cfg_s::lc_ch_sr_restrict_r15_c_::setup_e_");
  }
  return "";
}

// P-a ::= ENUMERATED
std::string p_a_e::to_string() const
{
  switch (value) {
    case db_minus6:
      return "dB-6";
    case db_minus4dot77:
      return "dB-4dot77";
    case db_minus3:
      return "dB-3";
    case db_minus1dot77:
      return "dB-1dot77";
    case db0:
      return "dB0";
    case db1:
      return "dB1";
    case db2:
      return "dB2";
    case db3:
      return "dB3";
    default:
      invalid_enum_value(value, "p_a_e");
  }
  return "";
}
float p_a_e::to_number() const
{
  const static float options[] = {-6.0, -4.77, -3.0, -1.77, 0.0, 1.0, 2.0, 3.0};
  return get_enum_number(options, 8, value, "p_a_e");
}
std::string p_a_e::to_number_string() const
{
  switch (value) {
    case db_minus6:
      return "-6";
    case db_minus4dot77:
      return "-4.77";
    case db_minus3:
      return "-3";
    case db_minus1dot77:
      return "-1.77";
    case db0:
      return "0";
    case db1:
      return "1";
    case db2:
      return "2";
    case db3:
      return "3";
    default:
      invalid_enum_number(value, "p_a_e");
  }
  return "";
}

std::string pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::crs_ports_count_r11_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n2:
      return "n2";
    case n4:
      return "n4";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::crs_ports_count_r11_e_");
  }
  return "";
}
uint8_t pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::crs_ports_count_r11_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 4};
  return get_enum_number(options, 3, value,
                         "pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::crs_ports_count_r11_e_");
}

std::string pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::pdsch_start_r11_e_::to_string() const
{
  switch (value) {
    case reserved:
      return "reserved";
    case n1:
      return "n1";
    case n2:
      return "n2";
    case n3:
      return "n3";
    case n4:
      return "n4";
    case assigned:
      return "assigned";
    default:
      invalid_enum_value(value, "pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::pdsch_start_r11_e_");
  }
  return "";
}
uint8_t pdsch_re_map_qcl_cfg_r11_s::optional_set_of_fields_r11_s_::pdsch_start_r11_e_::to_number() const
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

std::string pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::setup_s_::crs_ports_count_v1530_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n2:
      return "n2";
    case n4:
      return "n4";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value,
                         "pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::setup_s_::crs_ports_count_v1530_e_");
  }
  return "";
}
uint8_t pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::setup_s_::crs_ports_count_v1530_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 4};
  return get_enum_number(options, 3, value,
                         "pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::setup_s_::crs_ports_count_v1530_e_");
}

std::string pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::setup_s_::pdsch_start_v1530_e_::to_string() const
{
  switch (value) {
    case reserved:
      return "reserved";
    case n1:
      return "n1";
    case n2:
      return "n2";
    case n3:
      return "n3";
    case n4:
      return "n4";
    case assigned:
      return "assigned";
    default:
      invalid_enum_value(value,
                         "pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::setup_s_::pdsch_start_v1530_e_");
  }
  return "";
}
uint8_t pdsch_re_map_qcl_cfg_r11_s::codeword_one_cfg_v1530_c_::setup_s_::pdsch_start_v1530_e_::to_number() const
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
std::string poll_pdu_v1310_e::to_string() const
{
  switch (value) {
    case p512:
      return "p512";
    case p1024:
      return "p1024";
    case p2048:
      return "p2048";
    case p4096:
      return "p4096";
    case p6144:
      return "p6144";
    case p8192:
      return "p8192";
    case p12288:
      return "p12288";
    case p16384:
      return "p16384";
    default:
      invalid_enum_value(value, "poll_pdu_v1310_e");
  }
  return "";
}
uint16_t poll_pdu_v1310_e::to_number() const
{
  const static uint16_t options[] = {512, 1024, 2048, 4096, 6144, 8192, 12288, 16384};
  return get_enum_number(options, 8, value, "poll_pdu_v1310_e");
}

std::string rlc_cfg_r15_s::mode_r15_c_::types::to_string() const
{
  switch (value) {
    case am_r15:
      return "am-r15";
    case um_bi_dir_r15:
      return "um-Bi-Directional-r15";
    case um_uni_dir_ul_r15:
      return "um-Uni-Directional-UL-r15";
    case um_uni_dir_dl_r15:
      return "um-Uni-Directional-DL-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rlc_cfg_r15_s::mode_r15_c_::types");
  }
  return "";
}

// SRS-AntennaPort ::= ENUMERATED
std::string srs_ant_port_e::to_string() const
{
  switch (value) {
    case an1:
      return "an1";
    case an2:
      return "an2";
    case an4:
      return "an4";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "srs_ant_port_e");
  }
  return "";
}
uint8_t srs_ant_port_e::to_number() const
{
  const static uint8_t options[] = {1, 2, 4};
  return get_enum_number(options, 3, value, "srs_ant_port_e");
}

// ShortTTI-Length-r15 ::= ENUMERATED
std::string short_tti_len_r15_e::to_string() const
{
  switch (value) {
    case slot:
      return "slot";
    case subslot:
      return "subslot";
    default:
      invalid_enum_value(value, "short_tti_len_r15_e");
  }
  return "";
}

std::string ul_am_rlc_s::max_retx_thres_e_::to_string() const
{
  switch (value) {
    case t1:
      return "t1";
    case t2:
      return "t2";
    case t3:
      return "t3";
    case t4:
      return "t4";
    case t6:
      return "t6";
    case t8:
      return "t8";
    case t16:
      return "t16";
    case t32:
      return "t32";
    default:
      invalid_enum_value(value, "ul_am_rlc_s::max_retx_thres_e_");
  }
  return "";
}
uint8_t ul_am_rlc_s::max_retx_thres_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 4, 6, 8, 16, 32};
  return get_enum_number(options, 8, value, "ul_am_rlc_s::max_retx_thres_e_");
}

std::string ant_info_ded_stti_r15_c::setup_s_::tx_mode_dl_mbsfn_r15_e_::to_string() const
{
  switch (value) {
    case tm9:
      return "tm9";
    case tm10:
      return "tm10";
    default:
      invalid_enum_value(value, "ant_info_ded_stti_r15_c::setup_s_::tx_mode_dl_mbsfn_r15_e_");
  }
  return "";
}
uint8_t ant_info_ded_stti_r15_c::setup_s_::tx_mode_dl_mbsfn_r15_e_::to_number() const
{
  const static uint8_t options[] = {9, 10};
  return get_enum_number(options, 2, value, "ant_info_ded_stti_r15_c::setup_s_::tx_mode_dl_mbsfn_r15_e_");
}

std::string ant_info_ded_stti_r15_c::setup_s_::tx_mode_dl_non_mbsfn_r15_e_::to_string() const
{
  switch (value) {
    case tm1:
      return "tm1";
    case tm2:
      return "tm2";
    case tm3:
      return "tm3";
    case tm4:
      return "tm4";
    case tm6:
      return "tm6";
    case tm8:
      return "tm8";
    case tm9:
      return "tm9";
    case tm10:
      return "tm10";
    default:
      invalid_enum_value(value, "ant_info_ded_stti_r15_c::setup_s_::tx_mode_dl_non_mbsfn_r15_e_");
  }
  return "";
}
uint8_t ant_info_ded_stti_r15_c::setup_s_::tx_mode_dl_non_mbsfn_r15_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 4, 6, 8, 9, 10};
  return get_enum_number(options, 8, value, "ant_info_ded_stti_r15_c::setup_s_::tx_mode_dl_non_mbsfn_r15_e_");
}

std::string ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_::types::to_string() const
{
  switch (value) {
    case n2_tx_ant_tm3_r15:
      return "n2TxAntenna-tm3-r15";
    case n4_tx_ant_tm3_r15:
      return "n4TxAntenna-tm3-r15";
    case n2_tx_ant_tm4_r15:
      return "n2TxAntenna-tm4-r15";
    case n4_tx_ant_tm4_r15:
      return "n4TxAntenna-tm4-r15";
    case n2_tx_ant_tm5_r15:
      return "n2TxAntenna-tm5-r15";
    case n4_tx_ant_tm5_r15:
      return "n4TxAntenna-tm5-r15";
    case n2_tx_ant_tm6_r15:
      return "n2TxAntenna-tm6-r15";
    case n4_tx_ant_tm6_r15:
      return "n4TxAntenna-tm6-r15";
    case n2_tx_ant_tm8_r15:
      return "n2TxAntenna-tm8-r15";
    case n4_tx_ant_tm8_r15:
      return "n4TxAntenna-tm8-r15";
    case n2_tx_ant_tm9and10_r15:
      return "n2TxAntenna-tm9and10-r15";
    case n4_tx_ant_tm9and10_r15:
      return "n4TxAntenna-tm9and10-r15";
    case n8_tx_ant_tm9and10_r15:
      return "n8TxAntenna-tm9and10-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ant_info_ded_stti_r15_c::setup_s_::codebook_subset_restrict_c_::types");
  }
  return "";
}

std::string ant_info_ded_stti_r15_c::setup_s_::max_layers_mimo_stti_r15_e_::to_string() const
{
  switch (value) {
    case two_layers:
      return "twoLayers";
    case four_layers:
      return "fourLayers";
    default:
      invalid_enum_value(value, "ant_info_ded_stti_r15_c::setup_s_::max_layers_mimo_stti_r15_e_");
  }
  return "";
}
uint8_t ant_info_ded_stti_r15_c::setup_s_::max_layers_mimo_stti_r15_e_::to_number() const
{
  const static uint8_t options[] = {2, 4};
  return get_enum_number(options, 2, value, "ant_info_ded_stti_r15_c::setup_s_::max_layers_mimo_stti_r15_e_");
}

std::string ant_info_ul_stti_r15_s::tx_mode_ul_stti_r15_e_::to_string() const
{
  switch (value) {
    case tm1:
      return "tm1";
    case tm2:
      return "tm2";
    default:
      invalid_enum_value(value, "ant_info_ul_stti_r15_s::tx_mode_ul_stti_r15_e_");
  }
  return "";
}
uint8_t ant_info_ul_stti_r15_s::tx_mode_ul_stti_r15_e_::to_number() const
{
  const static uint8_t options[] = {1, 2};
  return get_enum_number(options, 2, value, "ant_info_ul_stti_r15_s::tx_mode_ul_stti_r15_e_");
}

std::string cqi_report_cfg_r15_c::setup_s_::alt_cqi_table_minus1024_qam_r15_e_::to_string() const
{
  switch (value) {
    case all_sfs:
      return "allSubframes";
    case csi_sf_set1:
      return "csi-SubframeSet1";
    case csi_sf_set2:
      return "csi-SubframeSet2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "cqi_report_cfg_r15_c::setup_s_::alt_cqi_table_minus1024_qam_r15_e_");
  }
  return "";
}
uint8_t cqi_report_cfg_r15_c::setup_s_::alt_cqi_table_minus1024_qam_r15_e_::to_number() const
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

std::string cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_::types::to_string() const
{
  switch (value) {
    case wideband_cqi:
      return "widebandCQI";
    case subband_cqi:
      return "subbandCQI";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "cqi_report_periodic_c::setup_s_::cqi_format_ind_periodic_c_::types");
  }
  return "";
}

std::string crs_assist_info_r11_s::ant_ports_count_r11_e_::to_string() const
{
  switch (value) {
    case an1:
      return "an1";
    case an2:
      return "an2";
    case an4:
      return "an4";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "crs_assist_info_r11_s::ant_ports_count_r11_e_");
  }
  return "";
}
uint8_t crs_assist_info_r11_s::ant_ports_count_r11_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 4};
  return get_enum_number(options, 3, value, "crs_assist_info_r11_s::ant_ports_count_r11_e_");
}

std::string crs_assist_info_r13_s::ant_ports_count_r13_e_::to_string() const
{
  switch (value) {
    case an1:
      return "an1";
    case an2:
      return "an2";
    case an4:
      return "an4";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "crs_assist_info_r13_s::ant_ports_count_r13_e_");
  }
  return "";
}
uint8_t crs_assist_info_r13_s::ant_ports_count_r13_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 4};
  return get_enum_number(options, 3, value, "crs_assist_info_r13_s::ant_ports_count_r13_e_");
}

std::string delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format1_r10_e_::to_string() const
{
  switch (value) {
    case db0:
      return "dB0";
    case db_minus2:
      return "dB-2";
    default:
      invalid_enum_value(value, "delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format1_r10_e_");
  }
  return "";
}
int8_t delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format1_r10_e_::to_number() const
{
  const static int8_t options[] = {0, -2};
  return get_enum_number(options, 2, value,
                         "delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format1_r10_e_");
}

std::string delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format1a1b_r10_e_::to_string() const
{
  switch (value) {
    case db0:
      return "dB0";
    case db_minus2:
      return "dB-2";
    default:
      invalid_enum_value(value, "delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format1a1b_r10_e_");
  }
  return "";
}
int8_t delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format1a1b_r10_e_::to_number() const
{
  const static int8_t options[] = {0, -2};
  return get_enum_number(options, 2, value,
                         "delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format1a1b_r10_e_");
}

std::string delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format22a2b_r10_e_::to_string() const
{
  switch (value) {
    case db0:
      return "dB0";
    case db_minus2:
      return "dB-2";
    default:
      invalid_enum_value(value, "delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format22a2b_r10_e_");
  }
  return "";
}
int8_t delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format22a2b_r10_e_::to_number() const
{
  const static int8_t options[] = {0, -2};
  return get_enum_number(options, 2, value,
                         "delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format22a2b_r10_e_");
}

std::string delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format3_r10_e_::to_string() const
{
  switch (value) {
    case db0:
      return "dB0";
    case db_minus2:
      return "dB-2";
    default:
      invalid_enum_value(value, "delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format3_r10_e_");
  }
  return "";
}
int8_t delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format3_r10_e_::to_number() const
{
  const static int8_t options[] = {0, -2};
  return get_enum_number(options, 2, value,
                         "delta_tx_d_offset_list_pucch_r10_s::delta_tx_d_offset_pucch_format3_r10_e_");
}

std::string delta_tx_d_offset_list_pucch_v1130_s::delta_tx_d_offset_pucch_format1b_cs_r11_e_::to_string() const
{
  switch (value) {
    case db0:
      return "dB0";
    case db_minus1:
      return "dB-1";
    default:
      invalid_enum_value(value, "delta_tx_d_offset_list_pucch_v1130_s::delta_tx_d_offset_pucch_format1b_cs_r11_e_");
  }
  return "";
}
int8_t delta_tx_d_offset_list_pucch_v1130_s::delta_tx_d_offset_pucch_format1b_cs_r11_e_::to_number() const
{
  const static int8_t options[] = {0, -1};
  return get_enum_number(options, 2, value,
                         "delta_tx_d_offset_list_pucch_v1130_s::delta_tx_d_offset_pucch_format1b_cs_r11_e_");
}

std::string eimta_main_cfg_r12_c::setup_s_::eimta_cmd_periodicity_r12_e_::to_string() const
{
  switch (value) {
    case sf10:
      return "sf10";
    case sf20:
      return "sf20";
    case sf40:
      return "sf40";
    case sf80:
      return "sf80";
    default:
      invalid_enum_value(value, "eimta_main_cfg_r12_c::setup_s_::eimta_cmd_periodicity_r12_e_");
  }
  return "";
}
uint8_t eimta_main_cfg_r12_c::setup_s_::eimta_cmd_periodicity_r12_e_::to_number() const
{
  const static uint8_t options[] = {10, 20, 40, 80};
  return get_enum_number(options, 4, value, "eimta_main_cfg_r12_c::setup_s_::eimta_cmd_periodicity_r12_e_");
}

std::string eimta_main_cfg_serv_cell_r12_c::setup_s_::eimta_harq_ref_cfg_r12_e_::to_string() const
{
  switch (value) {
    case sa2:
      return "sa2";
    case sa4:
      return "sa4";
    case sa5:
      return "sa5";
    default:
      invalid_enum_value(value, "eimta_main_cfg_serv_cell_r12_c::setup_s_::eimta_harq_ref_cfg_r12_e_");
  }
  return "";
}
uint8_t eimta_main_cfg_serv_cell_r12_c::setup_s_::eimta_harq_ref_cfg_r12_e_::to_number() const
{
  const static uint8_t options[] = {2, 4, 5};
  return get_enum_number(options, 3, value, "eimta_main_cfg_serv_cell_r12_c::setup_s_::eimta_harq_ref_cfg_r12_e_");
}

std::string neigh_cells_info_r12_s::crs_ports_count_r12_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n2:
      return "n2";
    case n4:
      return "n4";
    case spare:
      return "spare";
    default:
      invalid_enum_value(value, "neigh_cells_info_r12_s::crs_ports_count_r12_e_");
  }
  return "";
}
uint8_t neigh_cells_info_r12_s::crs_ports_count_r12_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 4};
  return get_enum_number(options, 3, value, "neigh_cells_info_r12_s::crs_ports_count_r12_e_");
}

// PDCCH-CandidateReductionValue-r13 ::= ENUMERATED
std::string pdcch_candidate_reduction_value_r13_e::to_string() const
{
  switch (value) {
    case n0:
      return "n0";
    case n33:
      return "n33";
    case n66:
      return "n66";
    case n100:
      return "n100";
    default:
      invalid_enum_value(value, "pdcch_candidate_reduction_value_r13_e");
  }
  return "";
}
uint8_t pdcch_candidate_reduction_value_r13_e::to_number() const
{
  const static uint8_t options[] = {0, 33, 66, 100};
  return get_enum_number(options, 4, value, "pdcch_candidate_reduction_value_r13_e");
}

std::string pdcp_cfg_s::discard_timer_e_::to_string() const
{
  switch (value) {
    case ms50:
      return "ms50";
    case ms100:
      return "ms100";
    case ms150:
      return "ms150";
    case ms300:
      return "ms300";
    case ms500:
      return "ms500";
    case ms750:
      return "ms750";
    case ms1500:
      return "ms1500";
    case infinity:
      return "infinity";
    default:
      invalid_enum_value(value, "pdcp_cfg_s::discard_timer_e_");
  }
  return "";
}
int16_t pdcp_cfg_s::discard_timer_e_::to_number() const
{
  const static int16_t options[] = {50, 100, 150, 300, 500, 750, 1500, -1};
  return get_enum_number(options, 8, value, "pdcp_cfg_s::discard_timer_e_");
}

std::string pdcp_cfg_s::rlc_um_s_::pdcp_sn_size_e_::to_string() const
{
  switch (value) {
    case len7bits:
      return "len7bits";
    case len12bits:
      return "len12bits";
    default:
      invalid_enum_value(value, "pdcp_cfg_s::rlc_um_s_::pdcp_sn_size_e_");
  }
  return "";
}
uint8_t pdcp_cfg_s::rlc_um_s_::pdcp_sn_size_e_::to_number() const
{
  const static uint8_t options[] = {7, 12};
  return get_enum_number(options, 2, value, "pdcp_cfg_s::rlc_um_s_::pdcp_sn_size_e_");
}

std::string pdcp_cfg_s::hdr_compress_c_::types::to_string() const
{
  switch (value) {
    case not_used:
      return "notUsed";
    case rohc:
      return "rohc";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "pdcp_cfg_s::hdr_compress_c_::types");
  }
  return "";
}

std::string pdcp_cfg_s::t_reordering_r12_e_::to_string() const
{
  switch (value) {
    case ms0:
      return "ms0";
    case ms20:
      return "ms20";
    case ms40:
      return "ms40";
    case ms60:
      return "ms60";
    case ms80:
      return "ms80";
    case ms100:
      return "ms100";
    case ms120:
      return "ms120";
    case ms140:
      return "ms140";
    case ms160:
      return "ms160";
    case ms180:
      return "ms180";
    case ms200:
      return "ms200";
    case ms220:
      return "ms220";
    case ms240:
      return "ms240";
    case ms260:
      return "ms260";
    case ms280:
      return "ms280";
    case ms300:
      return "ms300";
    case ms500:
      return "ms500";
    case ms750:
      return "ms750";
    case spare14:
      return "spare14";
    case spare13:
      return "spare13";
    case spare12:
      return "spare12";
    case spare11:
      return "spare11";
    case spare10:
      return "spare10";
    case spare9:
      return "spare9";
    case spare8:
      return "spare8";
    case spare7:
      return "spare7";
    case spare6:
      return "spare6";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "pdcp_cfg_s::t_reordering_r12_e_");
  }
  return "";
}
uint16_t pdcp_cfg_s::t_reordering_r12_e_::to_number() const
{
  const static uint16_t options[] = {0,   20,  40,  60,  80,  100, 120, 140, 160,
                                     180, 200, 220, 240, 260, 280, 300, 500, 750};
  return get_enum_number(options, 18, value, "pdcp_cfg_s::t_reordering_r12_e_");
}

std::string pdcp_cfg_s::ul_data_split_thres_r13_c_::setup_e_::to_string() const
{
  switch (value) {
    case b0:
      return "b0";
    case b100:
      return "b100";
    case b200:
      return "b200";
    case b400:
      return "b400";
    case b800:
      return "b800";
    case b1600:
      return "b1600";
    case b3200:
      return "b3200";
    case b6400:
      return "b6400";
    case b12800:
      return "b12800";
    case b25600:
      return "b25600";
    case b51200:
      return "b51200";
    case b102400:
      return "b102400";
    case b204800:
      return "b204800";
    case b409600:
      return "b409600";
    case b819200:
      return "b819200";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "pdcp_cfg_s::ul_data_split_thres_r13_c_::setup_e_");
  }
  return "";
}
uint32_t pdcp_cfg_s::ul_data_split_thres_r13_c_::setup_e_::to_number() const
{
  const static uint32_t options[] = {0,     100,   200,   400,    800,    1600,   3200,  6400,
                                     12800, 25600, 51200, 102400, 204800, 409600, 819200};
  return get_enum_number(options, 15, value, "pdcp_cfg_s::ul_data_split_thres_r13_c_::setup_e_");
}

std::string pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_type_for_polling_r13_e_::to_string() const
{
  switch (value) {
    case type1:
      return "type1";
    case type2:
      return "type2";
    default:
      invalid_enum_value(value, "pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_type_for_polling_r13_e_");
  }
  return "";
}
uint8_t pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_type_for_polling_r13_e_::to_number() const
{
  const static uint8_t options[] = {1, 2};
  return get_enum_number(options, 2, value,
                         "pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_type_for_polling_r13_e_");
}

std::string pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_type1_r13_e_::to_string() const
{
  switch (value) {
    case ms5:
      return "ms5";
    case ms10:
      return "ms10";
    case ms20:
      return "ms20";
    case ms30:
      return "ms30";
    case ms40:
      return "ms40";
    case ms50:
      return "ms50";
    case ms60:
      return "ms60";
    case ms70:
      return "ms70";
    case ms80:
      return "ms80";
    case ms90:
      return "ms90";
    case ms100:
      return "ms100";
    case ms150:
      return "ms150";
    case ms200:
      return "ms200";
    case ms300:
      return "ms300";
    case ms500:
      return "ms500";
    case ms1000:
      return "ms1000";
    case ms2000:
      return "ms2000";
    case ms5000:
      return "ms5000";
    case ms10000:
      return "ms10000";
    case ms20000:
      return "ms20000";
    case ms50000:
      return "ms50000";
    default:
      invalid_enum_value(value, "pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_type1_r13_e_");
  }
  return "";
}
uint16_t pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_type1_r13_e_::to_number() const
{
  const static uint16_t options[] = {5,   10,  20,  30,  40,   50,   60,   70,    80,    90,   100,
                                     150, 200, 300, 500, 1000, 2000, 5000, 10000, 20000, 50000};
  return get_enum_number(options, 21, value,
                         "pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_type1_r13_e_");
}

std::string pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_type2_r13_e_::to_string() const
{
  switch (value) {
    case ms5:
      return "ms5";
    case ms10:
      return "ms10";
    case ms20:
      return "ms20";
    case ms30:
      return "ms30";
    case ms40:
      return "ms40";
    case ms50:
      return "ms50";
    case ms60:
      return "ms60";
    case ms70:
      return "ms70";
    case ms80:
      return "ms80";
    case ms90:
      return "ms90";
    case ms100:
      return "ms100";
    case ms150:
      return "ms150";
    case ms200:
      return "ms200";
    case ms300:
      return "ms300";
    case ms500:
      return "ms500";
    case ms1000:
      return "ms1000";
    case ms2000:
      return "ms2000";
    case ms5000:
      return "ms5000";
    case ms10000:
      return "ms10000";
    case ms20000:
      return "ms20000";
    case ms50000:
      return "ms50000";
    default:
      invalid_enum_value(value, "pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_type2_r13_e_");
  }
  return "";
}
uint16_t pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_type2_r13_e_::to_number() const
{
  const static uint16_t options[] = {5,   10,  20,  30,  40,   50,   60,   70,    80,    90,   100,
                                     150, 200, 300, 500, 1000, 2000, 5000, 10000, 20000, 50000};
  return get_enum_number(options, 21, value,
                         "pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_type2_r13_e_");
}

std::string pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_offset_r13_e_::to_string() const
{
  switch (value) {
    case ms1:
      return "ms1";
    case ms2:
      return "ms2";
    case ms5:
      return "ms5";
    case ms10:
      return "ms10";
    case ms25:
      return "ms25";
    case ms50:
      return "ms50";
    case ms100:
      return "ms100";
    case ms250:
      return "ms250";
    case ms500:
      return "ms500";
    case ms2500:
      return "ms2500";
    case ms5000:
      return "ms5000";
    case ms25000:
      return "ms25000";
    default:
      invalid_enum_value(value, "pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_offset_r13_e_");
  }
  return "";
}
uint16_t pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_offset_r13_e_::to_number() const
{
  const static uint16_t options[] = {1, 2, 5, 10, 25, 50, 100, 250, 500, 2500, 5000, 25000};
  return get_enum_number(options, 12, value,
                         "pdcp_cfg_s::status_feedback_r13_c_::setup_s_::status_pdu_periodicity_offset_r13_e_");
}

std::string pdcp_cfg_s::ul_lwa_cfg_r14_c_::setup_s_::ul_lwa_data_split_thres_r14_e_::to_string() const
{
  switch (value) {
    case b0:
      return "b0";
    case b100:
      return "b100";
    case b200:
      return "b200";
    case b400:
      return "b400";
    case b800:
      return "b800";
    case b1600:
      return "b1600";
    case b3200:
      return "b3200";
    case b6400:
      return "b6400";
    case b12800:
      return "b12800";
    case b25600:
      return "b25600";
    case b51200:
      return "b51200";
    case b102400:
      return "b102400";
    case b204800:
      return "b204800";
    case b409600:
      return "b409600";
    case b819200:
      return "b819200";
    default:
      invalid_enum_value(value, "pdcp_cfg_s::ul_lwa_cfg_r14_c_::setup_s_::ul_lwa_data_split_thres_r14_e_");
  }
  return "";
}
uint32_t pdcp_cfg_s::ul_lwa_cfg_r14_c_::setup_s_::ul_lwa_data_split_thres_r14_e_::to_number() const
{
  const static uint32_t options[] = {0,     100,   200,   400,    800,    1600,   3200,  6400,
                                     12800, 25600, 51200, 102400, 204800, 409600, 819200};
  return get_enum_number(options, 15, value, "pdcp_cfg_s::ul_lwa_cfg_r14_c_::setup_s_::ul_lwa_data_split_thres_r14_e_");
}

std::string pdcp_cfg_s::ul_only_hdr_compress_r14_c_::types::to_string() const
{
  switch (value) {
    case not_used_r14:
      return "notUsed-r14";
    case rohc_r14:
      return "rohc-r14";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "pdcp_cfg_s::ul_only_hdr_compress_r14_c_::types");
  }
  return "";
}

std::string pdcp_cfg_s::ul_data_compress_r15_s_::buffer_size_r15_e_::to_string() const
{
  switch (value) {
    case kbyte2:
      return "kbyte2";
    case kbyte4:
      return "kbyte4";
    case kbyte8:
      return "kbyte8";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "pdcp_cfg_s::ul_data_compress_r15_s_::buffer_size_r15_e_");
  }
  return "";
}
uint8_t pdcp_cfg_s::ul_data_compress_r15_s_::buffer_size_r15_e_::to_number() const
{
  const static uint8_t options[] = {2, 4, 8};
  return get_enum_number(options, 3, value, "pdcp_cfg_s::ul_data_compress_r15_s_::buffer_size_r15_e_");
}

std::string pdcp_cfg_s::ul_data_compress_r15_s_::dictionary_r15_e_::to_string() const
{
  switch (value) {
    case sip_sdp:
      return "sip-SDP";
    case operator_type:
      return "operator";
    default:
      invalid_enum_value(value, "pdcp_cfg_s::ul_data_compress_r15_s_::dictionary_r15_e_");
  }
  return "";
}

std::string pdcp_cfg_s::pdcp_dupl_cfg_r15_c_::setup_s_::pdcp_dupl_r15_e_::to_string() const
{
  switch (value) {
    case cfgured:
      return "configured";
    case activ:
      return "activated";
    default:
      invalid_enum_value(value, "pdcp_cfg_s::pdcp_dupl_cfg_r15_c_::setup_s_::pdcp_dupl_r15_e_");
  }
  return "";
}

std::string pucch_cfg_ded_v1530_s::codebooksize_determination_stti_r15_e_::to_string() const
{
  switch (value) {
    case dai:
      return "dai";
    case cc:
      return "cc";
    default:
      invalid_enum_value(value, "pucch_cfg_ded_v1530_s::codebooksize_determination_stti_r15_e_");
  }
  return "";
}

std::string rlc_bearer_cfg_r15_c::setup_s_::lc_ch_id_cfg_r15_c_::types::to_string() const
{
  switch (value) {
    case lc_ch_id_r15:
      return "logicalChannelIdentity-r15";
    case lc_ch_id_ext_r15:
      return "logicalChannelIdentityExt-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rlc_bearer_cfg_r15_c::setup_s_::lc_ch_id_cfg_r15_c_::types");
  }
  return "";
}

std::string rlc_cfg_c::types::to_string() const
{
  switch (value) {
    case am:
      return "am";
    case um_bi_dir:
      return "um-Bi-Directional";
    case um_uni_dir_ul:
      return "um-Uni-Directional-UL";
    case um_uni_dir_dl:
      return "um-Uni-Directional-DL";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rlc_cfg_c::types");
  }
  return "";
}

std::string spdcch_cfg_r15_c::setup_s_::spdcch_l1_reuse_ind_r15_e_::to_string() const
{
  switch (value) {
    case n0:
      return "n0";
    case n1:
      return "n1";
    case n2:
      return "n2";
    default:
      invalid_enum_value(value, "spdcch_cfg_r15_c::setup_s_::spdcch_l1_reuse_ind_r15_e_");
  }
  return "";
}
uint8_t spdcch_cfg_r15_c::setup_s_::spdcch_l1_reuse_ind_r15_e_::to_number() const
{
  const static uint8_t options[] = {0, 1, 2};
  return get_enum_number(options, 3, value, "spdcch_cfg_r15_c::setup_s_::spdcch_l1_reuse_ind_r15_e_");
}

std::string sps_cfg_sl_r14_s::semi_persist_sched_interv_sl_r14_e_::to_string() const
{
  switch (value) {
    case sf20:
      return "sf20";
    case sf50:
      return "sf50";
    case sf100:
      return "sf100";
    case sf200:
      return "sf200";
    case sf300:
      return "sf300";
    case sf400:
      return "sf400";
    case sf500:
      return "sf500";
    case sf600:
      return "sf600";
    case sf700:
      return "sf700";
    case sf800:
      return "sf800";
    case sf900:
      return "sf900";
    case sf1000:
      return "sf1000";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "sps_cfg_sl_r14_s::semi_persist_sched_interv_sl_r14_e_");
  }
  return "";
}
uint16_t sps_cfg_sl_r14_s::semi_persist_sched_interv_sl_r14_e_::to_number() const
{
  const static uint16_t options[] = {20, 50, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
  return get_enum_number(options, 12, value, "sps_cfg_sl_r14_s::semi_persist_sched_interv_sl_r14_e_");
}

std::string sps_cfg_ul_c::setup_s_::semi_persist_sched_interv_ul_e_::to_string() const
{
  switch (value) {
    case sf10:
      return "sf10";
    case sf20:
      return "sf20";
    case sf32:
      return "sf32";
    case sf40:
      return "sf40";
    case sf64:
      return "sf64";
    case sf80:
      return "sf80";
    case sf128:
      return "sf128";
    case sf160:
      return "sf160";
    case sf320:
      return "sf320";
    case sf640:
      return "sf640";
    case sf1_v1430:
      return "sf1-v1430";
    case sf2_v1430:
      return "sf2-v1430";
    case sf3_v1430:
      return "sf3-v1430";
    case sf4_v1430:
      return "sf4-v1430";
    case sf5_v1430:
      return "sf5-v1430";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "sps_cfg_ul_c::setup_s_::semi_persist_sched_interv_ul_e_");
  }
  return "";
}
uint16_t sps_cfg_ul_c::setup_s_::semi_persist_sched_interv_ul_e_::to_number() const
{
  const static uint16_t options[] = {10, 20, 32, 40, 64, 80, 128, 160, 320, 640, 1, 2, 3, 4, 5};
  return get_enum_number(options, 15, value, "sps_cfg_ul_c::setup_s_::semi_persist_sched_interv_ul_e_");
}

std::string sps_cfg_ul_c::setup_s_::implicit_release_after_e_::to_string() const
{
  switch (value) {
    case e2:
      return "e2";
    case e3:
      return "e3";
    case e4:
      return "e4";
    case e8:
      return "e8";
    default:
      invalid_enum_value(value, "sps_cfg_ul_c::setup_s_::implicit_release_after_e_");
  }
  return "";
}
uint8_t sps_cfg_ul_c::setup_s_::implicit_release_after_e_::to_number() const
{
  const static uint8_t options[] = {2, 3, 4, 8};
  return get_enum_number(options, 4, value, "sps_cfg_ul_c::setup_s_::implicit_release_after_e_");
}

std::string sps_cfg_ul_c::setup_s_::semi_persist_sched_interv_ul_v1430_e_::to_string() const
{
  switch (value) {
    case sf50:
      return "sf50";
    case sf100:
      return "sf100";
    case sf200:
      return "sf200";
    case sf300:
      return "sf300";
    case sf400:
      return "sf400";
    case sf500:
      return "sf500";
    case sf600:
      return "sf600";
    case sf700:
      return "sf700";
    case sf800:
      return "sf800";
    case sf900:
      return "sf900";
    case sf1000:
      return "sf1000";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "sps_cfg_ul_c::setup_s_::semi_persist_sched_interv_ul_v1430_e_");
  }
  return "";
}
uint16_t sps_cfg_ul_c::setup_s_::semi_persist_sched_interv_ul_v1430_e_::to_number() const
{
  const static uint16_t options[] = {50, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
  return get_enum_number(options, 11, value, "sps_cfg_ul_c::setup_s_::semi_persist_sched_interv_ul_v1430_e_");
}

std::string sps_cfg_ul_c::setup_s_::cyclic_shift_sps_r15_e_::to_string() const
{
  switch (value) {
    case cs0:
      return "cs0";
    case cs1:
      return "cs1";
    case cs2:
      return "cs2";
    case cs3:
      return "cs3";
    case cs4:
      return "cs4";
    case cs5:
      return "cs5";
    case cs6:
      return "cs6";
    case cs7:
      return "cs7";
    default:
      invalid_enum_value(value, "sps_cfg_ul_c::setup_s_::cyclic_shift_sps_r15_e_");
  }
  return "";
}
uint8_t sps_cfg_ul_c::setup_s_::cyclic_shift_sps_r15_e_::to_number() const
{
  const static uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7};
  return get_enum_number(options, 8, value, "sps_cfg_ul_c::setup_s_::cyclic_shift_sps_r15_e_");
}

std::string sps_cfg_ul_c::setup_s_::rv_sps_ul_repeats_r15_e_::to_string() const
{
  switch (value) {
    case ulrvseq1:
      return "ulrvseq1";
    case ulrvseq2:
      return "ulrvseq2";
    case ulrvseq3:
      return "ulrvseq3";
    default:
      invalid_enum_value(value, "sps_cfg_ul_c::setup_s_::rv_sps_ul_repeats_r15_e_");
  }
  return "";
}
uint8_t sps_cfg_ul_c::setup_s_::rv_sps_ul_repeats_r15_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3};
  return get_enum_number(options, 3, value, "sps_cfg_ul_c::setup_s_::rv_sps_ul_repeats_r15_e_");
}

std::string sps_cfg_ul_c::setup_s_::total_num_pusch_sps_ul_repeats_r15_e_::to_string() const
{
  switch (value) {
    case n2:
      return "n2";
    case n3:
      return "n3";
    case n4:
      return "n4";
    case n6:
      return "n6";
    default:
      invalid_enum_value(value, "sps_cfg_ul_c::setup_s_::total_num_pusch_sps_ul_repeats_r15_e_");
  }
  return "";
}
uint8_t sps_cfg_ul_c::setup_s_::total_num_pusch_sps_ul_repeats_r15_e_::to_number() const
{
  const static uint8_t options[] = {2, 3, 4, 6};
  return get_enum_number(options, 4, value, "sps_cfg_ul_c::setup_s_::total_num_pusch_sps_ul_repeats_r15_e_");
}

std::string sps_cfg_ul_stti_r15_c::setup_s_::semi_persist_sched_interv_ul_stti_r15_e_::to_string() const
{
  switch (value) {
    case s_tti1:
      return "sTTI1";
    case s_tti2:
      return "sTTI2";
    case s_tti3:
      return "sTTI3";
    case s_tti4:
      return "sTTI4";
    case s_tti6:
      return "sTTI6";
    case s_tti8:
      return "sTTI8";
    case s_tti12:
      return "sTTI12";
    case s_tti16:
      return "sTTI16";
    case s_tti20:
      return "sTTI20";
    case s_tti40:
      return "sTTI40";
    case s_tti60:
      return "sTTI60";
    case s_tti80:
      return "sTTI80";
    case s_tti120:
      return "sTTI120";
    case s_tti240:
      return "sTTI240";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "sps_cfg_ul_stti_r15_c::setup_s_::semi_persist_sched_interv_ul_stti_r15_e_");
  }
  return "";
}
uint8_t sps_cfg_ul_stti_r15_c::setup_s_::semi_persist_sched_interv_ul_stti_r15_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 4, 6, 8, 12, 16, 20, 40, 60, 80, 120, 240};
  return get_enum_number(options, 14, value,
                         "sps_cfg_ul_stti_r15_c::setup_s_::semi_persist_sched_interv_ul_stti_r15_e_");
}

std::string sps_cfg_ul_stti_r15_c::setup_s_::implicit_release_after_e_::to_string() const
{
  switch (value) {
    case e2:
      return "e2";
    case e3:
      return "e3";
    case e4:
      return "e4";
    case e8:
      return "e8";
    default:
      invalid_enum_value(value, "sps_cfg_ul_stti_r15_c::setup_s_::implicit_release_after_e_");
  }
  return "";
}
uint8_t sps_cfg_ul_stti_r15_c::setup_s_::implicit_release_after_e_::to_number() const
{
  const static uint8_t options[] = {2, 3, 4, 8};
  return get_enum_number(options, 4, value, "sps_cfg_ul_stti_r15_c::setup_s_::implicit_release_after_e_");
}

std::string sps_cfg_ul_stti_r15_c::setup_s_::cyclic_shift_sps_s_tti_r15_e_::to_string() const
{
  switch (value) {
    case cs0:
      return "cs0";
    case cs1:
      return "cs1";
    case cs2:
      return "cs2";
    case cs3:
      return "cs3";
    case cs4:
      return "cs4";
    case cs5:
      return "cs5";
    case cs6:
      return "cs6";
    case cs7:
      return "cs7";
    default:
      invalid_enum_value(value, "sps_cfg_ul_stti_r15_c::setup_s_::cyclic_shift_sps_s_tti_r15_e_");
  }
  return "";
}
uint8_t sps_cfg_ul_stti_r15_c::setup_s_::cyclic_shift_sps_s_tti_r15_e_::to_number() const
{
  const static uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7};
  return get_enum_number(options, 8, value, "sps_cfg_ul_stti_r15_c::setup_s_::cyclic_shift_sps_s_tti_r15_e_");
}

std::string sps_cfg_ul_stti_r15_c::setup_s_::rv_sps_stti_ul_repeats_r15_e_::to_string() const
{
  switch (value) {
    case ulrvseq1:
      return "ulrvseq1";
    case ulrvseq2:
      return "ulrvseq2";
    case ulrvseq3:
      return "ulrvseq3";
    default:
      invalid_enum_value(value, "sps_cfg_ul_stti_r15_c::setup_s_::rv_sps_stti_ul_repeats_r15_e_");
  }
  return "";
}
uint8_t sps_cfg_ul_stti_r15_c::setup_s_::rv_sps_stti_ul_repeats_r15_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3};
  return get_enum_number(options, 3, value, "sps_cfg_ul_stti_r15_c::setup_s_::rv_sps_stti_ul_repeats_r15_e_");
}

std::string sps_cfg_ul_stti_r15_c::setup_s_::tbs_scaling_factor_subslot_sps_ul_repeats_r15_e_::to_string() const
{
  switch (value) {
    case n6:
      return "n6";
    case n12:
      return "n12";
    default:
      invalid_enum_value(value, "sps_cfg_ul_stti_r15_c::setup_s_::tbs_scaling_factor_subslot_sps_ul_repeats_r15_e_");
  }
  return "";
}
uint8_t sps_cfg_ul_stti_r15_c::setup_s_::tbs_scaling_factor_subslot_sps_ul_repeats_r15_e_::to_number() const
{
  const static uint8_t options[] = {6, 12};
  return get_enum_number(options, 2, value,
                         "sps_cfg_ul_stti_r15_c::setup_s_::tbs_scaling_factor_subslot_sps_ul_repeats_r15_e_");
}

std::string sps_cfg_ul_stti_r15_c::setup_s_::total_num_pusch_sps_stti_ul_repeats_r15_e_::to_string() const
{
  switch (value) {
    case n2:
      return "n2";
    case n3:
      return "n3";
    case n4:
      return "n4";
    case n6:
      return "n6";
    default:
      invalid_enum_value(value, "sps_cfg_ul_stti_r15_c::setup_s_::total_num_pusch_sps_stti_ul_repeats_r15_e_");
  }
  return "";
}
uint8_t sps_cfg_ul_stti_r15_c::setup_s_::total_num_pusch_sps_stti_ul_repeats_r15_e_::to_number() const
{
  const static uint8_t options[] = {2, 3, 4, 6};
  return get_enum_number(options, 4, value,
                         "sps_cfg_ul_stti_r15_c::setup_s_::total_num_pusch_sps_stti_ul_repeats_r15_e_");
}

std::string srs_cfg_ap_r10_s::srs_bw_ap_r10_e_::to_string() const
{
  switch (value) {
    case bw0:
      return "bw0";
    case bw1:
      return "bw1";
    case bw2:
      return "bw2";
    case bw3:
      return "bw3";
    default:
      invalid_enum_value(value, "srs_cfg_ap_r10_s::srs_bw_ap_r10_e_");
  }
  return "";
}
uint8_t srs_cfg_ap_r10_s::srs_bw_ap_r10_e_::to_number() const
{
  const static uint8_t options[] = {0, 1, 2, 3};
  return get_enum_number(options, 4, value, "srs_cfg_ap_r10_s::srs_bw_ap_r10_e_");
}

std::string srs_cfg_ap_r10_s::cyclic_shift_ap_r10_e_::to_string() const
{
  switch (value) {
    case cs0:
      return "cs0";
    case cs1:
      return "cs1";
    case cs2:
      return "cs2";
    case cs3:
      return "cs3";
    case cs4:
      return "cs4";
    case cs5:
      return "cs5";
    case cs6:
      return "cs6";
    case cs7:
      return "cs7";
    default:
      invalid_enum_value(value, "srs_cfg_ap_r10_s::cyclic_shift_ap_r10_e_");
  }
  return "";
}
uint8_t srs_cfg_ap_r10_s::cyclic_shift_ap_r10_e_::to_number() const
{
  const static uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7};
  return get_enum_number(options, 8, value, "srs_cfg_ap_r10_s::cyclic_shift_ap_r10_e_");
}

std::string srs_cfg_ap_r13_s::srs_bw_ap_r13_e_::to_string() const
{
  switch (value) {
    case bw0:
      return "bw0";
    case bw1:
      return "bw1";
    case bw2:
      return "bw2";
    case bw3:
      return "bw3";
    default:
      invalid_enum_value(value, "srs_cfg_ap_r13_s::srs_bw_ap_r13_e_");
  }
  return "";
}
uint8_t srs_cfg_ap_r13_s::srs_bw_ap_r13_e_::to_number() const
{
  const static uint8_t options[] = {0, 1, 2, 3};
  return get_enum_number(options, 4, value, "srs_cfg_ap_r13_s::srs_bw_ap_r13_e_");
}

std::string srs_cfg_ap_r13_s::cyclic_shift_ap_r13_e_::to_string() const
{
  switch (value) {
    case cs0:
      return "cs0";
    case cs1:
      return "cs1";
    case cs2:
      return "cs2";
    case cs3:
      return "cs3";
    case cs4:
      return "cs4";
    case cs5:
      return "cs5";
    case cs6:
      return "cs6";
    case cs7:
      return "cs7";
    case cs8:
      return "cs8";
    case cs9:
      return "cs9";
    case cs10:
      return "cs10";
    case cs11:
      return "cs11";
    default:
      invalid_enum_value(value, "srs_cfg_ap_r13_s::cyclic_shift_ap_r13_e_");
  }
  return "";
}
uint8_t srs_cfg_ap_r13_s::cyclic_shift_ap_r13_e_::to_number() const
{
  const static uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  return get_enum_number(options, 12, value, "srs_cfg_ap_r13_s::cyclic_shift_ap_r13_e_");
}

std::string srs_cfg_ap_r13_s::tx_comb_num_r13_e_::to_string() const
{
  switch (value) {
    case n2:
      return "n2";
    case n4:
      return "n4";
    default:
      invalid_enum_value(value, "srs_cfg_ap_r13_s::tx_comb_num_r13_e_");
  }
  return "";
}
uint8_t srs_cfg_ap_r13_s::tx_comb_num_r13_e_::to_number() const
{
  const static uint8_t options[] = {2, 4};
  return get_enum_number(options, 2, value, "srs_cfg_ap_r13_s::tx_comb_num_r13_e_");
}

std::string srs_cfg_ap_v1310_s::cyclic_shift_ap_v1310_e_::to_string() const
{
  switch (value) {
    case cs8:
      return "cs8";
    case cs9:
      return "cs9";
    case cs10:
      return "cs10";
    case cs11:
      return "cs11";
    default:
      invalid_enum_value(value, "srs_cfg_ap_v1310_s::cyclic_shift_ap_v1310_e_");
  }
  return "";
}
uint8_t srs_cfg_ap_v1310_s::cyclic_shift_ap_v1310_e_::to_number() const
{
  const static uint8_t options[] = {8, 9, 10, 11};
  return get_enum_number(options, 4, value, "srs_cfg_ap_v1310_s::cyclic_shift_ap_v1310_e_");
}

std::string srs_cfg_ap_v1310_s::tx_comb_num_r13_e_::to_string() const
{
  switch (value) {
    case n2:
      return "n2";
    case n4:
      return "n4";
    default:
      invalid_enum_value(value, "srs_cfg_ap_v1310_s::tx_comb_num_r13_e_");
  }
  return "";
}
uint8_t srs_cfg_ap_v1310_s::tx_comb_num_r13_e_::to_number() const
{
  const static uint8_t options[] = {2, 4};
  return get_enum_number(options, 2, value, "srs_cfg_ap_v1310_s::tx_comb_num_r13_e_");
}

std::string sched_request_cfg_v1530_c::setup_s_::dssr_trans_max_r15_e_::to_string() const
{
  switch (value) {
    case n4:
      return "n4";
    case n8:
      return "n8";
    case n16:
      return "n16";
    case n32:
      return "n32";
    case n64:
      return "n64";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "sched_request_cfg_v1530_c::setup_s_::dssr_trans_max_r15_e_");
  }
  return "";
}
uint8_t sched_request_cfg_v1530_c::setup_s_::dssr_trans_max_r15_e_::to_number() const
{
  const static uint8_t options[] = {4, 8, 16, 32, 64};
  return get_enum_number(options, 5, value, "sched_request_cfg_v1530_c::setup_s_::dssr_trans_max_r15_e_");
}

std::string slot_or_subslot_pdsch_cfg_r15_c::setup_s_::alt_cqi_table_stti_r15_e_::to_string() const
{
  switch (value) {
    case all_sfs:
      return "allSubframes";
    case csi_sf_set1:
      return "csi-SubframeSet1";
    case csi_sf_set2:
      return "csi-SubframeSet2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "slot_or_subslot_pdsch_cfg_r15_c::setup_s_::alt_cqi_table_stti_r15_e_");
  }
  return "";
}
uint8_t slot_or_subslot_pdsch_cfg_r15_c::setup_s_::alt_cqi_table_stti_r15_e_::to_number() const
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

std::string slot_or_subslot_pdsch_cfg_r15_c::setup_s_::alt_cqi_table1024_qam_stti_r15_e_::to_string() const
{
  switch (value) {
    case all_sfs:
      return "allSubframes";
    case csi_sf_set1:
      return "csi-SubframeSet1";
    case csi_sf_set2:
      return "csi-SubframeSet2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "slot_or_subslot_pdsch_cfg_r15_c::setup_s_::alt_cqi_table1024_qam_stti_r15_e_");
  }
  return "";
}
uint8_t slot_or_subslot_pdsch_cfg_r15_c::setup_s_::alt_cqi_table1024_qam_stti_r15_e_::to_number() const
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

std::string slot_or_subslot_pdsch_cfg_r15_c::setup_s_::res_alloc_r15_e_::to_string() const
{
  switch (value) {
    case res_alloc_type0:
      return "resourceAllocationType0";
    case res_alloc_type2:
      return "resourceAllocationType2";
    default:
      invalid_enum_value(value, "slot_or_subslot_pdsch_cfg_r15_c::setup_s_::res_alloc_r15_e_");
  }
  return "";
}
uint8_t slot_or_subslot_pdsch_cfg_r15_c::setup_s_::res_alloc_r15_e_::to_number() const
{
  const static uint8_t options[] = {0, 2};
  return get_enum_number(options, 2, value, "slot_or_subslot_pdsch_cfg_r15_c::setup_s_::res_alloc_r15_e_");
}

std::string tdd_pusch_up_pts_r14_c::setup_s_::sym_pusch_up_pts_r14_e_::to_string() const
{
  switch (value) {
    case sym1:
      return "sym1";
    case sym2:
      return "sym2";
    case sym3:
      return "sym3";
    case sym4:
      return "sym4";
    case sym5:
      return "sym5";
    case sym6:
      return "sym6";
    default:
      invalid_enum_value(value, "tdd_pusch_up_pts_r14_c::setup_s_::sym_pusch_up_pts_r14_e_");
  }
  return "";
}
uint8_t tdd_pusch_up_pts_r14_c::setup_s_::sym_pusch_up_pts_r14_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 4, 5, 6};
  return get_enum_number(options, 6, value, "tdd_pusch_up_pts_r14_c::setup_s_::sym_pusch_up_pts_r14_e_");
}

std::string ant_info_ded_s::tx_mode_e_::to_string() const
{
  switch (value) {
    case tm1:
      return "tm1";
    case tm2:
      return "tm2";
    case tm3:
      return "tm3";
    case tm4:
      return "tm4";
    case tm5:
      return "tm5";
    case tm6:
      return "tm6";
    case tm7:
      return "tm7";
    case tm8_v920:
      return "tm8-v920";
    default:
      invalid_enum_value(value, "ant_info_ded_s::tx_mode_e_");
  }
  return "";
}
uint8_t ant_info_ded_s::tx_mode_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 4, 5, 6, 7, 8};
  return get_enum_number(options, 8, value, "ant_info_ded_s::tx_mode_e_");
}

std::string ant_info_ded_s::codebook_subset_restrict_c_::types::to_string() const
{
  switch (value) {
    case n2_tx_ant_tm3:
      return "n2TxAntenna-tm3";
    case n4_tx_ant_tm3:
      return "n4TxAntenna-tm3";
    case n2_tx_ant_tm4:
      return "n2TxAntenna-tm4";
    case n4_tx_ant_tm4:
      return "n4TxAntenna-tm4";
    case n2_tx_ant_tm5:
      return "n2TxAntenna-tm5";
    case n4_tx_ant_tm5:
      return "n4TxAntenna-tm5";
    case n2_tx_ant_tm6:
      return "n2TxAntenna-tm6";
    case n4_tx_ant_tm6:
      return "n4TxAntenna-tm6";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ant_info_ded_s::codebook_subset_restrict_c_::types");
  }
  return "";
}

std::string ant_info_ded_s::ue_tx_ant_sel_c_::setup_e_::to_string() const
{
  switch (value) {
    case closed_loop:
      return "closedLoop";
    case open_loop:
      return "openLoop";
    default:
      invalid_enum_value(value, "ant_info_ded_s::ue_tx_ant_sel_c_::setup_e_");
  }
  return "";
}

std::string ant_info_ded_r10_s::tx_mode_r10_e_::to_string() const
{
  switch (value) {
    case tm1:
      return "tm1";
    case tm2:
      return "tm2";
    case tm3:
      return "tm3";
    case tm4:
      return "tm4";
    case tm5:
      return "tm5";
    case tm6:
      return "tm6";
    case tm7:
      return "tm7";
    case tm8_v920:
      return "tm8-v920";
    case tm9_v1020:
      return "tm9-v1020";
    case tm10_v1130:
      return "tm10-v1130";
    case spare6:
      return "spare6";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "ant_info_ded_r10_s::tx_mode_r10_e_");
  }
  return "";
}
uint8_t ant_info_ded_r10_s::tx_mode_r10_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  return get_enum_number(options, 10, value, "ant_info_ded_r10_s::tx_mode_r10_e_");
}

std::string ant_info_ded_r10_s::ue_tx_ant_sel_c_::setup_e_::to_string() const
{
  switch (value) {
    case closed_loop:
      return "closedLoop";
    case open_loop:
      return "openLoop";
    default:
      invalid_enum_value(value, "ant_info_ded_r10_s::ue_tx_ant_sel_c_::setup_e_");
  }
  return "";
}

std::string ant_info_ded_v1530_c::setup_c_::ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15_e_::to_string() const
{
  switch (value) {
    case two:
      return "two";
    case three:
      return "three";
    default:
      invalid_enum_value(value, "ant_info_ded_v1530_c::setup_c_::ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15_e_");
  }
  return "";
}
uint8_t ant_info_ded_v1530_c::setup_c_::ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15_e_::to_number() const
{
  const static uint8_t options[] = {2, 3};
  return get_enum_number(options, 2, value,
                         "ant_info_ded_v1530_c::setup_c_::ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15_e_");
}

std::string ant_info_ded_v1530_c::setup_c_::types::to_string() const
{
  switch (value) {
    case ue_tx_ant_sel_srs_minus1_t4_r_cfg_r15:
      return "ue-TxAntennaSelection-SRS-1T4R-Config-r15";
    case ue_tx_ant_sel_srs_minus2_t4_r_nr_of_pairs_r15:
      return "ue-TxAntennaSelection-SRS-2T4R-NrOfPairs-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ant_info_ded_v1530_c::setup_c_::types");
  }
  return "";
}
int8_t ant_info_ded_v1530_c::setup_c_::types::to_number() const
{
  const static int8_t options[] = {-1, -2};
  return get_enum_number(options, 2, value, "ant_info_ded_v1530_c::setup_c_::types");
}

std::string ant_info_ded_v920_s::codebook_subset_restrict_v920_c_::types::to_string() const
{
  switch (value) {
    case n2_tx_ant_tm8_r9:
      return "n2TxAntenna-tm8-r9";
    case n4_tx_ant_tm8_r9:
      return "n4TxAntenna-tm8-r9";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ant_info_ded_v920_s::codebook_subset_restrict_v920_c_::types");
  }
  return "";
}
uint8_t ant_info_ded_v920_s::codebook_subset_restrict_v920_c_::types::to_number() const
{
  const static uint8_t options[] = {2, 4};
  return get_enum_number(options, 2, value, "ant_info_ded_v920_s::codebook_subset_restrict_v920_c_::types");
}

std::string ant_info_ul_r10_s::tx_mode_ul_r10_e_::to_string() const
{
  switch (value) {
    case tm1:
      return "tm1";
    case tm2:
      return "tm2";
    case spare6:
      return "spare6";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "ant_info_ul_r10_s::tx_mode_ul_r10_e_");
  }
  return "";
}
uint8_t ant_info_ul_r10_s::tx_mode_ul_r10_e_::to_number() const
{
  const static uint8_t options[] = {1, 2};
  return get_enum_number(options, 2, value, "ant_info_ul_r10_s::tx_mode_ul_r10_e_");
}

std::string cqi_report_cfg_v1530_s::alt_cqi_table_minus1024_qam_r15_e_::to_string() const
{
  switch (value) {
    case all_sfs:
      return "allSubframes";
    case csi_sf_set1:
      return "csi-SubframeSet1";
    case csi_sf_set2:
      return "csi-SubframeSet2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "cqi_report_cfg_v1530_s::alt_cqi_table_minus1024_qam_r15_e_");
  }
  return "";
}
uint8_t cqi_report_cfg_v1530_s::alt_cqi_table_minus1024_qam_r15_e_::to_number() const
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

std::string drb_to_add_mod_s::drb_type_lwip_r13_e_::to_string() const
{
  switch (value) {
    case lwip:
      return "lwip";
    case lwip_dl_only:
      return "lwip-DL-only";
    case lwip_ul_only:
      return "lwip-UL-only";
    case eutran:
      return "eutran";
    default:
      invalid_enum_value(value, "drb_to_add_mod_s::drb_type_lwip_r13_e_");
  }
  return "";
}

std::string drb_to_add_mod_s::lwa_wlan_ac_r14_e_::to_string() const
{
  switch (value) {
    case ac_bk:
      return "ac-bk";
    case ac_be:
      return "ac-be";
    case ac_vi:
      return "ac-vi";
    case ac_vo:
      return "ac-vo";
    default:
      invalid_enum_value(value, "drb_to_add_mod_s::lwa_wlan_ac_r14_e_");
  }
  return "";
}

std::string drx_cfg_c::setup_s_::on_dur_timer_e_::to_string() const
{
  switch (value) {
    case psf1:
      return "psf1";
    case psf2:
      return "psf2";
    case psf3:
      return "psf3";
    case psf4:
      return "psf4";
    case psf5:
      return "psf5";
    case psf6:
      return "psf6";
    case psf8:
      return "psf8";
    case psf10:
      return "psf10";
    case psf20:
      return "psf20";
    case psf30:
      return "psf30";
    case psf40:
      return "psf40";
    case psf50:
      return "psf50";
    case psf60:
      return "psf60";
    case psf80:
      return "psf80";
    case psf100:
      return "psf100";
    case psf200:
      return "psf200";
    default:
      invalid_enum_value(value, "drx_cfg_c::setup_s_::on_dur_timer_e_");
  }
  return "";
}
uint8_t drx_cfg_c::setup_s_::on_dur_timer_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 4, 5, 6, 8, 10, 20, 30, 40, 50, 60, 80, 100, 200};
  return get_enum_number(options, 16, value, "drx_cfg_c::setup_s_::on_dur_timer_e_");
}

std::string drx_cfg_c::setup_s_::drx_inactivity_timer_e_::to_string() const
{
  switch (value) {
    case psf1:
      return "psf1";
    case psf2:
      return "psf2";
    case psf3:
      return "psf3";
    case psf4:
      return "psf4";
    case psf5:
      return "psf5";
    case psf6:
      return "psf6";
    case psf8:
      return "psf8";
    case psf10:
      return "psf10";
    case psf20:
      return "psf20";
    case psf30:
      return "psf30";
    case psf40:
      return "psf40";
    case psf50:
      return "psf50";
    case psf60:
      return "psf60";
    case psf80:
      return "psf80";
    case psf100:
      return "psf100";
    case psf200:
      return "psf200";
    case psf300:
      return "psf300";
    case psf500:
      return "psf500";
    case psf750:
      return "psf750";
    case psf1280:
      return "psf1280";
    case psf1920:
      return "psf1920";
    case psf2560:
      return "psf2560";
    case psf0_v1020:
      return "psf0-v1020";
    case spare9:
      return "spare9";
    case spare8:
      return "spare8";
    case spare7:
      return "spare7";
    case spare6:
      return "spare6";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "drx_cfg_c::setup_s_::drx_inactivity_timer_e_");
  }
  return "";
}
uint16_t drx_cfg_c::setup_s_::drx_inactivity_timer_e_::to_number() const
{
  const static uint16_t options[] = {1,  2,  3,   4,   5,   6,   8,   10,   20,   30,   40, 50,
                                     60, 80, 100, 200, 300, 500, 750, 1280, 1920, 2560, 0};
  return get_enum_number(options, 23, value, "drx_cfg_c::setup_s_::drx_inactivity_timer_e_");
}

std::string drx_cfg_c::setup_s_::drx_retx_timer_e_::to_string() const
{
  switch (value) {
    case psf1:
      return "psf1";
    case psf2:
      return "psf2";
    case psf4:
      return "psf4";
    case psf6:
      return "psf6";
    case psf8:
      return "psf8";
    case psf16:
      return "psf16";
    case psf24:
      return "psf24";
    case psf33:
      return "psf33";
    default:
      invalid_enum_value(value, "drx_cfg_c::setup_s_::drx_retx_timer_e_");
  }
  return "";
}
uint8_t drx_cfg_c::setup_s_::drx_retx_timer_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 4, 6, 8, 16, 24, 33};
  return get_enum_number(options, 8, value, "drx_cfg_c::setup_s_::drx_retx_timer_e_");
}

std::string drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::types::to_string() const
{
  switch (value) {
    case sf10:
      return "sf10";
    case sf20:
      return "sf20";
    case sf32:
      return "sf32";
    case sf40:
      return "sf40";
    case sf64:
      return "sf64";
    case sf80:
      return "sf80";
    case sf128:
      return "sf128";
    case sf160:
      return "sf160";
    case sf256:
      return "sf256";
    case sf320:
      return "sf320";
    case sf512:
      return "sf512";
    case sf640:
      return "sf640";
    case sf1024:
      return "sf1024";
    case sf1280:
      return "sf1280";
    case sf2048:
      return "sf2048";
    case sf2560:
      return "sf2560";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::types");
  }
  return "";
}
uint16_t drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::types::to_number() const
{
  const static uint16_t options[] = {10, 20, 32, 40, 64, 80, 128, 160, 256, 320, 512, 640, 1024, 1280, 2048, 2560};
  return get_enum_number(options, 16, value, "drx_cfg_c::setup_s_::long_drx_cycle_start_offset_c_::types");
}

std::string drx_cfg_c::setup_s_::short_drx_s_::short_drx_cycle_e_::to_string() const
{
  switch (value) {
    case sf2:
      return "sf2";
    case sf5:
      return "sf5";
    case sf8:
      return "sf8";
    case sf10:
      return "sf10";
    case sf16:
      return "sf16";
    case sf20:
      return "sf20";
    case sf32:
      return "sf32";
    case sf40:
      return "sf40";
    case sf64:
      return "sf64";
    case sf80:
      return "sf80";
    case sf128:
      return "sf128";
    case sf160:
      return "sf160";
    case sf256:
      return "sf256";
    case sf320:
      return "sf320";
    case sf512:
      return "sf512";
    case sf640:
      return "sf640";
    default:
      invalid_enum_value(value, "drx_cfg_c::setup_s_::short_drx_s_::short_drx_cycle_e_");
  }
  return "";
}
uint16_t drx_cfg_c::setup_s_::short_drx_s_::short_drx_cycle_e_::to_number() const
{
  const static uint16_t options[] = {2, 5, 8, 10, 16, 20, 32, 40, 64, 80, 128, 160, 256, 320, 512, 640};
  return get_enum_number(options, 16, value, "drx_cfg_c::setup_s_::short_drx_s_::short_drx_cycle_e_");
}

std::string drx_cfg_r13_s::on_dur_timer_v1310_e_::to_string() const
{
  switch (value) {
    case psf300:
      return "psf300";
    case psf400:
      return "psf400";
    case psf500:
      return "psf500";
    case psf600:
      return "psf600";
    case psf800:
      return "psf800";
    case psf1000:
      return "psf1000";
    case psf1200:
      return "psf1200";
    case psf1600:
      return "psf1600";
    default:
      invalid_enum_value(value, "drx_cfg_r13_s::on_dur_timer_v1310_e_");
  }
  return "";
}
uint16_t drx_cfg_r13_s::on_dur_timer_v1310_e_::to_number() const
{
  const static uint16_t options[] = {300, 400, 500, 600, 800, 1000, 1200, 1600};
  return get_enum_number(options, 8, value, "drx_cfg_r13_s::on_dur_timer_v1310_e_");
}

std::string drx_cfg_r13_s::drx_retx_timer_v1310_e_::to_string() const
{
  switch (value) {
    case psf40:
      return "psf40";
    case psf64:
      return "psf64";
    case psf80:
      return "psf80";
    case psf96:
      return "psf96";
    case psf112:
      return "psf112";
    case psf128:
      return "psf128";
    case psf160:
      return "psf160";
    case psf320:
      return "psf320";
    default:
      invalid_enum_value(value, "drx_cfg_r13_s::drx_retx_timer_v1310_e_");
  }
  return "";
}
uint16_t drx_cfg_r13_s::drx_retx_timer_v1310_e_::to_number() const
{
  const static uint16_t options[] = {40, 64, 80, 96, 112, 128, 160, 320};
  return get_enum_number(options, 8, value, "drx_cfg_r13_s::drx_retx_timer_v1310_e_");
}

std::string drx_cfg_r13_s::drx_ul_retx_timer_r13_e_::to_string() const
{
  switch (value) {
    case psf0:
      return "psf0";
    case psf1:
      return "psf1";
    case psf2:
      return "psf2";
    case psf4:
      return "psf4";
    case psf6:
      return "psf6";
    case psf8:
      return "psf8";
    case psf16:
      return "psf16";
    case psf24:
      return "psf24";
    case psf33:
      return "psf33";
    case psf40:
      return "psf40";
    case psf64:
      return "psf64";
    case psf80:
      return "psf80";
    case psf96:
      return "psf96";
    case psf112:
      return "psf112";
    case psf128:
      return "psf128";
    case psf160:
      return "psf160";
    case psf320:
      return "psf320";
    default:
      invalid_enum_value(value, "drx_cfg_r13_s::drx_ul_retx_timer_r13_e_");
  }
  return "";
}
uint16_t drx_cfg_r13_s::drx_ul_retx_timer_r13_e_::to_number() const
{
  const static uint16_t options[] = {0, 1, 2, 4, 6, 8, 16, 24, 33, 40, 64, 80, 96, 112, 128, 160, 320};
  return get_enum_number(options, 17, value, "drx_cfg_r13_s::drx_ul_retx_timer_r13_e_");
}

std::string drx_cfg_r15_s::drx_retx_timer_short_tti_r15_e_::to_string() const
{
  switch (value) {
    case tti10:
      return "tti10";
    case tti20:
      return "tti20";
    case tti40:
      return "tti40";
    case tti64:
      return "tti64";
    case tti80:
      return "tti80";
    case tti96:
      return "tti96";
    case tti112:
      return "tti112";
    case tti128:
      return "tti128";
    case tti160:
      return "tti160";
    case tti320:
      return "tti320";
    default:
      invalid_enum_value(value, "drx_cfg_r15_s::drx_retx_timer_short_tti_r15_e_");
  }
  return "";
}
uint16_t drx_cfg_r15_s::drx_retx_timer_short_tti_r15_e_::to_number() const
{
  const static uint16_t options[] = {10, 20, 40, 64, 80, 96, 112, 128, 160, 320};
  return get_enum_number(options, 10, value, "drx_cfg_r15_s::drx_retx_timer_short_tti_r15_e_");
}

std::string drx_cfg_r15_s::drx_ul_retx_timer_short_tti_r15_e_::to_string() const
{
  switch (value) {
    case tti0:
      return "tti0";
    case tti1:
      return "tti1";
    case tti2:
      return "tti2";
    case tti4:
      return "tti4";
    case tti6:
      return "tti6";
    case tti8:
      return "tti8";
    case tti16:
      return "tti16";
    case tti24:
      return "tti24";
    case tti33:
      return "tti33";
    case tti40:
      return "tti40";
    case tti64:
      return "tti64";
    case tti80:
      return "tti80";
    case tti96:
      return "tti96";
    case tti112:
      return "tti112";
    case tti128:
      return "tti128";
    case tti160:
      return "tti160";
    case tti320:
      return "tti320";
    default:
      invalid_enum_value(value, "drx_cfg_r15_s::drx_ul_retx_timer_short_tti_r15_e_");
  }
  return "";
}
uint16_t drx_cfg_r15_s::drx_ul_retx_timer_short_tti_r15_e_::to_number() const
{
  const static uint16_t options[] = {0, 1, 2, 4, 6, 8, 16, 24, 33, 40, 64, 80, 96, 112, 128, 160, 320};
  return get_enum_number(options, 17, value, "drx_cfg_r15_s::drx_ul_retx_timer_short_tti_r15_e_");
}

std::string drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_::types::to_string() const
{
  switch (value) {
    case sf60_v1130:
      return "sf60-v1130";
    case sf70_v1130:
      return "sf70-v1130";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_::types");
  }
  return "";
}
uint8_t drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_::types::to_number() const
{
  const static uint8_t options[] = {60, 70};
  return get_enum_number(options, 2, value, "drx_cfg_v1130_s::long_drx_cycle_start_offset_v1130_c_::types");
}

// DataInactivityTimer-r14 ::= ENUMERATED
std::string data_inactivity_timer_r14_e::to_string() const
{
  switch (value) {
    case s1:
      return "s1";
    case s2:
      return "s2";
    case s3:
      return "s3";
    case s5:
      return "s5";
    case s7:
      return "s7";
    case s10:
      return "s10";
    case s15:
      return "s15";
    case s20:
      return "s20";
    case s40:
      return "s40";
    case s50:
      return "s50";
    case s60:
      return "s60";
    case s80:
      return "s80";
    case s100:
      return "s100";
    case s120:
      return "s120";
    case s150:
      return "s150";
    case s180:
      return "s180";
    default:
      invalid_enum_value(value, "data_inactivity_timer_r14_e");
  }
  return "";
}
uint8_t data_inactivity_timer_r14_e::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 5, 7, 10, 15, 20, 40, 50, 60, 80, 100, 120, 150, 180};
  return get_enum_number(options, 16, value, "data_inactivity_timer_r14_e");
}

std::string pdsch_cfg_ded_s::p_a_e_::to_string() const
{
  switch (value) {
    case db_minus6:
      return "dB-6";
    case db_minus4dot77:
      return "dB-4dot77";
    case db_minus3:
      return "dB-3";
    case db_minus1dot77:
      return "dB-1dot77";
    case db0:
      return "dB0";
    case db1:
      return "dB1";
    case db2:
      return "dB2";
    case db3:
      return "dB3";
    default:
      invalid_enum_value(value, "pdsch_cfg_ded_s::p_a_e_");
  }
  return "";
}
float pdsch_cfg_ded_s::p_a_e_::to_number() const
{
  const static float options[] = {-6.0, -4.77, -3.0, -1.77, 0.0, 1.0, 2.0, 3.0};
  return get_enum_number(options, 8, value, "pdsch_cfg_ded_s::p_a_e_");
}
std::string pdsch_cfg_ded_s::p_a_e_::to_number_string() const
{
  switch (value) {
    case db_minus6:
      return "-6";
    case db_minus4dot77:
      return "-4.77";
    case db_minus3:
      return "-3";
    case db_minus1dot77:
      return "-1.77";
    case db0:
      return "0";
    case db1:
      return "1";
    case db2:
      return "2";
    case db3:
      return "3";
    default:
      invalid_enum_number(value, "pdsch_cfg_ded_s::p_a_e_");
  }
  return "";
}

std::string pdsch_cfg_ded_v1130_s::qcl_operation_e_::to_string() const
{
  switch (value) {
    case type_a:
      return "typeA";
    case type_b:
      return "typeB";
    default:
      invalid_enum_value(value, "pdsch_cfg_ded_v1130_s::qcl_operation_e_");
  }
  return "";
}

std::string pdsch_cfg_ded_v1280_s::tbs_idx_alt_r12_e_::to_string() const
{
  switch (value) {
    case a26:
      return "a26";
    case a33:
      return "a33";
    default:
      invalid_enum_value(value, "pdsch_cfg_ded_v1280_s::tbs_idx_alt_r12_e_");
  }
  return "";
}
uint8_t pdsch_cfg_ded_v1280_s::tbs_idx_alt_r12_e_::to_number() const
{
  const static uint8_t options[] = {26, 33};
  return get_enum_number(options, 2, value, "pdsch_cfg_ded_v1280_s::tbs_idx_alt_r12_e_");
}

std::string pdsch_cfg_ded_v1430_s::ce_pdsch_max_bw_r14_e_::to_string() const
{
  switch (value) {
    case bw5:
      return "bw5";
    case bw20:
      return "bw20";
    default:
      invalid_enum_value(value, "pdsch_cfg_ded_v1430_s::ce_pdsch_max_bw_r14_e_");
  }
  return "";
}
uint8_t pdsch_cfg_ded_v1430_s::ce_pdsch_max_bw_r14_e_::to_number() const
{
  const static uint8_t options[] = {5, 20};
  return get_enum_number(options, 2, value, "pdsch_cfg_ded_v1430_s::ce_pdsch_max_bw_r14_e_");
}

std::string pdsch_cfg_ded_v1430_s::ce_sched_enhancement_r14_e_::to_string() const
{
  switch (value) {
    case range1:
      return "range1";
    case range2:
      return "range2";
    default:
      invalid_enum_value(value, "pdsch_cfg_ded_v1430_s::ce_sched_enhancement_r14_e_");
  }
  return "";
}
uint8_t pdsch_cfg_ded_v1430_s::ce_sched_enhancement_r14_e_::to_number() const
{
  const static uint8_t options[] = {1, 2};
  return get_enum_number(options, 2, value, "pdsch_cfg_ded_v1430_s::ce_sched_enhancement_r14_e_");
}

std::string pdsch_cfg_ded_v1530_s::alt_mcs_table_scaling_cfg_r15_e_::to_string() const
{
  switch (value) {
    case o_dot5:
      return "oDot5";
    case o_dot625:
      return "oDot625";
    case o_dot75:
      return "oDot75";
    case o_dot875:
      return "oDot875";
    default:
      invalid_enum_value(value, "pdsch_cfg_ded_v1530_s::alt_mcs_table_scaling_cfg_r15_e_");
  }
  return "";
}
float pdsch_cfg_ded_v1530_s::alt_mcs_table_scaling_cfg_r15_e_::to_number() const
{
  const static float options[] = {0.5, 0.625, 0.75, 0.875};
  return get_enum_number(options, 4, value, "pdsch_cfg_ded_v1530_s::alt_mcs_table_scaling_cfg_r15_e_");
}
std::string pdsch_cfg_ded_v1530_s::alt_mcs_table_scaling_cfg_r15_e_::to_number_string() const
{
  switch (value) {
    case o_dot5:
      return "0.5";
    case o_dot625:
      return "0.625";
    case o_dot75:
      return "0.75";
    case o_dot875:
      return "0.875";
    default:
      invalid_enum_number(value, "pdsch_cfg_ded_v1530_s::alt_mcs_table_scaling_cfg_r15_e_");
  }
  return "";
}

std::string pucch_cfg_ded_s::ack_nack_repeat_c_::setup_s_::repeat_factor_e_::to_string() const
{
  switch (value) {
    case n2:
      return "n2";
    case n4:
      return "n4";
    case n6:
      return "n6";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "pucch_cfg_ded_s::ack_nack_repeat_c_::setup_s_::repeat_factor_e_");
  }
  return "";
}
uint8_t pucch_cfg_ded_s::ack_nack_repeat_c_::setup_s_::repeat_factor_e_::to_number() const
{
  const static uint8_t options[] = {2, 4, 6};
  return get_enum_number(options, 3, value, "pucch_cfg_ded_s::ack_nack_repeat_c_::setup_s_::repeat_factor_e_");
}

std::string pucch_cfg_ded_s::tdd_ack_nack_feedback_mode_e_::to_string() const
{
  switch (value) {
    case bundling:
      return "bundling";
    case mux:
      return "multiplexing";
    default:
      invalid_enum_value(value, "pucch_cfg_ded_s::tdd_ack_nack_feedback_mode_e_");
  }
  return "";
}

std::string pucch_cfg_ded_r13_s::ack_nack_repeat_r13_c_::setup_s_::repeat_factor_r13_e_::to_string() const
{
  switch (value) {
    case n2:
      return "n2";
    case n4:
      return "n4";
    case n6:
      return "n6";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "pucch_cfg_ded_r13_s::ack_nack_repeat_r13_c_::setup_s_::repeat_factor_r13_e_");
  }
  return "";
}
uint8_t pucch_cfg_ded_r13_s::ack_nack_repeat_r13_c_::setup_s_::repeat_factor_r13_e_::to_number() const
{
  const static uint8_t options[] = {2, 4, 6};
  return get_enum_number(options, 3, value,
                         "pucch_cfg_ded_r13_s::ack_nack_repeat_r13_c_::setup_s_::repeat_factor_r13_e_");
}

std::string pucch_cfg_ded_r13_s::tdd_ack_nack_feedback_mode_r13_e_::to_string() const
{
  switch (value) {
    case bundling:
      return "bundling";
    case mux:
      return "multiplexing";
    default:
      invalid_enum_value(value, "pucch_cfg_ded_r13_s::tdd_ack_nack_feedback_mode_r13_e_");
  }
  return "";
}

std::string pucch_cfg_ded_r13_s::pucch_format_r13_c_::types::to_string() const
{
  switch (value) {
    case format3_r13:
      return "format3-r13";
    case ch_sel_r13:
      return "channelSelection-r13";
    case format4_r13:
      return "format4-r13";
    case format5_r13:
      return "format5-r13";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "pucch_cfg_ded_r13_s::pucch_format_r13_c_::types");
  }
  return "";
}
uint8_t pucch_cfg_ded_r13_s::pucch_format_r13_c_::types::to_number() const
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

std::string pucch_cfg_ded_r13_s::codebooksize_determination_r13_e_::to_string() const
{
  switch (value) {
    case dai:
      return "dai";
    case cc:
      return "cc";
    default:
      invalid_enum_value(value, "pucch_cfg_ded_r13_s::codebooksize_determination_r13_e_");
  }
  return "";
}

std::string
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_a_s_::pucch_num_repeat_ce_format1_r13_e_::to_string()
    const
{
  switch (value) {
    case r1:
      return "r1";
    case r2:
      return "r2";
    case r4:
      return "r4";
    case r8:
      return "r8";
    default:
      invalid_enum_value(
          value,
          "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_a_s_::pucch_num_repeat_ce_format1_r13_e_");
  }
  return "";
}
uint8_t
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_a_s_::pucch_num_repeat_ce_format1_r13_e_::to_number()
    const
{
  const static uint8_t options[] = {1, 2, 4, 8};
  return get_enum_number(
      options, 4, value,
      "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_a_s_::pucch_num_repeat_ce_format1_r13_e_");
}

std::string
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_a_s_::pucch_num_repeat_ce_format2_r13_e_::to_string()
    const
{
  switch (value) {
    case r1:
      return "r1";
    case r2:
      return "r2";
    case r4:
      return "r4";
    case r8:
      return "r8";
    default:
      invalid_enum_value(
          value,
          "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_a_s_::pucch_num_repeat_ce_format2_r13_e_");
  }
  return "";
}
uint8_t
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_a_s_::pucch_num_repeat_ce_format2_r13_e_::to_number()
    const
{
  const static uint8_t options[] = {1, 2, 4, 8};
  return get_enum_number(
      options, 4, value,
      "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_a_s_::pucch_num_repeat_ce_format2_r13_e_");
}

std::string
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_b_s_::pucch_num_repeat_ce_format1_r13_e_::to_string()
    const
{
  switch (value) {
    case r4:
      return "r4";
    case r8:
      return "r8";
    case r16:
      return "r16";
    case r32:
      return "r32";
    default:
      invalid_enum_value(
          value,
          "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_b_s_::pucch_num_repeat_ce_format1_r13_e_");
  }
  return "";
}
uint8_t
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_b_s_::pucch_num_repeat_ce_format1_r13_e_::to_number()
    const
{
  const static uint8_t options[] = {4, 8, 16, 32};
  return get_enum_number(
      options, 4, value,
      "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_b_s_::pucch_num_repeat_ce_format1_r13_e_");
}

std::string
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_b_s_::pucch_num_repeat_ce_format2_r13_e_::to_string()
    const
{
  switch (value) {
    case r4:
      return "r4";
    case r8:
      return "r8";
    case r16:
      return "r16";
    case r32:
      return "r32";
    default:
      invalid_enum_value(
          value,
          "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_b_s_::pucch_num_repeat_ce_format2_r13_e_");
  }
  return "";
}
uint8_t
pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_b_s_::pucch_num_repeat_ce_format2_r13_e_::to_number()
    const
{
  const static uint8_t options[] = {4, 8, 16, 32};
  return get_enum_number(
      options, 4, value,
      "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::mode_b_s_::pucch_num_repeat_ce_format2_r13_e_");
}

std::string pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::types::to_string() const
{
  switch (value) {
    case mode_a:
      return "modeA";
    case mode_b:
      return "modeB";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "pucch_cfg_ded_r13_s::pucch_num_repeat_ce_r13_c_::setup_c_::types");
  }
  return "";
}

std::string pucch_cfg_ded_v1020_s::pucch_format_r10_c_::types::to_string() const
{
  switch (value) {
    case format3_r10:
      return "format3-r10";
    case ch_sel_r10:
      return "channelSelection-r10";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "pucch_cfg_ded_v1020_s::pucch_format_r10_c_::types");
  }
  return "";
}
uint8_t pucch_cfg_ded_v1020_s::pucch_format_r10_c_::types::to_number() const
{
  const static uint8_t options[] = {3};
  return get_enum_number(options, 1, value, "pucch_cfg_ded_v1020_s::pucch_format_r10_c_::types");
}

std::string pucch_cfg_ded_v1430_s::pucch_num_repeat_ce_format1_r14_e_::to_string() const
{
  switch (value) {
    case r64:
      return "r64";
    case r128:
      return "r128";
    default:
      invalid_enum_value(value, "pucch_cfg_ded_v1430_s::pucch_num_repeat_ce_format1_r14_e_");
  }
  return "";
}
uint8_t pucch_cfg_ded_v1430_s::pucch_num_repeat_ce_format1_r14_e_::to_number() const
{
  const static uint8_t options[] = {64, 128};
  return get_enum_number(options, 2, value, "pucch_cfg_ded_v1430_s::pucch_num_repeat_ce_format1_r14_e_");
}

std::string
pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::interv_fdd_pusch_enh_r14_e_::to_string() const
{
  switch (value) {
    case int1:
      return "int1";
    case int2:
      return "int2";
    case int4:
      return "int4";
    case int8:
      return "int8";
    default:
      invalid_enum_value(
          value, "pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::interv_fdd_pusch_enh_r14_e_");
  }
  return "";
}
uint8_t
pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::interv_fdd_pusch_enh_r14_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 4, 8};
  return get_enum_number(
      options, 4, value,
      "pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::interv_fdd_pusch_enh_r14_e_");
}

std::string
pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::interv_tdd_pusch_enh_r14_e_::to_string() const
{
  switch (value) {
    case int1:
      return "int1";
    case int5:
      return "int5";
    case int10:
      return "int10";
    case int20:
      return "int20";
    default:
      invalid_enum_value(
          value, "pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::interv_tdd_pusch_enh_r14_e_");
  }
  return "";
}
uint8_t
pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::interv_tdd_pusch_enh_r14_e_::to_number() const
{
  const static uint8_t options[] = {1, 5, 10, 20};
  return get_enum_number(
      options, 4, value,
      "pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::interv_tdd_pusch_enh_r14_e_");
}

std::string pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::types::to_string() const
{
  switch (value) {
    case interv_fdd_pusch_enh_r14:
      return "interval-FDD-PUSCH-Enh-r14";
    case interv_tdd_pusch_enh_r14:
      return "interval-TDD-PUSCH-Enh-r14";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "pusch_enhance_cfg_r14_c::setup_s_::interv_ul_hop_pusch_enh_r14_c_::types");
  }
  return "";
}

// PeriodicBSR-Timer-r12 ::= ENUMERATED
std::string periodic_bsr_timer_r12_e::to_string() const
{
  switch (value) {
    case sf5:
      return "sf5";
    case sf10:
      return "sf10";
    case sf16:
      return "sf16";
    case sf20:
      return "sf20";
    case sf32:
      return "sf32";
    case sf40:
      return "sf40";
    case sf64:
      return "sf64";
    case sf80:
      return "sf80";
    case sf128:
      return "sf128";
    case sf160:
      return "sf160";
    case sf320:
      return "sf320";
    case sf640:
      return "sf640";
    case sf1280:
      return "sf1280";
    case sf2560:
      return "sf2560";
    case infinity:
      return "infinity";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "periodic_bsr_timer_r12_e");
  }
  return "";
}
int16_t periodic_bsr_timer_r12_e::to_number() const
{
  const static int16_t options[] = {5, 10, 16, 20, 32, 40, 64, 80, 128, 160, 320, 640, 1280, 2560, -1};
  return get_enum_number(options, 15, value, "periodic_bsr_timer_r12_e");
}

std::string rrc_conn_reject_v1130_ies_s::depriorit_req_r11_s_::depriorit_type_r11_e_::to_string() const
{
  switch (value) {
    case freq:
      return "frequency";
    case e_utra:
      return "e-utra";
    default:
      invalid_enum_value(value, "rrc_conn_reject_v1130_ies_s::depriorit_req_r11_s_::depriorit_type_r11_e_");
  }
  return "";
}

std::string rrc_conn_reject_v1130_ies_s::depriorit_req_r11_s_::depriorit_timer_r11_e_::to_string() const
{
  switch (value) {
    case min5:
      return "min5";
    case min10:
      return "min10";
    case min15:
      return "min15";
    case min30:
      return "min30";
    default:
      invalid_enum_value(value, "rrc_conn_reject_v1130_ies_s::depriorit_req_r11_s_::depriorit_timer_r11_e_");
  }
  return "";
}
uint8_t rrc_conn_reject_v1130_ies_s::depriorit_req_r11_s_::depriorit_timer_r11_e_::to_number() const
{
  const static uint8_t options[] = {5, 10, 15, 30};
  return get_enum_number(options, 4, value,
                         "rrc_conn_reject_v1130_ies_s::depriorit_req_r11_s_::depriorit_timer_r11_e_");
}

// RetxBSR-Timer-r12 ::= ENUMERATED
std::string retx_bsr_timer_r12_e::to_string() const
{
  switch (value) {
    case sf320:
      return "sf320";
    case sf640:
      return "sf640";
    case sf1280:
      return "sf1280";
    case sf2560:
      return "sf2560";
    case sf5120:
      return "sf5120";
    case sf10240:
      return "sf10240";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "retx_bsr_timer_r12_e");
  }
  return "";
}
uint16_t retx_bsr_timer_r12_e::to_number() const
{
  const static uint16_t options[] = {320, 640, 1280, 2560, 5120, 10240};
  return get_enum_number(options, 6, value, "retx_bsr_timer_r12_e");
}

std::string sps_cfg_dl_c::setup_s_::semi_persist_sched_interv_dl_e_::to_string() const
{
  switch (value) {
    case sf10:
      return "sf10";
    case sf20:
      return "sf20";
    case sf32:
      return "sf32";
    case sf40:
      return "sf40";
    case sf64:
      return "sf64";
    case sf80:
      return "sf80";
    case sf128:
      return "sf128";
    case sf160:
      return "sf160";
    case sf320:
      return "sf320";
    case sf640:
      return "sf640";
    case spare6:
      return "spare6";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "sps_cfg_dl_c::setup_s_::semi_persist_sched_interv_dl_e_");
  }
  return "";
}
uint16_t sps_cfg_dl_c::setup_s_::semi_persist_sched_interv_dl_e_::to_number() const
{
  const static uint16_t options[] = {10, 20, 32, 40, 64, 80, 128, 160, 320, 640};
  return get_enum_number(options, 10, value, "sps_cfg_dl_c::setup_s_::semi_persist_sched_interv_dl_e_");
}

std::string srb_to_add_mod_s::rlc_cfg_c_::types::to_string() const
{
  switch (value) {
    case explicit_value:
      return "explicitValue";
    case default_value:
      return "defaultValue";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "srb_to_add_mod_s::rlc_cfg_c_::types");
  }
  return "";
}

std::string srb_to_add_mod_s::lc_ch_cfg_c_::types::to_string() const
{
  switch (value) {
    case explicit_value:
      return "explicitValue";
    case default_value:
      return "defaultValue";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "srb_to_add_mod_s::lc_ch_cfg_c_::types");
  }
  return "";
}

std::string sched_request_cfg_c::setup_s_::dsr_trans_max_e_::to_string() const
{
  switch (value) {
    case n4:
      return "n4";
    case n8:
      return "n8";
    case n16:
      return "n16";
    case n32:
      return "n32";
    case n64:
      return "n64";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "sched_request_cfg_c::setup_s_::dsr_trans_max_e_");
  }
  return "";
}
uint8_t sched_request_cfg_c::setup_s_::dsr_trans_max_e_::to_number() const
{
  const static uint8_t options[] = {4, 8, 16, 32, 64};
  return get_enum_number(options, 5, value, "sched_request_cfg_c::setup_s_::dsr_trans_max_e_");
}

std::string srs_ul_cfg_ded_c::setup_s_::srs_bw_e_::to_string() const
{
  switch (value) {
    case bw0:
      return "bw0";
    case bw1:
      return "bw1";
    case bw2:
      return "bw2";
    case bw3:
      return "bw3";
    default:
      invalid_enum_value(value, "srs_ul_cfg_ded_c::setup_s_::srs_bw_e_");
  }
  return "";
}
uint8_t srs_ul_cfg_ded_c::setup_s_::srs_bw_e_::to_number() const
{
  const static uint8_t options[] = {0, 1, 2, 3};
  return get_enum_number(options, 4, value, "srs_ul_cfg_ded_c::setup_s_::srs_bw_e_");
}

std::string srs_ul_cfg_ded_c::setup_s_::srs_hop_bw_e_::to_string() const
{
  switch (value) {
    case hbw0:
      return "hbw0";
    case hbw1:
      return "hbw1";
    case hbw2:
      return "hbw2";
    case hbw3:
      return "hbw3";
    default:
      invalid_enum_value(value, "srs_ul_cfg_ded_c::setup_s_::srs_hop_bw_e_");
  }
  return "";
}
uint8_t srs_ul_cfg_ded_c::setup_s_::srs_hop_bw_e_::to_number() const
{
  const static uint8_t options[] = {0, 1, 2, 3};
  return get_enum_number(options, 4, value, "srs_ul_cfg_ded_c::setup_s_::srs_hop_bw_e_");
}

std::string srs_ul_cfg_ded_c::setup_s_::cyclic_shift_e_::to_string() const
{
  switch (value) {
    case cs0:
      return "cs0";
    case cs1:
      return "cs1";
    case cs2:
      return "cs2";
    case cs3:
      return "cs3";
    case cs4:
      return "cs4";
    case cs5:
      return "cs5";
    case cs6:
      return "cs6";
    case cs7:
      return "cs7";
    default:
      invalid_enum_value(value, "srs_ul_cfg_ded_c::setup_s_::cyclic_shift_e_");
  }
  return "";
}
uint8_t srs_ul_cfg_ded_c::setup_s_::cyclic_shift_e_::to_number() const
{
  const static uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7};
  return get_enum_number(options, 8, value, "srs_ul_cfg_ded_c::setup_s_::cyclic_shift_e_");
}

std::string srs_ul_cfg_ded_v1310_c::setup_s_::cyclic_shift_v1310_e_::to_string() const
{
  switch (value) {
    case cs8:
      return "cs8";
    case cs9:
      return "cs9";
    case cs10:
      return "cs10";
    case cs11:
      return "cs11";
    default:
      invalid_enum_value(value, "srs_ul_cfg_ded_v1310_c::setup_s_::cyclic_shift_v1310_e_");
  }
  return "";
}
uint8_t srs_ul_cfg_ded_v1310_c::setup_s_::cyclic_shift_v1310_e_::to_number() const
{
  const static uint8_t options[] = {8, 9, 10, 11};
  return get_enum_number(options, 4, value, "srs_ul_cfg_ded_v1310_c::setup_s_::cyclic_shift_v1310_e_");
}

std::string srs_ul_cfg_ded_v1310_c::setup_s_::tx_comb_num_r13_e_::to_string() const
{
  switch (value) {
    case n2:
      return "n2";
    case n4:
      return "n4";
    default:
      invalid_enum_value(value, "srs_ul_cfg_ded_v1310_c::setup_s_::tx_comb_num_r13_e_");
  }
  return "";
}
uint8_t srs_ul_cfg_ded_v1310_c::setup_s_::tx_comb_num_r13_e_::to_number() const
{
  const static uint8_t options[] = {2, 4};
  return get_enum_number(options, 2, value, "srs_ul_cfg_ded_v1310_c::setup_s_::tx_comb_num_r13_e_");
}

std::string srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::setup_s_::srs_up_pts_add_r13_e_::to_string() const
{
  switch (value) {
    case sym2:
      return "sym2";
    case sym4:
      return "sym4";
    default:
      invalid_enum_value(value, "srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::setup_s_::srs_up_pts_add_r13_e_");
  }
  return "";
}
uint8_t srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::setup_s_::srs_up_pts_add_r13_e_::to_number() const
{
  const static uint8_t options[] = {2, 4};
  return get_enum_number(options, 2, value,
                         "srs_ul_cfg_ded_aperiodic_up_pts_ext_r13_c::setup_s_::srs_up_pts_add_r13_e_");
}

std::string srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_up_pts_add_r13_e_::to_string() const
{
  switch (value) {
    case sym2:
      return "sym2";
    case sym4:
      return "sym4";
    default:
      invalid_enum_value(value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_up_pts_add_r13_e_");
  }
  return "";
}
uint8_t srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_up_pts_add_r13_e_::to_number() const
{
  const static uint8_t options[] = {2, 4};
  return get_enum_number(options, 2, value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_up_pts_add_r13_e_");
}

std::string srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_bw_r13_e_::to_string() const
{
  switch (value) {
    case bw0:
      return "bw0";
    case bw1:
      return "bw1";
    case bw2:
      return "bw2";
    case bw3:
      return "bw3";
    default:
      invalid_enum_value(value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_bw_r13_e_");
  }
  return "";
}
uint8_t srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_bw_r13_e_::to_number() const
{
  const static uint8_t options[] = {0, 1, 2, 3};
  return get_enum_number(options, 4, value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_bw_r13_e_");
}

std::string srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_hop_bw_r13_e_::to_string() const
{
  switch (value) {
    case hbw0:
      return "hbw0";
    case hbw1:
      return "hbw1";
    case hbw2:
      return "hbw2";
    case hbw3:
      return "hbw3";
    default:
      invalid_enum_value(value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_hop_bw_r13_e_");
  }
  return "";
}
uint8_t srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_hop_bw_r13_e_::to_number() const
{
  const static uint8_t options[] = {0, 1, 2, 3};
  return get_enum_number(options, 4, value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::srs_hop_bw_r13_e_");
}

std::string srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::cyclic_shift_r13_e_::to_string() const
{
  switch (value) {
    case cs0:
      return "cs0";
    case cs1:
      return "cs1";
    case cs2:
      return "cs2";
    case cs3:
      return "cs3";
    case cs4:
      return "cs4";
    case cs5:
      return "cs5";
    case cs6:
      return "cs6";
    case cs7:
      return "cs7";
    case cs8:
      return "cs8";
    case cs9:
      return "cs9";
    case cs10:
      return "cs10";
    case cs11:
      return "cs11";
    default:
      invalid_enum_value(value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::cyclic_shift_r13_e_");
  }
  return "";
}
uint8_t srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::cyclic_shift_r13_e_::to_number() const
{
  const static uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  return get_enum_number(options, 12, value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::cyclic_shift_r13_e_");
}

std::string srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::tx_comb_num_r13_e_::to_string() const
{
  switch (value) {
    case n2:
      return "n2";
    case n4:
      return "n4";
    default:
      invalid_enum_value(value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::tx_comb_num_r13_e_");
  }
  return "";
}
uint8_t srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::tx_comb_num_r13_e_::to_number() const
{
  const static uint8_t options[] = {2, 4};
  return get_enum_number(options, 2, value, "srs_ul_cfg_ded_up_pts_ext_r13_c::setup_s_::tx_comb_num_r13_e_");
}

std::string ul_pwr_ctrl_ded_s::delta_mcs_enabled_e_::to_string() const
{
  switch (value) {
    case en0:
      return "en0";
    case en1:
      return "en1";
    default:
      invalid_enum_value(value, "ul_pwr_ctrl_ded_s::delta_mcs_enabled_e_");
  }
  return "";
}
uint8_t ul_pwr_ctrl_ded_s::delta_mcs_enabled_e_::to_number() const
{
  const static uint8_t options[] = {0, 1};
  return get_enum_number(options, 2, value, "ul_pwr_ctrl_ded_s::delta_mcs_enabled_e_");
}

std::string mac_main_cfg_s::ul_sch_cfg_s_::max_harq_tx_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n2:
      return "n2";
    case n3:
      return "n3";
    case n4:
      return "n4";
    case n5:
      return "n5";
    case n6:
      return "n6";
    case n7:
      return "n7";
    case n8:
      return "n8";
    case n10:
      return "n10";
    case n12:
      return "n12";
    case n16:
      return "n16";
    case n20:
      return "n20";
    case n24:
      return "n24";
    case n28:
      return "n28";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "mac_main_cfg_s::ul_sch_cfg_s_::max_harq_tx_e_");
  }
  return "";
}
uint8_t mac_main_cfg_s::ul_sch_cfg_s_::max_harq_tx_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 16, 20, 24, 28};
  return get_enum_number(options, 14, value, "mac_main_cfg_s::ul_sch_cfg_s_::max_harq_tx_e_");
}

std::string mac_main_cfg_s::phr_cfg_c_::setup_s_::periodic_phr_timer_e_::to_string() const
{
  switch (value) {
    case sf10:
      return "sf10";
    case sf20:
      return "sf20";
    case sf50:
      return "sf50";
    case sf100:
      return "sf100";
    case sf200:
      return "sf200";
    case sf500:
      return "sf500";
    case sf1000:
      return "sf1000";
    case infinity:
      return "infinity";
    default:
      invalid_enum_value(value, "mac_main_cfg_s::phr_cfg_c_::setup_s_::periodic_phr_timer_e_");
  }
  return "";
}
int16_t mac_main_cfg_s::phr_cfg_c_::setup_s_::periodic_phr_timer_e_::to_number() const
{
  const static int16_t options[] = {10, 20, 50, 100, 200, 500, 1000, -1};
  return get_enum_number(options, 8, value, "mac_main_cfg_s::phr_cfg_c_::setup_s_::periodic_phr_timer_e_");
}

std::string mac_main_cfg_s::phr_cfg_c_::setup_s_::prohibit_phr_timer_e_::to_string() const
{
  switch (value) {
    case sf0:
      return "sf0";
    case sf10:
      return "sf10";
    case sf20:
      return "sf20";
    case sf50:
      return "sf50";
    case sf100:
      return "sf100";
    case sf200:
      return "sf200";
    case sf500:
      return "sf500";
    case sf1000:
      return "sf1000";
    default:
      invalid_enum_value(value, "mac_main_cfg_s::phr_cfg_c_::setup_s_::prohibit_phr_timer_e_");
  }
  return "";
}
uint16_t mac_main_cfg_s::phr_cfg_c_::setup_s_::prohibit_phr_timer_e_::to_number() const
{
  const static uint16_t options[] = {0, 10, 20, 50, 100, 200, 500, 1000};
  return get_enum_number(options, 8, value, "mac_main_cfg_s::phr_cfg_c_::setup_s_::prohibit_phr_timer_e_");
}

std::string mac_main_cfg_s::phr_cfg_c_::setup_s_::dl_pathloss_change_e_::to_string() const
{
  switch (value) {
    case db1:
      return "dB1";
    case db3:
      return "dB3";
    case db6:
      return "dB6";
    case infinity:
      return "infinity";
    default:
      invalid_enum_value(value, "mac_main_cfg_s::phr_cfg_c_::setup_s_::dl_pathloss_change_e_");
  }
  return "";
}
int8_t mac_main_cfg_s::phr_cfg_c_::setup_s_::dl_pathloss_change_e_::to_number() const
{
  const static int8_t options[] = {1, 3, 6, -1};
  return get_enum_number(options, 4, value, "mac_main_cfg_s::phr_cfg_c_::setup_s_::dl_pathloss_change_e_");
}

std::string mac_main_cfg_s::mac_main_cfg_v1020_s_::s_cell_deactivation_timer_r10_e_::to_string() const
{
  switch (value) {
    case rf2:
      return "rf2";
    case rf4:
      return "rf4";
    case rf8:
      return "rf8";
    case rf16:
      return "rf16";
    case rf32:
      return "rf32";
    case rf64:
      return "rf64";
    case rf128:
      return "rf128";
    case spare:
      return "spare";
    default:
      invalid_enum_value(value, "mac_main_cfg_s::mac_main_cfg_v1020_s_::s_cell_deactivation_timer_r10_e_");
  }
  return "";
}
uint8_t mac_main_cfg_s::mac_main_cfg_v1020_s_::s_cell_deactivation_timer_r10_e_::to_number() const
{
  const static uint8_t options[] = {2, 4, 8, 16, 32, 64, 128};
  return get_enum_number(options, 7, value, "mac_main_cfg_s::mac_main_cfg_v1020_s_::s_cell_deactivation_timer_r10_e_");
}

std::string mac_main_cfg_s::dual_connect_phr_c_::setup_s_::phr_mode_other_cg_r12_e_::to_string() const
{
  switch (value) {
    case real:
      return "real";
    case virtual_type:
      return "virtual";
    default:
      invalid_enum_value(value, "mac_main_cfg_s::dual_connect_phr_c_::setup_s_::phr_mode_other_cg_r12_e_");
  }
  return "";
}

std::string mac_main_cfg_s::lc_ch_sr_cfg_r12_c_::setup_s_::lc_ch_sr_prohibit_timer_r12_e_::to_string() const
{
  switch (value) {
    case sf20:
      return "sf20";
    case sf40:
      return "sf40";
    case sf64:
      return "sf64";
    case sf128:
      return "sf128";
    case sf512:
      return "sf512";
    case sf1024:
      return "sf1024";
    case sf2560:
      return "sf2560";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "mac_main_cfg_s::lc_ch_sr_cfg_r12_c_::setup_s_::lc_ch_sr_prohibit_timer_r12_e_");
  }
  return "";
}
uint16_t mac_main_cfg_s::lc_ch_sr_cfg_r12_c_::setup_s_::lc_ch_sr_prohibit_timer_r12_e_::to_number() const
{
  const static uint16_t options[] = {20, 40, 64, 128, 512, 1024, 2560};
  return get_enum_number(options, 7, value,
                         "mac_main_cfg_s::lc_ch_sr_cfg_r12_c_::setup_s_::lc_ch_sr_prohibit_timer_r12_e_");
}

std::string mac_main_cfg_s::e_drx_cfg_cycle_start_offset_r13_c_::setup_c_::types::to_string() const
{
  switch (value) {
    case sf5120:
      return "sf5120";
    case sf10240:
      return "sf10240";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "mac_main_cfg_s::e_drx_cfg_cycle_start_offset_r13_c_::setup_c_::types");
  }
  return "";
}
uint16_t mac_main_cfg_s::e_drx_cfg_cycle_start_offset_r13_c_::setup_c_::types::to_number() const
{
  const static uint16_t options[] = {5120, 10240};
  return get_enum_number(options, 2, value, "mac_main_cfg_s::e_drx_cfg_cycle_start_offset_r13_c_::setup_c_::types");
}

std::string mac_main_cfg_s::short_tti_and_spt_r15_c_::setup_s_::periodic_bsr_timer_r15_e_::to_string() const
{
  switch (value) {
    case sf1:
      return "sf1";
    case sf5:
      return "sf5";
    case sf10:
      return "sf10";
    case sf16:
      return "sf16";
    case sf20:
      return "sf20";
    case sf32:
      return "sf32";
    case sf40:
      return "sf40";
    case sf64:
      return "sf64";
    case sf80:
      return "sf80";
    case sf128:
      return "sf128";
    case sf160:
      return "sf160";
    case sf320:
      return "sf320";
    case sf640:
      return "sf640";
    case sf1280:
      return "sf1280";
    case sf2560:
      return "sf2560";
    case infinity:
      return "infinity";
    default:
      invalid_enum_value(value, "mac_main_cfg_s::short_tti_and_spt_r15_c_::setup_s_::periodic_bsr_timer_r15_e_");
  }
  return "";
}
int16_t mac_main_cfg_s::short_tti_and_spt_r15_c_::setup_s_::periodic_bsr_timer_r15_e_::to_number() const
{
  const static int16_t options[] = {1, 5, 10, 16, 20, 32, 40, 64, 80, 128, 160, 320, 640, 1280, 2560, -1};
  return get_enum_number(options, 16, value,
                         "mac_main_cfg_s::short_tti_and_spt_r15_c_::setup_s_::periodic_bsr_timer_r15_e_");
}

std::string mac_main_cfg_s::short_tti_and_spt_r15_c_::setup_s_::proc_timeline_r15_e_::to_string() const
{
  switch (value) {
    case nplus4set1:
      return "nplus4set1";
    case nplus6set1:
      return "nplus6set1";
    case nplus6set2:
      return "nplus6set2";
    case nplus8set2:
      return "nplus8set2";
    default:
      invalid_enum_value(value, "mac_main_cfg_s::short_tti_and_spt_r15_c_::setup_s_::proc_timeline_r15_e_");
  }
  return "";
}

std::string mac_main_cfg_s::dormant_state_timers_r15_c_::setup_s_::s_cell_hibernation_timer_r15_e_::to_string() const
{
  switch (value) {
    case rf2:
      return "rf2";
    case rf4:
      return "rf4";
    case rf8:
      return "rf8";
    case rf16:
      return "rf16";
    case rf32:
      return "rf32";
    case rf64:
      return "rf64";
    case rf128:
      return "rf128";
    case spare:
      return "spare";
    default:
      invalid_enum_value(value,
                         "mac_main_cfg_s::dormant_state_timers_r15_c_::setup_s_::s_cell_hibernation_timer_r15_e_");
  }
  return "";
}
uint8_t mac_main_cfg_s::dormant_state_timers_r15_c_::setup_s_::s_cell_hibernation_timer_r15_e_::to_number() const
{
  const static uint8_t options[] = {2, 4, 8, 16, 32, 64, 128};
  return get_enum_number(options, 7, value,
                         "mac_main_cfg_s::dormant_state_timers_r15_c_::setup_s_::s_cell_hibernation_timer_r15_e_");
}

std::string
mac_main_cfg_s::dormant_state_timers_r15_c_::setup_s_::dormant_scell_deactivation_timer_r15_e_::to_string() const
{
  switch (value) {
    case rf2:
      return "rf2";
    case rf4:
      return "rf4";
    case rf8:
      return "rf8";
    case rf16:
      return "rf16";
    case rf32:
      return "rf32";
    case rf64:
      return "rf64";
    case rf128:
      return "rf128";
    case rf320:
      return "rf320";
    case rf640:
      return "rf640";
    case rf1280:
      return "rf1280";
    case rf2560:
      return "rf2560";
    case rf5120:
      return "rf5120";
    case rf10240:
      return "rf10240";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(
          value, "mac_main_cfg_s::dormant_state_timers_r15_c_::setup_s_::dormant_scell_deactivation_timer_r15_e_");
  }
  return "";
}
uint16_t
mac_main_cfg_s::dormant_state_timers_r15_c_::setup_s_::dormant_scell_deactivation_timer_r15_e_::to_number() const
{
  const static uint16_t options[] = {2, 4, 8, 16, 32, 64, 128, 320, 640, 1280, 2560, 5120, 10240};
  return get_enum_number(
      options, 13, value,
      "mac_main_cfg_s::dormant_state_timers_r15_c_::setup_s_::dormant_scell_deactivation_timer_r15_e_");
}

std::string phys_cfg_ded_s::ant_info_c_::types::to_string() const
{
  switch (value) {
    case explicit_value:
      return "explicitValue";
    case default_value:
      return "defaultValue";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "phys_cfg_ded_s::ant_info_c_::types");
  }
  return "";
}

std::string phys_cfg_ded_s::ant_info_r10_c_::types::to_string() const
{
  switch (value) {
    case explicit_value_r10:
      return "explicitValue-r10";
    case default_value:
      return "defaultValue";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "phys_cfg_ded_s::ant_info_r10_c_::types");
  }
  return "";
}

std::string phys_cfg_ded_s::ce_mode_r13_c_::setup_e_::to_string() const
{
  switch (value) {
    case ce_mode_a:
      return "ce-ModeA";
    case ce_mode_b:
      return "ce-ModeB";
    default:
      invalid_enum_value(value, "phys_cfg_ded_s::ce_mode_r13_c_::setup_e_");
  }
  return "";
}

std::string phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::k_max_r14_e_::to_string() const
{
  switch (value) {
    case l1:
      return "l1";
    case l3:
      return "l3";
    default:
      invalid_enum_value(value, "phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::k_max_r14_e_");
  }
  return "";
}
uint8_t phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::k_max_r14_e_::to_number() const
{
  const static uint8_t options[] = {1, 3};
  return get_enum_number(options, 2, value, "phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::k_max_r14_e_");
}

std::string phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_e_::to_string() const
{
  switch (value) {
    case db_minus6:
      return "dB-6";
    case db_minus4dot77:
      return "dB-4dot77";
    case db_minus3:
      return "dB-3";
    case db_minus1dot77:
      return "dB-1dot77";
    case db0:
      return "dB0";
    case db1:
      return "dB1";
    case db2:
      return "dB2";
    case db3:
      return "dB3";
    default:
      invalid_enum_value(value, "phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_e_");
  }
  return "";
}
float phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_e_::to_number() const
{
  const static float options[] = {-6.0, -4.77, -3.0, -1.77, 0.0, 1.0, 2.0, 3.0};
  return get_enum_number(options, 8, value, "phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_e_");
}
std::string phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_e_::to_number_string() const
{
  switch (value) {
    case db_minus6:
      return "-6";
    case db_minus4dot77:
      return "-4.77";
    case db_minus3:
      return "-3";
    case db_minus1dot77:
      return "-1.77";
    case db0:
      return "0";
    case db1:
      return "1";
    case db2:
      return "2";
    case db3:
      return "3";
    default:
      invalid_enum_number(value, "phys_cfg_ded_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_e_");
  }
  return "";
}

std::string phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::setup_c_::types::to_string() const
{
  switch (value) {
    case cfi_cfg_r15:
      return "cfi-Config-r15";
    case cfi_pattern_cfg_r15:
      return "cfi-PatternConfig-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "phys_cfg_ded_s::semi_static_cfi_cfg_r15_c_::setup_c_::types");
  }
  return "";
}

std::string phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_sf_pdsch_repeats_r15_e_::to_string() const
{
  switch (value) {
    case n4:
      return "n4";
    case n6:
      return "n6";
    default:
      invalid_enum_value(value,
                         "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_sf_pdsch_repeats_r15_e_");
  }
  return "";
}
uint8_t phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_sf_pdsch_repeats_r15_e_::to_number() const
{
  const static uint8_t options[] = {4, 6};
  return get_enum_number(options, 2, value,
                         "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_sf_pdsch_repeats_r15_e_");
}

std::string
phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_slot_subslot_pdsch_repeats_r15_e_::to_string() const
{
  switch (value) {
    case n4:
      return "n4";
    case n6:
      return "n6";
    default:
      invalid_enum_value(
          value, "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_slot_subslot_pdsch_repeats_r15_e_");
  }
  return "";
}
uint8_t
phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_slot_subslot_pdsch_repeats_r15_e_::to_number() const
{
  const static uint8_t options[] = {4, 6};
  return get_enum_number(
      options, 2, value,
      "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_slot_subslot_pdsch_repeats_r15_e_");
}

std::string phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_sf_pdsch_repeats_r15_e_::to_string() const
{
  switch (value) {
    case dlrvseq1:
      return "dlrvseq1";
    case dlrvseq2:
      return "dlrvseq2";
    default:
      invalid_enum_value(value, "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_sf_pdsch_repeats_r15_e_");
  }
  return "";
}
uint8_t phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_sf_pdsch_repeats_r15_e_::to_number() const
{
  const static uint8_t options[] = {1, 2};
  return get_enum_number(options, 2, value,
                         "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_sf_pdsch_repeats_r15_e_");
}

std::string
phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_slotsublot_pdsch_repeats_r15_e_::to_string() const
{
  switch (value) {
    case dlrvseq1:
      return "dlrvseq1";
    case dlrvseq2:
      return "dlrvseq2";
    default:
      invalid_enum_value(value,
                         "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_slotsublot_pdsch_repeats_r15_e_");
  }
  return "";
}
uint8_t phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_slotsublot_pdsch_repeats_r15_e_::to_number() const
{
  const static uint8_t options[] = {1, 2};
  return get_enum_number(options, 2, value,
                         "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_slotsublot_pdsch_repeats_r15_e_");
}

std::string
phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_sf_pdsch_repeats_r15_e_::to_string() const
{
  switch (value) {
    case n0:
      return "n0";
    case n1:
      return "n1";
    default:
      invalid_enum_value(
          value, "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_sf_pdsch_repeats_r15_e_");
  }
  return "";
}
uint8_t phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_sf_pdsch_repeats_r15_e_::to_number() const
{
  const static uint8_t options[] = {0, 1};
  return get_enum_number(
      options, 2, value,
      "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_sf_pdsch_repeats_r15_e_");
}

std::string
phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_slot_subslot_pdsch_repeats_r15_e_::to_string()
    const
{
  switch (value) {
    case n0:
      return "n0";
    case n1:
      return "n1";
    default:
      invalid_enum_value(
          value,
          "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_slot_subslot_pdsch_repeats_r15_e_");
  }
  return "";
}
uint8_t
phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_slot_subslot_pdsch_repeats_r15_e_::to_number()
    const
{
  const static uint8_t options[] = {0, 1};
  return get_enum_number(
      options, 2, value,
      "phys_cfg_ded_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_slot_subslot_pdsch_repeats_r15_e_");
}

std::string rlf_timers_and_consts_r13_c::setup_s_::t301_v1310_e_::to_string() const
{
  switch (value) {
    case ms2500:
      return "ms2500";
    case ms3000:
      return "ms3000";
    case ms3500:
      return "ms3500";
    case ms4000:
      return "ms4000";
    case ms5000:
      return "ms5000";
    case ms6000:
      return "ms6000";
    case ms8000:
      return "ms8000";
    case ms10000:
      return "ms10000";
    default:
      invalid_enum_value(value, "rlf_timers_and_consts_r13_c::setup_s_::t301_v1310_e_");
  }
  return "";
}
uint16_t rlf_timers_and_consts_r13_c::setup_s_::t301_v1310_e_::to_number() const
{
  const static uint16_t options[] = {2500, 3000, 3500, 4000, 5000, 6000, 8000, 10000};
  return get_enum_number(options, 8, value, "rlf_timers_and_consts_r13_c::setup_s_::t301_v1310_e_");
}

std::string rlf_timers_and_consts_r13_c::setup_s_::t310_v1330_e_::to_string() const
{
  switch (value) {
    case ms4000:
      return "ms4000";
    case ms6000:
      return "ms6000";
    default:
      invalid_enum_value(value, "rlf_timers_and_consts_r13_c::setup_s_::t310_v1330_e_");
  }
  return "";
}
uint16_t rlf_timers_and_consts_r13_c::setup_s_::t310_v1330_e_::to_number() const
{
  const static uint16_t options[] = {4000, 6000};
  return get_enum_number(options, 2, value, "rlf_timers_and_consts_r13_c::setup_s_::t310_v1330_e_");
}

std::string rlf_timers_and_consts_r9_c::setup_s_::t301_r9_e_::to_string() const
{
  switch (value) {
    case ms100:
      return "ms100";
    case ms200:
      return "ms200";
    case ms300:
      return "ms300";
    case ms400:
      return "ms400";
    case ms600:
      return "ms600";
    case ms1000:
      return "ms1000";
    case ms1500:
      return "ms1500";
    case ms2000:
      return "ms2000";
    default:
      invalid_enum_value(value, "rlf_timers_and_consts_r9_c::setup_s_::t301_r9_e_");
  }
  return "";
}
uint16_t rlf_timers_and_consts_r9_c::setup_s_::t301_r9_e_::to_number() const
{
  const static uint16_t options[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return get_enum_number(options, 8, value, "rlf_timers_and_consts_r9_c::setup_s_::t301_r9_e_");
}

std::string rlf_timers_and_consts_r9_c::setup_s_::t310_r9_e_::to_string() const
{
  switch (value) {
    case ms0:
      return "ms0";
    case ms50:
      return "ms50";
    case ms100:
      return "ms100";
    case ms200:
      return "ms200";
    case ms500:
      return "ms500";
    case ms1000:
      return "ms1000";
    case ms2000:
      return "ms2000";
    default:
      invalid_enum_value(value, "rlf_timers_and_consts_r9_c::setup_s_::t310_r9_e_");
  }
  return "";
}
uint16_t rlf_timers_and_consts_r9_c::setup_s_::t310_r9_e_::to_number() const
{
  const static uint16_t options[] = {0, 50, 100, 200, 500, 1000, 2000};
  return get_enum_number(options, 7, value, "rlf_timers_and_consts_r9_c::setup_s_::t310_r9_e_");
}

std::string rlf_timers_and_consts_r9_c::setup_s_::n310_r9_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n2:
      return "n2";
    case n3:
      return "n3";
    case n4:
      return "n4";
    case n6:
      return "n6";
    case n8:
      return "n8";
    case n10:
      return "n10";
    case n20:
      return "n20";
    default:
      invalid_enum_value(value, "rlf_timers_and_consts_r9_c::setup_s_::n310_r9_e_");
  }
  return "";
}
uint8_t rlf_timers_and_consts_r9_c::setup_s_::n310_r9_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 4, 6, 8, 10, 20};
  return get_enum_number(options, 8, value, "rlf_timers_and_consts_r9_c::setup_s_::n310_r9_e_");
}

std::string rlf_timers_and_consts_r9_c::setup_s_::t311_r9_e_::to_string() const
{
  switch (value) {
    case ms1000:
      return "ms1000";
    case ms3000:
      return "ms3000";
    case ms5000:
      return "ms5000";
    case ms10000:
      return "ms10000";
    case ms15000:
      return "ms15000";
    case ms20000:
      return "ms20000";
    case ms30000:
      return "ms30000";
    default:
      invalid_enum_value(value, "rlf_timers_and_consts_r9_c::setup_s_::t311_r9_e_");
  }
  return "";
}
uint16_t rlf_timers_and_consts_r9_c::setup_s_::t311_r9_e_::to_number() const
{
  const static uint16_t options[] = {1000, 3000, 5000, 10000, 15000, 20000, 30000};
  return get_enum_number(options, 7, value, "rlf_timers_and_consts_r9_c::setup_s_::t311_r9_e_");
}

std::string rlf_timers_and_consts_r9_c::setup_s_::n311_r9_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n2:
      return "n2";
    case n3:
      return "n3";
    case n4:
      return "n4";
    case n5:
      return "n5";
    case n6:
      return "n6";
    case n8:
      return "n8";
    case n10:
      return "n10";
    default:
      invalid_enum_value(value, "rlf_timers_and_consts_r9_c::setup_s_::n311_r9_e_");
  }
  return "";
}
uint8_t rlf_timers_and_consts_r9_c::setup_s_::n311_r9_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 4, 5, 6, 8, 10};
  return get_enum_number(options, 8, value, "rlf_timers_and_consts_r9_c::setup_s_::n311_r9_e_");
}

std::string idle_mode_mob_ctrl_info_s::t320_e_::to_string() const
{
  switch (value) {
    case min5:
      return "min5";
    case min10:
      return "min10";
    case min20:
      return "min20";
    case min30:
      return "min30";
    case min60:
      return "min60";
    case min120:
      return "min120";
    case min180:
      return "min180";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "idle_mode_mob_ctrl_info_s::t320_e_");
  }
  return "";
}
uint8_t idle_mode_mob_ctrl_info_s::t320_e_::to_number() const
{
  const static uint8_t options[] = {5, 10, 20, 30, 60, 120, 180};
  return get_enum_number(options, 7, value, "idle_mode_mob_ctrl_info_s::t320_e_");
}

std::string rr_cfg_ded_s::mac_main_cfg_c_::types::to_string() const
{
  switch (value) {
    case explicit_value:
      return "explicitValue";
    case default_value:
      return "defaultValue";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rr_cfg_ded_s::mac_main_cfg_c_::types");
  }
  return "";
}

std::string rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::crs_intf_mitig_num_prbs_r15_e_::to_string() const
{
  switch (value) {
    case n6:
      return "n6";
    case n24:
      return "n24";
    default:
      invalid_enum_value(value, "rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::crs_intf_mitig_num_prbs_r15_e_");
  }
  return "";
}
uint8_t rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::crs_intf_mitig_num_prbs_r15_e_::to_number() const
{
  const static uint8_t options[] = {6, 24};
  return get_enum_number(options, 2, value,
                         "rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::crs_intf_mitig_num_prbs_r15_e_");
}

std::string rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::types::to_string() const
{
  switch (value) {
    case crs_intf_mitig_enabled_minus15:
      return "crs-IntfMitigEnabled-15";
    case crs_intf_mitig_num_prbs_r15:
      return "crs-IntfMitigNumPRBs-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::types");
  }
  return "";
}
int8_t rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::types::to_number() const
{
  const static int8_t options[] = {-15};
  return get_enum_number(options, 1, value, "rr_cfg_ded_s::crs_intf_mitig_cfg_r15_c_::setup_c_::types");
}

std::string redirected_carrier_info_r15_ies_c::types::to_string() const
{
  switch (value) {
    case eutra_r15:
      return "eutra-r15";
    case geran_r15:
      return "geran-r15";
    case utra_fdd_r15:
      return "utra-FDD-r15";
    case cdma2000_hrpd_r15:
      return "cdma2000-HRPD-r15";
    case cdma2000_minus1x_rtt_r15:
      return "cdma2000-1xRTT-r15";
    case utra_tdd_r15:
      return "utra-TDD-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "redirected_carrier_info_r15_ies_c::types");
  }
  return "";
}

std::string c1_or_crit_ext_e::to_string() const
{
  switch (value) {
    case c1:
      return "c1";
    case crit_exts_future:
      return "criticalExtensionsFuture";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "c1_or_crit_ext_e");
  }
  return "";
}
uint8_t c1_or_crit_ext_e::to_number() const
{
  const static uint8_t options[] = {1};
  return get_enum_number(options, 1, value, "c1_or_crit_ext_e");
}

std::string rrc_conn_reest_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case rrc_conn_reest_r8:
      return "rrcConnectionReestablishment-r8";
    case spare7:
      return "spare7";
    case spare6:
      return "spare6";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rrc_conn_reest_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string rrc_conn_reest_reject_s::crit_exts_c_::types::to_string() const
{
  switch (value) {
    case rrc_conn_reest_reject_r8:
      return "rrcConnectionReestablishmentReject-r8";
    case crit_exts_future:
      return "criticalExtensionsFuture";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rrc_conn_reest_reject_s::crit_exts_c_::types");
  }
  return "";
}

std::string rrc_conn_reject_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case rrc_conn_reject_r8:
      return "rrcConnectionReject-r8";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rrc_conn_reject_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string rrc_conn_setup_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case rrc_conn_setup_r8:
      return "rrcConnectionSetup-r8";
    case spare7:
      return "spare7";
    case spare6:
      return "spare6";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rrc_conn_setup_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string rrc_early_data_complete_r15_s::crit_exts_c_::types::to_string() const
{
  switch (value) {
    case rrc_early_data_complete_r15:
      return "rrcEarlyDataComplete-r15";
    case crit_exts_future:
      return "criticalExtensionsFuture";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rrc_early_data_complete_r15_s::crit_exts_c_::types");
  }
  return "";
}

std::string dl_ccch_msg_type_c::c1_c_::types::to_string() const
{
  switch (value) {
    case rrc_conn_reest:
      return "rrcConnectionReestablishment";
    case rrc_conn_reest_reject:
      return "rrcConnectionReestablishmentReject";
    case rrc_conn_reject:
      return "rrcConnectionReject";
    case rrc_conn_setup:
      return "rrcConnectionSetup";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "dl_ccch_msg_type_c::c1_c_::types");
  }
  return "";
}

std::string dl_ccch_msg_type_c::msg_class_ext_c_::c2_c_::types::to_string() const
{
  switch (value) {
    case rrc_early_data_complete_r15:
      return "rrcEarlyDataComplete-r15";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "dl_ccch_msg_type_c::msg_class_ext_c_::c2_c_::types");
  }
  return "";
}

std::string dl_ccch_msg_type_c::msg_class_ext_c_::types::to_string() const
{
  switch (value) {
    case c2:
      return "c2";
    case msg_class_ext_future_r15:
      return "messageClassExtensionFuture-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "dl_ccch_msg_type_c::msg_class_ext_c_::types");
  }
  return "";
}
uint8_t dl_ccch_msg_type_c::msg_class_ext_c_::types::to_number() const
{
  const static uint8_t options[] = {2};
  return get_enum_number(options, 1, value, "dl_ccch_msg_type_c::msg_class_ext_c_::types");
}

std::string dl_ccch_msg_type_c::types::to_string() const
{
  switch (value) {
    case c1:
      return "c1";
    case msg_class_ext:
      return "messageClassExtension";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "dl_ccch_msg_type_c::types");
  }
  return "";
}
uint8_t dl_ccch_msg_type_c::types::to_number() const
{
  const static uint8_t options[] = {1};
  return get_enum_number(options, 1, value, "dl_ccch_msg_type_c::types");
}

// PDCCH-CandidateReductionValue-r14 ::= ENUMERATED
std::string pdcch_candidate_reduction_value_r14_e::to_string() const
{
  switch (value) {
    case n0:
      return "n0";
    case n50:
      return "n50";
    case n100:
      return "n100";
    case n150:
      return "n150";
    default:
      invalid_enum_value(value, "pdcch_candidate_reduction_value_r14_e");
  }
  return "";
}
uint8_t pdcch_candidate_reduction_value_r14_e::to_number() const
{
  const static uint8_t options[] = {0, 50, 100, 150};
  return get_enum_number(options, 4, value, "pdcch_candidate_reduction_value_r14_e");
}

std::string aul_cfg_r15_c::setup_s_::tx_mode_ul_aul_r15_e_::to_string() const
{
  switch (value) {
    case tm1:
      return "tm1";
    case tm2:
      return "tm2";
    default:
      invalid_enum_value(value, "aul_cfg_r15_c::setup_s_::tx_mode_ul_aul_r15_e_");
  }
  return "";
}
uint8_t aul_cfg_r15_c::setup_s_::tx_mode_ul_aul_r15_e_::to_number() const
{
  const static uint8_t options[] = {1, 2};
  return get_enum_number(options, 2, value, "aul_cfg_r15_c::setup_s_::tx_mode_ul_aul_r15_e_");
}

std::string aul_cfg_r15_c::setup_s_::aul_start_partial_bw_inside_mcot_r15_e_::to_string() const
{
  switch (value) {
    case o34:
      return "o34";
    case o43:
      return "o43";
    case o52:
      return "o52";
    case o61:
      return "o61";
    case o_os1:
      return "oOS1";
    default:
      invalid_enum_value(value, "aul_cfg_r15_c::setup_s_::aul_start_partial_bw_inside_mcot_r15_e_");
  }
  return "";
}
uint8_t aul_cfg_r15_c::setup_s_::aul_start_partial_bw_inside_mcot_r15_e_::to_number() const
{
  const static uint8_t options[] = {34, 43, 52, 61, 1};
  return get_enum_number(options, 5, value, "aul_cfg_r15_c::setup_s_::aul_start_partial_bw_inside_mcot_r15_e_");
}

std::string aul_cfg_r15_c::setup_s_::aul_start_partial_bw_outside_mcot_r15_e_::to_string() const
{
  switch (value) {
    case o16:
      return "o16";
    case o25:
      return "o25";
    case o34:
      return "o34";
    case o43:
      return "o43";
    case o52:
      return "o52";
    case o61:
      return "o61";
    case o_os1:
      return "oOS1";
    default:
      invalid_enum_value(value, "aul_cfg_r15_c::setup_s_::aul_start_partial_bw_outside_mcot_r15_e_");
  }
  return "";
}
uint8_t aul_cfg_r15_c::setup_s_::aul_start_partial_bw_outside_mcot_r15_e_::to_number() const
{
  const static uint8_t options[] = {16, 25, 34, 43, 52, 61, 1};
  return get_enum_number(options, 7, value, "aul_cfg_r15_c::setup_s_::aul_start_partial_bw_outside_mcot_r15_e_");
}

std::string aul_cfg_r15_c::setup_s_::aul_retx_timer_r15_e_::to_string() const
{
  switch (value) {
    case psf4:
      return "psf4";
    case psf5:
      return "psf5";
    case psf6:
      return "psf6";
    case psf8:
      return "psf8";
    case psf10:
      return "psf10";
    case psf12:
      return "psf12";
    case psf20:
      return "psf20";
    case psf28:
      return "psf28";
    case psf37:
      return "psf37";
    case psf44:
      return "psf44";
    case psf68:
      return "psf68";
    case psf84:
      return "psf84";
    case psf100:
      return "psf100";
    case psf116:
      return "psf116";
    case psf132:
      return "psf132";
    case psf164:
      return "psf164";
    case psf324:
      return "psf324";
    default:
      invalid_enum_value(value, "aul_cfg_r15_c::setup_s_::aul_retx_timer_r15_e_");
  }
  return "";
}
uint16_t aul_cfg_r15_c::setup_s_::aul_retx_timer_r15_e_::to_number() const
{
  const static uint16_t options[] = {4, 5, 6, 8, 10, 12, 20, 28, 37, 44, 68, 84, 100, 116, 132, 164, 324};
  return get_enum_number(options, 17, value, "aul_cfg_r15_c::setup_s_::aul_retx_timer_r15_e_");
}

std::string aul_cfg_r15_c::setup_s_::contention_win_size_timer_r15_e_::to_string() const
{
  switch (value) {
    case n0:
      return "n0";
    case n5:
      return "n5";
    case n10:
      return "n10";
    default:
      invalid_enum_value(value, "aul_cfg_r15_c::setup_s_::contention_win_size_timer_r15_e_");
  }
  return "";
}
uint8_t aul_cfg_r15_c::setup_s_::contention_win_size_timer_r15_e_::to_number() const
{
  const static uint8_t options[] = {0, 5, 10};
  return get_enum_number(options, 3, value, "aul_cfg_r15_c::setup_s_::contention_win_size_timer_r15_e_");
}

std::string cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::wideband_cqi_r15_s_::
    csi_report_mode_r15_e_::to_string() const
{
  switch (value) {
    case submode1:
      return "submode1";
    case submode2:
      return "submode2";
    default:
      invalid_enum_value(value, "cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::wideband_"
                                "cqi_r15_s_::csi_report_mode_r15_e_");
  }
  return "";
}
uint8_t cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::wideband_cqi_r15_s_::
    csi_report_mode_r15_e_::to_number() const
{
  const static uint8_t options[] = {1, 2};
  return get_enum_number(options, 2, value,
                         "cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::wideband_cqi_r15_s_"
                         "::csi_report_mode_r15_e_");
}

std::string cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::subband_cqi_r15_s_::
    periodicity_factor_r15_e_::to_string() const
{
  switch (value) {
    case n2:
      return "n2";
    case n4:
      return "n4";
    default:
      invalid_enum_value(value, "cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::subband_cqi_"
                                "r15_s_::periodicity_factor_r15_e_");
  }
  return "";
}
uint8_t cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::subband_cqi_r15_s_::
    periodicity_factor_r15_e_::to_number() const
{
  const static uint8_t options[] = {2, 4};
  return get_enum_number(options, 2, value,
                         "cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::subband_cqi_r15_s_:"
                         ":periodicity_factor_r15_e_");
}

std::string cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::types::to_string() const
{
  switch (value) {
    case wideband_cqi_r15:
      return "widebandCQI-r15";
    case subband_cqi_r15:
      return "subbandCQI-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "cqi_report_periodic_scell_r15_c::setup_s_::cqi_format_ind_dormant_r15_c_::types");
  }
  return "";
}

std::string lbt_cfg_r14_c::types::to_string() const
{
  switch (value) {
    case max_energy_detection_thres_r14:
      return "maxEnergyDetectionThreshold-r14";
    case energy_detection_thres_offset_r14:
      return "energyDetectionThresholdOffset-r14";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "lbt_cfg_r14_c::types");
  }
  return "";
}

std::string pdcch_cfg_laa_r14_s::max_nof_sched_sfs_format0_b_r14_e_::to_string() const
{
  switch (value) {
    case sf2:
      return "sf2";
    case sf3:
      return "sf3";
    case sf4:
      return "sf4";
    default:
      invalid_enum_value(value, "pdcch_cfg_laa_r14_s::max_nof_sched_sfs_format0_b_r14_e_");
  }
  return "";
}
uint8_t pdcch_cfg_laa_r14_s::max_nof_sched_sfs_format0_b_r14_e_::to_number() const
{
  const static uint8_t options[] = {2, 3, 4};
  return get_enum_number(options, 3, value, "pdcch_cfg_laa_r14_s::max_nof_sched_sfs_format0_b_r14_e_");
}

std::string pdcch_cfg_laa_r14_s::max_nof_sched_sfs_format4_b_r14_e_::to_string() const
{
  switch (value) {
    case sf2:
      return "sf2";
    case sf3:
      return "sf3";
    case sf4:
      return "sf4";
    default:
      invalid_enum_value(value, "pdcch_cfg_laa_r14_s::max_nof_sched_sfs_format4_b_r14_e_");
  }
  return "";
}
uint8_t pdcch_cfg_laa_r14_s::max_nof_sched_sfs_format4_b_r14_e_::to_number() const
{
  const static uint8_t options[] = {2, 3, 4};
  return get_enum_number(options, 3, value, "pdcch_cfg_laa_r14_s::max_nof_sched_sfs_format4_b_r14_e_");
}

std::string cqi_report_cfg_scell_r15_s::alt_cqi_table_minus1024_qam_r15_e_::to_string() const
{
  switch (value) {
    case all_sfs:
      return "allSubframes";
    case csi_sf_set1:
      return "csi-SubframeSet1";
    case csi_sf_set2:
      return "csi-SubframeSet2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "cqi_report_cfg_scell_r15_s::alt_cqi_table_minus1024_qam_r15_e_");
  }
  return "";
}
uint8_t cqi_report_cfg_scell_r15_s::alt_cqi_table_minus1024_qam_r15_e_::to_number() const
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
    csi_report_mode_short_r15_e_::to_string() const
{
  switch (value) {
    case submode1:
      return "submode1";
    case submode2:
      return "submode2";
    default:
      invalid_enum_value(value, "cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::wideband_cqi_short_"
                                "r15_s_::csi_report_mode_short_r15_e_");
  }
  return "";
}
uint8_t cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::wideband_cqi_short_r15_s_::
    csi_report_mode_short_r15_e_::to_number() const
{
  const static uint8_t options[] = {1, 2};
  return get_enum_number(options, 2, value,
                         "cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::wideband_cqi_short_r15_s_::"
                         "csi_report_mode_short_r15_e_");
}

std::string cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::subband_cqi_short_r15_s_::
    periodicity_factor_r15_e_::to_string() const
{
  switch (value) {
    case n2:
      return "n2";
    case n4:
      return "n4";
    default:
      invalid_enum_value(value, "cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::subband_cqi_short_"
                                "r15_s_::periodicity_factor_r15_e_");
  }
  return "";
}
uint8_t cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::subband_cqi_short_r15_s_::
    periodicity_factor_r15_e_::to_number() const
{
  const static uint8_t options[] = {2, 4};
  return get_enum_number(options, 2, value,
                         "cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::subband_cqi_short_r15_s_::"
                         "periodicity_factor_r15_e_");
}

std::string cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::types::to_string() const
{
  switch (value) {
    case wideband_cqi_short_r15:
      return "widebandCQI-Short-r15";
    case subband_cqi_short_r15:
      return "subbandCQI-Short-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "cqi_short_cfg_scell_r15_c::setup_s_::cqi_format_ind_short_r15_c_::types");
  }
  return "";
}

std::string cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_::types::to_string() const
{
  switch (value) {
    case own_r10:
      return "own-r10";
    case other_r10:
      return "other-r10";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "cross_carrier_sched_cfg_r10_s::sched_cell_info_r10_c_::types");
  }
  return "";
}

std::string cross_carrier_sched_cfg_r13_s::sched_cell_info_r13_c_::types::to_string() const
{
  switch (value) {
    case own_r13:
      return "own-r13";
    case other_r13:
      return "other-r13";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "cross_carrier_sched_cfg_r13_s::sched_cell_info_r13_c_::types");
  }
  return "";
}

std::string delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1_r15_e_::to_string() const
{
  switch (value) {
    case delta_f_minus1:
      return "deltaF-1";
    case delta_f0:
      return "deltaF0";
    case delta_f1:
      return "deltaF1";
    case delta_f2:
      return "deltaF2";
    case delta_f3:
      return "deltaF3";
    case delta_f4:
      return "deltaF4";
    case delta_f5:
      return "deltaF5";
    case delta_f6:
      return "deltaF6";
    default:
      invalid_enum_value(value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1_r15_e_");
  }
  return "";
}
int8_t delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1_r15_e_::to_number() const
{
  const static int8_t options[] = {-1, 0, 1, 2, 3, 4, 5, 6};
  return get_enum_number(options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1_r15_e_");
}

std::string delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1a_r15_e_::to_string() const
{
  switch (value) {
    case delta_f1:
      return "deltaF1";
    case delta_f2:
      return "deltaF2";
    case delta_f3:
      return "deltaF3";
    case delta_f4:
      return "deltaF4";
    case delta_f5:
      return "deltaF5";
    case delta_f6:
      return "deltaF6";
    case delta_f7:
      return "deltaF7";
    case delta_f8:
      return "deltaF8";
    default:
      invalid_enum_value(value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1a_r15_e_");
  }
  return "";
}
uint8_t delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1a_r15_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 4, 5, 6, 7, 8};
  return get_enum_number(options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1a_r15_e_");
}

std::string delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1b_r15_e_::to_string() const
{
  switch (value) {
    case delta_f3:
      return "deltaF3";
    case delta_f4:
      return "deltaF4";
    case delta_f5:
      return "deltaF5";
    case delta_f6:
      return "deltaF6";
    case delta_f7:
      return "deltaF7";
    case delta_f8:
      return "deltaF8";
    case delta_f9:
      return "deltaF9";
    case delta_f10:
      return "deltaF10";
    default:
      invalid_enum_value(value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1b_r15_e_");
  }
  return "";
}
uint8_t delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1b_r15_e_::to_number() const
{
  const static uint8_t options[] = {3, 4, 5, 6, 7, 8, 9, 10};
  return get_enum_number(options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1b_r15_e_");
}

std::string delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format3_r15_e_::to_string() const
{
  switch (value) {
    case delta_f4:
      return "deltaF4";
    case delta_f5:
      return "deltaF5";
    case delta_f6:
      return "deltaF6";
    case delta_f7:
      return "deltaF7";
    case delta_f8:
      return "deltaF8";
    case delta_f9:
      return "deltaF9";
    case delta_f10:
      return "deltaF10";
    case delta_f11:
      return "deltaF11";
    default:
      invalid_enum_value(value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format3_r15_e_");
  }
  return "";
}
uint8_t delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format3_r15_e_::to_number() const
{
  const static uint8_t options[] = {4, 5, 6, 7, 8, 9, 10, 11};
  return get_enum_number(options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format3_r15_e_");
}

std::string delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_rm_format4_r15_e_::to_string() const
{
  switch (value) {
    case delta_f13:
      return "deltaF13";
    case delta_f14:
      return "deltaF14";
    case delta_f15:
      return "deltaF15";
    case delta_f16:
      return "deltaF16";
    case delta_f17:
      return "deltaF17";
    case delta_f18:
      return "deltaF18";
    case delta_f19:
      return "deltaF19";
    case delta_f20:
      return "deltaF20";
    default:
      invalid_enum_value(value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_rm_format4_r15_e_");
  }
  return "";
}
uint8_t delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_rm_format4_r15_e_::to_number() const
{
  const static uint8_t options[] = {13, 14, 15, 16, 17, 18, 19, 20};
  return get_enum_number(options, 8, value,
                         "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_rm_format4_r15_e_");
}

std::string delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_tbcc_format4_r15_e_::to_string() const
{
  switch (value) {
    case delta_f10:
      return "deltaF10";
    case delta_f11:
      return "deltaF11";
    case delta_f12:
      return "deltaF12";
    case delta_f13:
      return "deltaF13";
    case delta_f14:
      return "deltaF14";
    case delta_f15:
      return "deltaF15";
    case delta_f16:
      return "deltaF16";
    case delta_f17:
      return "deltaF17";
    default:
      invalid_enum_value(value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_tbcc_format4_r15_e_");
  }
  return "";
}
uint8_t delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_tbcc_format4_r15_e_::to_number() const
{
  const static uint8_t options[] = {10, 11, 12, 13, 14, 15, 16, 17};
  return get_enum_number(options, 8, value,
                         "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_tbcc_format4_r15_e_");
}

std::string delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_format1and1a_r15_e_::to_string() const
{
  switch (value) {
    case delta_f5:
      return "deltaF5";
    case delta_f6:
      return "deltaF6";
    case delta_f7:
      return "deltaF7";
    case delta_f8:
      return "deltaF8";
    case delta_f9:
      return "deltaF9";
    case delta_f10:
      return "deltaF10";
    case delta_f11:
      return "deltaF11";
    case delta_f12:
      return "deltaF12";
    default:
      invalid_enum_value(value, "delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_format1and1a_r15_e_");
  }
  return "";
}
uint8_t delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_format1and1a_r15_e_::to_number() const
{
  const static uint8_t options[] = {5, 6, 7, 8, 9, 10, 11, 12};
  return get_enum_number(options, 8, value,
                         "delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_format1and1a_r15_e_");
}

std::string delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_format1b_r15_e_::to_string() const
{
  switch (value) {
    case delta_f6:
      return "deltaF6";
    case delta_f7:
      return "deltaF7";
    case delta_f8:
      return "deltaF8";
    case delta_f9:
      return "deltaF9";
    case delta_f10:
      return "deltaF10";
    case delta_f11:
      return "deltaF11";
    case delta_f12:
      return "deltaF12";
    case delta_f13:
      return "deltaF13";
    default:
      invalid_enum_value(value, "delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_format1b_r15_e_");
  }
  return "";
}
uint8_t delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_format1b_r15_e_::to_number() const
{
  const static uint8_t options[] = {6, 7, 8, 9, 10, 11, 12, 13};
  return get_enum_number(options, 8, value,
                         "delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_format1b_r15_e_");
}

std::string delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_rm_format4_r15_e_::to_string() const
{
  switch (value) {
    case delta_f15:
      return "deltaF15";
    case delta_f16:
      return "deltaF16";
    case delta_f17:
      return "deltaF17";
    case delta_f18:
      return "deltaF18";
    case delta_f19:
      return "deltaF19";
    case delta_f20:
      return "deltaF20";
    case delta_f21:
      return "deltaF21";
    case delta_f22:
      return "deltaF22";
    default:
      invalid_enum_value(value, "delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_rm_format4_r15_e_");
  }
  return "";
}
uint8_t delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_rm_format4_r15_e_::to_number() const
{
  const static uint8_t options[] = {15, 16, 17, 18, 19, 20, 21, 22};
  return get_enum_number(options, 8, value,
                         "delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_rm_format4_r15_e_");
}

std::string delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_tbcc_format4_r15_e_::to_string() const
{
  switch (value) {
    case delta_f10:
      return "deltaF10";
    case delta_f11:
      return "deltaF11";
    case delta_f12:
      return "deltaF12";
    case delta_f13:
      return "deltaF13";
    case delta_f14:
      return "deltaF14";
    case delta_f15:
      return "deltaF15";
    case delta_f16:
      return "deltaF16";
    case delta_f17:
      return "deltaF17";
    default:
      invalid_enum_value(value, "delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_tbcc_format4_r15_e_");
  }
  return "";
}
uint8_t delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_tbcc_format4_r15_e_::to_number() const
{
  const static uint8_t options[] = {10, 11, 12, 13, 14, 15, 16, 17};
  return get_enum_number(options, 8, value,
                         "delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_tbcc_format4_r15_e_");
}

std::string laa_scell_cfg_r13_s::sf_start_position_r13_e_::to_string() const
{
  switch (value) {
    case s0:
      return "s0";
    case s07:
      return "s07";
    default:
      invalid_enum_value(value, "laa_scell_cfg_r13_s::sf_start_position_r13_e_");
  }
  return "";
}
float laa_scell_cfg_r13_s::sf_start_position_r13_e_::to_number() const
{
  const static float options[] = {0.0, 0.7};
  return get_enum_number(options, 2, value, "laa_scell_cfg_r13_s::sf_start_position_r13_e_");
}
std::string laa_scell_cfg_r13_s::sf_start_position_r13_e_::to_number_string() const
{
  switch (value) {
    case s0:
      return "0";
    case s07:
      return "0.7";
    default:
      invalid_enum_number(value, "laa_scell_cfg_r13_s::sf_start_position_r13_e_");
  }
  return "";
}

std::string sched_request_cfg_scell_r13_c::setup_s_::dsr_trans_max_r13_e_::to_string() const
{
  switch (value) {
    case n4:
      return "n4";
    case n8:
      return "n8";
    case n16:
      return "n16";
    case n32:
      return "n32";
    case n64:
      return "n64";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "sched_request_cfg_scell_r13_c::setup_s_::dsr_trans_max_r13_e_");
  }
  return "";
}
uint8_t sched_request_cfg_scell_r13_c::setup_s_::dsr_trans_max_r13_e_::to_number() const
{
  const static uint8_t options[] = {4, 8, 16, 32, 64};
  return get_enum_number(options, 5, value, "sched_request_cfg_scell_r13_c::setup_s_::dsr_trans_max_r13_e_");
}

std::string ul_pwr_ctrl_ded_scell_r10_s::delta_mcs_enabled_r10_e_::to_string() const
{
  switch (value) {
    case en0:
      return "en0";
    case en1:
      return "en1";
    default:
      invalid_enum_value(value, "ul_pwr_ctrl_ded_scell_r10_s::delta_mcs_enabled_r10_e_");
  }
  return "";
}
uint8_t ul_pwr_ctrl_ded_scell_r10_s::delta_mcs_enabled_r10_e_::to_number() const
{
  const static uint8_t options[] = {0, 1};
  return get_enum_number(options, 2, value, "ul_pwr_ctrl_ded_scell_r10_s::delta_mcs_enabled_r10_e_");
}

std::string ul_pwr_ctrl_ded_scell_r10_s::pathloss_ref_linking_r10_e_::to_string() const
{
  switch (value) {
    case p_cell:
      return "pCell";
    case s_cell:
      return "sCell";
    default:
      invalid_enum_value(value, "ul_pwr_ctrl_ded_scell_r10_s::pathloss_ref_linking_r10_e_");
  }
  return "";
}

std::string ant_info_common_s::ant_ports_count_e_::to_string() const
{
  switch (value) {
    case an1:
      return "an1";
    case an2:
      return "an2";
    case an4:
      return "an4";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "ant_info_common_s::ant_ports_count_e_");
  }
  return "";
}
uint8_t ant_info_common_s::ant_ports_count_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 4};
  return get_enum_number(options, 3, value, "ant_info_common_s::ant_ports_count_e_");
}

std::string phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::k_max_r14_e_::to_string() const
{
  switch (value) {
    case l1:
      return "l1";
    case l3:
      return "l3";
    default:
      invalid_enum_value(value, "phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::k_max_r14_e_");
  }
  return "";
}
uint8_t phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::k_max_r14_e_::to_number() const
{
  const static uint8_t options[] = {1, 3};
  return get_enum_number(options, 2, value, "phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::k_max_r14_e_");
}

std::string phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_e_::to_string() const
{
  switch (value) {
    case db_minus6:
      return "dB-6";
    case db_minus4dot77:
      return "dB-4dot77";
    case db_minus3:
      return "dB-3";
    case db_minus1dot77:
      return "dB-1dot77";
    case db0:
      return "dB0";
    case db1:
      return "dB1";
    case db2:
      return "dB2";
    case db3:
      return "dB3";
    default:
      invalid_enum_value(value, "phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_e_");
  }
  return "";
}
float phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_e_::to_number() const
{
  const static float options[] = {-6.0, -4.77, -3.0, -1.77, 0.0, 1.0, 2.0, 3.0};
  return get_enum_number(options, 8, value, "phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_e_");
}
std::string phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_e_::to_number_string() const
{
  switch (value) {
    case db_minus6:
      return "-6";
    case db_minus4dot77:
      return "-4.77";
    case db_minus3:
      return "-3";
    case db_minus1dot77:
      return "-1.77";
    case db0:
      return "0";
    case db1:
      return "1";
    case db2:
      return "2";
    case db3:
      return "3";
    default:
      invalid_enum_number(value, "phys_cfg_ded_scell_r10_s::must_cfg_r14_c_::setup_s_::p_a_must_r14_e_");
  }
  return "";
}

std::string phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::setup_c_::types::to_string() const
{
  switch (value) {
    case cfi_cfg_r15:
      return "cfi-Config-r15";
    case cfi_pattern_cfg_r15:
      return "cfi-PatternConfig-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "phys_cfg_ded_scell_r10_s::semi_static_cfi_cfg_r15_c_::setup_c_::types");
  }
  return "";
}

std::string
phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_sf_pdsch_repeats_r15_e_::to_string() const
{
  switch (value) {
    case n4:
      return "n4";
    case n6:
      return "n6";
    default:
      invalid_enum_value(
          value, "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_sf_pdsch_repeats_r15_e_");
  }
  return "";
}
uint8_t
phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_sf_pdsch_repeats_r15_e_::to_number() const
{
  const static uint8_t options[] = {4, 6};
  return get_enum_number(
      options, 2, value,
      "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_sf_pdsch_repeats_r15_e_");
}

std::string phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::
    max_num_slot_subslot_pdsch_repeats_r15_e_::to_string() const
{
  switch (value) {
    case n4:
      return "n4";
    case n6:
      return "n6";
    default:
      invalid_enum_value(value, "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_slot_"
                                "subslot_pdsch_repeats_r15_e_");
  }
  return "";
}
uint8_t phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_slot_subslot_pdsch_repeats_r15_e_::
    to_number() const
{
  const static uint8_t options[] = {4, 6};
  return get_enum_number(
      options, 2, value,
      "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::max_num_slot_subslot_pdsch_repeats_r15_e_");
}

std::string
phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_sf_pdsch_repeats_r15_e_::to_string() const
{
  switch (value) {
    case dlrvseq1:
      return "dlrvseq1";
    case dlrvseq2:
      return "dlrvseq2";
    default:
      invalid_enum_value(
          value, "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_sf_pdsch_repeats_r15_e_");
  }
  return "";
}
uint8_t phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_sf_pdsch_repeats_r15_e_::to_number() const
{
  const static uint8_t options[] = {1, 2};
  return get_enum_number(
      options, 2, value,
      "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_sf_pdsch_repeats_r15_e_");
}

std::string
phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_slotsublot_pdsch_repeats_r15_e_::to_string() const
{
  switch (value) {
    case dlrvseq1:
      return "dlrvseq1";
    case dlrvseq2:
      return "dlrvseq2";
    default:
      invalid_enum_value(
          value,
          "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_slotsublot_pdsch_repeats_r15_e_");
  }
  return "";
}
uint8_t
phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_slotsublot_pdsch_repeats_r15_e_::to_number() const
{
  const static uint8_t options[] = {1, 2};
  return get_enum_number(
      options, 2, value,
      "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::rv_slotsublot_pdsch_repeats_r15_e_");
}

std::string
phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_sf_pdsch_repeats_r15_e_::to_string()
    const
{
  switch (value) {
    case n0:
      return "n0";
    case n1:
      return "n1";
    default:
      invalid_enum_value(
          value,
          "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_sf_pdsch_repeats_r15_e_");
  }
  return "";
}
uint8_t
phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_sf_pdsch_repeats_r15_e_::to_number()
    const
{
  const static uint8_t options[] = {0, 1};
  return get_enum_number(
      options, 2, value,
      "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_sf_pdsch_repeats_r15_e_");
}

std::string phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::
    mcs_restrict_slot_subslot_pdsch_repeats_r15_e_::to_string() const
{
  switch (value) {
    case n0:
      return "n0";
    case n1:
      return "n1";
    default:
      invalid_enum_value(value, "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_slot_"
                                "subslot_pdsch_repeats_r15_e_");
  }
  return "";
}
uint8_t phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::
    mcs_restrict_slot_subslot_pdsch_repeats_r15_e_::to_number() const
{
  const static uint8_t options[] = {0, 1};
  return get_enum_number(options, 2, value,
                         "phys_cfg_ded_scell_r10_s::blind_pdsch_repeat_cfg_r15_c_::setup_s_::mcs_restrict_slot_subslot_"
                         "pdsch_repeats_r15_e_");
}

std::string ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format3_r12_e_::to_string() const
{
  switch (value) {
    case delta_f_minus1:
      return "deltaF-1";
    case delta_f0:
      return "deltaF0";
    case delta_f1:
      return "deltaF1";
    case delta_f2:
      return "deltaF2";
    case delta_f3:
      return "deltaF3";
    case delta_f4:
      return "deltaF4";
    case delta_f5:
      return "deltaF5";
    case delta_f6:
      return "deltaF6";
    default:
      invalid_enum_value(value, "ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format3_r12_e_");
  }
  return "";
}
int8_t ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format3_r12_e_::to_number() const
{
  const static int8_t options[] = {-1, 0, 1, 2, 3, 4, 5, 6};
  return get_enum_number(options, 8, value, "ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format3_r12_e_");
}

std::string ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format1b_cs_r12_e_::to_string() const
{
  switch (value) {
    case delta_f1:
      return "deltaF1";
    case delta_f2:
      return "deltaF2";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format1b_cs_r12_e_");
  }
  return "";
}
uint8_t ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format1b_cs_r12_e_::to_number() const
{
  const static uint8_t options[] = {1, 2};
  return get_enum_number(options, 2, value, "ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format1b_cs_r12_e_");
}

std::string ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format4_r13_e_::to_string() const
{
  switch (value) {
    case delta_f16:
      return "deltaF16";
    case delta_f15:
      return "deltaF15";
    case delta_f14:
      return "deltaF14";
    case delta_f13:
      return "deltaF13";
    case delta_f12:
      return "deltaF12";
    case delta_f11:
      return "deltaF11";
    case delta_f10:
      return "deltaF10";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format4_r13_e_");
  }
  return "";
}
uint8_t ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format4_r13_e_::to_number() const
{
  const static uint8_t options[] = {16, 15, 14, 13, 12, 11, 10};
  return get_enum_number(options, 7, value, "ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format4_r13_e_");
}

std::string ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format5_minus13_e_::to_string() const
{
  switch (value) {
    case delta_f13:
      return "deltaF13";
    case delta_f12:
      return "deltaF12";
    case delta_f11:
      return "deltaF11";
    case delta_f10:
      return "deltaF10";
    case delta_f9:
      return "deltaF9";
    case delta_f8:
      return "deltaF8";
    case delta_f7:
      return "deltaF7";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format5_minus13_e_");
  }
  return "";
}
uint8_t ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format5_minus13_e_::to_number() const
{
  const static uint8_t options[] = {13, 12, 11, 10, 9, 8, 7};
  return get_enum_number(options, 7, value, "ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format5_minus13_e_");
}

std::string ant_info_ded_v10i0_s::max_layers_mimo_r10_e_::to_string() const
{
  switch (value) {
    case two_layers:
      return "twoLayers";
    case four_layers:
      return "fourLayers";
    case eight_layers:
      return "eightLayers";
    default:
      invalid_enum_value(value, "ant_info_ded_v10i0_s::max_layers_mimo_r10_e_");
  }
  return "";
}
uint8_t ant_info_ded_v10i0_s::max_layers_mimo_r10_e_::to_number() const
{
  const static uint8_t options[] = {2, 4, 8};
  return get_enum_number(options, 3, value, "ant_info_ded_v10i0_s::max_layers_mimo_r10_e_");
}

std::string rr_cfg_common_scell_r10_s::non_ul_cfg_r10_s_::dl_bw_r10_e_::to_string() const
{
  switch (value) {
    case n6:
      return "n6";
    case n15:
      return "n15";
    case n25:
      return "n25";
    case n50:
      return "n50";
    case n75:
      return "n75";
    case n100:
      return "n100";
    default:
      invalid_enum_value(value, "rr_cfg_common_scell_r10_s::non_ul_cfg_r10_s_::dl_bw_r10_e_");
  }
  return "";
}
uint8_t rr_cfg_common_scell_r10_s::non_ul_cfg_r10_s_::dl_bw_r10_e_::to_number() const
{
  const static uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return get_enum_number(options, 6, value, "rr_cfg_common_scell_r10_s::non_ul_cfg_r10_s_::dl_bw_r10_e_");
}

std::string rr_cfg_common_scell_r10_s::ul_cfg_r10_s_::ul_freq_info_r10_s_::ul_bw_r10_e_::to_string() const
{
  switch (value) {
    case n6:
      return "n6";
    case n15:
      return "n15";
    case n25:
      return "n25";
    case n50:
      return "n50";
    case n75:
      return "n75";
    case n100:
      return "n100";
    default:
      invalid_enum_value(value, "rr_cfg_common_scell_r10_s::ul_cfg_r10_s_::ul_freq_info_r10_s_::ul_bw_r10_e_");
  }
  return "";
}
uint8_t rr_cfg_common_scell_r10_s::ul_cfg_r10_s_::ul_freq_info_r10_s_::ul_bw_r10_e_::to_number() const
{
  const static uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return get_enum_number(options, 6, value,
                         "rr_cfg_common_scell_r10_s::ul_cfg_r10_s_::ul_freq_info_r10_s_::ul_bw_r10_e_");
}

std::string rr_cfg_common_scell_r10_s::ul_cfg_r14_s_::ul_freq_info_r14_s_::ul_bw_r14_e_::to_string() const
{
  switch (value) {
    case n6:
      return "n6";
    case n15:
      return "n15";
    case n25:
      return "n25";
    case n50:
      return "n50";
    case n75:
      return "n75";
    case n100:
      return "n100";
    default:
      invalid_enum_value(value, "rr_cfg_common_scell_r10_s::ul_cfg_r14_s_::ul_freq_info_r14_s_::ul_bw_r14_e_");
  }
  return "";
}
uint8_t rr_cfg_common_scell_r10_s::ul_cfg_r14_s_::ul_freq_info_r14_s_::ul_bw_r14_e_::to_number() const
{
  const static uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return get_enum_number(options, 6, value,
                         "rr_cfg_common_scell_r10_s::ul_cfg_r14_s_::ul_freq_info_r14_s_::ul_bw_r14_e_");
}

std::string rr_cfg_common_scell_r10_s::harq_ref_cfg_r14_e_::to_string() const
{
  switch (value) {
    case sa2:
      return "sa2";
    case sa4:
      return "sa4";
    case sa5:
      return "sa5";
    default:
      invalid_enum_value(value, "rr_cfg_common_scell_r10_s::harq_ref_cfg_r14_e_");
  }
  return "";
}
uint8_t rr_cfg_common_scell_r10_s::harq_ref_cfg_r14_e_::to_number() const
{
  const static uint8_t options[] = {2, 4, 5};
  return get_enum_number(options, 3, value, "rr_cfg_common_scell_r10_s::harq_ref_cfg_r14_e_");
}

// CipheringAlgorithm-r12 ::= ENUMERATED
std::string ciphering_algorithm_r12_e::to_string() const
{
  switch (value) {
    case eea0:
      return "eea0";
    case eea1:
      return "eea1";
    case eea2:
      return "eea2";
    case eea3_v1130:
      return "eea3-v1130";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "ciphering_algorithm_r12_e");
  }
  return "";
}
uint8_t ciphering_algorithm_r12_e::to_number() const
{
  const static uint8_t options[] = {0, 1, 2, 3};
  return get_enum_number(options, 4, value, "ciphering_algorithm_r12_e");
}

std::string sl_hop_cfg_disc_r12_s::c_r12_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n5:
      return "n5";
    default:
      invalid_enum_value(value, "sl_hop_cfg_disc_r12_s::c_r12_e_");
  }
  return "";
}
uint8_t sl_hop_cfg_disc_r12_s::c_r12_e_::to_number() const
{
  const static uint8_t options[] = {1, 5};
  return get_enum_number(options, 2, value, "sl_hop_cfg_disc_r12_s::c_r12_e_");
}

std::string security_algorithm_cfg_s::integrity_prot_algorithm_e_::to_string() const
{
  switch (value) {
    case eia0_v920:
      return "eia0-v920";
    case eia1:
      return "eia1";
    case eia2:
      return "eia2";
    case eia3_v1130:
      return "eia3-v1130";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "security_algorithm_cfg_s::integrity_prot_algorithm_e_");
  }
  return "";
}
uint8_t security_algorithm_cfg_s::integrity_prot_algorithm_e_::to_number() const
{
  const static uint8_t options[] = {0, 1, 2, 3};
  return get_enum_number(options, 4, value, "security_algorithm_cfg_s::integrity_prot_algorithm_e_");
}

std::string drb_to_add_mod_scg_r12_s::drb_type_r12_c_::types::to_string() const
{
  switch (value) {
    case split_r12:
      return "split-r12";
    case scg_r12:
      return "scg-r12";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "drb_to_add_mod_scg_r12_s::drb_type_r12_c_::types");
  }
  return "";
}

std::string ip_address_r13_c::types::to_string() const
{
  switch (value) {
    case ipv4_r13:
      return "ipv4-r13";
    case ipv6_r13:
      return "ipv6-r13";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ip_address_r13_c::types");
  }
  return "";
}
uint8_t ip_address_r13_c::types::to_number() const
{
  const static uint8_t options[] = {4, 6};
  return get_enum_number(options, 2, value, "ip_address_r13_c::types");
}

std::string security_cfg_ho_v1530_s::ho_type_v1530_c_::types::to_string() const
{
  switch (value) {
    case intra5_gc_r15:
      return "intra5GC-r15";
    case ngc_to_epc_r15:
      return "ngc-ToEPC-r15";
    case epc_to_ngc_r15:
      return "epc-ToNGC-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "security_cfg_ho_v1530_s::ho_type_v1530_c_::types");
  }
  return "";
}
uint8_t security_cfg_ho_v1530_s::ho_type_v1530_c_::types::to_number() const
{
  const static uint8_t options[] = {5};
  return get_enum_number(options, 1, value, "security_cfg_ho_v1530_s::ho_type_v1530_c_::types");
}

std::string ul_pwr_ctrl_common_v1310_s::delta_f_pucch_format4_r13_e_::to_string() const
{
  switch (value) {
    case delta_f16:
      return "deltaF16";
    case delta_f15:
      return "deltaF15";
    case delta_f14:
      return "deltaF14";
    case delta_f13:
      return "deltaF13";
    case delta_f12:
      return "deltaF12";
    case delta_f11:
      return "deltaF11";
    case delta_f10:
      return "deltaF10";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "ul_pwr_ctrl_common_v1310_s::delta_f_pucch_format4_r13_e_");
  }
  return "";
}
uint8_t ul_pwr_ctrl_common_v1310_s::delta_f_pucch_format4_r13_e_::to_number() const
{
  const static uint8_t options[] = {16, 15, 14, 13, 12, 11, 10};
  return get_enum_number(options, 7, value, "ul_pwr_ctrl_common_v1310_s::delta_f_pucch_format4_r13_e_");
}

std::string ul_pwr_ctrl_common_v1310_s::delta_f_pucch_format5_minus13_e_::to_string() const
{
  switch (value) {
    case delta_f13:
      return "deltaF13";
    case delta_f12:
      return "deltaF12";
    case delta_f11:
      return "deltaF11";
    case delta_f10:
      return "deltaF10";
    case delta_f9:
      return "deltaF9";
    case delta_f8:
      return "deltaF8";
    case delta_f7:
      return "deltaF7";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "ul_pwr_ctrl_common_v1310_s::delta_f_pucch_format5_minus13_e_");
  }
  return "";
}
uint8_t ul_pwr_ctrl_common_v1310_s::delta_f_pucch_format5_minus13_e_::to_number() const
{
  const static uint8_t options[] = {13, 12, 11, 10, 9, 8, 7};
  return get_enum_number(options, 7, value, "ul_pwr_ctrl_common_v1310_s::delta_f_pucch_format5_minus13_e_");
}

std::string ul_pwr_ctrl_common_ps_cell_r12_s::delta_f_pucch_format3_r12_e_::to_string() const
{
  switch (value) {
    case delta_f_minus1:
      return "deltaF-1";
    case delta_f0:
      return "deltaF0";
    case delta_f1:
      return "deltaF1";
    case delta_f2:
      return "deltaF2";
    case delta_f3:
      return "deltaF3";
    case delta_f4:
      return "deltaF4";
    case delta_f5:
      return "deltaF5";
    case delta_f6:
      return "deltaF6";
    default:
      invalid_enum_value(value, "ul_pwr_ctrl_common_ps_cell_r12_s::delta_f_pucch_format3_r12_e_");
  }
  return "";
}
int8_t ul_pwr_ctrl_common_ps_cell_r12_s::delta_f_pucch_format3_r12_e_::to_number() const
{
  const static int8_t options[] = {-1, 0, 1, 2, 3, 4, 5, 6};
  return get_enum_number(options, 8, value, "ul_pwr_ctrl_common_ps_cell_r12_s::delta_f_pucch_format3_r12_e_");
}

std::string ul_pwr_ctrl_common_ps_cell_r12_s::delta_f_pucch_format1b_cs_r12_e_::to_string() const
{
  switch (value) {
    case delta_f1:
      return "deltaF1";
    case delta_f2:
      return "deltaF2";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "ul_pwr_ctrl_common_ps_cell_r12_s::delta_f_pucch_format1b_cs_r12_e_");
  }
  return "";
}
uint8_t ul_pwr_ctrl_common_ps_cell_r12_s::delta_f_pucch_format1b_cs_r12_e_::to_number() const
{
  const static uint8_t options[] = {1, 2};
  return get_enum_number(options, 2, value, "ul_pwr_ctrl_common_ps_cell_r12_s::delta_f_pucch_format1b_cs_r12_e_");
}

std::string rach_skip_r14_s::target_ta_r14_c_::types::to_string() const
{
  switch (value) {
    case ta0_r14:
      return "ta0-r14";
    case mcg_ptag_r14:
      return "mcg-PTAG-r14";
    case scg_ptag_r14:
      return "scg-PTAG-r14";
    case mcg_stag_r14:
      return "mcg-STAG-r14";
    case scg_stag_r14:
      return "scg-STAG-r14";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rach_skip_r14_s::target_ta_r14_c_::types");
  }
  return "";
}
uint8_t rach_skip_r14_s::target_ta_r14_c_::types::to_number() const
{
  const static uint8_t options[] = {0};
  return get_enum_number(options, 1, value, "rach_skip_r14_s::target_ta_r14_c_::types");
}

std::string rach_skip_r14_s::ul_cfg_info_r14_s_::ul_sched_interv_r14_e_::to_string() const
{
  switch (value) {
    case sf2:
      return "sf2";
    case sf5:
      return "sf5";
    case sf10:
      return "sf10";
    default:
      invalid_enum_value(value, "rach_skip_r14_s::ul_cfg_info_r14_s_::ul_sched_interv_r14_e_");
  }
  return "";
}
uint8_t rach_skip_r14_s::ul_cfg_info_r14_s_::ul_sched_interv_r14_e_::to_number() const
{
  const static uint8_t options[] = {2, 5, 10};
  return get_enum_number(options, 3, value, "rach_skip_r14_s::ul_cfg_info_r14_s_::ul_sched_interv_r14_e_");
}

std::string rlf_timers_and_consts_scg_r12_c::setup_s_::t313_r12_e_::to_string() const
{
  switch (value) {
    case ms0:
      return "ms0";
    case ms50:
      return "ms50";
    case ms100:
      return "ms100";
    case ms200:
      return "ms200";
    case ms500:
      return "ms500";
    case ms1000:
      return "ms1000";
    case ms2000:
      return "ms2000";
    default:
      invalid_enum_value(value, "rlf_timers_and_consts_scg_r12_c::setup_s_::t313_r12_e_");
  }
  return "";
}
uint16_t rlf_timers_and_consts_scg_r12_c::setup_s_::t313_r12_e_::to_number() const
{
  const static uint16_t options[] = {0, 50, 100, 200, 500, 1000, 2000};
  return get_enum_number(options, 7, value, "rlf_timers_and_consts_scg_r12_c::setup_s_::t313_r12_e_");
}

std::string rlf_timers_and_consts_scg_r12_c::setup_s_::n313_r12_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n2:
      return "n2";
    case n3:
      return "n3";
    case n4:
      return "n4";
    case n6:
      return "n6";
    case n8:
      return "n8";
    case n10:
      return "n10";
    case n20:
      return "n20";
    default:
      invalid_enum_value(value, "rlf_timers_and_consts_scg_r12_c::setup_s_::n313_r12_e_");
  }
  return "";
}
uint8_t rlf_timers_and_consts_scg_r12_c::setup_s_::n313_r12_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 4, 6, 8, 10, 20};
  return get_enum_number(options, 8, value, "rlf_timers_and_consts_scg_r12_c::setup_s_::n313_r12_e_");
}

std::string rlf_timers_and_consts_scg_r12_c::setup_s_::n314_r12_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n2:
      return "n2";
    case n3:
      return "n3";
    case n4:
      return "n4";
    case n5:
      return "n5";
    case n6:
      return "n6";
    case n8:
      return "n8";
    case n10:
      return "n10";
    default:
      invalid_enum_value(value, "rlf_timers_and_consts_scg_r12_c::setup_s_::n314_r12_e_");
  }
  return "";
}
uint8_t rlf_timers_and_consts_scg_r12_c::setup_s_::n314_r12_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 4, 5, 6, 8, 10};
  return get_enum_number(options, 8, value, "rlf_timers_and_consts_scg_r12_c::setup_s_::n314_r12_e_");
}

std::string scell_to_add_mod_r10_s::s_cell_state_r15_e_::to_string() const
{
  switch (value) {
    case activ:
      return "activated";
    case dormant:
      return "dormant";
    default:
      invalid_enum_value(value, "scell_to_add_mod_r10_s::s_cell_state_r15_e_");
  }
  return "";
}

std::string scell_to_add_mod_ext_v1430_s::s_cell_state_r15_e_::to_string() const
{
  switch (value) {
    case activ:
      return "activated";
    case dormant:
      return "dormant";
    default:
      invalid_enum_value(value, "scell_to_add_mod_ext_v1430_s::s_cell_state_r15_e_");
  }
  return "";
}

std::string sl_disc_tx_ref_carrier_ded_r13_c::types::to_string() const
{
  switch (value) {
    case p_cell:
      return "pCell";
    case s_cell:
      return "sCell";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sl_disc_tx_ref_carrier_ded_r13_c::types");
  }
  return "";
}

std::string sl_disc_tx_res_r13_c::setup_c_::types::to_string() const
{
  switch (value) {
    case sched_r13:
      return "scheduled-r13";
    case ue_sel_r13:
      return "ue-Selected-r13";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sl_disc_tx_res_r13_c::setup_c_::types");
  }
  return "";
}

std::string sl_gap_pattern_r13_s::gap_period_r13_e_::to_string() const
{
  switch (value) {
    case sf40:
      return "sf40";
    case sf60:
      return "sf60";
    case sf70:
      return "sf70";
    case sf80:
      return "sf80";
    case sf120:
      return "sf120";
    case sf140:
      return "sf140";
    case sf160:
      return "sf160";
    case sf240:
      return "sf240";
    case sf280:
      return "sf280";
    case sf320:
      return "sf320";
    case sf640:
      return "sf640";
    case sf1280:
      return "sf1280";
    case sf2560:
      return "sf2560";
    case sf5120:
      return "sf5120";
    case sf10240:
      return "sf10240";
    default:
      invalid_enum_value(value, "sl_gap_pattern_r13_s::gap_period_r13_e_");
  }
  return "";
}
uint16_t sl_gap_pattern_r13_s::gap_period_r13_e_::to_number() const
{
  const static uint16_t options[] = {40, 60, 70, 80, 120, 140, 160, 240, 280, 320, 640, 1280, 2560, 5120, 10240};
  return get_enum_number(options, 15, value, "sl_gap_pattern_r13_s::gap_period_r13_e_");
}

// SubframeAssignment-r15 ::= ENUMERATED
std::string sf_assign_r15_e::to_string() const
{
  switch (value) {
    case sa0:
      return "sa0";
    case sa1:
      return "sa1";
    case sa2:
      return "sa2";
    case sa3:
      return "sa3";
    case sa4:
      return "sa4";
    case sa5:
      return "sa5";
    case sa6:
      return "sa6";
    default:
      invalid_enum_value(value, "sf_assign_r15_e");
  }
  return "";
}
uint8_t sf_assign_r15_e::to_number() const
{
  const static uint8_t options[] = {0, 1, 2, 3, 4, 5, 6};
  return get_enum_number(options, 7, value, "sf_assign_r15_e");
}

std::string wlan_mob_cfg_r13_s::association_timer_r13_e_::to_string() const
{
  switch (value) {
    case s10:
      return "s10";
    case s30:
      return "s30";
    case s60:
      return "s60";
    case s120:
      return "s120";
    case s240:
      return "s240";
    default:
      invalid_enum_value(value, "wlan_mob_cfg_r13_s::association_timer_r13_e_");
  }
  return "";
}
uint8_t wlan_mob_cfg_r13_s::association_timer_r13_e_::to_number() const
{
  const static uint8_t options[] = {10, 30, 60, 120, 240};
  return get_enum_number(options, 5, value, "wlan_mob_cfg_r13_s::association_timer_r13_e_");
}

// CA-BandwidthClass-r10 ::= ENUMERATED
std::string ca_bw_class_r10_e::to_string() const
{
  switch (value) {
    case a:
      return "a";
    case b:
      return "b";
    case c:
      return "c";
    case d:
      return "d";
    case e:
      return "e";
    case f:
      return "f";
    default:
      invalid_enum_value(value, "ca_bw_class_r10_e");
  }
  return "";
}

std::string mob_ctrl_info_scg_r12_s::t307_r12_e_::to_string() const
{
  switch (value) {
    case ms50:
      return "ms50";
    case ms100:
      return "ms100";
    case ms150:
      return "ms150";
    case ms200:
      return "ms200";
    case ms500:
      return "ms500";
    case ms1000:
      return "ms1000";
    case ms2000:
      return "ms2000";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "mob_ctrl_info_scg_r12_s::t307_r12_e_");
  }
  return "";
}
uint16_t mob_ctrl_info_scg_r12_s::t307_r12_e_::to_number() const
{
  const static uint16_t options[] = {50, 100, 150, 200, 500, 1000, 2000};
  return get_enum_number(options, 7, value, "mob_ctrl_info_scg_r12_s::t307_r12_e_");
}

std::string rclwi_cfg_r13_s::cmd_c_::types::to_string() const
{
  switch (value) {
    case steer_to_wlan_r13:
      return "steerToWLAN-r13";
    case steer_to_lte_r13:
      return "steerToLTE-r13";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rclwi_cfg_r13_s::cmd_c_::types");
  }
  return "";
}

std::string sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::setup_c_::types::to_string() const
{
  switch (value) {
    case sched_r14:
      return "scheduled-r14";
    case ue_sel_r14:
      return "ue-Selected-r14";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sl_v2x_cfg_ded_r14_s::comm_tx_res_r14_c_::setup_c_::types");
  }
  return "";
}

std::string sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::setup_c_::types::to_string() const
{
  switch (value) {
    case sched_v1530:
      return "scheduled-v1530";
    case ue_sel_v1530:
      return "ue-Selected-v1530";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sl_v2x_cfg_ded_r14_s::comm_tx_res_v1530_c_::setup_c_::types");
  }
  return "";
}

std::string ran_notif_area_info_r15_c::types::to_string() const
{
  switch (value) {
    case cell_list_r15:
      return "cellList-r15";
    case ran_area_cfg_list_r15:
      return "ran-AreaConfigList-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ran_notif_area_info_r15_c::types");
  }
  return "";
}

std::string meas_idle_cfg_ded_r15_s::meas_idle_dur_r15_e_::to_string() const
{
  switch (value) {
    case sec10:
      return "sec10";
    case sec30:
      return "sec30";
    case sec60:
      return "sec60";
    case sec120:
      return "sec120";
    case sec180:
      return "sec180";
    case sec240:
      return "sec240";
    case sec300:
      return "sec300";
    case spare:
      return "spare";
    default:
      invalid_enum_value(value, "meas_idle_cfg_ded_r15_s::meas_idle_dur_r15_e_");
  }
  return "";
}
uint16_t meas_idle_cfg_ded_r15_s::meas_idle_dur_r15_e_::to_number() const
{
  const static uint16_t options[] = {10, 30, 60, 120, 180, 240, 300};
  return get_enum_number(options, 7, value, "meas_idle_cfg_ded_r15_s::meas_idle_dur_r15_e_");
}

std::string rrc_inactive_cfg_r15_s::ran_paging_cycle_r15_e_::to_string() const
{
  switch (value) {
    case rf32:
      return "rf32";
    case rf64:
      return "rf64";
    case rf128:
      return "rf128";
    case rf256:
      return "rf256";
    default:
      invalid_enum_value(value, "rrc_inactive_cfg_r15_s::ran_paging_cycle_r15_e_");
  }
  return "";
}
uint16_t rrc_inactive_cfg_r15_s::ran_paging_cycle_r15_e_::to_number() const
{
  const static uint16_t options[] = {32, 64, 128, 256};
  return get_enum_number(options, 4, value, "rrc_inactive_cfg_r15_s::ran_paging_cycle_r15_e_");
}

std::string rrc_inactive_cfg_r15_s::periodic_rnau_timer_r15_e_::to_string() const
{
  switch (value) {
    case min5:
      return "min5";
    case min10:
      return "min10";
    case min20:
      return "min20";
    case min30:
      return "min30";
    case min60:
      return "min60";
    case min120:
      return "min120";
    case min360:
      return "min360";
    case min720:
      return "min720";
    default:
      invalid_enum_value(value, "rrc_inactive_cfg_r15_s::periodic_rnau_timer_r15_e_");
  }
  return "";
}
uint16_t rrc_inactive_cfg_r15_s::periodic_rnau_timer_r15_e_::to_number() const
{
  const static uint16_t options[] = {5, 10, 20, 30, 60, 120, 360, 720};
  return get_enum_number(options, 8, value, "rrc_inactive_cfg_r15_s::periodic_rnau_timer_r15_e_");
}

std::string sl_comm_cfg_r12_s::comm_tx_res_r12_c_::setup_c_::types::to_string() const
{
  switch (value) {
    case sched_r12:
      return "scheduled-r12";
    case ue_sel_r12:
      return "ue-Selected-r12";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sl_comm_cfg_r12_s::comm_tx_res_r12_c_::setup_c_::types");
  }
  return "";
}

std::string sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::setup_c_::types::to_string() const
{
  switch (value) {
    case sched_v1310:
      return "scheduled-v1310";
    case ue_sel_v1310:
      return "ue-Selected-v1310";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sl_comm_cfg_r12_s::comm_tx_res_v1310_c_::setup_c_::types");
  }
  return "";
}

std::string sl_disc_cfg_r12_s::disc_tx_res_r12_c_::setup_c_::types::to_string() const
{
  switch (value) {
    case sched_r12:
      return "scheduled-r12";
    case ue_sel_r12:
      return "ue-Selected-r12";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sl_disc_cfg_r12_s::disc_tx_res_r12_c_::setup_c_::types");
  }
  return "";
}

std::string sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::setup_c_::types::to_string() const
{
  switch (value) {
    case sched_r13:
      return "scheduled-r13";
    case ue_sel_r13:
      return "ue-Selected-r13";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sl_disc_cfg_r12_s::disc_tx_res_ps_r13_c_::setup_c_::types");
  }
  return "";
}

std::string sl_sync_tx_ctrl_r12_s::network_ctrl_sync_tx_r12_e_::to_string() const
{
  switch (value) {
    case on:
      return "on";
    case off:
      return "off";
    default:
      invalid_enum_value(value, "sl_sync_tx_ctrl_r12_s::network_ctrl_sync_tx_r12_e_");
  }
  return "";
}

// CDMA2000-Type ::= ENUMERATED
std::string cdma2000_type_e::to_string() const
{
  switch (value) {
    case type1_xrtt:
      return "type1XRTT";
    case type_hrpd:
      return "typeHRPD";
    default:
      invalid_enum_value(value, "cdma2000_type_e");
  }
  return "";
}
uint8_t cdma2000_type_e::to_number() const
{
  const static uint8_t options[] = {1};
  return get_enum_number(options, 1, value, "cdma2000_type_e");
}

// MeasCycleSCell-r10 ::= ENUMERATED
std::string meas_cycle_scell_r10_e::to_string() const
{
  switch (value) {
    case sf160:
      return "sf160";
    case sf256:
      return "sf256";
    case sf320:
      return "sf320";
    case sf512:
      return "sf512";
    case sf640:
      return "sf640";
    case sf1024:
      return "sf1024";
    case sf1280:
      return "sf1280";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "meas_cycle_scell_r10_e");
  }
  return "";
}
uint16_t meas_cycle_scell_r10_e::to_number() const
{
  const static uint16_t options[] = {160, 256, 320, 512, 640, 1024, 1280};
  return get_enum_number(options, 7, value, "meas_cycle_scell_r10_e");
}

std::string meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_::types::to_string() const
{
  switch (value) {
    case ms40_r12:
      return "ms40-r12";
    case ms80_r12:
      return "ms80-r12";
    case ms160_r12:
      return "ms160-r12";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_::types");
  }
  return "";
}
uint8_t meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_::types::to_number() const
{
  const static uint8_t options[] = {40, 80, 160};
  return get_enum_number(options, 3, value, "meas_ds_cfg_r12_c::setup_s_::dmtc_period_offset_r12_c_::types");
}

std::string meas_ds_cfg_r12_c::setup_s_::ds_occasion_dur_r12_c_::types::to_string() const
{
  switch (value) {
    case dur_fdd_r12:
      return "durationFDD-r12";
    case dur_tdd_r12:
      return "durationTDD-r12";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "meas_ds_cfg_r12_c::setup_s_::ds_occasion_dur_r12_c_::types");
  }
  return "";
}

std::string meas_gap_cfg_c::setup_s_::gap_offset_c_::types::to_string() const
{
  switch (value) {
    case gp0:
      return "gp0";
    case gp1:
      return "gp1";
    case gp2_r14:
      return "gp2-r14";
    case gp3_r14:
      return "gp3-r14";
    case gp_ncsg0_r14:
      return "gp-ncsg0-r14";
    case gp_ncsg1_r14:
      return "gp-ncsg1-r14";
    case gp_ncsg2_r14:
      return "gp-ncsg2-r14";
    case gp_ncsg3_r14:
      return "gp-ncsg3-r14";
    case gp_non_uniform1_r14:
      return "gp-nonUniform1-r14";
    case gp_non_uniform2_r14:
      return "gp-nonUniform2-r14";
    case gp_non_uniform3_r14:
      return "gp-nonUniform3-r14";
    case gp_non_uniform4_r14:
      return "gp-nonUniform4-r14";
    case gp4_r15:
      return "gp4-r15";
    case gp5_r15:
      return "gp5-r15";
    case gp6_r15:
      return "gp6-r15";
    case gp7_r15:
      return "gp7-r15";
    case gp8_r15:
      return "gp8-r15";
    case gp9_r15:
      return "gp9-r15";
    case gp10_r15:
      return "gp10-r15";
    case gp11_r15:
      return "gp11-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "meas_gap_cfg_c::setup_s_::gap_offset_c_::types");
  }
  return "";
}

std::string meas_sensing_cfg_r15_s::sensing_periodicity_r15_e_::to_string() const
{
  switch (value) {
    case ms20:
      return "ms20";
    case ms50:
      return "ms50";
    case ms100:
      return "ms100";
    case ms200:
      return "ms200";
    case ms300:
      return "ms300";
    case ms400:
      return "ms400";
    case ms500:
      return "ms500";
    case ms600:
      return "ms600";
    case ms700:
      return "ms700";
    case ms800:
      return "ms800";
    case ms900:
      return "ms900";
    case ms1000:
      return "ms1000";
    default:
      invalid_enum_value(value, "meas_sensing_cfg_r15_s::sensing_periodicity_r15_e_");
  }
  return "";
}
uint16_t meas_sensing_cfg_r15_s::sensing_periodicity_r15_e_::to_number() const
{
  const static uint16_t options[] = {20, 50, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
  return get_enum_number(options, 12, value, "meas_sensing_cfg_r15_s::sensing_periodicity_r15_e_");
}

std::string rmtc_cfg_r13_c::setup_s_::rmtc_period_r13_e_::to_string() const
{
  switch (value) {
    case ms40:
      return "ms40";
    case ms80:
      return "ms80";
    case ms160:
      return "ms160";
    case ms320:
      return "ms320";
    case ms640:
      return "ms640";
    default:
      invalid_enum_value(value, "rmtc_cfg_r13_c::setup_s_::rmtc_period_r13_e_");
  }
  return "";
}
uint16_t rmtc_cfg_r13_c::setup_s_::rmtc_period_r13_e_::to_number() const
{
  const static uint16_t options[] = {40, 80, 160, 320, 640};
  return get_enum_number(options, 5, value, "rmtc_cfg_r13_c::setup_s_::rmtc_period_r13_e_");
}

std::string rmtc_cfg_r13_c::setup_s_::meas_dur_r13_e_::to_string() const
{
  switch (value) {
    case sym1:
      return "sym1";
    case sym14:
      return "sym14";
    case sym28:
      return "sym28";
    case sym42:
      return "sym42";
    case sym70:
      return "sym70";
    default:
      invalid_enum_value(value, "rmtc_cfg_r13_c::setup_s_::meas_dur_r13_e_");
  }
  return "";
}
uint8_t rmtc_cfg_r13_c::setup_s_::meas_dur_r13_e_::to_number() const
{
  const static uint8_t options[] = {1, 14, 28, 42, 70};
  return get_enum_number(options, 5, value, "rmtc_cfg_r13_c::setup_s_::meas_dur_r13_e_");
}

std::string rrc_conn_recfg_v1250_ies_s::wlan_offload_info_r12_c_::setup_s_::t350_r12_e_::to_string() const
{
  switch (value) {
    case min5:
      return "min5";
    case min10:
      return "min10";
    case min20:
      return "min20";
    case min30:
      return "min30";
    case min60:
      return "min60";
    case min120:
      return "min120";
    case min180:
      return "min180";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "rrc_conn_recfg_v1250_ies_s::wlan_offload_info_r12_c_::setup_s_::t350_r12_e_");
  }
  return "";
}
uint8_t rrc_conn_recfg_v1250_ies_s::wlan_offload_info_r12_c_::setup_s_::t350_r12_e_::to_number() const
{
  const static uint8_t options[] = {5, 10, 20, 30, 60, 120, 180};
  return get_enum_number(options, 7, value,
                         "rrc_conn_recfg_v1250_ies_s::wlan_offload_info_r12_c_::setup_s_::t350_r12_e_");
}

std::string rrc_conn_release_v1530_ies_s::cn_type_r15_e_::to_string() const
{
  switch (value) {
    case epc:
      return "epc";
    case fivegc:
      return "fivegc";
    default:
      invalid_enum_value(value, "rrc_conn_release_v1530_ies_s::cn_type_r15_e_");
  }
  return "";
}
uint8_t rrc_conn_release_v1530_ies_s::cn_type_r15_e_::to_number() const
{
  switch (value) {
    case fivegc:
      return 5;
    default:
      invalid_enum_number(value, "rrc_conn_release_v1530_ies_s::cn_type_r15_e_");
  }
  return 0;
}

std::string rs_cfg_ssb_nr_r15_s::subcarrier_spacing_ssb_r15_e_::to_string() const
{
  switch (value) {
    case k_hz15:
      return "kHz15";
    case k_hz30:
      return "kHz30";
    case k_hz120:
      return "kHz120";
    case k_hz240:
      return "kHz240";
    default:
      invalid_enum_value(value, "rs_cfg_ssb_nr_r15_s::subcarrier_spacing_ssb_r15_e_");
  }
  return "";
}
uint8_t rs_cfg_ssb_nr_r15_s::subcarrier_spacing_ssb_r15_e_::to_number() const
{
  const static uint8_t options[] = {15, 30, 120, 240};
  return get_enum_number(options, 4, value, "rs_cfg_ssb_nr_r15_s::subcarrier_spacing_ssb_r15_e_");
}

// ReportInterval ::= ENUMERATED
std::string report_interv_e::to_string() const
{
  switch (value) {
    case ms120:
      return "ms120";
    case ms240:
      return "ms240";
    case ms480:
      return "ms480";
    case ms640:
      return "ms640";
    case ms1024:
      return "ms1024";
    case ms2048:
      return "ms2048";
    case ms5120:
      return "ms5120";
    case ms10240:
      return "ms10240";
    case min1:
      return "min1";
    case min6:
      return "min6";
    case min12:
      return "min12";
    case min30:
      return "min30";
    case min60:
      return "min60";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "report_interv_e");
  }
  return "";
}
uint16_t report_interv_e::to_number() const
{
  const static uint16_t options[] = {120, 240, 480, 640, 1024, 2048, 5120, 10240, 1, 6, 12, 30, 60};
  return get_enum_number(options, 13, value, "report_interv_e");
}

std::string thres_eutra_c::types::to_string() const
{
  switch (value) {
    case thres_rsrp:
      return "threshold-RSRP";
    case thres_rsrq:
      return "threshold-RSRQ";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "thres_eutra_c::types");
  }
  return "";
}

std::string thres_nr_r15_c::types::to_string() const
{
  switch (value) {
    case nr_rsrp_r15:
      return "nr-RSRP-r15";
    case nr_rsrq_r15:
      return "nr-RSRQ-r15";
    case nr_sinr_r15:
      return "nr-SINR-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "thres_nr_r15_c::types");
  }
  return "";
}

std::string thres_utra_c::types::to_string() const
{
  switch (value) {
    case utra_rscp:
      return "utra-RSCP";
    case utra_ec_n0:
      return "utra-EcN0";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "thres_utra_c::types");
  }
  return "";
}
uint8_t thres_utra_c::types::to_number() const
{
  switch (value) {
    case utra_ec_n0:
      return 0;
    default:
      invalid_enum_number(value, "thres_utra_c::types");
  }
  return 0;
}

// TimeToTrigger ::= ENUMERATED
std::string time_to_trigger_e::to_string() const
{
  switch (value) {
    case ms0:
      return "ms0";
    case ms40:
      return "ms40";
    case ms64:
      return "ms64";
    case ms80:
      return "ms80";
    case ms100:
      return "ms100";
    case ms128:
      return "ms128";
    case ms160:
      return "ms160";
    case ms256:
      return "ms256";
    case ms320:
      return "ms320";
    case ms480:
      return "ms480";
    case ms512:
      return "ms512";
    case ms640:
      return "ms640";
    case ms1024:
      return "ms1024";
    case ms1280:
      return "ms1280";
    case ms2560:
      return "ms2560";
    case ms5120:
      return "ms5120";
    default:
      invalid_enum_value(value, "time_to_trigger_e");
  }
  return "";
}
uint16_t time_to_trigger_e::to_number() const
{
  const static uint16_t options[] = {0, 40, 64, 80, 100, 128, 160, 256, 320, 480, 512, 640, 1024, 1280, 2560, 5120};
  return get_enum_number(options, 16, value, "time_to_trigger_e");
}

std::string ul_delay_cfg_r13_c::setup_s_::delay_thres_r13_e_::to_string() const
{
  switch (value) {
    case ms30:
      return "ms30";
    case ms40:
      return "ms40";
    case ms50:
      return "ms50";
    case ms60:
      return "ms60";
    case ms70:
      return "ms70";
    case ms80:
      return "ms80";
    case ms90:
      return "ms90";
    case ms100:
      return "ms100";
    case ms150:
      return "ms150";
    case ms300:
      return "ms300";
    case ms500:
      return "ms500";
    case ms750:
      return "ms750";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "ul_delay_cfg_r13_c::setup_s_::delay_thres_r13_e_");
  }
  return "";
}
uint16_t ul_delay_cfg_r13_c::setup_s_::delay_thres_r13_e_::to_number() const
{
  const static uint16_t options[] = {30, 40, 50, 60, 70, 80, 90, 100, 150, 300, 500, 750};
  return get_enum_number(options, 12, value, "ul_delay_cfg_r13_c::setup_s_::delay_thres_r13_e_");
}

// WLAN-BandIndicator-r13 ::= ENUMERATED
std::string wlan_band_ind_r13_e::to_string() const
{
  switch (value) {
    case band2dot4:
      return "band2dot4";
    case band5:
      return "band5";
    case band60_v1430:
      return "band60-v1430";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "wlan_band_ind_r13_e");
  }
  return "";
}
float wlan_band_ind_r13_e::to_number() const
{
  const static float options[] = {2.4, 5.0, 60.0};
  return get_enum_number(options, 3, value, "wlan_band_ind_r13_e");
}
std::string wlan_band_ind_r13_e::to_number_string() const
{
  switch (value) {
    case band2dot4:
      return "2.4";
    case band5:
      return "5";
    case band60_v1430:
      return "60";
    default:
      invalid_enum_number(value, "wlan_band_ind_r13_e");
  }
  return "";
}

std::string wlan_carrier_info_r13_s::country_code_r13_e_::to_string() const
{
  switch (value) {
    case united_states:
      return "unitedStates";
    case europe:
      return "europe";
    case japan:
      return "japan";
    case global:
      return "global";
    default:
      invalid_enum_value(value, "wlan_carrier_info_r13_s::country_code_r13_e_");
  }
  return "";
}

std::string idc_cfg_r11_s::autonomous_denial_params_r11_s_::autonomous_denial_sfs_r11_e_::to_string() const
{
  switch (value) {
    case n2:
      return "n2";
    case n5:
      return "n5";
    case n10:
      return "n10";
    case n15:
      return "n15";
    case n20:
      return "n20";
    case n30:
      return "n30";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "idc_cfg_r11_s::autonomous_denial_params_r11_s_::autonomous_denial_sfs_r11_e_");
  }
  return "";
}
uint8_t idc_cfg_r11_s::autonomous_denial_params_r11_s_::autonomous_denial_sfs_r11_e_::to_number() const
{
  const static uint8_t options[] = {2, 5, 10, 15, 20, 30};
  return get_enum_number(options, 6, value,
                         "idc_cfg_r11_s::autonomous_denial_params_r11_s_::autonomous_denial_sfs_r11_e_");
}

std::string idc_cfg_r11_s::autonomous_denial_params_r11_s_::autonomous_denial_validity_r11_e_::to_string() const
{
  switch (value) {
    case sf200:
      return "sf200";
    case sf500:
      return "sf500";
    case sf1000:
      return "sf1000";
    case sf2000:
      return "sf2000";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "idc_cfg_r11_s::autonomous_denial_params_r11_s_::autonomous_denial_validity_r11_e_");
  }
  return "";
}
uint16_t idc_cfg_r11_s::autonomous_denial_params_r11_s_::autonomous_denial_validity_r11_e_::to_number() const
{
  const static uint16_t options[] = {200, 500, 1000, 2000};
  return get_enum_number(options, 4, value,
                         "idc_cfg_r11_s::autonomous_denial_params_r11_s_::autonomous_denial_validity_r11_e_");
}

std::string meas_obj_eutra_s::t312_r12_c_::setup_e_::to_string() const
{
  switch (value) {
    case ms0:
      return "ms0";
    case ms50:
      return "ms50";
    case ms100:
      return "ms100";
    case ms200:
      return "ms200";
    case ms300:
      return "ms300";
    case ms400:
      return "ms400";
    case ms500:
      return "ms500";
    case ms1000:
      return "ms1000";
    default:
      invalid_enum_value(value, "meas_obj_eutra_s::t312_r12_c_::setup_e_");
  }
  return "";
}
uint16_t meas_obj_eutra_s::t312_r12_c_::setup_e_::to_number() const
{
  const static uint16_t options[] = {0, 50, 100, 200, 300, 400, 500, 1000};
  return get_enum_number(options, 8, value, "meas_obj_eutra_s::t312_r12_c_::setup_e_");
}

std::string meas_obj_utra_s::cells_to_add_mod_list_c_::types::to_string() const
{
  switch (value) {
    case cells_to_add_mod_list_utra_fdd:
      return "cellsToAddModListUTRA-FDD";
    case cells_to_add_mod_list_utra_tdd:
      return "cellsToAddModListUTRA-TDD";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "meas_obj_utra_s::cells_to_add_mod_list_c_::types");
  }
  return "";
}

std::string meas_obj_utra_s::cell_for_which_to_report_cgi_c_::types::to_string() const
{
  switch (value) {
    case utra_fdd:
      return "utra-FDD";
    case utra_tdd:
      return "utra-TDD";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "meas_obj_utra_s::cell_for_which_to_report_cgi_c_::types");
  }
  return "";
}

std::string meas_obj_wlan_r13_s::carrier_freq_r13_c_::types::to_string() const
{
  switch (value) {
    case band_ind_list_wlan_r13:
      return "bandIndicatorListWLAN-r13";
    case carrier_info_list_wlan_r13:
      return "carrierInfoListWLAN-r13";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "meas_obj_wlan_r13_s::carrier_freq_r13_c_::types");
  }
  return "";
}

std::string pwr_pref_ind_cfg_r11_c::setup_s_::pwr_pref_ind_timer_r11_e_::to_string() const
{
  switch (value) {
    case s0:
      return "s0";
    case s0dot5:
      return "s0dot5";
    case s1:
      return "s1";
    case s2:
      return "s2";
    case s5:
      return "s5";
    case s10:
      return "s10";
    case s20:
      return "s20";
    case s30:
      return "s30";
    case s60:
      return "s60";
    case s90:
      return "s90";
    case s120:
      return "s120";
    case s300:
      return "s300";
    case s600:
      return "s600";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "pwr_pref_ind_cfg_r11_c::setup_s_::pwr_pref_ind_timer_r11_e_");
  }
  return "";
}
float pwr_pref_ind_cfg_r11_c::setup_s_::pwr_pref_ind_timer_r11_e_::to_number() const
{
  const static float options[] = {0.0, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 30.0, 60.0, 90.0, 120.0, 300.0, 600.0};
  return get_enum_number(options, 13, value, "pwr_pref_ind_cfg_r11_c::setup_s_::pwr_pref_ind_timer_r11_e_");
}
std::string pwr_pref_ind_cfg_r11_c::setup_s_::pwr_pref_ind_timer_r11_e_::to_number_string() const
{
  switch (value) {
    case s0:
      return "0";
    case s0dot5:
      return "0.5";
    case s1:
      return "1";
    case s2:
      return "2";
    case s5:
      return "5";
    case s10:
      return "10";
    case s20:
      return "20";
    case s30:
      return "30";
    case s60:
      return "60";
    case s90:
      return "90";
    case s120:
      return "120";
    case s300:
      return "300";
    case s600:
      return "600";
    default:
      invalid_enum_number(value, "pwr_pref_ind_cfg_r11_c::setup_s_::pwr_pref_ind_timer_r11_e_");
  }
  return "";
}

std::string report_cfg_eutra_s::trigger_type_c_::event_s_::event_id_c_::types::to_string() const
{
  switch (value) {
    case event_a1:
      return "eventA1";
    case event_a2:
      return "eventA2";
    case event_a3:
      return "eventA3";
    case event_a4:
      return "eventA4";
    case event_a5:
      return "eventA5";
    case event_a6_r10:
      return "eventA6-r10";
    case event_c1_r12:
      return "eventC1-r12";
    case event_c2_r12:
      return "eventC2-r12";
    case event_v1_r14:
      return "eventV1-r14";
    case event_v2_r14:
      return "eventV2-r14";
    case event_h1_r15:
      return "eventH1-r15";
    case event_h2_r15:
      return "eventH2-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "report_cfg_eutra_s::trigger_type_c_::event_s_::event_id_c_::types");
  }
  return "";
}

std::string report_cfg_eutra_s::trigger_type_c_::periodical_s_::purpose_e_::to_string() const
{
  switch (value) {
    case report_strongest_cells:
      return "reportStrongestCells";
    case report_cgi:
      return "reportCGI";
    default:
      invalid_enum_value(value, "report_cfg_eutra_s::trigger_type_c_::periodical_s_::purpose_e_");
  }
  return "";
}

std::string report_cfg_eutra_s::trigger_type_c_::types::to_string() const
{
  switch (value) {
    case event:
      return "event";
    case periodical:
      return "periodical";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "report_cfg_eutra_s::trigger_type_c_::types");
  }
  return "";
}

std::string report_cfg_eutra_s::trigger_quant_e_::to_string() const
{
  switch (value) {
    case rsrp:
      return "rsrp";
    case rsrq:
      return "rsrq";
    default:
      invalid_enum_value(value, "report_cfg_eutra_s::trigger_quant_e_");
  }
  return "";
}

std::string report_cfg_eutra_s::report_quant_e_::to_string() const
{
  switch (value) {
    case same_as_trigger_quant:
      return "sameAsTriggerQuantity";
    case both:
      return "both";
    default:
      invalid_enum_value(value, "report_cfg_eutra_s::report_quant_e_");
  }
  return "";
}

std::string report_cfg_eutra_s::report_amount_e_::to_string() const
{
  switch (value) {
    case r1:
      return "r1";
    case r2:
      return "r2";
    case r4:
      return "r4";
    case r8:
      return "r8";
    case r16:
      return "r16";
    case r32:
      return "r32";
    case r64:
      return "r64";
    case infinity:
      return "infinity";
    default:
      invalid_enum_value(value, "report_cfg_eutra_s::report_amount_e_");
  }
  return "";
}
int8_t report_cfg_eutra_s::report_amount_e_::to_number() const
{
  const static int8_t options[] = {1, 2, 4, 8, 16, 32, 64, -1};
  return get_enum_number(options, 8, value, "report_cfg_eutra_s::report_amount_e_");
}

std::string report_cfg_eutra_s::rs_sinr_cfg_r13_c_::setup_s_::report_quant_v1310_e_::to_string() const
{
  switch (value) {
    case rsrp_andsinr:
      return "rsrpANDsinr";
    case rsrq_andsinr:
      return "rsrqANDsinr";
    case all:
      return "all";
    default:
      invalid_enum_value(value, "report_cfg_eutra_s::rs_sinr_cfg_r13_c_::setup_s_::report_quant_v1310_e_");
  }
  return "";
}

std::string report_cfg_eutra_s::purpose_v1430_e_::to_string() const
{
  switch (value) {
    case report_location:
      return "reportLocation";
    case sidelink:
      return "sidelink";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "report_cfg_eutra_s::purpose_v1430_e_");
  }
  return "";
}

std::string
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b1_s_::b1_thres_c_::types::to_string() const
{
  switch (value) {
    case b1_thres_utra:
      return "b1-ThresholdUTRA";
    case b1_thres_geran:
      return "b1-ThresholdGERAN";
    case b1_thres_cdma2000:
      return "b1-ThresholdCDMA2000";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(
          value, "report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b1_s_::b1_thres_c_::types");
  }
  return "";
}

std::string
report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b2_s_::b2_thres2_c_::types::to_string() const
{
  switch (value) {
    case b2_thres2_utra:
      return "b2-Threshold2UTRA";
    case b2_thres2_geran:
      return "b2-Threshold2GERAN";
    case b2_thres2_cdma2000:
      return "b2-Threshold2CDMA2000";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(
          value, "report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::event_b2_s_::b2_thres2_c_::types");
  }
  return "";
}

std::string report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::types::to_string() const
{
  switch (value) {
    case event_b1:
      return "eventB1";
    case event_b2:
      return "eventB2";
    case event_w1_r13:
      return "eventW1-r13";
    case event_w2_r13:
      return "eventW2-r13";
    case event_w3_r13:
      return "eventW3-r13";
    case event_b1_nr_r15:
      return "eventB1-NR-r15";
    case event_b2_nr_r15:
      return "eventB2-NR-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "report_cfg_inter_rat_s::trigger_type_c_::event_s_::event_id_c_::types");
  }
  return "";
}

std::string report_cfg_inter_rat_s::trigger_type_c_::periodical_s_::purpose_e_::to_string() const
{
  switch (value) {
    case report_strongest_cells:
      return "reportStrongestCells";
    case report_strongest_cells_for_son:
      return "reportStrongestCellsForSON";
    case report_cgi:
      return "reportCGI";
    default:
      invalid_enum_value(value, "report_cfg_inter_rat_s::trigger_type_c_::periodical_s_::purpose_e_");
  }
  return "";
}

std::string report_cfg_inter_rat_s::trigger_type_c_::types::to_string() const
{
  switch (value) {
    case event:
      return "event";
    case periodical:
      return "periodical";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "report_cfg_inter_rat_s::trigger_type_c_::types");
  }
  return "";
}

std::string report_cfg_inter_rat_s::report_amount_e_::to_string() const
{
  switch (value) {
    case r1:
      return "r1";
    case r2:
      return "r2";
    case r4:
      return "r4";
    case r8:
      return "r8";
    case r16:
      return "r16";
    case r32:
      return "r32";
    case r64:
      return "r64";
    case infinity:
      return "infinity";
    default:
      invalid_enum_value(value, "report_cfg_inter_rat_s::report_amount_e_");
  }
  return "";
}
int8_t report_cfg_inter_rat_s::report_amount_e_::to_number() const
{
  const static int8_t options[] = {1, 2, 4, 8, 16, 32, 64, -1};
  return get_enum_number(options, 8, value, "report_cfg_inter_rat_s::report_amount_e_");
}

std::string report_cfg_inter_rat_s::report_sftd_meas_r15_e_::to_string() const
{
  switch (value) {
    case p_scell:
      return "pSCell";
    case neighbor_cells:
      return "neighborCells";
    default:
      invalid_enum_value(value, "report_cfg_inter_rat_s::report_sftd_meas_r15_e_");
  }
  return "";
}

std::string meas_obj_to_add_mod_s::meas_obj_c_::types::to_string() const
{
  switch (value) {
    case meas_obj_eutra:
      return "measObjectEUTRA";
    case meas_obj_utra:
      return "measObjectUTRA";
    case meas_obj_geran:
      return "measObjectGERAN";
    case meas_obj_cdma2000:
      return "measObjectCDMA2000";
    case meas_obj_wlan_r13:
      return "measObjectWLAN-r13";
    case meas_obj_nr_r15:
      return "measObjectNR-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "meas_obj_to_add_mod_s::meas_obj_c_::types");
  }
  return "";
}
uint16_t meas_obj_to_add_mod_s::meas_obj_c_::types::to_number() const
{
  switch (value) {
    case meas_obj_cdma2000:
      return 2000;
    default:
      invalid_enum_number(value, "meas_obj_to_add_mod_s::meas_obj_c_::types");
  }
  return 0;
}

std::string meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_::types::to_string() const
{
  switch (value) {
    case meas_obj_eutra_r13:
      return "measObjectEUTRA-r13";
    case meas_obj_utra_r13:
      return "measObjectUTRA-r13";
    case meas_obj_geran_r13:
      return "measObjectGERAN-r13";
    case meas_obj_cdma2000_r13:
      return "measObjectCDMA2000-r13";
    case meas_obj_wlan_v1320:
      return "measObjectWLAN-v1320";
    case meas_obj_nr_r15:
      return "measObjectNR-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_::types");
  }
  return "";
}
uint16_t meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_::types::to_number() const
{
  switch (value) {
    case meas_obj_cdma2000_r13:
      return 2000;
    default:
      invalid_enum_number(value, "meas_obj_to_add_mod_ext_r13_s::meas_obj_r13_c_::types");
  }
  return 0;
}

std::string other_cfg_r9_s::bw_pref_ind_timer_r14_e_::to_string() const
{
  switch (value) {
    case s0:
      return "s0";
    case s0dot5:
      return "s0dot5";
    case s1:
      return "s1";
    case s2:
      return "s2";
    case s5:
      return "s5";
    case s10:
      return "s10";
    case s20:
      return "s20";
    case s30:
      return "s30";
    case s60:
      return "s60";
    case s90:
      return "s90";
    case s120:
      return "s120";
    case s300:
      return "s300";
    case s600:
      return "s600";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "other_cfg_r9_s::bw_pref_ind_timer_r14_e_");
  }
  return "";
}
float other_cfg_r9_s::bw_pref_ind_timer_r14_e_::to_number() const
{
  const static float options[] = {0.0, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 30.0, 60.0, 90.0, 120.0, 300.0, 600.0};
  return get_enum_number(options, 13, value, "other_cfg_r9_s::bw_pref_ind_timer_r14_e_");
}
std::string other_cfg_r9_s::bw_pref_ind_timer_r14_e_::to_number_string() const
{
  switch (value) {
    case s0:
      return "0";
    case s0dot5:
      return "0.5";
    case s1:
      return "1";
    case s2:
      return "2";
    case s5:
      return "5";
    case s10:
      return "10";
    case s20:
      return "20";
    case s30:
      return "30";
    case s60:
      return "60";
    case s90:
      return "90";
    case s120:
      return "120";
    case s300:
      return "300";
    case s600:
      return "600";
    default:
      invalid_enum_number(value, "other_cfg_r9_s::bw_pref_ind_timer_r14_e_");
  }
  return "";
}

std::string
other_cfg_r9_s::delay_budget_report_cfg_r14_c_::setup_s_::delay_budget_report_prohibit_timer_r14_e_::to_string() const
{
  switch (value) {
    case s0:
      return "s0";
    case s0dot4:
      return "s0dot4";
    case s0dot8:
      return "s0dot8";
    case s1dot6:
      return "s1dot6";
    case s3:
      return "s3";
    case s6:
      return "s6";
    case s12:
      return "s12";
    case s30:
      return "s30";
    default:
      invalid_enum_value(
          value, "other_cfg_r9_s::delay_budget_report_cfg_r14_c_::setup_s_::delay_budget_report_prohibit_timer_r14_e_");
  }
  return "";
}
float other_cfg_r9_s::delay_budget_report_cfg_r14_c_::setup_s_::delay_budget_report_prohibit_timer_r14_e_::to_number()
    const
{
  const static float options[] = {0.0, 0.4, 0.8, 1.6, 3.0, 6.0, 12.0, 30.0};
  return get_enum_number(
      options, 8, value,
      "other_cfg_r9_s::delay_budget_report_cfg_r14_c_::setup_s_::delay_budget_report_prohibit_timer_r14_e_");
}
std::string
other_cfg_r9_s::delay_budget_report_cfg_r14_c_::setup_s_::delay_budget_report_prohibit_timer_r14_e_::to_number_string()
    const
{
  switch (value) {
    case s0:
      return "0";
    case s0dot4:
      return "0.4";
    case s0dot8:
      return "0.8";
    case s1dot6:
      return "1.6";
    case s3:
      return "3";
    case s6:
      return "6";
    case s12:
      return "12";
    case s30:
      return "30";
    default:
      invalid_enum_number(
          value, "other_cfg_r9_s::delay_budget_report_cfg_r14_c_::setup_s_::delay_budget_report_prohibit_timer_r14_e_");
  }
  return "";
}

std::string other_cfg_r9_s::rlm_report_cfg_r14_c_::setup_s_::rlm_report_timer_r14_e_::to_string() const
{
  switch (value) {
    case s0:
      return "s0";
    case s0dot5:
      return "s0dot5";
    case s1:
      return "s1";
    case s2:
      return "s2";
    case s5:
      return "s5";
    case s10:
      return "s10";
    case s20:
      return "s20";
    case s30:
      return "s30";
    case s60:
      return "s60";
    case s90:
      return "s90";
    case s120:
      return "s120";
    case s300:
      return "s300";
    case s600:
      return "s600";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "other_cfg_r9_s::rlm_report_cfg_r14_c_::setup_s_::rlm_report_timer_r14_e_");
  }
  return "";
}
float other_cfg_r9_s::rlm_report_cfg_r14_c_::setup_s_::rlm_report_timer_r14_e_::to_number() const
{
  const static float options[] = {0.0, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 30.0, 60.0, 90.0, 120.0, 300.0, 600.0};
  return get_enum_number(options, 13, value,
                         "other_cfg_r9_s::rlm_report_cfg_r14_c_::setup_s_::rlm_report_timer_r14_e_");
}
std::string other_cfg_r9_s::rlm_report_cfg_r14_c_::setup_s_::rlm_report_timer_r14_e_::to_number_string() const
{
  switch (value) {
    case s0:
      return "0";
    case s0dot5:
      return "0.5";
    case s1:
      return "1";
    case s2:
      return "2";
    case s5:
      return "5";
    case s10:
      return "10";
    case s20:
      return "20";
    case s30:
      return "30";
    case s60:
      return "60";
    case s90:
      return "90";
    case s120:
      return "120";
    case s300:
      return "300";
    case s600:
      return "600";
    default:
      invalid_enum_number(value, "other_cfg_r9_s::rlm_report_cfg_r14_c_::setup_s_::rlm_report_timer_r14_e_");
  }
  return "";
}

std::string other_cfg_r9_s::overheat_assist_cfg_r14_c_::setup_s_::overheat_ind_prohibit_timer_r14_e_::to_string() const
{
  switch (value) {
    case s0:
      return "s0";
    case s0dot5:
      return "s0dot5";
    case s1:
      return "s1";
    case s2:
      return "s2";
    case s5:
      return "s5";
    case s10:
      return "s10";
    case s20:
      return "s20";
    case s30:
      return "s30";
    case s60:
      return "s60";
    case s90:
      return "s90";
    case s120:
      return "s120";
    case s300:
      return "s300";
    case s600:
      return "s600";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value,
                         "other_cfg_r9_s::overheat_assist_cfg_r14_c_::setup_s_::overheat_ind_prohibit_timer_r14_e_");
  }
  return "";
}
float other_cfg_r9_s::overheat_assist_cfg_r14_c_::setup_s_::overheat_ind_prohibit_timer_r14_e_::to_number() const
{
  const static float options[] = {0.0, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 30.0, 60.0, 90.0, 120.0, 300.0, 600.0};
  return get_enum_number(options, 13, value,
                         "other_cfg_r9_s::overheat_assist_cfg_r14_c_::setup_s_::overheat_ind_prohibit_timer_r14_e_");
}
std::string
other_cfg_r9_s::overheat_assist_cfg_r14_c_::setup_s_::overheat_ind_prohibit_timer_r14_e_::to_number_string() const
{
  switch (value) {
    case s0:
      return "0";
    case s0dot5:
      return "0.5";
    case s1:
      return "1";
    case s2:
      return "2";
    case s5:
      return "5";
    case s10:
      return "10";
    case s20:
      return "20";
    case s30:
      return "30";
    case s60:
      return "60";
    case s90:
      return "90";
    case s120:
      return "120";
    case s300:
      return "300";
    case s600:
      return "600";
    default:
      invalid_enum_number(value,
                          "other_cfg_r9_s::overheat_assist_cfg_r14_c_::setup_s_::overheat_ind_prohibit_timer_r14_e_");
  }
  return "";
}

std::string other_cfg_r9_s::meas_cfg_app_layer_r15_c_::setup_s_::service_type_e_::to_string() const
{
  switch (value) {
    case qoe:
      return "qoe";
    case qoemtsi:
      return "qoemtsi";
    case spare6:
      return "spare6";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "other_cfg_r9_s::meas_cfg_app_layer_r15_c_::setup_s_::service_type_e_");
  }
  return "";
}

std::string prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_e_::to_string() const
{
  switch (value) {
    case v1:
      return "v1";
    case v1dot5:
      return "v1dot5";
    case v2:
      return "v2";
    case v2dot5:
      return "v2dot5";
    case v4:
      return "v4";
    case v5:
      return "v5";
    case v8:
      return "v8";
    case v10:
      return "v10";
    default:
      invalid_enum_value(value, "prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_e_");
  }
  return "";
}
float prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_e_::to_number() const
{
  const static float options[] = {1.0, 1.5, 2.0, 2.5, 4.0, 5.0, 8.0, 10.0};
  return get_enum_number(options, 8, value, "prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_e_");
}
std::string prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_e_::to_number_string() const
{
  switch (value) {
    case v1:
      return "1";
    case v1dot5:
      return "1.5";
    case v2:
      return "2";
    case v2dot5:
      return "2.5";
    case v4:
      return "4";
    case v5:
      return "5";
    case v8:
      return "8";
    case v10:
      return "10";
    default:
      invalid_enum_number(value, "prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_e_");
  }
  return "";
}

std::string prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::tdd_r13_e_::to_string() const
{
  switch (value) {
    case v1:
      return "v1";
    case v2:
      return "v2";
    case v4:
      return "v4";
    case v5:
      return "v5";
    case v8:
      return "v8";
    case v10:
      return "v10";
    case v20:
      return "v20";
    case spare:
      return "spare";
    default:
      invalid_enum_value(value, "prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::tdd_r13_e_");
  }
  return "";
}
uint8_t prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::tdd_r13_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 4, 5, 8, 10, 20};
  return get_enum_number(options, 7, value, "prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::tdd_r13_e_");
}

std::string prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::types::to_string() const
{
  switch (value) {
    case fdd_r13:
      return "fdd-r13";
    case tdd_r13:
      return "tdd-r13";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::types");
  }
  return "";
}

std::string quant_cfg_cdma2000_s::meas_quant_cdma2000_e_::to_string() const
{
  switch (value) {
    case pilot_strength:
      return "pilotStrength";
    case pilot_pn_phase_and_pilot_strength:
      return "pilotPnPhaseAndPilotStrength";
    default:
      invalid_enum_value(value, "quant_cfg_cdma2000_s::meas_quant_cdma2000_e_");
  }
  return "";
}

std::string quant_cfg_utra_s::meas_quant_utra_fdd_e_::to_string() const
{
  switch (value) {
    case cpich_rscp:
      return "cpich-RSCP";
    case cpich_ec_n0:
      return "cpich-EcN0";
    default:
      invalid_enum_value(value, "quant_cfg_utra_s::meas_quant_utra_fdd_e_");
  }
  return "";
}
uint8_t quant_cfg_utra_s::meas_quant_utra_fdd_e_::to_number() const
{
  switch (value) {
    case cpich_ec_n0:
      return 0;
    default:
      invalid_enum_number(value, "quant_cfg_utra_s::meas_quant_utra_fdd_e_");
  }
  return 0;
}

std::string report_cfg_to_add_mod_s::report_cfg_c_::types::to_string() const
{
  switch (value) {
    case report_cfg_eutra:
      return "reportConfigEUTRA";
    case report_cfg_inter_rat:
      return "reportConfigInterRAT";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "report_cfg_to_add_mod_s::report_cfg_c_::types");
  }
  return "";
}

std::string carrier_bw_eutra_s::dl_bw_e_::to_string() const
{
  switch (value) {
    case n6:
      return "n6";
    case n15:
      return "n15";
    case n25:
      return "n25";
    case n50:
      return "n50";
    case n75:
      return "n75";
    case n100:
      return "n100";
    case spare10:
      return "spare10";
    case spare9:
      return "spare9";
    case spare8:
      return "spare8";
    case spare7:
      return "spare7";
    case spare6:
      return "spare6";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "carrier_bw_eutra_s::dl_bw_e_");
  }
  return "";
}
uint8_t carrier_bw_eutra_s::dl_bw_e_::to_number() const
{
  const static uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return get_enum_number(options, 6, value, "carrier_bw_eutra_s::dl_bw_e_");
}

std::string carrier_bw_eutra_s::ul_bw_e_::to_string() const
{
  switch (value) {
    case n6:
      return "n6";
    case n15:
      return "n15";
    case n25:
      return "n25";
    case n50:
      return "n50";
    case n75:
      return "n75";
    case n100:
      return "n100";
    case spare10:
      return "spare10";
    case spare9:
      return "spare9";
    case spare8:
      return "spare8";
    case spare7:
      return "spare7";
    case spare6:
      return "spare6";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "carrier_bw_eutra_s::ul_bw_e_");
  }
  return "";
}
uint8_t carrier_bw_eutra_s::ul_bw_e_::to_number() const
{
  const static uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return get_enum_number(options, 6, value, "carrier_bw_eutra_s::ul_bw_e_");
}

std::string carrier_info_nr_r15_s::subcarrier_spacing_ssb_r15_e_::to_string() const
{
  switch (value) {
    case k_hz15:
      return "kHz15";
    case k_hz30:
      return "kHz30";
    case k_hz120:
      return "kHz120";
    case k_hz240:
      return "kHz240";
    default:
      invalid_enum_value(value, "carrier_info_nr_r15_s::subcarrier_spacing_ssb_r15_e_");
  }
  return "";
}
uint8_t carrier_info_nr_r15_s::subcarrier_spacing_ssb_r15_e_::to_number() const
{
  const static uint8_t options[] = {15, 30, 120, 240};
  return get_enum_number(options, 4, value, "carrier_info_nr_r15_s::subcarrier_spacing_ssb_r15_e_");
}

std::string meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::types::to_string() const
{
  switch (value) {
    case rstd0_r15:
      return "rstd0-r15";
    case rstd1_r15:
      return "rstd1-r15";
    case rstd2_r15:
      return "rstd2-r15";
    case rstd3_r15:
      return "rstd3-r15";
    case rstd4_r15:
      return "rstd4-r15";
    case rstd5_r15:
      return "rstd5-r15";
    case rstd6_r15:
      return "rstd6-r15";
    case rstd7_r15:
      return "rstd7-r15";
    case rstd8_r15:
      return "rstd8-r15";
    case rstd9_r15:
      return "rstd9-r15";
    case rstd10_r15:
      return "rstd10-r15";
    case rstd11_r15:
      return "rstd11-r15";
    case rstd12_r15:
      return "rstd12-r15";
    case rstd13_r15:
      return "rstd13-r15";
    case rstd14_r15:
      return "rstd14-r15";
    case rstd15_r15:
      return "rstd15-r15";
    case rstd16_r15:
      return "rstd16-r15";
    case rstd17_r15:
      return "rstd17-r15";
    case rstd18_r15:
      return "rstd18-r15";
    case rstd19_r15:
      return "rstd19-r15";
    case rstd20_r15:
      return "rstd20-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::types");
  }
  return "";
}
uint8_t meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::types::to_number() const
{
  const static uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
  return get_enum_number(options, 21, value,
                         "meas_gap_cfg_dense_prs_r15_c::setup_s_::gap_offset_dense_prs_r15_c_::types");
}

std::string meas_gap_sharing_cfg_r14_c::setup_s_::meas_gap_sharing_scheme_r14_e_::to_string() const
{
  switch (value) {
    case scheme00:
      return "scheme00";
    case scheme01:
      return "scheme01";
    case scheme10:
      return "scheme10";
    case scheme11:
      return "scheme11";
    default:
      invalid_enum_value(value, "meas_gap_sharing_cfg_r14_c::setup_s_::meas_gap_sharing_scheme_r14_e_");
  }
  return "";
}
float meas_gap_sharing_cfg_r14_c::setup_s_::meas_gap_sharing_scheme_r14_e_::to_number() const
{
  const static float options[] = {0.0, 0.1, 1.0, 1.1};
  return get_enum_number(options, 4, value, "meas_gap_sharing_cfg_r14_c::setup_s_::meas_gap_sharing_scheme_r14_e_");
}
std::string meas_gap_sharing_cfg_r14_c::setup_s_::meas_gap_sharing_scheme_r14_e_::to_number_string() const
{
  switch (value) {
    case scheme00:
      return "0.0";
    case scheme01:
      return "0.1";
    case scheme10:
      return "1.0";
    case scheme11:
      return "1.1";
    default:
      invalid_enum_number(value, "meas_gap_sharing_cfg_r14_c::setup_s_::meas_gap_sharing_scheme_r14_e_");
  }
  return "";
}

// MeasScaleFactor-r12 ::= ENUMERATED
std::string meas_scale_factor_r12_e::to_string() const
{
  switch (value) {
    case sf_eutra_cf1:
      return "sf-EUTRA-cf1";
    case sf_eutra_cf2:
      return "sf-EUTRA-cf2";
    default:
      invalid_enum_value(value, "meas_scale_factor_r12_e");
  }
  return "";
}
uint8_t meas_scale_factor_r12_e::to_number() const
{
  const static uint8_t options[] = {1, 2};
  return get_enum_number(options, 2, value, "meas_scale_factor_r12_e");
}

// RAT-Type ::= ENUMERATED
std::string rat_type_e::to_string() const
{
  switch (value) {
    case eutra:
      return "eutra";
    case utra:
      return "utra";
    case geran_cs:
      return "geran-cs";
    case geran_ps:
      return "geran-ps";
    case cdma2000_minus1_xrtt:
      return "cdma2000-1XRTT";
    case nr:
      return "nr";
    case eutra_nr:
      return "eutra-nr";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "rat_type_e");
  }
  return "";
}
uint16_t rat_type_e::to_number() const
{
  switch (value) {
    case cdma2000_minus1_xrtt:
      return 2000;
    default:
      invalid_enum_number(value, "rat_type_e");
  }
  return 0;
}

std::string rrc_conn_release_v920_ies_s::cell_info_list_r9_c_::types::to_string() const
{
  switch (value) {
    case geran_r9:
      return "geran-r9";
    case utra_fdd_r9:
      return "utra-FDD-r9";
    case utra_tdd_r9:
      return "utra-TDD-r9";
    case utra_tdd_r10:
      return "utra-TDD-r10";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rrc_conn_release_v920_ies_s::cell_info_list_r9_c_::types");
  }
  return "";
}

std::string si_or_psi_geran_c::types::to_string() const
{
  switch (value) {
    case si:
      return "si";
    case psi:
      return "psi";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "si_or_psi_geran_c::types");
  }
  return "";
}

std::string area_cfg_r10_c::types::to_string() const
{
  switch (value) {
    case cell_global_id_list_r10:
      return "cellGlobalIdList-r10";
    case tac_list_r10:
      return "trackingAreaCodeList-r10";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "area_cfg_r10_c::types");
  }
  return "";
}

std::string cell_change_order_s::t304_e_::to_string() const
{
  switch (value) {
    case ms100:
      return "ms100";
    case ms200:
      return "ms200";
    case ms500:
      return "ms500";
    case ms1000:
      return "ms1000";
    case ms2000:
      return "ms2000";
    case ms4000:
      return "ms4000";
    case ms8000:
      return "ms8000";
    case ms10000_v1310:
      return "ms10000-v1310";
    default:
      invalid_enum_value(value, "cell_change_order_s::t304_e_");
  }
  return "";
}
uint16_t cell_change_order_s::t304_e_::to_number() const
{
  const static uint16_t options[] = {100, 200, 500, 1000, 2000, 4000, 8000, 10000};
  return get_enum_number(options, 8, value, "cell_change_order_s::t304_e_");
}

std::string cell_change_order_s::target_rat_type_c_::types::to_string() const
{
  switch (value) {
    case geran:
      return "geran";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "cell_change_order_s::target_rat_type_c_::types");
  }
  return "";
}

std::string e_csfb_r9_s::mob_cdma2000_hrpd_r9_e_::to_string() const
{
  switch (value) {
    case ho:
      return "handover";
    case redirection:
      return "redirection";
    default:
      invalid_enum_value(value, "e_csfb_r9_s::mob_cdma2000_hrpd_r9_e_");
  }
  return "";
}

std::string ho_s::target_rat_type_e_::to_string() const
{
  switch (value) {
    case utra:
      return "utra";
    case geran:
      return "geran";
    case cdma2000_minus1_xrtt:
      return "cdma2000-1XRTT";
    case cdma2000_hrpd:
      return "cdma2000-HRPD";
    case nr:
      return "nr";
    case eutra:
      return "eutra";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "ho_s::target_rat_type_e_");
  }
  return "";
}

// LoggingDuration-r10 ::= ENUMERATED
std::string logging_dur_r10_e::to_string() const
{
  switch (value) {
    case min10:
      return "min10";
    case min20:
      return "min20";
    case min40:
      return "min40";
    case min60:
      return "min60";
    case min90:
      return "min90";
    case min120:
      return "min120";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "logging_dur_r10_e");
  }
  return "";
}
uint8_t logging_dur_r10_e::to_number() const
{
  const static uint8_t options[] = {10, 20, 40, 60, 90, 120};
  return get_enum_number(options, 6, value, "logging_dur_r10_e");
}

// LoggingInterval-r10 ::= ENUMERATED
std::string logging_interv_r10_e::to_string() const
{
  switch (value) {
    case ms1280:
      return "ms1280";
    case ms2560:
      return "ms2560";
    case ms5120:
      return "ms5120";
    case ms10240:
      return "ms10240";
    case ms20480:
      return "ms20480";
    case ms30720:
      return "ms30720";
    case ms40960:
      return "ms40960";
    case ms61440:
      return "ms61440";
    default:
      invalid_enum_value(value, "logging_interv_r10_e");
  }
  return "";
}
uint16_t logging_interv_r10_e::to_number() const
{
  const static uint16_t options[] = {1280, 2560, 5120, 10240, 20480, 30720, 40960, 61440};
  return get_enum_number(options, 8, value, "logging_interv_r10_e");
}

std::string mob_ctrl_info_s::t304_e_::to_string() const
{
  switch (value) {
    case ms50:
      return "ms50";
    case ms100:
      return "ms100";
    case ms150:
      return "ms150";
    case ms200:
      return "ms200";
    case ms500:
      return "ms500";
    case ms1000:
      return "ms1000";
    case ms2000:
      return "ms2000";
    case ms10000_v1310:
      return "ms10000-v1310";
    default:
      invalid_enum_value(value, "mob_ctrl_info_s::t304_e_");
  }
  return "";
}
uint16_t mob_ctrl_info_s::t304_e_::to_number() const
{
  const static uint16_t options[] = {50, 100, 150, 200, 500, 1000, 2000, 10000};
  return get_enum_number(options, 8, value, "mob_ctrl_info_s::t304_e_");
}

std::string mob_ctrl_info_s::ho_without_wt_change_r14_e_::to_string() const
{
  switch (value) {
    case keep_lwa_cfg:
      return "keepLWA-Config";
    case send_end_marker:
      return "sendEndMarker";
    default:
      invalid_enum_value(value, "mob_ctrl_info_s::ho_without_wt_change_r14_e_");
  }
  return "";
}

std::string rn_sf_cfg_r10_s::sf_cfg_pattern_r10_c_::types::to_string() const
{
  switch (value) {
    case sf_cfg_pattern_fdd_r10:
      return "subframeConfigPatternFDD-r10";
    case sf_cfg_pattern_tdd_r10:
      return "subframeConfigPatternTDD-r10";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rn_sf_cfg_r10_s::sf_cfg_pattern_r10_c_::types");
  }
  return "";
}

std::string rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_alloc_type_r10_e_::to_string() const
{
  switch (value) {
    case type0:
      return "type0";
    case type1:
      return "type1";
    case type2_localized:
      return "type2Localized";
    case type2_distributed:
      return "type2Distributed";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_alloc_type_r10_e_");
  }
  return "";
}

std::string rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_::types::to_string() const
{
  switch (value) {
    case nrb6_r10:
      return "nrb6-r10";
    case nrb15_r10:
      return "nrb15-r10";
    case nrb25_r10:
      return "nrb25-r10";
    case nrb50_r10:
      return "nrb50-r10";
    case nrb75_r10:
      return "nrb75-r10";
    case nrb100_r10:
      return "nrb100-r10";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_::types");
  }
  return "";
}
uint8_t rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_::types::to_number() const
{
  const static uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return get_enum_number(options, 6, value,
                         "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type01_r10_c_::types");
}

std::string rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_::types::to_string() const
{
  switch (value) {
    case nrb6_r10:
      return "nrb6-r10";
    case nrb15_r10:
      return "nrb15-r10";
    case nrb25_r10:
      return "nrb25-r10";
    case nrb50_r10:
      return "nrb50-r10";
    case nrb75_r10:
      return "nrb75-r10";
    case nrb100_r10:
      return "nrb100-r10";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_::types");
  }
  return "";
}
uint8_t rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_::types::to_number() const
{
  const static uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return get_enum_number(options, 6, value,
                         "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::type2_r10_c_::types");
}

std::string rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::types::to_string() const
{
  switch (value) {
    case type01_r10:
      return "type01-r10";
    case type2_r10:
      return "type2-r10";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::types");
  }
  return "";
}
float rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::types::to_number() const
{
  const static float options[] = {0.1, 2.0};
  return get_enum_number(options, 2, value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::types");
}
std::string rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::types::to_number_string() const
{
  switch (value) {
    case type01_r10:
      return "0.1";
    case type2_r10:
      return "2";
    default:
      invalid_enum_number(value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::res_block_assign_r10_c_::types");
  }
  return "";
}

std::string rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::demod_rs_r10_c_::no_interleaving_r10_e_::to_string() const
{
  switch (value) {
    case crs:
      return "crs";
    case dmrs:
      return "dmrs";
    default:
      invalid_enum_value(value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::demod_rs_r10_c_::no_interleaving_r10_e_");
  }
  return "";
}

std::string rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::demod_rs_r10_c_::types::to_string() const
{
  switch (value) {
    case interleaving_r10:
      return "interleaving-r10";
    case no_interleaving_r10:
      return "noInterleaving-r10";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::demod_rs_r10_c_::types");
  }
  return "";
}

std::string rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_::types::to_string() const
{
  switch (value) {
    case ch_sel_mux_bundling:
      return "channelSelectionMultiplexingBundling";
    case fallback_for_format3:
      return "fallbackForFormat3";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_::types");
  }
  return "";
}
uint8_t rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_::types::to_number() const
{
  switch (value) {
    case fallback_for_format3:
      return 3;
    default:
      invalid_enum_number(value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::tdd_c_::types");
  }
  return 0;
}

std::string rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::types::to_string() const
{
  switch (value) {
    case tdd:
      return "tdd";
    case fdd:
      return "fdd";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rn_sf_cfg_r10_s::rpdcch_cfg_r10_s_::pucch_cfg_r10_c_::types");
  }
  return "";
}

std::string redirected_carrier_info_c::types::to_string() const
{
  switch (value) {
    case eutra:
      return "eutra";
    case geran:
      return "geran";
    case utra_fdd:
      return "utra-FDD";
    case utra_tdd:
      return "utra-TDD";
    case cdma2000_hrpd:
      return "cdma2000-HRPD";
    case cdma2000_minus1x_rtt:
      return "cdma2000-1xRTT";
    case utra_tdd_r10:
      return "utra-TDD-r10";
    case nr_r15:
      return "nr-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "redirected_carrier_info_c::types");
  }
  return "";
}

// ReleaseCause ::= ENUMERATED
std::string release_cause_e::to_string() const
{
  switch (value) {
    case load_balancing_ta_urequired:
      return "loadBalancingTAUrequired";
    case other:
      return "other";
    case cs_fallback_high_prio_v1020:
      return "cs-FallbackHighPriority-v1020";
    case rrc_suspend_v1320:
      return "rrc-Suspend-v1320";
    default:
      invalid_enum_value(value, "release_cause_e");
  }
  return "";
}

std::string security_cfg_ho_s::ho_type_c_::types::to_string() const
{
  switch (value) {
    case intra_lte:
      return "intraLTE";
    case inter_rat:
      return "interRAT";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "security_cfg_ho_s::ho_type_c_::types");
  }
  return "";
}

std::string dl_info_transfer_r15_ies_s::ded_info_type_r15_c_::types::to_string() const
{
  switch (value) {
    case ded_info_nas_r15:
      return "dedicatedInfoNAS-r15";
    case ded_info_cdma2000_minus1_xrtt_r15:
      return "dedicatedInfoCDMA2000-1XRTT-r15";
    case ded_info_cdma2000_hrpd_r15:
      return "dedicatedInfoCDMA2000-HRPD-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "dl_info_transfer_r15_ies_s::ded_info_type_r15_c_::types");
  }
  return "";
}

std::string dl_info_transfer_r8_ies_s::ded_info_type_c_::types::to_string() const
{
  switch (value) {
    case ded_info_nas:
      return "dedicatedInfoNAS";
    case ded_info_cdma2000_minus1_xrtt:
      return "dedicatedInfoCDMA2000-1XRTT";
    case ded_info_cdma2000_hrpd:
      return "dedicatedInfoCDMA2000-HRPD";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "dl_info_transfer_r8_ies_s::ded_info_type_c_::types");
  }
  return "";
}

std::string mob_from_eutra_cmd_r8_ies_s::purpose_c_::types::to_string() const
{
  switch (value) {
    case ho:
      return "handover";
    case cell_change_order:
      return "cellChangeOrder";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "mob_from_eutra_cmd_r8_ies_s::purpose_c_::types");
  }
  return "";
}

std::string mob_from_eutra_cmd_r9_ies_s::purpose_c_::types::to_string() const
{
  switch (value) {
    case ho:
      return "handover";
    case cell_change_order:
      return "cellChangeOrder";
    case e_csfb_r9:
      return "e-CSFB-r9";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "mob_from_eutra_cmd_r9_ies_s::purpose_c_::types");
  }
  return "";
}

std::string csfb_params_resp_cdma2000_s::crit_exts_c_::types::to_string() const
{
  switch (value) {
    case csfb_params_resp_cdma2000_r8:
      return "csfbParametersResponseCDMA2000-r8";
    case crit_exts_future:
      return "criticalExtensionsFuture";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "csfb_params_resp_cdma2000_s::crit_exts_c_::types");
  }
  return "";
}
uint16_t csfb_params_resp_cdma2000_s::crit_exts_c_::types::to_number() const
{
  const static uint16_t options[] = {2000};
  return get_enum_number(options, 1, value, "csfb_params_resp_cdma2000_s::crit_exts_c_::types");
}

std::string counter_check_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case counter_check_r8:
      return "counterCheck-r8";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "counter_check_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string dl_info_transfer_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case dl_info_transfer_r8:
      return "dlInformationTransfer-r8";
    case dl_info_transfer_r15:
      return "dlInformationTransfer-r15";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "dl_info_transfer_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string ho_from_eutra_prep_request_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case ho_from_eutra_prep_request_r8:
      return "handoverFromEUTRAPreparationRequest-r8";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ho_from_eutra_prep_request_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string logged_meas_cfg_r10_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case logged_meas_cfg_r10:
      return "loggedMeasurementConfiguration-r10";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "logged_meas_cfg_r10_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string mob_from_eutra_cmd_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case mob_from_eutra_cmd_r8:
      return "mobilityFromEUTRACommand-r8";
    case mob_from_eutra_cmd_r9:
      return "mobilityFromEUTRACommand-r9";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "mob_from_eutra_cmd_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string rn_recfg_r10_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case rn_recfg_r10:
      return "rnReconfiguration-r10";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rn_recfg_r10_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string rrc_conn_recfg_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case rrc_conn_recfg_r8:
      return "rrcConnectionReconfiguration-r8";
    case spare7:
      return "spare7";
    case spare6:
      return "spare6";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rrc_conn_recfg_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string rrc_conn_release_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case rrc_conn_release_r8:
      return "rrcConnectionRelease-r8";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rrc_conn_release_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string rrc_conn_resume_r13_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case rrc_conn_resume_r13:
      return "rrcConnectionResume-r13";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rrc_conn_resume_r13_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string security_mode_cmd_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case security_mode_cmd_r8:
      return "securityModeCommand-r8";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "security_mode_cmd_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string ue_cap_enquiry_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case ue_cap_enquiry_r8:
      return "ueCapabilityEnquiry-r8";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ue_cap_enquiry_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string ue_info_request_r9_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case ue_info_request_r9:
      return "ueInformationRequest-r9";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ue_info_request_r9_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string dl_dcch_msg_type_c::c1_c_::types::to_string() const
{
  switch (value) {
    case csfb_params_resp_cdma2000:
      return "csfbParametersResponseCDMA2000";
    case dl_info_transfer:
      return "dlInformationTransfer";
    case ho_from_eutra_prep_request:
      return "handoverFromEUTRAPreparationRequest";
    case mob_from_eutra_cmd:
      return "mobilityFromEUTRACommand";
    case rrc_conn_recfg:
      return "rrcConnectionReconfiguration";
    case rrc_conn_release:
      return "rrcConnectionRelease";
    case security_mode_cmd:
      return "securityModeCommand";
    case ue_cap_enquiry:
      return "ueCapabilityEnquiry";
    case counter_check:
      return "counterCheck";
    case ue_info_request_r9:
      return "ueInformationRequest-r9";
    case logged_meas_cfg_r10:
      return "loggedMeasurementConfiguration-r10";
    case rn_recfg_r10:
      return "rnReconfiguration-r10";
    case rrc_conn_resume_r13:
      return "rrcConnectionResume-r13";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "dl_dcch_msg_type_c::c1_c_::types");
  }
  return "";
}
uint16_t dl_dcch_msg_type_c::c1_c_::types::to_number() const
{
  const static uint16_t options[] = {2000};
  return get_enum_number(options, 1, value, "dl_dcch_msg_type_c::c1_c_::types");
}

std::string dl_dcch_msg_type_c::types::to_string() const
{
  switch (value) {
    case c1:
      return "c1";
    case msg_class_ext:
      return "messageClassExtension";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "dl_dcch_msg_type_c::types");
  }
  return "";
}
uint8_t dl_dcch_msg_type_c::types::to_number() const
{
  const static uint8_t options[] = {1};
  return get_enum_number(options, 1, value, "dl_dcch_msg_type_c::types");
}

std::string tmgi_r9_s::plmn_id_r9_c_::types::to_string() const
{
  switch (value) {
    case plmn_idx_r9:
      return "plmn-Index-r9";
    case explicit_value_r9:
      return "explicitValue-r9";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "tmgi_r9_s::plmn_id_r9_c_::types");
  }
  return "";
}

std::string pmch_cfg_r12_s::data_mcs_r12_c_::types::to_string() const
{
  switch (value) {
    case normal_r12:
      return "normal-r12";
    case higer_order_r12:
      return "higerOrder-r12";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "pmch_cfg_r12_s::data_mcs_r12_c_::types");
  }
  return "";
}

std::string pmch_cfg_r12_s::mch_sched_period_r12_e_::to_string() const
{
  switch (value) {
    case rf4:
      return "rf4";
    case rf8:
      return "rf8";
    case rf16:
      return "rf16";
    case rf32:
      return "rf32";
    case rf64:
      return "rf64";
    case rf128:
      return "rf128";
    case rf256:
      return "rf256";
    case rf512:
      return "rf512";
    case rf1024:
      return "rf1024";
    default:
      invalid_enum_value(value, "pmch_cfg_r12_s::mch_sched_period_r12_e_");
  }
  return "";
}
uint16_t pmch_cfg_r12_s::mch_sched_period_r12_e_::to_number() const
{
  const static uint16_t options[] = {4, 8, 16, 32, 64, 128, 256, 512, 1024};
  return get_enum_number(options, 9, value, "pmch_cfg_r12_s::mch_sched_period_r12_e_");
}

std::string pmch_cfg_r12_s::mch_sched_period_v1430_e_::to_string() const
{
  switch (value) {
    case rf1:
      return "rf1";
    case rf2:
      return "rf2";
    default:
      invalid_enum_value(value, "pmch_cfg_r12_s::mch_sched_period_v1430_e_");
  }
  return "";
}
uint8_t pmch_cfg_r12_s::mch_sched_period_v1430_e_::to_number() const
{
  const static uint8_t options[] = {1, 2};
  return get_enum_number(options, 2, value, "pmch_cfg_r12_s::mch_sched_period_v1430_e_");
}

std::string pmch_cfg_r9_s::mch_sched_period_r9_e_::to_string() const
{
  switch (value) {
    case rf8:
      return "rf8";
    case rf16:
      return "rf16";
    case rf32:
      return "rf32";
    case rf64:
      return "rf64";
    case rf128:
      return "rf128";
    case rf256:
      return "rf256";
    case rf512:
      return "rf512";
    case rf1024:
      return "rf1024";
    default:
      invalid_enum_value(value, "pmch_cfg_r9_s::mch_sched_period_r9_e_");
  }
  return "";
}
uint16_t pmch_cfg_r9_s::mch_sched_period_r9_e_::to_number() const
{
  const static uint16_t options[] = {8, 16, 32, 64, 128, 256, 512, 1024};
  return get_enum_number(options, 8, value, "pmch_cfg_r9_s::mch_sched_period_r9_e_");
}

std::string mbsfn_area_cfg_r9_s::common_sf_alloc_period_r9_e_::to_string() const
{
  switch (value) {
    case rf4:
      return "rf4";
    case rf8:
      return "rf8";
    case rf16:
      return "rf16";
    case rf32:
      return "rf32";
    case rf64:
      return "rf64";
    case rf128:
      return "rf128";
    case rf256:
      return "rf256";
    default:
      invalid_enum_value(value, "mbsfn_area_cfg_r9_s::common_sf_alloc_period_r9_e_");
  }
  return "";
}
uint16_t mbsfn_area_cfg_r9_s::common_sf_alloc_period_r9_e_::to_number() const
{
  const static uint16_t options[] = {4, 8, 16, 32, 64, 128, 256};
  return get_enum_number(options, 7, value, "mbsfn_area_cfg_r9_s::common_sf_alloc_period_r9_e_");
}

std::string mcch_msg_type_c::c1_c_::types::to_string() const
{
  switch (value) {
    case mbsfn_area_cfg_r9:
      return "mbsfnAreaConfiguration-r9";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "mcch_msg_type_c::c1_c_::types");
  }
  return "";
}

std::string mcch_msg_type_c::later_c_::c2_c_::types::to_string() const
{
  switch (value) {
    case mbms_count_request_r10:
      return "mbmsCountingRequest-r10";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "mcch_msg_type_c::later_c_::c2_c_::types");
  }
  return "";
}

std::string mcch_msg_type_c::later_c_::types::to_string() const
{
  switch (value) {
    case c2:
      return "c2";
    case msg_class_ext:
      return "messageClassExtension";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "mcch_msg_type_c::later_c_::types");
  }
  return "";
}
uint8_t mcch_msg_type_c::later_c_::types::to_number() const
{
  const static uint8_t options[] = {2};
  return get_enum_number(options, 1, value, "mcch_msg_type_c::later_c_::types");
}

std::string mcch_msg_type_c::types::to_string() const
{
  switch (value) {
    case c1:
      return "c1";
    case later:
      return "later";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "mcch_msg_type_c::types");
  }
  return "";
}
uint8_t mcch_msg_type_c::types::to_number() const
{
  const static uint8_t options[] = {1};
  return get_enum_number(options, 1, value, "mcch_msg_type_c::types");
}

std::string paging_ue_id_c::types::to_string() const
{
  switch (value) {
    case s_tmsi:
      return "s-TMSI";
    case imsi:
      return "imsi";
    case ng_minus5_g_s_tmsi_r15:
      return "ng-5G-S-TMSI-r15";
    case i_rnti_r15:
      return "i-RNTI-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "paging_ue_id_c::types");
  }
  return "";
}
int8_t paging_ue_id_c::types::to_number() const
{
  switch (value) {
    case ng_minus5_g_s_tmsi_r15:
      return -5;
    default:
      invalid_enum_number(value, "paging_ue_id_c::types");
  }
  return 0;
}

std::string paging_record_s::cn_domain_e_::to_string() const
{
  switch (value) {
    case ps:
      return "ps";
    case cs:
      return "cs";
    default:
      invalid_enum_value(value, "paging_record_s::cn_domain_e_");
  }
  return "";
}

std::string pcch_msg_type_c::c1_c_::types::to_string() const
{
  switch (value) {
    case paging:
      return "paging";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "pcch_msg_type_c::c1_c_::types");
  }
  return "";
}

std::string pcch_msg_type_c::types::to_string() const
{
  switch (value) {
    case c1:
      return "c1";
    case msg_class_ext:
      return "messageClassExtension";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "pcch_msg_type_c::types");
  }
  return "";
}
uint8_t pcch_msg_type_c::types::to_number() const
{
  const static uint8_t options[] = {1};
  return get_enum_number(options, 1, value, "pcch_msg_type_c::types");
}

std::string sc_mtch_sched_info_br_r14_s::on_dur_timer_scptm_r14_e_::to_string() const
{
  switch (value) {
    case psf300:
      return "psf300";
    case psf400:
      return "psf400";
    case psf500:
      return "psf500";
    case psf600:
      return "psf600";
    case psf800:
      return "psf800";
    case psf1000:
      return "psf1000";
    case psf1200:
      return "psf1200";
    case psf1600:
      return "psf1600";
    default:
      invalid_enum_value(value, "sc_mtch_sched_info_br_r14_s::on_dur_timer_scptm_r14_e_");
  }
  return "";
}
uint16_t sc_mtch_sched_info_br_r14_s::on_dur_timer_scptm_r14_e_::to_number() const
{
  const static uint16_t options[] = {300, 400, 500, 600, 800, 1000, 1200, 1600};
  return get_enum_number(options, 8, value, "sc_mtch_sched_info_br_r14_s::on_dur_timer_scptm_r14_e_");
}

std::string sc_mtch_sched_info_br_r14_s::drx_inactivity_timer_scptm_r14_e_::to_string() const
{
  switch (value) {
    case psf0:
      return "psf0";
    case psf1:
      return "psf1";
    case psf2:
      return "psf2";
    case psf4:
      return "psf4";
    case psf8:
      return "psf8";
    case psf16:
      return "psf16";
    case psf32:
      return "psf32";
    case psf64:
      return "psf64";
    case psf128:
      return "psf128";
    case psf256:
      return "psf256";
    case ps512:
      return "ps512";
    case psf1024:
      return "psf1024";
    case psf2048:
      return "psf2048";
    case psf4096:
      return "psf4096";
    case psf8192:
      return "psf8192";
    case psf16384:
      return "psf16384";
    default:
      invalid_enum_value(value, "sc_mtch_sched_info_br_r14_s::drx_inactivity_timer_scptm_r14_e_");
  }
  return "";
}
uint16_t sc_mtch_sched_info_br_r14_s::drx_inactivity_timer_scptm_r14_e_::to_number() const
{
  const static uint16_t options[] = {0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384};
  return get_enum_number(options, 16, value, "sc_mtch_sched_info_br_r14_s::drx_inactivity_timer_scptm_r14_e_");
}

std::string sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::types::to_string() const
{
  switch (value) {
    case sf10:
      return "sf10";
    case sf20:
      return "sf20";
    case sf32:
      return "sf32";
    case sf40:
      return "sf40";
    case sf64:
      return "sf64";
    case sf80:
      return "sf80";
    case sf128:
      return "sf128";
    case sf160:
      return "sf160";
    case sf256:
      return "sf256";
    case sf320:
      return "sf320";
    case sf512:
      return "sf512";
    case sf640:
      return "sf640";
    case sf1024:
      return "sf1024";
    case sf2048:
      return "sf2048";
    case sf4096:
      return "sf4096";
    case sf8192:
      return "sf8192";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::types");
  }
  return "";
}
uint16_t sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::types::to_number() const
{
  const static uint16_t options[] = {10, 20, 32, 40, 64, 80, 128, 160, 256, 320, 512, 640, 1024, 2048, 4096, 8192};
  return get_enum_number(options, 16, value,
                         "sc_mtch_sched_info_br_r14_s::sched_period_start_offset_scptm_r14_c_::types");
}

std::string sc_mtch_sched_info_r13_s::on_dur_timer_scptm_r13_e_::to_string() const
{
  switch (value) {
    case psf1:
      return "psf1";
    case psf2:
      return "psf2";
    case psf3:
      return "psf3";
    case psf4:
      return "psf4";
    case psf5:
      return "psf5";
    case psf6:
      return "psf6";
    case psf8:
      return "psf8";
    case psf10:
      return "psf10";
    case psf20:
      return "psf20";
    case psf30:
      return "psf30";
    case psf40:
      return "psf40";
    case psf50:
      return "psf50";
    case psf60:
      return "psf60";
    case psf80:
      return "psf80";
    case psf100:
      return "psf100";
    case psf200:
      return "psf200";
    default:
      invalid_enum_value(value, "sc_mtch_sched_info_r13_s::on_dur_timer_scptm_r13_e_");
  }
  return "";
}
uint8_t sc_mtch_sched_info_r13_s::on_dur_timer_scptm_r13_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 4, 5, 6, 8, 10, 20, 30, 40, 50, 60, 80, 100, 200};
  return get_enum_number(options, 16, value, "sc_mtch_sched_info_r13_s::on_dur_timer_scptm_r13_e_");
}

std::string sc_mtch_sched_info_r13_s::drx_inactivity_timer_scptm_r13_e_::to_string() const
{
  switch (value) {
    case psf0:
      return "psf0";
    case psf1:
      return "psf1";
    case psf2:
      return "psf2";
    case psf4:
      return "psf4";
    case psf8:
      return "psf8";
    case psf10:
      return "psf10";
    case psf20:
      return "psf20";
    case psf40:
      return "psf40";
    case psf80:
      return "psf80";
    case psf160:
      return "psf160";
    case ps320:
      return "ps320";
    case psf640:
      return "psf640";
    case psf960:
      return "psf960";
    case psf1280:
      return "psf1280";
    case psf1920:
      return "psf1920";
    case psf2560:
      return "psf2560";
    default:
      invalid_enum_value(value, "sc_mtch_sched_info_r13_s::drx_inactivity_timer_scptm_r13_e_");
  }
  return "";
}
uint16_t sc_mtch_sched_info_r13_s::drx_inactivity_timer_scptm_r13_e_::to_number() const
{
  const static uint16_t options[] = {0, 1, 2, 4, 8, 10, 20, 40, 80, 160, 320, 640, 960, 1280, 1920, 2560};
  return get_enum_number(options, 16, value, "sc_mtch_sched_info_r13_s::drx_inactivity_timer_scptm_r13_e_");
}

std::string sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::types::to_string() const
{
  switch (value) {
    case sf10:
      return "sf10";
    case sf20:
      return "sf20";
    case sf32:
      return "sf32";
    case sf40:
      return "sf40";
    case sf64:
      return "sf64";
    case sf80:
      return "sf80";
    case sf128:
      return "sf128";
    case sf160:
      return "sf160";
    case sf256:
      return "sf256";
    case sf320:
      return "sf320";
    case sf512:
      return "sf512";
    case sf640:
      return "sf640";
    case sf1024:
      return "sf1024";
    case sf2048:
      return "sf2048";
    case sf4096:
      return "sf4096";
    case sf8192:
      return "sf8192";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::types");
  }
  return "";
}
uint16_t sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::types::to_number() const
{
  const static uint16_t options[] = {10, 20, 32, 40, 64, 80, 128, 160, 256, 320, 512, 640, 1024, 2048, 4096, 8192};
  return get_enum_number(options, 16, value, "sc_mtch_sched_info_r13_s::sched_period_start_offset_scptm_r13_c_::types");
}

std::string sc_mtch_info_br_r14_s::mpdcch_num_repeat_sc_mtch_r14_e_::to_string() const
{
  switch (value) {
    case r1:
      return "r1";
    case r2:
      return "r2";
    case r4:
      return "r4";
    case r8:
      return "r8";
    case r16:
      return "r16";
    case r32:
      return "r32";
    case r64:
      return "r64";
    case r128:
      return "r128";
    case r256:
      return "r256";
    default:
      invalid_enum_value(value, "sc_mtch_info_br_r14_s::mpdcch_num_repeat_sc_mtch_r14_e_");
  }
  return "";
}
uint16_t sc_mtch_info_br_r14_s::mpdcch_num_repeat_sc_mtch_r14_e_::to_number() const
{
  const static uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
  return get_enum_number(options, 9, value, "sc_mtch_info_br_r14_s::mpdcch_num_repeat_sc_mtch_r14_e_");
}

std::string sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::fdd_r14_e_::to_string() const
{
  switch (value) {
    case v1:
      return "v1";
    case v1dot5:
      return "v1dot5";
    case v2:
      return "v2";
    case v2dot5:
      return "v2dot5";
    case v4:
      return "v4";
    case v5:
      return "v5";
    case v8:
      return "v8";
    case v10:
      return "v10";
    default:
      invalid_enum_value(value, "sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::fdd_r14_e_");
  }
  return "";
}
float sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::fdd_r14_e_::to_number() const
{
  const static float options[] = {1.0, 1.5, 2.0, 2.5, 4.0, 5.0, 8.0, 10.0};
  return get_enum_number(options, 8, value, "sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::fdd_r14_e_");
}
std::string sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::fdd_r14_e_::to_number_string() const
{
  switch (value) {
    case v1:
      return "1";
    case v1dot5:
      return "1.5";
    case v2:
      return "2";
    case v2dot5:
      return "2.5";
    case v4:
      return "4";
    case v5:
      return "5";
    case v8:
      return "8";
    case v10:
      return "10";
    default:
      invalid_enum_number(value, "sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::fdd_r14_e_");
  }
  return "";
}

std::string sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::tdd_r14_e_::to_string() const
{
  switch (value) {
    case v1:
      return "v1";
    case v2:
      return "v2";
    case v4:
      return "v4";
    case v5:
      return "v5";
    case v8:
      return "v8";
    case v10:
      return "v10";
    case v20:
      return "v20";
    default:
      invalid_enum_value(value, "sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::tdd_r14_e_");
  }
  return "";
}
uint8_t sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::tdd_r14_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 4, 5, 8, 10, 20};
  return get_enum_number(options, 7, value, "sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::tdd_r14_e_");
}

std::string sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::types::to_string() const
{
  switch (value) {
    case fdd_r14:
      return "fdd-r14";
    case tdd_r14:
      return "tdd-r14";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sc_mtch_info_br_r14_s::mpdcch_start_sf_sc_mtch_r14_c_::types");
  }
  return "";
}

std::string sc_mtch_info_br_r14_s::mpdcch_pdsch_hop_cfg_sc_mtch_r14_e_::to_string() const
{
  switch (value) {
    case on:
      return "on";
    case off:
      return "off";
    default:
      invalid_enum_value(value, "sc_mtch_info_br_r14_s::mpdcch_pdsch_hop_cfg_sc_mtch_r14_e_");
  }
  return "";
}

std::string sc_mtch_info_br_r14_s::mpdcch_pdsch_cemode_cfg_sc_mtch_r14_e_::to_string() const
{
  switch (value) {
    case ce_mode_a:
      return "ce-ModeA";
    case ce_mode_b:
      return "ce-ModeB";
    default:
      invalid_enum_value(value, "sc_mtch_info_br_r14_s::mpdcch_pdsch_cemode_cfg_sc_mtch_r14_e_");
  }
  return "";
}

std::string sc_mtch_info_br_r14_s::mpdcch_pdsch_max_bw_sc_mtch_r14_e_::to_string() const
{
  switch (value) {
    case bw1dot4:
      return "bw1dot4";
    case bw5:
      return "bw5";
    default:
      invalid_enum_value(value, "sc_mtch_info_br_r14_s::mpdcch_pdsch_max_bw_sc_mtch_r14_e_");
  }
  return "";
}
float sc_mtch_info_br_r14_s::mpdcch_pdsch_max_bw_sc_mtch_r14_e_::to_number() const
{
  const static float options[] = {1.4, 5.0};
  return get_enum_number(options, 2, value, "sc_mtch_info_br_r14_s::mpdcch_pdsch_max_bw_sc_mtch_r14_e_");
}
std::string sc_mtch_info_br_r14_s::mpdcch_pdsch_max_bw_sc_mtch_r14_e_::to_number_string() const
{
  switch (value) {
    case bw1dot4:
      return "1.4";
    case bw5:
      return "5";
    default:
      invalid_enum_number(value, "sc_mtch_info_br_r14_s::mpdcch_pdsch_max_bw_sc_mtch_r14_e_");
  }
  return "";
}

std::string sc_mtch_info_br_r14_s::mpdcch_offset_sc_mtch_r14_e_::to_string() const
{
  switch (value) {
    case zero:
      return "zero";
    case one_eighth:
      return "oneEighth";
    case one_quarter:
      return "oneQuarter";
    case three_eighth:
      return "threeEighth";
    case one_half:
      return "oneHalf";
    case five_eighth:
      return "fiveEighth";
    case three_quarter:
      return "threeQuarter";
    case seven_eighth:
      return "sevenEighth";
    default:
      invalid_enum_value(value, "sc_mtch_info_br_r14_s::mpdcch_offset_sc_mtch_r14_e_");
  }
  return "";
}
float sc_mtch_info_br_r14_s::mpdcch_offset_sc_mtch_r14_e_::to_number() const
{
  const static float options[] = {0.0, 0.125, 0.25, 0.375, 0.5, 0.625, 0.75, 0.875};
  return get_enum_number(options, 8, value, "sc_mtch_info_br_r14_s::mpdcch_offset_sc_mtch_r14_e_");
}
std::string sc_mtch_info_br_r14_s::mpdcch_offset_sc_mtch_r14_e_::to_number_string() const
{
  switch (value) {
    case zero:
      return "0";
    case one_eighth:
      return "1/8";
    case one_quarter:
      return "1/4";
    case three_eighth:
      return "3/8";
    case one_half:
      return "1/2";
    case five_eighth:
      return "5/8";
    case three_quarter:
      return "3/4";
    case seven_eighth:
      return "7/8";
    default:
      invalid_enum_number(value, "sc_mtch_info_br_r14_s::mpdcch_offset_sc_mtch_r14_e_");
  }
  return "";
}

std::string sc_mtch_info_br_r14_s::p_a_r14_e_::to_string() const
{
  switch (value) {
    case db_minus6:
      return "dB-6";
    case db_minus4dot77:
      return "dB-4dot77";
    case db_minus3:
      return "dB-3";
    case db_minus1dot77:
      return "dB-1dot77";
    case db0:
      return "dB0";
    case db1:
      return "dB1";
    case db2:
      return "dB2";
    case db3:
      return "dB3";
    default:
      invalid_enum_value(value, "sc_mtch_info_br_r14_s::p_a_r14_e_");
  }
  return "";
}
float sc_mtch_info_br_r14_s::p_a_r14_e_::to_number() const
{
  const static float options[] = {-6.0, -4.77, -3.0, -1.77, 0.0, 1.0, 2.0, 3.0};
  return get_enum_number(options, 8, value, "sc_mtch_info_br_r14_s::p_a_r14_e_");
}
std::string sc_mtch_info_br_r14_s::p_a_r14_e_::to_number_string() const
{
  switch (value) {
    case db_minus6:
      return "-6";
    case db_minus4dot77:
      return "-4.77";
    case db_minus3:
      return "-3";
    case db_minus1dot77:
      return "-1.77";
    case db0:
      return "0";
    case db1:
      return "1";
    case db2:
      return "2";
    case db3:
      return "3";
    default:
      invalid_enum_number(value, "sc_mtch_info_br_r14_s::p_a_r14_e_");
  }
  return "";
}

std::string sc_mtch_info_r13_s::p_a_r13_e_::to_string() const
{
  switch (value) {
    case db_minus6:
      return "dB-6";
    case db_minus4dot77:
      return "dB-4dot77";
    case db_minus3:
      return "dB-3";
    case db_minus1dot77:
      return "dB-1dot77";
    case db0:
      return "dB0";
    case db1:
      return "dB1";
    case db2:
      return "dB2";
    case db3:
      return "dB3";
    default:
      invalid_enum_value(value, "sc_mtch_info_r13_s::p_a_r13_e_");
  }
  return "";
}
float sc_mtch_info_r13_s::p_a_r13_e_::to_number() const
{
  const static float options[] = {-6.0, -4.77, -3.0, -1.77, 0.0, 1.0, 2.0, 3.0};
  return get_enum_number(options, 8, value, "sc_mtch_info_r13_s::p_a_r13_e_");
}
std::string sc_mtch_info_r13_s::p_a_r13_e_::to_number_string() const
{
  switch (value) {
    case db_minus6:
      return "-6";
    case db_minus4dot77:
      return "-4.77";
    case db_minus3:
      return "-3";
    case db_minus1dot77:
      return "-1.77";
    case db0:
      return "0";
    case db1:
      return "1";
    case db2:
      return "2";
    case db3:
      return "3";
    default:
      invalid_enum_number(value, "sc_mtch_info_r13_s::p_a_r13_e_");
  }
  return "";
}

std::string sc_mcch_msg_type_r13_c::c1_c_::types::to_string() const
{
  switch (value) {
    case scptm_cfg_r13:
      return "scptmConfiguration-r13";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sc_mcch_msg_type_r13_c::c1_c_::types");
  }
  return "";
}

std::string sc_mcch_msg_type_r13_c::msg_class_ext_c_::c2_c_::types::to_string() const
{
  switch (value) {
    case scptm_cfg_br_r14:
      return "scptmConfiguration-BR-r14";
    case spare:
      return "spare";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sc_mcch_msg_type_r13_c::msg_class_ext_c_::c2_c_::types");
  }
  return "";
}

std::string sc_mcch_msg_type_r13_c::msg_class_ext_c_::types::to_string() const
{
  switch (value) {
    case c2:
      return "c2";
    case msg_class_ext_future_r14:
      return "messageClassExtensionFuture-r14";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sc_mcch_msg_type_r13_c::msg_class_ext_c_::types");
  }
  return "";
}
uint8_t sc_mcch_msg_type_r13_c::msg_class_ext_c_::types::to_number() const
{
  const static uint8_t options[] = {2};
  return get_enum_number(options, 1, value, "sc_mcch_msg_type_r13_c::msg_class_ext_c_::types");
}

std::string sc_mcch_msg_type_r13_c::types::to_string() const
{
  switch (value) {
    case c1:
      return "c1";
    case msg_class_ext:
      return "messageClassExtension";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sc_mcch_msg_type_r13_c::types");
  }
  return "";
}
uint8_t sc_mcch_msg_type_r13_c::types::to_number() const
{
  const static uint8_t options[] = {1};
  return get_enum_number(options, 1, value, "sc_mcch_msg_type_r13_c::types");
}

// EstablishmentCause ::= ENUMERATED
std::string establishment_cause_e::to_string() const
{
  switch (value) {
    case emergency:
      return "emergency";
    case high_prio_access:
      return "highPriorityAccess";
    case mt_access:
      return "mt-Access";
    case mo_sig:
      return "mo-Signalling";
    case mo_data:
      return "mo-Data";
    case delay_tolerant_access_v1020:
      return "delayTolerantAccess-v1020";
    case mo_voice_call_v1280:
      return "mo-VoiceCall-v1280";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "establishment_cause_e");
  }
  return "";
}

std::string init_ue_id_c::types::to_string() const
{
  switch (value) {
    case s_tmsi:
      return "s-TMSI";
    case random_value:
      return "randomValue";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "init_ue_id_c::types");
  }
  return "";
}

// ReestablishmentCause ::= ENUMERATED
std::string reest_cause_e::to_string() const
{
  switch (value) {
    case recfg_fail:
      return "reconfigurationFailure";
    case ho_fail:
      return "handoverFailure";
    case other_fail:
      return "otherFailure";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "reest_cause_e");
  }
  return "";
}

// ResumeCause ::= ENUMERATED
std::string resume_cause_e::to_string() const
{
  switch (value) {
    case emergency:
      return "emergency";
    case high_prio_access:
      return "highPriorityAccess";
    case mt_access:
      return "mt-Access";
    case mo_sig:
      return "mo-Signalling";
    case mo_data:
      return "mo-Data";
    case delay_tolerant_access_v1020:
      return "delayTolerantAccess-v1020";
    case mo_voice_call_v1280:
      return "mo-VoiceCall-v1280";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "resume_cause_e");
  }
  return "";
}

// ResumeCause-r15 ::= ENUMERATED
std::string resume_cause_r15_e::to_string() const
{
  switch (value) {
    case emergency:
      return "emergency";
    case high_prio_access:
      return "highPriorityAccess";
    case mt_access:
      return "mt-Access";
    case mo_sig:
      return "mo-Signalling";
    case mo_data:
      return "mo-Data";
    case rna_update:
      return "rna-Update";
    case mo_voice_call:
      return "mo-VoiceCall";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "resume_cause_r15_e");
  }
  return "";
}

std::string rrc_conn_resume_request_minus5_gc_r15_ies_s::resume_id_r15_c_::types::to_string() const
{
  switch (value) {
    case full_i_rnti_r15:
      return "fullI-RNTI-r15";
    case short_i_rnti_r15:
      return "shortI-RNTI-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rrc_conn_resume_request_minus5_gc_r15_ies_s::resume_id_r15_c_::types");
  }
  return "";
}

std::string rrc_conn_resume_request_r13_ies_s::resume_id_r13_c_::types::to_string() const
{
  switch (value) {
    case resume_id_r13:
      return "resumeID-r13";
    case truncated_resume_id_r13:
      return "truncatedResumeID-r13";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rrc_conn_resume_request_r13_ies_s::resume_id_r13_c_::types");
  }
  return "";
}

std::string rrc_early_data_request_r15_ies_s::establishment_cause_r15_e_::to_string() const
{
  switch (value) {
    case mo_data_r15:
      return "mo-Data-r15";
    case delay_tolerant_access_r15:
      return "delayTolerantAccess-r15";
    default:
      invalid_enum_value(value, "rrc_early_data_request_r15_ies_s::establishment_cause_r15_e_");
  }
  return "";
}

std::string rrc_conn_reest_request_s::crit_exts_c_::types::to_string() const
{
  switch (value) {
    case rrc_conn_reest_request_r8:
      return "rrcConnectionReestablishmentRequest-r8";
    case crit_exts_future:
      return "criticalExtensionsFuture";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rrc_conn_reest_request_s::crit_exts_c_::types");
  }
  return "";
}

std::string rrc_conn_request_s::crit_exts_c_::types::to_string() const
{
  switch (value) {
    case rrc_conn_request_r8:
      return "rrcConnectionRequest-r8";
    case crit_exts_future:
      return "criticalExtensionsFuture";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rrc_conn_request_s::crit_exts_c_::types");
  }
  return "";
}

std::string rrc_conn_resume_request_r13_s::crit_exts_c_::types::to_string() const
{
  switch (value) {
    case rrc_conn_resume_request_r13:
      return "rrcConnectionResumeRequest-r13";
    case rrc_conn_resume_request_r15:
      return "rrcConnectionResumeRequest-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rrc_conn_resume_request_r13_s::crit_exts_c_::types");
  }
  return "";
}

std::string rrc_early_data_request_r15_s::crit_exts_c_::types::to_string() const
{
  switch (value) {
    case rrc_early_data_request_r15:
      return "rrcEarlyDataRequest-r15";
    case crit_exts_future:
      return "criticalExtensionsFuture";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rrc_early_data_request_r15_s::crit_exts_c_::types");
  }
  return "";
}

std::string ul_ccch_msg_type_c::c1_c_::types::to_string() const
{
  switch (value) {
    case rrc_conn_reest_request:
      return "rrcConnectionReestablishmentRequest";
    case rrc_conn_request:
      return "rrcConnectionRequest";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ul_ccch_msg_type_c::c1_c_::types");
  }
  return "";
}

std::string ul_ccch_msg_type_c::msg_class_ext_c_::c2_c_::types::to_string() const
{
  switch (value) {
    case rrc_conn_resume_request_r13:
      return "rrcConnectionResumeRequest-r13";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ul_ccch_msg_type_c::msg_class_ext_c_::c2_c_::types");
  }
  return "";
}

std::string ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::c3_c_::types::to_string() const
{
  switch (value) {
    case rrc_early_data_request_r15:
      return "rrcEarlyDataRequest-r15";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::c3_c_::types");
  }
  return "";
}

std::string ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::types::to_string() const
{
  switch (value) {
    case c3:
      return "c3";
    case msg_class_ext_future_r15:
      return "messageClassExtensionFuture-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::types");
  }
  return "";
}
uint8_t ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::types::to_number() const
{
  const static uint8_t options[] = {3};
  return get_enum_number(options, 1, value, "ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::types");
}

std::string ul_ccch_msg_type_c::msg_class_ext_c_::types::to_string() const
{
  switch (value) {
    case c2:
      return "c2";
    case msg_class_ext_future_r13:
      return "messageClassExtensionFuture-r13";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ul_ccch_msg_type_c::msg_class_ext_c_::types");
  }
  return "";
}
uint8_t ul_ccch_msg_type_c::msg_class_ext_c_::types::to_number() const
{
  const static uint8_t options[] = {2};
  return get_enum_number(options, 1, value, "ul_ccch_msg_type_c::msg_class_ext_c_::types");
}

std::string ul_ccch_msg_type_c::types::to_string() const
{
  switch (value) {
    case c1:
      return "c1";
    case msg_class_ext:
      return "messageClassExtension";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ul_ccch_msg_type_c::types");
  }
  return "";
}
uint8_t ul_ccch_msg_type_c::types::to_number() const
{
  const static uint8_t options[] = {1};
  return get_enum_number(options, 1, value, "ul_ccch_msg_type_c::types");
}

std::string cell_global_id_cdma2000_c::types::to_string() const
{
  switch (value) {
    case cell_global_id1_xrtt:
      return "cellGlobalId1XRTT";
    case cell_global_id_hrpd:
      return "cellGlobalIdHRPD";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "cell_global_id_cdma2000_c::types");
  }
  return "";
}
uint8_t cell_global_id_cdma2000_c::types::to_number() const
{
  const static uint8_t options[] = {1};
  return get_enum_number(options, 1, value, "cell_global_id_cdma2000_c::types");
}

std::string meas_result_utra_s::pci_c_::types::to_string() const
{
  switch (value) {
    case fdd:
      return "fdd";
    case tdd:
      return "tdd";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "meas_result_utra_s::pci_c_::types");
  }
  return "";
}

std::string s_nssai_r15_c::types::to_string() const
{
  switch (value) {
    case sst:
      return "sst";
    case sst_sd:
      return "sst-SD";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "s_nssai_r15_c::types");
  }
  return "";
}

std::string location_info_r10_s::location_coordinates_r10_c_::types::to_string() const
{
  switch (value) {
    case ellipsoid_point_r10:
      return "ellipsoid-Point-r10";
    case ellipsoid_point_with_altitude_r10:
      return "ellipsoidPointWithAltitude-r10";
    case ellipsoid_point_with_uncertainty_circle_r11:
      return "ellipsoidPointWithUncertaintyCircle-r11";
    case ellipsoid_point_with_uncertainty_ellipse_r11:
      return "ellipsoidPointWithUncertaintyEllipse-r11";
    case ellipsoid_point_with_altitude_and_uncertainty_ellipsoid_r11:
      return "ellipsoidPointWithAltitudeAndUncertaintyEllipsoid-r11";
    case ellipsoid_arc_r11:
      return "ellipsoidArc-r11";
    case polygon_r11:
      return "polygon-r11";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "location_info_r10_s::location_coordinates_r10_c_::types");
  }
  return "";
}

std::string location_info_r10_s::vertical_velocity_info_r15_c_::types::to_string() const
{
  switch (value) {
    case vertical_velocity_r15:
      return "verticalVelocity-r15";
    case vertical_velocity_and_uncertainty_r15:
      return "verticalVelocityAndUncertainty-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "location_info_r10_s::vertical_velocity_info_r15_c_::types");
  }
  return "";
}

std::string rrc_conn_setup_complete_v1530_ies_s::ng_minus5_g_s_tmsi_bits_r15_c_::types::to_string() const
{
  switch (value) {
    case ng_minus5_g_s_tmsi_r15:
      return "ng-5G-S-TMSI-r15";
    case ng_minus5_g_s_tmsi_part2_r15:
      return "ng-5G-S-TMSI-Part2-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rrc_conn_setup_complete_v1530_ies_s::ng_minus5_g_s_tmsi_bits_r15_c_::types");
  }
  return "";
}

std::string wlan_rtt_r15_s::rtt_units_r15_e_::to_string() const
{
  switch (value) {
    case microseconds:
      return "microseconds";
    case hundredsofnanoseconds:
      return "hundredsofnanoseconds";
    case tensofnanoseconds:
      return "tensofnanoseconds";
    case nanoseconds:
      return "nanoseconds";
    case tenthsofnanoseconds:
      return "tenthsofnanoseconds";
    default:
      invalid_enum_value(value, "wlan_rtt_r15_s::rtt_units_r15_e_");
  }
  return "";
}

std::string meas_result_idle_r15_s::meas_result_neigh_cells_r15_c_::types::to_string() const
{
  switch (value) {
    case meas_result_idle_list_eutra_r15:
      return "measResultIdleListEUTRA-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "meas_result_idle_r15_s::meas_result_neigh_cells_r15_c_::types");
  }
  return "";
}

std::string per_cc_gap_ind_r14_s::gap_ind_r14_e_::to_string() const
{
  switch (value) {
    case gap:
      return "gap";
    case ncsg:
      return "ncsg";
    case nogap_no_ncsg:
      return "nogap-noNcsg";
    default:
      invalid_enum_value(value, "per_cc_gap_ind_r14_s::gap_ind_r14_e_");
  }
  return "";
}

std::string visited_cell_info_r12_s::visited_cell_id_r12_c_::types::to_string() const
{
  switch (value) {
    case cell_global_id_r12:
      return "cellGlobalId-r12";
    case pci_arfcn_r12:
      return "pci-arfcn-r12";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "visited_cell_info_r12_s::visited_cell_id_r12_c_::types");
  }
  return "";
}

std::string affected_carrier_freq_comb_info_mrdc_r15_s::interference_direction_mrdc_r15_e_::to_string() const
{
  switch (value) {
    case eutra_nr:
      return "eutra-nr";
    case nr:
      return "nr";
    case other:
      return "other";
    case eutra_nr_other:
      return "eutra-nr-other";
    case nr_other:
      return "nr-other";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "affected_carrier_freq_comb_info_mrdc_r15_s::interference_direction_mrdc_r15_e_");
  }
  return "";
}

std::string rrc_conn_setup_complete_v1250_ies_s::mob_state_r12_e_::to_string() const
{
  switch (value) {
    case normal:
      return "normal";
    case medium:
      return "medium";
    case high:
      return "high";
    case spare:
      return "spare";
    default:
      invalid_enum_value(value, "rrc_conn_setup_complete_v1250_ies_s::mob_state_r12_e_");
  }
  return "";
}

std::string idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_::types::to_string() const
{
  switch (value) {
    case sf_cfg0_r11:
      return "subframeConfig0-r11";
    case sf_cfg1_minus5_r11:
      return "subframeConfig1-5-r11";
    case sf_cfg6_r11:
      return "subframeConfig6-r11";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_::types");
  }
  return "";
}
uint8_t idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_::types::to_number() const
{
  const static uint8_t options[] = {0, 1, 6};
  return get_enum_number(options, 3, value, "idc_sf_pattern_r11_c::sf_pattern_tdd_r11_c_::types");
}

std::string idc_sf_pattern_r11_c::types::to_string() const
{
  switch (value) {
    case sf_pattern_fdd_r11:
      return "subframePatternFDD-r11";
    case sf_pattern_tdd_r11:
      return "subframePatternTDD-r11";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "idc_sf_pattern_r11_c::types");
  }
  return "";
}

std::string sl_disc_sys_info_report_r13_s::cell_resel_info_r13_s_::q_hyst_r13_e_::to_string() const
{
  switch (value) {
    case db0:
      return "dB0";
    case db1:
      return "dB1";
    case db2:
      return "dB2";
    case db3:
      return "dB3";
    case db4:
      return "dB4";
    case db5:
      return "dB5";
    case db6:
      return "dB6";
    case db8:
      return "dB8";
    case db10:
      return "dB10";
    case db12:
      return "dB12";
    case db14:
      return "dB14";
    case db16:
      return "dB16";
    case db18:
      return "dB18";
    case db20:
      return "dB20";
    case db22:
      return "dB22";
    case db24:
      return "dB24";
    default:
      invalid_enum_value(value, "sl_disc_sys_info_report_r13_s::cell_resel_info_r13_s_::q_hyst_r13_e_");
  }
  return "";
}
uint8_t sl_disc_sys_info_report_r13_s::cell_resel_info_r13_s_::q_hyst_r13_e_::to_number() const
{
  const static uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24};
  return get_enum_number(options, 16, value, "sl_disc_sys_info_report_r13_s::cell_resel_info_r13_s_::q_hyst_r13_e_");
}

std::string sl_disc_sys_info_report_r13_s::freq_info_r13_s_::ul_bw_r13_e_::to_string() const
{
  switch (value) {
    case n6:
      return "n6";
    case n15:
      return "n15";
    case n25:
      return "n25";
    case n50:
      return "n50";
    case n75:
      return "n75";
    case n100:
      return "n100";
    default:
      invalid_enum_value(value, "sl_disc_sys_info_report_r13_s::freq_info_r13_s_::ul_bw_r13_e_");
  }
  return "";
}
uint8_t sl_disc_sys_info_report_r13_s::freq_info_r13_s_::ul_bw_r13_e_::to_number() const
{
  const static uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return get_enum_number(options, 6, value, "sl_disc_sys_info_report_r13_s::freq_info_r13_s_::ul_bw_r13_e_");
}

std::string traffic_pattern_info_r14_s::traffic_periodicity_r14_e_::to_string() const
{
  switch (value) {
    case sf20:
      return "sf20";
    case sf50:
      return "sf50";
    case sf100:
      return "sf100";
    case sf200:
      return "sf200";
    case sf300:
      return "sf300";
    case sf400:
      return "sf400";
    case sf500:
      return "sf500";
    case sf600:
      return "sf600";
    case sf700:
      return "sf700";
    case sf800:
      return "sf800";
    case sf900:
      return "sf900";
    case sf1000:
      return "sf1000";
    default:
      invalid_enum_value(value, "traffic_pattern_info_r14_s::traffic_periodicity_r14_e_");
  }
  return "";
}
uint16_t traffic_pattern_info_r14_s::traffic_periodicity_r14_e_::to_number() const
{
  const static uint16_t options[] = {20, 50, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
  return get_enum_number(options, 12, value, "traffic_pattern_info_r14_s::traffic_periodicity_r14_e_");
}

std::string ue_radio_paging_info_r12_s::wake_up_signal_min_gap_e_drx_r15_e_::to_string() const
{
  switch (value) {
    case ms40:
      return "ms40";
    case ms240:
      return "ms240";
    case ms1000:
      return "ms1000";
    case ms2000:
      return "ms2000";
    default:
      invalid_enum_value(value, "ue_radio_paging_info_r12_s::wake_up_signal_min_gap_e_drx_r15_e_");
  }
  return "";
}
uint16_t ue_radio_paging_info_r12_s::wake_up_signal_min_gap_e_drx_r15_e_::to_number() const
{
  const static uint16_t options[] = {40, 240, 1000, 2000};
  return get_enum_number(options, 4, value, "ue_radio_paging_info_r12_s::wake_up_signal_min_gap_e_drx_r15_e_");
}

std::string ue_radio_paging_info_r12_s::wake_up_signal_min_gap_e_drx_tdd_r15_e_::to_string() const
{
  switch (value) {
    case ms40:
      return "ms40";
    case ms240:
      return "ms240";
    case ms1000:
      return "ms1000";
    case ms2000:
      return "ms2000";
    default:
      invalid_enum_value(value, "ue_radio_paging_info_r12_s::wake_up_signal_min_gap_e_drx_tdd_r15_e_");
  }
  return "";
}
uint16_t ue_radio_paging_info_r12_s::wake_up_signal_min_gap_e_drx_tdd_r15_e_::to_number() const
{
  const static uint16_t options[] = {40, 240, 1000, 2000};
  return get_enum_number(options, 4, value, "ue_radio_paging_info_r12_s::wake_up_signal_min_gap_e_drx_tdd_r15_e_");
}

std::string ul_pdcp_delay_result_r13_s::qci_id_r13_e_::to_string() const
{
  switch (value) {
    case qci1:
      return "qci1";
    case qci2:
      return "qci2";
    case qci3:
      return "qci3";
    case qci4:
      return "qci4";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "ul_pdcp_delay_result_r13_s::qci_id_r13_e_");
  }
  return "";
}
uint8_t ul_pdcp_delay_result_r13_s::qci_id_r13_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 4};
  return get_enum_number(options, 4, value, "ul_pdcp_delay_result_r13_s::qci_id_r13_e_");
}

std::string affected_carrier_freq_r11_s::interference_direction_r11_e_::to_string() const
{
  switch (value) {
    case eutra:
      return "eutra";
    case other:
      return "other";
    case both:
      return "both";
    case spare:
      return "spare";
    default:
      invalid_enum_value(value, "affected_carrier_freq_r11_s::interference_direction_r11_e_");
  }
  return "";
}

std::string bw_pref_r14_s::dl_pref_r14_e_::to_string() const
{
  switch (value) {
    case mhz1dot4:
      return "mhz1dot4";
    case mhz5:
      return "mhz5";
    case mhz20:
      return "mhz20";
    default:
      invalid_enum_value(value, "bw_pref_r14_s::dl_pref_r14_e_");
  }
  return "";
}
float bw_pref_r14_s::dl_pref_r14_e_::to_number() const
{
  const static float options[] = {1.4, 5.0, 20.0};
  return get_enum_number(options, 3, value, "bw_pref_r14_s::dl_pref_r14_e_");
}
std::string bw_pref_r14_s::dl_pref_r14_e_::to_number_string() const
{
  switch (value) {
    case mhz1dot4:
      return "1.4";
    case mhz5:
      return "5";
    case mhz20:
      return "20";
    default:
      invalid_enum_number(value, "bw_pref_r14_s::dl_pref_r14_e_");
  }
  return "";
}

std::string bw_pref_r14_s::ul_pref_r14_e_::to_string() const
{
  switch (value) {
    case mhz1dot4:
      return "mhz1dot4";
    case mhz5:
      return "mhz5";
    default:
      invalid_enum_value(value, "bw_pref_r14_s::ul_pref_r14_e_");
  }
  return "";
}
float bw_pref_r14_s::ul_pref_r14_e_::to_number() const
{
  const static float options[] = {1.4, 5.0};
  return get_enum_number(options, 2, value, "bw_pref_r14_s::ul_pref_r14_e_");
}
std::string bw_pref_r14_s::ul_pref_r14_e_::to_number_string() const
{
  switch (value) {
    case mhz1dot4:
      return "1.4";
    case mhz5:
      return "5";
    default:
      invalid_enum_number(value, "bw_pref_r14_s::ul_pref_r14_e_");
  }
  return "";
}

std::string delay_budget_report_r14_c::type1_e_::to_string() const
{
  switch (value) {
    case ms_minus1280:
      return "msMinus1280";
    case ms_minus640:
      return "msMinus640";
    case ms_minus320:
      return "msMinus320";
    case ms_minus160:
      return "msMinus160";
    case ms_minus80:
      return "msMinus80";
    case ms_minus60:
      return "msMinus60";
    case ms_minus40:
      return "msMinus40";
    case ms_minus20:
      return "msMinus20";
    case ms0:
      return "ms0";
    case ms20:
      return "ms20";
    case ms40:
      return "ms40";
    case ms60:
      return "ms60";
    case ms80:
      return "ms80";
    case ms160:
      return "ms160";
    case ms320:
      return "ms320";
    case ms640:
      return "ms640";
    case ms1280:
      return "ms1280";
    default:
      invalid_enum_value(value, "delay_budget_report_r14_c::type1_e_");
  }
  return "";
}
int16_t delay_budget_report_r14_c::type1_e_::to_number() const
{
  const static int16_t options[] = {-1280, -640, -320, -160, -80, -60, -40, -20, 0,
                                    20,    40,   60,   80,   160, 320, 640, 1280};
  return get_enum_number(options, 17, value, "delay_budget_report_r14_c::type1_e_");
}

std::string delay_budget_report_r14_c::type2_e_::to_string() const
{
  switch (value) {
    case ms_minus192:
      return "msMinus192";
    case ms_minus168:
      return "msMinus168";
    case ms_minus144:
      return "msMinus144";
    case ms_minus120:
      return "msMinus120";
    case ms_minus96:
      return "msMinus96";
    case ms_minus72:
      return "msMinus72";
    case ms_minus48:
      return "msMinus48";
    case ms_minus24:
      return "msMinus24";
    case ms0:
      return "ms0";
    case ms24:
      return "ms24";
    case ms48:
      return "ms48";
    case ms72:
      return "ms72";
    case ms96:
      return "ms96";
    case ms120:
      return "ms120";
    case ms144:
      return "ms144";
    case ms168:
      return "ms168";
    case ms192:
      return "ms192";
    default:
      invalid_enum_value(value, "delay_budget_report_r14_c::type2_e_");
  }
  return "";
}
int16_t delay_budget_report_r14_c::type2_e_::to_number() const
{
  const static int16_t options[] = {-192, -168, -144, -120, -96, -72, -48, -24, 0, 24, 48, 72, 96, 120, 144, 168, 192};
  return get_enum_number(options, 17, value, "delay_budget_report_r14_c::type2_e_");
}

std::string delay_budget_report_r14_c::types::to_string() const
{
  switch (value) {
    case type1:
      return "type1";
    case type2:
      return "type2";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "delay_budget_report_r14_c::types");
  }
  return "";
}
uint8_t delay_budget_report_r14_c::types::to_number() const
{
  const static uint8_t options[] = {1, 2};
  return get_enum_number(options, 2, value, "delay_budget_report_r14_c::types");
}

std::string rrc_conn_setup_complete_v1020_ies_s::gummei_type_r10_e_::to_string() const
{
  switch (value) {
    case native:
      return "native";
    case mapped:
      return "mapped";
    default:
      invalid_enum_value(value, "rrc_conn_setup_complete_v1020_ies_s::gummei_type_r10_e_");
  }
  return "";
}

std::string rrc_conn_setup_complete_v1020_ies_s::rn_sf_cfg_req_r10_e_::to_string() const
{
  switch (value) {
    case required:
      return "required";
    case not_required:
      return "notRequired";
    default:
      invalid_enum_value(value, "rrc_conn_setup_complete_v1020_ies_s::rn_sf_cfg_req_r10_e_");
  }
  return "";
}

std::string rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::types::to_string() const
{
  switch (value) {
    case rstd0_r15:
      return "rstd0-r15";
    case rstd1_r15:
      return "rstd1-r15";
    case rstd2_r15:
      return "rstd2-r15";
    case rstd3_r15:
      return "rstd3-r15";
    case rstd4_r15:
      return "rstd4-r15";
    case rstd5_r15:
      return "rstd5-r15";
    case rstd6_r15:
      return "rstd6-r15";
    case rstd7_r15:
      return "rstd7-r15";
    case rstd8_r15:
      return "rstd8-r15";
    case rstd9_r15:
      return "rstd9-r15";
    case rstd10_r15:
      return "rstd10-r15";
    case rstd11_r15:
      return "rstd11-r15";
    case rstd12_r15:
      return "rstd12-r15";
    case rstd13_r15:
      return "rstd13-r15";
    case rstd14_r15:
      return "rstd14-r15";
    case rstd15_r15:
      return "rstd15-r15";
    case rstd16_r15:
      return "rstd16-r15";
    case rstd17_r15:
      return "rstd17-r15";
    case rstd18_r15:
      return "rstd18-r15";
    case rstd19_r15:
      return "rstd19-r15";
    case rstd20_r15:
      return "rstd20-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::types");
  }
  return "";
}
uint8_t rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::types::to_number() const
{
  const static uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
  return get_enum_number(options, 21, value, "rstd_inter_freq_info_r10_s::meas_prs_offset_r15_c_::types");
}

// WLAN-Status-v1430 ::= ENUMERATED
std::string wlan_status_v1430_e::to_string() const
{
  switch (value) {
    case suspended:
      return "suspended";
    case resumed:
      return "resumed";
    default:
      invalid_enum_value(value, "wlan_status_v1430_e");
  }
  return "";
}

std::string fail_report_scg_nr_r15_s::fail_type_r15_e_::to_string() const
{
  switch (value) {
    case t310_expiry:
      return "t310-Expiry";
    case random_access_problem:
      return "randomAccessProblem";
    case rlc_max_num_retx:
      return "rlc-MaxNumRetx";
    case scg_change_fail:
      return "scg-ChangeFailure";
    case scg_recfg_fail:
      return "scg-reconfigFailure";
    case srb3_integrity_fail:
      return "srb3-IntegrityFailure";
    default:
      invalid_enum_value(value, "fail_report_scg_nr_r15_s::fail_type_r15_e_");
  }
  return "";
}
uint16_t fail_report_scg_nr_r15_s::fail_type_r15_e_::to_number() const
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

std::string fail_report_scg_r12_s::fail_type_r12_e_::to_string() const
{
  switch (value) {
    case t313_expiry:
      return "t313-Expiry";
    case random_access_problem:
      return "randomAccessProblem";
    case rlc_max_num_retx:
      return "rlc-MaxNumRetx";
    case scg_change_fail:
      return "scg-ChangeFailure";
    default:
      invalid_enum_value(value, "fail_report_scg_r12_s::fail_type_r12_e_");
  }
  return "";
}
uint16_t fail_report_scg_r12_s::fail_type_r12_e_::to_number() const
{
  const static uint16_t options[] = {313};
  return get_enum_number(options, 1, value, "fail_report_scg_r12_s::fail_type_r12_e_");
}

std::string meas_results_s::meas_result_neigh_cells_c_::types::to_string() const
{
  switch (value) {
    case meas_result_list_eutra:
      return "measResultListEUTRA";
    case meas_result_list_utra:
      return "measResultListUTRA";
    case meas_result_list_geran:
      return "measResultListGERAN";
    case meas_results_cdma2000:
      return "measResultsCDMA2000";
    case meas_result_neigh_cell_list_nr_r15:
      return "measResultNeighCellListNR-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "meas_results_s::meas_result_neigh_cells_c_::types");
  }
  return "";
}
uint16_t meas_results_s::meas_result_neigh_cells_c_::types::to_number() const
{
  switch (value) {
    case meas_results_cdma2000:
      return 2000;
    default:
      invalid_enum_number(value, "meas_results_s::meas_result_neigh_cells_c_::types");
  }
  return 0;
}

std::string rlf_report_r9_s::failed_pcell_id_r10_c_::types::to_string() const
{
  switch (value) {
    case cell_global_id_r10:
      return "cellGlobalId-r10";
    case pci_arfcn_r10:
      return "pci-arfcn-r10";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rlf_report_r9_s::failed_pcell_id_r10_c_::types");
  }
  return "";
}

std::string rlf_report_r9_s::conn_fail_type_r10_e_::to_string() const
{
  switch (value) {
    case rlf:
      return "rlf";
    case hof:
      return "hof";
    default:
      invalid_enum_value(value, "rlf_report_r9_s::conn_fail_type_r10_e_");
  }
  return "";
}

std::string rlf_report_r9_s::basic_fields_r11_s_::rlf_cause_r11_e_::to_string() const
{
  switch (value) {
    case t310_expiry:
      return "t310-Expiry";
    case random_access_problem:
      return "randomAccessProblem";
    case rlc_max_num_retx:
      return "rlc-MaxNumRetx";
    case t312_expiry_r12:
      return "t312-Expiry-r12";
    default:
      invalid_enum_value(value, "rlf_report_r9_s::basic_fields_r11_s_::rlf_cause_r11_e_");
  }
  return "";
}
uint16_t rlf_report_r9_s::basic_fields_r11_s_::rlf_cause_r11_e_::to_number() const
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

std::string rlf_report_r9_s::prev_utra_cell_id_r11_s_::pci_r11_c_::types::to_string() const
{
  switch (value) {
    case fdd_r11:
      return "fdd-r11";
    case tdd_r11:
      return "tdd-r11";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rlf_report_r9_s::prev_utra_cell_id_r11_s_::pci_r11_c_::types");
  }
  return "";
}

std::string rlf_report_r9_s::sel_utra_cell_id_r11_s_::pci_r11_c_::types::to_string() const
{
  switch (value) {
    case fdd_r11:
      return "fdd-r11";
    case tdd_r11:
      return "tdd-r11";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rlf_report_r9_s::sel_utra_cell_id_r11_s_::pci_r11_c_::types");
  }
  return "";
}

std::string sidelink_ue_info_v1310_ies_s::comm_tx_res_info_req_relay_r13_s_::ue_type_r13_e_::to_string() const
{
  switch (value) {
    case relay_ue:
      return "relayUE";
    case remote_ue:
      return "remoteUE";
    default:
      invalid_enum_value(value, "sidelink_ue_info_v1310_ies_s::comm_tx_res_info_req_relay_r13_s_::ue_type_r13_e_");
  }
  return "";
}

std::string tdm_assist_info_r11_c::drx_assist_info_r11_s_::drx_cycle_len_r11_e_::to_string() const
{
  switch (value) {
    case sf40:
      return "sf40";
    case sf64:
      return "sf64";
    case sf80:
      return "sf80";
    case sf128:
      return "sf128";
    case sf160:
      return "sf160";
    case sf256:
      return "sf256";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "tdm_assist_info_r11_c::drx_assist_info_r11_s_::drx_cycle_len_r11_e_");
  }
  return "";
}
uint16_t tdm_assist_info_r11_c::drx_assist_info_r11_s_::drx_cycle_len_r11_e_::to_number() const
{
  const static uint16_t options[] = {40, 64, 80, 128, 160, 256};
  return get_enum_number(options, 6, value, "tdm_assist_info_r11_c::drx_assist_info_r11_s_::drx_cycle_len_r11_e_");
}

std::string tdm_assist_info_r11_c::drx_assist_info_r11_s_::drx_active_time_r11_e_::to_string() const
{
  switch (value) {
    case sf20:
      return "sf20";
    case sf30:
      return "sf30";
    case sf40:
      return "sf40";
    case sf60:
      return "sf60";
    case sf80:
      return "sf80";
    case sf100:
      return "sf100";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "tdm_assist_info_r11_c::drx_assist_info_r11_s_::drx_active_time_r11_e_");
  }
  return "";
}
uint8_t tdm_assist_info_r11_c::drx_assist_info_r11_s_::drx_active_time_r11_e_::to_number() const
{
  const static uint8_t options[] = {20, 30, 40, 60, 80, 100};
  return get_enum_number(options, 6, value, "tdm_assist_info_r11_c::drx_assist_info_r11_s_::drx_active_time_r11_e_");
}

std::string tdm_assist_info_r11_c::types::to_string() const
{
  switch (value) {
    case drx_assist_info_r11:
      return "drx-AssistanceInfo-r11";
    case idc_sf_pattern_list_r11:
      return "idc-SubframePatternList-r11";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "tdm_assist_info_r11_c::types");
  }
  return "";
}

std::string ueassist_info_v1430_ies_s::rlm_report_r14_s_::rlm_event_r14_e_::to_string() const
{
  switch (value) {
    case early_out_of_sync:
      return "earlyOutOfSync";
    case early_in_sync:
      return "earlyInSync";
    default:
      invalid_enum_value(value, "ueassist_info_v1430_ies_s::rlm_report_r14_s_::rlm_event_r14_e_");
  }
  return "";
}

std::string ueassist_info_v1430_ies_s::rlm_report_r14_s_::excess_rep_mpdcch_r14_e_::to_string() const
{
  switch (value) {
    case excess_rep1:
      return "excessRep1";
    case excess_rep2:
      return "excessRep2";
    default:
      invalid_enum_value(value, "ueassist_info_v1430_ies_s::rlm_report_r14_s_::excess_rep_mpdcch_r14_e_");
  }
  return "";
}
uint8_t ueassist_info_v1430_ies_s::rlm_report_r14_s_::excess_rep_mpdcch_r14_e_::to_number() const
{
  const static uint8_t options[] = {1, 2};
  return get_enum_number(options, 2, value, "ueassist_info_v1430_ies_s::rlm_report_r14_s_::excess_rep_mpdcch_r14_e_");
}

// WLAN-Status-r13 ::= ENUMERATED
std::string wlan_status_r13_e::to_string() const
{
  switch (value) {
    case successful_association:
      return "successfulAssociation";
    case fail_wlan_radio_link:
      return "failureWlanRadioLink";
    case fail_wlan_unavailable:
      return "failureWlanUnavailable";
    case fail_timeout:
      return "failureTimeout";
    default:
      invalid_enum_value(value, "wlan_status_r13_e");
  }
  return "";
}

std::string failed_lc_ch_info_r15_s::failed_lc_ch_id_r15_s_::cell_group_ind_r15_e_::to_string() const
{
  switch (value) {
    case mn:
      return "mn";
    case sn:
      return "sn";
    default:
      invalid_enum_value(value, "failed_lc_ch_info_r15_s::failed_lc_ch_id_r15_s_::cell_group_ind_r15_e_");
  }
  return "";
}

std::string failed_lc_ch_info_r15_s::fail_type_e_::to_string() const
{
  switch (value) {
    case dupl:
      return "duplication";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "failed_lc_ch_info_r15_s::fail_type_e_");
  }
  return "";
}

std::string inter_freq_rstd_meas_ind_r10_ies_s::rstd_inter_freq_ind_r10_c_::types::to_string() const
{
  switch (value) {
    case start:
      return "start";
    case stop:
      return "stop";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "inter_freq_rstd_meas_ind_r10_ies_s::rstd_inter_freq_ind_r10_c_::types");
  }
  return "";
}

std::string meas_report_app_layer_r15_ies_s::service_type_e_::to_string() const
{
  switch (value) {
    case qoe:
      return "qoe";
    case qoemtsi:
      return "qoemtsi";
    case spare6:
      return "spare6";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "meas_report_app_layer_r15_ies_s::service_type_e_");
  }
  return "";
}

std::string proximity_ind_r9_ies_s::type_r9_e_::to_string() const
{
  switch (value) {
    case entering:
      return "entering";
    case leaving:
      return "leaving";
    default:
      invalid_enum_value(value, "proximity_ind_r9_ies_s::type_r9_e_");
  }
  return "";
}

std::string proximity_ind_r9_ies_s::carrier_freq_r9_c_::types::to_string() const
{
  switch (value) {
    case eutra_r9:
      return "eutra-r9";
    case utra_r9:
      return "utra-r9";
    case eutra2_v9e0:
      return "eutra2-v9e0";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "proximity_ind_r9_ies_s::carrier_freq_r9_c_::types");
  }
  return "";
}
uint8_t proximity_ind_r9_ies_s::carrier_freq_r9_c_::types::to_number() const
{
  switch (value) {
    case eutra2_v9e0:
      return 2;
    default:
      invalid_enum_number(value, "proximity_ind_r9_ies_s::carrier_freq_r9_c_::types");
  }
  return 0;
}

std::string rrc_conn_resume_complete_r13_ies_s::mob_state_r13_e_::to_string() const
{
  switch (value) {
    case normal:
      return "normal";
    case medium:
      return "medium";
    case high:
      return "high";
    case spare:
      return "spare";
    default:
      invalid_enum_value(value, "rrc_conn_resume_complete_r13_ies_s::mob_state_r13_e_");
  }
  return "";
}

std::string ueassist_info_r11_ies_s::pwr_pref_ind_r11_e_::to_string() const
{
  switch (value) {
    case normal:
      return "normal";
    case low_pwr_consumption:
      return "lowPowerConsumption";
    default:
      invalid_enum_value(value, "ueassist_info_r11_ies_s::pwr_pref_ind_r11_e_");
  }
  return "";
}

std::string ul_info_transfer_r8_ies_s::ded_info_type_c_::types::to_string() const
{
  switch (value) {
    case ded_info_nas:
      return "dedicatedInfoNAS";
    case ded_info_cdma2000_minus1_xrtt:
      return "dedicatedInfoCDMA2000-1XRTT";
    case ded_info_cdma2000_hrpd:
      return "dedicatedInfoCDMA2000-HRPD";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ul_info_transfer_r8_ies_s::ded_info_type_c_::types");
  }
  return "";
}

std::string csfb_params_request_cdma2000_s::crit_exts_c_::types::to_string() const
{
  switch (value) {
    case csfb_params_request_cdma2000_r8:
      return "csfbParametersRequestCDMA2000-r8";
    case crit_exts_future:
      return "criticalExtensionsFuture";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "csfb_params_request_cdma2000_s::crit_exts_c_::types");
  }
  return "";
}
uint16_t csfb_params_request_cdma2000_s::crit_exts_c_::types::to_number() const
{
  const static uint16_t options[] = {2000};
  return get_enum_number(options, 1, value, "csfb_params_request_cdma2000_s::crit_exts_c_::types");
}

std::string counter_check_resp_s::crit_exts_c_::types::to_string() const
{
  switch (value) {
    case counter_check_resp_r8:
      return "counterCheckResponse-r8";
    case crit_exts_future:
      return "criticalExtensionsFuture";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "counter_check_resp_s::crit_exts_c_::types");
  }
  return "";
}

std::string in_dev_coex_ind_r11_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case in_dev_coex_ind_r11:
      return "inDeviceCoexIndication-r11";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "in_dev_coex_ind_r11_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string inter_freq_rstd_meas_ind_r10_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case inter_freq_rstd_meas_ind_r10:
      return "interFreqRSTDMeasurementIndication-r10";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "inter_freq_rstd_meas_ind_r10_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string mbms_count_resp_r10_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case count_resp_r10:
      return "countingResponse-r10";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "mbms_count_resp_r10_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string mbms_interest_ind_r11_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case interest_ind_r11:
      return "interestIndication-r11";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "mbms_interest_ind_r11_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string meas_report_app_layer_r15_s::crit_exts_c_::types::to_string() const
{
  switch (value) {
    case meas_report_app_layer_r15:
      return "measReportAppLayer-r15";
    case crit_exts_future:
      return "criticalExtensionsFuture";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "meas_report_app_layer_r15_s::crit_exts_c_::types");
  }
  return "";
}

std::string meas_report_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case meas_report_r8:
      return "measurementReport-r8";
    case spare7:
      return "spare7";
    case spare6:
      return "spare6";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "meas_report_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string proximity_ind_r9_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case proximity_ind_r9:
      return "proximityIndication-r9";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "proximity_ind_r9_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string rn_recfg_complete_r10_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case rn_recfg_complete_r10:
      return "rnReconfigurationComplete-r10";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rn_recfg_complete_r10_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string rrc_conn_recfg_complete_s::crit_exts_c_::types::to_string() const
{
  switch (value) {
    case rrc_conn_recfg_complete_r8:
      return "rrcConnectionReconfigurationComplete-r8";
    case crit_exts_future:
      return "criticalExtensionsFuture";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rrc_conn_recfg_complete_s::crit_exts_c_::types");
  }
  return "";
}

std::string rrc_conn_reest_complete_s::crit_exts_c_::types::to_string() const
{
  switch (value) {
    case rrc_conn_reest_complete_r8:
      return "rrcConnectionReestablishmentComplete-r8";
    case crit_exts_future:
      return "criticalExtensionsFuture";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rrc_conn_reest_complete_s::crit_exts_c_::types");
  }
  return "";
}

std::string rrc_conn_resume_complete_r13_s::crit_exts_c_::types::to_string() const
{
  switch (value) {
    case rrc_conn_resume_complete_r13:
      return "rrcConnectionResumeComplete-r13";
    case crit_exts_future:
      return "criticalExtensionsFuture";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rrc_conn_resume_complete_r13_s::crit_exts_c_::types");
  }
  return "";
}

std::string rrc_conn_setup_complete_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case rrc_conn_setup_complete_r8:
      return "rrcConnectionSetupComplete-r8";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "rrc_conn_setup_complete_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string scg_fail_info_r12_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case scg_fail_info_r12:
      return "scgFailureInformation-r12";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "scg_fail_info_r12_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string scg_fail_info_nr_r15_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case scg_fail_info_nr_r15:
      return "scgFailureInformationNR-r15";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "scg_fail_info_nr_r15_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string security_mode_complete_s::crit_exts_c_::types::to_string() const
{
  switch (value) {
    case security_mode_complete_r8:
      return "securityModeComplete-r8";
    case crit_exts_future:
      return "criticalExtensionsFuture";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "security_mode_complete_s::crit_exts_c_::types");
  }
  return "";
}

std::string security_mode_fail_s::crit_exts_c_::types::to_string() const
{
  switch (value) {
    case security_mode_fail_r8:
      return "securityModeFailure-r8";
    case crit_exts_future:
      return "criticalExtensionsFuture";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "security_mode_fail_s::crit_exts_c_::types");
  }
  return "";
}

std::string sidelink_ue_info_r12_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case sidelink_ue_info_r12:
      return "sidelinkUEInformation-r12";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "sidelink_ue_info_r12_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string ueassist_info_r11_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case ue_assist_info_r11:
      return "ueAssistanceInformation-r11";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ueassist_info_r11_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string ue_cap_info_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case ue_cap_info_r8:
      return "ueCapabilityInformation-r8";
    case spare7:
      return "spare7";
    case spare6:
      return "spare6";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ue_cap_info_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string ue_info_resp_r9_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case ue_info_resp_r9:
      return "ueInformationResponse-r9";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ue_info_resp_r9_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string ul_ho_prep_transfer_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case ul_ho_prep_transfer_r8:
      return "ulHandoverPreparationTransfer-r8";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ul_ho_prep_transfer_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string ul_info_transfer_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case ul_info_transfer_r8:
      return "ulInformationTransfer-r8";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ul_info_transfer_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string ul_info_transfer_mrdc_r15_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case ul_info_transfer_mrdc_r15:
      return "ulInformationTransferMRDC-r15";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ul_info_transfer_mrdc_r15_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string wlan_conn_status_report_r13_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case wlan_conn_status_report_r13:
      return "wlanConnectionStatusReport-r13";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "wlan_conn_status_report_r13_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string ul_dcch_msg_type_c::c1_c_::types::to_string() const
{
  switch (value) {
    case csfb_params_request_cdma2000:
      return "csfbParametersRequestCDMA2000";
    case meas_report:
      return "measurementReport";
    case rrc_conn_recfg_complete:
      return "rrcConnectionReconfigurationComplete";
    case rrc_conn_reest_complete:
      return "rrcConnectionReestablishmentComplete";
    case rrc_conn_setup_complete:
      return "rrcConnectionSetupComplete";
    case security_mode_complete:
      return "securityModeComplete";
    case security_mode_fail:
      return "securityModeFailure";
    case ue_cap_info:
      return "ueCapabilityInformation";
    case ul_ho_prep_transfer:
      return "ulHandoverPreparationTransfer";
    case ul_info_transfer:
      return "ulInformationTransfer";
    case counter_check_resp:
      return "counterCheckResponse";
    case ue_info_resp_r9:
      return "ueInformationResponse-r9";
    case proximity_ind_r9:
      return "proximityIndication-r9";
    case rn_recfg_complete_r10:
      return "rnReconfigurationComplete-r10";
    case mbms_count_resp_r10:
      return "mbmsCountingResponse-r10";
    case inter_freq_rstd_meas_ind_r10:
      return "interFreqRSTDMeasurementIndication-r10";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ul_dcch_msg_type_c::c1_c_::types");
  }
  return "";
}
uint16_t ul_dcch_msg_type_c::c1_c_::types::to_number() const
{
  const static uint16_t options[] = {2000};
  return get_enum_number(options, 1, value, "ul_dcch_msg_type_c::c1_c_::types");
}

std::string ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::types::to_string() const
{
  switch (value) {
    case ue_assist_info_r11:
      return "ueAssistanceInformation-r11";
    case in_dev_coex_ind_r11:
      return "inDeviceCoexIndication-r11";
    case mbms_interest_ind_r11:
      return "mbmsInterestIndication-r11";
    case scg_fail_info_r12:
      return "scgFailureInformation-r12";
    case sidelink_ue_info_r12:
      return "sidelinkUEInformation-r12";
    case wlan_conn_status_report_r13:
      return "wlanConnectionStatusReport-r13";
    case rrc_conn_resume_complete_r13:
      return "rrcConnectionResumeComplete-r13";
    case ul_info_transfer_mrdc_r15:
      return "ulInformationTransferMRDC-r15";
    case scg_fail_info_nr_r15:
      return "scgFailureInformationNR-r15";
    case meas_report_app_layer_r15:
      return "measReportAppLayer-r15";
    case fail_info_r15:
      return "failureInformation-r15";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::types");
  }
  return "";
}

std::string ul_dcch_msg_type_c::msg_class_ext_c_::types::to_string() const
{
  switch (value) {
    case c2:
      return "c2";
    case msg_class_ext_future_r11:
      return "messageClassExtensionFuture-r11";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ul_dcch_msg_type_c::msg_class_ext_c_::types");
  }
  return "";
}
uint8_t ul_dcch_msg_type_c::msg_class_ext_c_::types::to_number() const
{
  const static uint8_t options[] = {2};
  return get_enum_number(options, 1, value, "ul_dcch_msg_type_c::msg_class_ext_c_::types");
}

std::string ul_dcch_msg_type_c::types::to_string() const
{
  switch (value) {
    case c1:
      return "c1";
    case msg_class_ext:
      return "messageClassExtension";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ul_dcch_msg_type_c::types");
  }
  return "";
}
uint8_t ul_dcch_msg_type_c::types::to_number() const
{
  const static uint8_t options[] = {1};
  return get_enum_number(options, 1, value, "ul_dcch_msg_type_c::types");
}

// MIMO-CapabilityDL-r10 ::= ENUMERATED
std::string mimo_cap_dl_r10_e::to_string() const
{
  switch (value) {
    case two_layers:
      return "twoLayers";
    case four_layers:
      return "fourLayers";
    case eight_layers:
      return "eightLayers";
    default:
      invalid_enum_value(value, "mimo_cap_dl_r10_e");
  }
  return "";
}
uint8_t mimo_cap_dl_r10_e::to_number() const
{
  const static uint8_t options[] = {2, 4, 8};
  return get_enum_number(options, 3, value, "mimo_cap_dl_r10_e");
}

// MIMO-CapabilityUL-r10 ::= ENUMERATED
std::string mimo_cap_ul_r10_e::to_string() const
{
  switch (value) {
    case two_layers:
      return "twoLayers";
    case four_layers:
      return "fourLayers";
    default:
      invalid_enum_value(value, "mimo_cap_ul_r10_e");
  }
  return "";
}
uint8_t mimo_cap_ul_r10_e::to_number() const
{
  const static uint8_t options[] = {2, 4};
  return get_enum_number(options, 2, value, "mimo_cap_ul_r10_e");
}

std::string band_params_r11_s::supported_csi_proc_r11_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n3:
      return "n3";
    case n4:
      return "n4";
    default:
      invalid_enum_value(value, "band_params_r11_s::supported_csi_proc_r11_e_");
  }
  return "";
}
uint8_t band_params_r11_s::supported_csi_proc_r11_e_::to_number() const
{
  const static uint8_t options[] = {1, 3, 4};
  return get_enum_number(options, 3, value, "band_params_r11_s::supported_csi_proc_r11_e_");
}

std::string intra_band_contiguous_cc_info_r12_s::supported_csi_proc_r12_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n3:
      return "n3";
    case n4:
      return "n4";
    default:
      invalid_enum_value(value, "intra_band_contiguous_cc_info_r12_s::supported_csi_proc_r12_e_");
  }
  return "";
}
uint8_t intra_band_contiguous_cc_info_r12_s::supported_csi_proc_r12_e_::to_number() const
{
  const static uint8_t options[] = {1, 3, 4};
  return get_enum_number(options, 3, value, "intra_band_contiguous_cc_info_r12_s::supported_csi_proc_r12_e_");
}

std::string band_params_r13_s::supported_csi_proc_r13_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n3:
      return "n3";
    case n4:
      return "n4";
    default:
      invalid_enum_value(value, "band_params_r13_s::supported_csi_proc_r13_e_");
  }
  return "";
}
uint8_t band_params_r13_s::supported_csi_proc_r13_e_::to_number() const
{
  const static uint8_t options[] = {1, 3, 4};
  return get_enum_number(options, 3, value, "band_params_r13_s::supported_csi_proc_r13_e_");
}

std::string band_combination_params_r13_s::dc_support_r13_s_::supported_cell_grouping_r13_c_::types::to_string() const
{
  switch (value) {
    case three_entries_r13:
      return "threeEntries-r13";
    case four_entries_r13:
      return "fourEntries-r13";
    case five_entries_r13:
      return "fiveEntries-r13";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value,
                         "band_combination_params_r13_s::dc_support_r13_s_::supported_cell_grouping_r13_c_::types");
  }
  return "";
}
uint8_t band_combination_params_r13_s::dc_support_r13_s_::supported_cell_grouping_r13_c_::types::to_number() const
{
  const static uint8_t options[] = {3, 4, 5};
  return get_enum_number(options, 3, value,
                         "band_combination_params_r13_s::dc_support_r13_s_::supported_cell_grouping_r13_c_::types");
}

std::string band_params_v1130_s::supported_csi_proc_r11_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n3:
      return "n3";
    case n4:
      return "n4";
    default:
      invalid_enum_value(value, "band_params_v1130_s::supported_csi_proc_r11_e_");
  }
  return "";
}
uint8_t band_params_v1130_s::supported_csi_proc_r11_e_::to_number() const
{
  const static uint8_t options[] = {1, 3, 4};
  return get_enum_number(options, 3, value, "band_params_v1130_s::supported_csi_proc_r11_e_");
}

std::string band_combination_params_v1250_s::dc_support_r12_s_::supported_cell_grouping_r12_c_::types::to_string() const
{
  switch (value) {
    case three_entries_r12:
      return "threeEntries-r12";
    case four_entries_r12:
      return "fourEntries-r12";
    case five_entries_r12:
      return "fiveEntries-r12";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value,
                         "band_combination_params_v1250_s::dc_support_r12_s_::supported_cell_grouping_r12_c_::types");
  }
  return "";
}
uint8_t band_combination_params_v1250_s::dc_support_r12_s_::supported_cell_grouping_r12_c_::types::to_number() const
{
  const static uint8_t options[] = {3, 4, 5};
  return get_enum_number(options, 3, value,
                         "band_combination_params_v1250_s::dc_support_r12_s_::supported_cell_grouping_r12_c_::types");
}

std::string retuning_time_info_r14_s::retuning_info_s_::rf_retuning_time_dl_r14_e_::to_string() const
{
  switch (value) {
    case n0:
      return "n0";
    case n0dot5:
      return "n0dot5";
    case n1:
      return "n1";
    case n1dot5:
      return "n1dot5";
    case n2:
      return "n2";
    case n2dot5:
      return "n2dot5";
    case n3:
      return "n3";
    case n3dot5:
      return "n3dot5";
    case n4:
      return "n4";
    case n4dot5:
      return "n4dot5";
    case n5:
      return "n5";
    case n5dot5:
      return "n5dot5";
    case n6:
      return "n6";
    case n6dot5:
      return "n6dot5";
    case n7:
      return "n7";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "retuning_time_info_r14_s::retuning_info_s_::rf_retuning_time_dl_r14_e_");
  }
  return "";
}
float retuning_time_info_r14_s::retuning_info_s_::rf_retuning_time_dl_r14_e_::to_number() const
{
  const static float options[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0};
  return get_enum_number(options, 15, value, "retuning_time_info_r14_s::retuning_info_s_::rf_retuning_time_dl_r14_e_");
}
std::string retuning_time_info_r14_s::retuning_info_s_::rf_retuning_time_dl_r14_e_::to_number_string() const
{
  switch (value) {
    case n0:
      return "0";
    case n0dot5:
      return "0.5";
    case n1:
      return "1";
    case n1dot5:
      return "1.5";
    case n2:
      return "2";
    case n2dot5:
      return "2.5";
    case n3:
      return "3";
    case n3dot5:
      return "3.5";
    case n4:
      return "4";
    case n4dot5:
      return "4.5";
    case n5:
      return "5";
    case n5dot5:
      return "5.5";
    case n6:
      return "6";
    case n6dot5:
      return "6.5";
    case n7:
      return "7";
    default:
      invalid_enum_number(value, "retuning_time_info_r14_s::retuning_info_s_::rf_retuning_time_dl_r14_e_");
  }
  return "";
}

std::string retuning_time_info_r14_s::retuning_info_s_::rf_retuning_time_ul_r14_e_::to_string() const
{
  switch (value) {
    case n0:
      return "n0";
    case n0dot5:
      return "n0dot5";
    case n1:
      return "n1";
    case n1dot5:
      return "n1dot5";
    case n2:
      return "n2";
    case n2dot5:
      return "n2dot5";
    case n3:
      return "n3";
    case n3dot5:
      return "n3dot5";
    case n4:
      return "n4";
    case n4dot5:
      return "n4dot5";
    case n5:
      return "n5";
    case n5dot5:
      return "n5dot5";
    case n6:
      return "n6";
    case n6dot5:
      return "n6dot5";
    case n7:
      return "n7";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "retuning_time_info_r14_s::retuning_info_s_::rf_retuning_time_ul_r14_e_");
  }
  return "";
}
float retuning_time_info_r14_s::retuning_info_s_::rf_retuning_time_ul_r14_e_::to_number() const
{
  const static float options[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0};
  return get_enum_number(options, 15, value, "retuning_time_info_r14_s::retuning_info_s_::rf_retuning_time_ul_r14_e_");
}
std::string retuning_time_info_r14_s::retuning_info_s_::rf_retuning_time_ul_r14_e_::to_number_string() const
{
  switch (value) {
    case n0:
      return "0";
    case n0dot5:
      return "0.5";
    case n1:
      return "1";
    case n1dot5:
      return "1.5";
    case n2:
      return "2";
    case n2dot5:
      return "2.5";
    case n3:
      return "3";
    case n3dot5:
      return "3.5";
    case n4:
      return "4";
    case n4dot5:
      return "4.5";
    case n5:
      return "5";
    case n5dot5:
      return "5.5";
    case n6:
      return "6";
    case n6dot5:
      return "6.5";
    case n7:
      return "7";
    default:
      invalid_enum_number(value, "retuning_time_info_r14_s::retuning_info_s_::rf_retuning_time_ul_r14_e_");
  }
  return "";
}

std::string mimo_ca_params_per_bo_bc_per_tm_v1470_s::csi_report_advanced_max_ports_r14_e_::to_string() const
{
  switch (value) {
    case n8:
      return "n8";
    case n12:
      return "n12";
    case n16:
      return "n16";
    case n20:
      return "n20";
    case n24:
      return "n24";
    case n28:
      return "n28";
    default:
      invalid_enum_value(value, "mimo_ca_params_per_bo_bc_per_tm_v1470_s::csi_report_advanced_max_ports_r14_e_");
  }
  return "";
}
uint8_t mimo_ca_params_per_bo_bc_per_tm_v1470_s::csi_report_advanced_max_ports_r14_e_::to_number() const
{
  const static uint8_t options[] = {8, 12, 16, 20, 24, 28};
  return get_enum_number(options, 6, value,
                         "mimo_ca_params_per_bo_bc_per_tm_v1470_s::csi_report_advanced_max_ports_r14_e_");
}

std::string stti_spt_band_params_r15_s::s_tti_supported_csi_proc_r15_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n3:
      return "n3";
    case n4:
      return "n4";
    default:
      invalid_enum_value(value, "stti_spt_band_params_r15_s::s_tti_supported_csi_proc_r15_e_");
  }
  return "";
}
uint8_t stti_spt_band_params_r15_s::s_tti_supported_csi_proc_r15_e_::to_number() const
{
  const static uint8_t options[] = {1, 3, 4};
  return get_enum_number(options, 3, value, "stti_spt_band_params_r15_s::s_tti_supported_csi_proc_r15_e_");
}

// V2X-BandwidthClass-r14 ::= ENUMERATED
std::string v2x_bw_class_r14_e::to_string() const
{
  switch (value) {
    case a:
      return "a";
    case b:
      return "b";
    case c:
      return "c";
    case d:
      return "d";
    case e:
      return "e";
    case f:
      return "f";
    case c1_v1530:
      return "c1-v1530";
    default:
      invalid_enum_value(value, "v2x_bw_class_r14_e");
  }
  return "";
}
uint8_t v2x_bw_class_r14_e::to_number() const
{
  switch (value) {
    case c1_v1530:
      return 1;
    default:
      invalid_enum_number(value, "v2x_bw_class_r14_e");
  }
  return 0;
}

std::string feature_set_dl_per_cc_r15_s::supported_csi_proc_r15_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n3:
      return "n3";
    case n4:
      return "n4";
    default:
      invalid_enum_value(value, "feature_set_dl_per_cc_r15_s::supported_csi_proc_r15_e_");
  }
  return "";
}
uint8_t feature_set_dl_per_cc_r15_s::supported_csi_proc_r15_e_::to_number() const
{
  const static uint8_t options[] = {1, 3, 4};
  return get_enum_number(options, 3, value, "feature_set_dl_per_cc_r15_s::supported_csi_proc_r15_e_");
}

std::string irat_params_cdma2000_minus1_xrtt_s::tx_cfg1_xrtt_e_::to_string() const
{
  switch (value) {
    case single:
      return "single";
    case dual:
      return "dual";
    default:
      invalid_enum_value(value, "irat_params_cdma2000_minus1_xrtt_s::tx_cfg1_xrtt_e_");
  }
  return "";
}

std::string irat_params_cdma2000_minus1_xrtt_s::rx_cfg1_xrtt_e_::to_string() const
{
  switch (value) {
    case single:
      return "single";
    case dual:
      return "dual";
    default:
      invalid_enum_value(value, "irat_params_cdma2000_minus1_xrtt_s::rx_cfg1_xrtt_e_");
  }
  return "";
}

std::string irat_params_cdma2000_hrpd_s::tx_cfg_hrpd_e_::to_string() const
{
  switch (value) {
    case single:
      return "single";
    case dual:
      return "dual";
    default:
      invalid_enum_value(value, "irat_params_cdma2000_hrpd_s::tx_cfg_hrpd_e_");
  }
  return "";
}

std::string irat_params_cdma2000_hrpd_s::rx_cfg_hrpd_e_::to_string() const
{
  switch (value) {
    case single:
      return "single";
    case dual:
      return "dual";
    default:
      invalid_enum_value(value, "irat_params_cdma2000_hrpd_s::rx_cfg_hrpd_e_");
  }
  return "";
}

// SupportedBandGERAN ::= ENUMERATED
std::string supported_band_geran_e::to_string() const
{
  switch (value) {
    case gsm450:
      return "gsm450";
    case gsm480:
      return "gsm480";
    case gsm710:
      return "gsm710";
    case gsm750:
      return "gsm750";
    case gsm810:
      return "gsm810";
    case gsm850:
      return "gsm850";
    case gsm900_p:
      return "gsm900P";
    case gsm900_e:
      return "gsm900E";
    case gsm900_r:
      return "gsm900R";
    case gsm1800:
      return "gsm1800";
    case gsm1900:
      return "gsm1900";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "supported_band_geran_e");
  }
  return "";
}

// SupportedBandUTRA-FDD ::= ENUMERATED
std::string supported_band_utra_fdd_e::to_string() const
{
  switch (value) {
    case band_i:
      return "bandI";
    case band_ii:
      return "bandII";
    case band_iii:
      return "bandIII";
    case band_iv:
      return "bandIV";
    case band_v:
      return "bandV";
    case band_vi:
      return "bandVI";
    case band_vii:
      return "bandVII";
    case band_viii:
      return "bandVIII";
    case band_ix:
      return "bandIX";
    case band_x:
      return "bandX";
    case band_xi:
      return "bandXI";
    case band_xii:
      return "bandXII";
    case band_xiii:
      return "bandXIII";
    case band_xiv:
      return "bandXIV";
    case band_xv:
      return "bandXV";
    case band_xvi:
      return "bandXVI";
    case band_xvii_minus8a0:
      return "bandXVII-8a0";
    case band_xviii_minus8a0:
      return "bandXVIII-8a0";
    case band_xix_minus8a0:
      return "bandXIX-8a0";
    case band_xx_minus8a0:
      return "bandXX-8a0";
    case band_xxi_minus8a0:
      return "bandXXI-8a0";
    case band_xxii_minus8a0:
      return "bandXXII-8a0";
    case band_xxiii_minus8a0:
      return "bandXXIII-8a0";
    case band_xxiv_minus8a0:
      return "bandXXIV-8a0";
    case band_xxv_minus8a0:
      return "bandXXV-8a0";
    case band_xxvi_minus8a0:
      return "bandXXVI-8a0";
    case band_xxvii_minus8a0:
      return "bandXXVII-8a0";
    case band_xxviii_minus8a0:
      return "bandXXVIII-8a0";
    case band_xxix_minus8a0:
      return "bandXXIX-8a0";
    case band_xxx_minus8a0:
      return "bandXXX-8a0";
    case band_xxxi_minus8a0:
      return "bandXXXI-8a0";
    case band_xxxii_minus8a0:
      return "bandXXXII-8a0";
    default:
      invalid_enum_value(value, "supported_band_utra_fdd_e");
  }
  return "";
}

// SupportedBandUTRA-TDD128 ::= ENUMERATED
std::string supported_band_utra_tdd128_e::to_string() const
{
  switch (value) {
    case a:
      return "a";
    case b:
      return "b";
    case c:
      return "c";
    case d:
      return "d";
    case e:
      return "e";
    case f:
      return "f";
    case g:
      return "g";
    case h:
      return "h";
    case i:
      return "i";
    case j:
      return "j";
    case k:
      return "k";
    case l:
      return "l";
    case m:
      return "m";
    case n:
      return "n";
    case o:
      return "o";
    case p:
      return "p";
    default:
      invalid_enum_value(value, "supported_band_utra_tdd128_e");
  }
  return "";
}

// SupportedBandUTRA-TDD384 ::= ENUMERATED
std::string supported_band_utra_tdd384_e::to_string() const
{
  switch (value) {
    case a:
      return "a";
    case b:
      return "b";
    case c:
      return "c";
    case d:
      return "d";
    case e:
      return "e";
    case f:
      return "f";
    case g:
      return "g";
    case h:
      return "h";
    case i:
      return "i";
    case j:
      return "j";
    case k:
      return "k";
    case l:
      return "l";
    case m:
      return "m";
    case n:
      return "n";
    case o:
      return "o";
    case p:
      return "p";
    default:
      invalid_enum_value(value, "supported_band_utra_tdd384_e");
  }
  return "";
}

// SupportedBandUTRA-TDD768 ::= ENUMERATED
std::string supported_band_utra_tdd768_e::to_string() const
{
  switch (value) {
    case a:
      return "a";
    case b:
      return "b";
    case c:
      return "c";
    case d:
      return "d";
    case e:
      return "e";
    case f:
      return "f";
    case g:
      return "g";
    case h:
      return "h";
    case i:
      return "i";
    case j:
      return "j";
    case k:
      return "k";
    case l:
      return "l";
    case m:
      return "m";
    case n:
      return "n";
    case o:
      return "o";
    case p:
      return "p";
    default:
      invalid_enum_value(value, "supported_band_utra_tdd768_e");
  }
  return "";
}

// ProcessingTimelineSet-r15 ::= ENUMERATED
std::string processing_timeline_set_r15_e::to_string() const
{
  switch (value) {
    case set1:
      return "set1";
    case set2:
      return "set2";
    default:
      invalid_enum_value(value, "processing_timeline_set_r15_e");
  }
  return "";
}
uint8_t processing_timeline_set_r15_e::to_number() const
{
  const static uint8_t options[] = {1, 2};
  return get_enum_number(options, 2, value, "processing_timeline_set_r15_e");
}

std::string mimo_ue_params_per_tm_v1430_s::nzp_csi_rs_aperiodic_info_r14_s_::n_max_res_r14_e_::to_string() const
{
  switch (value) {
    case ffs1:
      return "ffs1";
    case ffs2:
      return "ffs2";
    case ffs3:
      return "ffs3";
    case ffs4:
      return "ffs4";
    default:
      invalid_enum_value(value, "mimo_ue_params_per_tm_v1430_s::nzp_csi_rs_aperiodic_info_r14_s_::n_max_res_r14_e_");
  }
  return "";
}
uint8_t mimo_ue_params_per_tm_v1430_s::nzp_csi_rs_aperiodic_info_r14_s_::n_max_res_r14_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 4};
  return get_enum_number(options, 4, value,
                         "mimo_ue_params_per_tm_v1430_s::nzp_csi_rs_aperiodic_info_r14_s_::n_max_res_r14_e_");
}

std::string mimo_ue_params_per_tm_v1430_s::nzp_csi_rs_periodic_info_r14_s_::n_max_res_r14_e_::to_string() const
{
  switch (value) {
    case ffs1:
      return "ffs1";
    case ffs2:
      return "ffs2";
    case ffs3:
      return "ffs3";
    case ffs4:
      return "ffs4";
    default:
      invalid_enum_value(value, "mimo_ue_params_per_tm_v1430_s::nzp_csi_rs_periodic_info_r14_s_::n_max_res_r14_e_");
  }
  return "";
}
uint8_t mimo_ue_params_per_tm_v1430_s::nzp_csi_rs_periodic_info_r14_s_::n_max_res_r14_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 4};
  return get_enum_number(options, 4, value,
                         "mimo_ue_params_per_tm_v1430_s::nzp_csi_rs_periodic_info_r14_s_::n_max_res_r14_e_");
}

std::string mimo_ue_params_per_tm_v1470_s::csi_report_advanced_max_ports_r14_e_::to_string() const
{
  switch (value) {
    case n8:
      return "n8";
    case n12:
      return "n12";
    case n16:
      return "n16";
    case n20:
      return "n20";
    case n24:
      return "n24";
    case n28:
      return "n28";
    default:
      invalid_enum_value(value, "mimo_ue_params_per_tm_v1470_s::csi_report_advanced_max_ports_r14_e_");
  }
  return "";
}
uint8_t mimo_ue_params_per_tm_v1470_s::csi_report_advanced_max_ports_r14_e_::to_number() const
{
  const static uint8_t options[] = {8, 12, 16, 20, 24, 28};
  return get_enum_number(options, 6, value, "mimo_ue_params_per_tm_v1470_s::csi_report_advanced_max_ports_r14_e_");
}

std::string naics_cap_entry_r12_s::nof_aggregated_prb_r12_e_::to_string() const
{
  switch (value) {
    case n50:
      return "n50";
    case n75:
      return "n75";
    case n100:
      return "n100";
    case n125:
      return "n125";
    case n150:
      return "n150";
    case n175:
      return "n175";
    case n200:
      return "n200";
    case n225:
      return "n225";
    case n250:
      return "n250";
    case n275:
      return "n275";
    case n300:
      return "n300";
    case n350:
      return "n350";
    case n400:
      return "n400";
    case n450:
      return "n450";
    case n500:
      return "n500";
    case spare:
      return "spare";
    default:
      invalid_enum_value(value, "naics_cap_entry_r12_s::nof_aggregated_prb_r12_e_");
  }
  return "";
}
uint16_t naics_cap_entry_r12_s::nof_aggregated_prb_r12_e_::to_number() const
{
  const static uint16_t options[] = {50, 75, 100, 125, 150, 175, 200, 225, 250, 275, 300, 350, 400, 450, 500};
  return get_enum_number(options, 15, value, "naics_cap_entry_r12_s::nof_aggregated_prb_r12_e_");
}

std::string pdcp_params_s::max_num_rohc_context_sessions_e_::to_string() const
{
  switch (value) {
    case cs2:
      return "cs2";
    case cs4:
      return "cs4";
    case cs8:
      return "cs8";
    case cs12:
      return "cs12";
    case cs16:
      return "cs16";
    case cs24:
      return "cs24";
    case cs32:
      return "cs32";
    case cs48:
      return "cs48";
    case cs64:
      return "cs64";
    case cs128:
      return "cs128";
    case cs256:
      return "cs256";
    case cs512:
      return "cs512";
    case cs1024:
      return "cs1024";
    case cs16384:
      return "cs16384";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "pdcp_params_s::max_num_rohc_context_sessions_e_");
  }
  return "";
}
uint16_t pdcp_params_s::max_num_rohc_context_sessions_e_::to_number() const
{
  const static uint16_t options[] = {2, 4, 8, 12, 16, 24, 32, 48, 64, 128, 256, 512, 1024, 16384};
  return get_enum_number(options, 14, value, "pdcp_params_s::max_num_rohc_context_sessions_e_");
}

std::string pdcp_params_nr_r15_s::rohc_context_max_sessions_r15_e_::to_string() const
{
  switch (value) {
    case cs2:
      return "cs2";
    case cs4:
      return "cs4";
    case cs8:
      return "cs8";
    case cs12:
      return "cs12";
    case cs16:
      return "cs16";
    case cs24:
      return "cs24";
    case cs32:
      return "cs32";
    case cs48:
      return "cs48";
    case cs64:
      return "cs64";
    case cs128:
      return "cs128";
    case cs256:
      return "cs256";
    case cs512:
      return "cs512";
    case cs1024:
      return "cs1024";
    case cs16384:
      return "cs16384";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "pdcp_params_nr_r15_s::rohc_context_max_sessions_r15_e_");
  }
  return "";
}
uint16_t pdcp_params_nr_r15_s::rohc_context_max_sessions_r15_e_::to_number() const
{
  const static uint16_t options[] = {2, 4, 8, 12, 16, 24, 32, 48, 64, 128, 256, 512, 1024, 16384};
  return get_enum_number(options, 14, value, "pdcp_params_nr_r15_s::rohc_context_max_sessions_r15_e_");
}

std::string phy_layer_params_v1430_s::ce_pdsch_pusch_max_bw_r14_e_::to_string() const
{
  switch (value) {
    case bw5:
      return "bw5";
    case bw20:
      return "bw20";
    default:
      invalid_enum_value(value, "phy_layer_params_v1430_s::ce_pdsch_pusch_max_bw_r14_e_");
  }
  return "";
}
uint8_t phy_layer_params_v1430_s::ce_pdsch_pusch_max_bw_r14_e_::to_number() const
{
  const static uint8_t options[] = {5, 20};
  return get_enum_number(options, 2, value, "phy_layer_params_v1430_s::ce_pdsch_pusch_max_bw_r14_e_");
}

std::string phy_layer_params_v1430_s::ce_retuning_symbols_r14_e_::to_string() const
{
  switch (value) {
    case n0:
      return "n0";
    case n1:
      return "n1";
    default:
      invalid_enum_value(value, "phy_layer_params_v1430_s::ce_retuning_symbols_r14_e_");
  }
  return "";
}
uint8_t phy_layer_params_v1430_s::ce_retuning_symbols_r14_e_::to_number() const
{
  const static uint8_t options[] = {0, 1};
  return get_enum_number(options, 2, value, "phy_layer_params_v1430_s::ce_retuning_symbols_r14_e_");
}

std::string
phy_layer_params_v1530_s::stti_spt_capabilities_r15_s_::max_layers_slot_or_subslot_pusch_r15_e_::to_string() const
{
  switch (value) {
    case one_layer:
      return "oneLayer";
    case two_layers:
      return "twoLayers";
    case four_layers:
      return "fourLayers";
    default:
      invalid_enum_value(
          value, "phy_layer_params_v1530_s::stti_spt_capabilities_r15_s_::max_layers_slot_or_subslot_pusch_r15_e_");
  }
  return "";
}
uint8_t
phy_layer_params_v1530_s::stti_spt_capabilities_r15_s_::max_layers_slot_or_subslot_pusch_r15_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 4};
  return get_enum_number(
      options, 3, value,
      "phy_layer_params_v1530_s::stti_spt_capabilities_r15_s_::max_layers_slot_or_subslot_pusch_r15_e_");
}

std::string phy_layer_params_v1530_s::stti_spt_capabilities_r15_s_::sps_stti_r15_e_::to_string() const
{
  switch (value) {
    case slot:
      return "slot";
    case subslot:
      return "subslot";
    case slot_and_subslot:
      return "slotAndSubslot";
    default:
      invalid_enum_value(value, "phy_layer_params_v1530_s::stti_spt_capabilities_r15_s_::sps_stti_r15_e_");
  }
  return "";
}

std::string supported_band_eutra_v1320_s::ue_pwr_class_n_r13_e_::to_string() const
{
  switch (value) {
    case class1:
      return "class1";
    case class2:
      return "class2";
    case class4:
      return "class4";
    default:
      invalid_enum_value(value, "supported_band_eutra_v1320_s::ue_pwr_class_n_r13_e_");
  }
  return "";
}
uint8_t supported_band_eutra_v1320_s::ue_pwr_class_n_r13_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 4};
  return get_enum_number(options, 3, value, "supported_band_eutra_v1320_s::ue_pwr_class_n_r13_e_");
}

std::string sl_params_r12_s::disc_supported_proc_r12_e_::to_string() const
{
  switch (value) {
    case n50:
      return "n50";
    case n400:
      return "n400";
    default:
      invalid_enum_value(value, "sl_params_r12_s::disc_supported_proc_r12_e_");
  }
  return "";
}
uint16_t sl_params_r12_s::disc_supported_proc_r12_e_::to_number() const
{
  const static uint16_t options[] = {50, 400};
  return get_enum_number(options, 2, value, "sl_params_r12_s::disc_supported_proc_r12_e_");
}

std::string sl_params_v1530_s::slss_supported_tx_freq_r15_e_::to_string() const
{
  switch (value) {
    case single:
      return "single";
    case multiple:
      return "multiple";
    default:
      invalid_enum_value(value, "sl_params_v1530_s::slss_supported_tx_freq_r15_e_");
  }
  return "";
}

std::string sps_cfg_dl_stti_r15_c::setup_s_::semi_persist_sched_interv_dl_stti_r15_e_::to_string() const
{
  switch (value) {
    case s_tti1:
      return "sTTI1";
    case s_tti2:
      return "sTTI2";
    case s_tti3:
      return "sTTI3";
    case s_tti4:
      return "sTTI4";
    case s_tti6:
      return "sTTI6";
    case s_tti8:
      return "sTTI8";
    case s_tti12:
      return "sTTI12";
    case s_tti16:
      return "sTTI16";
    case s_tti20:
      return "sTTI20";
    case s_tti40:
      return "sTTI40";
    case s_tti60:
      return "sTTI60";
    case s_tti80:
      return "sTTI80";
    case s_tti120:
      return "sTTI120";
    case s_tti240:
      return "sTTI240";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "sps_cfg_dl_stti_r15_c::setup_s_::semi_persist_sched_interv_dl_stti_r15_e_");
  }
  return "";
}
uint8_t sps_cfg_dl_stti_r15_c::setup_s_::semi_persist_sched_interv_dl_stti_r15_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 4, 6, 8, 12, 16, 20, 40, 60, 80, 120, 240};
  return get_enum_number(options, 14, value,
                         "sps_cfg_dl_stti_r15_c::setup_s_::semi_persist_sched_interv_dl_stti_r15_e_");
}

std::string laa_params_v1430_s::two_step_sched_timing_info_r14_e_::to_string() const
{
  switch (value) {
    case n_plus1:
      return "nPlus1";
    case n_plus2:
      return "nPlus2";
    case n_plus3:
      return "nPlus3";
    default:
      invalid_enum_value(value, "laa_params_v1430_s::two_step_sched_timing_info_r14_e_");
  }
  return "";
}
uint8_t laa_params_v1430_s::two_step_sched_timing_info_r14_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3};
  return get_enum_number(options, 3, value, "laa_params_v1430_s::two_step_sched_timing_info_r14_e_");
}

std::string pdcp_params_v1430_s::max_num_rohc_context_sessions_r14_e_::to_string() const
{
  switch (value) {
    case cs2:
      return "cs2";
    case cs4:
      return "cs4";
    case cs8:
      return "cs8";
    case cs12:
      return "cs12";
    case cs16:
      return "cs16";
    case cs24:
      return "cs24";
    case cs32:
      return "cs32";
    case cs48:
      return "cs48";
    case cs64:
      return "cs64";
    case cs128:
      return "cs128";
    case cs256:
      return "cs256";
    case cs512:
      return "cs512";
    case cs1024:
      return "cs1024";
    case cs16384:
      return "cs16384";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "pdcp_params_v1430_s::max_num_rohc_context_sessions_r14_e_");
  }
  return "";
}
uint16_t pdcp_params_v1430_s::max_num_rohc_context_sessions_r14_e_::to_number() const
{
  const static uint16_t options[] = {2, 4, 8, 12, 16, 24, 32, 48, 64, 128, 256, 512, 1024, 16384};
  return get_enum_number(options, 14, value, "pdcp_params_v1430_s::max_num_rohc_context_sessions_r14_e_");
}

std::string mbms_params_v1470_s::mbms_max_bw_r14_c_::types::to_string() const
{
  switch (value) {
    case implicit_value:
      return "implicitValue";
    case explicit_value:
      return "explicitValue";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "mbms_params_v1470_s::mbms_max_bw_r14_c_::types");
  }
  return "";
}

std::string mbms_params_v1470_s::mbms_scaling_factor1dot25_r14_e_::to_string() const
{
  switch (value) {
    case n3:
      return "n3";
    case n6:
      return "n6";
    case n9:
      return "n9";
    case n12:
      return "n12";
    default:
      invalid_enum_value(value, "mbms_params_v1470_s::mbms_scaling_factor1dot25_r14_e_");
  }
  return "";
}
uint8_t mbms_params_v1470_s::mbms_scaling_factor1dot25_r14_e_::to_number() const
{
  const static uint8_t options[] = {3, 6, 9, 12};
  return get_enum_number(options, 4, value, "mbms_params_v1470_s::mbms_scaling_factor1dot25_r14_e_");
}

std::string mbms_params_v1470_s::mbms_scaling_factor7dot5_r14_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n2:
      return "n2";
    case n3:
      return "n3";
    case n4:
      return "n4";
    default:
      invalid_enum_value(value, "mbms_params_v1470_s::mbms_scaling_factor7dot5_r14_e_");
  }
  return "";
}
uint8_t mbms_params_v1470_s::mbms_scaling_factor7dot5_r14_e_::to_number() const
{
  const static uint8_t options[] = {1, 2, 3, 4};
  return get_enum_number(options, 4, value, "mbms_params_v1470_s::mbms_scaling_factor7dot5_r14_e_");
}

std::string ue_eutra_cap_v1430_ies_s::ue_category_ul_v1430_e_::to_string() const
{
  switch (value) {
    case n16:
      return "n16";
    case n17:
      return "n17";
    case n18:
      return "n18";
    case n19:
      return "n19";
    case n20:
      return "n20";
    case m2:
      return "m2";
    default:
      invalid_enum_value(value, "ue_eutra_cap_v1430_ies_s::ue_category_ul_v1430_e_");
  }
  return "";
}
uint8_t ue_eutra_cap_v1430_ies_s::ue_category_ul_v1430_e_::to_number() const
{
  const static uint8_t options[] = {16, 17, 18, 19, 20, 2};
  return get_enum_number(options, 6, value, "ue_eutra_cap_v1430_ies_s::ue_category_ul_v1430_e_");
}

std::string ue_eutra_cap_v1310_ies_s::ue_category_dl_v1310_e_::to_string() const
{
  switch (value) {
    case n17:
      return "n17";
    case m1:
      return "m1";
    default:
      invalid_enum_value(value, "ue_eutra_cap_v1310_ies_s::ue_category_dl_v1310_e_");
  }
  return "";
}
uint8_t ue_eutra_cap_v1310_ies_s::ue_category_dl_v1310_e_::to_number() const
{
  const static uint8_t options[] = {17, 1};
  return get_enum_number(options, 2, value, "ue_eutra_cap_v1310_ies_s::ue_category_dl_v1310_e_");
}

std::string ue_eutra_cap_v1310_ies_s::ue_category_ul_v1310_e_::to_string() const
{
  switch (value) {
    case n14:
      return "n14";
    case m1:
      return "m1";
    default:
      invalid_enum_value(value, "ue_eutra_cap_v1310_ies_s::ue_category_ul_v1310_e_");
  }
  return "";
}
uint8_t ue_eutra_cap_v1310_ies_s::ue_category_ul_v1310_e_::to_number() const
{
  const static uint8_t options[] = {14, 1};
  return get_enum_number(options, 2, value, "ue_eutra_cap_v1310_ies_s::ue_category_ul_v1310_e_");
}

// AccessStratumRelease ::= ENUMERATED
std::string access_stratum_release_e::to_string() const
{
  switch (value) {
    case rel8:
      return "rel8";
    case rel9:
      return "rel9";
    case rel10:
      return "rel10";
    case rel11:
      return "rel11";
    case rel12:
      return "rel12";
    case rel13:
      return "rel13";
    case rel14:
      return "rel14";
    case rel15:
      return "rel15";
    default:
      invalid_enum_value(value, "access_stratum_release_e");
  }
  return "";
}
uint8_t access_stratum_release_e::to_number() const
{
  const static uint8_t options[] = {8, 9, 10, 11, 12, 13, 14, 15};
  return get_enum_number(options, 8, value, "access_stratum_release_e");
}

std::string scg_cfg_r12_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case scg_cfg_r12:
      return "scg-Config-r12";
    case spare7:
      return "spare7";
    case spare6:
      return "spare6";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "scg_cfg_r12_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string cells_triggered_list_item_c_::pci_utra_c_::types::to_string() const
{
  switch (value) {
    case fdd:
      return "fdd";
    case tdd:
      return "tdd";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "cells_triggered_list_item_c_::pci_utra_c_::types");
  }
  return "";
}

std::string cells_triggered_list_item_c_::types::to_string() const
{
  switch (value) {
    case pci_eutra:
      return "physCellIdEUTRA";
    case pci_utra:
      return "physCellIdUTRA";
    case pci_geran:
      return "physCellIdGERAN";
    case pci_cdma2000:
      return "physCellIdCDMA2000";
    case wlan_ids_r13:
      return "wlan-Identifiers-r13";
    case pci_nr_r15:
      return "physCellIdNR-r15";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "cells_triggered_list_item_c_::types");
  }
  return "";
}
uint16_t cells_triggered_list_item_c_::types::to_number() const
{
  switch (value) {
    case pci_cdma2000:
      return 2000;
    default:
      invalid_enum_number(value, "cells_triggered_list_item_c_::types");
  }
  return 0;
}

std::string drb_info_scg_r12_s::drb_type_r12_e_::to_string() const
{
  switch (value) {
    case split:
      return "split";
    case scg:
      return "scg";
    default:
      invalid_enum_value(value, "drb_info_scg_r12_s::drb_type_r12_e_");
  }
  return "";
}

std::string ho_cmd_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case ho_cmd_r8:
      return "handoverCommand-r8";
    case spare7:
      return "spare7";
    case spare6:
      return "spare6";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ho_cmd_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string ho_prep_info_v920_ies_s::ue_cfg_release_r9_e_::to_string() const
{
  switch (value) {
    case rel9:
      return "rel9";
    case rel10:
      return "rel10";
    case rel11:
      return "rel11";
    case rel12:
      return "rel12";
    case v10j0:
      return "v10j0";
    case v11e0:
      return "v11e0";
    case v1280:
      return "v1280";
    case rel13:
      return "rel13";
    case rel14:
      return "rel14";
    case rel15:
      return "rel15";
    default:
      invalid_enum_value(value, "ho_prep_info_v920_ies_s::ue_cfg_release_r9_e_");
  }
  return "";
}

std::string rrm_cfg_s::ue_inactive_time_e_::to_string() const
{
  switch (value) {
    case s1:
      return "s1";
    case s2:
      return "s2";
    case s3:
      return "s3";
    case s5:
      return "s5";
    case s7:
      return "s7";
    case s10:
      return "s10";
    case s15:
      return "s15";
    case s20:
      return "s20";
    case s25:
      return "s25";
    case s30:
      return "s30";
    case s40:
      return "s40";
    case s50:
      return "s50";
    case min1:
      return "min1";
    case min1s20c:
      return "min1s20c";
    case min1s40:
      return "min1s40";
    case min2:
      return "min2";
    case min2s30:
      return "min2s30";
    case min3:
      return "min3";
    case min3s30:
      return "min3s30";
    case min4:
      return "min4";
    case min5:
      return "min5";
    case min6:
      return "min6";
    case min7:
      return "min7";
    case min8:
      return "min8";
    case min9:
      return "min9";
    case min10:
      return "min10";
    case min12:
      return "min12";
    case min14:
      return "min14";
    case min17:
      return "min17";
    case min20:
      return "min20";
    case min24:
      return "min24";
    case min28:
      return "min28";
    case min33:
      return "min33";
    case min38:
      return "min38";
    case min44:
      return "min44";
    case min50:
      return "min50";
    case hr1:
      return "hr1";
    case hr1min30:
      return "hr1min30";
    case hr2:
      return "hr2";
    case hr2min30:
      return "hr2min30";
    case hr3:
      return "hr3";
    case hr3min30:
      return "hr3min30";
    case hr4:
      return "hr4";
    case hr5:
      return "hr5";
    case hr6:
      return "hr6";
    case hr8:
      return "hr8";
    case hr10:
      return "hr10";
    case hr13:
      return "hr13";
    case hr16:
      return "hr16";
    case hr20:
      return "hr20";
    case day1:
      return "day1";
    case day1hr12:
      return "day1hr12";
    case day2:
      return "day2";
    case day2hr12:
      return "day2hr12";
    case day3:
      return "day3";
    case day4:
      return "day4";
    case day5:
      return "day5";
    case day7:
      return "day7";
    case day10:
      return "day10";
    case day14:
      return "day14";
    case day19:
      return "day19";
    case day24:
      return "day24";
    case day30:
      return "day30";
    case day_more_than30:
      return "dayMoreThan30";
    default:
      invalid_enum_value(value, "rrm_cfg_s::ue_inactive_time_e_");
  }
  return "";
}

std::string ho_prep_info_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case ho_prep_info_r8:
      return "handoverPreparationInformation-r8";
    case spare7:
      return "spare7";
    case spare6:
      return "spare6";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ho_prep_info_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string tdd_cfg_sl_r12_s::sf_assign_sl_r12_e_::to_string() const
{
  switch (value) {
    case none:
      return "none";
    case sa0:
      return "sa0";
    case sa1:
      return "sa1";
    case sa2:
      return "sa2";
    case sa3:
      return "sa3";
    case sa4:
      return "sa4";
    case sa5:
      return "sa5";
    case sa6:
      return "sa6";
    default:
      invalid_enum_value(value, "tdd_cfg_sl_r12_s::sf_assign_sl_r12_e_");
  }
  return "";
}

std::string mib_sl_s::sl_bw_r12_e_::to_string() const
{
  switch (value) {
    case n6:
      return "n6";
    case n15:
      return "n15";
    case n25:
      return "n25";
    case n50:
      return "n50";
    case n75:
      return "n75";
    case n100:
      return "n100";
    default:
      invalid_enum_value(value, "mib_sl_s::sl_bw_r12_e_");
  }
  return "";
}
uint8_t mib_sl_s::sl_bw_r12_e_::to_number() const
{
  const static uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return get_enum_number(options, 6, value, "mib_sl_s::sl_bw_r12_e_");
}

std::string mib_sl_v2x_r14_s::sl_bw_r14_e_::to_string() const
{
  switch (value) {
    case n6:
      return "n6";
    case n15:
      return "n15";
    case n25:
      return "n25";
    case n50:
      return "n50";
    case n75:
      return "n75";
    case n100:
      return "n100";
    default:
      invalid_enum_value(value, "mib_sl_v2x_r14_s::sl_bw_r14_e_");
  }
  return "";
}
uint8_t mib_sl_v2x_r14_s::sl_bw_r14_e_::to_number() const
{
  const static uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return get_enum_number(options, 6, value, "mib_sl_v2x_r14_s::sl_bw_r14_e_");
}

std::string scg_cfg_info_r12_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case scg_cfg_info_r12:
      return "scg-ConfigInfo-r12";
    case spare7:
      return "spare7";
    case spare6:
      return "spare6";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "scg_cfg_info_r12_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string sl_precfg_disc_pool_r13_s::disc_period_r13_e_::to_string() const
{
  switch (value) {
    case rf4:
      return "rf4";
    case rf6:
      return "rf6";
    case rf7:
      return "rf7";
    case rf8:
      return "rf8";
    case rf12:
      return "rf12";
    case rf14:
      return "rf14";
    case rf16:
      return "rf16";
    case rf24:
      return "rf24";
    case rf28:
      return "rf28";
    case rf32:
      return "rf32";
    case rf64:
      return "rf64";
    case rf128:
      return "rf128";
    case rf256:
      return "rf256";
    case rf512:
      return "rf512";
    case rf1024:
      return "rf1024";
    case spare:
      return "spare";
    default:
      invalid_enum_value(value, "sl_precfg_disc_pool_r13_s::disc_period_r13_e_");
  }
  return "";
}
uint16_t sl_precfg_disc_pool_r13_s::disc_period_r13_e_::to_number() const
{
  const static uint16_t options[] = {4, 6, 7, 8, 12, 14, 16, 24, 28, 32, 64, 128, 256, 512, 1024};
  return get_enum_number(options, 15, value, "sl_precfg_disc_pool_r13_s::disc_period_r13_e_");
}

std::string sl_precfg_disc_pool_r13_s::tx_params_r13_s_::tx_probability_r13_e_::to_string() const
{
  switch (value) {
    case p25:
      return "p25";
    case p50:
      return "p50";
    case p75:
      return "p75";
    case p100:
      return "p100";
    default:
      invalid_enum_value(value, "sl_precfg_disc_pool_r13_s::tx_params_r13_s_::tx_probability_r13_e_");
  }
  return "";
}
uint8_t sl_precfg_disc_pool_r13_s::tx_params_r13_s_::tx_probability_r13_e_::to_number() const
{
  const static uint8_t options[] = {25, 50, 75, 100};
  return get_enum_number(options, 4, value, "sl_precfg_disc_pool_r13_s::tx_params_r13_s_::tx_probability_r13_e_");
}

std::string sl_precfg_general_r12_s::sl_bw_r12_e_::to_string() const
{
  switch (value) {
    case n6:
      return "n6";
    case n15:
      return "n15";
    case n25:
      return "n25";
    case n50:
      return "n50";
    case n75:
      return "n75";
    case n100:
      return "n100";
    default:
      invalid_enum_value(value, "sl_precfg_general_r12_s::sl_bw_r12_e_");
  }
  return "";
}
uint8_t sl_precfg_general_r12_s::sl_bw_r12_e_::to_number() const
{
  const static uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return get_enum_number(options, 6, value, "sl_precfg_general_r12_s::sl_bw_r12_e_");
}

std::string sl_precfg_sync_r12_s::sync_ref_min_hyst_r12_e_::to_string() const
{
  switch (value) {
    case db0:
      return "dB0";
    case db3:
      return "dB3";
    case db6:
      return "dB6";
    case db9:
      return "dB9";
    case db12:
      return "dB12";
    default:
      invalid_enum_value(value, "sl_precfg_sync_r12_s::sync_ref_min_hyst_r12_e_");
  }
  return "";
}
uint8_t sl_precfg_sync_r12_s::sync_ref_min_hyst_r12_e_::to_number() const
{
  const static uint8_t options[] = {0, 3, 6, 9, 12};
  return get_enum_number(options, 5, value, "sl_precfg_sync_r12_s::sync_ref_min_hyst_r12_e_");
}

std::string sl_precfg_sync_r12_s::sync_ref_diff_hyst_r12_e_::to_string() const
{
  switch (value) {
    case db0:
      return "dB0";
    case db3:
      return "dB3";
    case db6:
      return "dB6";
    case db9:
      return "dB9";
    case db12:
      return "dB12";
    case dbinf:
      return "dBinf";
    default:
      invalid_enum_value(value, "sl_precfg_sync_r12_s::sync_ref_diff_hyst_r12_e_");
  }
  return "";
}
uint8_t sl_precfg_sync_r12_s::sync_ref_diff_hyst_r12_e_::to_number() const
{
  const static uint8_t options[] = {0, 3, 6, 9, 12};
  return get_enum_number(options, 5, value, "sl_precfg_sync_r12_s::sync_ref_diff_hyst_r12_e_");
}

std::string sl_v2x_precfg_comm_pool_r14_s::size_subch_r14_e_::to_string() const
{
  switch (value) {
    case n4:
      return "n4";
    case n5:
      return "n5";
    case n6:
      return "n6";
    case n8:
      return "n8";
    case n9:
      return "n9";
    case n10:
      return "n10";
    case n12:
      return "n12";
    case n15:
      return "n15";
    case n16:
      return "n16";
    case n18:
      return "n18";
    case n20:
      return "n20";
    case n25:
      return "n25";
    case n30:
      return "n30";
    case n48:
      return "n48";
    case n50:
      return "n50";
    case n72:
      return "n72";
    case n75:
      return "n75";
    case n96:
      return "n96";
    case n100:
      return "n100";
    case spare13:
      return "spare13";
    case spare12:
      return "spare12";
    case spare11:
      return "spare11";
    case spare10:
      return "spare10";
    case spare9:
      return "spare9";
    case spare8:
      return "spare8";
    case spare7:
      return "spare7";
    case spare6:
      return "spare6";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "sl_v2x_precfg_comm_pool_r14_s::size_subch_r14_e_");
  }
  return "";
}
uint8_t sl_v2x_precfg_comm_pool_r14_s::size_subch_r14_e_::to_number() const
{
  const static uint8_t options[] = {4, 5, 6, 8, 9, 10, 12, 15, 16, 18, 20, 25, 30, 48, 50, 72, 75, 96, 100};
  return get_enum_number(options, 19, value, "sl_v2x_precfg_comm_pool_r14_s::size_subch_r14_e_");
}

std::string sl_v2x_precfg_comm_pool_r14_s::num_subch_r14_e_::to_string() const
{
  switch (value) {
    case n1:
      return "n1";
    case n3:
      return "n3";
    case n5:
      return "n5";
    case n8:
      return "n8";
    case n10:
      return "n10";
    case n15:
      return "n15";
    case n20:
      return "n20";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "sl_v2x_precfg_comm_pool_r14_s::num_subch_r14_e_");
  }
  return "";
}
uint8_t sl_v2x_precfg_comm_pool_r14_s::num_subch_r14_e_::to_number() const
{
  const static uint8_t options[] = {1, 3, 5, 8, 10, 15, 20};
  return get_enum_number(options, 7, value, "sl_v2x_precfg_comm_pool_r14_s::num_subch_r14_e_");
}

std::string sl_precfg_v2x_sync_r14_s::sync_ref_min_hyst_r14_e_::to_string() const
{
  switch (value) {
    case db0:
      return "dB0";
    case db3:
      return "dB3";
    case db6:
      return "dB6";
    case db9:
      return "dB9";
    case db12:
      return "dB12";
    default:
      invalid_enum_value(value, "sl_precfg_v2x_sync_r14_s::sync_ref_min_hyst_r14_e_");
  }
  return "";
}
uint8_t sl_precfg_v2x_sync_r14_s::sync_ref_min_hyst_r14_e_::to_number() const
{
  const static uint8_t options[] = {0, 3, 6, 9, 12};
  return get_enum_number(options, 5, value, "sl_precfg_v2x_sync_r14_s::sync_ref_min_hyst_r14_e_");
}

std::string sl_precfg_v2x_sync_r14_s::sync_ref_diff_hyst_r14_e_::to_string() const
{
  switch (value) {
    case db0:
      return "dB0";
    case db3:
      return "dB3";
    case db6:
      return "dB6";
    case db9:
      return "dB9";
    case db12:
      return "dB12";
    case dbinf:
      return "dBinf";
    default:
      invalid_enum_value(value, "sl_precfg_v2x_sync_r14_s::sync_ref_diff_hyst_r14_e_");
  }
  return "";
}
uint8_t sl_precfg_v2x_sync_r14_s::sync_ref_diff_hyst_r14_e_::to_number() const
{
  const static uint8_t options[] = {0, 3, 6, 9, 12};
  return get_enum_number(options, 5, value, "sl_precfg_v2x_sync_r14_s::sync_ref_diff_hyst_r14_e_");
}

std::string sl_v2x_precfg_freq_info_r14_s::sync_prio_r14_e_::to_string() const
{
  switch (value) {
    case gnss:
      return "gnss";
    case enb:
      return "enb";
    default:
      invalid_enum_value(value, "sl_v2x_precfg_freq_info_r14_s::sync_prio_r14_e_");
  }
  return "";
}

// SL-V2X-TxProfile-r15 ::= ENUMERATED
std::string sl_v2x_tx_profile_r15_e::to_string() const
{
  switch (value) {
    case rel14:
      return "rel14";
    case rel15:
      return "rel15";
    case spare6:
      return "spare6";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    default:
      invalid_enum_value(value, "sl_v2x_tx_profile_r15_e");
  }
  return "";
}
uint8_t sl_v2x_tx_profile_r15_e::to_number() const
{
  const static uint8_t options[] = {14, 15};
  return get_enum_number(options, 2, value, "sl_v2x_tx_profile_r15_e");
}

std::string ue_paging_coverage_info_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case ue_paging_coverage_info_r13:
      return "uePagingCoverageInformation-r13";
    case spare7:
      return "spare7";
    case spare6:
      return "spare6";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ue_paging_coverage_info_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case ue_radio_access_cap_info_r8:
      return "ueRadioAccessCapabilityInformation-r8";
    case spare7:
      return "spare7";
    case spare6:
      return "spare6";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string ue_radio_paging_info_s::crit_exts_c_::c1_c_::types::to_string() const
{
  switch (value) {
    case ue_radio_paging_info_r12:
      return "ueRadioPagingInformation-r12";
    case spare7:
      return "spare7";
    case spare6:
      return "spare6";
    case spare5:
      return "spare5";
    case spare4:
      return "spare4";
    case spare3:
      return "spare3";
    case spare2:
      return "spare2";
    case spare1:
      return "spare1";
    case nulltype:
      return "NULL";
    default:
      invalid_enum_value(value, "ue_radio_paging_info_s::crit_exts_c_::c1_c_::types");
  }
  return "";
}

std::string var_meas_idle_cfg_r15_s::meas_idle_dur_r15_e_::to_string() const
{
  switch (value) {
    case sec10:
      return "sec10";
    case sec30:
      return "sec30";
    case sec60:
      return "sec60";
    case sec120:
      return "sec120";
    case sec180:
      return "sec180";
    case sec240:
      return "sec240";
    case sec300:
      return "sec300";
    default:
      invalid_enum_value(value, "var_meas_idle_cfg_r15_s::meas_idle_dur_r15_e_");
  }
  return "";
}
uint16_t var_meas_idle_cfg_r15_s::meas_idle_dur_r15_e_::to_number() const
{
  const static uint16_t options[] = {10, 30, 60, 120, 180, 240, 300};
  return get_enum_number(options, 7, value, "var_meas_idle_cfg_r15_s::meas_idle_dur_r15_e_");
}
