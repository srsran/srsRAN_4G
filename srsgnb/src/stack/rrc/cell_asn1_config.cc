/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/hdr/stack/rrc/cell_asn1_config.h"
#include "srsran/rrc/nr/rrc_nr_cfg_utils.h"
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

/// Fill list of CSI-ReportConfig with gNB config
int fill_csi_report_from_enb_cfg(const rrc_nr_cfg_t& cfg, csi_meas_cfg_s& csi_meas_cfg)
{
  csi_meas_cfg.csi_report_cfg_to_add_mod_list_present = true;
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
  csi_report.report_freq_cfg.cqi_format_ind =
      asn1::rrc_nr::csi_report_cfg_s::report_freq_cfg_s_::cqi_format_ind_opts::wideband_cqi;
  csi_report.time_restrict_for_ch_meass = asn1::rrc_nr::csi_report_cfg_s::time_restrict_for_ch_meass_opts::not_cfgured;
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

  return SRSRAN_SUCCESS;
}

/// Fill lists of NZP-CSI-RS-Resource and NZP-CSI-RS-ResourceSet with gNB config
void fill_nzp_csi_rs_from_enb_cfg(const rrc_nr_cfg_t& cfg, csi_meas_cfg_s& csi_meas_cfg)
{
  csi_meas_cfg.nzp_csi_rs_res_to_add_mod_list_present = true;
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
  csi_meas_cfg.nzp_csi_rs_res_set_to_add_mod_list_present = true;
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

/// Fill csi-ResoureConfigToAddModList
void fill_csi_resource_cfg_to_add(const rrc_nr_cfg_t& cfg, csi_meas_cfg_s& csi_meas_cfg)
{
  if (cfg.cell_list[0].duplex_mode == SRSRAN_DUPLEX_MODE_FDD) {
    csi_meas_cfg.csi_res_cfg_to_add_mod_list_present = true;
    csi_meas_cfg.csi_res_cfg_to_add_mod_list.resize(3);

    csi_meas_cfg.csi_res_cfg_to_add_mod_list[0].csi_res_cfg_id = 0;
    auto& nzp = csi_meas_cfg.csi_res_cfg_to_add_mod_list[0].csi_rs_res_set_list.set_nzp_csi_rs_ssb();
    nzp.nzp_csi_rs_res_set_list_present = true;
    nzp.nzp_csi_rs_res_set_list.push_back(0);
    csi_meas_cfg.csi_res_cfg_to_add_mod_list[0].bwp_id         = 0;
    csi_meas_cfg.csi_res_cfg_to_add_mod_list[0].res_type.value = csi_res_cfg_s::res_type_opts::periodic;

    csi_meas_cfg.csi_res_cfg_to_add_mod_list[1].csi_res_cfg_id = 1;
    auto& imres = csi_meas_cfg.csi_res_cfg_to_add_mod_list[1].csi_rs_res_set_list.set_csi_im_res_set_list();
    imres.push_back(0);
    csi_meas_cfg.csi_res_cfg_to_add_mod_list[1].bwp_id         = 0;
    csi_meas_cfg.csi_res_cfg_to_add_mod_list[1].res_type.value = csi_res_cfg_s::res_type_opts::periodic;

    csi_meas_cfg.csi_res_cfg_to_add_mod_list[2].csi_res_cfg_id = 2;
    auto& nzp2 = csi_meas_cfg.csi_res_cfg_to_add_mod_list[2].csi_rs_res_set_list.set_nzp_csi_rs_ssb();
    nzp2.nzp_csi_rs_res_set_list_present = true;
    nzp2.nzp_csi_rs_res_set_list.push_back(1);
    csi_meas_cfg.csi_res_cfg_to_add_mod_list[2].bwp_id         = 0;
    csi_meas_cfg.csi_res_cfg_to_add_mod_list[2].res_type.value = csi_res_cfg_s::res_type_opts::periodic;
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

  // CSI IM config
  // TODO: add csi im config

  // CSI resource config
  // TODO: add csi resource config

  return SRSRAN_SUCCESS;
}

/// Fill InitDlBwp with gNB config
int fill_pdcch_cfg_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, pdcch_cfg_s& pdcch_cfg)
{
  auto& cell_cfg = cfg.cell_list.at(cc);

  // Note: Skip CORESET#0
  for (uint32_t cs_idx = 1; cs_idx < SRSRAN_UE_DL_NR_MAX_NOF_CORESET; cs_idx++) {
    if (cell_cfg.phy_cell.pdcch.coreset_present[cs_idx]) {
      auto& coreset_cfg = cell_cfg.phy_cell.pdcch.coreset[cs_idx];

      pdcch_cfg.ctrl_res_set_to_add_mod_list_present = true;

      uint8_t cs_mod_list_idx = pdcch_cfg.ctrl_res_set_to_add_mod_list.size();
      pdcch_cfg.ctrl_res_set_to_add_mod_list.resize(cs_mod_list_idx + 1);
      auto& ctrl_res_items                            = pdcch_cfg.ctrl_res_set_to_add_mod_list;
      ctrl_res_items[cs_mod_list_idx].ctrl_res_set_id = coreset_cfg.id;

      std::bitset<SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE> freq_domain_res;
      for (uint32_t i = 0; i < SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE; i++) {
        freq_domain_res[SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE - 1 - i] = coreset_cfg.freq_resources[i];
      }

      ctrl_res_items[cs_mod_list_idx].freq_domain_res.from_number(freq_domain_res.to_ulong());
      ctrl_res_items[cs_mod_list_idx].dur = coreset_cfg.duration;

      if (coreset_cfg.mapping_type == srsran_coreset_mapping_type_non_interleaved) {
        ctrl_res_items[cs_mod_list_idx].cce_reg_map_type.set_non_interleaved();
      } else {
        ctrl_res_items[cs_mod_list_idx].cce_reg_map_type.set_interleaved();
      }

      if (coreset_cfg.precoder_granularity == srsran_coreset_precoder_granularity_reg_bundle) {
        ctrl_res_items[cs_mod_list_idx].precoder_granularity =
            asn1::rrc_nr::ctrl_res_set_s::precoder_granularity_opts::same_as_reg_bundle;
      } else {
        ctrl_res_items[cs_mod_list_idx].precoder_granularity =
            asn1::rrc_nr::ctrl_res_set_s::precoder_granularity_opts::all_contiguous_rbs;
      }
    }
  }

  // Note: Skip SearchSpace#0
  for (uint32_t ss_idx = 1; ss_idx < SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE; ss_idx++) {
    if (cell_cfg.phy_cell.pdcch.search_space_present[ss_idx]) {
      // search spaces
      auto& search_space_cfg                          = cell_cfg.phy_cell.pdcch.search_space[ss_idx];
      pdcch_cfg.search_spaces_to_add_mod_list_present = true;

      uint8_t ss_mod_list_idx = pdcch_cfg.search_spaces_to_add_mod_list.size();
      pdcch_cfg.search_spaces_to_add_mod_list.resize(ss_mod_list_idx + 1);
      auto& search_spaces                                    = pdcch_cfg.search_spaces_to_add_mod_list;
      search_spaces[ss_mod_list_idx].search_space_id         = search_space_cfg.id;
      search_spaces[ss_mod_list_idx].ctrl_res_set_id_present = true;
      search_spaces[ss_mod_list_idx].ctrl_res_set_id         = search_space_cfg.coreset_id;
      search_spaces[ss_mod_list_idx].monitoring_slot_periodicity_and_offset_present = true;
      search_spaces[ss_mod_list_idx].monitoring_slot_periodicity_and_offset.set_sl1();
      search_spaces[ss_mod_list_idx].monitoring_symbols_within_slot_present = true;
      search_spaces[ss_mod_list_idx].monitoring_symbols_within_slot.from_number(0b10000000000000);
      search_spaces[ss_mod_list_idx].nrof_candidates_present = true;
      search_spaces[ss_mod_list_idx].nrof_candidates.aggregation_level1 =
          asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level1_opts::n0;
      search_spaces[ss_mod_list_idx].nrof_candidates.aggregation_level2 =
          asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level2_opts::n2;
      search_spaces[ss_mod_list_idx].nrof_candidates.aggregation_level4 =
          asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level4_opts::n2;
      search_spaces[ss_mod_list_idx].nrof_candidates.aggregation_level8 =
          asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level8_opts::n0;
      search_spaces[ss_mod_list_idx].nrof_candidates.aggregation_level16 =
          asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level16_opts::n0;
      search_spaces[ss_mod_list_idx].search_space_type_present = true;

      if ((search_space_cfg.type == srsran_search_space_type_common_0) or
          (search_space_cfg.type == srsran_search_space_type_common_0A) or
          (search_space_cfg.type == srsran_search_space_type_common_1) or
          (search_space_cfg.type == srsran_search_space_type_common_2) or
          (search_space_cfg.type == srsran_search_space_type_common_3)) {
        search_spaces[ss_mod_list_idx].search_space_type.set_common();

        search_spaces[ss_mod_list_idx].search_space_type.common().dci_format0_minus0_and_format1_minus0_present = true;
      } else {
        get_logger(cfg).error("Config Error: Unsupported search space type.");
        return SRSRAN_ERROR;
      }
    }
  }
  return SRSRAN_SUCCESS;
}

void fill_pdsch_cfg_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, pdsch_cfg_s& out)
{
  out.dmrs_dl_for_pdsch_map_type_a_present = true;
  out.dmrs_dl_for_pdsch_map_type_a.set_setup();
  out.dmrs_dl_for_pdsch_map_type_a.setup().dmrs_add_position_present = true;
  out.dmrs_dl_for_pdsch_map_type_a.setup().dmrs_add_position         = dmrs_dl_cfg_s::dmrs_add_position_opts::pos1;

  out.tci_states_to_add_mod_list_present = true;
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

  // ZP-CSI
  out.zp_csi_rs_res_to_add_mod_list_present = false; // TEMP
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
  init_dl_bwp.pdcch_cfg_present = true;
  HANDLE_ERROR(fill_pdcch_cfg_from_enb_cfg(cfg, cc, init_dl_bwp.pdcch_cfg.set_setup()));

  init_dl_bwp.pdsch_cfg_present = true;
  fill_pdsch_cfg_from_enb_cfg(cfg, cc, init_dl_bwp.pdsch_cfg.set_setup());

  // TODO: ADD missing fields

  return SRSRAN_SUCCESS;
}

void fill_pucch_cfg_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, pucch_cfg_s& out)
{
  // Make 2 PUCCH resource sets
  out.res_set_to_add_mod_list_present = true;
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
  out.res_to_add_mod_list_present = true;
  out.res_to_add_mod_list.resize(18);
  uint32_t j = 0, j2 = 0;
  for (uint32_t i = 0; i < out.res_to_add_mod_list.size(); ++i) {
    out.res_to_add_mod_list[i].pucch_res_id                = i;
    out.res_to_add_mod_list[i].intra_slot_freq_hop_present = true;
    out.res_to_add_mod_list[i].second_hop_prb_present      = true;
    if (i < 8 or i == 16) {
      out.res_to_add_mod_list[i].start_prb                              = 51;
      out.res_to_add_mod_list[i].second_hop_prb                         = 0;
      out.res_to_add_mod_list[i].format.set_format1().init_cyclic_shift = (4 * (j % 3));
      out.res_to_add_mod_list[i].format.format1().nrof_symbols          = 14;
      out.res_to_add_mod_list[i].format.format1().start_symbol_idx      = 0;
      out.res_to_add_mod_list[i].format.format1().time_domain_occ       = j / 3;
      j++;
    } else if (i < 15) {
      out.res_to_add_mod_list[i].start_prb                         = 1;
      out.res_to_add_mod_list[i].second_hop_prb                    = 50;
      out.res_to_add_mod_list[i].format.set_format2().nrof_prbs    = 1;
      out.res_to_add_mod_list[i].format.format2().nrof_symbols     = 2;
      out.res_to_add_mod_list[i].format.format2().start_symbol_idx = 2 * (j2 % 7);
      j2++;
    } else {
      out.res_to_add_mod_list[i].start_prb                         = 50;
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

  // SR resources
  out.sched_request_res_to_add_mod_list_present = true;
  out.sched_request_res_to_add_mod_list.resize(1);
  auto& sr_res1                             = out.sched_request_res_to_add_mod_list[0];
  sr_res1.sched_request_res_id              = 1;
  sr_res1.sched_request_id                  = 0;
  sr_res1.periodicity_and_offset_present    = true;
  sr_res1.periodicity_and_offset.set_sl40() = 0;
  sr_res1.res_present                       = true;
  sr_res1.res                               = 16;

  // DL data
  out.dl_data_to_ul_ack_present = true;
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

int fill_pdcch_cfg_common_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, pdcch_cfg_common_s& pdcch_cfg_common)
{
  pdcch_cfg_common.common_ctrl_res_set_present         = true;
  pdcch_cfg_common.common_ctrl_res_set.ctrl_res_set_id = 1;
  pdcch_cfg_common.common_ctrl_res_set.freq_domain_res.from_number(0b111111110000000000000000000000000000000000000);
  pdcch_cfg_common.common_ctrl_res_set.dur = 1;
  pdcch_cfg_common.common_ctrl_res_set.cce_reg_map_type.set_non_interleaved();
  pdcch_cfg_common.common_ctrl_res_set.precoder_granularity =
      asn1::rrc_nr::ctrl_res_set_s::precoder_granularity_opts::same_as_reg_bundle;

  // common search space list
  pdcch_cfg_common.common_search_space_list_present = true;
  pdcch_cfg_common.common_search_space_list.resize(1);
  pdcch_cfg_common.common_search_space_list[0].search_space_id           = 1;
  pdcch_cfg_common.common_search_space_list[0].ctrl_res_set_id_present   = true;
  pdcch_cfg_common.common_search_space_list[0].ctrl_res_set_id           = 1;
  pdcch_cfg_common.common_search_space_list[0].search_space_type_present = true;
  pdcch_cfg_common.common_search_space_list[0].search_space_type.set_common();
  pdcch_cfg_common.common_search_space_list[0]
      .search_space_type.common()
      .dci_format0_minus0_and_format1_minus0_present                   = true;
  pdcch_cfg_common.common_search_space_list[0].nrof_candidates_present = true;
  pdcch_cfg_common.common_search_space_list[0].nrof_candidates.aggregation_level1 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level1_opts::n1;
  pdcch_cfg_common.common_search_space_list[0].nrof_candidates.aggregation_level2 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level2_opts::n1;
  pdcch_cfg_common.common_search_space_list[0].nrof_candidates.aggregation_level4 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level4_opts::n1;
  pdcch_cfg_common.common_search_space_list[0].nrof_candidates.aggregation_level8 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level8_opts::n0;
  pdcch_cfg_common.common_search_space_list[0].nrof_candidates.aggregation_level16 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level16_opts::n0;
  pdcch_cfg_common.common_search_space_list[0].monitoring_slot_periodicity_and_offset_present = true;
  pdcch_cfg_common.common_search_space_list[0].monitoring_slot_periodicity_and_offset.set_sl1();
  pdcch_cfg_common.common_search_space_list[0].monitoring_symbols_within_slot_present = true;
  pdcch_cfg_common.common_search_space_list[0].monitoring_symbols_within_slot.from_number(0b10000000000000);
  pdcch_cfg_common.ra_search_space_present = true;
  pdcch_cfg_common.ra_search_space         = 1;

  if (cfg.cell_list[cc].duplex_mode == SRSRAN_DUPLEX_MODE_TDD) {
    pdcch_cfg_common.ext = false;
  }

  return SRSRAN_SUCCESS;
}

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
  HANDLE_ERROR(fill_pdcch_cfg_common_from_enb_cfg(cfg, cc, init_dl_bwp.pdcch_cfg_common.set_setup()));
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

  freq_info_ul.freq_band_list_present = true;
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

/// Fill RachConfigCommon with gNB config
int fill_rach_cfg_common_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, rach_cfg_common_s& rach_cfg_common)
{
  auto& cell_cfg = cfg.cell_list.at(cc);

  rach_cfg_common = cfg.rach_cfg_common;

  rach_cfg_common.rach_cfg_generic.msg1_freq_start           = cell_cfg.phy_cell.prach.freq_offset;
  rach_cfg_common.rach_cfg_generic.prach_cfg_idx             = cell_cfg.phy_cell.prach.config_idx;
  rach_cfg_common.rach_cfg_generic.zero_correlation_zone_cfg = cell_cfg.phy_cell.prach.zero_corr_zone;

  if (cfg.prach_root_seq_idx_type == 139) {
    rach_cfg_common.prach_root_seq_idx.set_l139() = cell_cfg.phy_cell.prach.root_seq_idx;
  } else if (cfg.prach_root_seq_idx_type == 839) {
    rach_cfg_common.prach_root_seq_idx.set_l839() = cell_cfg.phy_cell.prach.root_seq_idx;
  } else {
    get_logger(cfg).error("Config Error: Invalid prach_root_seq_idx_type (%d)\n", cfg.prach_root_seq_idx_type);
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

/// Fill InitUlBwp with gNB config
int fill_init_ul_bwp_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, bwp_ul_common_s& init_ul_bwp)
{
  init_ul_bwp.rach_cfg_common_present = true;
  fill_rach_cfg_common_from_enb_cfg(cfg, cc, init_ul_bwp.rach_cfg_common.set_setup());

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

  serv_common.ss_pbch_block_pwr               = cell_cfg.phy_cell.pdsch.rs_power;
  serv_common.n_timing_advance_offset_present = true;
  serv_common.n_timing_advance_offset = asn1::rrc_nr::serving_cell_cfg_common_s::n_timing_advance_offset_opts::n0;
  serv_common.n_timing_advance_offset_present = true;
  serv_common.dmrs_type_a_position = asn1::rrc_nr::serving_cell_cfg_common_s::dmrs_type_a_position_opts::pos2;

  serv_common.pci_present = true;
  serv_common.pci         = cell_cfg.phy_cell.carrier.pci;

  serv_common.ssb_periodicity_serving_cell_present = true;
  if (not asn1::number_to_enum(serv_common.ssb_periodicity_serving_cell, cell_cfg.ssb_cfg.periodicity_ms)) {
    get_logger(cfg).error("Config Error: Invalid SSB periodicity = %d\n", cell_cfg.ssb_cfg.periodicity_ms);
    return SRSRAN_ERROR;
  }

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
    auto& tdd_config                         = serv_common.tdd_ul_dl_cfg_common;
    tdd_config.ref_subcarrier_spacing        = subcarrier_spacing_e::khz15;
    tdd_config.pattern1.dl_ul_tx_periodicity = asn1::rrc_nr::tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms10;
    tdd_config.pattern1.nrof_dl_slots        = 6;
    tdd_config.pattern1.nrof_dl_symbols      = 0;
    tdd_config.pattern1.nrof_ul_slots        = 4;
    tdd_config.pattern1.nrof_ul_symbols      = 0;
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
  sp_cell.recfg_with_sync_present = true;
  HANDLE_ERROR(fill_recfg_with_sync_from_enb_cfg(cfg, cc, sp_cell.recfg_with_sync));

  sp_cell.sp_cell_cfg_ded_present = true;
  HANDLE_ERROR(fill_serv_cell_from_enb_cfg(cfg, cc, sp_cell.sp_cell_cfg_ded));

  return SRSRAN_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void fill_srb1(const rrc_nr_cfg_t& cfg, rlc_bearer_cfg_s& srb1)
{
  srb1.lc_ch_id                         = 1;
  srb1.served_radio_bearer_present      = true;
  srb1.served_radio_bearer.set_srb_id() = 1;
  srb1.rlc_cfg_present                  = true;
  ul_am_rlc_s& am_ul                    = srb1.rlc_cfg.set_am().ul_am_rlc;
  am_ul.sn_field_len_present            = true;
  am_ul.sn_field_len.value              = asn1::rrc_nr::sn_field_len_am_opts::size12;
  am_ul.t_poll_retx.value               = asn1::rrc_nr::t_poll_retx_opts::ms45;
  am_ul.poll_pdu.value                  = asn1::rrc_nr::poll_pdu_opts::infinity;
  am_ul.poll_byte.value                 = asn1::rrc_nr::poll_byte_opts::infinity;
  am_ul.max_retx_thres.value            = asn1::rrc_nr::ul_am_rlc_s::max_retx_thres_opts::t8;
  dl_am_rlc_s& am_dl                    = srb1.rlc_cfg.am().dl_am_rlc;
  am_dl.sn_field_len_present            = true;
  am_dl.sn_field_len.value              = asn1::rrc_nr::sn_field_len_am_opts::size12;
  am_dl.t_reassembly.value              = t_reassembly_opts::ms35;
  am_dl.t_status_prohibit.value         = asn1::rrc_nr::t_status_prohibit_opts::ms0;

  // mac-LogicalChannelConfig -- Cond LCH-Setup
  srb1.mac_lc_ch_cfg_present                    = true;
  srb1.mac_lc_ch_cfg.ul_specific_params_present = true;
  srb1.mac_lc_ch_cfg.ul_specific_params.prio    = 1;
  srb1.mac_lc_ch_cfg.ul_specific_params.prioritised_bit_rate.value =
      lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_opts::infinity;
  srb1.mac_lc_ch_cfg.ul_specific_params.bucket_size_dur.value =
      lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_opts::ms5;
  srb1.mac_lc_ch_cfg.ul_specific_params.lc_ch_group_present          = true;
  srb1.mac_lc_ch_cfg.ul_specific_params.lc_ch_group                  = 0;
  srb1.mac_lc_ch_cfg.ul_specific_params.sched_request_id_present     = true;
  srb1.mac_lc_ch_cfg.ul_specific_params.sched_request_id             = 0;
  srb1.mac_lc_ch_cfg.ul_specific_params.lc_ch_sr_mask                = false;
  srb1.mac_lc_ch_cfg.ul_specific_params.lc_ch_sr_delay_timer_applied = false;
}

/// Fill MasterCellConfig with gNB config
int fill_master_cell_cfg_from_enb_cfg(const rrc_nr_cfg_t& cfg, uint32_t cc, asn1::rrc_nr::cell_group_cfg_s& out)
{
  out.cell_group_id                      = 0;
  out.rlc_bearer_to_add_mod_list_present = true;
  out.rlc_bearer_to_add_mod_list.resize(1);
  fill_srb1(cfg, out.rlc_bearer_to_add_mod_list[0]);

  // mac-CellGroupConfig -- Need M
  out.mac_cell_group_cfg_present                                                 = true;
  out.mac_cell_group_cfg.sched_request_cfg_present                               = true;
  out.mac_cell_group_cfg.sched_request_cfg.sched_request_to_add_mod_list_present = true;
  out.mac_cell_group_cfg.sched_request_cfg.sched_request_to_add_mod_list.resize(1);
  out.mac_cell_group_cfg.sched_request_cfg.sched_request_to_add_mod_list[0].sched_request_id = 0;
  out.mac_cell_group_cfg.sched_request_cfg.sched_request_to_add_mod_list[0].sr_trans_max.value =
      sched_request_to_add_mod_s::sr_trans_max_opts::n64;
  out.mac_cell_group_cfg.bsr_cfg_present                     = true;
  out.mac_cell_group_cfg.bsr_cfg.periodic_bsr_timer.value    = bsr_cfg_s::periodic_bsr_timer_opts::sf20;
  out.mac_cell_group_cfg.bsr_cfg.retx_bsr_timer.value        = bsr_cfg_s::retx_bsr_timer_opts::sf320;
  out.mac_cell_group_cfg.tag_cfg_present                     = true;
  out.mac_cell_group_cfg.tag_cfg.tag_to_add_mod_list_present = true;
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

  // physicalCellGroupConfig -- Need M
  out.phys_cell_group_cfg_present          = true;
  out.phys_cell_group_cfg.p_nr_fr1_present = true;
  out.phys_cell_group_cfg.p_nr_fr1         = 10;
  out.phys_cell_group_cfg.pdsch_harq_ack_codebook.value =
      phys_cell_group_cfg_s::pdsch_harq_ack_codebook_opts::dynamic_value;

  // spCellConfig -- Need M
  out.sp_cell_cfg_present = true;
  fill_sp_cell_cfg_from_enb_cfg(cfg, cc, out.sp_cell_cfg);
  out.sp_cell_cfg.recfg_with_sync_present = false;

  return SRSRAN_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int fill_mib_from_enb_cfg(const rrc_nr_cfg_t& cfg, asn1::rrc_nr::mib_s& mib)
{
  uint32_t scs =
      subcarrier_spacing_e{(subcarrier_spacing_opts::options)cfg.cell_list[0].phy_cell.carrier.scs}.to_number();
  srsran::generate_default_mib(scs, cfg.cell_list[0].coreset0_idx, mib);
  return SRSRAN_SUCCESS;
}

int fill_sib1_from_enb_cfg(const rrc_nr_cfg_t& cfg, asn1::rrc_nr::sib1_s& sib1)
{
  srsran::basic_cell_args_t args;
  args.is_standalone = cfg.is_standalone;
  args.scs = subcarrier_spacing_e{(subcarrier_spacing_opts::options)cfg.cell_list[0].phy_cell.carrier.scs}.to_number();
  args.is_fdd = cfg.cell_list[0].duplex_mode == SRSRAN_DUPLEX_MODE_FDD;
  srsran::generate_default_sib1(args, sib1);
  return SRSRAN_SUCCESS;
}

} // namespace srsenb
