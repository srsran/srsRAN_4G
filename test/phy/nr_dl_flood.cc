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

#include "srsran/common/test_common.h"
#include "test_bench.h"

test_bench::args_t::args_t(int argc, char** argv)
{
  // Flag configuration as valid
  valid = true;

  cell_list.resize(1);
  cell_list[0].carrier.nof_prb = 52;

  phy_cfg.carrier     = cell_list[0].carrier;
  phy_cfg.carrier.pci = 500;

  phy_cfg.carrier.absolute_frequency_point_a = 633928;
  phy_cfg.carrier.absolute_frequency_ssb     = 634176;
  phy_cfg.carrier.offset_to_carrier          = 0;
  phy_cfg.carrier.scs                        = srsran_subcarrier_spacing_15kHz;
  phy_cfg.carrier.nof_prb                    = 52;

  phy_cfg.ssb.periodicity_ms       = 5;
  phy_cfg.ssb.position_in_burst[0] = true;
  phy_cfg.ssb.scs                  = srsran_subcarrier_spacing_30kHz;

  phy_cfg.pdcch.coreset_present[1]              = true;
  phy_cfg.pdcch.coreset[1].id                   = 1;
  phy_cfg.pdcch.coreset[1].freq_resources[0]    = true;
  phy_cfg.pdcch.coreset[1].freq_resources[1]    = true;
  phy_cfg.pdcch.coreset[1].freq_resources[2]    = true;
  phy_cfg.pdcch.coreset[1].freq_resources[3]    = true;
  phy_cfg.pdcch.coreset[1].freq_resources[4]    = true;
  phy_cfg.pdcch.coreset[1].freq_resources[5]    = true;
  phy_cfg.pdcch.coreset[1].freq_resources[6]    = true;
  phy_cfg.pdcch.coreset[1].freq_resources[7]    = true;
  phy_cfg.pdcch.coreset[1].duration             = 1;
  phy_cfg.pdcch.coreset[1].mapping_type         = srsran_coreset_mapping_type_non_interleaved;
  phy_cfg.pdcch.coreset[1].precoder_granularity = srsran_coreset_precoder_granularity_reg_bundle;
  phy_cfg.pdcch.coreset_present[2]              = true;
  phy_cfg.pdcch.coreset[2].id                   = 2;
  phy_cfg.pdcch.coreset[2].freq_resources[0]    = true;
  phy_cfg.pdcch.coreset[2].freq_resources[1]    = true;
  phy_cfg.pdcch.coreset[2].freq_resources[2]    = true;
  phy_cfg.pdcch.coreset[2].freq_resources[3]    = true;
  phy_cfg.pdcch.coreset[2].freq_resources[4]    = true;
  phy_cfg.pdcch.coreset[2].freq_resources[5]    = true;
  phy_cfg.pdcch.coreset[2].freq_resources[6]    = true;
  phy_cfg.pdcch.coreset[2].freq_resources[7]    = true;
  phy_cfg.pdcch.coreset[2].duration             = 1;
  phy_cfg.pdcch.coreset[2].mapping_type         = srsran_coreset_mapping_type_non_interleaved;
  phy_cfg.pdcch.coreset[2].precoder_granularity = srsran_coreset_precoder_granularity_reg_bundle;

  phy_cfg.pdcch.search_space_present[1]           = true;
  phy_cfg.pdcch.search_space[1].id                = 1;
  phy_cfg.pdcch.search_space[1].coreset_id        = 1;
  phy_cfg.pdcch.search_space[1].duration          = 1;
  phy_cfg.pdcch.search_space[1].nof_candidates[0] = 1;
  phy_cfg.pdcch.search_space[1].nof_candidates[1] = 1;
  phy_cfg.pdcch.search_space[1].nof_candidates[2] = 1;
  phy_cfg.pdcch.search_space[1].nof_candidates[3] = 0;
  phy_cfg.pdcch.search_space[1].nof_candidates[4] = 0;
  phy_cfg.pdcch.search_space[1].formats[0]        = srsran_dci_format_nr_0_0;
  phy_cfg.pdcch.search_space[1].formats[1]        = srsran_dci_format_nr_1_0;
  phy_cfg.pdcch.search_space[1].nof_formats       = 2;
  phy_cfg.pdcch.search_space[1].type              = srsran_search_space_type_common_3;

  phy_cfg.pdcch.search_space_present[2]           = true;
  phy_cfg.pdcch.search_space[2].id                = 2;
  phy_cfg.pdcch.search_space[2].coreset_id        = 2;
  phy_cfg.pdcch.search_space[2].duration          = 1;
  phy_cfg.pdcch.search_space[2].nof_candidates[0] = 0;
  phy_cfg.pdcch.search_space[2].nof_candidates[1] = 2;
  phy_cfg.pdcch.search_space[2].nof_candidates[2] = 1;
  phy_cfg.pdcch.search_space[2].nof_candidates[3] = 0;
  phy_cfg.pdcch.search_space[2].nof_candidates[4] = 0;
  phy_cfg.pdcch.search_space[2].formats[0]        = srsran_dci_format_nr_0_0;
  phy_cfg.pdcch.search_space[2].formats[1]        = srsran_dci_format_nr_1_0;
  phy_cfg.pdcch.search_space[2].nof_formats       = 2;
  phy_cfg.pdcch.search_space[2].type              = srsran_search_space_type_ue;

  phy_cfg.pdcch.ra_search_space_present = true;
  phy_cfg.pdcch.ra_search_space         = phy_cfg.pdcch.search_space[1];
  phy_cfg.pdcch.ra_search_space.type    = srsran_search_space_type_common_1;

  phy_cfg.pdsch.common_time_ra[0].mapping_type = srsran_sch_mapping_type_A;
  phy_cfg.pdsch.common_time_ra[0].sliv         = 40;
  phy_cfg.pdsch.common_time_ra[1].mapping_type = srsran_sch_mapping_type_A;
  phy_cfg.pdsch.common_time_ra[1].sliv         = 57;
  phy_cfg.pdsch.nof_common_time_ra             = 2;

  phy_cfg.pusch.common_time_ra[0].k            = 4;
  phy_cfg.pusch.common_time_ra[0].mapping_type = srsran_sch_mapping_type_A;
  phy_cfg.pusch.common_time_ra[0].sliv         = 27;
  phy_cfg.pusch.common_time_ra[1].k            = 5;
  phy_cfg.pusch.common_time_ra[1].mapping_type = srsran_sch_mapping_type_A;
  phy_cfg.pusch.common_time_ra[1].sliv         = 27;
  phy_cfg.pusch.nof_common_time_ra             = 2;

  phy_cfg.pdsch.typeA_pos = srsran_dmrs_sch_typeA_pos_2;
  phy_cfg.pusch.typeA_pos = srsran_dmrs_sch_typeA_pos_2;

  phy_cfg.tdd.pattern1.period_ms      = 10;
  phy_cfg.tdd.pattern1.nof_dl_slots   = 7;
  phy_cfg.tdd.pattern1.nof_dl_symbols = 6;
  phy_cfg.tdd.pattern1.nof_ul_slots   = 4;
  phy_cfg.tdd.pattern1.nof_ul_symbols = 4;

  phy_cfg.pdsch.dmrs_typeA.additional_pos = srsran_dmrs_sch_add_pos_1;
  phy_cfg.pdsch.dmrs_typeA.present        = true;
  phy_cfg.pdsch.alloc                     = srsran_resource_alloc_type1;

  phy_cfg.pucch.enabled               = true;
  srsran_pucch_nr_resource_t& pucch0  = phy_cfg.pucch.sets[0].resources[0];
  srsran_pucch_nr_resource_t& pucch1  = phy_cfg.pucch.sets[0].resources[1];
  srsran_pucch_nr_resource_t& pucch2  = phy_cfg.pucch.sets[0].resources[2];
  srsran_pucch_nr_resource_t& pucch3  = phy_cfg.pucch.sets[0].resources[3];
  srsran_pucch_nr_resource_t& pucch4  = phy_cfg.pucch.sets[0].resources[4];
  srsran_pucch_nr_resource_t& pucch5  = phy_cfg.pucch.sets[0].resources[5];
  srsran_pucch_nr_resource_t& pucch6  = phy_cfg.pucch.sets[0].resources[6];
  srsran_pucch_nr_resource_t& pucch7  = phy_cfg.pucch.sets[0].resources[7];
  phy_cfg.pucch.sets[0].nof_resources = 8;
  srsran_pucch_nr_resource_t& pucch8  = phy_cfg.pucch.sets[1].resources[0];
  srsran_pucch_nr_resource_t& pucch9  = phy_cfg.pucch.sets[1].resources[1];
  srsran_pucch_nr_resource_t& pucch10 = phy_cfg.pucch.sets[1].resources[2];
  srsran_pucch_nr_resource_t& pucch11 = phy_cfg.pucch.sets[1].resources[3];
  srsran_pucch_nr_resource_t& pucch12 = phy_cfg.pucch.sets[1].resources[4];
  srsran_pucch_nr_resource_t& pucch13 = phy_cfg.pucch.sets[1].resources[5];
  srsran_pucch_nr_resource_t& pucch14 = phy_cfg.pucch.sets[1].resources[6];
  srsran_pucch_nr_resource_t& pucch15 = phy_cfg.pucch.sets[1].resources[7];
  phy_cfg.pucch.sets[1].nof_resources = 8;

  pucch0.starting_prb         = 0;
  pucch0.format               = SRSRAN_PUCCH_NR_FORMAT_1;
  pucch0.initial_cyclic_shift = 0;
  pucch0.nof_symbols          = 14;
  pucch0.start_symbol_idx     = 0;
  pucch0.time_domain_occ      = 0;
  pucch1                      = pucch0;
  pucch1.initial_cyclic_shift = 4;
  pucch1.time_domain_occ      = 0;
  pucch2                      = pucch0;
  pucch2.initial_cyclic_shift = 8;
  pucch2.time_domain_occ      = 0;
  pucch3                      = pucch0;
  pucch3.initial_cyclic_shift = 0;
  pucch3.time_domain_occ      = 1;
  pucch4                      = pucch0;
  pucch4.initial_cyclic_shift = 0;
  pucch4.time_domain_occ      = 1;
  pucch5                      = pucch0;
  pucch5.initial_cyclic_shift = 4;
  pucch5.time_domain_occ      = 1;
  pucch6                      = pucch0;
  pucch6.initial_cyclic_shift = 0;
  pucch6.time_domain_occ      = 2;
  pucch7                      = pucch0;
  pucch7.initial_cyclic_shift = 4;
  pucch7.time_domain_occ      = 2;

  pucch8.starting_prb     = 51;
  pucch8.format           = SRSRAN_PUCCH_NR_FORMAT_2;
  pucch8.nof_prb          = 1;
  pucch8.nof_symbols      = 2;
  pucch8.start_symbol_idx = 0;

  pucch9                   = pucch8;
  pucch9.start_symbol_idx  = 2;
  pucch10                  = pucch8;
  pucch10.start_symbol_idx = 4;
  pucch11                  = pucch8;
  pucch11.start_symbol_idx = 6;
  pucch12                  = pucch8;
  pucch12.start_symbol_idx = 8;
  pucch13                  = pucch8;
  pucch13.start_symbol_idx = 10;
  pucch14                  = pucch8;
  pucch14.start_symbol_idx = 12;
  pucch15                  = pucch8;
  pucch15.starting_prb     = 1;
  pucch15.start_symbol_idx = 0;

  srsran_pucch_nr_resource_t& pucch16 = phy_cfg.pucch.sr_resources[1].resource;
  pucch16.starting_prb                = 0;
  pucch16.format                      = SRSRAN_PUCCH_NR_FORMAT_1;
  pucch16.initial_cyclic_shift        = 8;
  pucch16.nof_symbols                 = 14;
  pucch16.start_symbol_idx            = 0;
  pucch16.time_domain_occ             = 2;

  phy_cfg.pucch.sr_resources[1].configured = true;
  phy_cfg.pucch.sr_resources[1].sr_id      = 0;
  phy_cfg.pucch.sr_resources[1].period     = 40;
  phy_cfg.pucch.sr_resources[1].offset     = 8;
  phy_cfg.pucch.sr_resources[1].resource   = pucch16;

  phy_cfg.harq_ack.dl_data_to_ul_ack[0] = 8;
  phy_cfg.harq_ack.dl_data_to_ul_ack[1] = 7;
  phy_cfg.harq_ack.dl_data_to_ul_ack[2] = 6;
  phy_cfg.harq_ack.dl_data_to_ul_ack[3] = 5;
  phy_cfg.harq_ack.dl_data_to_ul_ack[4] = 4;
  phy_cfg.harq_ack.dl_data_to_ul_ack[5] = 12;
  phy_cfg.harq_ack.dl_data_to_ul_ack[6] = 11;

  phy_cfg.prach.freq_offset = 2;

  //             pusch-Config: setup (1)
  //                 setup
  //                     dmrs-UplinkForPUSCH-MappingTypeA: setup (1)
  //                         setup
  //                             dmrs-AdditionalPosition: pos1 (1)
  //                             transformPrecodingDisabled
  //                     pusch-PowerControl
  //                         msg3-Alpha: alpha1 (7)
  //                         p0-NominalWithoutGrant: -90dBm
  //                         p0-AlphaSets: 1 item
  //                             Item 0
  //                                 P0-PUSCH-AlphaSet
  //                                     p0-PUSCH-AlphaSetId: 0
  //                                     p0: 0dB
  //                                     alpha: alpha1 (7)
  //                         pathlossReferenceRSToAddModList: 1 item
  //                             Item 0
  //                                 PUSCH-PathlossReferenceRS
  //                                     pusch-PathlossReferenceRS-Id: 0
  //                                     referenceSignal: ssb-Index (0)
  //                                         ssb-Index: 0
  //                         sri-PUSCH-MappingToAddModList: 1 item
  //                             Item 0
  //                                 SRI-PUSCH-PowerControl
  //                                     sri-PUSCH-PowerControlId: 0
  //                                     sri-PUSCH-PathlossReferenceRS-Id: 0
  //                                     sri-P0-PUSCH-AlphaSetId: 0
  //                                     sri-PUSCH-ClosedLoopIndex: i0 (0)
  //                     resourceAllocation: resourceAllocationType1 (1)
  //                     uci-OnPUSCH: setup (1)
  //                         setup
  //                             betaOffsets: semiStatic (1)
  //                                 semiStatic
  //                                     betaOffsetACK-Index1: 9
  //                                     betaOffsetACK-Index2: 9
  //                                     betaOffsetACK-Index3: 9
  //                                     betaOffsetCSI-Part1-Index1: 6
  //                                     betaOffsetCSI-Part1-Index2: 6
  //                                     betaOffsetCSI-Part2-Index1: 6
  //                                     betaOffsetCSI-Part2-Index2: 6
  //                             scaling: f1 (3)
  //             srs-Config: setup (1)
  //                 setup
  //                     srs-ResourceSetToAddModList: 1 item
  //                         Item 0
  //                             SRS-ResourceSet
  //                                 srs-ResourceSetId: 0
  //                                 srs-ResourceIdList: 1 item
  //                                     Item 0
  //                                         SRS-ResourceId: 0
  //                                 resourceType: aperiodic (0)
  //                                     aperiodic
  //                                         aperiodicSRS-ResourceTrigger: 1
  //                                         slotOffset: 7
  //                                 usage: codebook (1)
  //                                 p0: -90dBm
  //                                 pathlossReferenceRS: ssb-Index (0)
  //                                     ssb-Index: 0
  //                     srs-ResourceToAddModList: 1 item
  //                         Item 0
  //                             SRS-Resource
  //                                 srs-ResourceId: 0
  //                                 nrofSRS-Ports: port1 (0)
  //                                 transmissionComb: n2 (0)
  //                                     n2
  //                                         combOffset-n2: 0
  //                                         cyclicShift-n2: 0
  //                                 resourceMapping
  //                                     startPosition: 0
  //                                     nrofSymbols: n1 (0)
  //                                     repetitionFactor: n1 (0)
  //                                 freqDomainPosition: 0
  //                                 freqDomainShift: 6
  //                                 freqHopping
  //                                     c-SRS: 11
  //                                     b-SRS: 3
  //                                     b-hop: 0
  //                                 groupOrSequenceHopping: neither (0)
  //                                 resourceType: aperiodic (0)
  //                                     aperiodic
  //                                 sequenceId: 500
  //         firstActiveUplinkBWP-Id: 0
  //         pusch-ServingCellConfig: setup (1)
  //             setup
  //     pdcch-ServingCellConfig: setup (1)
  //         setup
  //     pdsch-ServingCellConfig: setup (1)
  //         setup
  //             nrofHARQ-ProcessesForPDSCH: n16 (5)
  //     csi-MeasConfig: setup (1)
  //         setup
  //             nzp-CSI-RS-ResourceToAddModList: 5 items
  //                 Item 0
  //                     NZP-CSI-RS-Resource
  //                         nzp-CSI-RS-ResourceId: 0
  //                         resourceMapping
  //                             frequencyDomainAllocation: row2 (1)
  //                                 row2: 8000 [bit length 12, 4 LSB pad bits, 1000 0000  0000
  //                                 .... decimal value 2048]
  //                             nrofPorts: p1 (0)
  //                             firstOFDMSymbolInTimeDomain: 4
  //                             cdm-Type: noCDM (0)
  //                             density: one (1)
  //                                 one: NULL
  //                             freqBand
  //                                 startingRB: 0
  //                                 nrofRBs: 52
  //                         powerControlOffset: 0dB
  //                         powerControlOffsetSS: db0 (1)
  //                         scramblingID: 0
  //                         periodicityAndOffset: slots80 (9)
  //                             slots80: 1
  //                         qcl-InfoPeriodicCSI-RS: 0
  //                 Item 1
  //                     NZP-CSI-RS-Resource
  //                         nzp-CSI-RS-ResourceId: 1
  //                         resourceMapping
  //                             frequencyDomainAllocation: row1 (0)
  //                                 row1: 10 [bit length 4, 4 LSB pad bits, 0001 .... decimal
  //                                 value 1]
  //                             nrofPorts: p1 (0)
  //                             firstOFDMSymbolInTimeDomain: 4
  //                             cdm-Type: noCDM (0)
  //                             density: three (2)
  //                                 three: NULL
  //                             freqBand
  //                                 startingRB: 0
  //                                 nrofRBs: 52
  //                         powerControlOffset: 0dB
  //                         powerControlOffsetSS: db0 (1)
  //                         scramblingID: 0
  //                         periodicityAndOffset: slots40 (7)
  //                             slots40: 11
  //                         qcl-InfoPeriodicCSI-RS: 0
  //                 Item 2
  //                     NZP-CSI-RS-Resource
  //                         nzp-CSI-RS-ResourceId: 2
  //                         resourceMapping
  //                             frequencyDomainAllocation: row1 (0)
  //                                 row1: 10 [bit length 4, 4 LSB pad bits, 0001 .... decimal
  //                                 value 1]
  //                             nrofPorts: p1 (0)
  //                             firstOFDMSymbolInTimeDomain: 8
  //                             cdm-Type: noCDM (0)
  //                             density: three (2)
  //                                 three: NULL
  //                             freqBand
  //                                 startingRB: 0
  //                                 nrofRBs: 52
  //                         powerControlOffset: 0dB
  //                         powerControlOffsetSS: db0 (1)
  //                         scramblingID: 0
  //                         periodicityAndOffset: slots40 (7)
  //                             slots40: 11
  //                         qcl-InfoPeriodicCSI-RS: 0
  //                 Item 3
  //                     NZP-CSI-RS-Resource
  //                         nzp-CSI-RS-ResourceId: 3
  //                         resourceMapping
  //                             frequencyDomainAllocation: row1 (0)
  //                                 row1: 10 [bit length 4, 4 LSB pad bits, 0001 .... decimal
  //                                 value 1]
  //                             nrofPorts: p1 (0)
  //                             firstOFDMSymbolInTimeDomain: 4
  //                             cdm-Type: noCDM (0)
  //                             density: three (2)
  //                                 three: NULL
  //                             freqBand
  //                                 startingRB: 0
  //                                 nrofRBs: 52
  //                         powerControlOffset: 0dB
  //                         powerControlOffsetSS: db0 (1)
  //                         scramblingID: 0
  //                         periodicityAndOffset: slots40 (7)
  //                             slots40: 12
  //                         qcl-InfoPeriodicCSI-RS: 0
  //                 Item 4
  //                     NZP-CSI-RS-Resource
  //                         nzp-CSI-RS-ResourceId: 4
  //                         resourceMapping
  //                             frequencyDomainAllocation: row1 (0)
  //                                 row1: 10 [bit length 4, 4 LSB pad bits, 0001 .... decimal
  //                                 value 1]
  //                             nrofPorts: p1 (0)
  //                             firstOFDMSymbolInTimeDomain: 8
  //                             cdm-Type: noCDM (0)
  //                             density: three (2)
  //                                 three: NULL
  //                             freqBand
  //                                 startingRB: 0
  //                                 nrofRBs: 52
  //                         powerControlOffset: 0dB
  //                         powerControlOffsetSS: db0 (1)
  //                         scramblingID: 0
  //                         periodicityAndOffset: slots40 (7)
  //                             slots40: 12
  //                         qcl-InfoPeriodicCSI-RS: 0
  //             nzp-CSI-RS-ResourceSetToAddModList: 2 items
  //                 Item 0
  //                     NZP-CSI-RS-ResourceSet
  //                         nzp-CSI-ResourceSetId: 0
  //                         nzp-CSI-RS-Resources: 1 item
  //                             Item 0
  //                                 NZP-CSI-RS-ResourceId: 0
  //                 Item 1
  //                     NZP-CSI-RS-ResourceSet
  //                         nzp-CSI-ResourceSetId: 1
  //                         nzp-CSI-RS-Resources: 4 items
  //                             Item 0
  //                                 NZP-CSI-RS-ResourceId: 1
  //                             Item 1
  //                                 NZP-CSI-RS-ResourceId: 2
  //                             Item 2
  //                                 NZP-CSI-RS-ResourceId: 3
  //                             Item 3
  //                                 NZP-CSI-RS-ResourceId: 4
  //                         trs-Info: true (0)
  //             csi-IM-ResourceToAddModList: 1 item
  //                 Item 0
  //                     CSI-IM-Resource
  //                         csi-IM-ResourceId: 0
  //                         csi-IM-ResourceElementPattern: pattern1 (1)
  //                             pattern1
  //                                 subcarrierLocation-p1: s8 (2)
  //                                 symbolLocation-p1: 8
  //                         freqBand
  //                             startingRB: 0
  //                             nrofRBs: 52
  //                         periodicityAndOffset: slots80 (9)
  //                             slots80: 1
  //             csi-IM-ResourceSetToAddModList: 1 item
  //                 Item 0
  //                     CSI-IM-ResourceSet
  //                         csi-IM-ResourceSetId: 0
  //                         csi-IM-Resources: 1 item
  //                             Item 0
  //                                 CSI-IM-ResourceId: 0
  //             csi-ResourceConfigToAddModList: 3 items
  //                 Item 0
  //                     CSI-ResourceConfig
  //                         csi-ResourceConfigId: 0
  //                         csi-RS-ResourceSetList: nzp-CSI-RS-SSB (0)
  //                             nzp-CSI-RS-SSB
  //                                 nzp-CSI-RS-ResourceSetList: 1 item
  //                                     Item 0
  //                                         NZP-CSI-RS-ResourceSetId: 0
  //                         bwp-Id: 0
  //                         resourceType: periodic (2)
  //                 Item 1
  //                     CSI-ResourceConfig
  //                         csi-ResourceConfigId: 1
  //                         csi-RS-ResourceSetList: csi-IM-ResourceSetList (1)
  //                             csi-IM-ResourceSetList: 1 item
  //                                 Item 0
  //                                     CSI-IM-ResourceSetId: 0
  //                         bwp-Id: 0
  //                         resourceType: periodic (2)
  //                 Item 2
  //                     CSI-ResourceConfig
  //                         csi-ResourceConfigId: 2
  //                         csi-RS-ResourceSetList: nzp-CSI-RS-SSB (0)
  //                             nzp-CSI-RS-SSB
  //                                 nzp-CSI-RS-ResourceSetList: 1 item
  //                                     Item 0
  //                                         NZP-CSI-RS-ResourceSetId: 1
  //                         bwp-Id: 0
  //                         resourceType: periodic (2)
  //             csi-ReportConfigToAddModList: 1 item
  //                 Item 0
  //                     CSI-ReportConfig
  //                         reportConfigId: 0
  //                         resourcesForChannelMeasurement: 0
  //                         csi-IM-ResourcesForInterference: 1
  //                         reportConfigType: periodic (0)
  //                             periodic
  //                                 reportSlotConfig: slots80 (7)
  //                                     slots80: 9
  //                                 pucch-CSI-ResourceList: 1 item
  //                                     Item 0
  //                                         PUCCH-CSI-Resource
  //                                             uplinkBandwidthPartId: 0
  //                                             pucch-Resource: 17
  //                         reportQuantity: cri-RI-PMI-CQI (1)
  //                             cri-RI-PMI-CQI: NULL
  //                         reportFreqConfiguration
  //                             cqi-FormatIndicator: widebandCQI (0)
  //                         timeRestrictionForChannelMeasurements: notConfigured (1)
  //                         timeRestrictionForInterferenceMeasurements: notConfigured (1)
  //                         groupBasedBeamReporting: disabled (1)
  //                             disabled
  //                         cqi-Table: table2 (1)
  //                         subbandSize: value1 (0)
  //     tag-Id: 0
}

class ue_dummy_stack : public srsue::stack_interface_phy_nr
{
public:
  void         in_sync() override {}
  void         out_of_sync() override {}
  void         run_tti(const uint32_t tti) override {}
  int          sf_indication(const uint32_t tti) override { return 0; }
  sched_rnti_t get_dl_sched_rnti_nr(const uint32_t tti) override { return sched_rnti_t(); }
  sched_rnti_t get_ul_sched_rnti_nr(const uint32_t tti) override { return sched_rnti_t(); }
  void         new_grant_dl(const uint32_t cc_idx, const mac_nr_grant_dl_t& grant, tb_action_dl_t* action) override {}
  void tb_decoded(const uint32_t cc_idx, const mac_nr_grant_dl_t& grant, tb_action_dl_result_t result) override {}
  void new_grant_ul(const uint32_t cc_idx, const mac_nr_grant_ul_t& grant, tb_action_ul_t* action) override {}
  void prach_sent(uint32_t tti, uint32_t s_id, uint32_t t_id, uint32_t f_id, uint32_t ul_carrier_id) override {}
  bool sr_opportunity(uint32_t tti, uint32_t sr_id, bool meas_gap, bool ul_sch_tx) override { return false; }
};

class gnb_dummy_stack : public srsenb::stack_interface_phy_nr
{
public:
  int sf_indication(const uint32_t tti) override { return 0; }
  int rx_data_indication(rx_data_ind_t& grant) override { return 0; }
  int get_dl_sched(uint32_t tti, dl_sched_list_t& dl_sched_res) override
  {
    dl_sched_res[0].nof_grants = 0;
    return SRSRAN_SUCCESS;
  }
  int get_ul_sched(uint32_t tti, ul_sched_list_t& ul_sched_res) override { return 0; }
};

int main(int argc, char** argv)
{
  test_bench::args_t args(argc, argv);

  // Parse arguments
  TESTASSERT(args.valid);

  ue_dummy_stack  ue_stack;
  gnb_dummy_stack gnb_stack;

  // Create test bench
  test_bench tb(args, gnb_stack, ue_stack);

  // Assert bench is initialised correctly
  TESTASSERT(tb.is_initialised());

  for (uint32_t i = 0; i < 1000; i++) {
    TESTASSERT(tb.run_tti());
  }

  // If reached here, the test is successful
  return SRSRAN_SUCCESS;
}