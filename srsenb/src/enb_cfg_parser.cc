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

#include "enb_cfg_parser.h"
#include "srsenb/hdr/enb.h"
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
  data->intra_freq_black_cell_list.resize((uint32_t)root.getLength());
  data->intra_freq_black_cell_list_present = data->intra_freq_neigh_cell_list.size() > 0;
  for (uint32_t i = 0; i < data->intra_freq_black_cell_list.size() && i < ASN1_RRC_MAX_CELL_BLACK; i++) {
    if (not parse_enum_by_number(data->intra_freq_black_cell_list[i].range, "range", root[i])) {
      fprintf(stderr, "Missing field range in black_cell=%d\n", i);
      return SRSRAN_ERROR;
    }
    data->intra_freq_black_cell_list[i].range_present = true;

    int start = 0;
    if (!root[i].lookupValue("start", start)) {
      fprintf(stderr, "Missing field start in black_cell=%d\n", i);
      return SRSRAN_ERROR;
    }
    data->intra_freq_black_cell_list[i].start = (uint16)start;
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
      ERROR("Error parsing `ncc_permitted` in carrier_freqs_info_lsit=%d", i);
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
      default:
        ERROR("Invalid or unsupported event A%d in meas_report_desc (only A1-A4 are supported)\n",
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

    if (cellroot.exists("scell_list")) {
      HANDLEPARSERCODE(parse_scell_list(cell_cfg, cellroot));
    }

    rrc_cfg->cell_list.push_back(cell_cfg);
  }

  // Configuration check
  for (auto it = rrc_cfg->cell_list.begin(); it != rrc_cfg->cell_list.end(); it++) {
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
    rrc_cell_cfg_nr_t cell_cfg = {};
    auto&             cellroot = root[n];

    parse_opt_field(cell_cfg.phy_cell.rf_port, cellroot, "rf_port");
    HANDLEPARSERCODE(parse_required_field(cell_cfg.phy_cell.carrier.pci, cellroot, "pci"));
    HANDLEPARSERCODE(parse_required_field(cell_cfg.phy_cell.cell_id, cellroot, "cell_id"));
    HANDLEPARSERCODE(parse_required_field(cell_cfg.phy_cell.root_seq_idx, cellroot, "root_seq_idx"));
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
  for (auto it = rrc_cfg_nr->cell_list.begin(); it != rrc_cfg_nr->cell_list.end(); ++it) {
    // check against NR cells
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
      std::vector<uint32_t> bands          = band_helper.get_bands_nr(it->ul_arfcn);
      for (uint32_t band_idx = 0; band_idx < bands.size(); band_idx++) {
        if (bands.at(band_idx) == it->band) {
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
    if (drb_sections::parse_drb(args_, rrc_cfg_) != SRSRAN_SUCCESS) {
      fprintf(stderr, "Error parsing DRB configuration\n");
      return SRSRAN_ERROR;
    }
  } catch (const SettingTypeException& stex) {
    fprintf(stderr, "Error parsing DRB configuration: %s\n", stex.getPath());
    return SRSRAN_ERROR;
  } catch (const ConfigException& cex) {
    fprintf(stderr, "Error parsing DRB configuration\n");
    return SRSRAN_ERROR;
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
  rrc_cfg_->num_nr_cells = rrc_nr_cfg_->cell_list.size();
  args_->rf.nof_carriers = rrc_cfg_->cell_list.size() + rrc_nr_cfg_->cell_list.size();

  // update EUTRA RRC params for ENDC
  if (rrc_nr_cfg_->cell_list.size() == 1) {
    rrc_cfg_->endc_cfg.abs_frequency_ssb = rrc_nr_cfg_->cell_list.at(0).ssb_absolute_freq_point;
    rrc_cfg_->endc_cfg.nr_band           = rrc_nr_cfg_->cell_list.at(0).band;
    rrc_cfg_->endc_cfg.ssb_period_offset.set_sf10_r15();
    rrc_cfg_->endc_cfg.ssb_duration      = asn1::rrc::mtc_ssb_nr_r15_s::ssb_dur_r15_opts::sf1;
    rrc_cfg_->endc_cfg.ssb_ssc           = asn1::rrc::rs_cfg_ssb_nr_r15_s::subcarrier_spacing_ssb_r15_opts::khz15;
    rrc_cfg_->endc_cfg.act_from_b1_event = true; // ENDC will only be activated from B1 measurment
  }

  return SRSRAN_SUCCESS;
}

int set_derived_args(all_args_t* args_, rrc_cfg_t* rrc_cfg_, phy_cfg_t* phy_cfg_, const srsran_cell_t& cell_cfg_)
{
  // Sanity checks
  ASSERT_VALID_CFG(not rrc_cfg_->cell_list.empty(), "No cell specified in rr.conf.");
  ASSERT_VALID_CFG(args_->stack.mac.nof_prealloc_ues <= SRSENB_MAX_UES,
                   "mac.nof_prealloc_ues=%d must be within [0, %d]",
                   args_->stack.mac.nof_prealloc_ues,
                   SRSENB_MAX_UES);

  // Check for a forced  DL EARFCN or frequency (only valid for a single cell config (Xico's favorite feature))
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

  // Create dedicated cell configuration from RRC configuration
  for (auto it = rrc_cfg_->cell_list.begin(); it != rrc_cfg_->cell_list.end(); ++it) {
    auto&          cfg          = *it;
    phy_cell_cfg_t phy_cell_cfg = {};
    phy_cell_cfg.cell           = cell_cfg_;
    phy_cell_cfg.cell.id        = cfg.pci;
    phy_cell_cfg.cell_id        = cfg.cell_id;
    phy_cell_cfg.root_seq_idx   = cfg.root_seq_idx;
    phy_cell_cfg.rf_port        = cfg.rf_port;
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
  // set rach cfg common
  auto& rach_cfg_common  = rrc_nr_cfg_->rach_cfg_common;
  auto& rach_cfg_generic = rach_cfg_common.rach_cfg_generic;

  uint8_t msg1_fdm = 1; // TODO read from config
  if (!asn1::number_to_enum(rach_cfg_generic.msg1_fdm, msg1_fdm)) {
    ERROR("Config Error: Invalid msg1_fdm (%d)\n", msg1_fdm);
    return SRSRAN_ERROR;
  }

  rach_cfg_generic.preamb_rx_target_pwr = -110; // TODO read from config

  uint8_t preamb_trans_max = 7; // TODO read from config
  if (!asn1::number_to_enum(rach_cfg_generic.preamb_trans_max, preamb_trans_max)) {
    ERROR("Config Error: Invalid preamble_trans_max (%d)\n", preamb_trans_max);
    return SRSRAN_ERROR;
  }

  uint8_t pwr_ramp_step = 4; // TODO read from config
  if (!asn1::number_to_enum(rach_cfg_generic.pwr_ramp_step, pwr_ramp_step)) {
    ERROR("Config Error: Invalid pwr_ramp_step (%d)\n", pwr_ramp_step);
    return SRSRAN_ERROR;
  }

  uint8_t ra_resp_win_size = 10; // TODO read from config
  if (!asn1::number_to_enum(rach_cfg_generic.ra_resp_win, ra_resp_win_size)) {
    ERROR("Config Error: Invalid ra_resp_win_size (%d)\n", ra_resp_win_size);
    return SRSRAN_ERROR;
  }

  uint8_t ra_contention_resolution_timer = 64; // TODO read from config
  if (!asn1::number_to_enum(rach_cfg_common.ra_contention_resolution_timer, ra_contention_resolution_timer)) {
    ERROR("Config Error: Invalid mac_con_res_timer (%d)\n", ra_contention_resolution_timer);
    return SRSRAN_ERROR;
  }

  rrc_nr_cfg_->prach_root_seq_idx_type = 839; // TODO read from config

  std::string restricted_set_cfg = "unrestrictedSet"; // TODO read from config
  asn1::rrc_nr::rach_cfg_common_s::prach_root_seq_idx_c_::types_opts root_seq_idx_type;
  if (!asn1::string_to_enum(rach_cfg_common.restricted_set_cfg, restricted_set_cfg)) {
    ERROR("Config Error: Invalid restricted_set_cfg (%s)\n", restricted_set_cfg.c_str());
    return SRSRAN_ERROR;
  }

  rach_cfg_common.ssb_per_rach_occasion_and_cb_preambs_per_ssb_present = true;
  rach_cfg_common.ssb_per_rach_occasion_and_cb_preambs_per_ssb.set_one(); // TODO read from config

  uint8_t one_opts = 64; // TODO read from config
  if (!asn1::number_to_enum(rach_cfg_common.ssb_per_rach_occasion_and_cb_preambs_per_ssb.one(), one_opts)) {
    ERROR("Config Error: Invalid one_opts (%d)\n", one_opts);
    return SRSRAN_ERROR;
  }

  // Use helper class to derive NR carrier parameters
  srsran::srsran_band_helper band_helper;

  // we only support one NR cell
  if (rrc_nr_cfg_->cell_list.size() > 1) {
    ERROR("Only a single NR cell supported.");
    return SRSRAN_ERROR;
  }

  // Create NR dedicated cell configuration from RRC configuration
  for (auto it = rrc_nr_cfg_->cell_list.begin(); it != rrc_nr_cfg_->cell_list.end(); ++it) {
    auto& cfg                            = *it;
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
    cfg.phy_cell.num_ra_preambles = 52; // FIXME: read from config

    if (cfg.phy_cell.dl_freq_hz == 0) {
      cfg.phy_cell.dl_freq_hz = band_helper.nr_arfcn_to_freq(cfg.dl_arfcn);
    }

    if (cfg.phy_cell.ul_freq_hz == 0) {
      // auto-detect UL frequency
      if (cfg.ul_arfcn == 0) {
        // derive UL ARFCN from given DL ARFCN
        cfg.ul_arfcn = band_helper.get_ul_arfcn_from_dl_arfcn(cfg.dl_arfcn);
        if (cfg.ul_arfcn == 0) {
          ERROR("Can't derive UL ARFCN from DL ARFCN %d", cfg.dl_arfcn);
          return SRSRAN_ERROR;
        }
      }
      cfg.phy_cell.ul_freq_hz = band_helper.nr_arfcn_to_freq(cfg.ul_arfcn);
    }

    // duplex mode
    cfg.duplex_mode = band_helper.get_duplex_mode(cfg.band);

    // PRACH
    cfg.phy_cell.prach.is_nr                 = true;
    cfg.phy_cell.prach.config_idx            = 8;
    cfg.phy_cell.prach.root_seq_idx          = 0;
    cfg.phy_cell.prach.freq_offset           = 1;
    cfg.phy_cell.prach.num_ra_preambles      = cfg.phy_cell.num_ra_preambles;
    cfg.phy_cell.prach.hs_flag               = phy_cfg_->prach_cnfg.prach_cfg_info.high_speed_flag;
    cfg.phy_cell.prach.tdd_config.configured = (cfg.duplex_mode == SRSRAN_DUPLEX_MODE_TDD);

    // PDCCH
    // Configure CORESET ID 1
    cfg.phy_cell.pdcch.coreset_present[1]              = true;
    cfg.phy_cell.pdcch.coreset[1].id                   = 1;
    cfg.phy_cell.pdcch.coreset[1].duration             = 1;
    cfg.phy_cell.pdcch.coreset[1].mapping_type         = srsran_coreset_mapping_type_non_interleaved;
    cfg.phy_cell.pdcch.coreset[1].precoder_granularity = srsran_coreset_precoder_granularity_reg_bundle;

    // Generate frequency resources for the full BW
    for (uint32_t i = 0; i < SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE; i++) {
      cfg.phy_cell.pdcch.coreset[1].freq_resources[i] = i < SRSRAN_FLOOR(cfg.phy_cell.carrier.nof_prb, 6);
    }

    // Configure Search Space 1 as common
    cfg.phy_cell.pdcch.search_space_present[1]     = true;
    cfg.phy_cell.pdcch.search_space[1].id          = 1;
    cfg.phy_cell.pdcch.search_space[1].coreset_id  = 1;
    cfg.phy_cell.pdcch.search_space[1].duration    = 1;
    cfg.phy_cell.pdcch.search_space[1].formats[0]  = srsran_dci_format_nr_0_0; // DCI format for PUSCH
    cfg.phy_cell.pdcch.search_space[1].formats[1]  = srsran_dci_format_nr_1_0; // DCI format for PDSCH
    cfg.phy_cell.pdcch.search_space[1].nof_formats = 2;
    cfg.phy_cell.pdcch.search_space[1].type        = srsran_search_space_type_common_3;

    // Generate 1 candidate for each aggregation level if possible
    for (uint32_t L = 0; L < SRSRAN_SEARCH_SPACE_NOF_AGGREGATION_LEVELS_NR; L++) {
      cfg.phy_cell.pdcch.search_space[1].nof_candidates[L] =
          SRSRAN_MIN(2, srsran_pdcch_nr_max_candidates_coreset(&cfg.phy_cell.pdcch.coreset[1], L));
    }

    cfg.phy_cell.pdcch.ra_search_space_present = true;
    cfg.phy_cell.pdcch.ra_search_space         = cfg.phy_cell.pdcch.search_space[1];
    cfg.phy_cell.pdcch.ra_search_space.type    = srsran_search_space_type_common_1;

    // PDSCH
    cfg.phy_cell.pdsch.rs_power = phy_cfg_->pdsch_cnfg.ref_sig_pwr;
    cfg.phy_cell.pdsch.p_b      = phy_cfg_->pdsch_cnfg.p_b;

    // copy center frequencies
    cfg.phy_cell.carrier.dl_center_frequency_hz = cfg.phy_cell.dl_freq_hz;
    cfg.phy_cell.carrier.ul_center_frequency_hz = cfg.phy_cell.ul_freq_hz;

    cfg.dl_absolute_freq_point_a = band_helper.get_abs_freq_point_a_arfcn(cfg.phy_cell.carrier.nof_prb, cfg.dl_arfcn);
    cfg.ul_absolute_freq_point_a = band_helper.get_abs_freq_point_a_arfcn(cfg.phy_cell.carrier.nof_prb, cfg.ul_arfcn);

    // Calculate SSB params depending on band/duplex
    cfg.ssb_cfg.duplex_mode = band_helper.get_duplex_mode(cfg.band);
    cfg.ssb_cfg.pattern     = band_helper.get_ssb_pattern(cfg.band, srsran_subcarrier_spacing_15kHz);
    if (cfg.ssb_cfg.pattern == SRSRAN_SSB_PATTERN_A) {
      // 15kHz SSB SCS
      cfg.ssb_cfg.scs = srsran_subcarrier_spacing_15kHz;
    } else {
      // try to optain SSB pattern for same band with 30kHz SCS
      cfg.ssb_cfg.pattern = band_helper.get_ssb_pattern(cfg.band, srsran_subcarrier_spacing_30kHz);
      if (cfg.ssb_cfg.pattern == SRSRAN_SSB_PATTERN_B || cfg.ssb_cfg.pattern == SRSRAN_SSB_PATTERN_C) {
        // SSB SCS is 30 kHz
        cfg.ssb_cfg.scs = srsran_subcarrier_spacing_30kHz;
      } else {
        ERROR("Can't derive SSB pattern for band %d", cfg.band);
        return SRSRAN_ERROR;
      }
    }

    // fill remaining SSB fields
    cfg.ssb_absolute_freq_point =
        band_helper.get_abs_freq_ssb_arfcn(cfg.band, cfg.ssb_cfg.scs, cfg.dl_absolute_freq_point_a);
    if (cfg.ssb_absolute_freq_point == 0) {
      ERROR("Can't derive SSB freq point for dl_arfcn %d and band %d", cfg.dl_arfcn, cfg.band);
      return SRSRAN_ERROR;
    }

    // Convert to frequency for PHY
    cfg.phy_cell.carrier.ssb_center_freq_hz = band_helper.nr_arfcn_to_freq(cfg.ssb_absolute_freq_point);

    cfg.ssb_cfg.center_freq_hz = cfg.phy_cell.carrier.dl_center_frequency_hz;
    cfg.ssb_cfg.ssb_freq_hz    = cfg.phy_cell.carrier.ssb_center_freq_hz;
    cfg.ssb_cfg.periodicity_ms = 10; // TODO: make a param
    cfg.ssb_cfg.beta_pss       = 0.0;
    cfg.ssb_cfg.beta_sss       = 0.0;
    cfg.ssb_cfg.beta_pbch      = 0.0;
    cfg.ssb_cfg.beta_pbch_dmrs = 0.0;
    // set by PHY layer in worker_pool::set_common_cfg
    cfg.ssb_cfg.srate_hz = 0.0;
    cfg.ssb_cfg.scaling  = 0.0;

    phy_cfg_->phy_cell_cfg_nr.push_back(cfg.phy_cell);
  }

  // MAC-NR PCAP options
  args_->nr_stack.mac.pcap.enable = args_->stack.mac_pcap.enable;
  args_->nr_stack.log             = args_->stack.log;

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
  acbarring_data.add_field(make_asn1_enum_number_parser("fime", &data->ac_barr_info.ac_barr_for_mo_data.ac_barr_time));
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

int parse_sib7(std::string filename, sib_type7_s* data)
{
  parser::section sib7("sib7");

  sib7.add_field(new parser::field<uint8>("t_resel_geran", &data->t_resel_geran));
  // TODO: t_resel_geran_sf

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
  cell_access->plmn_id_list.resize(1);
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

namespace drb_sections {

int parse_drb(all_args_t* args_, rrc_cfg_t* rrc_cfg_)
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

  // Run parser with two sections
  parser p(args_->enb_files.rb_config);
  p.add_section(&srb1);
  p.add_section(&srb2);
  p.add_section(&qci);

  int ret = p.parse();
  if (not srb1_present) {
    rrc_cfg_->srb1_cfg.rlc_cfg.set_default_value();
  }
  if (not srb2_present) {
    rrc_cfg_->srb2_cfg.rlc_cfg.set_default_value();
  }

  return ret;
}

} // namespace drb_sections

} // namespace srsenb
