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

#include <getopt.h>
#include <iostream>

#include "srsran/asn1/rrc_nr.h"
#include "srsran/asn1/rrc_nr_utils.h"
#include "srsran/common/common.h"
#include "srsran/common/test_common.h"

using namespace srsran;
using namespace asn1::rrc_nr;

int test_rlc_config()
{
  asn1::rrc_nr::rlc_cfg_c rlc_cfg_asn1;
  rlc_cfg_asn1.set_um_bi_dir();
  rlc_cfg_asn1.um_bi_dir().dl_um_rlc.sn_field_len_present = true;
  rlc_cfg_asn1.um_bi_dir().dl_um_rlc.sn_field_len         = asn1::rrc_nr::sn_field_len_um_e::size12;
  rlc_cfg_asn1.um_bi_dir().dl_um_rlc.t_reassembly         = asn1::rrc_nr::t_reassembly_e::ms50;
  rlc_cfg_asn1.um_bi_dir().ul_um_rlc.sn_field_len_present = true;
  rlc_cfg_asn1.um_bi_dir().ul_um_rlc.sn_field_len         = asn1::rrc_nr::sn_field_len_um_e::size12;
  asn1::json_writer jw;
  rlc_cfg_asn1.to_json(jw);
  srslog::fetch_basic_logger("RRC").info("RLC NR Config: \n %s", jw.to_string().c_str());

  rlc_config_t rlc_cfg;
  TESTASSERT(make_rlc_config_t(rlc_cfg_asn1, &rlc_cfg) == SRSRAN_SUCCESS);
  TESTASSERT(rlc_cfg.rat == srsran_rat_t::nr);
  TESTASSERT(rlc_cfg.um_nr.sn_field_length == rlc_um_nr_sn_size_t::size12bits);
  return SRSRAN_SUCCESS;
}

int test_mac_rach_common_config()
{
  asn1::rrc_nr::rach_cfg_common_s rach_common_config_asn1;
  rach_common_config_asn1.ra_contention_resolution_timer =
      asn1::rrc_nr::rach_cfg_common_s::ra_contention_resolution_timer_opts::sf64;
  rach_common_config_asn1.rach_cfg_generic.ra_resp_win   = asn1::rrc_nr::rach_cfg_generic_s::ra_resp_win_opts::sl10;
  rach_common_config_asn1.rach_cfg_generic.prach_cfg_idx = 160;
  rach_common_config_asn1.rach_cfg_generic.preamb_rx_target_pwr = -110;
  rach_common_config_asn1.rach_cfg_generic.pwr_ramp_step = asn1::rrc_nr::rach_cfg_generic_s::pwr_ramp_step_opts::db4;
  rach_common_config_asn1.rach_cfg_generic.preamb_trans_max =
      asn1::rrc_nr::rach_cfg_generic_s::preamb_trans_max_opts::n7;

  asn1::json_writer jw;
  rach_common_config_asn1.to_json(jw);
  srslog::fetch_basic_logger("RRC").info("MAC NR RACH Common config: \n %s", jw.to_string().c_str());

  rach_nr_cfg_t rach_nr_cfg = make_mac_rach_cfg(rach_common_config_asn1);
  TESTASSERT(rach_nr_cfg.ra_responseWindow == 10);
  TESTASSERT(rach_nr_cfg.ra_ContentionResolutionTimer == 64);
  TESTASSERT(rach_nr_cfg.prach_ConfigurationIndex == 160);
  TESTASSERT(rach_nr_cfg.PreambleReceivedTargetPower == -110);
  TESTASSERT(rach_nr_cfg.preambleTransMax == 7);
  TESTASSERT(rach_nr_cfg.powerRampingStep == 4);
  return SRSRAN_SUCCESS;
}

// Phy tests

int make_phy_tdd_cfg_test()
{
  // tdd-UL-DL-ConfigurationCommon
  //    referenceSubcarrierSpacing: kHz15 (0)
  //    pattern1
  //        dl-UL-TransmissionPeriodicity: ms10 (7)
  //        nrofDownlinkSlots: 7
  //        nrofDownlinkSymbols: 6
  //        nrofUplinkSlots: 2
  //        nrofUplinkSymbols: 4
  tdd_ul_dl_cfg_common_s tdd_ul_dl_cfg_common = {};

  tdd_ul_dl_cfg_common.ref_subcarrier_spacing        = subcarrier_spacing_opts::khz15;
  tdd_ul_dl_cfg_common.pattern1.dl_ul_tx_periodicity = tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms10;
  tdd_ul_dl_cfg_common.pattern1.nrof_dl_slots        = 7;
  tdd_ul_dl_cfg_common.pattern1.nrof_dl_symbols      = 6;
  tdd_ul_dl_cfg_common.pattern1.nrof_ul_slots        = 2;
  tdd_ul_dl_cfg_common.pattern1.nrof_ul_symbols      = 4;

  srsran_duplex_config_nr_t srsran_duplex_config_nr;
  TESTASSERT(make_phy_tdd_cfg(tdd_ul_dl_cfg_common, &srsran_duplex_config_nr) == true);

  TESTASSERT(srsran_duplex_config_nr.tdd.pattern1.period_ms == 10);
  TESTASSERT(srsran_duplex_config_nr.tdd.pattern1.nof_dl_slots == 7);
  TESTASSERT(srsran_duplex_config_nr.tdd.pattern1.nof_dl_symbols == 6);
  TESTASSERT(srsran_duplex_config_nr.tdd.pattern1.nof_ul_slots == 2);
  TESTASSERT(srsran_duplex_config_nr.tdd.pattern1.nof_ul_symbols == 4);
  TESTASSERT(srsran_duplex_config_nr.tdd.pattern2.period_ms == 0);
  return SRSRAN_SUCCESS;
}

int make_phy_harq_ack_cfg_test()
{
  // physicalCellGroupConfig
  //    pdsch-HARQ-ACK-Codebook: dynamic (1)
  phys_cell_group_cfg_s phys_cell_group_cfg   = {};
  phys_cell_group_cfg.pdsch_harq_ack_codebook = phys_cell_group_cfg_s::pdsch_harq_ack_codebook_opts::dynamic_value;

  srsran_harq_ack_cfg_hl_t srsran_ue_dl_nr_harq_ack_cfg;
  TESTASSERT(make_phy_harq_ack_cfg(phys_cell_group_cfg, &srsran_ue_dl_nr_harq_ack_cfg) == true);

  TESTASSERT(srsran_ue_dl_nr_harq_ack_cfg.harq_ack_codebook == srsran_pdsch_harq_ack_codebook_dynamic);
  return SRSRAN_SUCCESS;
}

int make_phy_coreset_cfg_test()
{
  ctrl_res_set_s ctrl_res_set       = {};
  ctrl_res_set.ctrl_res_set_id      = 1;
  ctrl_res_set.precoder_granularity = ctrl_res_set_s::precoder_granularity_opts::same_as_reg_bundle;
  ctrl_res_set.dur                  = 1;
  ctrl_res_set.cce_reg_map_type.set_non_interleaved();
  ctrl_res_set.freq_domain_res.from_string("111111110000000000000000000000000000000000000");

  srsran_coreset_t srsran_coreset;
  TESTASSERT(make_phy_coreset_cfg(ctrl_res_set, &srsran_coreset) == true);

  TESTASSERT(srsran_coreset.id == 1);
  TESTASSERT(srsran_coreset.precoder_granularity == srsran_coreset_precoder_granularity_reg_bundle);
  TESTASSERT(srsran_coreset.duration == 1);
  TESTASSERT(srsran_coreset.mapping_type == srsran_coreset_mapping_type_non_interleaved);

  for (uint32_t i = 0; i < SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE; i++) {
    TESTASSERT(srsran_coreset.freq_resources[i] == (i < 8));
  }

  return SRSRAN_SUCCESS;
}

int make_phy_search_space_cfg_test()
{
  // SearchSpace
  //    searchSpaceId: 1
  //    controlResourceSetId: 1
  //    monitoringSlotPeriodicityAndOffset: sl1 (0)
  //        sl1: NULL
  //    monitoringSymbolsWithinSlot: 8000 [bit length 14, 2 LSB pad bits, 1000 0000  0000 00.. decimal value 8192]
  //    nrofCandidates
  //        aggregationLevel1: n0 (0)
  //        aggregationLevel2: n0 (0)
  //        aggregationLevel4: n1 (1)
  //        aggregationLevel8: n0 (0)
  //        aggregationLevel16: n0 (0)
  //    searchSpaceType: common (0)
  //        common
  //            dci-Format0-0-AndFormat1-0
  search_space_s search_space                                 = {};
  search_space.search_space_id                                = 1;
  search_space.ctrl_res_set_id_present                        = true;
  search_space.ctrl_res_set_id                                = 1;
  search_space.monitoring_slot_periodicity_and_offset_present = true;
  search_space.monitoring_slot_periodicity_and_offset.set_sl1();
  search_space.monitoring_symbols_within_slot_present = true;
  search_space.monitoring_symbols_within_slot.from_string("10000000000000");
  search_space.nrof_candidates_present = true;
  search_space.nrof_candidates.aggregation_level1 =
      search_space_s::nrof_candidates_s_::aggregation_level1_opts::options::n0;
  search_space.nrof_candidates.aggregation_level2 =
      search_space_s::nrof_candidates_s_::aggregation_level2_opts::options::n0;
  search_space.nrof_candidates.aggregation_level4 =
      search_space_s::nrof_candidates_s_::aggregation_level4_opts::options::n1;
  search_space.nrof_candidates.aggregation_level8 =
      search_space_s::nrof_candidates_s_::aggregation_level8_opts::options::n0;
  search_space.nrof_candidates.aggregation_level16 =
      search_space_s::nrof_candidates_s_::aggregation_level16_opts::options::n0;
  search_space.search_space_type_present = true;
  search_space.search_space_type.set_common();
  search_space.search_space_type.common().dci_format0_minus0_and_format1_minus0_present = true;

  srsran_search_space_t srsran_search_space;
  TESTASSERT(make_phy_search_space_cfg(search_space, &srsran_search_space) == true);

  TESTASSERT(srsran_search_space.id == 1);
  TESTASSERT(srsran_search_space.coreset_id == 1);
  TESTASSERT(srsran_search_space.nof_candidates[0] == 0);
  TESTASSERT(srsran_search_space.nof_candidates[1] == 0);
  TESTASSERT(srsran_search_space.nof_candidates[2] == 1);
  TESTASSERT(srsran_search_space.nof_candidates[3] == 0);
  TESTASSERT(srsran_search_space.nof_candidates[4] == 0);
  TESTASSERT(srsran_search_space.type == srsran_search_space_type_common_3);

  // searchSpacesToAddModList: 1 item
  //    Item 0
  //        SearchSpace
  //            searchSpaceId: 2
  //            controlResourceSetId: 2
  //            monitoringSlotPeriodicityAndOffset: sl1 (0)
  //                sl1: NULL
  //            monitoringSymbolsWithinSlot: 8000 [bit length 14, 2 LSB pad bits, 1000 0000  0000
  //            00.. decimal value 8192] nrofCandidates
  //                aggregationLevel1: n0 (0)
  //                aggregationLevel2: n2 (2)
  //                aggregationLevel4: n1 (1)
  //                aggregationLevel8: n0 (0)
  //                aggregationLevel16: n0 (0)
  //            searchSpaceType: ue-Specific (1)
  //                ue-Specific
  //                    dci-Formats: formats0-0-And-1-0 (0)

  search_space_s search_space_2                                 = {};
  search_space_2.search_space_id                                = 2;
  search_space_2.ctrl_res_set_id_present                        = true;
  search_space_2.ctrl_res_set_id                                = 2;
  search_space_2.monitoring_slot_periodicity_and_offset_present = true;
  search_space_2.monitoring_slot_periodicity_and_offset.set_sl1();
  search_space_2.monitoring_symbols_within_slot_present = true;
  search_space_2.monitoring_symbols_within_slot.from_string("10000000000000");
  search_space_2.nrof_candidates_present = true;
  search_space_2.nrof_candidates.aggregation_level1 =
      search_space_s::nrof_candidates_s_::aggregation_level1_opts::options::n0;
  search_space_2.nrof_candidates.aggregation_level2 =
      search_space_s::nrof_candidates_s_::aggregation_level2_opts::options::n2;
  search_space_2.nrof_candidates.aggregation_level4 =
      search_space_s::nrof_candidates_s_::aggregation_level4_opts::options::n1;
  search_space_2.nrof_candidates.aggregation_level8 =
      search_space_s::nrof_candidates_s_::aggregation_level8_opts::options::n0;
  search_space_2.nrof_candidates.aggregation_level16 =
      search_space_s::nrof_candidates_s_::aggregation_level16_opts::options::n0;
  search_space_2.search_space_type_present = true;
  search_space_2.search_space_type.set_ue_specific();
  search_space_2.search_space_type.ue_specific().dci_formats =
      search_space_s::search_space_type_c_::ue_specific_s_::dci_formats_opts::formats0_minus0_and_minus1_minus0;

  srsran_search_space_t srsran_search_space_2;
  TESTASSERT(make_phy_search_space_cfg(search_space_2, &srsran_search_space_2) == true);

  TESTASSERT(srsran_search_space_2.id == 2);
  TESTASSERT(srsran_search_space_2.coreset_id == 2);
  TESTASSERT(srsran_search_space_2.nof_candidates[0] == 0);
  TESTASSERT(srsran_search_space_2.nof_candidates[1] == 2);
  TESTASSERT(srsran_search_space_2.nof_candidates[2] == 1);
  TESTASSERT(srsran_search_space_2.nof_candidates[3] == 0);
  TESTASSERT(srsran_search_space_2.nof_candidates[4] == 0);
  TESTASSERT(srsran_search_space_2.type == srsran_search_space_type_ue);

  return SRSRAN_SUCCESS;
}

int make_phy_csi_report_test()
{
  // csi-ReportConfigToAddModList: 1 item
  //    Item 0
  //        CSI-ReportConfig
  //            reportConfigId: 0
  //            resourcesForChannelMeasurement: 0
  //            csi-IM-ResourcesForInterference: 1
  //            reportConfigType: periodic (0)
  //                periodic
  //                    reportSlotConfig: slots80 (7)
  //                        slots80: 9
  //                    pucch-CSI-ResourceList: 1 item
  //                        Item 0
  //                            PUCCH-CSI-Resource
  //                                uplinkBandwidthPartId: 0
  //                                pucch-Resource: 17
  //            reportQuantity: cri-RI-PMI-CQI (1)
  //                cri-RI-PMI-CQI: NULL
  //            reportFreqConfiguration
  //                cqi-FormatIndicator: widebandCQI (0)
  //            timeRestrictionForChannelMeasurements: notConfigured (1)
  //            timeRestrictionForInterferenceMeasurements: notConfigured (1)
  //            groupBasedBeamReporting: disabled (1)
  //                disabled
  //            cqi-Table: table2 (1)
  //            subbandSize: value1 (0)
  csi_report_cfg_s csi_report_cfg                    = {};
  csi_report_cfg.report_cfg_id                       = 0;
  csi_report_cfg.res_for_ch_meas                     = 0;
  csi_report_cfg.csi_im_res_for_interference_present = true;
  csi_report_cfg.csi_im_res_for_interference         = 1;
  csi_report_cfg.report_cfg_type.set_periodic();
  csi_report_cfg.report_cfg_type.periodic().report_slot_cfg.set_slots80();
  csi_report_cfg.report_cfg_type.periodic().report_slot_cfg.slots80() = 9;
  pucch_csi_res_s pucch_csi_res;
  pucch_csi_res.pucch_res     = 17;
  pucch_csi_res.ul_bw_part_id = 0;
  csi_report_cfg.report_cfg_type.periodic().pucch_csi_res_list.push_back(pucch_csi_res);
  csi_report_cfg.report_quant.set_cri_ri_pmi_cqi();
  csi_report_cfg.report_freq_cfg_present                = true;
  csi_report_cfg.report_freq_cfg.cqi_format_ind_present = true;
  csi_report_cfg.report_freq_cfg.cqi_format_ind =
      csi_report_cfg_s::report_freq_cfg_s_::cqi_format_ind_opts::wideband_cqi;
  csi_report_cfg.cqi_table_present = true;
  csi_report_cfg.cqi_table         = csi_report_cfg_s::cqi_table_opts::table2;
  csi_report_cfg.subband_size      = csi_report_cfg_s::subband_size_opts::value1;

  srsran_csi_hl_report_cfg_t srsran_csi_hl_report_cfg;
  TESTASSERT(make_phy_csi_report(csi_report_cfg, &srsran_csi_hl_report_cfg) == true);

  TESTASSERT(srsran_csi_hl_report_cfg.type == SRSRAN_CSI_REPORT_TYPE_PERIODIC);
  TESTASSERT(srsran_csi_hl_report_cfg.channel_meas_id == 0);
  TESTASSERT(srsran_csi_hl_report_cfg.interf_meas_present == true);
  TESTASSERT(srsran_csi_hl_report_cfg.interf_meas_id == 1);
  TESTASSERT(srsran_csi_hl_report_cfg.periodic.period == 80);
  TESTASSERT(srsran_csi_hl_report_cfg.periodic.offset == 9);
  TESTASSERT(srsran_csi_hl_report_cfg.quantity == SRSRAN_CSI_REPORT_QUANTITY_CRI_RI_PMI_CQI);
  TESTASSERT(srsran_csi_hl_report_cfg.freq_cfg == SRSRAN_CSI_REPORT_FREQ_WIDEBAND);
  TESTASSERT(srsran_csi_hl_report_cfg.cqi_table == SRSRAN_CSI_CQI_TABLE_2);
  return SRSRAN_SUCCESS;
}

int make_phy_common_time_ra_test()
{
  // Test 1 & 2
  // pdsch-ConfigCommon: setup (1)
  //    setup
  //        pdsch-TimeDomainAllocationList: 2 items
  //            Item 0
  //                PDSCH-TimeDomainResourceAllocation
  //                    mappingType: typeA (0)
  //                    startSymbolAndLength: 40
  //            Item 1
  //                PDSCH-TimeDomainResourceAllocation
  //                    mappingType: typeA (0)
  //                    startSymbolAndLength: 57
  srsran_sch_time_ra_t srsran_sch_time_ra = {};

  pdsch_time_domain_res_alloc_s pdsch_time_domain_res_alloc = {};
  pdsch_time_domain_res_alloc.start_symbol_and_len          = 40;
  pdsch_time_domain_res_alloc.map_type                      = pdsch_time_domain_res_alloc_s::map_type_opts::type_a;

  TESTASSERT(make_phy_common_time_ra(pdsch_time_domain_res_alloc, &srsran_sch_time_ra) == true);

  TESTASSERT(srsran_sch_time_ra.mapping_type == srsran_sch_mapping_type_A);
  TESTASSERT(srsran_sch_time_ra.sliv == 40);
  TESTASSERT(srsran_sch_time_ra.k == 0);

  pdsch_time_domain_res_alloc.start_symbol_and_len = 57;
  pdsch_time_domain_res_alloc.map_type             = pdsch_time_domain_res_alloc_s::map_type_opts::type_a;

  TESTASSERT(make_phy_common_time_ra(pdsch_time_domain_res_alloc, &srsran_sch_time_ra) == true);
  TESTASSERT(srsran_sch_time_ra.mapping_type == srsran_sch_mapping_type_A);
  TESTASSERT(srsran_sch_time_ra.sliv == 57);
  TESTASSERT(srsran_sch_time_ra.k == 0);

  // Test 3 & 4

  // pusch-ConfigCommon: setup (1)
  //    setup
  //        pusch-TimeDomainAllocationList: 2 items
  //            Item 0
  //                PUSCH-TimeDomainResourceAllocation
  //                    k2: 4
  //                    mappingType: typeA (0)
  //                    startSymbolAndLength: 27
  //            Item 1
  //                PUSCH-TimeDomainResourceAllocation
  //                    k2: 5
  //                    mappingType: typeA (0)
  //                    startSymbolAndLength: 27
  //        p0-NominalWithGrant: -90dBm

  srsran_sch_time_ra                                        = {};
  pusch_time_domain_res_alloc_s pusch_time_domain_res_alloc = {};
  pusch_time_domain_res_alloc.k2_present                    = true;
  pusch_time_domain_res_alloc.k2                            = 4;
  pusch_time_domain_res_alloc.start_symbol_and_len          = 27;
  pusch_time_domain_res_alloc.map_type                      = pusch_time_domain_res_alloc_s::map_type_opts::type_a;

  TESTASSERT(make_phy_common_time_ra(pusch_time_domain_res_alloc, &srsran_sch_time_ra) == true);

  TESTASSERT(srsran_sch_time_ra.mapping_type == srsran_sch_mapping_type_A);
  TESTASSERT(srsran_sch_time_ra.sliv == 27);
  TESTASSERT(srsran_sch_time_ra.k == 4);

  pusch_time_domain_res_alloc                      = {};
  pusch_time_domain_res_alloc.k2_present           = true;
  pusch_time_domain_res_alloc.k2                   = 5;
  pusch_time_domain_res_alloc.start_symbol_and_len = 27;
  pusch_time_domain_res_alloc.map_type             = pusch_time_domain_res_alloc_s::map_type_opts::type_a;

  TESTASSERT(make_phy_common_time_ra(pusch_time_domain_res_alloc, &srsran_sch_time_ra) == true);

  TESTASSERT(srsran_sch_time_ra.mapping_type == srsran_sch_mapping_type_A);
  TESTASSERT(srsran_sch_time_ra.sliv == 27);
  TESTASSERT(srsran_sch_time_ra.k == 5);

  return SRSRAN_SUCCESS;
}

int make_phy_max_code_rate_test()
{
  //        format1: setup (1)
  //            setup
  //        format2: setup (1)
  //            setup
  //                maxCodeRate: zeroDot25 (2)
  pucch_format_cfg_s pucch_format_cfg    = {};
  pucch_format_cfg.max_code_rate_present = true;
  pucch_format_cfg.max_code_rate         = pucch_max_code_rate_opts::options::zero_dot25;
  uint32_t max_code_rate;
  TESTASSERT(make_phy_max_code_rate(pucch_format_cfg, &max_code_rate) == true);
  TESTASSERT(max_code_rate == 2);
  return SRSRAN_SUCCESS;
}

int make_phy_res_config_test()
{
  uint32_t    format_2_max_code_rate = 2;
  pucch_res_s pucch_res              = {};

  //            Item 0
  //                PUCCH-Resource
  //                    pucch-ResourceId: 0
  //                    startingPRB: 0
  //                    format: format1 (1)
  //                        format1
  //                            initialCyclicShift: 0
  //                            nrofSymbols: 14
  //                            startingSymbolIndex: 0
  //                            timeDomainOCC: 0

  pucch_res.pucch_res_id = 0;
  pucch_res.start_prb    = 0;
  pucch_res.format.set_format1();
  pucch_res.format.format1().init_cyclic_shift = 0;
  pucch_res.format.format1().nrof_symbols      = 14;
  pucch_res.format.format1().start_symbol_idx  = 0;
  pucch_res.format.format1().time_domain_occ   = 0;

  srsran_pucch_nr_resource_t srsran_pucch_nr_resource;

  TESTASSERT(make_phy_res_config(pucch_res, format_2_max_code_rate, &srsran_pucch_nr_resource) == true);

  TESTASSERT(srsran_pucch_nr_resource.format == SRSRAN_PUCCH_NR_FORMAT_1);
  TESTASSERT(srsran_pucch_nr_resource.starting_prb == 0);
  TESTASSERT(srsran_pucch_nr_resource.initial_cyclic_shift == 0);
  TESTASSERT(srsran_pucch_nr_resource.nof_symbols == 14);
  TESTASSERT(srsran_pucch_nr_resource.start_symbol_idx == 0);
  TESTASSERT(srsran_pucch_nr_resource.time_domain_occ == 0);
  TESTASSERT(srsran_pucch_nr_resource.max_code_rate == 2);

  return SRSRAN_SUCCESS;
}

int make_phy_sr_resource_test()
{
  //        schedulingRequestResourceToAddModList: 1 item
  //            Item 0
  //                SchedulingRequestResourceConfig
  //                    schedulingRequestResourceId: 1
  //                    schedulingRequestID: 0
  //                    periodicityAndOffset: sl40 (10)
  //                        sl40: 8
  //                    resource: 16
  sched_request_res_cfg_s sched_request_res_cfg;
  sched_request_res_cfg.sched_request_res_id           = 1;
  sched_request_res_cfg.sched_request_id               = 0;
  sched_request_res_cfg.periodicity_and_offset_present = true;
  sched_request_res_cfg.periodicity_and_offset.set_sl40();
  sched_request_res_cfg.periodicity_and_offset.sl40() = 8;
  sched_request_res_cfg.res_present                   = true;
  sched_request_res_cfg.res                           = 16;

  srsran_pucch_nr_sr_resource_t srsran_pucch_nr_sr_resource;
  TESTASSERT(make_phy_sr_resource(sched_request_res_cfg, &srsran_pucch_nr_sr_resource) == true);

  TESTASSERT(srsran_pucch_nr_sr_resource.sr_id == 0);
  TESTASSERT(srsran_pucch_nr_sr_resource.period == 40);
  TESTASSERT(srsran_pucch_nr_sr_resource.offset == 8);
  TESTASSERT(srsran_pucch_nr_sr_resource.configured == true);

  return SRSRAN_SUCCESS;
}

int make_phy_dmrs_additional_pos_test()
{
  // pusch-Config: setup (1)
  //    setup
  //        dmrs-UplinkForPUSCH-MappingTypeA: setup (1)
  //            setup
  //                dmrs-AdditionalPosition: pos1 (1)
  //                transformPrecodingDisabled
  dmrs_ul_cfg_s dmrs_ul_cfg;
  dmrs_ul_cfg.dmrs_add_position_present = true;
  dmrs_ul_cfg.dmrs_add_position         = dmrs_ul_cfg_s::dmrs_add_position_opts::pos1;
  srsran_dmrs_sch_add_pos_t srsran_dmrs_sch_add_pos;
  TESTASSERT(make_phy_dmrs_ul_additional_pos(dmrs_ul_cfg, &srsran_dmrs_sch_add_pos) == true);

  TESTASSERT(srsran_dmrs_sch_add_pos == srsran_dmrs_sch_add_pos_1);

  return SRSRAN_SUCCESS;
}

int make_phy_beta_offsets_test()
{
  // uci-OnPUSCH: setup (1)
  //    setup
  //        betaOffsets: semiStatic (1)
  //            semiStatic
  //                betaOffsetACK-Index1: 9
  //                betaOffsetACK-Index2: 9
  //                betaOffsetACK-Index3: 9
  //                betaOffsetCSI-Part1-Index1: 6
  //                betaOffsetCSI-Part1-Index2: 6
  //                betaOffsetCSI-Part2-Index1: 6
  //                betaOffsetCSI-Part2-Index2: 6

  beta_offsets_s beta_offsets;
  beta_offsets.beta_offset_ack_idx1_present       = true;
  beta_offsets.beta_offset_ack_idx1               = 9;
  beta_offsets.beta_offset_ack_idx2_present       = true;
  beta_offsets.beta_offset_ack_idx2               = 9;
  beta_offsets.beta_offset_ack_idx3_present       = true;
  beta_offsets.beta_offset_ack_idx3               = 9;
  beta_offsets.beta_offset_csi_part1_idx1_present = true;
  beta_offsets.beta_offset_csi_part1_idx1         = 6;
  beta_offsets.beta_offset_csi_part1_idx2_present = true;
  beta_offsets.beta_offset_csi_part1_idx2         = 6;
  beta_offsets.beta_offset_csi_part2_idx1_present = true;
  beta_offsets.beta_offset_csi_part2_idx1         = 6;
  beta_offsets.beta_offset_csi_part2_idx2_present = true;
  beta_offsets.beta_offset_csi_part2_idx2         = 6;

  srsran_beta_offsets_t srsran_beta_offsets;
  TESTASSERT(make_phy_beta_offsets(beta_offsets, &srsran_beta_offsets) == true);

  TESTASSERT(srsran_beta_offsets.ack_index1 == 9);
  TESTASSERT(srsran_beta_offsets.ack_index2 == 9);
  TESTASSERT(srsran_beta_offsets.ack_index3 == 9);
  TESTASSERT(srsran_beta_offsets.csi1_index1 == 6);
  TESTASSERT(srsran_beta_offsets.csi1_index2 == 6);
  TESTASSERT(srsran_beta_offsets.csi2_index1 == 6);
  TESTASSERT(srsran_beta_offsets.csi2_index2 == 6);
  return SRSRAN_SUCCESS;
}

int make_phy_pusch_scaling_test()
{
  uci_on_pusch_s uci_on_pusch;
  uci_on_pusch.scaling = uci_on_pusch_s::scaling_opts::f1;
  float scaling;
  TESTASSERT(make_phy_pusch_scaling(uci_on_pusch, &scaling) == true);
  TESTASSERT(scaling == 1.0);
  return SRSRAN_SUCCESS;
}

int make_phy_zp_csi_rs_resource_test()
{
  srsran_csi_rs_zp_resource_t zp_csi_rs_resource0 = {};
  //             Item 0
  //                 ZP-CSI-RS-Resource
  //                     zp-CSI-RS-ResourceId: 0
  //                     resourceMapping
  //                         frequencyDomainAllocation: row4 (2)
  //                             row4: 80 [bit length 3, 5 LSB pad bits, 100. ....
  //                             decimal value 4]
  //                         nrofPorts: p4 (2)
  //                         firstOFDMSymbolInTimeDomain: 8
  //                         cdm-Type: fd-CDM2 (1)
  //                         density: one (1)
  //                             one: NULL
  //                         freqBand
  //                             startingRB: 0
  //                             nrofRBs: 52
  //                     periodicityAndOffset: slots80 (9)
  //                         slots80: 1

  asn1::rrc_nr::zp_csi_rs_res_s zp_csi_rs_res0;
  zp_csi_rs_res0.res_map.freq_domain_alloc.set_row4();
  zp_csi_rs_res0.res_map.freq_domain_alloc.row4().from_string("100");
  zp_csi_rs_res0.res_map.nrof_ports                       = csi_rs_res_map_s::nrof_ports_opts::options::p4;
  zp_csi_rs_res0.res_map.first_ofdm_symbol_in_time_domain = 8;
  zp_csi_rs_res0.res_map.cdm_type                         = csi_rs_res_map_s::cdm_type_opts::options::fd_cdm2;
  zp_csi_rs_res0.res_map.density.set_one();
  zp_csi_rs_res0.res_map.freq_band.start_rb           = 0;
  zp_csi_rs_res0.res_map.freq_band.nrof_rbs           = 52;
  zp_csi_rs_res0.periodicity_and_offset_present       = true;
  zp_csi_rs_res0.periodicity_and_offset.set_slots80() = 1;

  TESTASSERT(make_phy_zp_csi_rs_resource(zp_csi_rs_res0, &zp_csi_rs_resource0) == true);

  TESTASSERT(zp_csi_rs_resource0.resource_mapping.row == srsran_csi_rs_resource_mapping_row_4);
  TESTASSERT(zp_csi_rs_resource0.resource_mapping.frequency_domain_alloc[0] == true);
  TESTASSERT(zp_csi_rs_resource0.resource_mapping.frequency_domain_alloc[1] == false);
  TESTASSERT(zp_csi_rs_resource0.resource_mapping.frequency_domain_alloc[2] == false);
  TESTASSERT(zp_csi_rs_resource0.resource_mapping.nof_ports == 4);
  TESTASSERT(zp_csi_rs_resource0.resource_mapping.first_symbol_idx == 8);
  TESTASSERT(zp_csi_rs_resource0.resource_mapping.cdm == srsran_csi_rs_cdm_fd_cdm2);
  TESTASSERT(zp_csi_rs_resource0.resource_mapping.density == srsran_csi_rs_resource_mapping_density_one);
  TESTASSERT(zp_csi_rs_resource0.resource_mapping.freq_band.start_rb == 0);
  TESTASSERT(zp_csi_rs_resource0.resource_mapping.freq_band.nof_rb == 52);
  TESTASSERT(zp_csi_rs_resource0.periodicity.period == 80);
  TESTASSERT(zp_csi_rs_resource0.periodicity.offset == 1);
  return SRSRAN_SUCCESS;
}

int make_phy_nzp_csi_rs_resource_test()
{
  // nzp-CSI-RS-ResourceToAddModList: 5 items
  //    Item 0
  //        NZP-CSI-RS-Resource
  //            nzp-CSI-RS-ResourceId: 0
  //            resourceMapping
  //                frequencyDomainAllocation: row2 (1)
  //                    row2: 8000 [bit length 12, 4 LSB pad bits, 1000 0000  0000 .... decimal value 2048]
  //                nrofPorts: p1 (0)
  //                firstOFDMSymbolInTimeDomain: 4
  //                cdm-Type: noCDM (0)
  //                density: one (1)
  //                    one: NULL
  //                freqBand
  //                    startingRB: 0
  //                    nrofRBs: 52
  //            powerControlOffset: 0dB
  //            powerControlOffsetSS: db0 (1)
  //            scramblingID: 0
  //            periodicityAndOffset: slots80 (9)
  //                slots80: 1
  //            qcl-InfoPeriodicCSI-RS: 0
  asn1::rrc_nr::nzp_csi_rs_res_s nzp_csi_rs_res;
  nzp_csi_rs_res.nzp_csi_rs_res_id = 0;
  nzp_csi_rs_res.res_map.freq_domain_alloc.set_row2();
  nzp_csi_rs_res.res_map.freq_domain_alloc.row2().from_string("100000000000");
  nzp_csi_rs_res.res_map.nrof_ports                       = csi_rs_res_map_s::nrof_ports_opts::options::p1;
  nzp_csi_rs_res.res_map.first_ofdm_symbol_in_time_domain = 4;
  nzp_csi_rs_res.res_map.cdm_type                         = csi_rs_res_map_s::cdm_type_opts::options::no_cdm;
  nzp_csi_rs_res.res_map.density.set_one();
  nzp_csi_rs_res.res_map.freq_band.start_rb           = 0;
  nzp_csi_rs_res.res_map.freq_band.nrof_rbs           = 52;
  nzp_csi_rs_res.pwr_ctrl_offset                      = 0;
  nzp_csi_rs_res.pwr_ctrl_offset_ss_present           = true;
  nzp_csi_rs_res.pwr_ctrl_offset_ss                   = nzp_csi_rs_res_s::pwr_ctrl_offset_ss_opts::options::db0;
  nzp_csi_rs_res.scrambling_id                        = 0;
  nzp_csi_rs_res.periodicity_and_offset_present       = true;
  nzp_csi_rs_res.periodicity_and_offset.set_slots80() = 1;

  srsran_csi_rs_nzp_resource_t nzp_resource_0;
  TESTASSERT(make_phy_nzp_csi_rs_resource(nzp_csi_rs_res, &nzp_resource_0) == true);

  TESTASSERT(nzp_resource_0.resource_mapping.row == srsran_csi_rs_resource_mapping_row_2);
  TESTASSERT(nzp_resource_0.resource_mapping.frequency_domain_alloc[0] == true);
  TESTASSERT(nzp_resource_0.resource_mapping.frequency_domain_alloc[1] == false);
  TESTASSERT(nzp_resource_0.resource_mapping.frequency_domain_alloc[2] == false);
  TESTASSERT(nzp_resource_0.resource_mapping.frequency_domain_alloc[3] == false);
  TESTASSERT(nzp_resource_0.resource_mapping.frequency_domain_alloc[4] == false);
  TESTASSERT(nzp_resource_0.resource_mapping.frequency_domain_alloc[5] == false);
  TESTASSERT(nzp_resource_0.resource_mapping.frequency_domain_alloc[6] == false);
  TESTASSERT(nzp_resource_0.resource_mapping.frequency_domain_alloc[7] == false);
  TESTASSERT(nzp_resource_0.resource_mapping.frequency_domain_alloc[8] == false);
  TESTASSERT(nzp_resource_0.resource_mapping.frequency_domain_alloc[9] == false);
  TESTASSERT(nzp_resource_0.resource_mapping.frequency_domain_alloc[10] == false);
  TESTASSERT(nzp_resource_0.resource_mapping.frequency_domain_alloc[11] == false);
  TESTASSERT(nzp_resource_0.resource_mapping.nof_ports == 1);
  TESTASSERT(nzp_resource_0.resource_mapping.first_symbol_idx == 4);
  TESTASSERT(nzp_resource_0.resource_mapping.cdm == srsran_csi_rs_cdm_nocdm);
  TESTASSERT(nzp_resource_0.resource_mapping.density == srsran_csi_rs_resource_mapping_density_one);
  TESTASSERT(nzp_resource_0.resource_mapping.freq_band.start_rb == 0);
  TESTASSERT(nzp_resource_0.resource_mapping.freq_band.nof_rb == 52);
  TESTASSERT(nzp_resource_0.power_control_offset == 0);
  TESTASSERT(nzp_resource_0.power_control_offset_ss == 0);
  TESTASSERT(nzp_resource_0.scrambling_id == 0);
  TESTASSERT(nzp_resource_0.periodicity.period == 80);
  TESTASSERT(nzp_resource_0.periodicity.offset == 1);

  return SRSRAN_SUCCESS;
}

int main()
{
  auto& asn1_logger = srslog::fetch_basic_logger("ASN1", false);
  asn1_logger.set_level(srslog::basic_levels::debug);
  asn1_logger.set_hex_dump_max_size(-1);
  auto& rrc_logger = srslog::fetch_basic_logger("RRC", false);
  rrc_logger.set_level(srslog::basic_levels::debug);
  rrc_logger.set_hex_dump_max_size(-1);

  // Start the log backend.
  srslog::init();

  TESTASSERT(test_rlc_config() == SRSRAN_SUCCESS);
  TESTASSERT(test_mac_rach_common_config() == SRSRAN_SUCCESS);
  TESTASSERT(make_phy_tdd_cfg_test() == SRSRAN_SUCCESS);
  TESTASSERT(make_phy_harq_ack_cfg_test() == SRSRAN_SUCCESS);
  TESTASSERT(make_phy_search_space_cfg_test() == SRSRAN_SUCCESS);
  TESTASSERT(make_phy_csi_report_test() == SRSRAN_SUCCESS);
  TESTASSERT(make_phy_coreset_cfg_test() == SRSRAN_SUCCESS);
  TESTASSERT(make_phy_common_time_ra_test() == SRSRAN_SUCCESS);
  TESTASSERT(make_phy_max_code_rate_test() == SRSRAN_SUCCESS);
  TESTASSERT(make_phy_res_config_test() == SRSRAN_SUCCESS);
  TESTASSERT(make_phy_sr_resource_test() == SRSRAN_SUCCESS);
  TESTASSERT(make_phy_dmrs_additional_pos_test() == SRSRAN_SUCCESS);
  TESTASSERT(make_phy_beta_offsets_test() == SRSRAN_SUCCESS);
  TESTASSERT(make_phy_pusch_scaling_test() == SRSRAN_SUCCESS);
  TESTASSERT(make_phy_zp_csi_rs_resource_test() == SRSRAN_SUCCESS);
  TESTASSERT(make_phy_nzp_csi_rs_resource_test() == SRSRAN_SUCCESS);
  srslog::flush();

  printf("Success\n");
  return 0;
}
