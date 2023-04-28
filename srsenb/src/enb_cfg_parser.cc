/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#include "enb_cfg_parser.h"
#include "srsenb/hdr/enb.h"
#include "srsgnb/hdr/stack/rrc/rrc_nr_config_utils.h"
#include "srsran/asn1/rrc_utils.h"
#include "srsran/common/band_helper.h"
#include "srsran/common/multiqueue.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/rrc/rrc_common.h"
#include <boost/algorithm/string.hpp>

#define HANDLEPARSERCODE(cond)                                                                                         \
  do {                                                                                                                 \
    if ((cond) != 0) {                                                                                                 \
      printf("[%d][%s()] Parser Error detected\n", __LINE__, __FUNCTION__);                                            \
      return -1;                                                                                                       \
    }                                                                                                                  \
  } while (0)

#define COND_PARSER_WARN(cond, msg_fmt, ...)                                                                           \
  do {                                                                                                                 \
    if (cond) {                                                                                                        \
      printf(msg_fmt, ##__VA_ARGS__);                                                                                  \
    }                                                                                                                  \
  } while (0)

#define ASSERT_VALID_CFG(cond, msg_fmt, ...)                                                                           \
  do {                                                                                                                 \
    if (not(cond)) {                                                                                                   \
      fprintf(stderr, "Error: Invalid configuration - " msg_fmt "\n", ##__VA_ARGS__);                                  \
      return SRSRAN_ERROR;                                                                                             \
    }                                                                                                                  \
  } while (0)

using namespace asn1::rrc;

namespace srsenb {

template <typename T>
bool contains_value(T value, const std::initializer_list<T>& list)
{
  for (auto& v : list) {
    if (v == value) {
      return true;
    }
  }
  return false;
}

bool sib_is_present(const sched_info_list_l& l, sib_type_e sib_num)
{
  for (uint32_t i = 0; i < l.size(); i++) {
    for (uint32_t j = 0; j < l[i].sib_map_info.size(); j++) {
      if (l[i].sib_map_info[j] == sib_num) {
        return true;
      }
    }
  }
  return false;
}

int field_additional_plmns::parse(libconfig::Setting& root)
{
  if (root.getLength() > ASN1_RRC_MAX_PLMN_MINUS1_R14) {
    ERROR("PLMN-IdentityList cannot have more than %d entries", ASN1_RRC_MAX_PLMN_R11);
    return SRSRAN_ERROR;
  }
  // Reserve the first place to the primary PLMN, see "SystemInformationBlockType1 field descriptions" in TS 36.331
  data->plmn_id_list.resize((uint32_t)root.getLength() + 1);
  for (uint32_t i = 0; i < data->plmn_id_list.size() - 1; i++) {
    std::string mcc_str, mnc_str;
    if (!root[i].lookupValue("mcc", mcc_str)) {
      ERROR("Missing field mcc in additional_plmn=%d\n", i);
      return SRSRAN_ERROR;
    }

    if (!root[i].lookupValue("mnc", mnc_str)) {
      ERROR("Missing field mnc in additional_plmn=%d\n", i);
      return SRSRAN_ERROR;
    }

    srsran::plmn_id_t plmn;
    if (plmn.from_string(mcc_str + mnc_str) == SRSRAN_ERROR) {
      ERROR("Could not convert %s to a plmn_id in additional_plmn=%d", (mcc_str + mnc_str).c_str(), i);
      return SRSRAN_ERROR;
    }
    srsran::to_asn1(&data->plmn_id_list[i + 1].plmn_id, plmn);
    if (not parse_enum_by_str(data->plmn_id_list[i + 1].cell_reserved_for_oper, "cell_reserved_for_oper", root[i])) {
      data->plmn_id_list[i + 1].cell_reserved_for_oper = plmn_id_info_s::cell_reserved_for_oper_e_::not_reserved;
    }
  }
  return 0;
}

int field_sched_info::parse(libconfig::Setting& root)
{
  data->sched_info_list.resize((uint32_t)root.getLength());
  for (uint32_t i = 0; i < data->sched_info_list.size(); i++) {
    if (not parse_enum_by_number(data->sched_info_list[i].si_periodicity, "si_periodicity", root[i])) {
      fprintf(stderr, "Missing field si_periodicity in sched_info=%d\n", i);
      return SRSRAN_ERROR;
    }
    if (root[i].exists("si_mapping_info")) {
      data->sched_info_list[i].sib_map_info.resize((uint32_t)root[i]["si_mapping_info"].getLength());
      if (data->sched_info_list[i].sib_map_info.size() < ASN1_RRC_MAX_SIB) {
        for (uint32_t j = 0; j < data->sched_info_list[i].sib_map_info.size(); j++) {
          uint32_t sib_index = root[i]["si_mapping_info"][j];
          if (sib_index >= 3 && sib_index <= 13) {
            data->sched_info_list[i].sib_map_info[j].value = (sib_type_e::options)(sib_index - 3);
          } else {
            fprintf(stderr, "Invalid SIB index %d for si_mapping_info=%d in sched_info=%d\n", sib_index, j, i);
            return SRSRAN_ERROR;
          }
        }
      } else {
        fprintf(stderr, "Number of si_mapping_info values exceeds maximum (%d)\n", ASN1_RRC_MAX_SIB);
        return SRSRAN_ERROR;
      }
    } else {
      data->sched_info_list[i].sib_map_info.resize(0);
    }
  }
  return 0;
}

int field_intra_neigh_cell_list::parse(libconfig::Setting& root)
{
  data->intra_freq_neigh_cell_list.resize((uint32_t)root.getLength());
  data->intra_freq_neigh_cell_list_present = data->intra_freq_neigh_cell_list.size() > 0;
  for (uint32_t i = 0; i < data->intra_freq_neigh_cell_list.size() && i < ASN1_RRC_MAX_CELL_INTRA; i++) {
    if (not parse_enum_by_number(data->intra_freq_neigh_cell_list[i].q_offset_cell, "q_offset_range", root[i])) {
      fprintf(stderr, "Missing field q_offset_range in neigh_cell=%d\n", i);
      return SRSRAN_ERROR;
    }

    int phys_cell_id = 0;
    if (!root[i].lookupValue("phys_cell_id", phys_cell_id)) {
      fprintf(stderr, "Missing field phys_cell_id in neigh_cell=%d\n", i);
      return SRSRAN_ERROR;
    }
    data->intra_freq_neigh_cell_list[i].pci = (uint16)phys_cell_id;
  }
  return 0;
}

int field_intra_black_cell_list::parse(libconfig::Setting& root)
{
  data->intra_freq_excluded_cell_list.resize((uint32_t)root.getLength());
  data->intra_freq_excluded_cell_list_present = data->intra_freq_excluded_cell_list.size() > 0;
  for (uint32_t i = 0; i < data->intra_freq_excluded_cell_list.size() && i < ASN1_RRC_MAX_EXCLUDED_CELL; i++) {
    if (not parse_enum_by_number(data->intra_freq_excluded_cell_list[i].range, "range", root[i])) {
      fprintf(stderr, "Missing field range in excluded_cell=%d\n", i);
      return SRSRAN_ERROR;
    }
    data->intra_freq_excluded_cell_list[i].range_present = true;

    int start = 0;
    if (!root[i].lookupValue("start", start)) {
      fprintf(stderr, "Missing field start in excluded_cell=%d\n", i);
      return SRSRAN_ERROR;
    }
    data->intra_freq_excluded_cell_list[i].start = (uint16)start;
  }
  return 0;
}

int field_inter_freq_carrier_freq_list::parse(libconfig::Setting& root)
{
  data->inter_freq_carrier_freq_list.resize((uint32_t)root.getLength());
  for (uint32_t i = 0; i < data->inter_freq_carrier_freq_list.size() && i < ASN1_RRC_MAX_FREQ; i++) {
    unsigned int dl_carrier_freq = 0;
    if (!root[i].lookupValue("dl_carrier_freq", dl_carrier_freq)) {
      ERROR("Missing field `dl_carrier_freq` in inter_freq_carrier_freq_list=%d", i);
      return SRSRAN_ERROR;
    }
    data->inter_freq_carrier_freq_list[i].dl_carrier_freq = dl_carrier_freq;

    int q_rx_lev_min = 0;
    if (!root[i].lookupValue("q_rx_lev_min", q_rx_lev_min)) {
      ERROR("Missing field `q_rx_lev_min` in inter_freq_carrier_freq_list=%d", i);
      return SRSRAN_ERROR;
    }
    data->inter_freq_carrier_freq_list[i].q_rx_lev_min = q_rx_lev_min;

    int p_max = 0;
    if (root[i].lookupValue("p_max", p_max)) {
      data->inter_freq_carrier_freq_list[i].p_max_present = true;
      data->inter_freq_carrier_freq_list[i].p_max         = p_max;
    }

    unsigned int t_resel_eutra = 0;
    if (!root[i].lookupValue("t_resel_eutra", t_resel_eutra)) {
      ERROR("Missing field `t_resel_eutra` in inter_freq_carrier_freq_list=%d", i);
      return SRSRAN_ERROR;
    }
    data->inter_freq_carrier_freq_list[i].t_resel_eutra = t_resel_eutra;

    if (root[i].exists("t_resel_eutra_sf")) {
      data->inter_freq_carrier_freq_list[i].t_resel_eutra_sf_present = true;

      field_asn1_enum_number_str<asn1::rrc::speed_state_scale_factors_s::sf_medium_e_> sf_medium(
          "sf_medium", &data->inter_freq_carrier_freq_list[i].t_resel_eutra_sf.sf_medium);
      if (sf_medium.parse(root[i]["t_resel_eutra_sf"])) {
        ERROR("Error parsing `sf_medium` in inter_freq_carrier_freq_list=%d t_resel_eutra_sf", i);
        return SRSRAN_ERROR;
      }

      field_asn1_enum_number_str<asn1::rrc::speed_state_scale_factors_s::sf_high_e_> sf_high(
          "sf_high", &data->inter_freq_carrier_freq_list[i].t_resel_eutra_sf.sf_high);
      if (sf_high.parse(root[i]["t_resel_eutra_sf"])) {
        ERROR("Error parsing `sf_high` in inter_freq_carrier_freq_list=%d t_resel_eutra_sf", i);
        return SRSRAN_ERROR;
      }
    }

    unsigned int thresh_x_high = 0;
    if (!root[i].lookupValue("thresh_x_high", thresh_x_high)) {
      ERROR("Missing field `thresh_x_high` in inter_freq_carrier_freq_list=%d", i);
      return SRSRAN_ERROR;
    }
    data->inter_freq_carrier_freq_list[i].thresh_x_high = thresh_x_high;

    unsigned int thresh_x_low = 0;
    if (!root[i].lookupValue("thresh_x_low", thresh_x_low)) {
      ERROR("Missing field `thresh_x_low` in inter_freq_carrier_freq_list=%d", i);
      return SRSRAN_ERROR;
    }
    data->inter_freq_carrier_freq_list[i].thresh_x_low = thresh_x_low;

    field_asn1_enum_number<asn1::rrc::allowed_meas_bw_e> allowed_meas_bw(
        "allowed_meas_bw", &data->inter_freq_carrier_freq_list[i].allowed_meas_bw);
    if (allowed_meas_bw.parse(root[i])) {
      ERROR("Error parsing `allowed_meas_bw` in inter_freq_carrier_freq_list=%d", i);
      return SRSRAN_ERROR;
    }

    bool presence_ant_port1 = 0;
    if (!root[i].lookupValue("presence_ant_port_1", presence_ant_port1)) {
      ERROR("Missing field `presence_ant_port_1` in inter_freq_carrier_freq_list=%d", i);
      return SRSRAN_ERROR;
    }
    data->inter_freq_carrier_freq_list[i].presence_ant_port1 = presence_ant_port1;

    unsigned int cell_resel_prio = 0;
    if (root[i].lookupValue("cell_resel_prio", cell_resel_prio)) {
      data->inter_freq_carrier_freq_list[i].cell_resel_prio_present = true;
      data->inter_freq_carrier_freq_list[i].cell_resel_prio         = cell_resel_prio;
    }

    field_asn1_enum_number<asn1::rrc::q_offset_range_e> q_offset_freq(
        "q_offset_freq", &data->inter_freq_carrier_freq_list[i].q_offset_freq);
    if (!q_offset_freq.parse(root[i])) {
      data->inter_freq_carrier_freq_list[i].q_offset_freq_present = true;
    }

    field_asn1_bitstring_number<asn1::fixed_bitstring<2>, uint8_t> neigh_cell_cfg(
        "neigh_cell_cfg", &data->inter_freq_carrier_freq_list[i].neigh_cell_cfg);
    if (neigh_cell_cfg.parse(root[i])) {
      ERROR("Error parsing `neigh_cell_cfg` in inter_freq_carrier_freq_list=%d", i);
      return SRSRAN_ERROR;
    }

    if (root[i].exists("inter_freq_neigh_cell_list")) {
      auto inter_neigh_cell_list_parser = new field_inter_freq_neigh_cell_list(&data->inter_freq_carrier_freq_list[i]);
      HANDLEPARSERCODE(inter_neigh_cell_list_parser->parse(root[i]["inter_freq_neigh_cell_list"]));
    }

    if (root[i].exists("inter_freq_black_cell_list")) {
      auto inter_black_cell_list_parser = new field_inter_freq_black_cell_list(&data->inter_freq_carrier_freq_list[i]);
      HANDLEPARSERCODE(inter_black_cell_list_parser->parse(root[i]["inter_freq_black_cell_list"]));
    }
  }
  return 0;
}

int field_inter_freq_neigh_cell_list::parse(libconfig::Setting& root)
{
  data->inter_freq_neigh_cell_list.resize((uint32_t)root.getLength());
  data->inter_freq_neigh_cell_list_present = data->inter_freq_neigh_cell_list.size() > 0;
  for (uint32_t i = 0; i < data->inter_freq_neigh_cell_list.size() && i < ASN1_RRC_MAX_EXCLUDED_CELL; i++) {
    if (not parse_enum_by_number(data->inter_freq_neigh_cell_list[i].q_offset_cell, "q_offset_cell", root[i])) {
      ERROR("Missing field q_offset_cell in neigh_cell=%d\n", i);
      return SRSRAN_ERROR;
    }

    unsigned int phys_cell_id = 0;
    if (!root[i].lookupValue("phys_cell_id", phys_cell_id)) {
      ERROR("Missing field phys_cell_id in neigh_cell=%d\n", i);
      return SRSRAN_ERROR;
    }
    data->inter_freq_neigh_cell_list[i].pci = (uint16)phys_cell_id;
  }
  return 0;
}

int field_inter_freq_black_cell_list::parse(libconfig::Setting& root)
{
  data->inter_freq_excluded_cell_list.resize((uint32_t)root.getLength());
  data->inter_freq_excluded_cell_list_present = data->inter_freq_excluded_cell_list.size() > 0;
  for (uint32_t i = 0; i < data->inter_freq_excluded_cell_list.size() && i < ASN1_RRC_MAX_EXCLUDED_CELL; i++) {
    if (not parse_enum_by_number(data->inter_freq_excluded_cell_list[i].range, "range", root[i])) {
      ERROR("Missing field range in excluded_cell=%d\n", i);
      return SRSRAN_ERROR;
    }
    data->inter_freq_excluded_cell_list[i].range_present = true;

    unsigned int start = 0;
    if (!root[i].lookupValue("start", start)) {
      ERROR("Missing field start in excluded_cell=%d\n", i);
      return SRSRAN_ERROR;
    }
    data->inter_freq_excluded_cell_list[i].start = (uint16)start;
  }
  return 0;
}

int field_carrier_freq_list_utra_fdd::parse(libconfig::Setting& root)
{
  data->carrier_freq_list_utra_fdd.resize((uint32_t)root.getLength());
  data->carrier_freq_list_utra_fdd_present = data->carrier_freq_list_utra_fdd.size() > 0;
  for (uint32_t i = 0; i < data->carrier_freq_list_utra_fdd.size() && i < ASN1_RRC_MAX_UTRA_FDD_CARRIER; i++) {
    unsigned int carrier_freq = 0;
    if (!root[i].lookupValue("carrier_freq", carrier_freq)) {
      fprintf(stderr, "Missing field `carrier_freq` in carrier_freq_list_utra_fdd=%d\n", i);
      return SRSRAN_ERROR;
    }
    data->carrier_freq_list_utra_fdd[i].carrier_freq = carrier_freq;

    unsigned int cell_resel_prio = 0;
    if (root[i].lookupValue("cell_resel_prio", cell_resel_prio)) {
      data->carrier_freq_list_utra_fdd[i].cell_resel_prio_present = true;
      data->carrier_freq_list_utra_fdd[i].cell_resel_prio         = cell_resel_prio;
    }

    unsigned int thresh_x_high = 0;
    if (!root[i].lookupValue("thresh_x_high", thresh_x_high)) {
      ERROR("Missing field `thresh_x_high` in carrier_freq_list_utra_fdd=%d", i);
      return SRSRAN_ERROR;
    }
    data->carrier_freq_list_utra_fdd[i].thresh_x_high = thresh_x_high;

    unsigned int thresh_x_low = 0;
    if (!root[i].lookupValue("thresh_x_low", thresh_x_low)) {
      ERROR("Missing field `thresh_x_low` in carrier_freq_list_utra_fdd=%d", i);
      return SRSRAN_ERROR;
    }
    data->carrier_freq_list_utra_fdd[i].thresh_x_low = thresh_x_low;

    int q_rx_lev_min = 0;
    if (!root[i].lookupValue("q_rx_lev_min", q_rx_lev_min)) {
      ERROR("Missing field `q_rx_lev_min` in carrier_freq_list_utra_fdd=%d", i);
      return SRSRAN_ERROR;
    }
    data->carrier_freq_list_utra_fdd[i].q_rx_lev_min = q_rx_lev_min;

    int p_max_utra = 0;
    if (!root[i].lookupValue("p_max_utra", p_max_utra)) {
      ERROR("Missing field `p_max_utra` in carrier_freq_list_utra_fdd=%d", i);
      return SRSRAN_ERROR;
    }
    data->carrier_freq_list_utra_fdd[i].p_max_utra = p_max_utra;

    int q_qual_min = 0;
    if (!root[i].lookupValue("q_qual_min", q_qual_min)) {
      ERROR("Missing field `q_qual_min` in carrier_freq_list_utra_fdd=%d", i);
      return SRSRAN_ERROR;
    }
    data->carrier_freq_list_utra_fdd[i].q_qual_min = q_qual_min;
  }
  return 0;
}

int field_carrier_freq_list_utra_tdd::parse(libconfig::Setting& root)
{
  data->carrier_freq_list_utra_tdd.resize((uint32_t)root.getLength());
  data->carrier_freq_list_utra_tdd_present = data->carrier_freq_list_utra_tdd.size() > 0;
  for (uint32_t i = 0; i < data->carrier_freq_list_utra_tdd.size() && i < ASN1_RRC_MAX_UTRA_TDD_CARRIER; i++) {
    unsigned int carrier_freq = 0;
    if (!root[i].lookupValue("carrier_freq", carrier_freq)) {
      fprintf(stderr, "Missing field `carrier_freq` in carrier_freq_list_utra_tdd=%d\n", i);
      return SRSRAN_ERROR;
    }
    data->carrier_freq_list_utra_tdd[i].carrier_freq = carrier_freq;

    unsigned int cell_resel_prio = 0;
    if (root[i].lookupValue("cell_resel_prio", cell_resel_prio)) {
      data->carrier_freq_list_utra_tdd[i].cell_resel_prio_present = true;
      data->carrier_freq_list_utra_tdd[i].cell_resel_prio         = cell_resel_prio;
    }

    unsigned int thresh_x_high = 0;
    if (!root[i].lookupValue("thresh_x_high", thresh_x_high)) {
      ERROR("Missing field `thresh_x_high` in carrier_freq_list_utra_tdd=%d", i);
      return SRSRAN_ERROR;
    }
    data->carrier_freq_list_utra_tdd[i].thresh_x_high = thresh_x_high;

    unsigned int thresh_x_low = 0;
    if (!root[i].lookupValue("thresh_x_low", thresh_x_low)) {
      ERROR("Missing field `thresh_x_low` in carrier_freq_list_utra_tdd=%d", i);
      return SRSRAN_ERROR;
    }
    data->carrier_freq_list_utra_tdd[i].thresh_x_low = thresh_x_low;

    int q_rx_lev_min = 0;
    if (!root[i].lookupValue("q_rx_lev_min", q_rx_lev_min)) {
      ERROR("Missing field `q_rx_lev_min` in carrier_freq_list_utra_tdd=%d", i);
      return SRSRAN_ERROR;
    }
    data->carrier_freq_list_utra_tdd[i].q_rx_lev_min = q_rx_lev_min;

    int p_max_utra = 0;
    if (!root[i].lookupValue("p_max_utra", p_max_utra)) {
      ERROR("Missing field `p_max_utra` in carrier_freq_list_utra_tdd=%d", i);
      return SRSRAN_ERROR;
    }
    data->carrier_freq_list_utra_tdd[i].p_max_utra = p_max_utra;
  }
  return 0;
}

int field_carrier_freqs_info_list::parse(libconfig::Setting& root)
{
  data->carrier_freqs_info_list.resize((uint32_t)root.getLength());
  data->carrier_freqs_info_list_present = data->carrier_freqs_info_list.size() > 0;
  if (data->carrier_freqs_info_list.size() > ASN1_RRC_MAX_GNFG) {
    ERROR("CarrierFreqsInfoGERAN cannot have more than %d entries", ASN1_RRC_MAX_GNFG);
    return SRSRAN_ERROR;
  }
  for (uint32_t i = 0; i < data->carrier_freqs_info_list.size(); i++) {
    int cell_resel_prio;
    if (root[i].lookupValue("cell_resel_prio", cell_resel_prio)) {
      data->carrier_freqs_info_list[i].common_info.cell_resel_prio_present = true;
      data->carrier_freqs_info_list[i].common_info.cell_resel_prio         = cell_resel_prio;
    }

    int p_max_geran;
    if (root[i].lookupValue("p_max_geran", p_max_geran)) {
      data->carrier_freqs_info_list[i].common_info.p_max_geran_present = true;
      data->carrier_freqs_info_list[i].common_info.p_max_geran         = p_max_geran;
    }

    field_asn1_bitstring_number<asn1::fixed_bitstring<8>, uint8_t> ncc_permitted(
        "ncc_permitted", &data->carrier_freqs_info_list[i].common_info.ncc_permitted);
    if (ncc_permitted.parse(root[i])) {
      ERROR("Error parsing `ncc_permitted` in carrier_freqs_info_list=%d", i);
      return SRSRAN_ERROR;
    }

    int q_rx_lev_min = 0;
    if (!root[i].lookupValue("q_rx_lev_min", q_rx_lev_min)) {
      ERROR("Missing field `q_rx_lev_min` in carrier_freqs_info_list=%d", i);
      return SRSRAN_ERROR;
    }
    data->carrier_freqs_info_list[i].common_info.q_rx_lev_min = q_rx_lev_min;

    int thresh_x_high = 0;
    if (!root[i].lookupValue("thresh_x_high", thresh_x_high)) {
      ERROR("Missing field `thresh_x_high` in carrier_freqs_info_list=%d", i);
      return SRSRAN_ERROR;
    }
    data->carrier_freqs_info_list[i].common_info.thresh_x_high = thresh_x_high;

    int thresh_x_low = 0;
    if (!root[i].lookupValue("thresh_x_low", thresh_x_low)) {
      ERROR("Missing field `thresh_x_low` in carrier_freqs_info_list=%d", i);
      return SRSRAN_ERROR;
    }
    data->carrier_freqs_info_list[i].common_info.thresh_x_low = thresh_x_low;

    int start_arfcn = 0;
    if (root[i].lookupValue("start_arfcn", start_arfcn)) {
      data->carrier_freqs_info_list[i].carrier_freqs.start_arfcn = start_arfcn;
    }

    field_asn1_enum_str<asn1::rrc::band_ind_geran_e> band_ind("band_ind",
                                                              &data->carrier_freqs_info_list[i].carrier_freqs.band_ind);
    if (band_ind.parse(root[i])) {
      ERROR("Error parsing `band_ind` in carrier_freqs_info_list=%d", i);
      return SRSRAN_ERROR;
    }

    data->carrier_freqs_info_list[i].carrier_freqs.following_arfcns.set_explicit_list_of_arfcns();

    explicit_list_of_arfcns_l& exp_l =
        data->carrier_freqs_info_list[i].carrier_freqs.following_arfcns.explicit_list_of_arfcns();
    if (root[i].exists("explicit_list_of_arfcns")) {
      exp_l.resize((uint32_t)root[i]["explicit_list_of_arfcns"].getLength());
      if (exp_l.size() < 31) { /* SEQUENCE (SIZE (0..31)) OF ARFCN-ValueGERAN */
        for (uint32_t j = 0; j < exp_l.size(); j++) {
          int arfcn = root[i]["explicit_list_of_arfcns"][j];
          if (arfcn >= 0 && arfcn <= 1024) {
            exp_l[j] = (short unsigned int)arfcn;
          } else {
            fprintf(stderr, "Invalid ARFCN %d in for carrier_freqs_info_list=%d explicit_list_of_arfcns\n", i, j);
            return SRSRAN_ERROR;
          }
        }
      } else {
        fprintf(stderr, "Number of ARFCN in explicit_list_of_arfcns exceeds maximum (%d)\n", 31);
        return SRSRAN_ERROR;
      }
    } else {
      exp_l.resize(0);
    }
  }
  return 0;
}

bool extract_sf_alloc(mbsfn_sf_cfg_s::sf_alloc_c_* store_ptr, const char* name, Setting& root)
{
  uint32_t alloc;
  if (root.lookupValue(name, alloc)) {
    switch (store_ptr->type()) {
      case mbsfn_sf_cfg_s::sf_alloc_c_::types::one_frame:
        store_ptr->one_frame().from_number(alloc);
        break;
      case mbsfn_sf_cfg_s::sf_alloc_c_::types::four_frames:
        store_ptr->four_frames().from_number(alloc);
        break;
      default:
        return false;
    }
    return true;
  }
  return false;
}

int mbsfn_sf_cfg_list_parser::parse(Setting& root)
{
  if (not root.exists("mbsfnSubframeConfigList")) {
    *enabled = false;
    mbsfn_list->resize(0);
    return 0;
  }
  int len = 1;
  if (root.exists("mbsfnSubframeConfigListLength")) {
    root.lookupValue("mbsfnSubframeConfigListLength", len);
  }
  if (len <= 0) {
    *enabled = false;
    mbsfn_list->resize(0);
    return 0;
  }
  if (len > 1) {
    fprintf(stderr, "Only mbsfnSubframeConfigListLengths of size 1 are supported\n");
    return SRSRAN_ERROR;
  }
  *enabled = true;
  mbsfn_list->resize(len);

  field_asn1_choice_number<mbsfn_sf_cfg_s::sf_alloc_c_> c(
      "subframeAllocation", "subframeAllocationNumFrames", &extract_sf_alloc, &(*mbsfn_list)[0].sf_alloc);
  HANDLEPARSERCODE(c.parse(root["mbsfnSubframeConfigList"]));

  parser::field<uint8_t> f("radioframeAllocationOffset", &(*mbsfn_list)[0].radioframe_alloc_offset);
  f.parse(root["mbsfnSubframeConfigList"]);

  (*mbsfn_list)[0].radioframe_alloc_period.value = mbsfn_sf_cfg_s::radioframe_alloc_period_opts::n1;
  field_asn1_enum_number<mbsfn_sf_cfg_s::radioframe_alloc_period_e_> e("radioframeAllocationPeriod",
                                                                       &(*mbsfn_list)[0].radioframe_alloc_period);
  HANDLEPARSERCODE(e.parse(root["mbsfnSubframeConfigList"]));

  // TODO: Did you forget subframeAllocationNumFrames?

  return 0;
}

int mbsfn_area_info_list_parser::parse(Setting& root)
{
  if (not root.exists("mbsfn_area_info_list")) {
    if (enabled) {
      *enabled = false;
    }
    mbsfn_list->resize(0);
    return 0;
  }

  mbsfn_list->resize(1);
  if (enabled) {
    *enabled = true;
  }
  mbsfn_area_info_r9_s* mbsfn_item = &(*mbsfn_list)[0];

  field_asn1_enum_str<mbsfn_area_info_r9_s::non_mbsfn_region_len_e_> fieldlen("non_mbsfn_region_length",
                                                                              &mbsfn_item->non_mbsfn_region_len);
  if (fieldlen.parse(root["mbsfn_area_info_list"])) {
    fprintf(stderr, "Error parsing non_mbsfn_region_length\n");
    return SRSRAN_ERROR;
  }

  field_asn1_enum_str<mbsfn_area_info_r9_s::mcch_cfg_r9_s_::mcch_repeat_period_r9_e_> repeat(
      "mcch_repetition_period", &mbsfn_item->mcch_cfg_r9.mcch_repeat_period_r9);
  if (repeat.parse(root["mbsfn_area_info_list"])) {
    fprintf(stderr, "Error parsing mcch_repetition_period\n");
    return SRSRAN_ERROR;
  }

  field_asn1_enum_str<mbsfn_area_info_r9_s::mcch_cfg_r9_s_::mcch_mod_period_r9_e_> mod(
      "mcch_modification_period", &mbsfn_item->mcch_cfg_r9.mcch_mod_period_r9);
  if (mod.parse(root["mbsfn_area_info_list"])) {
    fprintf(stderr, "Error parsing mcch_modification_period\n");
    return SRSRAN_ERROR;
  }

  field_asn1_enum_str<mbsfn_area_info_r9_s::mcch_cfg_r9_s_::sig_mcs_r9_e_> sig("signalling_mcs",
                                                                               &mbsfn_item->mcch_cfg_r9.sig_mcs_r9);
  if (sig.parse(root["mbsfn_area_info_list"])) {
    fprintf(stderr, "Error parsing signalling_mcs\n");
    return SRSRAN_ERROR;
  }

  parser::field<uint16_t> areaid("mbsfn_area_id", &mbsfn_item->mbsfn_area_id_r9);
  if (areaid.parse(root["mbsfn_area_info_list"])) {
    fprintf(stderr, "Error parsing mbsfn_area_id\n");
    return SRSRAN_ERROR;
  }

  parser::field<uint8_t> notif_ind("notification_indicator", &mbsfn_item->notif_ind_r9);
  if (notif_ind.parse(root["mbsfn_area_info_list"])) {
    fprintf(stderr, "Error parsing notification_indicator\n");
    return SRSRAN_ERROR;
  }

  parser::field<uint8_t> offset("mcch_offset", &mbsfn_item->mcch_cfg_r9.mcch_offset_r9);
  if (offset.parse(root["mbsfn_area_info_list"])) {
    fprintf(stderr, "Error parsing mcch_offset\n");
    return SRSRAN_ERROR;
  }

  field_asn1_bitstring_number<asn1::fixed_bitstring<6>, uint8_t> alloc_info("sf_alloc_info",
                                                                            &mbsfn_item->mcch_cfg_r9.sf_alloc_info_r9);
  if (alloc_info.parse(root["mbsfn_area_info_list"])) {
    fprintf(stderr, "Error parsing mbsfn_area_info_list\n");
    return SRSRAN_ERROR;
  }

  return 0;
}

int field_sf_mapping::parse(libconfig::Setting& root)
{
  if (root.exists("subframe")) {
    *nof_subframes = root["subframe"].getLength();
    for (uint32_t i = 0; i < *nof_subframes; i++) {
      sf_mapping[i] = root["subframe"][i];
    }
  } else {
    // Note: By default we evenly distribute PUCCH resources between SR/CQI.
    // The default SR resources will be {0, 2, 4, ...}, while the CQI will be {1, 3, 5, ...}.
    *nof_subframes = static_cast<uint32_t>(root["period"]) / 2;
    for (uint32_t i = 0; i < *nof_subframes; i++) {
      sf_mapping[i] = i * 2 + default_offset;
    }
  }
  return 0;
}

int phr_cnfg_parser::parse(libconfig::Setting& root)
{
  if (not root.exists("phr_cnfg")) {
    phr_cfg->set(mac_main_cfg_s::phr_cfg_c_::types::release);
    return 0;
  }
  phr_cfg->set_setup();
  mac_main_cfg_s::phr_cfg_c_::setup_s_& s = phr_cfg->setup();

  if (not parse_enum_by_str(s.dl_pathloss_change, "dl_pathloss_change", root["phr_cnfg"])) {
    return SRSRAN_ERROR;
  }
  if (not parse_enum_by_number(s.periodic_phr_timer, "periodic_phr_timer", root["phr_cnfg"])) {
    return SRSRAN_ERROR;
  }
  if (not parse_enum_by_number(s.prohibit_phr_timer, "prohibit_phr_timer", root["phr_cnfg"])) {
    return SRSRAN_ERROR;
  }
  return 0;
}

int field_srb::parse(libconfig::Setting& root)
{
  // Parse RLC AM section
  rlc_cfg_c* rlc_cfg = &cfg.rlc_cfg.set_explicit_value();
  if (root.exists("ul_am") && root.exists("dl_am")) {
    rlc_cfg->set_am();
  }

  // RLC-UM Should not exist section
  if (root.exists("ul_um") || root.exists("dl_um")) {
    ERROR("Error SRBs must be AM.");
    return SRSRAN_ERROR;
  }

  // Parse RLC-AM section
  if (root.exists("ul_am")) {
    ul_am_rlc_s* am_rlc = &rlc_cfg->am().ul_am_rlc;

    field_asn1_enum_number<t_poll_retx_e> t_poll_retx("t_poll_retx", &am_rlc->t_poll_retx);
    if (t_poll_retx.parse(root["ul_am"])) {
      ERROR("Error can't find t_poll_retx in section ul_am");
      return SRSRAN_ERROR;
    }

    field_asn1_enum_number<poll_pdu_e> poll_pdu("poll_pdu", &am_rlc->poll_pdu);
    if (poll_pdu.parse(root["ul_am"])) {
      ERROR("Error can't find poll_pdu in section ul_am");
      return SRSRAN_ERROR;
    }

    field_asn1_enum_number<poll_byte_e> poll_byte("poll_byte", &am_rlc->poll_byte);
    if (poll_byte.parse(root["ul_am"])) {
      ERROR("Error can't find poll_byte in section ul_am");
      return SRSRAN_ERROR;
    }

    field_asn1_enum_number<ul_am_rlc_s::max_retx_thres_e_> max_retx_thresh("max_retx_thresh", &am_rlc->max_retx_thres);
    if (max_retx_thresh.parse(root["ul_am"])) {
      ERROR("Error can't find max_retx_thresh in section ul_am");
      return SRSRAN_ERROR;
    }
  }

  if (root.exists("dl_am")) {
    dl_am_rlc_s* am_rlc = &rlc_cfg->am().dl_am_rlc;

    field_asn1_enum_number<t_reordering_e> t_reordering("t_reordering", &am_rlc->t_reordering);
    if (t_reordering.parse(root["dl_am"])) {
      ERROR("Error can't find t_reordering in section dl_am");
      return SRSRAN_ERROR;
    }

    field_asn1_enum_number<t_status_prohibit_e> t_status_prohibit("t_status_prohibit", &am_rlc->t_status_prohibit);
    if (t_status_prohibit.parse(root["dl_am"])) {
      ERROR("Error can't find t_status_prohibit in section dl_am");
      return SRSRAN_ERROR;
    }
  }

  if (root.exists("enb_specific")) {
    cfg.enb_dl_max_retx_thres = (int)root["enb_specific"]["dl_max_retx_thresh"];
  }

  return 0;
}

int field_qci::parse(libconfig::Setting& root)
{
  auto nof_qci = (uint32_t)root.getLength();

  for (uint32_t i = 0; i < nof_qci; i++) {
    libconfig::Setting& q = root[i];

    uint32_t qci = q["qci"];

    // Parse PDCP section
    if (!q.exists("pdcp_config")) {
      fprintf(stderr, "Error section pdcp_config not found for qci=%d\n", qci);
      return SRSRAN_ERROR;
    }

    rrc_cfg_qci_t qcicfg;

    field_asn1_enum_number<pdcp_cfg_s::discard_timer_e_> discard_timer(
        "discard_timer", &qcicfg.pdcp_cfg.discard_timer, &qcicfg.pdcp_cfg.discard_timer_present);
    HANDLEPARSERCODE(discard_timer.parse(q["pdcp_config"]));

    field_asn1_enum_number<pdcp_cfg_s::rlc_um_s_::pdcp_sn_size_e_> pdcp_sn_size(
        "pdcp_sn_size", &qcicfg.pdcp_cfg.rlc_um.pdcp_sn_size, &qcicfg.pdcp_cfg.rlc_um_present);
    HANDLEPARSERCODE(pdcp_sn_size.parse(q["pdcp_config"]));

    qcicfg.pdcp_cfg.rlc_am_present =
        q["pdcp_config"].lookupValue("status_report_required", qcicfg.pdcp_cfg.rlc_am.status_report_required);
    qcicfg.pdcp_cfg.hdr_compress.set(pdcp_cfg_s::hdr_compress_c_::types::not_used);

    // Parse RLC section
    rlc_cfg_c* rlc_cfg = &qcicfg.rlc_cfg;
    if (q["rlc_config"].exists("ul_am")) {
      rlc_cfg->set_am();
    } else if (q["rlc_config"].exists("ul_um") && q["rlc_config"].exists("dl_um")) {
      rlc_cfg->set_um_bi_dir();
    } else if (q["rlc_config"].exists("ul_um") && !q["rlc_config"].exists("dl_um")) {
      rlc_cfg->set_um_uni_dir_ul();
    } else if (!q["rlc_config"].exists("ul_um") && q["rlc_config"].exists("dl_um")) {
      rlc_cfg->set_um_uni_dir_dl();
    } else {
      fprintf(stderr, "Invalid combination of UL/DL UM/AM for qci=%d\n", qci);
      return SRSRAN_ERROR;
    }

    // Parse RLC-UM section
    if (q["rlc_config"].exists("ul_um")) {
      ul_um_rlc_s* um_rlc;
      if (rlc_cfg->type() == rlc_cfg_c::types::um_uni_dir_ul) {
        um_rlc = &rlc_cfg->um_uni_dir_ul().ul_um_rlc;
      } else {
        um_rlc = &rlc_cfg->um_bi_dir().ul_um_rlc;
      }

      field_asn1_enum_number<sn_field_len_e> sn_field_len("sn_field_length", &um_rlc->sn_field_len);
      if (sn_field_len.parse(q["rlc_config"]["ul_um"])) {
        ERROR("Error can't find sn_field_length in section ul_um");
        return SRSRAN_ERROR;
      }
    }

    if (q["rlc_config"].exists("dl_um")) {
      dl_um_rlc_s* um_rlc;
      if (rlc_cfg->type() == rlc_cfg_c::types::um_uni_dir_dl) {
        um_rlc = &rlc_cfg->um_uni_dir_dl().dl_um_rlc;
      } else {
        um_rlc = &rlc_cfg->um_bi_dir().dl_um_rlc;
      }

      field_asn1_enum_number<sn_field_len_e> sn_field_len("sn_field_length", &um_rlc->sn_field_len);
      if (sn_field_len.parse(q["rlc_config"]["dl_um"])) {
        ERROR("Error can't find sn_field_length in section dl_um");
        return SRSRAN_ERROR;
      }

      field_asn1_enum_number<t_reordering_e> t_reordering("t_reordering", &um_rlc->t_reordering);
      if (t_reordering.parse(q["rlc_config"]["dl_um"])) {
        ERROR("Error can't find t_reordering in section dl_um");
        return SRSRAN_ERROR;
      }
    }

    // Parse RLC-AM section
    if (q["rlc_config"].exists("ul_am")) {
      ul_am_rlc_s* am_rlc = &rlc_cfg->am().ul_am_rlc;

      field_asn1_enum_number<t_poll_retx_e> t_poll_retx("t_poll_retx", &am_rlc->t_poll_retx);
      if (t_poll_retx.parse(q["rlc_config"]["ul_am"])) {
        ERROR("Error can't find t_poll_retx in section ul_am");
        return SRSRAN_ERROR;
      }

      field_asn1_enum_number<poll_pdu_e> poll_pdu("poll_pdu", &am_rlc->poll_pdu);
      if (poll_pdu.parse(q["rlc_config"]["ul_am"])) {
        ERROR("Error can't find poll_pdu in section ul_am");
        return SRSRAN_ERROR;
      }

      field_asn1_enum_number<poll_byte_e> poll_byte("poll_byte", &am_rlc->poll_byte);
      if (poll_byte.parse(q["rlc_config"]["ul_am"])) {
        ERROR("Error can't find poll_byte in section ul_am");
        return SRSRAN_ERROR;
      }

      field_asn1_enum_number<ul_am_rlc_s::max_retx_thres_e_> max_retx_thresh("max_retx_thresh",
                                                                             &am_rlc->max_retx_thres);
      if (max_retx_thresh.parse(q["rlc_config"]["ul_am"])) {
        ERROR("Error can't find max_retx_thresh in section ul_am");
        return SRSRAN_ERROR;
      }
    }

    if (q["rlc_config"].exists("dl_am")) {
      dl_am_rlc_s* am_rlc = &rlc_cfg->am().dl_am_rlc;

      field_asn1_enum_number<t_reordering_e> t_reordering("t_reordering", &am_rlc->t_reordering);
      if (t_reordering.parse(q["rlc_config"]["dl_am"])) {
        ERROR("Error can't find t_reordering in section dl_am");
        return SRSRAN_ERROR;
      }

      field_asn1_enum_number<t_status_prohibit_e> t_status_prohibit("t_status_prohibit", &am_rlc->t_status_prohibit);
      if (t_status_prohibit.parse(q["rlc_config"]["dl_am"])) {
        ERROR("Error can't find t_status_prohibit in section dl_am");
        return SRSRAN_ERROR;
      }
    }

    // Parse logical channel configuration section
    if (!q.exists("logical_channel_config")) {
      fprintf(stderr, "Error section logical_channel_config not found for qci=%d\n", qci);
      return SRSRAN_ERROR;
    }

    lc_ch_cfg_s::ul_specific_params_s_* lc_cfg = &qcicfg.lc_cfg;

    parser::field<uint8> priority("priority", &lc_cfg->prio);
    if (priority.parse(q["logical_channel_config"])) {
      ERROR("Error can't find logical_channel_config in section priority");
      return SRSRAN_ERROR;
    }

    field_asn1_enum_number<lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_e_> prioritised_bit_rate(
        "prioritized_bit_rate", &lc_cfg->prioritised_bit_rate);
    if (prioritised_bit_rate.parse(q["logical_channel_config"])) {
      fprintf(stderr, "Error can't find prioritized_bit_rate in section logical_channel_config\n");
      return SRSRAN_ERROR;
    }

    field_asn1_enum_number<lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_e_> bucket_size_duration(
        "bucket_size_duration", &lc_cfg->bucket_size_dur);
    if (bucket_size_duration.parse(q["logical_channel_config"])) {
      ERROR("Error can't find bucket_size_duration in section logical_channel_config");
      return SRSRAN_ERROR;
    }

    parser::field<uint8> log_chan_group("log_chan_group", &lc_cfg->lc_ch_group);
    lc_cfg->lc_ch_group_present = not log_chan_group.parse(q["logical_channel_config"]);
    qcicfg.configured           = true;

    if (q.exists("enb_specific")) {
      qcicfg.enb_dl_max_retx_thres = (int)q["enb_specific"]["dl_max_retx_thresh"];
    }

    cfg.insert(std::make_pair(qci, qcicfg));
  }

  return 0;
}

int field_5g_srb::parse(libconfig::Setting& root)
{
  // Parse RLC AM section
  asn1::rrc_nr::rlc_cfg_c* rlc_cfg = &cfg.rlc_cfg;
  if (root.exists("ul_am") && root.exists("dl_am")) {
    rlc_cfg->set_am();
    cfg.present = true;
  }

  // RLC-UM must not exist in this section
  if (root.exists("ul_um") || root.exists("dl_um")) {
    ERROR("Error SRBs must be AM.");
    return SRSRAN_ERROR;
  }

  // Parse RLC-AM section
  if (root.exists("ul_am")) {
    asn1::rrc_nr::ul_am_rlc_s& ul_am_rlc = rlc_cfg->am().ul_am_rlc;

    // SN length
    field_asn1_enum_number<asn1::rrc_nr::sn_field_len_am_e> rlc_sn_size_ul("sn_field_len", &ul_am_rlc.sn_field_len);
    if (rlc_sn_size_ul.parse(root["ul_am"]) == SRSRAN_ERROR) {
      ul_am_rlc.sn_field_len_present = false;
    } else {
      ul_am_rlc.sn_field_len_present = true;
    }

    field_asn1_enum_number<asn1::rrc_nr::t_poll_retx_e> t_poll_retx("t_poll_retx", &ul_am_rlc.t_poll_retx);
    if (t_poll_retx.parse(root["ul_am"])) {
      ERROR("Error can't find t_poll_retx in section ul_am");
      return SRSRAN_ERROR;
    }

    field_asn1_enum_number<asn1::rrc_nr::poll_pdu_e> poll_pdu("poll_pdu", &ul_am_rlc.poll_pdu);
    if (poll_pdu.parse(root["ul_am"])) {
      ERROR("Error can't find poll_pdu in section ul_am");
      return SRSRAN_ERROR;
    }

    field_asn1_enum_number<asn1::rrc_nr::poll_byte_e> poll_byte("poll_byte", &ul_am_rlc.poll_byte);
    if (poll_byte.parse(root["ul_am"])) {
      ERROR("Error can't find poll_byte in section ul_am");
      return SRSRAN_ERROR;
    }

    field_asn1_enum_number<asn1::rrc_nr::ul_am_rlc_s::max_retx_thres_e_> max_retx_thresh("max_retx_thres",
                                                                                         &ul_am_rlc.max_retx_thres);
    if (max_retx_thresh.parse(root["ul_am"])) {
      ERROR("Error can't find max_retx_thresh in section ul_am");
      return SRSRAN_ERROR;
    }
  }

  if (root.exists("dl_am")) {
    asn1::rrc_nr::dl_am_rlc_s& dl_am_rlc = rlc_cfg->am().dl_am_rlc;

    // SN length
    field_asn1_enum_number<asn1::rrc_nr::sn_field_len_am_e> rlc_sn_size_ul("sn_field_len", &dl_am_rlc.sn_field_len);
    if (rlc_sn_size_ul.parse(root["dl_am"]) == SRSRAN_ERROR) {
      dl_am_rlc.sn_field_len_present = false;
    } else {
      dl_am_rlc.sn_field_len_present = true;
    }

    field_asn1_enum_number<asn1::rrc_nr::t_reassembly_e> t_reassembly("t_reassembly", &dl_am_rlc.t_reassembly);
    if (t_reassembly.parse(root["dl_am"])) {
      ERROR("Error can't find t_reordering in section dl_am");
      return SRSRAN_ERROR;
    }

    field_asn1_enum_number<asn1::rrc_nr::t_status_prohibit_e> t_status_prohibit("t_status_prohibit",
                                                                                &dl_am_rlc.t_status_prohibit);
    if (t_status_prohibit.parse(root["dl_am"])) {
      ERROR("Error can't find t_status_prohibit in section dl_am");
      return SRSRAN_ERROR;
    }
  }

  return 0;
}

int field_five_qi::parse(libconfig::Setting& root)
{
  uint32_t nof_five_qi = (uint32_t)root.getLength();
  for (uint32_t i = 0; i < nof_five_qi; i++) {
    libconfig::Setting& q = root[i];

    uint32_t five_qi = q["five_qi"];

    rrc_nr_cfg_five_qi_t five_qi_cfg;

    // Parse PDCP section
    if (!q.exists("pdcp_nr_config")) {
      fprintf(stderr, "Error section pdcp_nr_config not found for 5qi=%d\n", five_qi);
      return SRSRAN_ERROR;
    }
    libconfig::Setting&       pdcp_nr  = q["pdcp_nr_config"];
    asn1::rrc_nr::pdcp_cfg_s* pdcp_cfg = &five_qi_cfg.pdcp_cfg;

    // Get PDCP-NR DRB configs
    if (!pdcp_nr.exists("drb")) {
      fprintf(stderr, "Error section drb not found for 5QI=%d\n", five_qi);
      return SRSRAN_ERROR;
    }
    libconfig::Setting&               drb     = pdcp_nr["drb"];
    asn1::rrc_nr::pdcp_cfg_s::drb_s_* drb_cfg = &pdcp_cfg->drb;
    pdcp_cfg->drb_present                     = true;

    // PDCP SN size UL
    field_asn1_enum_number<asn1::rrc_nr::pdcp_cfg_s::drb_s_::pdcp_sn_size_ul_e_> pdcp_sn_size_ul(
        "pdcp_sn_size_ul", &drb_cfg->pdcp_sn_size_ul);
    if (pdcp_sn_size_ul.parse(drb) == SRSRAN_ERROR) {
      drb_cfg->pdcp_sn_size_ul_present = false;
    } else {
      drb_cfg->pdcp_sn_size_ul_present = true;
    }

    // PDCP SN size DL
    field_asn1_enum_number<asn1::rrc_nr::pdcp_cfg_s::drb_s_::pdcp_sn_size_dl_e_> pdcp_sn_size_dl(
        "pdcp_sn_size_dl", &drb_cfg->pdcp_sn_size_dl);
    if (pdcp_sn_size_dl.parse(drb) == SRSRAN_ERROR) {
      drb_cfg->pdcp_sn_size_dl_present = false;
    } else {
      drb_cfg->pdcp_sn_size_dl_present = true;
    }

    // Discard timer
    field_asn1_enum_number<asn1::rrc_nr::pdcp_cfg_s::drb_s_::discard_timer_e_> discard_timer("discard_timer",
                                                                                             &drb_cfg->discard_timer);
    if (discard_timer.parse(drb) == -1) {
      drb_cfg->discard_timer_present = false;
    } else {
      drb_cfg->discard_timer_present = true;
    }

    parser::field<bool> status_report_required("status_report_required", &drb_cfg->status_report_required_present);
    status_report_required.parse(drb);

    parser::field<bool> out_of_order_delivery("out_of_order_delivery", &drb_cfg->out_of_order_delivery_present);
    out_of_order_delivery.parse(drb);

    parser::field<bool> integrity_protection("integrity_protection", &drb_cfg->integrity_protection_present);
    integrity_protection.parse(drb);

    drb_cfg->hdr_compress.set_not_used();
    // Finish DRB config

    // t_Reordering
    field_asn1_enum_number<asn1::rrc_nr::pdcp_cfg_s::t_reordering_e_> t_reordering("t_reordering",
                                                                                   &pdcp_cfg->t_reordering);
    if (t_reordering.parse(pdcp_nr) == SRSRAN_ERROR) {
      pdcp_cfg->t_reordering_present = false;
    } else {
      pdcp_cfg->t_reordering_present = true;
    }

    // Parse RLC section
    if (!q.exists("rlc_config")) {
      fprintf(stderr, "Error section rlc_config not found for 5qi=%d\n", five_qi);
      return SRSRAN_ERROR;
    }
    libconfig::Setting&      rlc     = q["rlc_config"];
    asn1::rrc_nr::rlc_cfg_c* rlc_cfg = &five_qi_cfg.rlc_cfg;
    if (rlc.exists("um_uni_dir_ul") || rlc.exists("um_uni_dir_dl")) {
      // Sanity check: RLC UM uni-directional is not supported.
      fprintf(stderr, "Error uni-directional UM not supported. 5QI=%d\n", five_qi);
      return SRSRAN_ERROR;
    }

    if (rlc.exists("am")) {
      rlc_cfg->set_am();
    } else if (rlc.exists("um_bi_dir")) {
      rlc_cfg->set_um_bi_dir();
    } else {
      fprintf(stderr, "Invalid combination of UL/DL UM/AM for 5QI=%d\n", five_qi);
      return SRSRAN_ERROR;
    }

    // Parse RLC-AM section
    if (rlc_cfg->type() == asn1::rrc_nr::rlc_cfg_c::types::am) {
      libconfig::Setting&        rlc_am    = rlc["am"];
      libconfig::Setting&        rlc_am_ul = rlc_am["ul_am"];
      libconfig::Setting&        rlc_am_dl = rlc_am["dl_am"];
      asn1::rrc_nr::ul_am_rlc_s& ul_am_cfg = rlc_cfg->am().ul_am_rlc;
      asn1::rrc_nr::dl_am_rlc_s& dl_am_cfg = rlc_cfg->am().dl_am_rlc;

      // RLC AM UL
      // SN length
      field_asn1_enum_number<asn1::rrc_nr::sn_field_len_am_e> rlc_sn_size_ul("sn_field_len", &ul_am_cfg.sn_field_len);
      if (rlc_sn_size_ul.parse(rlc_am_ul) == SRSRAN_ERROR) {
        ul_am_cfg.sn_field_len_present = false;
      } else {
        ul_am_cfg.sn_field_len_present = true;
      }
      // t-PollRetx
      field_asn1_enum_number<asn1::rrc_nr::t_poll_retx_e> rlc_t_poll_retx("t_poll_retx", &ul_am_cfg.t_poll_retx);
      rlc_t_poll_retx.parse(rlc_am_ul);
      // pollPDU
      field_asn1_enum_number<asn1::rrc_nr::poll_pdu_e> rlc_poll_pdu("poll_pdu", &ul_am_cfg.poll_pdu);
      rlc_poll_pdu.parse(rlc_am_ul);
      // pollBYTE
      field_asn1_enum_number<asn1::rrc_nr::poll_byte_e> rlc_poll_bytes("poll_byte", &ul_am_cfg.poll_byte);
      rlc_poll_bytes.parse(rlc_am_ul);
      // maxRetxThreshold
      field_asn1_enum_number<asn1::rrc_nr::ul_am_rlc_s::max_retx_thres_e_> rlc_max_retx_thres(
          "max_retx_thres", &ul_am_cfg.max_retx_thres);
      rlc_max_retx_thres.parse(rlc_am_ul);

      // RLC AM DL
      // SN length
      field_asn1_enum_number<asn1::rrc_nr::sn_field_len_am_e> rlc_sn_size_dl("sn_field_len", &dl_am_cfg.sn_field_len);
      if (rlc_sn_size_dl.parse(rlc_am_dl) == SRSRAN_ERROR) {
        dl_am_cfg.sn_field_len_present = false;
      } else {
        dl_am_cfg.sn_field_len_present = true;
      }
      // t-reassembly
      field_asn1_enum_number<asn1::rrc_nr::t_reassembly_e> rlc_t_reassembly("t_reassembly", &dl_am_cfg.t_reassembly);
      rlc_t_reassembly.parse(rlc_am_dl);
      // t-statusProhibit
      field_asn1_enum_number<asn1::rrc_nr::t_status_prohibit_e> rlc_status_prohibit("t_status_prohibit",
                                                                                    &dl_am_cfg.t_status_prohibit);
      rlc_status_prohibit.parse(rlc_am_dl);
    } else if (rlc_cfg->type() == asn1::rrc_nr::rlc_cfg_c::types::um_bi_dir) {
      libconfig::Setting&        rlc_um    = rlc["um_bi_dir"];
      libconfig::Setting&        rlc_um_ul = rlc_um["ul_um"];
      libconfig::Setting&        rlc_um_dl = rlc_um["dl_um"];
      asn1::rrc_nr::ul_um_rlc_s& ul_um_cfg = rlc_cfg->um_bi_dir().ul_um_rlc;
      asn1::rrc_nr::dl_um_rlc_s& dl_um_cfg = rlc_cfg->um_bi_dir().dl_um_rlc;

      // RLC UM UL
      // SN field length
      field_asn1_enum_number<asn1::rrc_nr::sn_field_len_um_e> rlc_sn_size_ul("sn_field_len", &ul_um_cfg.sn_field_len);
      if (rlc_sn_size_ul.parse(rlc_um_ul) == SRSRAN_ERROR) {
        ul_um_cfg.sn_field_len_present = false;
      } else {
        ul_um_cfg.sn_field_len_present = true;
      }

      // RLC UM DL
      // SN field length
      field_asn1_enum_number<asn1::rrc_nr::sn_field_len_um_e> rlc_sn_size_dl("sn_field_len", &dl_um_cfg.sn_field_len);
      if (rlc_sn_size_dl.parse(rlc_um_dl) == SRSRAN_ERROR) {
        dl_um_cfg.sn_field_len_present = false;
      } else {
        dl_um_cfg.sn_field_len_present = true;
      }
      // t-reassembly
      field_asn1_enum_number<asn1::rrc_nr::t_reassembly_e> rlc_t_reassembly_dl("t_reassembly", &dl_um_cfg.t_reassembly);
      rlc_t_reassembly_dl.parse(rlc_um_dl);
    }

    cfg.insert(std::make_pair(five_qi, five_qi_cfg));
  }
  return 0;
}
namespace rr_sections {

int parse_rr(all_args_t* args_, rrc_cfg_t* rrc_cfg_, rrc_nr_cfg_t* rrc_nr_cfg_)
{
  /* Transmission mode config section */
  if (args_->enb.transmission_mode < 1 || args_->enb.transmission_mode > 4) {
    ERROR("Invalid transmission mode (%d). Only indexes 1-4 are implemented.", args_->enb.transmission_mode);
    return SRSRAN_ERROR;
  }
  if (args_->enb.transmission_mode == 1 && args_->enb.nof_ports > 1) {
    ERROR("Invalid number of ports (%d) for transmission mode (%d). Only one antenna port is allowed.",
          args_->enb.nof_ports,
          args_->enb.transmission_mode);
    return SRSRAN_ERROR;
  }
  if (args_->enb.transmission_mode > 1 && args_->enb.nof_ports != 2) {
    ERROR("The selected number of ports (%d) are insufficient for the selected transmission mode (%d).",
          args_->enb.nof_ports,
          args_->enb.transmission_mode);
    return SRSRAN_ERROR;
  }

  rrc_cfg_->antenna_info.tx_mode = (ant_info_ded_s::tx_mode_e_::options)(args_->enb.transmission_mode - 1);

  rrc_cfg_->antenna_info.ue_tx_ant_sel.set_setup();
  switch (rrc_cfg_->antenna_info.tx_mode) {
    case ant_info_ded_s::tx_mode_e_::tm1:
    case ant_info_ded_s::tx_mode_e_::tm2:
      rrc_cfg_->antenna_info.ue_tx_ant_sel.set(setup_e::release);
      rrc_cfg_->antenna_info.codebook_subset_restrict_present = false;
      break;
    case ant_info_ded_s::tx_mode_e_::tm3:
      rrc_cfg_->antenna_info.ue_tx_ant_sel.setup().value = ant_info_ded_s::ue_tx_ant_sel_c_::setup_e_::open_loop;

      rrc_cfg_->antenna_info.codebook_subset_restrict_present = true;
      rrc_cfg_->antenna_info.codebook_subset_restrict.set_n2_tx_ant_tm3();
      rrc_cfg_->antenna_info.codebook_subset_restrict.n2_tx_ant_tm3().from_number(0b11);
      break;
    case ant_info_ded_s::tx_mode_e_::tm4:
      rrc_cfg_->antenna_info.ue_tx_ant_sel.setup().value = ant_info_ded_s::ue_tx_ant_sel_c_::setup_e_::closed_loop;

      rrc_cfg_->antenna_info.codebook_subset_restrict_present = true;
      rrc_cfg_->antenna_info.codebook_subset_restrict.set_n2_tx_ant_tm4();
      rrc_cfg_->antenna_info.codebook_subset_restrict.n2_tx_ant_tm4().from_number(0b111111);
      break;
    default:
      ERROR("Unsupported transmission mode %d", rrc_cfg_->antenna_info.tx_mode.to_number());
      return SRSRAN_ERROR;
  }

  /* Parse power allocation */
  if (not asn1::number_to_enum(rrc_cfg_->pdsch_cfg, args_->enb.p_a)) {
    ERROR("Invalid p_a value (%f) only -6, -4.77, -3, -1.77, 0, 1, 2, 3 values allowed.", args_->enb.p_a);
    return SRSRAN_ERROR;
  }

  /* MAC config section */
  parser::section mac_cnfg("mac_cnfg");
  parser::section phr_cnfg("phr_cnfg");
  mac_cnfg.add_subsection(&phr_cnfg);
  rrc_cfg_->mac_cnfg.phr_cfg.set(
      mac_main_cfg_s::phr_cfg_c_::types::release); // default is release if "phr_cnfg" is not found
  mac_cnfg.add_field(new phr_cnfg_parser(&rrc_cfg_->mac_cnfg.phr_cfg));
  //  mac_cnfg.add_field(new phr_cnfg_parser(&rrc_cfg_->mac_cnfg.phr_cfg));

  parser::section ulsch_cnfg("ulsch_cnfg");
  mac_cnfg.add_subsection(&ulsch_cnfg);

  rrc_cfg_->mac_cnfg.ul_sch_cfg.tti_bundling = false;
  ulsch_cnfg.add_field(make_asn1_enum_number_parser(
      "max_harq_tx", &rrc_cfg_->mac_cnfg.ul_sch_cfg.max_harq_tx, &rrc_cfg_->mac_cnfg.ul_sch_cfg.max_harq_tx_present));
  ulsch_cnfg.add_field(make_asn1_enum_number_parser("periodic_bsr_timer",
                                                    &rrc_cfg_->mac_cnfg.ul_sch_cfg.periodic_bsr_timer,
                                                    &rrc_cfg_->mac_cnfg.ul_sch_cfg.periodic_bsr_timer_present));
  ulsch_cnfg.add_field(make_asn1_enum_number_parser("retx_bsr_timer", &rrc_cfg_->mac_cnfg.ul_sch_cfg.retx_bsr_timer));

  mac_cnfg.add_field(make_asn1_enum_number_parser("time_alignment_timer", &rrc_cfg_->mac_cnfg.time_align_timer_ded));

  /* PHY config section */
  parser::section phy_cfg_("phy_cnfg");

  parser::section pusch_cnfg_ded("pusch_cnfg_ded");
  phy_cfg_.add_subsection(&pusch_cnfg_ded);

  pusch_cnfg_ded.add_field(new parser::field<uint8>("beta_offset_ack_idx", &rrc_cfg_->pusch_cfg.beta_offset_ack_idx));
  pusch_cnfg_ded.add_field(new parser::field<uint8>("beta_offset_ri_idx", &rrc_cfg_->pusch_cfg.beta_offset_ri_idx));
  pusch_cnfg_ded.add_field(new parser::field<uint8>("beta_offset_cqi_idx", &rrc_cfg_->pusch_cfg.beta_offset_cqi_idx));

  parser::section sched_request_cnfg("sched_request_cnfg");
  phy_cfg_.add_subsection(&sched_request_cnfg);

  sched_request_cnfg.add_field(make_asn1_enum_number_parser("dsr_trans_max", &rrc_cfg_->sr_cfg.dsr_max));
  sched_request_cnfg.add_field(new parser::field<uint32>("period", &rrc_cfg_->sr_cfg.period));
  sched_request_cnfg.add_field(new parser::field<uint32>("nof_prb", &rrc_cfg_->sr_cfg.nof_prb));
  sched_request_cnfg.add_field(new field_sf_mapping(rrc_cfg_->sr_cfg.sf_mapping, &rrc_cfg_->sr_cfg.nof_subframes, 0));

  parser::section cqi_report_cnfg("cqi_report_cnfg");
  phy_cfg_.add_subsection(&cqi_report_cnfg);

  cqi_report_cnfg.add_field(new parser::field_enum_str<rrc_cfg_cqi_mode_t>(
      "mode", &rrc_cfg_->cqi_cfg.mode, rrc_cfg_cqi_mode_text, RRC_CFG_CQI_MODE_N_ITEMS));
  cqi_report_cnfg.add_field(new parser::field<uint32>("period", &rrc_cfg_->cqi_cfg.period));
  cqi_report_cnfg.add_field(new parser::field<uint32>("m_ri", &rrc_cfg_->cqi_cfg.m_ri));
  cqi_report_cnfg.add_field(
      new parser::field<uint32>("subband_k", &rrc_cfg_->cqi_cfg.subband_k, &rrc_cfg_->cqi_cfg.is_subband_enabled));
  cqi_report_cnfg.add_field(new parser::field<bool>("simultaneousAckCQI", &rrc_cfg_->cqi_cfg.simultaneousAckCQI));
  cqi_report_cnfg.add_field(new field_sf_mapping(rrc_cfg_->cqi_cfg.sf_mapping, &rrc_cfg_->cqi_cfg.nof_subframes, 1));

  // EUTRA RRC and cell config section
  parser::section cell_cnfg("cell_list");
  cell_cnfg.set_optional(&rrc_cfg_->meas_cfg_present);
  cell_cnfg.add_field(new rr_sections::cell_list_section(args_, rrc_cfg_));

  // NR RRC and cell config section
  bool            nr_cell_cnfg_present = false;
  parser::section nr_cell_cnfg("nr_cell_list");
  nr_cell_cnfg.set_optional(&nr_cell_cnfg_present);
  nr_cell_cnfg.add_field(new rr_sections::nr_cell_list_section(args_, rrc_nr_cfg_, rrc_cfg_));

  // Run parser with two sections
  parser p(args_->enb_files.rr_config);
  p.add_section(&mac_cnfg);
  p.add_section(&phy_cfg_);
  p.add_section(&cell_cnfg);
  p.add_section(&nr_cell_cnfg);

  return p.parse();
}

static int parse_meas_cell_list(rrc_meas_cfg_t* meas_cfg, Setting& root)
{
  meas_cfg->meas_cells.resize(root.getLength());
  for (uint32_t i = 0; i < meas_cfg->meas_cells.size(); ++i) {
    auto& cell  = meas_cfg->meas_cells[i];
    cell.earfcn = root[i]["dl_earfcn"];
    cell.pci    = (unsigned int)root[i]["pci"] % SRSRAN_NUM_PCI;
    cell.eci    = (unsigned int)root[i]["eci"];
    parse_default_field(cell.direct_forward_path_available, root[i], "direct_forward_path_available", false);
    parse_default_field(cell.allowed_meas_bw, root[i], "allowed_meas_bw", 6u);
    asn1_parsers::default_number_to_enum(
        cell.cell_individual_offset, root[i], "cell_individual_offset", asn1::rrc::q_offset_range_opts::db0);
    parse_default_field(cell.tac, root[i], "tac", -1);
    srsran_assert(srsran::is_lte_cell_nof_prb(cell.allowed_meas_bw), "Invalid measurement Bandwidth");
  }
  return 0;
}

static int parse_meas_report_desc(rrc_meas_cfg_t* meas_cfg, Setting& cellroot)
{
  // NOTE: Events A1, A2, A3 and A4 are supported. A3 and A4 will be configured for all neighbour cells

  Setting& root = cellroot["meas_report_desc"];

  meas_cfg->meas_reports.resize(root.getLength());
  for (int i = 0; i < root.getLength(); i++) {
    asn1::rrc::report_cfg_eutra_s& meas_item = meas_cfg->meas_reports[i];

    // Parse trigger quantity before event
    HANDLEPARSERCODE(asn1_parsers::str_to_enum(meas_item.trigger_quant, root[i]["trigger_quant"]));

    auto& event = meas_item.trigger_type.set_event();

    // Configure event
    switch ((int)root[i]["eventA"]) {
      case 1:
        if (!root[i].exists("a1_thresh")) {
          ERROR("Missing a1_thresh field for A1 event\n");
          return SRSRAN_ERROR;
        }
        if (meas_item.trigger_quant == report_cfg_eutra_s::trigger_quant_opts::rsrp) {
          event.event_id.set_event_a1().a1_thres.set_thres_rsrp() =
              rrc_value_to_range(srsran::quant_rsrp, (int)root[i]["a1_thresh"]);
        } else {
          event.event_id.set_event_a1().a1_thres.set_thres_rsrq() =
              rrc_value_to_range(srsran::quant_rsrq, (int)root[i]["a1_thresh"]);
        }
        break;
      case 2:
        if (!root[i].exists("a2_thresh")) {
          ERROR("Missing a2_thresh field for A2 event\n");
          return SRSRAN_ERROR;
        }
        if (meas_item.trigger_quant == report_cfg_eutra_s::trigger_quant_opts::rsrp) {
          event.event_id.set_event_a2().a2_thres.set_thres_rsrp() =
              rrc_value_to_range(srsran::quant_rsrp, (int)root[i]["a2_thresh"]);
        } else {
          event.event_id.set_event_a2().a2_thres.set_thres_rsrq() =
              rrc_value_to_range(srsran::quant_rsrq, (int)root[i]["a2_thresh"]);
        }
        break;
      case 3:
        if (!root[i].exists("a3_offset")) {
          ERROR("Missing a3_offset field for A3 event\n");
          return SRSRAN_ERROR;
        }
        event.event_id.set_event_a3().report_on_leave = false;
        event.event_id.event_a3().a3_offset           = (int)root[i]["a3_offset"];
        break;
      case 4:
        if (!root[i].exists("a4_thresh")) {
          ERROR("Missing a4_thresh field for A4 event\n");
          return SRSRAN_ERROR;
        }
        if (meas_item.trigger_quant == report_cfg_eutra_s::trigger_quant_opts::rsrp) {
          event.event_id.set_event_a4().a4_thres.set_thres_rsrp() =
              rrc_value_to_range(srsran::quant_rsrp, (int)root[i]["a4_thresh"]);
        } else {
          event.event_id.set_event_a4().a4_thres.set_thres_rsrq() =
              rrc_value_to_range(srsran::quant_rsrq, (int)root[i]["a4_thresh"]);
        }
        break;
      case 5:
        // a5-threshold1
        if (!root[i].exists("a5_thresh1")) {
          ERROR("Missing a5_thresh1 field for A5 event\n");
          return SRSRAN_ERROR;
        }
        if (meas_item.trigger_quant == report_cfg_eutra_s::trigger_quant_opts::rsrp) {
          event.event_id.set_event_a5().a5_thres1.set_thres_rsrp() =
              rrc_value_to_range(srsran::quant_rsrp, (int)root[i]["a5_thresh1"]);
        } else {
          event.event_id.set_event_a5().a5_thres1.set_thres_rsrq() =
              rrc_value_to_range(srsran::quant_rsrq, (int)root[i]["a5_thresh1"]);
        }

        // a5-threshold2
        if (!root[i].exists("a5_thresh2")) {
          ERROR("Missing a5_thresh2 field for A5 event\n");
          return SRSRAN_ERROR;
        }
        if (meas_item.trigger_quant == report_cfg_eutra_s::trigger_quant_opts::rsrp) {
          event.event_id.set_event_a5().a5_thres2.set_thres_rsrp() =
              rrc_value_to_range(srsran::quant_rsrp, (int)root[i]["a5_thresh2"]);
        } else {
          event.event_id.set_event_a5().a5_thres2.set_thres_rsrq() =
              rrc_value_to_range(srsran::quant_rsrq, (int)root[i]["a5_thresh2"]);
        }
        break;
      default:
        ERROR("Invalid or unsupported event A%d in meas_report_desc (only A1-A5 are supported)\n",
              (int)root[i]["eventA"]);
        return SRSRAN_ERROR;
    }

    // Configure common variables
    event.hysteresis = (int)root[i]["hysteresis"];
    HANDLEPARSERCODE(asn1_parsers::number_to_enum(event.time_to_trigger, root[i]["time_to_trigger"]));
    meas_item.report_quant.value = report_cfg_eutra_s::report_quant_opts::both; // TODO: parse
    meas_item.max_report_cells   = (int)root[i]["max_report_cells"];
    HANDLEPARSERCODE(asn1_parsers::number_to_enum(meas_item.report_interv, root[i]["report_interv"]));
    HANDLEPARSERCODE(asn1_parsers::number_to_enum(meas_item.report_amount, root[i]["report_amount"]));
  }

  // quant coeff parsing
  auto& quant = meas_cfg->quant_cfg;

  HANDLEPARSERCODE(asn1_parsers::opt_number_to_enum(
      quant.filt_coef_rsrp, quant.filt_coef_rsrp_present, cellroot["meas_quant_desc"], "rsrp_config"));
  HANDLEPARSERCODE(asn1_parsers::opt_number_to_enum(
      quant.filt_coef_rsrq, quant.filt_coef_rsrq_present, cellroot["meas_quant_desc"], "rsrq_config"));

  return SRSRAN_SUCCESS;
}

static int parse_scell_list(cell_cfg_t& cell_cfg, Setting& cellroot)
{
  auto cell_id_parser = [](uint32_t& cell_id, Setting& root) { return parse_bounded_number(cell_id, root, 0u, 255u); };

  cell_cfg.scell_list.resize(cellroot["scell_list"].getLength());
  for (uint32_t i = 0; i < cell_cfg.scell_list.size(); ++i) {
    auto& scell     = cell_cfg.scell_list[i];
    auto& scellroot = cellroot["scell_list"][i];
    cell_id_parser(scell.cell_id, scellroot["cell_id"]);
    scell.cross_carrier_sched = (bool)scellroot["cross_carrier_scheduling"];
    cell_id_parser(scell.sched_cell_id, scellroot["scheduling_cell_id"]);
    scell.ul_allowed = (bool)scellroot["ul_allowed"];
  }

  return SRSRAN_SUCCESS;
}

static int parse_cell_list(all_args_t* args, rrc_cfg_t* rrc_cfg, Setting& root)
{
  for (uint32_t n = 0; n < (uint32_t)root.getLength(); ++n) {
    cell_cfg_t cell_cfg = {};
    auto&      cellroot = root[n];

    parse_opt_field(cell_cfg.rf_port, cellroot, "rf_port");
    HANDLEPARSERCODE(parse_required_field(cell_cfg.cell_id, cellroot, "cell_id"));
    HANDLEPARSERCODE(parse_required_field(cell_cfg.tac, cellroot, "tac"));
    HANDLEPARSERCODE(parse_required_field(cell_cfg.pci, cellroot, "pci"));
    parse_default_field(cell_cfg.tx_gain, cellroot, "tx_gain", 0.0);
    cell_cfg.pci = cell_cfg.pci % SRSRAN_NUM_PCI;
    HANDLEPARSERCODE(parse_required_field(cell_cfg.dl_earfcn, cellroot, "dl_earfcn"));
    parse_default_field(cell_cfg.dl_freq_hz, cellroot, "dl_freq", 0.0); // will be derived from DL EARFCN If not set
    parse_default_field(cell_cfg.ul_freq_hz, cellroot, "ul_freq", 0.0); // will be derived from DL EARFCN If not set
    parse_default_field(cell_cfg.ul_earfcn, cellroot, "ul_earfcn", 0u); // will be derived from DL EARFCN If not set
    parse_default_field(
        cell_cfg.root_seq_idx, cellroot, "root_seq_idx", rrc_cfg->sibs[1].sib2().rr_cfg_common.prach_cfg.root_seq_idx);
    parse_default_field(cell_cfg.meas_cfg.meas_gap_period, cellroot, "meas_gap_period", 0u);
    if (cellroot.exists("meas_gap_offset_subframe")) {
      cell_cfg.meas_cfg.meas_gap_offset_subframe.resize(cellroot["meas_gap_offset_subframe"].getLength());
      for (uint32_t j = 0; j < (uint32_t)cellroot["meas_gap_offset_subframe"].getLength(); ++j) {
        cell_cfg.meas_cfg.meas_gap_offset_subframe[j] = (uint32_t)cellroot["meas_gap_offset_subframe"][j];
        srsran_assert(cell_cfg.meas_cfg.meas_gap_offset_subframe[j] < cell_cfg.meas_cfg.meas_gap_period,
                      "meas gap offsets must be smaller than meas gap period");
      }
    }
    HANDLEPARSERCODE(parse_default_field(cell_cfg.target_pusch_sinr_db, cellroot, "target_pusch_sinr", -1));
    HANDLEPARSERCODE(parse_default_field(cell_cfg.target_pucch_sinr_db, cellroot, "target_pucch_sinr", -1));
    HANDLEPARSERCODE(parse_default_field(cell_cfg.enable_phr_handling, cellroot, "enable_phr_handling", false));
    HANDLEPARSERCODE(parse_default_field(cell_cfg.min_phr_thres, cellroot, "min_phr_thres", 0));
    parse_default_field(cell_cfg.meas_cfg.allowed_meas_bw, cellroot, "allowed_meas_bw", 6u);
    srsran_assert(srsran::is_lte_cell_nof_prb(cell_cfg.meas_cfg.allowed_meas_bw), "Invalid measurement Bandwidth");
    HANDLEPARSERCODE(asn1_parsers::default_number_to_enum(
        cell_cfg.t304, cellroot, "t304", asn1::rrc::mob_ctrl_info_s::t304_opts::ms2000));

    if (cellroot.exists("ho_active") and cellroot["ho_active"]) {
      HANDLEPARSERCODE(parse_meas_cell_list(&cell_cfg.meas_cfg, cellroot["meas_cell_list"]));
      if (not cellroot.exists("meas_report_desc")) {
        ERROR("PARSER ERROR: \"ho_active\" is set to true, but field \"meas_report_desc\" doesn't exist.\n");
        return SRSRAN_ERROR;
      }
      HANDLEPARSERCODE(parse_meas_report_desc(&cell_cfg.meas_cfg, cellroot));
    }

    if (cellroot.exists("barred") and cellroot["barred"]) {
      cell_cfg.barred = true;
    }

    if (cellroot.exists("scell_list")) {
      HANDLEPARSERCODE(parse_scell_list(cell_cfg, cellroot));
    }

    rrc_cfg->cell_list.push_back(cell_cfg);
  }

  // Configuration check
  // counter for every RF port used by the eNB to avoid misconfiguration/mapping of cells
  uint32_t next_rf_port = 0;
  for (auto it = rrc_cfg->cell_list.begin(); it != rrc_cfg->cell_list.end(); it++) {
    // Make sure RF ports are assigned in order
    if (it->rf_port != next_rf_port) {
      ERROR("RF ports need to be in order starting with 0 (%d != %d)", it->rf_port, next_rf_port);
      return SRSRAN_ERROR;
    }
    next_rf_port++;

    for (auto it2 = it + 1; it2 != rrc_cfg->cell_list.end(); it2++) {
      // Check RF port is not repeated
      if (it->rf_port == it2->rf_port) {
        ERROR("Repeated RF port for multiple cells");
        return SRSRAN_ERROR;
      }

      // Check cell ID is not repeated
      if (it->cell_id == it2->cell_id) {
        ERROR("Repeated Cell identifier");
        return SRSRAN_ERROR;
      }
    }
  }

  return SRSRAN_SUCCESS;
}

static int parse_nr_cell_list(all_args_t* args, rrc_nr_cfg_t* rrc_cfg_nr, rrc_cfg_t* rrc_cfg_eutra, Setting& root)
{
  for (uint32_t n = 0; n < (uint32_t)root.getLength(); ++n) {
    auto& cellroot = root[n];

    rrc_cell_cfg_nr_t cell_cfg = {};
    generate_default_nr_cell(cell_cfg);

    parse_opt_field(cell_cfg.phy_cell.rf_port, cellroot, "rf_port");
    HANDLEPARSERCODE(parse_required_field(cell_cfg.phy_cell.carrier.pci, cellroot, "pci"));
    HANDLEPARSERCODE(parse_required_field(cell_cfg.phy_cell.cell_id, cellroot, "cell_id"));
    HANDLEPARSERCODE(parse_opt_field(cell_cfg.coreset0_idx, cellroot, "coreset0_idx"));
    HANDLEPARSERCODE(parse_required_field(cell_cfg.prach_root_seq_idx, cellroot, "root_seq_idx"));
    HANDLEPARSERCODE(parse_required_field(cell_cfg.tac, cellroot, "tac"));

    cell_cfg.phy_cell.carrier.pci = cell_cfg.phy_cell.carrier.pci % SRSRAN_NOF_NID_NR;
    HANDLEPARSERCODE(parse_required_field(cell_cfg.dl_arfcn, cellroot, "dl_arfcn"));
    parse_opt_field(cell_cfg.ul_arfcn, cellroot, "ul_arfcn");
    HANDLEPARSERCODE(parse_required_field(cell_cfg.band, cellroot, "band"));
    // frequencies get derived from ARFCN

    // TODO: Add further cell-specific parameters

    rrc_cfg_nr->cell_list.push_back(cell_cfg);
  }

  srsran::srsran_band_helper band_helper;
  // Configuration check
  // counter for every RF port used by the eNB to avoid misconfiguration/mapping of cells
  uint32_t next_rf_port = rrc_cfg_eutra->cell_list.size();
  for (auto it = rrc_cfg_nr->cell_list.begin(); it != rrc_cfg_nr->cell_list.end(); ++it) {
    // Make sure RF ports are assigned in order
    if (it->phy_cell.rf_port != next_rf_port) {
      ERROR("RF ports need to be in order starting with 0 (%d != %d)", it->phy_cell.rf_port, next_rf_port);
      return SRSRAN_ERROR;
    }
    next_rf_port++;

    // check against other NR cells
    for (auto it2 = it + 1; it2 != rrc_cfg_nr->cell_list.end(); it2++) {
      // Check RF port is not repeated
      if (it->phy_cell.rf_port == it2->phy_cell.rf_port) {
        ERROR("Repeated RF port for multiple cells");
        return SRSRAN_ERROR;
      }

      // Check cell PCI not repeated
      if (it->phy_cell.carrier.pci == it2->phy_cell.carrier.pci) {
        ERROR("Repeated cell PCI");
        return SRSRAN_ERROR;
      }

      // Check cell PCI and cell ID is not repeated
      if (it->phy_cell.cell_id == it2->phy_cell.cell_id) {
        ERROR("Repeated Cell identifier");
        return SRSRAN_ERROR;
      }
    }

    // also check RF port against EUTRA cells
    for (auto it_eutra = rrc_cfg_eutra->cell_list.begin(); it_eutra != rrc_cfg_eutra->cell_list.end(); ++it_eutra) {
      // Check RF port is not repeated
      if (it->phy_cell.rf_port == it_eutra->rf_port) {
        ERROR("Repeated RF port for multiple cells");
        return SRSRAN_ERROR;
      }
    }

    // Check if dl_arfcn is valid for the given band
    bool                  dl_arfcn_valid = false;
    std::vector<uint32_t> bands          = band_helper.get_bands_nr(it->dl_arfcn);
    for (uint32_t band_idx = 0; band_idx < bands.size(); band_idx++) {
      if (bands.at(band_idx) == it->band) {
        dl_arfcn_valid = true;
      }
    }
    if (!dl_arfcn_valid) {
      if (not bands.empty()) {
        std::stringstream ss;
        for (uint32_t& band : bands) {
          ss << band << " ";
        }
        ERROR("DL ARFCN (%d) does not belong to band (%d). Recommended bands: %s",
              it->dl_arfcn,
              it->band,
              ss.str().c_str());
        return SRSRAN_ERROR;
      }
      ERROR("DL ARFCN (%d) is not valid for the specified band (%d)", it->dl_arfcn, it->band);
      return SRSRAN_ERROR;
    }

    if (it->ul_arfcn != 0) {
      // Check if ul_arfcn is valid for the given band
      bool                  ul_arfcn_valid = false;
      std::vector<uint32_t> ul_bands       = band_helper.get_bands_nr(it->ul_arfcn);
      for (uint32_t band_idx = 0; band_idx < ul_bands.size(); band_idx++) {
        if (ul_bands.at(band_idx) == it->band) {
          ul_arfcn_valid = true;
        }
      }
      if (!ul_arfcn_valid) {
        ERROR("UL ARFCN (%d) is not valid for the specified band (%d)", it->ul_arfcn, it->band);
        return SRSRAN_ERROR;
      }
    }
  }

  return SRSRAN_SUCCESS;
}

int cell_list_section::parse(libconfig::Setting& root)
{
  HANDLEPARSERCODE(parse_cell_list(args, rrc_cfg, root));
  return 0;
}

int nr_cell_list_section::parse(libconfig::Setting& root)
{
  HANDLEPARSERCODE(parse_nr_cell_list(args, nr_rrc_cfg, eutra_rrc_cfg, root));
  return 0;
}

} // namespace rr_sections

namespace enb_conf_sections {

int parse_cell_cfg(all_args_t* args_, srsran_cell_t* cell)
{
  cell->frame_type = SRSRAN_FDD;
  cell->cp         = args_->phy.extended_cp ? SRSRAN_CP_EXT : SRSRAN_CP_NORM;
  cell->nof_ports  = args_->enb.nof_ports;
  cell->nof_prb    = args_->enb.n_prb;
  // PCI not configured yet

  phich_cfg_s     phichcfg;
  parser::section phy_cnfg("phy_cnfg");
  parser::section phich_cnfg("phich_cnfg");
  phy_cnfg.add_subsection(&phich_cnfg);
  phich_cnfg.add_field(make_asn1_enum_str_parser("duration", &phichcfg.phich_dur));
  phich_cnfg.add_field(make_asn1_enum_number_str_parser("resources", &phichcfg.phich_res));
  parser::parse_section(args_->enb_files.rr_config, &phy_cnfg);

  cell->phich_length    = (srsran_phich_length_t)(int)phichcfg.phich_dur;
  cell->phich_resources = (srsran_phich_r_t)(int)phichcfg.phich_res;

  if (!srsran_cell_isvalid(cell)) {
    fprintf(stderr,
            "Invalid cell parameters: nof_prb=%d, nof_ports=%d, cell_id=%d\n",
            cell->nof_prb,
            cell->nof_ports,
            cell->id);
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

// Parse the relevant CFR configuration params
int parse_cfr_args(all_args_t* args, srsran_cfr_cfg_t* cfr_config)
{
  cfr_config->cfr_enable  = args->phy.cfr_args.enable;
  cfr_config->cfr_mode    = args->phy.cfr_args.mode;
  cfr_config->alpha       = args->phy.cfr_args.strength;
  cfr_config->manual_thr  = args->phy.cfr_args.manual_thres;
  cfr_config->max_papr_db = args->phy.cfr_args.auto_target_papr;
  cfr_config->ema_alpha   = args->phy.cfr_args.ema_alpha;

  if (!srsran_cfr_params_valid(cfr_config)) {
    fprintf(stderr,
            "Invalid CFR parameters: cfr_mode=%d, alpha=%.2f, manual_thr=%.2f, \n "
            "max_papr_db=%.2f, ema_alpha=%.2f\n",
            cfr_config->cfr_mode,
            cfr_config->alpha,
            cfr_config->manual_thr,
            cfr_config->max_papr_db,
            cfr_config->ema_alpha);
    return SRSRAN_ERROR;
  }
  return SRSRAN_SUCCESS;
}

int parse_cfg_files(all_args_t* args_, rrc_cfg_t* rrc_cfg_, rrc_nr_cfg_t* rrc_nr_cfg_, phy_cfg_t* phy_cfg_)
{
  // Parse config files
  srsran_cell_t cell_common_cfg = {};

  try {
    if (enb_conf_sections::parse_cell_cfg(args_, &cell_common_cfg) != SRSRAN_SUCCESS) {
      fprintf(stderr, "Error parsing Cell configuration\n");
      return SRSRAN_ERROR;
    }
  } catch (const SettingTypeException& stex) {
    fprintf(stderr, "Error parsing Cell configuration: %s\n", stex.getPath());
    return SRSRAN_ERROR;
  } catch (const ConfigException& cex) {
    fprintf(stderr, "Error parsing Cell configuration\n");
    return SRSRAN_ERROR;
  }

  try {
    if (sib_sections::parse_sibs(args_, rrc_cfg_, phy_cfg_) != SRSRAN_SUCCESS) {
      fprintf(stderr, "Error parsing SIB configuration\n");
      return SRSRAN_ERROR;
    }
  } catch (const SettingTypeException& stex) {
    fprintf(stderr, "Error parsing SIB configuration: %s\n", stex.getPath());
    return SRSRAN_ERROR;
  } catch (const ConfigException& cex) {
    fprintf(stderr, "Error parsing SIB configurationn\n");
    return SRSRAN_ERROR;
  }

  try {
    if (rr_sections::parse_rr(args_, rrc_cfg_, rrc_nr_cfg_) != SRSRAN_SUCCESS) {
      fprintf(stderr, "Error parsing Radio Resources configuration\n");
      return SRSRAN_ERROR;
    }
  } catch (const SettingTypeException& stex) {
    fprintf(stderr, "Error parsing Radio Resources configuration: %s\n", stex.getPath());
    return SRSRAN_ERROR;
  } catch (const ConfigException& cex) {
    fprintf(stderr, "Error parsing Radio Resources configuration\n");
    return SRSRAN_ERROR;
  }

  try {
    if (rb_sections::parse_rb(args_, rrc_cfg_, rrc_nr_cfg_) != SRSRAN_SUCCESS) {
      fprintf(stderr, "Error parsing RB configuration\n");
      return SRSRAN_ERROR;
    }
  } catch (const SettingTypeException& stex) {
    fprintf(stderr, "Error parsing RB configuration: %s\n", stex.getPath());
    return SRSRAN_ERROR;
  } catch (const ConfigException& cex) {
    fprintf(stderr, "Error parsing RB configuration\n");
    return SRSRAN_ERROR;
  }

  // update number of NR cells
  rrc_cfg_->num_nr_cells = rrc_nr_cfg_->cell_list.size();
  args_->rf.nof_carriers = rrc_cfg_->cell_list.size() + rrc_nr_cfg_->cell_list.size();
  ASSERT_VALID_CFG(args_->rf.nof_carriers > 0, "There must be at least one NR or LTE cell");
  if (rrc_nr_cfg_->cell_list.size() > 0) {
    // NR cells available.
    if (rrc_cfg_->cell_list.size() == 0) {
      // SA mode.
      rrc_nr_cfg_->is_standalone = true;
    } else {
      // NSA mode.
      rrc_nr_cfg_->is_standalone = false;
    }
  }

  // Set fields derived from others, and check for correctness of the parsed configuration
  if (enb_conf_sections::set_derived_args(args_, rrc_cfg_, phy_cfg_, cell_common_cfg) != SRSRAN_SUCCESS) {
    fprintf(stderr, "Error deriving EUTRA cell parameters\n");
    return SRSRAN_ERROR;
  }

  // do the same for NR
  if (enb_conf_sections::set_derived_args_nr(args_, rrc_nr_cfg_, phy_cfg_) != SRSRAN_SUCCESS) {
    fprintf(stderr, "Error deriving NR cell parameters\n");
    return SRSRAN_ERROR;
  }

  // update number of NR cells
  if (rrc_nr_cfg_->cell_list.size() > 0) {
    // NR cells available.
    if (rrc_nr_cfg_->is_standalone) {
      // SA mode. Update NGAP args
      args_->nr_stack.ngap.cell_id = rrc_nr_cfg_->cell_list[0].phy_cell.cell_id;
      args_->nr_stack.ngap.tac     = rrc_nr_cfg_->cell_list[0].tac;
      // take equivalent S1AP params to update NGAP params
      args_->nr_stack.ngap.gnb_name           = args_->stack.s1ap.enb_name;
      args_->nr_stack.ngap.gnb_id             = args_->enb.enb_id;
      args_->nr_stack.ngap.mcc                = args_->stack.s1ap.mcc;
      args_->nr_stack.ngap.mnc                = args_->stack.s1ap.mnc;
      args_->nr_stack.ngap.gtp_bind_addr      = args_->stack.s1ap.gtp_bind_addr;
      args_->nr_stack.ngap.gtp_advertise_addr = args_->stack.s1ap.gtp_advertise_addr;
      args_->nr_stack.ngap.amf_addr           = args_->stack.s1ap.mme_addr;
      args_->nr_stack.ngap.ngc_bind_addr      = args_->stack.s1ap.gtp_bind_addr;

      // Parse NIA/NEA preference list (use same as LTE for now)
      for (uint32_t i = 0; i < rrc_cfg_->eea_preference_list.size(); i++) {
        rrc_nr_cfg_->nea_preference_list[i] = (srsran::CIPHERING_ALGORITHM_ID_NR_ENUM)rrc_cfg_->eea_preference_list[i];
        rrc_nr_cfg_->nia_preference_list[i] = (srsran::INTEGRITY_ALGORITHM_ID_NR_ENUM)rrc_cfg_->eia_preference_list[i];
      }

    } else {
      // NSA mode.
      // update EUTRA RRC params for ENDC
      rrc_cfg_->endc_cfg.abs_frequency_ssb = rrc_nr_cfg_->cell_list.at(0).ssb_absolute_freq_point;
      rrc_cfg_->endc_cfg.nr_band           = rrc_nr_cfg_->cell_list.at(0).band;
      rrc_cfg_->endc_cfg.ssb_period_offset.set_sf10_r15();
      rrc_cfg_->endc_cfg.ssb_duration      = asn1::rrc::mtc_ssb_nr_r15_s::ssb_dur_r15_opts::sf1;
      rrc_cfg_->endc_cfg.ssb_ssc           = asn1::rrc::rs_cfg_ssb_nr_r15_s::subcarrier_spacing_ssb_r15_opts::khz15;
      rrc_cfg_->endc_cfg.act_from_b1_event = true; // ENDC will only be activated from B1 measurment
    }
  }

  // Parse CFR args
  if (parse_cfr_args(args_, &phy_cfg_->cfr_config) < SRSRAN_SUCCESS) {
    fprintf(stderr, "Error parsing CFR configuration\n");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int set_derived_args(all_args_t* args_, rrc_cfg_t* rrc_cfg_, phy_cfg_t* phy_cfg_, const srsran_cell_t& cell_cfg_)
{
  // Sanity checks
  ASSERT_VALID_CFG(args_->stack.mac.nof_prealloc_ues <= SRSENB_MAX_UES,
                   "mac.nof_prealloc_ues=%d must be within [0, %d]",
                   args_->stack.mac.nof_prealloc_ues,
                   SRSENB_MAX_UES);

  // Check for a forced  DL EARFCN or frequency (only valid for a single cell config
  if (rrc_cfg_->cell_list.size() > 0) {
    if (rrc_cfg_->cell_list.size() == 1) {
      auto& cfg = rrc_cfg_->cell_list.at(0);
      if (args_->enb.dl_earfcn > 0 and args_->enb.dl_earfcn != cfg.dl_earfcn) {
        cfg.dl_earfcn = args_->enb.dl_earfcn;
        ERROR("Force DL EARFCN for cell PCI=%d to %d", cfg.pci, cfg.dl_earfcn);
      }
      if (args_->rf.dl_freq > 0) {
        cfg.dl_freq_hz = args_->rf.dl_freq;
        ERROR("Force DL freq for cell PCI=%d to %f MHz", cfg.pci, cfg.dl_freq_hz / 1e6f);
      }
      if (args_->rf.ul_freq > 0) {
        cfg.ul_freq_hz = args_->rf.ul_freq;
        ERROR("Force UL freq for cell PCI=%d to %f MHz", cfg.pci, cfg.ul_freq_hz / 1e6f);
      }
    } else {
      // If more than one cell is defined, single EARFCN or DL freq will be ignored
      if (args_->enb.dl_earfcn > 0 || args_->rf.dl_freq > 0) {
        INFO("Multiple cells defined in rr.conf. Ignoring single EARFCN and/or frequency config.");
      }
    }

    // set config for RRC's base cell
    rrc_cfg_->cell = cell_cfg_;

    // Set S1AP related params from cell list
    args_->stack.s1ap.enb_id  = args_->enb.enb_id;
    args_->stack.s1ap.cell_id = rrc_cfg_->cell_list.at(0).cell_id;
    args_->stack.s1ap.tac     = rrc_cfg_->cell_list.at(0).tac;
  }

  // Create dedicated cell configuration from RRC configuration
  for (auto it = rrc_cfg_->cell_list.begin(); it != rrc_cfg_->cell_list.end(); ++it) {
    cell_cfg_t&    cfg          = *it;
    phy_cell_cfg_t phy_cell_cfg = {};
    phy_cell_cfg.cell           = cell_cfg_;
    phy_cell_cfg.cell.id        = cfg.pci;
    phy_cell_cfg.cell_id        = cfg.cell_id;
    phy_cell_cfg.root_seq_idx   = cfg.root_seq_idx;
    phy_cell_cfg.rf_port        = cfg.rf_port;
    phy_cell_cfg.gain_db        = cfg.tx_gain;
    phy_cell_cfg.num_ra_preambles =
        rrc_cfg_->sibs[1].sib2().rr_cfg_common.rach_cfg_common.preamb_info.nof_ra_preambs.to_number();

    if (cfg.dl_freq_hz > 0) {
      phy_cell_cfg.dl_freq_hz = cfg.dl_freq_hz;
    } else {
      phy_cell_cfg.dl_freq_hz = 1e6 * srsran_band_fd(cfg.dl_earfcn);
      if (phy_cell_cfg.dl_freq_hz == 0.0) {
        ERROR("Couldn't derive DL frequency for EARFCN=%d", cfg.dl_earfcn);
        return SRSRAN_ERROR;
      }
    }

    if (cfg.ul_freq_hz > 0) {
      phy_cell_cfg.ul_freq_hz = cfg.ul_freq_hz;
    } else {
      if (cfg.ul_earfcn == 0) {
        cfg.ul_earfcn = srsran_band_ul_earfcn(cfg.dl_earfcn);
      }
      phy_cell_cfg.ul_freq_hz = 1e6 * srsran_band_fu(cfg.ul_earfcn);
      if (phy_cell_cfg.ul_freq_hz == 0.0) {
        ERROR("Couldn't derive UL frequency for EARFCN=%d", cfg.ul_earfcn);
        return SRSRAN_ERROR;
      }
    }

    for (auto scell_it = cfg.scell_list.begin(); scell_it != cfg.scell_list.end();) {
      auto cell_it = std::find_if(rrc_cfg_->cell_list.begin(),
                                  rrc_cfg_->cell_list.end(),
                                  [scell_it](const cell_cfg_t& c) { return scell_it->cell_id == c.cell_id; });
      if (cell_it == rrc_cfg_->cell_list.end()) {
        ERROR("Scell with cell_id=0x%x is not present in rr.conf. Ignoring it.", scell_it->cell_id);
        scell_it = cfg.scell_list.erase(scell_it);
      } else if (cell_it->cell_id == cfg.cell_id) {
        ERROR("A cell cannot have an scell with the same cell_id=0x%x", cfg.cell_id);
        return SRSRAN_ERROR;
      } else {
        scell_it++;
      }
    }

    for (meas_cell_cfg_t& meas_cell : cfg.meas_cfg.meas_cells) {
      if (meas_cell.tac < 0) {
        // if meas cell TAC was not set, use current cell TAC.
        meas_cell.tac = cfg.tac;
      }
    }

    // Check if the enb cells PCIs won't lead to PSS detection issues
    auto is_pss_collision = [&cfg](const cell_cfg_t& c) {
      return c.pci % 3 == cfg.pci % 3 and c.dl_earfcn == cfg.dl_earfcn;
    };
    auto collision_it = std::find_if(it + 1, rrc_cfg_->cell_list.end(), is_pss_collision);
    if (collision_it != rrc_cfg_->cell_list.end()) {
      ERROR("The cells pci1=%d and pci2=%d will have the same PSS. Consider changing one of the cells' PCI values, "
            "otherwise a UE may fail to correctly detect and distinguish them",
            it->pci,
            collision_it->pci);
    }

    phy_cfg_->phy_cell_cfg.push_back(phy_cell_cfg);
  }

  if (args_->enb.transmission_mode == 1) {
    phy_cfg_->pdsch_cnfg.p_b                                    = 0; // Default TM1
    rrc_cfg_->sibs[1].sib2().rr_cfg_common.pdsch_cfg_common.p_b = 0;
  } else {
    phy_cfg_->pdsch_cnfg.p_b                                    = 1; // Default TM2,3,4
    rrc_cfg_->sibs[1].sib2().rr_cfg_common.pdsch_cfg_common.p_b = 1;
  }

  rrc_cfg_->inactivity_timeout_ms   = args_->general.rrc_inactivity_timer;
  uint32_t t310                     = rrc_cfg_->sibs[1].sib2().ue_timers_and_consts.t310.to_number();
  uint32_t t311                     = rrc_cfg_->sibs[1].sib2().ue_timers_and_consts.t311.to_number();
  uint32_t n310                     = rrc_cfg_->sibs[1].sib2().ue_timers_and_consts.n310.to_number();
  uint32_t min_rrc_inactivity_timer = t310 + t311 + n310 + 50;
  if (args_->general.rrc_inactivity_timer < min_rrc_inactivity_timer) {
    ERROR("rrc_inactivity_timer=%d is too low. Consider setting it to a value equal or above %d",
          args_->general.rrc_inactivity_timer,
          min_rrc_inactivity_timer);
  }
  rrc_cfg_->enable_mbsfn = args_->stack.embms.enable;
  rrc_cfg_->mbms_mcs     = args_->stack.embms.mcs;

  // Check number of control symbols
  if (args_->stack.mac.sched.min_nof_ctrl_symbols > args_->stack.mac.sched.max_nof_ctrl_symbols) {
    ERROR("Invalid minimum number of control symbols %d. Setting it to 1.",
          args_->stack.mac.sched.min_nof_ctrl_symbols);
    args_->stack.mac.sched.min_nof_ctrl_symbols = 1;
  }

  // Parse EEA preference list
  std::vector<std::string> eea_pref_list;
  boost::split(eea_pref_list, args_->general.eea_pref_list, boost::is_any_of(","));
  int i = 0;
  for (auto it = eea_pref_list.begin(); it != eea_pref_list.end() && i < srsran::CIPHERING_ALGORITHM_ID_N_ITEMS; it++) {
    boost::trim_left(*it);
    if ((*it) == "EEA0") {
      rrc_cfg_->eea_preference_list[i] = srsran::CIPHERING_ALGORITHM_ID_EEA0;
      i++;
    } else if ((*it) == "EEA1") {
      rrc_cfg_->eea_preference_list[i] = srsran::CIPHERING_ALGORITHM_ID_128_EEA1;
      i++;
    } else if ((*it) == "EEA2") {
      rrc_cfg_->eea_preference_list[i] = srsran::CIPHERING_ALGORITHM_ID_128_EEA2;
      i++;
    } else if ((*it) == "EEA3") {
      rrc_cfg_->eea_preference_list[i] = srsran::CIPHERING_ALGORITHM_ID_128_EEA3;
      i++;
    } else {
      fprintf(stderr, "Failed to parse EEA prefence list %s \n", args_->general.eea_pref_list.c_str());
      return SRSRAN_ERROR;
    }
  }

  // Parse EIA preference list
  std::vector<std::string> eia_pref_list;
  boost::split(eia_pref_list, args_->general.eia_pref_list, boost::is_any_of(","));
  i = 0;
  for (auto it = eia_pref_list.begin(); it != eia_pref_list.end() && i < srsran::INTEGRITY_ALGORITHM_ID_N_ITEMS; it++) {
    boost::trim_left(*it);
    if ((*it) == "EIA0") {
      rrc_cfg_->eia_preference_list[i] = srsran::INTEGRITY_ALGORITHM_ID_EIA0;
      i++;
    } else if ((*it) == "EIA1") {
      rrc_cfg_->eia_preference_list[i] = srsran::INTEGRITY_ALGORITHM_ID_128_EIA1;
      i++;
    } else if ((*it) == "EIA2") {
      rrc_cfg_->eia_preference_list[i] = srsran::INTEGRITY_ALGORITHM_ID_128_EIA2;
      i++;
    } else if ((*it) == "EIA3") {
      rrc_cfg_->eia_preference_list[i] = srsran::INTEGRITY_ALGORITHM_ID_128_EIA3;
      i++;
    } else {
      fprintf(stderr, "Failed to parse EIA prefence list %s \n", args_->general.eia_pref_list.c_str());
      return SRSRAN_ERROR;
    }
  }

  // Check PUCCH and PRACH configuration
  uint32_t nrb_pucch =
      std::max(rrc_cfg_->sr_cfg.nof_prb, (uint32_t)rrc_cfg_->sibs[1].sib2().rr_cfg_common.pucch_cfg_common.nrb_cqi);
  uint32_t prach_freq_offset = rrc_cfg_->sibs[1].sib2().rr_cfg_common.prach_cfg.prach_cfg_info.prach_freq_offset;
  if (args_->enb.n_prb > 6) {
    uint32_t lower_bound = nrb_pucch;
    uint32_t upper_bound = args_->enb.n_prb - nrb_pucch;
    if (prach_freq_offset + 6 > upper_bound or prach_freq_offset < lower_bound) {
      fprintf(stderr,
              "ERROR: Invalid PRACH configuration - prach_freq_offset=%d collides with PUCCH.\n",
              prach_freq_offset);
      fprintf(stderr,
              "       Consider changing \"prach_freq_offset\" in sib.conf to a value between %d and %d.\n",
              lower_bound,
              upper_bound);
      return SRSRAN_ERROR;
    }
  } else { // 6 PRB case
    if (prach_freq_offset + 6 > args_->enb.n_prb) {
      fprintf(stderr,
              "ERROR: Invalid PRACH configuration - prach=(%d, %d) does not fit into the eNB PRBs=(0, %d).\n",
              prach_freq_offset,
              prach_freq_offset + 6,
              args_->enb.n_prb);
      fprintf(
          stderr,
          "       Consider changing the \"prach_freq_offset\" value to 0 in the sib.conf file when using 6 PRBs.\n");
      // patch PRACH config for PHY and in RRC for SIB2
      rrc_cfg_->sibs[1].sib2().rr_cfg_common.prach_cfg.prach_cfg_info.prach_freq_offset = 0;
      phy_cfg_->prach_cnfg.prach_cfg_info.prach_freq_offset                             = 0;
    }
  }

  // Patch certain args that are not exposed yet
  args_->rf.nof_antennas = args_->enb.nof_ports;

  // MAC needs to know the cell bandwidth to dimension softbuffers
  args_->stack.mac.nof_prb = args_->enb.n_prb;

  // RRC needs eNB id for SIB1 packing
  rrc_cfg_->enb_id = args_->stack.s1ap.enb_id;

  // Set max number of KOs
  rrc_cfg_->max_mac_dl_kos       = args_->general.max_mac_dl_kos;
  rrc_cfg_->max_mac_ul_kos       = args_->general.max_mac_ul_kos;
  rrc_cfg_->rlf_release_timer_ms = args_->general.rlf_release_timer_ms;

  // Set sync queue capacity to 1 for ZMQ
  if (args_->rf.device_name == "zmq") {
    srslog::fetch_basic_logger("ENB").info("Using sync queue size of one for ZMQ based radio.");
    args_->stack.sync_queue_size = 1;
  } else {
    // use default size
    args_->stack.sync_queue_size = MULTIQUEUE_DEFAULT_CAPACITY;
  }

  return SRSRAN_SUCCESS;
}

/**
 * @brief Set the derived args for the NR RRC and PHY config
 *
 * Mainly configures the RRC parameter based on the arguments and config files
 * read. Since for NSA we are still using a commong PHY between EUTRA and NR
 * the PHY configuration is also updated accordingly.
 *
 * @param args_
 * @param nr_rrc_cfg_
 * @param phy_cfg_
 * @return int
 */
int set_derived_args_nr(all_args_t* args_, rrc_nr_cfg_t* rrc_nr_cfg_, phy_cfg_t* phy_cfg_)
{
  // Use helper class to derive NR carrier parameters
  srsran::srsran_band_helper band_helper;

  // we only support one NR cell
  if (rrc_nr_cfg_->cell_list.size() > 1) {
    ERROR("Only a single NR cell supported.");
    return SRSRAN_ERROR;
  }

  rrc_nr_cfg_->inactivity_timeout_ms = args_->general.rrc_inactivity_timer;

  // Create NR dedicated cell configuration from RRC configuration
  for (auto& cfg : rrc_nr_cfg_->cell_list) {
    cfg.phy_cell.carrier.max_mimo_layers = args_->enb.nof_ports;

    // NR cells have the same bandwidth as EUTRA cells, adjust PRB sizes
    switch (args_->enb.n_prb) {
      case 25:
        cfg.phy_cell.carrier.nof_prb = 25;
        break;
      case 50:
        cfg.phy_cell.carrier.nof_prb = 52;
        break;
      case 100:
        cfg.phy_cell.carrier.nof_prb = 106;
        break;
      default:
        ERROR("The only accepted number of PRB is: 25, 50, 100");
        return SRSRAN_ERROR;
    }

    // phy_cell_cfg.root_seq_idx = cfg.root_seq_idx;

    // PDSCH
    cfg.pdsch_rs_power = phy_cfg_->pdsch_cnfg.ref_sig_pwr;
  }
  rrc_nr_cfg_->enb_id = args_->enb.enb_id;
  rrc_nr_cfg_->mcc    = args_->stack.s1ap.mcc;
  rrc_nr_cfg_->mnc    = args_->stack.s1ap.mnc;

  // Derive cross-dependent cell params
  if (set_derived_nr_rrc_params(*rrc_nr_cfg_) != SRSRAN_SUCCESS) {
    ERROR("Failed to derive NR cell params.");
    return SRSRAN_ERROR;
  }

  // Update PHY with RRC cell configs
  for (auto& cfg : rrc_nr_cfg_->cell_list) {
    phy_cfg_->phy_cell_cfg_nr.push_back(cfg.phy_cell);
  }

  // MAC-NR PCAP options
  args_->nr_stack.mac.pcap.enable = args_->stack.mac_pcap.enable;
  args_->nr_stack.log             = args_->stack.log;

  // Sanity check for unsupported/untested configuration
  for (auto& cfg : rrc_nr_cfg_->cell_list) {
    if (cfg.phy_cell.carrier.nof_prb != 52) {
      ERROR("Only 10 MHz bandwidth supported.");
      return SRSRAN_ERROR;
    }
  }

  return SRSRAN_SUCCESS;
}

} // namespace enb_conf_sections

namespace sib_sections {

int parse_sib1(std::string filename, sib_type1_s* data)
{
  parser::section sib1("sib1");

  sib1.add_field(make_asn1_enum_str_parser("intra_freq_reselection", &data->cell_access_related_info.intra_freq_resel));
  sib1.add_field(new parser::field<int8>("q_rx_lev_min", &data->cell_sel_info.q_rx_lev_min));
  sib1.add_field(new parser::field<int8>("p_max", &data->p_max, &data->p_max_present));
  sib1.add_field(make_asn1_enum_str_parser("cell_barred", &data->cell_access_related_info.cell_barred));
  sib1.add_field(make_asn1_enum_number_parser("si_window_length", &data->si_win_len));
  sib1.add_field(new parser::field<uint8_t>("system_info_value_tag", &data->sys_info_value_tag));

  // additional_plmns subsection uses a custom field class
  parser::section additional_plmns("additional_plmns");
  sib1.add_subsection(&additional_plmns);
  bool dummy_bool = true;
  additional_plmns.set_optional(&dummy_bool);
  additional_plmns.add_field(new field_additional_plmns(&data->cell_access_related_info));

  // sched_info subsection uses a custom field class
  parser::section sched_info("sched_info");
  sib1.add_subsection(&sched_info);
  sched_info.add_field(new field_sched_info(data));

  // Run parser with single section
  return parser::parse_section(std::move(filename), &sib1);
}

int parse_sib2(std::string filename, sib_type2_s* data)
{
  parser::section sib2("sib2");

  sib2.add_field(make_asn1_enum_str_parser("time_alignment_timer", &data->time_align_timer_common));
  sib2.add_field(new mbsfn_sf_cfg_list_parser(&data->mbsfn_sf_cfg_list, &data->mbsfn_sf_cfg_list_present));

  parser::section freqinfo("freqInfo");
  sib2.add_subsection(&freqinfo);
  freqinfo.add_field(new parser::field<uint8>("additional_spectrum_emission", &data->freq_info.add_spec_emission));
  freqinfo.add_field(new parser::field<bool>("ul_carrier_freq_present", &data->freq_info.ul_carrier_freq_present));
  freqinfo.add_field(new parser::field<bool>("ul_bw_present", &data->freq_info.ul_bw_present));

  // AC barring configuration
  parser::section acbarring("ac_barring");
  sib2.add_subsection(&acbarring);
  acbarring.set_optional(&data->ac_barr_info_present);

  acbarring.add_field(new parser::field<bool>("ac_barring_for_emergency", &data->ac_barr_info.ac_barr_for_emergency));

  parser::section acbarring_signalling("ac_barring_for_mo_signalling");
  acbarring.add_subsection(&acbarring_signalling);
  acbarring_signalling.set_optional(&data->ac_barr_info.ac_barr_for_mo_sig_present);

  acbarring_signalling.add_field(
      make_asn1_enum_number_str_parser("factor", &data->ac_barr_info.ac_barr_for_mo_sig.ac_barr_factor));
  acbarring_signalling.add_field(
      make_asn1_enum_number_parser("time", &data->ac_barr_info.ac_barr_for_mo_sig.ac_barr_time));
  acbarring_signalling.add_field(make_asn1_bitstring_number_parser(
      "for_special_ac", &data->ac_barr_info.ac_barr_for_mo_sig.ac_barr_for_special_ac));

  parser::section acbarring_data("ac_barring_for_mo_data");
  acbarring.add_subsection(&acbarring_data);
  acbarring_data.set_optional(&data->ac_barr_info.ac_barr_for_mo_data_present);

  acbarring_data.add_field(
      make_asn1_enum_number_str_parser("factor", &data->ac_barr_info.ac_barr_for_mo_data.ac_barr_factor));
  acbarring_data.add_field(make_asn1_enum_number_parser("time", &data->ac_barr_info.ac_barr_for_mo_data.ac_barr_time));
  acbarring_data.add_field(make_asn1_bitstring_number_parser(
      "for_special_ac", &data->ac_barr_info.ac_barr_for_mo_data.ac_barr_for_special_ac));

  // UE timers and constants
  parser::section uetimers("ue_timers_and_constants");
  sib2.add_subsection(&uetimers);
  uetimers.add_field(make_asn1_enum_number_parser("t300", &data->ue_timers_and_consts.t300));
  uetimers.add_field(make_asn1_enum_number_parser("t301", &data->ue_timers_and_consts.t301));
  uetimers.add_field(make_asn1_enum_number_parser("t310", &data->ue_timers_and_consts.t310));
  uetimers.add_field(make_asn1_enum_number_parser("n310", &data->ue_timers_and_consts.n310));
  uetimers.add_field(make_asn1_enum_number_parser("t311", &data->ue_timers_and_consts.t311));
  uetimers.add_field(make_asn1_enum_number_parser("n311", &data->ue_timers_and_consts.n311));

  // Radio-resource configuration section
  parser::section rr_config("rr_config_common_sib");
  sib2.add_subsection(&rr_config);
  rr_cfg_common_sib_s* rr_cfg_common = &data->rr_cfg_common;

  rr_config.add_field(make_asn1_enum_str_parser("ul_cp_length", &rr_cfg_common->ul_cp_len));

  // RACH configuration
  parser::section rach_cnfg("rach_cnfg");
  rr_config.add_subsection(&rach_cnfg);

  rach_cnfg.add_field(
      make_asn1_enum_number_parser("num_ra_preambles", &rr_cfg_common->rach_cfg_common.preamb_info.nof_ra_preambs));
  rach_cnfg.add_field(make_asn1_enum_number_parser(
      "preamble_init_rx_target_pwr", &rr_cfg_common->rach_cfg_common.pwr_ramp_params.preamb_init_rx_target_pwr));
  rach_cnfg.add_field(
      make_asn1_enum_number_parser("pwr_ramping_step", &rr_cfg_common->rach_cfg_common.pwr_ramp_params.pwr_ramp_step));
  rach_cnfg.add_field(make_asn1_enum_number_parser(
      "preamble_trans_max", &rr_cfg_common->rach_cfg_common.ra_supervision_info.preamb_trans_max));
  rach_cnfg.add_field(make_asn1_enum_number_parser(
      "ra_resp_win_size", &rr_cfg_common->rach_cfg_common.ra_supervision_info.ra_resp_win_size));
  rach_cnfg.add_field(make_asn1_enum_number_parser(
      "mac_con_res_timer", &rr_cfg_common->rach_cfg_common.ra_supervision_info.mac_contention_resolution_timer));
  rach_cnfg.add_field(new parser::field<uint8>("max_harq_msg3_tx", &rr_cfg_common->rach_cfg_common.max_harq_msg3_tx));

  parser::section groupa_cnfg("preambles_group_a_cnfg");
  rach_cnfg.add_subsection(&groupa_cnfg);
  groupa_cnfg.set_optional(&rr_cfg_common->rach_cfg_common.preamb_info.preambs_group_a_cfg_present);
  rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_* group_acfg =
      &rr_cfg_common->rach_cfg_common.preamb_info.preambs_group_a_cfg;
  groupa_cnfg.add_field(make_asn1_enum_number_parser("size_of_ra", &group_acfg->size_of_ra_preambs_group_a));
  groupa_cnfg.add_field(make_asn1_enum_number_parser("msg_size", &group_acfg->msg_size_group_a));
  groupa_cnfg.add_field(make_asn1_enum_number_parser("msg_pwr_offset_group_b", &group_acfg->msg_pwr_offset_group_b));
  //  rr_cfg_common->rach_cfg_common.preamb_info.nof_ra_preambs = TODO: ???

  // BCCH configuration
  parser::section bcch_cnfg("bcch_cnfg");
  rr_config.add_subsection(&bcch_cnfg);
  bcch_cnfg.add_field(
      make_asn1_enum_number_parser("modification_period_coeff", &rr_cfg_common->bcch_cfg.mod_period_coeff));

  // PCCH configuration
  parser::section pcch_cnfg("pcch_cnfg");
  rr_config.add_subsection(&pcch_cnfg);
  pcch_cnfg.add_field(
      make_asn1_enum_number_parser("default_paging_cycle", &rr_cfg_common->pcch_cfg.default_paging_cycle));
  pcch_cnfg.add_field(make_asn1_enum_number_str_parser("nB", &rr_cfg_common->pcch_cfg.nb));

  // PRACH configuration
  parser::section prach_cnfg("prach_cnfg");
  rr_config.add_subsection(&prach_cnfg);
  prach_cnfg.add_field(new parser::field<uint16_t>("root_sequence_index", &rr_cfg_common->prach_cfg.root_seq_idx));
  parser::section prach_cnfg_info("prach_cnfg_info");
  prach_cnfg.add_subsection(&prach_cnfg_info);
  prach_cnfg_info.add_field(
      new parser::field<bool>("high_speed_flag", &rr_cfg_common->prach_cfg.prach_cfg_info.high_speed_flag));
  prach_cnfg_info.add_field(
      new parser::field<uint8_t>("prach_config_index", &rr_cfg_common->prach_cfg.prach_cfg_info.prach_cfg_idx));
  prach_cnfg_info.add_field(
      new parser::field<uint8_t>("prach_freq_offset", &rr_cfg_common->prach_cfg.prach_cfg_info.prach_freq_offset));
  prach_cnfg_info.add_field(new parser::field<uint8_t>(
      "zero_correlation_zone_config", &rr_cfg_common->prach_cfg.prach_cfg_info.zero_correlation_zone_cfg));

  // PDSCH configuration
  parser::section pdsch_cnfg("pdsch_cnfg");
  rr_config.add_subsection(&pdsch_cnfg);
  pdsch_cnfg.add_field(new parser::field<uint8_t>("p_b", &rr_cfg_common->pdsch_cfg_common.p_b));
  pdsch_cnfg.add_field(new parser::field<int8_t>("rs_power", &rr_cfg_common->pdsch_cfg_common.ref_sig_pwr));

  // PUSCH configuration
  parser::section pusch_cnfg("pusch_cnfg");
  rr_config.add_subsection(&pusch_cnfg);
  pusch_cnfg.add_field(new parser::field<uint8>("n_sb", &rr_cfg_common->pusch_cfg_common.pusch_cfg_basic.n_sb));
  pusch_cnfg.add_field(
      make_asn1_enum_str_parser("hopping_mode", &rr_cfg_common->pusch_cfg_common.pusch_cfg_basic.hop_mode));
  pusch_cnfg.add_field(new parser::field<uint8>("pusch_hopping_offset",
                                                &rr_cfg_common->pusch_cfg_common.pusch_cfg_basic.pusch_hop_offset));
  pusch_cnfg.add_field(
      new parser::field<bool>("enable_64_qam", &rr_cfg_common->pusch_cfg_common.pusch_cfg_basic.enable64_qam));

  // PUSCH-ULRS configuration
  parser::section ulrs_cnfg("ul_rs");
  pusch_cnfg.add_subsection(&ulrs_cnfg);
  ulrs_cnfg.add_field(
      new parser::field<uint8>("cyclic_shift", &rr_cfg_common->pusch_cfg_common.ul_ref_sigs_pusch.cyclic_shift));
  ulrs_cnfg.add_field(new parser::field<uint8>("group_assignment_pusch",
                                               &rr_cfg_common->pusch_cfg_common.ul_ref_sigs_pusch.group_assign_pusch));
  ulrs_cnfg.add_field(new parser::field<bool>("group_hopping_enabled",
                                              &rr_cfg_common->pusch_cfg_common.ul_ref_sigs_pusch.group_hop_enabled));
  ulrs_cnfg.add_field(new parser::field<bool>("sequence_hopping_enabled",
                                              &rr_cfg_common->pusch_cfg_common.ul_ref_sigs_pusch.seq_hop_enabled));

  // PUCCH configuration
  parser::section pucch_cnfg("pucch_cnfg");
  rr_config.add_subsection(&pucch_cnfg);
  pucch_cnfg.add_field(
      make_asn1_enum_number_parser("delta_pucch_shift", &rr_cfg_common->pucch_cfg_common.delta_pucch_shift));
  pucch_cnfg.add_field(new parser::field<uint8>("n_rb_cqi", &rr_cfg_common->pucch_cfg_common.nrb_cqi));
  pucch_cnfg.add_field(new parser::field<uint8>("n_cs_an", &rr_cfg_common->pucch_cfg_common.ncs_an));
  pucch_cnfg.add_field(new parser::field<uint16>("n1_pucch_an", &rr_cfg_common->pucch_cfg_common.n1_pucch_an));

  // UL PWR Ctrl configuration
  parser::section ul_pwr_ctrl("ul_pwr_ctrl");
  rr_config.add_subsection(&ul_pwr_ctrl);
  ul_pwr_ctrl.add_field(
      new parser::field<int8>("p0_nominal_pusch", &rr_cfg_common->ul_pwr_ctrl_common.p0_nominal_pusch));
  ul_pwr_ctrl.add_field(make_asn1_enum_number_parser("alpha", &rr_cfg_common->ul_pwr_ctrl_common.alpha));
  ul_pwr_ctrl.add_field(
      new parser::field<int8>("p0_nominal_pucch", &rr_cfg_common->ul_pwr_ctrl_common.p0_nominal_pucch));
  ul_pwr_ctrl.add_field(
      new parser::field<int8>("delta_preamble_msg3", &rr_cfg_common->ul_pwr_ctrl_common.delta_preamb_msg3));

  // Delta Flist PUCCH
  parser::section delta_flist("delta_flist_pucch");
  ul_pwr_ctrl.add_subsection(&delta_flist);
  delta_flist.add_field(make_asn1_enum_number_parser(
      "format_1", &rr_cfg_common->ul_pwr_ctrl_common.delta_flist_pucch.delta_f_pucch_format1));
  delta_flist.add_field(make_asn1_enum_number_parser(
      "format_1b", &rr_cfg_common->ul_pwr_ctrl_common.delta_flist_pucch.delta_f_pucch_format1b));
  delta_flist.add_field(make_asn1_enum_number_parser(
      "format_2", &rr_cfg_common->ul_pwr_ctrl_common.delta_flist_pucch.delta_f_pucch_format2));
  delta_flist.add_field(make_asn1_enum_number_parser(
      "format_2a", &rr_cfg_common->ul_pwr_ctrl_common.delta_flist_pucch.delta_f_pucch_format2a));
  delta_flist.add_field(make_asn1_enum_number_parser(
      "format_2b", &rr_cfg_common->ul_pwr_ctrl_common.delta_flist_pucch.delta_f_pucch_format2b));

  // Run parser with single section
  return parser::parse_section(std::move(filename), &sib2);
}

int parse_sib3(std::string filename, sib_type3_s* data)
{
  parser::section sib3("sib3");

  // CellReselectionInfoCommon
  parser::section resel_common("cell_reselection_common");
  sib3.add_subsection(&resel_common);

  resel_common.add_field(make_asn1_enum_number_parser("q_hyst", &data->cell_resel_info_common.q_hyst));
  parser::section speed_resel("speed_state_resel_params");
  speed_resel.set_optional(&data->cell_resel_info_common.speed_state_resel_pars_present);
  resel_common.add_subsection(&speed_resel);
  sib_type3_s::cell_resel_info_common_s_::speed_state_resel_pars_s_* resel_pars =
      &data->cell_resel_info_common.speed_state_resel_pars;

  parser::section q_hyst_sf("q_hyst_sf");
  speed_resel.add_subsection(&q_hyst_sf);
  q_hyst_sf.add_field(make_asn1_enum_number_parser("medium", &resel_pars->q_hyst_sf.sf_medium));
  q_hyst_sf.add_field(make_asn1_enum_number_parser("high", &resel_pars->q_hyst_sf.sf_high));

  parser::section mob_params("mobility_state_params");
  speed_resel.add_subsection(&mob_params);
  mob_params.add_field(make_asn1_enum_number_parser("t_eval", &resel_pars->mob_state_params.t_eval));
  mob_params.add_field(make_asn1_enum_number_parser("t_hyst_normal", &resel_pars->mob_state_params.t_hyst_normal));
  mob_params.add_field(
      new parser::field<uint8>("n_cell_change_medium", &resel_pars->mob_state_params.n_cell_change_medium));
  mob_params.add_field(
      new parser::field<uint8>("n_cell_change_high", &resel_pars->mob_state_params.n_cell_change_high));

  // CellReselectionServingFreqInfo
  parser::section resel_serving("cell_reselection_serving");
  sib3.add_subsection(&resel_serving);
  sib_type3_s::cell_resel_serving_freq_info_s_* freqinfo = &data->cell_resel_serving_freq_info;

  resel_serving.add_field(new parser::field<uint8>(
      "s_non_intra_search", &freqinfo->s_non_intra_search, &freqinfo->s_non_intra_search_present));
  resel_serving.add_field(new parser::field<uint8>("thresh_serving_low", &freqinfo->thresh_serving_low));
  resel_serving.add_field(new parser::field<uint8>("cell_resel_prio", &freqinfo->cell_resel_prio));

  // intraFreqCellReselectionInfo
  parser::section intra_freq("intra_freq_reselection");
  sib3.add_subsection(&intra_freq);
  sib_type3_s::intra_freq_cell_resel_info_s_* intrafreq = &data->intra_freq_cell_resel_info;

  intra_freq.add_field(new parser::field<int8_t>("q_rx_lev_min", &intrafreq->q_rx_lev_min));
  intra_freq.add_field(new parser::field<int8>("p_max", &intrafreq->p_max, &intrafreq->p_max_present));
  intra_freq.add_field(
      new parser::field<uint8>("s_intra_search", &intrafreq->s_intra_search, &intrafreq->s_intra_search_present));
  intra_freq.add_field(make_asn1_enum_number_parser(
      "allowed_meas_bw", &intrafreq->allowed_meas_bw, &intrafreq->allowed_meas_bw_present));
  intra_freq.add_field(new parser::field<bool>("presence_ant_port_1", &intrafreq->presence_ant_port1));
  intra_freq.add_field(make_asn1_bitstring_number_parser("neigh_cell_cnfg", &intrafreq->neigh_cell_cfg));
  intra_freq.add_field(new parser::field<uint8>("t_resel_eutra", &intrafreq->t_resel_eutra));

  parser::section t_resel_eutra_sf("t_resel_eutra_sf");
  intra_freq.add_subsection(&t_resel_eutra_sf);
  t_resel_eutra_sf.set_optional(&intrafreq->t_resel_eutra_sf_present);

  t_resel_eutra_sf.add_field(make_asn1_enum_number_str_parser("sf_medium", &intrafreq->t_resel_eutra_sf.sf_medium));
  t_resel_eutra_sf.add_field(make_asn1_enum_number_str_parser("sf_high", &intrafreq->t_resel_eutra_sf.sf_high));

  // Run parser with single section
  return parser::parse_section(std::move(filename), &sib3);
}

int parse_sib4(std::string filename, sib_type4_s* data)
{
  parser::section sib4("sib4");

  // csg-PhysCellIdRange
  parser::section csg_range("csg_phys_cell_id_range");
  sib4.add_subsection(&csg_range);
  csg_range.set_optional(&data->csg_pci_range_present);
  csg_range.add_field(make_asn1_enum_number_parser("range", &data->csg_pci_range.range));
  csg_range.add_field(new parser::field<uint16>("start", &data->csg_pci_range.start));

  // intraFreqNeighCellList
  parser::section intra_neigh("intra_freq_neigh_cell_list");
  sib4.add_subsection(&intra_neigh);
  bool dummy_bool = false;
  intra_neigh.set_optional(&dummy_bool);
  intra_neigh.add_field(new field_intra_neigh_cell_list(data));

  // intraFreqBlackCellList
  parser::section intra_black("intra_freq_black_cell_list");
  sib4.add_subsection(&intra_black);
  intra_black.set_optional(&dummy_bool);
  intra_black.add_field(new field_intra_black_cell_list(data));

  // Run parser with single section
  return parser::parse_section(std::move(filename), &sib4);
}

int parse_sib5(std::string filename, sib_type5_s* data)
{
  parser::section sib5("sib5");

  // interFreqCarrierFreqList
  parser::section inter_freq_carrier_freq_list("inter_freq_carrier_freq_list");
  sib5.add_subsection(&inter_freq_carrier_freq_list);
  bool dummy_bool = false;
  inter_freq_carrier_freq_list.set_optional(&dummy_bool);
  inter_freq_carrier_freq_list.add_field(new field_inter_freq_carrier_freq_list(data));

  return parser::parse_section(std::move(filename), &sib5);
}

int parse_sib6(std::string filename, sib_type6_s* data)
{
  parser::section sib6("sib6");

  // t-ReselectionUTRA
  sib6.add_field(new parser::field<uint8>("t_resel_utra", &data->t_resel_utra));

  // t-ReselectionUTRA-SF
  parser::section t_resel_utra_sf("t_resel_utra_sf");
  sib6.add_subsection(&t_resel_utra_sf);
  t_resel_utra_sf.set_optional(&data->t_resel_utra_sf_present);
  t_resel_utra_sf.add_field(make_asn1_enum_number_str_parser("sf_medium", &data->t_resel_utra_sf.sf_medium));
  t_resel_utra_sf.add_field(make_asn1_enum_number_str_parser("sf_high", &data->t_resel_utra_sf.sf_high));

  // carrierFreqListUTRA-FDD
  parser::section carrier_freq_list_utra_fdd("carrier_freq_list_utra_fdd");
  sib6.add_subsection(&carrier_freq_list_utra_fdd);
  bool dummy_bool = false;
  carrier_freq_list_utra_fdd.set_optional(&dummy_bool);
  carrier_freq_list_utra_fdd.add_field(new field_carrier_freq_list_utra_fdd(data));

  // carrierFreqListUTRA-TDD
  parser::section carrier_freq_list_utra_tdd("carrier_freq_list_utra_tdd");
  sib6.add_subsection(&carrier_freq_list_utra_tdd);
  carrier_freq_list_utra_tdd.set_optional(&dummy_bool);
  carrier_freq_list_utra_tdd.add_field(new field_carrier_freq_list_utra_tdd(data));

  return parser::parse_section(std::move(filename), &sib6);
}

int parse_sib7(std::string filename, sib_type7_s* data)
{
  parser::section sib7("sib7");

  sib7.add_field(new parser::field<uint8>("t_resel_geran", &data->t_resel_geran));

  parser::section t_resel_geran_sf("t_resel_geran_sf");
  sib7.add_subsection(&t_resel_geran_sf);
  t_resel_geran_sf.set_optional(&data->t_resel_geran_sf_present);

  t_resel_geran_sf.add_field(make_asn1_enum_number_str_parser("sf_medium", &data->t_resel_geran_sf.sf_medium));
  t_resel_geran_sf.add_field(make_asn1_enum_number_str_parser("sf_high", &data->t_resel_geran_sf.sf_high));

  data->carrier_freqs_info_list_present = true;
  parser::section geran_neigh("carrier_freqs_info_list");
  sib7.add_subsection(&geran_neigh);

  bool dummy_bool = false;
  geran_neigh.set_optional(&dummy_bool);
  geran_neigh.add_field(new field_carrier_freqs_info_list(data));

  return parser::parse_section(std::move(filename), &sib7);
}

int parse_sib9(std::string filename, sib_type9_s* data)
{
  parser::section sib9("sib9");

  bool        name_enabled, hex_enabled;
  std::string hnb_name, hex_value;

  sib9.add_field(new parser::field<std::string>("hnb_name", &hnb_name, &name_enabled));
  sib9.add_field(new parser::field<std::string>("hex_value", &hex_value, &hex_enabled));

  // Run parser with single section
  if (!parser::parse_section(std::move(filename), &sib9)) {
    data->hnb_name_present = true;
    if (name_enabled) {
      data->hnb_name.resize(SRSRAN_MIN((uint32_t)hnb_name.size(), 48));
      memcpy(data->hnb_name.data(), hnb_name.c_str(), data->hnb_name.size());
    } else if (hex_enabled) {
      if (hex_value.size() > 48) {
        hex_value.resize(48);
      }
      data->hnb_name.from_string(hex_value);
    } else {
      data->hnb_name_present = false;
    }
    return 0;
  } else {
    return SRSRAN_ERROR;
  }
}

int parse_sib13(std::string filename, sib_type13_r9_s* data)
{
  parser::section sib13("sib13");

  sib13.add_field(make_asn1_seqof_size_parser("mbsfn_area_info_list_size", &data->mbsfn_area_info_list_r9));

  parser::section mbsfn_notification_config("mbsfn_notification_config");
  sib13.add_subsection(&mbsfn_notification_config);

  mbsfn_notification_config.add_field(
      make_asn1_enum_str_parser("mbsfn_notification_repetition_coeff", &data->notif_cfg_r9.notif_repeat_coeff_r9));

  mbsfn_notification_config.add_field(
      new parser::field<uint8>("mbsfn_notification_offset", &data->notif_cfg_r9.notif_offset_r9));

  mbsfn_notification_config.add_field(
      new parser::field<uint8>("mbsfn_notification_sf_index", &data->notif_cfg_r9.notif_sf_idx_r9));

  sib13.add_field(new mbsfn_area_info_list_parser(&data->mbsfn_area_info_list_r9, nullptr));

  return parser::parse_section(std::move(filename), &sib13);
}

int parse_sibs(all_args_t* args_, rrc_cfg_t* rrc_cfg_, srsenb::phy_cfg_t* phy_config_common)
{
  // TODO: Leave 0 blank for now
  sib_type2_s*     sib2  = &rrc_cfg_->sibs[1].set_sib2();
  sib_type3_s*     sib3  = &rrc_cfg_->sibs[2].set_sib3();
  sib_type4_s*     sib4  = &rrc_cfg_->sibs[3].set_sib4();
  sib_type5_s*     sib5  = &rrc_cfg_->sibs[4].set_sib5();
  sib_type6_s*     sib6  = &rrc_cfg_->sibs[5].set_sib6();
  sib_type7_s*     sib7  = &rrc_cfg_->sibs[6].set_sib7();
  sib_type9_s*     sib9  = &rrc_cfg_->sibs[8].set_sib9();
  sib_type13_r9_s* sib13 = &rrc_cfg_->sibs[12].set_sib13_v920();

  sib_type1_s* sib1 = &rrc_cfg_->sib1;
  if (sib_sections::parse_sib1(args_->enb_files.sib_config, sib1) != SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  // Fill rest of data from enb config
  std::string mnc_str;
  if (not srsran::mnc_to_string(args_->stack.s1ap.mnc, &mnc_str)) {
    ERROR("The provided mnc=%d is not valid", args_->stack.s1ap.mnc);
    return SRSRAN_ERROR;
  }
  std::string mcc_str;
  if (not srsran::mcc_to_string(args_->stack.s1ap.mcc, &mcc_str)) {
    ERROR("The provided mnc=%d is not valid", args_->stack.s1ap.mcc);
    return SRSRAN_ERROR;
  }
  sib_type1_s::cell_access_related_info_s_* cell_access = &sib1->cell_access_related_info;
  // In case additional PLMNs were given, resizing will remove them
  if (cell_access->plmn_id_list.size() == 0) {
    cell_access->plmn_id_list.resize(1);
  }
  srsran::plmn_id_t plmn;
  if (plmn.from_string(mcc_str + mnc_str) == SRSRAN_ERROR) {
    ERROR("Could not convert %s to a plmn_id", (mcc_str + mnc_str).c_str());
    return SRSRAN_ERROR;
  }
  srsran::to_asn1(&cell_access->plmn_id_list[0].plmn_id, plmn);
  cell_access->plmn_id_list[0].cell_reserved_for_oper = plmn_id_info_s::cell_reserved_for_oper_e_::not_reserved;
  sib1->cell_sel_info.q_rx_lev_min_offset             = 0;

  // Generate SIB2
  if (sib_sections::parse_sib2(args_->enb_files.sib_config, sib2) != SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  // SRS not yet supported
  sib2->rr_cfg_common.srs_ul_cfg_common.set(srs_ul_cfg_common_c::types::release);
  if (sib2->freq_info.ul_bw_present) {
    asn1::number_to_enum(sib2->freq_info.ul_bw, args_->enb.n_prb);
  }
  // UL carrier freq is patched before packing the SIB for each CC

  // Update MBSFN list counter. Only 1 supported
  if (not args_->stack.embms.enable) {
    sib2->mbsfn_sf_cfg_list_present = false;
    sib2->mbsfn_sf_cfg_list.resize(0);
  } else {
    // verify SIB13 is available
    if (not sib_is_present(sib1->sched_info_list, sib_type_e::sib_type13_v920)) {
      fprintf(stderr, "SIB13 not present in sched_info.\n");
      return SRSRAN_ERROR;
    }
  }

  // Generate SIB3 if defined in mapping info
  if (sib_is_present(sib1->sched_info_list, sib_type_e::sib_type3)) {
    if (sib_sections::parse_sib3(args_->enb_files.sib_config, sib3) != SRSRAN_SUCCESS) {
      return SRSRAN_ERROR;
    }
  }

  // Generate SIB4 if defined in mapping info
  if (sib_is_present(sib1->sched_info_list, sib_type_e::sib_type4)) {
    if (sib_sections::parse_sib4(args_->enb_files.sib_config, sib4) != SRSRAN_SUCCESS) {
      return SRSRAN_ERROR;
    }
  }

  // Generate SIB5 if defined in mapping info
  if (sib_is_present(sib1->sched_info_list, sib_type_e::sib_type5)) {
    if (sib_sections::parse_sib5(args_->enb_files.sib_config, sib5) != SRSRAN_SUCCESS) {
      return SRSRAN_ERROR;
    }
  }

  // Generate SIB6 if defined in mapping info
  if (sib_is_present(sib1->sched_info_list, sib_type_e::sib_type6)) {
    if (sib_sections::parse_sib6(args_->enb_files.sib_config, sib6) != SRSRAN_SUCCESS) {
      return SRSRAN_ERROR;
    }
  }

  // Generate SIB7 if defined in mapping info
  if (sib_is_present(sib1->sched_info_list, sib_type_e::sib_type7)) {
    if (sib_sections::parse_sib7(args_->enb_files.sib_config, sib7) != SRSRAN_SUCCESS) {
      return SRSRAN_ERROR;
    }
  }

  // Generate SIB9 if defined in mapping info
  if (sib_is_present(sib1->sched_info_list, sib_type_e::sib_type9)) {
    if (sib_sections::parse_sib9(args_->enb_files.sib_config, sib9) != SRSRAN_SUCCESS) {
      return SRSRAN_ERROR;
    }
  }

  if (sib_is_present(sib1->sched_info_list, sib_type_e::sib_type13_v920)) {
    if (sib_sections::parse_sib13(args_->enb_files.sib_config, sib13) != SRSRAN_SUCCESS) {
      return SRSRAN_ERROR;
    }
  }

  // Copy PHY common configuration
  phy_config_common->prach_cnfg  = sib2->rr_cfg_common.prach_cfg;
  phy_config_common->pdsch_cnfg  = sib2->rr_cfg_common.pdsch_cfg_common;
  phy_config_common->pusch_cnfg  = sib2->rr_cfg_common.pusch_cfg_common;
  phy_config_common->pucch_cnfg  = sib2->rr_cfg_common.pucch_cfg_common;
  phy_config_common->srs_ul_cnfg = sib2->rr_cfg_common.srs_ul_cfg_common;

  return 0;
}

} // namespace sib_sections

namespace rb_sections {

int parse_rb(all_args_t* args_, rrc_cfg_t* rrc_cfg_, rrc_nr_cfg_t* rrc_nr_cfg_)
{
  parser::section srb1("srb1_config");
  bool            srb1_present = false;
  srb1.set_optional(&srb1_present);

  parser::section srb1_rlc_cfg("rlc_config");
  srb1.add_subsection(&srb1_rlc_cfg);
  srb1_rlc_cfg.add_field(new field_srb(rrc_cfg_->srb1_cfg));

  parser::section srb2("srb2_config");
  bool            srb2_present = false;
  srb2.set_optional(&srb2_present);

  parser::section srb2_rlc_cfg("rlc_config");
  srb2.add_subsection(&srb2_rlc_cfg);
  srb2_rlc_cfg.add_field(new field_srb(rrc_cfg_->srb2_cfg));

  parser::section qci("qci_config");
  qci.add_field(new field_qci(rrc_cfg_->qci_cfg));

  parser::section srb1_5g("srb1_5g_config");
  bool            srb1_5g_present = false;
  srb1_5g.set_optional(&srb1_5g_present);

  parser::section srb1_5g_rlc_cfg("rlc_config");
  srb1_5g.add_subsection(&srb1_5g_rlc_cfg);
  srb1_5g_rlc_cfg.add_field(new field_5g_srb(rrc_nr_cfg_->srb1_cfg));

  parser::section srb2_5g("srb2_5g_config");
  bool            srb2_5g_present = false;
  srb2_5g.set_optional(&srb2_5g_present);

  parser::section srb2_5g_rlc_cfg("rlc_config");
  srb2_5g.add_subsection(&srb2_5g_rlc_cfg);
  srb2_5g_rlc_cfg.add_field(new field_5g_srb(rrc_nr_cfg_->srb2_cfg));

  parser::section five_qi("five_qi_config");
  five_qi.add_field(new field_five_qi(rrc_nr_cfg_->five_qi_cfg));

  // Run parser with two sections
  parser p(args_->enb_files.rb_config);
  p.add_section(&srb1);
  p.add_section(&srb2);
  p.add_section(&qci);
  p.add_section(&srb1_5g);
  p.add_section(&srb2_5g);
  p.add_section(&five_qi);

  int ret = p.parse();
  if (not srb1_present) {
    rrc_cfg_->srb1_cfg.rlc_cfg.set_default_value();
  }
  if (not srb2_present) {
    rrc_cfg_->srb2_cfg.rlc_cfg.set_default_value();
  }

  if (!srb1_5g_present || !srb2_5g_present) {
    fprintf(stderr, "Optional 5G SRB configuration is not supported yet.\n");
    fprintf(stderr, "Please specify 5G SRB1 and SRB2 configuration.\n");
    return SRSRAN_ERROR;
  }
  rrc_nr_cfg_->srb1_cfg.present = srb1_5g_present;
  rrc_nr_cfg_->srb2_cfg.present = srb1_5g_present;

  return ret;
}

} // namespace rb_sections

} // namespace srsenb
