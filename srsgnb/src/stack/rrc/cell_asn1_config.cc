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

#include "srsgnb/hdr/stack/rrc/cell_asn1_config.h"
#include "srsenb/hdr/common/common_enb.h"
#include "srsran/asn1/obj_id_cmp_utils.h"
#include "srsran/asn1/rrc_nr_utils.h"
#include "srsran/common/band_helper.h"
#include <bitset>

using namespace asn1::rrc_nr;

#define HANDLE_ERROR(ret)                                                                                              \
  if ((ret) != SRSRAN_SUCCESS) {                                                                                       \
    return SRSRAN_ERROR;                                                                                               \
  }

namespace srsenb {

srslog::basic_logger& get_logger(const rrc_nr_cfg_t& cfg)
{
  static srslog::basic_logger& log_obj = srslog::fetch_basic_logger(cfg.log_name);
  return log_obj;
}

void set_search_space_from_phy_cfg(const srsran_search_space_t& cfg, asn1::rrc_nr::search_space_s& out)
{
  out.search_space_id                                = cfg.id;
  out.ctrl_res_set_id_present                        = true;
  out.ctrl_res_set_id                                = cfg.coreset_id;
  out.monitoring_slot_periodicity_and_offset_present = true;
  out.monitoring_slot_periodicity_and_offset.set_sl1();
  out.dur_present                            = false; // false for duration=1
  out.monitoring_symbols_within_slot_present = true;
  out.monitoring_symbols_within_slot.from_number(0b10000000000000);

  out.nrof_candidates_present = true;
  bool ret                    = asn1::number_to_enum(out.nrof_candidates.aggregation_level1, cfg.nof_candidates[0]);
  srsran_assert(ret, "Failed to convert nof candidates=%d", cfg.nof_candidates[0]);
  ret = asn1::number_to_enum(out.nrof_candidates.aggregation_level2, cfg.nof_candidates[1]);
  srsran_assert(ret, "Failed to convert nof candidates=%d", cfg.nof_candidates[1]);
  ret = asn1::number_to_enum(out.nrof_candidates.aggregation_level4, cfg.nof_candidates[2]);
  srsran_assert(ret, "Failed to convert nof candidates=%d", cfg.nof_candidates[2]);
  ret = asn1::number_to_enum(out.nrof_candidates.aggregation_level8, cfg.nof_candidates[3]);
  srsran_assert(ret, "Failed to convert nof candidates=%d", cfg.nof_candidates[3]);
  ret = asn1::number_to_enum(out.nrof_candidates.aggregation_level16, cfg.nof_candidates[4]);
  srsran_assert(ret, "Failed to convert nof candidates=%d", cfg.nof_candidates[4]);

  out.search_space_type_present = true;
  if ((cfg.type == srsran_search_space_type_common_0) or (cfg.type == srsran_search_space_type_common_0A) or
      (cfg.type == srsran_search_space_type_common_1) or (cfg.type == srsran_search_space_type_common_2) or
      (cfg.type == srsran_search_space_type_common_3)) {
    out.search_space_type.set_common();

    out.search_space_type.common().dci_format0_minus0_and_format1_minus0_present = true;
  } else if (cfg.type == srsran_search_space_type_ue) {
    out.search_space_type.set_ue_specific().dci_formats.value =
        search_space_s::search_space_type_c_::ue_specific_s_::dci_formats_opts::formats0_minus0_and_minus1_minus0;
  } else {
    srsran_terminate("Config Error: Unsupported search space type.");
  }
}

void set_coreset_from_phy_cfg(const srsran_coreset_t& coreset_cfg, asn1::rrc_nr::ctrl_res_set_s& out)
{
  out.ctrl_res_set_id = coreset_cfg.id;

  std::bitset<SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE> freq_domain_res;
  for (uint32_t i = 0; i < SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE; i++) {
    freq_domain_res[SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE - 1 - i] = coreset_cfg.freq_resources[i];
  }
  out.freq_domain_res.from_number(freq_domain_res.to_ulong());

  out.dur = coreset_cfg.duration;

  if (coreset_cfg.mapping_type == srsran_coreset_mapping_type_non_interleaved) {
    out.cce_reg_map_type.set_non_interleaved();
  } else {
    auto& interleaved                  = out.cce_reg_map_type.set_interleaved();
    interleaved.reg_bundle_size.value  = (decltype(interleaved.reg_bundle_size.value))coreset_cfg.reg_bundle_size;
    interleaved.interleaver_size.value = (decltype(interleaved.interleaver_size.value))coreset_cfg.interleaver_size;
    interleaved.shift_idx_present      = true;
    interleaved.shift_idx              = coreset_cfg.shift_index;
  }

  if (coreset_cfg.precoder_granularity == srsran_coreset_precoder_granularity_reg_bundle) {
    out.precoder_granularity = asn1::rrc_nr::ctrl_res_set_s::precoder_granularity_opts::same_as_reg_bundle;
  } else {
    out.precoder_granularity = asn1::rrc_nr::ctrl_res_set_s::precoder_granularity_opts::all_contiguous_rbs;
  }

  // TODO: Remaining fields
}

void set_rach_cfg_common(const srsran_prach_cfg_t& prach_cfg, asn1::rrc_nr::rach_cfg_common_s& out)
{
  // rach-ConfigGeneric
  out.rach_cfg_generic.prach_cfg_idx             = prach_cfg.config_idx;
  out.rach_cfg_generic.msg1_fdm.value            = rach_cfg_generic_s::msg1_fdm_opts::one;
  out.rach_cfg_generic.msg1_freq_start           = prach_cfg.freq_offset;
  out.rach_cfg_generic.zero_correlation_zone_cfg = prach_cfg.zero_corr_zone;
  out.rach_cfg_generic.preamb_rx_target_pwr      = -110;
  out.rach_cfg_generic.preamb_trans_max.value    = rach_cfg_generic_s::preamb_trans_max_opts::n7;
  out.rach_cfg_generic.pwr_ramp_step.value       = rach_cfg_generic_s::pwr_ramp_step_opts::db4;
  out.rach_cfg_generic.ra_resp_win.value         = rach_cfg_generic_s::ra_resp_win_opts::sl10;

  out.ssb_per_rach_occasion_and_cb_preambs_per_ssb_present = true;
  asn1::number_to_enum(out.ssb_per_rach_occasion_and_cb_preambs_per_ssb.set_one(), prach_cfg.num_ra_preambles);
  out.ra_contention_resolution_timer.value = rach_cfg_common_s::ra_contention_resolution_timer_opts::sf64;
  out.prach_root_seq_idx.set_l839()        = prach_cfg.root_seq_idx;
  out.restricted_set_cfg.value             = rach_cfg_common_s::restricted_set_cfg_opts::unrestricted_set;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void fill_tdd_ul_dl_config_common(const rrc_cell_cfg_nr_t& cfg, asn1::rrc_nr::tdd_ul_dl_cfg_common_s& tdd)
{
  srsran_assert(cfg.duplex_mode == SRSRAN_DUPLEX_MODE_TDD, "This function should only be called for TDD configs");
  // TDD UL-DL config
  tdd.ref_subcarrier_spacing.value  = (asn1::rrc_nr::subcarrier_spacing_opts::options)cfg.phy_cell.carrier.scs;
  tdd.pattern1.dl_ul_tx_periodicity = asn1::rrc_nr::tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms10;
  tdd.pattern1.nrof_dl_slots        = 6;
  tdd.pattern1.nrof_dl_symbols      = 0;
  tdd.pattern1.nrof_ul_slots        = 4;
  tdd.pattern1.nrof_ul_symbols      = 0;
}

/// Fill list of CSI-ReportConfig with gNB config
int fill_csi_report_from_enb_cfg(const rrc_nr_cfg_t& cfg, csi_meas_cfg_s& csi_meas_cfg)
{
  if (cfg.is_standalone) {
    csi_meas_cfg.csi_report_cfg_to_add_mod_list.resize(1);

    auto& csi_report                               = csi_meas_cfg.csi_report_cfg_to_add_mod_list[0];
    csi_report.report_cfg_id                       = 0;
    csi_report.res_for_ch_meas                     = 0;
    csi_report.csi_im_res_for_interference_present = true;
    csi_report.csi_im_res_for_interference         = 1;
    csi_report.report_cfg_type.set_periodic();
    csi_report.report_cfg_type.periodic().report_slot_cfg.set_slots80();
    csi_report.report_cfg_type.periodic().pucch_csi_res_list.resize(1);
    csi_report.report_cfg_type.periodic().pucch_csi_res_list[0].ul_bw_part_id = 0;
    csi_report.report_cfg_type.periodic().pucch_csi_res_list[0].pucch_res =
        17; // was 17 in orig PCAP, but code for NSA it was set to 1
    csi_report.report_quant.set_cri_ri_pmi_cqi();
    // Report freq config (optional)
    csi_report.report_freq_cfg_present                = true;
    csi_report.report_freq_cfg.cqi_format_ind_present = true;
    csi_report.report_freq_cfg.cqi_format_ind.value =
        csi_report_cfg_s::report_freq_cfg_s_::cqi_format_ind_opts::wideband_cqi;
    csi_report.report_freq_cfg.pmi_format_ind_present = true;
    csi_report.report_freq_cfg.pmi_format_ind.value =
        csi_report_cfg_s::report_freq_cfg_s_::pmi_format_ind_opts::wideband_pmi;
    csi_report.time_restrict_for_ch_meass.value = csi_report_cfg_s::time_restrict_for_ch_meass_opts::not_cfgured;
    csi_report.time_restrict_for_interference_meass.value =
        asn1::rrc_nr::csi_report_cfg_s::time_restrict_for_interference_meass_opts::not_cfgured;
    csi_report.codebook_cfg_present = true;
    auto& type1                     = csi_report.codebook_cfg.codebook_type.set_type1();
    type1.sub_type.set_type_i_single_panel();
    type1.sub_type.type_i_single_panel().nr_of_ant_ports.set_two();
    type1.sub_type.type_i_single_panel().nr_of_ant_ports.two().two_tx_codebook_subset_restrict.from_number(0b111111);
    type1.sub_type.type_i_single_panel().type_i_single_panel_ri_restrict.from_number(0x03);
    type1.codebook_mode = 1;
    csi_report.group_based_beam_report.set_disabled();
    // Skip CQI table (optional)
    csi_report.cqi_table_present = true;
    csi_report.cqi_table         = asn1::rrc_nr::csi_report_cfg_s::cqi_table_opts::table1;
    csi_report.subband_size      = asn1::rrc_nr::csi_report_cfg_s::subband_size_opts::value1;

    if (cfg.cell_list[0].duplex_mode == SRSRAN_DUPLEX_MODE_FDD) {
      csi_report.report_cfg_type.periodic().report_slot_cfg.slots80() = 1;
    } else {
      csi_report.report_cfg_type.periodic().report_slot_cfg.slots80() = 7;
    }
  } else {
    csi_meas_cfg.csi_report_cfg_to_add_mod_list.resize(1);

    auto& csi_report                               = csi_meas_cfg.csi_report_cfg_to_add_mod_list[0];
    csi_report.report_cfg_id                       = 0;
    csi_report.res_for_ch_meas                     = 0;
    csi_report.csi_im_res_for_interference_present = true;
    csi_report.csi_im_res_for_interference         = 1;
    csi_report.report_cfg_type.set_periodic();
    csi_report.report_cfg_type.periodic().report_slot_cfg.set_slots80();
    csi_report.report_cfg_type.periodic().pucch_csi_res_list.resize(1);
    csi_report.report_cfg_type.periodic().pucch_csi_res_list[0].ul_bw_part_id = 0;
    csi_report.report_cfg_type.periodic().pucch_csi_res_list[0].pucch_res     = 1; // was 17 in orig PCAP
    csi_report.report_quant.set_cri_ri_pmi_cqi();
    // Report freq config (optional)
    csi_report.report_freq_cfg_present                = true;
    csi_report.report_freq_cfg.cqi_format_ind_present = true;
    csi_report.report_freq_cfg.cqi_format_ind = csi_report_cfg_s::report_freq_cfg_s_::cqi_format_ind_opts::wideband_cqi;
    csi_report.time_restrict_for_ch_meass     = csi_report_cfg_s::time_restrict_for_ch_meass_opts::not_cfgured;
    csi_report.time_restrict_for_interference_meass =
        asn1::rrc_nr::csi_report_cfg_s::time_restrict_for_interference_meass_opts::not_cfgured;
    csi_report.group_based_beam_report.set_disabled();
    // Skip CQI table (optional)
    csi_report.cqi_table_present = true;
    csi_report.cqi_table         = asn1::rrc_nr::csi_report_cfg_s::cqi_table_opts::table2;
    csi_report.subband_size      = asn1::rrc_nr::csi_report_cfg_s::subband_size_opts::value1;

    if (cfg.cell_list[0].duplex_mode == SRSRAN_DUPLEX_MODE_FDD) {
      csi_report.report_cfg_type.periodic().report_slot_cfg.slots80() = 5;
    } else {
      csi_report.report_cfg_type.periodic().report_slot_cfg.slots80() = 7;
    }
  }

  return SRSRAN_SUCCESS;
}

/// Fill lists of NZP-CSI-RS-Resource and NZP-CSI-RS-ResourceSet with gNB config
void fill_nzp_csi_rs_from_enb_cfg(const rrc_nr_cfg_t& cfg, csi_meas_cfg_s& csi_meas_cfg)
{
  if (cfg.is_standalone) {
    if (cfg.cell_list[0].duplex_mode == SRSRAN_DUPLEX_MODE_FDD) {
      csi_meas_cfg.nzp_csi_rs_res_to_add_mod_list.resize(5);
      auto& nzp_csi_res = csi_meas_cfg.nzp_csi_rs_res_to_add_mod_list;
      // item 0
      nzp_csi_res[0].nzp_csi_rs_res_id = 0;
      nzp_csi_res[0].res_map.freq_domain_alloc.set_row2();
      nzp_csi_res[0].res_map.freq_domain_alloc.row2().from_number(0x800);
      nzp_csi_res[0].res_map.nrof_ports.value                 = asn1::rrc_nr::csi_rs_res_map_s::nrof_ports_opts::p1;
      nzp_csi_res[0].res_map.first_ofdm_symbol_in_time_domain = 4;
      nzp_csi_res[0].res_map.cdm_type.value                   = asn1::rrc_nr::csi_rs_res_map_s::cdm_type_opts::no_cdm;
      nzp_csi_res[0].res_map.density.set_one();
      nzp_csi_res[0].res_map.freq_band.start_rb     = 0;
      nzp_csi_res[0].res_map.freq_band.nrof_rbs     = 52;
      nzp_csi_res[0].pwr_ctrl_offset                = 0;
      nzp_csi_res[0].pwr_ctrl_offset_ss_present     = true;
      nzp_csi_res[0].pwr_ctrl_offset_ss.value       = asn1::rrc_nr::nzp_csi_rs_res_s::pwr_ctrl_offset_ss_opts::db0;
      nzp_csi_res[0].scrambling_id                  = cfg.cell_list[0].phy_cell.cell_id;
      nzp_csi_res[0].periodicity_and_offset_present = true;
      nzp_csi_res[0].periodicity_and_offset.set_slots80();
      nzp_csi_res[0].periodicity_and_offset.slots80() = 1;
      // optional
      nzp_csi_res[0].qcl_info_periodic_csi_rs_present = true;
      nzp_csi_res[0].qcl_info_periodic_csi_rs         = 0;
      // item 1
      nzp_csi_res[1]                   = nzp_csi_res[0];
      nzp_csi_res[1].nzp_csi_rs_res_id = 1;
      nzp_csi_res[1].res_map.freq_domain_alloc.set_row1();
      nzp_csi_res[1].res_map.freq_domain_alloc.row1().from_number(0x1);
      nzp_csi_res[1].res_map.nrof_ports.value = asn1::rrc_nr::csi_rs_res_map_s::nrof_ports_opts::p1;
      nzp_csi_res[1].res_map.cdm_type.value   = asn1::rrc_nr::csi_rs_res_map_s::cdm_type_opts::no_cdm;
      nzp_csi_res[1].res_map.density.set_three();
      nzp_csi_res[1].periodicity_and_offset.set_slots40();
      nzp_csi_res[1].periodicity_and_offset.slots40() = 11;
      // item 2
      nzp_csi_res[2]                                          = nzp_csi_res[1];
      nzp_csi_res[2].nzp_csi_rs_res_id                        = 2;
      nzp_csi_res[2].res_map.first_ofdm_symbol_in_time_domain = 8;
      // item 3
      nzp_csi_res[3]                                          = nzp_csi_res[1];
      nzp_csi_res[3].nzp_csi_rs_res_id                        = 3;
      nzp_csi_res[3].res_map.first_ofdm_symbol_in_time_domain = 4;
      nzp_csi_res[3].periodicity_and_offset.set_slots40();
      nzp_csi_res[3].periodicity_and_offset.slots40() = 12;
      // item 4
      nzp_csi_res[4]                                          = nzp_csi_res[1];
      nzp_csi_res[4].nzp_csi_rs_res_id                        = 4;
      nzp_csi_res[4].res_map.first_ofdm_symbol_in_time_domain = 8;
      nzp_csi_res[4].periodicity_and_offset.set_slots40();
      nzp_csi_res[4].periodicity_and_offset.slots40() = 12;
    } else {
      csi_meas_cfg.nzp_csi_rs_res_to_add_mod_list.resize(1);
      auto& nzp_csi_res                = csi_meas_cfg.nzp_csi_rs_res_to_add_mod_list;
      nzp_csi_res[0].nzp_csi_rs_res_id = 0;
      nzp_csi_res[0].res_map.freq_domain_alloc.set_row2();
      nzp_csi_res[0].res_map.freq_domain_alloc.row2().from_number(0b100000000000);
      nzp_csi_res[0].res_map.nrof_ports.value                 = asn1::rrc_nr::csi_rs_res_map_s::nrof_ports_opts::p1;
      nzp_csi_res[0].res_map.first_ofdm_symbol_in_time_domain = 4;
      nzp_csi_res[0].res_map.cdm_type.value                   = asn1::rrc_nr::csi_rs_res_map_s::cdm_type_opts::no_cdm;
      nzp_csi_res[0].res_map.density.set_one();
      nzp_csi_res[0].res_map.freq_band.start_rb = 0;
      nzp_csi_res[0].res_map.freq_band.nrof_rbs = 52;
      nzp_csi_res[0].pwr_ctrl_offset            = 0;
      // Skip pwr_ctrl_offset_ss_present
      nzp_csi_res[0].periodicity_and_offset_present       = true;
      nzp_csi_res[0].periodicity_and_offset.set_slots80() = 0;
      // optional
      nzp_csi_res[0].qcl_info_periodic_csi_rs_present = true;
      nzp_csi_res[0].qcl_info_periodic_csi_rs         = 0;
    }

    // Fill NZP-CSI Resource Sets
    if (cfg.cell_list[0].duplex_mode == SRSRAN_DUPLEX_MODE_FDD) {
      csi_meas_cfg.nzp_csi_rs_res_set_to_add_mod_list.resize(2);
      auto& nzp_csi_res_set = csi_meas_cfg.nzp_csi_rs_res_set_to_add_mod_list;
      // item 0
      nzp_csi_res_set[0].nzp_csi_res_set_id = 0;
      nzp_csi_res_set[0].nzp_csi_rs_res.resize(1);
      nzp_csi_res_set[0].nzp_csi_rs_res[0] = 0;
      // item 1
      nzp_csi_res_set[1].nzp_csi_res_set_id = 1;
      nzp_csi_res_set[1].nzp_csi_rs_res.resize(4);
      nzp_csi_res_set[1].nzp_csi_rs_res[0] = 1;
      nzp_csi_res_set[1].nzp_csi_rs_res[1] = 2;
      nzp_csi_res_set[1].nzp_csi_rs_res[2] = 3;
      nzp_csi_res_set[1].nzp_csi_rs_res[3] = 4;
      nzp_csi_res_set[1].trs_info_present  = true;
      //    // Skip TRS info
    } else {
      csi_meas_cfg.nzp_csi_rs_res_set_to_add_mod_list.resize(1);
      auto& nzp_csi_res_set                 = csi_meas_cfg.nzp_csi_rs_res_set_to_add_mod_list;
      nzp_csi_res_set[0].nzp_csi_res_set_id = 0;
      nzp_csi_res_set[0].nzp_csi_rs_res.resize(1);
      nzp_csi_res_set[0].nzp_csi_rs_res[0] = 0;
      // Skip TRS info
    }
  } else {
    if (cfg.cell_list[0].duplex_mode == SRSRAN_DUPLEX_MODE_FDD) {
      csi_meas_cfg.nzp_csi_rs_res_to_add_mod_list.resize(5);
      auto& nzp_csi_res = csi_meas_cfg.nzp_csi_rs_res_to_add_mod_list;
      // item 0
      nzp_csi_res[0].nzp_csi_rs_res_id = 0;
      nzp_csi_res[0].res_map.freq_domain_alloc.set_row2();
      nzp_csi_res[0].res_map.freq_domain_alloc.row2().from_number(0b100000000000);
      nzp_csi_res[0].res_map.nrof_ports                       = asn1::rrc_nr::csi_rs_res_map_s::nrof_ports_opts::p1;
      nzp_csi_res[0].res_map.first_ofdm_symbol_in_time_domain = 4;
      nzp_csi_res[0].res_map.cdm_type                         = asn1::rrc_nr::csi_rs_res_map_s::cdm_type_opts::no_cdm;
      nzp_csi_res[0].res_map.density.set_one();
      nzp_csi_res[0].res_map.freq_band.start_rb = 0;
      nzp_csi_res[0].res_map.freq_band.nrof_rbs = 52;
      nzp_csi_res[0].pwr_ctrl_offset            = 0;
      // Skip pwr_ctrl_offset_ss_present
      nzp_csi_res[0].scrambling_id                  = cfg.cell_list[0].phy_cell.cell_id;
      nzp_csi_res[0].periodicity_and_offset_present = true;
      nzp_csi_res[0].periodicity_and_offset.set_slots80();
      nzp_csi_res[0].periodicity_and_offset.slots80() = 1;
      // optional
      nzp_csi_res[0].qcl_info_periodic_csi_rs_present = true;
      nzp_csi_res[0].qcl_info_periodic_csi_rs         = 0;
      // item 1
      nzp_csi_res[1]                   = nzp_csi_res[0];
      nzp_csi_res[1].nzp_csi_rs_res_id = 1;
      nzp_csi_res[1].res_map.freq_domain_alloc.set_row1();
      nzp_csi_res[1].res_map.freq_domain_alloc.row1().from_number(0b0001);
      nzp_csi_res[1].res_map.first_ofdm_symbol_in_time_domain = 4;
      nzp_csi_res[1].res_map.density.set_three();
      nzp_csi_res[1].periodicity_and_offset.set_slots40();
      nzp_csi_res[1].periodicity_and_offset.slots40() = 11;
      // item 2
      nzp_csi_res[2]                                          = nzp_csi_res[1];
      nzp_csi_res[2].nzp_csi_rs_res_id                        = 2;
      nzp_csi_res[2].res_map.first_ofdm_symbol_in_time_domain = 8;
      nzp_csi_res[2].periodicity_and_offset.set_slots40();
      nzp_csi_res[2].periodicity_and_offset.slots40() = 11;
      // item 3
      nzp_csi_res[3]                                          = nzp_csi_res[1];
      nzp_csi_res[3].nzp_csi_rs_res_id                        = 3;
      nzp_csi_res[3].res_map.first_ofdm_symbol_in_time_domain = 4;
      nzp_csi_res[3].periodicity_and_offset.set_slots40();
      nzp_csi_res[3].periodicity_and_offset.slots40() = 12;
      // item 4
      nzp_csi_res[4]                                          = nzp_csi_res[1];
      nzp_csi_res[4].nzp_csi_rs_res_id                        = 4;
      nzp_csi_res[4].res_map.first_ofdm_symbol_in_time_domain = 8;
      nzp_csi_res[4].periodicity_and_offset.set_slots40();
      nzp_csi_res[4].periodicity_and_offset.slots40() = 12;
    } else {
      csi_meas_cfg.nzp_csi_rs_res_to_add_mod_list.resize(1);
      auto& nzp_csi_res                = csi_meas_cfg.nzp_csi_rs_res_to_add_mod_list;
      nzp_csi_res[0].nzp_csi_rs_res_id = 0;
      nzp_csi_res[0].res_map.freq_domain_alloc.set_row2();
      nzp_csi_res[0].res_map.freq_domain_alloc.row2().from_number(0b100000000000);
      nzp_csi_res[0].res_map.nrof_ports                       = asn1::rrc_nr::csi_rs_res_map_s::nrof_ports_opts::p1;
      nzp_csi_res[0].res_map.first_ofdm_symbol_in_time_domain = 4;
      nzp_csi_res[0].res_map.cdm_type                         = asn1::rrc_nr::csi_rs_res_map_s::cdm_type_opts::no_cdm;
      nzp_csi_res[0].res_map.density.set_one();
      nzp_csi_res[0].res_map.freq_band.start_rb = 0;
      nzp_csi_res[0].res_map.freq_band.nrof_rbs = 52;
      nzp_csi_res[0].pwr_ctrl_offset            = 0;
      // Skip pwr_ctrl_offset_ss_present
      nzp_csi_res[0].periodicity_and_offset_present       = true;
      nzp_csi_res[0].periodicity_and_offset.set_slots80() = 0;
      // optional
      nzp_csi_res[0].qcl_info_periodic_csi_rs_present = true;
      nzp_csi_res[0].qcl_info_periodic_csi_rs         = 0;
    }

    // Fill NZP-CSI Resource Sets
    if (cfg.cell_list[0].duplex_mode == SRSRAN_DUPLEX_MODE_FDD) {
      csi_meas_cfg.nzp_csi_rs_res_set_to_add_mod_list.resize(2);
      auto& nzp_csi_res_set = csi_meas_cfg.nzp_csi_rs_res_set_to_add_mod_list;
      // item 0
      nzp_csi_res_set[0].nzp_csi_res_set_id = 0;
      nzp_csi_res_set[0].nzp_csi_rs_res.resize(1);
      nzp_csi_res_set[0].nzp_csi_rs_res[0] = 0;
      // item 1
      nzp_csi_res_set[1].nzp_csi_res_set_id = 1;
      nzp_csi_res_set[1].nzp_csi_rs_res.resize(4);
      nzp_csi_res_set[1].nzp_csi_rs_res[0] = 1;
      nzp_csi_res_set[1].nzp_csi_rs_res[1] = 2;
      nzp_csi_res_set[1].nzp_csi_rs_res[2] = 3;
      nzp_csi_res_set[1].nzp_csi_rs_res[3] = 4;
      //    // Skip TRS info
    } else {
      csi_meas_cfg.nzp_csi_rs_res_set_to_add_mod_list.resize(1);
      auto& nzp_csi_res_set                 = csi_meas_cfg.nzp_csi_rs_res_set_to_add_mod_list;
      nzp_csi_res_set[0].nzp_csi_res_set_id = 0;
      nzp_csi_res_set[0].nzp_csi_rs_res.resize(1);
      nzp_csi_res_set[0].nzp_csi_rs_res[0] = 0;
      // Skip TRS info
    }
  }
}

/// Fill csi-ResoureConfigToAddModList
void fill_csi_resource_cfg_to_add(const rrc_nr_cfg_t& cfg, csi_meas_cfg_s& csi_meas_cfg)
{
  if (cfg.cell_list[0].duplex_mode == SRSRAN_DUPLEX_MODE_FDD) {
    csi_meas_cfg.csi_res_cfg_to_add_mod_list.resize(2);

    auto& res0          = csi_meas_cfg.csi_res_cfg_to_add_mod_list[0];
    res0.csi_res_cfg_id = 0;
    res0.bwp_id         = 0;
    res0.res_type.value = csi_res_cfg_s::res_type_opts::periodic;
    auto& nzp           = res0.csi_rs_res_set_list.set_nzp_csi_rs_ssb();
    nzp.nzp_csi_rs_res_set_list.push_back(0);

    auto& res2          = csi_meas_cfg.csi_res_cfg_to_add_mod_list[1];
    res2.csi_res_cfg_id = 1;
    res2.bwp_id         = 0;
    res2.res_type.value = csi_res_cfg_s::res_type_opts::periodic;
    auto& nzp2          = res2.csi_rs_res_set_list.set_nzp_csi_rs_ssb();
    nzp2.nzp_csi_rs_res_set_list.push_back(1);
  }
}

void fill_csi_im_resource_cfg_to_add(const rrc_nr_cfg_t& cfg, csi_meas_cfg_s& csi_meas_cfg)
{
  if (cfg.cell_list[0].duplex_mode == SRSRAN_DUPLEX_MODE_FDD) {
    // csi-IM-ResourceToAddModList
    csi_meas_cfg.csi_im_res_to_add_mod_list.resize(1);

    auto& csi_im_res                           = csi_meas_cfg.csi_im_res_to_add_mod_list[0];
    csi_im_res.csi_im_res_id                   = 0;
    csi_im_res.csi_im_res_elem_pattern_present = true;
    // csi-im-resource pattern1
    auto& csi_res_pattern1 = csi_im_res.csi_im_res_elem_pattern.set_pattern1();
    csi_res_pattern1.subcarrier_location_p1.value =
        csi_im_res_s::csi_im_res_elem_pattern_c_::pattern1_s_::subcarrier_location_p1_opts::s8;
    csi_res_pattern1.symbol_location_p1 = 8;
    // csi-im-resource freqBand
    csi_im_res.freq_band_present  = true;
    csi_im_res.freq_band.start_rb = 0;
    csi_im_res.freq_band.nrof_rbs = 52;
    // csi-im-resource periodicity_and_offset
    csi_im_res.periodicity_and_offset_present = true;
    csi_im_res.periodicity_and_offset.set_slots80();
    csi_im_res.periodicity_and_offset.slots80() = 1;

    // csi-IM-ResourceSetToAddModList
    csi_meas_cfg.csi_im_res_set_to_add_mod_list.resize(1);

    auto& csi_im_res_set             = csi_meas_cfg.csi_im_res_set_to_add_mod_list[0];
    csi_im_res_set.csi_im_res_set_id = 0;
    csi_im_res_set.csi_im_res.push_back(0);
  }
}

/// Fill CSI-MeasConfig with gNB config
int fill_csi_meas_from_enb_cfg(const rrc_nr_cfg_t& cfg, csi_meas_cfg_s& csi_meas_cfg)
{
  //  // Fill CSI Report
  //  if (fill_csi_report_from_enb_cfg(cfg, csi_meas_cfg) != SRSRAN_SUCCESS) {
  //    get_logger(cfg).error("Failed to configure eNB CSI Report");
  //    return SRSRAN_ERROR;
  //  }

  // Fill CSI resource config
  fill_csi_resource_cfg_to_add(cfg, csi_meas_cfg);

  // Fill NZP-CSI Resources
  fill_nzp_csi_rs_from_enb_cfg(cfg, csi_meas_cfg);

  if (cfg.is_standalone) {
    // CSI report config
    fill_csi_report_from_enb_cfg(cfg, csi_meas_cfg);
  }

  return SRSRAN_SUCCESS;
}

void fill_pdsch_cfg_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, pdsch_cfg_s& out)
{
  out.dmrs_dl_for_pdsch_map_type_a_present = true;
  out.dmrs_dl_for_pdsch_map_type_a.set_setup();
  out.dmrs_dl_for_pdsch_map_type_a.setup().dmrs_add_position_present = true;
  out.dmrs_dl_for_pdsch_map_type_a.setup().dmrs_add_position         = dmrs_dl_cfg_s::dmrs_add_position_opts::pos1;

  out.tci_states_to_add_mod_list.resize(1);
  out.tci_states_to_add_mod_list[0].tci_state_id = 0;
  out.tci_states_to_add_mod_list[0].qcl_type1.ref_sig.set_ssb();
  out.tci_states_to_add_mod_list[0].qcl_type1.ref_sig.ssb() = 0;
  out.tci_states_to_add_mod_list[0].qcl_type1.qcl_type      = asn1::rrc_nr::qcl_info_s::qcl_type_opts::type_d;

  out.res_alloc = pdsch_cfg_s::res_alloc_opts::res_alloc_type1;
  out.rbg_size  = pdsch_cfg_s::rbg_size_opts::cfg1;
  out.prb_bundling_type.set_static_bundling();
  out.prb_bundling_type.static_bundling().bundle_size_present = true;
  out.prb_bundling_type.static_bundling().bundle_size =
      pdsch_cfg_s::prb_bundling_type_c_::static_bundling_s_::bundle_size_opts::wideband;

  // MCS Table
  // NOTE: For Table 1 or QAM64, set false and comment value
  // out.mcs_table_present = true;
  // out.mcs_table.value = pdsch_cfg_s::mcs_table_opts::qam256;

  // ZP-CSI
  out.zp_csi_rs_res_to_add_mod_list.resize(1);
  out.zp_csi_rs_res_to_add_mod_list[0].zp_csi_rs_res_id = 0;
  out.zp_csi_rs_res_to_add_mod_list[0].res_map.freq_domain_alloc.set_row4();
  out.zp_csi_rs_res_to_add_mod_list[0].res_map.freq_domain_alloc.row4().from_number(0b100);
  out.zp_csi_rs_res_to_add_mod_list[0].res_map.nrof_ports = asn1::rrc_nr::csi_rs_res_map_s::nrof_ports_opts::p4;

  out.zp_csi_rs_res_to_add_mod_list[0].res_map.first_ofdm_symbol_in_time_domain = 8;
  out.zp_csi_rs_res_to_add_mod_list[0].res_map.cdm_type = asn1::rrc_nr::csi_rs_res_map_s::cdm_type_opts::fd_cdm2;
  out.zp_csi_rs_res_to_add_mod_list[0].res_map.density.set_one();

  out.zp_csi_rs_res_to_add_mod_list[0].res_map.freq_band.start_rb     = 0;
  out.zp_csi_rs_res_to_add_mod_list[0].res_map.freq_band.nrof_rbs     = cfg.cell_list[cc].phy_cell.carrier.nof_prb;
  out.zp_csi_rs_res_to_add_mod_list[0].periodicity_and_offset_present = true;
  out.zp_csi_rs_res_to_add_mod_list[0].periodicity_and_offset.set_slots80() = 1;

  out.p_zp_csi_rs_res_set_present = false; // TEMP
  out.p_zp_csi_rs_res_set.set_setup();
  out.p_zp_csi_rs_res_set.setup().zp_csi_rs_res_set_id = 0;
  out.p_zp_csi_rs_res_set.setup().zp_csi_rs_res_id_list.resize(1);
  out.p_zp_csi_rs_res_set.setup().zp_csi_rs_res_id_list[0] = 0;
}

/// Fill InitDlBwp with gNB config
int fill_init_dl_bwp_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, bwp_dl_ded_s& init_dl_bwp)
{
  init_dl_bwp.pdcch_cfg_present     = true;
  init_dl_bwp.pdcch_cfg.set_setup() = cfg.cell_list[cc].pdcch_cfg_ded;

  init_dl_bwp.pdsch_cfg_present = true;
  fill_pdsch_cfg_from_enb_cfg(cfg, cc, init_dl_bwp.pdsch_cfg.set_setup());

  // TODO: ADD missing fields

  return SRSRAN_SUCCESS;
}

void fill_pucch_cfg_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, pucch_cfg_s& out)
{
  // Make 2 PUCCH resource sets
  out.res_set_to_add_mod_list.resize(2);

  // Make PUCCH resource set for 1-2 bit
  for (uint32_t set_id = 0; set_id < out.res_set_to_add_mod_list.size(); ++set_id) {
    auto& res_set            = out.res_set_to_add_mod_list[set_id];
    res_set.pucch_res_set_id = set_id;
    res_set.res_list.resize(8);
    for (uint32_t i = 0; i < res_set.res_list.size(); ++i) {
      if (cfg.is_standalone) {
        res_set.res_list[i] = i + set_id * 8;
      } else {
        res_set.res_list[i] = set_id;
      }
    }
  }

  // Make 3 possible resources
  out.res_to_add_mod_list.resize(18);
  uint32_t j = 0, j2 = 0;
  for (uint32_t i = 0; i < out.res_to_add_mod_list.size(); ++i) {
    out.res_to_add_mod_list[i].pucch_res_id                = i;
    out.res_to_add_mod_list[i].intra_slot_freq_hop_present = false;
    if (i < 8 or i == 16) {
      out.res_to_add_mod_list[i].start_prb                              = 51;
      out.res_to_add_mod_list[i].second_hop_prb_present                 = true;
      out.res_to_add_mod_list[i].second_hop_prb                         = 0;
      out.res_to_add_mod_list[i].format.set_format1().init_cyclic_shift = (4 * (j % 3));
      out.res_to_add_mod_list[i].format.format1().nrof_symbols          = 14;
      out.res_to_add_mod_list[i].format.format1().start_symbol_idx      = 0;
      out.res_to_add_mod_list[i].format.format1().time_domain_occ       = j / 3;
      j++;
    } else if (i < 15) {
      out.res_to_add_mod_list[i].start_prb                         = 1;
      out.res_to_add_mod_list[i].second_hop_prb_present            = true;
      out.res_to_add_mod_list[i].second_hop_prb                    = 50;
      out.res_to_add_mod_list[i].format.set_format2().nrof_prbs    = 1;
      out.res_to_add_mod_list[i].format.format2().nrof_symbols     = 2;
      out.res_to_add_mod_list[i].format.format2().start_symbol_idx = 2 * (j2 % 7);
      j2++;
    } else {
      out.res_to_add_mod_list[i].start_prb                         = 50;
      out.res_to_add_mod_list[i].second_hop_prb_present            = true;
      out.res_to_add_mod_list[i].second_hop_prb                    = 1;
      out.res_to_add_mod_list[i].format.set_format2().nrof_prbs    = 1;
      out.res_to_add_mod_list[i].format.format2().nrof_symbols     = 2;
      out.res_to_add_mod_list[i].format.format2().start_symbol_idx = 2 * (j2 % 7);
      j2++;
    }
  }

  out.format1_present = true;
  out.format1.set_setup();

  out.format2_present = true;
  out.format2.set_setup();
  out.format2.setup().max_code_rate_present = true;
  out.format2.setup().max_code_rate         = pucch_max_code_rate_opts::zero_dot25;
  // NOTE: IMPORTANT!! The gNB expects the CSI to be reported along with HARQ-ACK
  // If simul_harq_ack_csi_present = false, PUCCH might not be decoded properly when CSI is reported
  out.format2.setup().simul_harq_ack_csi_present = true;

  // SR resources
  out.sched_request_res_to_add_mod_list.resize(1);
  auto& sr_res1                             = out.sched_request_res_to_add_mod_list[0];
  sr_res1.sched_request_res_id              = 1;
  sr_res1.sched_request_id                  = 0;
  sr_res1.periodicity_and_offset_present    = true;
  sr_res1.periodicity_and_offset.set_sl40() = 8;
  sr_res1.res_present                       = true;
  sr_res1.res                               = 2;

  // DL data
  if (cfg.cell_list[cc].duplex_mode == SRSRAN_DUPLEX_MODE_FDD) {
    out.dl_data_to_ul_ack.resize(1);
    out.dl_data_to_ul_ack[0] = 4;
  } else {
    out.dl_data_to_ul_ack.resize(6);
    out.dl_data_to_ul_ack[0] = 6;
    out.dl_data_to_ul_ack[1] = 5;
    out.dl_data_to_ul_ack[2] = 4;
    out.dl_data_to_ul_ack[3] = 4;
    out.dl_data_to_ul_ack[4] = 4;
    out.dl_data_to_ul_ack[5] = 4;
  }
}

void fill_pusch_cfg_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, pusch_cfg_s& out)
{
  out.dmrs_ul_for_pusch_map_type_a_present = true;
  out.dmrs_ul_for_pusch_map_type_a.set_setup();
  out.dmrs_ul_for_pusch_map_type_a.setup().dmrs_add_position_present = true;
  out.dmrs_ul_for_pusch_map_type_a.setup().dmrs_add_position         = dmrs_ul_cfg_s::dmrs_add_position_opts::pos1;
  // PUSH power control skipped
  out.res_alloc = pusch_cfg_s::res_alloc_opts::res_alloc_type1;

  // UCI
  out.uci_on_pusch_present = true;
  out.uci_on_pusch.set_setup();
  out.uci_on_pusch.setup().beta_offsets_present = true;
  out.uci_on_pusch.setup().beta_offsets.set_semi_static();
  auto& beta_offset_semi_static                              = out.uci_on_pusch.setup().beta_offsets.semi_static();
  beta_offset_semi_static.beta_offset_ack_idx1_present       = true;
  beta_offset_semi_static.beta_offset_ack_idx1               = 9;
  beta_offset_semi_static.beta_offset_ack_idx2_present       = true;
  beta_offset_semi_static.beta_offset_ack_idx2               = 9;
  beta_offset_semi_static.beta_offset_ack_idx3_present       = true;
  beta_offset_semi_static.beta_offset_ack_idx3               = 9;
  beta_offset_semi_static.beta_offset_csi_part1_idx1_present = true;
  beta_offset_semi_static.beta_offset_csi_part1_idx1         = 6;
  beta_offset_semi_static.beta_offset_csi_part1_idx2_present = true;
  beta_offset_semi_static.beta_offset_csi_part1_idx2         = 6;
  beta_offset_semi_static.beta_offset_csi_part2_idx1_present = true;
  beta_offset_semi_static.beta_offset_csi_part2_idx1         = 6;
  beta_offset_semi_static.beta_offset_csi_part2_idx2_present = true;
  beta_offset_semi_static.beta_offset_csi_part2_idx2         = 6;

  out.uci_on_pusch.setup().scaling = uci_on_pusch_s::scaling_opts::f1;
}

void fill_init_ul_bwp_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, bwp_ul_ded_s& out)
{
  if (cfg.is_standalone) {
    out.pucch_cfg_present = true;
    fill_pucch_cfg_from_enb_cfg(cfg, cc, out.pucch_cfg.set_setup());

    out.pusch_cfg_present = true;
    fill_pusch_cfg_from_enb_cfg(cfg, cc, out.pusch_cfg.set_setup());
  }
}

/// Fill InitUlBwp with gNB config
void fill_ul_cfg_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, ul_cfg_s& out)
{
  out.init_ul_bwp_present = true;
  fill_init_ul_bwp_from_enb_cfg(cfg, cc, out.init_ul_bwp);
}

/// Fill ServingCellConfig with gNB config
int fill_serv_cell_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, serving_cell_cfg_s& serv_cell)
{
  serv_cell.csi_meas_cfg_present = true;
  HANDLE_ERROR(fill_csi_meas_from_enb_cfg(cfg, serv_cell.csi_meas_cfg.set_setup()));

  serv_cell.init_dl_bwp_present = true;
  fill_init_dl_bwp_from_enb_cfg(cfg, cc, serv_cell.init_dl_bwp);

  serv_cell.first_active_dl_bwp_id_present = true;
  if (cfg.cell_list[0].duplex_mode == SRSRAN_DUPLEX_MODE_FDD) {
    serv_cell.first_active_dl_bwp_id = 0;
  } else {
    serv_cell.first_active_dl_bwp_id = 1;
  }

  serv_cell.ul_cfg_present = true;
  fill_ul_cfg_from_enb_cfg(cfg, cc, serv_cell.ul_cfg);

  // TODO: remaining fields

  return SRSRAN_SUCCESS;
}

void fill_pdcch_cfg_common(const rrc_nr_cfg_t& cfg, uint32_t cc, pdcch_cfg_common_s& out);

/// Fill FrequencyInfoDL with gNB config
int fill_freq_info_dl_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, freq_info_dl_s& freq_info_dl)
{
  auto& cell_cfg = cfg.cell_list.at(cc);

  freq_info_dl.freq_band_list.push_back(cell_cfg.band);
  freq_info_dl.absolute_freq_point_a     = cell_cfg.dl_absolute_freq_point_a;
  freq_info_dl.absolute_freq_ssb_present = true;
  freq_info_dl.absolute_freq_ssb         = cell_cfg.ssb_absolute_freq_point;

  freq_info_dl.scs_specific_carrier_list.resize(1);
  auto& dl_carrier             = freq_info_dl.scs_specific_carrier_list[0];
  dl_carrier.offset_to_carrier = cell_cfg.phy_cell.carrier.offset_to_carrier;

  if (!asn1::number_to_enum(dl_carrier.subcarrier_spacing,
                            SRSRAN_SUBC_SPACING_NR(cell_cfg.phy_cell.carrier.scs) / 1000)) {
    get_logger(cfg).error("Config Error: Invalid subcarrier spacing (%d).\n",
                          SRSRAN_SUBC_SPACING_NR(cell_cfg.phy_cell.carrier.scs));
    return SRSRAN_ERROR;
  }

  dl_carrier.carrier_bw = cell_cfg.phy_cell.carrier.nof_prb;

  return SRSRAN_SUCCESS;
}

/// Fill InitDlBwp with gNB config
int fill_init_dl_bwp_common_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, bwp_dl_common_s& init_dl_bwp)
{
  init_dl_bwp.pdcch_cfg_common_present = true;
  fill_pdcch_cfg_common(cfg, cc, init_dl_bwp.pdcch_cfg_common.set_setup());
  // TODO: ADD missing fields
  return SRSRAN_SUCCESS;
}

/// Fill DLCellConfigCommon with gNB config
int fill_dl_cfg_common_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, dl_cfg_common_s& dl_cfg_common)
{
  dl_cfg_common.freq_info_dl_present = true;
  HANDLE_ERROR(fill_freq_info_dl_from_enb_cfg(cfg, cc, dl_cfg_common.freq_info_dl));

  dl_cfg_common.init_dl_bwp_present = true;
  HANDLE_ERROR(fill_init_dl_bwp_common_from_enb_cfg(cfg, cc, dl_cfg_common.init_dl_bwp));

  // TODO: ADD missing fields

  return SRSRAN_SUCCESS;
}

/// Fill FrequencyInfoUL with gNB config
int fill_freq_info_ul_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, freq_info_ul_s& freq_info_ul)
{
  auto& cell_cfg = cfg.cell_list.at(cc);

  freq_info_ul.freq_band_list.push_back(cell_cfg.band);
  freq_info_ul.absolute_freq_point_a_present = true;
  freq_info_ul.absolute_freq_point_a         = cell_cfg.ul_absolute_freq_point_a;
  freq_info_ul.scs_specific_carrier_list.resize(1);

  auto& ul_carrier             = freq_info_ul.scs_specific_carrier_list[0];
  ul_carrier.offset_to_carrier = cell_cfg.phy_cell.carrier.offset_to_carrier;

  if (!asn1::number_to_enum(ul_carrier.subcarrier_spacing,
                            SRSRAN_SUBC_SPACING_NR(cell_cfg.phy_cell.carrier.scs) / 1000)) {
    get_logger(cfg).error("Config Error: Invalid subcarrier spacing (%d).\n",
                          SRSRAN_SUBC_SPACING_NR(cell_cfg.phy_cell.carrier.scs));
    return SRSRAN_ERROR;
  }

  ul_carrier.carrier_bw = cell_cfg.phy_cell.carrier.nof_prb;

  return SRSRAN_SUCCESS;
}

int fill_rach_cfg_common(const rrc_nr_cfg_t& cfg, uint32_t cc, rach_cfg_common_s& rach)
{
  // rach-ConfigGeneric
  rach.rach_cfg_generic.prach_cfg_idx = 0;
  if (cfg.cell_list[cc].duplex_mode == SRSRAN_DUPLEX_MODE_TDD) {
    // Note: Give more time margin to fit RAR
    rach.rach_cfg_generic.prach_cfg_idx = 8;
  }
  rach.rach_cfg_generic.msg1_fdm.value            = rach_cfg_generic_s::msg1_fdm_opts::one;
  rach.rach_cfg_generic.msg1_freq_start           = 1; // zero not supported with current PRACH implementation
  rach.rach_cfg_generic.zero_correlation_zone_cfg = 0;
  rach.rach_cfg_generic.preamb_rx_target_pwr      = -110;
  rach.rach_cfg_generic.preamb_trans_max.value    = rach_cfg_generic_s::preamb_trans_max_opts::n7;
  rach.rach_cfg_generic.pwr_ramp_step.value       = rach_cfg_generic_s::pwr_ramp_step_opts::db4;
  rach.rach_cfg_generic.ra_resp_win.value         = rach_cfg_generic_s::ra_resp_win_opts::sl10;

  // totalNumberOfRA-Preambles
  if (cfg.cell_list[cc].num_ra_preambles != 64) {
    rach.total_nof_ra_preambs_present = true;
    rach.total_nof_ra_preambs         = cfg.cell_list[cc].num_ra_preambles;
  }

  // ssb-perRACH-OccasionAndCB-PreamblesPerSSB
  rach.ssb_per_rach_occasion_and_cb_preambs_per_ssb_present = true;
  if (not asn1::number_to_enum(rach.ssb_per_rach_occasion_and_cb_preambs_per_ssb.set_one(),
                               cfg.cell_list[cc].num_ra_preambles)) {
    get_logger(cfg).error("Invalid number of RA preambles=%d", cfg.cell_list[cc].num_ra_preambles);
    return -1;
  }

  rach.ra_contention_resolution_timer.value = rach_cfg_common_s::ra_contention_resolution_timer_opts::sf64;
  rach.prach_root_seq_idx.set_l839()        = cfg.cell_list[cc].prach_root_seq_idx;
  rach.restricted_set_cfg.value             = rach_cfg_common_s::restricted_set_cfg_opts::unrestricted_set;

  return SRSRAN_SUCCESS;
}

/// Fill InitUlBwp with gNB config
int fill_init_ul_bwp_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, bwp_ul_common_s& init_ul_bwp)
{
  init_ul_bwp.rach_cfg_common_present = true;
  HANDLE_ERROR(fill_rach_cfg_common(cfg, cc, init_ul_bwp.rach_cfg_common.set_setup()));

  // TODO: Add missing fields

  return SRSRAN_SUCCESS;
}

/// Fill ULCellConfigCommon with gNB config
int fill_ul_cfg_common_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, ul_cfg_common_s& ul_cfg_common)
{
  ul_cfg_common.dummy = time_align_timer_opts::ms500;

  ul_cfg_common.freq_info_ul_present = true;
  HANDLE_ERROR(fill_freq_info_ul_from_enb_cfg(cfg, cc, ul_cfg_common.freq_info_ul));

  ul_cfg_common.init_ul_bwp_present = true;
  fill_init_ul_bwp_from_enb_cfg(cfg, cc, ul_cfg_common.init_ul_bwp);

  // TODO: Add missing fields

  return SRSRAN_SUCCESS;
}

/// Fill ServingCellConfigCommon with gNB config
int fill_serv_cell_common_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, serving_cell_cfg_common_s& serv_common)
{
  auto& cell_cfg = cfg.cell_list.at(cc);

  serv_common.ss_pbch_block_pwr               = cell_cfg.pdsch_rs_power;
  serv_common.n_timing_advance_offset_present = true;
  serv_common.n_timing_advance_offset         = serving_cell_cfg_common_s::n_timing_advance_offset_opts::n0;
  serv_common.n_timing_advance_offset_present = true;
  serv_common.dmrs_type_a_position            = serving_cell_cfg_common_s::dmrs_type_a_position_opts::pos2;

  serv_common.pci_present = true;
  serv_common.pci         = cell_cfg.phy_cell.carrier.pci;

  serv_common.ssb_periodicity_serving_cell_present = true;
  serv_common.ssb_periodicity_serving_cell.value   = serving_cell_cfg_common_s::ssb_periodicity_serving_cell_opts::ms10;

  // Fill SSB config
  serv_common.ssb_positions_in_burst_present = true;
  if (cfg.cell_list[cc].duplex_mode == SRSRAN_DUPLEX_MODE_FDD) {
    serv_common.ssb_positions_in_burst.set_short_bitmap().from_number(0b1000);
  } else {
    serv_common.ssb_positions_in_burst.set_medium_bitmap().from_number(0b10000000);
  }

  // Set SSB SCS
  serv_common.ssb_subcarrier_spacing_present = true;
  if (cfg.cell_list[cc].duplex_mode == SRSRAN_DUPLEX_MODE_FDD) {
    serv_common.ssb_subcarrier_spacing = subcarrier_spacing_opts::khz15;
  } else {
    serv_common.ssb_subcarrier_spacing = subcarrier_spacing_opts::khz30;
  }

  if (cfg.cell_list[cc].duplex_mode == SRSRAN_DUPLEX_MODE_TDD) {
    // TDD UL-DL config
    serv_common.tdd_ul_dl_cfg_common_present = true;
    fill_tdd_ul_dl_config_common(cfg.cell_list[cc], serv_common.tdd_ul_dl_cfg_common);
  }

  serv_common.ul_cfg_common_present = true;
  fill_ul_cfg_common_from_enb_cfg(cfg, cc, serv_common.ul_cfg_common);

  serv_common.dl_cfg_common_present = true;
  fill_dl_cfg_common_from_enb_cfg(cfg, cc, serv_common.dl_cfg_common);

  return SRSRAN_SUCCESS;
}

/// Fill reconfigurationWithSync with gNB config
int fill_recfg_with_sync_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, recfg_with_sync_s& sync)
{
  sync.sp_cell_cfg_common_present = true;
  HANDLE_ERROR(fill_serv_cell_common_from_enb_cfg(cfg, cc, sync.sp_cell_cfg_common));

  return SRSRAN_SUCCESS;
}

/// Fill spCellConfig with gNB config
int fill_sp_cell_cfg_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, sp_cell_cfg_s& sp_cell)
{
  if (not cfg.is_standalone) {
    sp_cell.recfg_with_sync_present = true;
  }
  HANDLE_ERROR(fill_recfg_with_sync_from_enb_cfg(cfg, cc, sp_cell.recfg_with_sync));

  sp_cell.sp_cell_cfg_ded_present = true;
  HANDLE_ERROR(fill_serv_cell_from_enb_cfg(cfg, cc, sp_cell.sp_cell_cfg_ded));

  return SRSRAN_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Fill SRB with parameters derived from cfg
void fill_srb(const rrc_nr_cfg_t& cfg, srsran::nr_srb srb_id, asn1::rrc_nr::rlc_bearer_cfg_s& out)
{
  srsran_assert(srb_id > srsran::nr_srb::srb0 and srb_id < srsran::nr_srb::count, "Invalid srb_id argument");

  out.lc_ch_id                         = srsran::srb_to_lcid(srb_id);
  out.served_radio_bearer_present      = true;
  out.served_radio_bearer.set_srb_id() = (uint8_t)srb_id;

  if (srb_id == srsran::nr_srb::srb1) {
    if (cfg.srb1_cfg.present) {
      out.rlc_cfg_present = true;
      out.rlc_cfg         = cfg.srb1_cfg.rlc_cfg;
    } else {
      out.rlc_cfg_present = false;
    }
  } else if (srb_id == srsran::nr_srb::srb2) {
    if (cfg.srb2_cfg.present) {
      out.rlc_cfg_present = true;
      out.rlc_cfg         = cfg.srb2_cfg.rlc_cfg;
    } else {
      out.rlc_cfg_present = false;
    }
  } else {
    out.rlc_cfg_present           = true;
    auto& ul_am                   = out.rlc_cfg.set_am().ul_am_rlc;
    ul_am.sn_field_len_present    = true;
    ul_am.sn_field_len.value      = asn1::rrc_nr::sn_field_len_am_opts::size12;
    ul_am.t_poll_retx.value       = asn1::rrc_nr::t_poll_retx_opts::ms45;
    ul_am.poll_pdu.value          = asn1::rrc_nr::poll_pdu_opts::infinity;
    ul_am.poll_byte.value         = asn1::rrc_nr::poll_byte_opts::infinity;
    ul_am.max_retx_thres.value    = asn1::rrc_nr::ul_am_rlc_s::max_retx_thres_opts::t8;
    auto& dl_am                   = out.rlc_cfg.am().dl_am_rlc;
    dl_am.sn_field_len_present    = true;
    dl_am.sn_field_len.value      = asn1::rrc_nr::sn_field_len_am_opts::size12;
    dl_am.t_reassembly.value      = t_reassembly_opts::ms35;
    dl_am.t_status_prohibit.value = asn1::rrc_nr::t_status_prohibit_opts::ms0;
  }

  // mac-LogicalChannelConfig -- Cond LCH-Setup
  out.mac_lc_ch_cfg_present                    = true;
  out.mac_lc_ch_cfg.ul_specific_params_present = true;
  out.mac_lc_ch_cfg.ul_specific_params.prio    = srb_id == srsran::nr_srb::srb1 ? 1 : 3;
  out.mac_lc_ch_cfg.ul_specific_params.prioritised_bit_rate.value =
      lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_opts::infinity;
  out.mac_lc_ch_cfg.ul_specific_params.bucket_size_dur.value =
      lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_opts::ms5;
  out.mac_lc_ch_cfg.ul_specific_params.lc_ch_group_present          = true;
  out.mac_lc_ch_cfg.ul_specific_params.lc_ch_group                  = 0;
  out.mac_lc_ch_cfg.ul_specific_params.sched_request_id_present     = true;
  out.mac_lc_ch_cfg.ul_specific_params.sched_request_id             = 0;
  out.mac_lc_ch_cfg.ul_specific_params.lc_ch_sr_mask                = false;
  out.mac_lc_ch_cfg.ul_specific_params.lc_ch_sr_delay_timer_applied = false;
}

/// Fill DRB with parameters derived from cfg
void fill_drb(const rrc_nr_cfg_t&                       cfg,
              const enb_bearer_manager::radio_bearer_t& rb,
              srsran::nr_drb                            drb_id,
              asn1::rrc_nr::rlc_bearer_cfg_s&           out)
{
  out.lc_ch_id                         = rb.lcid;
  out.served_radio_bearer_present      = true;
  out.served_radio_bearer.set_drb_id() = (uint8_t)drb_id;

  out.rlc_cfg_present = true;
  out.rlc_cfg         = cfg.five_qi_cfg.at(rb.five_qi).rlc_cfg;

  // MAC logical channel config
  out.mac_lc_ch_cfg_present                    = true;
  out.mac_lc_ch_cfg.ul_specific_params_present = true;
  out.mac_lc_ch_cfg.ul_specific_params.prio    = 11; // TODO
  out.mac_lc_ch_cfg.ul_specific_params.prioritised_bit_rate =
      lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_opts::kbps0;
  out.mac_lc_ch_cfg.ul_specific_params.bucket_size_dur =
      asn1::rrc_nr::lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_opts::ms100;
  out.mac_lc_ch_cfg.ul_specific_params.lc_ch_group_present          = true;
  out.mac_lc_ch_cfg.ul_specific_params.lc_ch_group                  = 3; // TODO
  out.mac_lc_ch_cfg.ul_specific_params.sched_request_id_present     = true;
  out.mac_lc_ch_cfg.ul_specific_params.sched_request_id             = 0; // TODO
  out.mac_lc_ch_cfg.ul_specific_params.lc_ch_sr_mask                = false;
  out.mac_lc_ch_cfg.ul_specific_params.lc_ch_sr_delay_timer_applied = false;
  // TODO: add LC config to MAC
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Fill MasterCellConfig with gNB config
int fill_master_cell_cfg_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, asn1::rrc_nr::cell_group_cfg_s& out)
{
  out.cell_group_id = 0;
  out.rlc_bearer_to_add_mod_list.resize(1);
  fill_srb(cfg, srsran::nr_srb::srb1, out.rlc_bearer_to_add_mod_list[0]);

  // mac-CellGroupConfig -- Need M
  out.mac_cell_group_cfg_present                   = true;
  out.mac_cell_group_cfg.sched_request_cfg_present = true;
  out.mac_cell_group_cfg.sched_request_cfg.sched_request_to_add_mod_list.resize(1);
  out.mac_cell_group_cfg.sched_request_cfg.sched_request_to_add_mod_list[0].sched_request_id = 0;
  out.mac_cell_group_cfg.sched_request_cfg.sched_request_to_add_mod_list[0].sr_trans_max.value =
      sched_request_to_add_mod_s::sr_trans_max_opts::n64;
  out.mac_cell_group_cfg.bsr_cfg_present                  = true;
  out.mac_cell_group_cfg.bsr_cfg.periodic_bsr_timer.value = bsr_cfg_s::periodic_bsr_timer_opts::sf20;
  out.mac_cell_group_cfg.bsr_cfg.retx_bsr_timer.value     = bsr_cfg_s::retx_bsr_timer_opts::sf320;
  out.mac_cell_group_cfg.tag_cfg_present                  = true;
  out.mac_cell_group_cfg.tag_cfg.tag_to_add_mod_list.resize(1);
  out.mac_cell_group_cfg.tag_cfg.tag_to_add_mod_list[0].tag_id                 = 0;
  out.mac_cell_group_cfg.tag_cfg.tag_to_add_mod_list[0].time_align_timer.value = time_align_timer_opts::infinity;
  out.mac_cell_group_cfg.phr_cfg_present                                       = true;
  phr_cfg_s& phr                            = out.mac_cell_group_cfg.phr_cfg.set_setup();
  phr.phr_periodic_timer.value              = asn1::rrc_nr::phr_cfg_s::phr_periodic_timer_opts::sf500;
  phr.phr_prohibit_timer.value              = asn1::rrc_nr::phr_cfg_s::phr_prohibit_timer_opts::sf200;
  phr.phr_tx_pwr_factor_change.value        = asn1::rrc_nr::phr_cfg_s::phr_tx_pwr_factor_change_opts::db3;
  phr.multiple_phr                          = false;
  phr.dummy                                 = false;
  phr.phr_type2_other_cell                  = false;
  phr.phr_mode_other_cg.value               = asn1::rrc_nr::phr_cfg_s::phr_mode_other_cg_opts::real;
  out.mac_cell_group_cfg.skip_ul_tx_dynamic = false;
  out.mac_cell_group_cfg.phr_cfg_present    = false; // Note: not supported

  // physicalCellGroupConfig -- Need M
  out.phys_cell_group_cfg_present          = true;
  out.phys_cell_group_cfg.p_nr_fr1_present = true;
  out.phys_cell_group_cfg.p_nr_fr1         = 10;
  out.phys_cell_group_cfg.pdsch_harq_ack_codebook.value =
      phys_cell_group_cfg_s::pdsch_harq_ack_codebook_opts::dynamic_value;

  // spCellConfig -- Need M
  out.sp_cell_cfg_present = true;
  fill_sp_cell_cfg_from_enb_cfg(cfg, cc, out.sp_cell_cfg);

  return SRSRAN_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int fill_mib_from_enb_cfg(const rrc_cell_cfg_nr_t& cell_cfg, asn1::rrc_nr::mib_s& mib)
{
  mib.sys_frame_num.from_number(0);
  switch (cell_cfg.phy_cell.carrier.scs) {
    case srsran_subcarrier_spacing_15kHz:
    case srsran_subcarrier_spacing_60kHz:
      mib.sub_carrier_spacing_common.value = mib_s::sub_carrier_spacing_common_opts::scs15or60;
      break;
    case srsran_subcarrier_spacing_30kHz:
    case srsran_subcarrier_spacing_120kHz:
      mib.sub_carrier_spacing_common.value = mib_s::sub_carrier_spacing_common_opts::scs30or120;
      break;
    default:
      srsran_terminate("Invalid carrier SCS=%d Hz", SRSRAN_SUBC_SPACING_NR(cell_cfg.phy_cell.carrier.scs));
  }
  mib.ssb_subcarrier_offset            = cell_cfg.ssb_offset;
  mib.dmrs_type_a_position.value       = mib_s::dmrs_type_a_position_opts::pos2;
  mib.pdcch_cfg_sib1.search_space_zero = 0;
  mib.pdcch_cfg_sib1.ctrl_res_set_zero = cell_cfg.coreset0_idx;
  mib.cell_barred.value                = mib_s::cell_barred_opts::not_barred;
  mib.intra_freq_resel.value           = mib_s::intra_freq_resel_opts::allowed;
  mib.spare.from_number(0);
  return SRSRAN_SUCCESS;
}

// Called for SA and NSA
void fill_pdcch_cfg_common(const rrc_nr_cfg_t& cfg, uint32_t cc, pdcch_cfg_common_s& out)
{
  auto& cell_cfg = cfg.cell_list[cc];

  out.ctrl_res_set_zero_present = false;
  out.search_space_zero_present = false;

  out.common_ctrl_res_set_present = cell_cfg.pdcch_cfg_common.common_ctrl_res_set_present;
  out.common_ctrl_res_set         = cell_cfg.pdcch_cfg_common.common_ctrl_res_set;
  out.common_search_space_list    = cell_cfg.pdcch_cfg_common.common_search_space_list;

  out.search_space_sib1_present           = true;
  out.search_space_sib1                   = 0;
  out.search_space_other_sys_info_present = true;
  out.search_space_other_sys_info         = 1;
  out.paging_search_space_present         = true;
  out.paging_search_space                 = 1;
  out.ra_search_space_present             = true;
  out.ra_search_space                     = 1;
}

void fill_pdsch_cfg_common(const rrc_cell_cfg_nr_t& cell_cfg, pdsch_cfg_common_s& cfg)
{
  cfg.pdsch_time_domain_alloc_list.resize(1);
  cfg.pdsch_time_domain_alloc_list[0].map_type.value       = pdsch_time_domain_res_alloc_s::map_type_opts::type_a;
  cfg.pdsch_time_domain_alloc_list[0].start_symbol_and_len = 40;
}

// Called for SA
void fill_init_dl_bwp(const rrc_nr_cfg_t& cfg, uint32_t cc, bwp_dl_common_s& out)
{
  auto& cell_cfg = cfg.cell_list[cc];

  out.generic_params.location_and_bw    = 14025;
  out.generic_params.subcarrier_spacing = (subcarrier_spacing_opts::options)cell_cfg.phy_cell.carrier.scs;

  out.pdcch_cfg_common_present = true;
  fill_pdcch_cfg_common(cfg, cc, out.pdcch_cfg_common.set_setup());
  out.pdsch_cfg_common_present = true;
  fill_pdsch_cfg_common(cell_cfg, out.pdsch_cfg_common.set_setup());
}

void fill_dl_cfg_common_sib(const rrc_nr_cfg_t& cfg, uint32_t cc, dl_cfg_common_sib_s& out)
{
  auto& cell_cfg = cfg.cell_list[cc];

  uint32_t scs_hz = SRSRAN_SUBC_SPACING_NR(cell_cfg.phy_cell.carrier.scs);
  uint32_t prb_bw = scs_hz * SRSRAN_NRE;

  srsran::srsran_band_helper band_helper;
  out.freq_info_dl.freq_band_list.resize(1);
  out.freq_info_dl.freq_band_list[0].freq_band_ind_nr_present = true;
  out.freq_info_dl.freq_band_list[0].freq_band_ind_nr         = cell_cfg.band;
  double   ssb_freq_start                                     = cell_cfg.ssb_freq_hz - SRSRAN_SSB_BW_SUBC * scs_hz / 2;
  double   offset_point_a_hz         = ssb_freq_start - band_helper.nr_arfcn_to_freq(cell_cfg.dl_absolute_freq_point_a);
  uint32_t offset_point_a_prbs       = offset_point_a_hz / prb_bw;
  out.freq_info_dl.offset_to_point_a = offset_point_a_prbs;
  out.freq_info_dl.scs_specific_carrier_list.resize(1);
  out.freq_info_dl.scs_specific_carrier_list[0].offset_to_carrier = cell_cfg.phy_cell.carrier.offset_to_carrier;
  out.freq_info_dl.scs_specific_carrier_list[0].subcarrier_spacing =
      (subcarrier_spacing_opts::options)cell_cfg.phy_cell.carrier.scs;
  out.freq_info_dl.scs_specific_carrier_list[0].carrier_bw = cell_cfg.phy_cell.carrier.nof_prb;

  fill_init_dl_bwp(cfg, cc, out.init_dl_bwp);
  // disable InitialBWP-Only fields
  out.init_dl_bwp.pdcch_cfg_common.setup().ctrl_res_set_zero_present = false;
  out.init_dl_bwp.pdcch_cfg_common.setup().search_space_zero_present = false;

  out.bcch_cfg.mod_period_coeff.value = bcch_cfg_s::mod_period_coeff_opts::n4;

  out.pcch_cfg.default_paging_cycle.value = paging_cycle_opts::rf128;
  out.pcch_cfg.nand_paging_frame_offset.set_one_t();
  out.pcch_cfg.ns.value = pcch_cfg_s::ns_opts::one;
}

void fill_ul_cfg_common_sib(const rrc_nr_cfg_t& cfg, uint32_t cc, ul_cfg_common_sib_s& out)
{
  auto&                      cell_cfg = cfg.cell_list[cc];
  srsran::srsran_band_helper band_helper;

  out.freq_info_ul.freq_band_list.resize(1);
  out.freq_info_ul.freq_band_list[0].freq_band_ind_nr_present = true;
  out.freq_info_ul.freq_band_list[0].freq_band_ind_nr         = cell_cfg.band;

  out.freq_info_ul.absolute_freq_point_a_present = true;
  out.freq_info_ul.absolute_freq_point_a =
      band_helper.get_abs_freq_point_a_arfcn(cell_cfg.phy_cell.carrier.nof_prb, cell_cfg.ul_arfcn);

  out.freq_info_ul.scs_specific_carrier_list.resize(1);
  out.freq_info_ul.scs_specific_carrier_list[0].offset_to_carrier = cell_cfg.phy_cell.carrier.offset_to_carrier;
  out.freq_info_ul.scs_specific_carrier_list[0].subcarrier_spacing =
      (subcarrier_spacing_opts::options)cell_cfg.phy_cell.carrier.scs;
  out.freq_info_ul.scs_specific_carrier_list[0].carrier_bw = cell_cfg.phy_cell.carrier.nof_prb;

  out.freq_info_ul.p_max_present = true;
  out.freq_info_ul.p_max         = 10;

  out.init_ul_bwp.generic_params.location_and_bw = 14025;
  out.init_ul_bwp.generic_params.subcarrier_spacing.value =
      (subcarrier_spacing_opts::options)cell_cfg.phy_cell.carrier.scs;

  out.init_ul_bwp.rach_cfg_common_present = true;
  fill_rach_cfg_common(cfg, cc, out.init_ul_bwp.rach_cfg_common.set_setup());

  out.init_ul_bwp.pusch_cfg_common_present = true;
  pusch_cfg_common_s& pusch                = out.init_ul_bwp.pusch_cfg_common.set_setup();
  pusch.pusch_time_domain_alloc_list.resize(1);
  pusch.pusch_time_domain_alloc_list[0].k2_present           = true;
  pusch.pusch_time_domain_alloc_list[0].k2                   = 4;
  pusch.pusch_time_domain_alloc_list[0].map_type.value       = pusch_time_domain_res_alloc_s::map_type_opts::type_a;
  pusch.pusch_time_domain_alloc_list[0].start_symbol_and_len = 27;
  pusch.p0_nominal_with_grant_present                        = true;
  pusch.p0_nominal_with_grant                                = -76;

  out.init_ul_bwp.pucch_cfg_common_present = true;
  pucch_cfg_common_s& pucch                = out.init_ul_bwp.pucch_cfg_common.set_setup();
  pucch.pucch_res_common_present           = true;
  pucch.pucch_res_common                   = 11;
  pucch.pucch_group_hop.value              = pucch_cfg_common_s::pucch_group_hop_opts::neither;
  pucch.p0_nominal_present                 = true;
  pucch.p0_nominal                         = -90;

  out.time_align_timer_common.value = time_align_timer_opts::infinity;
}

int fill_serv_cell_cfg_common_sib(const rrc_nr_cfg_t& cfg, uint32_t cc, serving_cell_cfg_common_sib_s& out)
{
  auto& cell_cfg = cfg.cell_list[cc];

  fill_dl_cfg_common_sib(cfg, cc, out.dl_cfg_common);

  out.ul_cfg_common_present = true;
  fill_ul_cfg_common_sib(cfg, cc, out.ul_cfg_common);

  out.ssb_positions_in_burst.in_one_group.from_number(0x80);

  out.ssb_periodicity_serving_cell.value = serving_cell_cfg_common_sib_s::ssb_periodicity_serving_cell_opts::ms10;

  // The time advance offset is not supported by the current PHY
  out.n_timing_advance_offset_present = true;
  out.n_timing_advance_offset         = serving_cell_cfg_common_sib_s::n_timing_advance_offset_opts::n0;

  // TDD UL-DL config
  if (cell_cfg.duplex_mode == SRSRAN_DUPLEX_MODE_TDD) {
    out.tdd_ul_dl_cfg_common_present = true;
    fill_tdd_ul_dl_config_common(cell_cfg, out.tdd_ul_dl_cfg_common);
  }

  out.ss_pbch_block_pwr = cell_cfg.pdsch_rs_power;

  return SRSRAN_SUCCESS;
}

int fill_sib1_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, asn1::rrc_nr::sib1_s& sib1)
{
  const rrc_cell_cfg_nr_t& cell_cfg = cfg.cell_list[cc];

  sib1.cell_sel_info_present            = true;
  sib1.cell_sel_info.q_rx_lev_min       = -70;
  sib1.cell_sel_info.q_qual_min_present = true;
  sib1.cell_sel_info.q_qual_min         = -20;

  sib1.cell_access_related_info.plmn_id_list.resize(1);
  sib1.cell_access_related_info.plmn_id_list[0].plmn_id_list.resize(1);
  srsran::plmn_id_t plmn;
  plmn.from_number(cfg.mcc, cfg.mnc);
  srsran::to_asn1(&sib1.cell_access_related_info.plmn_id_list[0].plmn_id_list[0], plmn);
  sib1.cell_access_related_info.plmn_id_list[0].tac_present = true;
  sib1.cell_access_related_info.plmn_id_list[0].tac.from_number(cell_cfg.tac);
  sib1.cell_access_related_info.plmn_id_list[0].cell_id.from_number((cfg.enb_id << 8U) + cell_cfg.phy_cell.cell_id);
  sib1.cell_access_related_info.plmn_id_list[0].cell_reserved_for_oper.value =
      plmn_id_info_s::cell_reserved_for_oper_opts::not_reserved;

  sib1.conn_est_fail_ctrl_present                   = true;
  sib1.conn_est_fail_ctrl.conn_est_fail_count.value = conn_est_fail_ctrl_s::conn_est_fail_count_opts::n1;
  sib1.conn_est_fail_ctrl.conn_est_fail_offset_validity.value =
      conn_est_fail_ctrl_s::conn_est_fail_offset_validity_opts::s30;
  sib1.conn_est_fail_ctrl.conn_est_fail_offset_present = true;
  sib1.conn_est_fail_ctrl.conn_est_fail_offset         = 1;

  //  sib1.si_sched_info_present                                  = true;
  //  sib1.si_sched_info.si_request_cfg.rach_occasions_si_present = true;
  //  sib1.si_sched_info.si_request_cfg.rach_occasions_si.rach_cfg_si.ra_resp_win.value =
  //      rach_cfg_generic_s::ra_resp_win_opts::sl8;
  //  sib1.si_sched_info.si_win_len.value = si_sched_info_s::si_win_len_opts::s20;
  //  sib1.si_sched_info.sched_info_list.resize(1);
  //  sib1.si_sched_info.sched_info_list[0].si_broadcast_status.value =
  //  sched_info_s::si_broadcast_status_opts::broadcasting; sib1.si_sched_info.sched_info_list[0].si_periodicity.value =
  //  sched_info_s::si_periodicity_opts::rf16; sib1.si_sched_info.sched_info_list[0].sib_map_info.resize(1);
  //  // scheduling of SI messages
  //  sib1.si_sched_info.sched_info_list[0].sib_map_info[0].type.value        = sib_type_info_s::type_opts::sib_type2;
  //  sib1.si_sched_info.sched_info_list[0].sib_map_info[0].value_tag_present = true;
  //  sib1.si_sched_info.sched_info_list[0].sib_map_info[0].value_tag         = 0;

  sib1.serving_cell_cfg_common_present = true;
  HANDLE_ERROR(fill_serv_cell_cfg_common_sib(cfg, cc, sib1.serving_cell_cfg_common));

  sib1.ue_timers_and_consts_present    = true;
  sib1.ue_timers_and_consts.t300.value = ue_timers_and_consts_s::t300_opts::ms1000;
  sib1.ue_timers_and_consts.t301.value = ue_timers_and_consts_s::t301_opts::ms1000;
  sib1.ue_timers_and_consts.t310.value = ue_timers_and_consts_s::t310_opts::ms1000;
  sib1.ue_timers_and_consts.n310.value = ue_timers_and_consts_s::n310_opts::n1;
  sib1.ue_timers_and_consts.t311.value = ue_timers_and_consts_s::t311_opts::ms30000;
  sib1.ue_timers_and_consts.n311.value = ue_timers_and_consts_s::n311_opts::n1;
  sib1.ue_timers_and_consts.t319.value = ue_timers_and_consts_s::t319_opts::ms1000;

  return SRSRAN_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool compute_diff_radio_bearer_cfg(const rrc_nr_cfg_t&       cfg,
                                   const radio_bearer_cfg_s& prev_bearers,
                                   const radio_bearer_cfg_s& next_bearers,
                                   radio_bearer_cfg_s&       diff)
{
  // Compute SRB differences
  std::vector<uint8_t> srbs_to_rem;
  srsran::compute_cfg_diff(
      prev_bearers.srb_to_add_mod_list, next_bearers.srb_to_add_mod_list, diff.srb_to_add_mod_list, srbs_to_rem);

  // Compute DRB differences
  srsran::compute_cfg_diff(prev_bearers.drb_to_add_mod_list,
                           next_bearers.drb_to_add_mod_list,
                           diff.drb_to_add_mod_list,
                           diff.drb_to_release_list);

  return diff.srb_to_add_mod_list.size() > 0 or diff.drb_to_release_list.size() > 0 or
         diff.drb_to_add_mod_list.size() > 0;
}

int fill_cellgroup_with_radio_bearer_cfg(const rrc_nr_cfg_t&                     cfg,
                                         const uint32_t                          rnti,
                                         const enb_bearer_manager&               bearer_mapper,
                                         const asn1::rrc_nr::radio_bearer_cfg_s& bearers,
                                         asn1::rrc_nr::cell_group_cfg_s&         out)
{
  out.rlc_bearer_to_add_mod_list.clear();
  out.rlc_bearer_to_release_list.clear();

  // Add SRBs
  for (const srb_to_add_mod_s& srb : bearers.srb_to_add_mod_list) {
    out.rlc_bearer_to_add_mod_list.push_back({});
    fill_srb(cfg, (srsran::nr_srb)srb.srb_id, out.rlc_bearer_to_add_mod_list.back());
  }
  // Add DRBs
  for (const drb_to_add_mod_s& drb : bearers.drb_to_add_mod_list) {
    out.rlc_bearer_to_add_mod_list.push_back({});
    uint32_t                           lcid = drb.drb_id + srsran::MAX_NR_SRB_ID;
    enb_bearer_manager::radio_bearer_t rb   = bearer_mapper.get_lcid_bearer(rnti, lcid);
    if (rb.is_valid() and cfg.five_qi_cfg.find(rb.five_qi) != cfg.five_qi_cfg.end()) {
      fill_drb(cfg, rb, (srsran::nr_drb)drb.drb_id, out.rlc_bearer_to_add_mod_list.back());
    } else {
      return SRSRAN_ERROR;
    }
  }

  // Release DRBs
  for (uint8_t drb_id : bearers.drb_to_release_list) {
    out.rlc_bearer_to_release_list.push_back(drb_id);
  }

  return SRSRAN_SUCCESS;
}

} // namespace srsenb
