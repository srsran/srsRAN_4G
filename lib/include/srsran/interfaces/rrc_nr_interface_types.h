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

#ifndef SRSRAN_RRC_NR_INTERFACE_TYPES_H
#define SRSRAN_RRC_NR_INTERFACE_TYPES_H

#include "srsran/config.h"
#include "srsran/srsran.h"
#include <string>

namespace srsran {

/***************************
 *      PHY Config
 **************************/

struct phy_cfg_nr_t {
  srsran_tdd_config_nr_t         tdd      = {};
  srsran_sch_hl_cfg_nr_t         pdsch    = {};
  srsran_sch_hl_cfg_nr_t         pusch    = {};
  srsran_pucch_nr_hl_cfg_t       pucch    = {};
  srsran_prach_cfg_t             prach    = {};
  srsran_ue_dl_nr_pdcch_cfg_t    pdcch    = {};
  srsran_ue_dl_nr_harq_ack_cfg_t harq_ack = {};
  srsran_csi_hl_cfg_t            csi      = {};

  phy_cfg_nr_t()
  {
    // tdd-UL-DL-ConfigurationCommon
    //    referenceSubcarrierSpacing: kHz15 (0)
    //    pattern1
    //        dl-UL-TransmissionPeriodicity: ms10 (7)
    //        nrofDownlinkSlots: 7
    //        nrofDownlinkSymbols: 6
    //        nrofUplinkSlots: 2
    //        nrofUplinkSymbols: 4
    tdd.pattern1.period_ms      = 10;
    tdd.pattern1.nof_dl_slots   = 7;
    tdd.pattern1.nof_dl_symbols = 6;
    tdd.pattern1.nof_ul_slots   = 2;
    tdd.pattern1.nof_ul_symbols = 4;
    tdd.pattern2.period_ms      = 0;

    // physicalCellGroupConfig
    //    pdsch-HARQ-ACK-Codebook: dynamic (1)
    harq_ack.pdsch_harq_ack_codebook = srsran_pdsch_harq_ack_codebook_dynamic;

    // commonControlResourceSet
    //    controlResourceSetId: 1
    //    frequencyDomainResources: ff0000000000
    //    duration: 1
    //    cce-REG-MappingType: nonInterleaved (1)
    //        nonInterleaved: NULL
    //    precoderGranularity: sameAsREG-bundle (0)
    pdcch.coreset[1].coreset_id           = 1;
    pdcch.coreset[1].precoder_granularity = srsran_coreset_precoder_granularity_reg_bundle;
    pdcch.coreset[1].duration             = 1;
    pdcch.coreset[1].mapping_type         = srsran_coreset_mapping_type_non_interleaved;
    for (uint32_t i = 0; i < SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE; i++) {
      pdcch.coreset[1].freq_resources[i] = (i < 8);
    }
    pdcch.coreset_present[1] = true;

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
    srsran_search_space_t search_space1 = {};
    search_space1.id                    = 1;
    search_space1.coreset_id            = 1;
    search_space1.nof_candidates[0]     = 1;
    search_space1.nof_candidates[1]     = 1;
    search_space1.nof_candidates[2]     = 1;
    search_space1.nof_candidates[3]     = 0;
    search_space1.nof_candidates[4]     = 0;
    search_space1.type                  = srsran_search_space_type_common_3;
    pdcch.search_space[1]               = search_space1;
    pdcch.search_space_present[1]       = true;

    // ra-SearchSpace: 1
    pdcch.ra_rnti                 = 0x16; //< Supposed to be deduced from PRACH configuration
    pdcch.ra_search_space         = search_space1;
    pdcch.ra_search_space.type    = srsran_search_space_type_common_1;
    pdcch.ra_search_space_present = true;

    // spCellConfigDedicated
    //     initialDownlinkBWP
    //         pdcch-Config: setup (1)
    //             setup
    //                 controlResourceSetToAddModList: 1 item
    //                     Item 0
    //                         ControlResourceSet
    //                             controlResourceSetId: 2
    //                             frequencyDomainResources: ff0000000000 [bit length 45, 3 LSB pad bits, 1111 1111 0000
    //                             0000  0000 0000  0000 0000  0000 0000  0000 0... decimal value 35046933135360]
    //                             duration: 1
    //                             cce-REG-MappingType: nonInterleaved (1)
    //                                 nonInterleaved: NULL
    //                             precoderGranularity: sameAsREG-bundle (0)
    pdcch.coreset[2].id                   = 2;
    pdcch.coreset[2].precoder_granularity = srsran_coreset_precoder_granularity_reg_bundle;
    pdcch.coreset[2].duration             = 1;
    pdcch.coreset[2].mapping_type         = srsran_coreset_mapping_type_non_interleaved;
    for (uint32_t i = 0; i < SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE; i++) {
      pdcch.coreset[2].freq_resources[i] = (i < 8);
    }
    pdcch.coreset_present[2] = true;

    //                 searchSpacesToAddModList: 1 item
    //                     Item 0
    //                         SearchSpace
    //                             searchSpaceId: 2
    //                             controlResourceSetId: 2
    //                             monitoringSlotPeriodicityAndOffset: sl1 (0)
    //                                 sl1: NULL
    //                             monitoringSymbolsWithinSlot: 8000 [bit length 14, 2 LSB pad bits, 1000 0000  0000
    //                             00.. decimal value 8192] nrofCandidates
    //                                 aggregationLevel1: n0 (0)
    //                                 aggregationLevel2: n2 (2)
    //                                 aggregationLevel4: n1 (1)
    //                                 aggregationLevel8: n0 (0)
    //                                 aggregationLevel16: n0 (0)
    //                             searchSpaceType: ue-Specific (1)
    //                                 ue-Specific
    //                                     dci-Formats: formats0-0-And-1-0 (0)
    pdcch.search_space[2].id                = 2;
    pdcch.search_space[2].coreset_id        = 2;
    pdcch.search_space[2].nof_candidates[0] = 0;
    pdcch.search_space[2].nof_candidates[1] = 2;
    pdcch.search_space[2].nof_candidates[2] = 1;
    pdcch.search_space[2].nof_candidates[3] = 0;
    pdcch.search_space[2].nof_candidates[4] = 0;
    pdcch.search_space[2].type              = srsran_search_space_type_ue;
    pdcch.search_space_present[2]           = true;
    // pdsch-Config: setup (1)
    //     setup
    //         dmrs-DownlinkForPDSCH-MappingTypeA: setup (1)
    //             setup
    //                 dmrs-AdditionalPosition: pos1 (1)
    //         tci-StatesToAddModList: 1 item
    //             Item 0
    //                 TCI-State
    //                     tci-StateId: 0
    //                     qcl-Type1
    //                         referenceSignal: ssb (1)
    //                             ssb: 0
    //                         qcl-Type: typeD (3)
    //         resourceAllocation: resourceAllocationType1 (1)
    //         rbg-Size: config1 (0)
    //         prb-BundlingType: staticBundling (0)
    //             staticBundling
    //                 bundleSize: wideband (1)
    //         zp-CSI-RS-ResourceToAddModList: 1 item
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
    //         p-ZP-CSI-RS-ResourceSet: setup (1)
    //             setup
    //                 zp-CSI-RS-ResourceSetId: 0
    //                 zp-CSI-RS-ResourceIdList: 1 item
    //                     Item 0
    //                         ZP-CSI-RS-ResourceId: 0
    srsran_csi_rs_zp_resource_t zp_csi_rs_resource0                = {};
    zp_csi_rs_resource0.resource_mapping.row                       = srsran_csi_rs_resource_mapping_row_4;
    zp_csi_rs_resource0.resource_mapping.frequency_domain_alloc[0] = true;
    zp_csi_rs_resource0.resource_mapping.frequency_domain_alloc[1] = false;
    zp_csi_rs_resource0.resource_mapping.frequency_domain_alloc[2] = false;
    zp_csi_rs_resource0.resource_mapping.nof_ports                 = 4;
    zp_csi_rs_resource0.resource_mapping.first_symbol_idx          = 8;
    zp_csi_rs_resource0.resource_mapping.cdm                       = srsran_csi_rs_cdm_fd_cdm2;
    zp_csi_rs_resource0.resource_mapping.density                   = srsran_csi_rs_resource_mapping_density_one;
    zp_csi_rs_resource0.resource_mapping.freq_band.start_rb        = 0;
    zp_csi_rs_resource0.resource_mapping.freq_band.nof_rb          = 52;
    zp_csi_rs_resource0.periodicity.period                         = 80;
    zp_csi_rs_resource0.periodicity.offset                         = 1;
    pdsch.p_zp_csi_rs_set.data[0]                                  = zp_csi_rs_resource0;
    pdsch.p_zp_csi_rs_set.count                                    = 1;

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
    pdsch.common_time_ra[0].mapping_type = srsran_sch_mapping_type_A;
    pdsch.common_time_ra[0].sliv         = 40;
    pdsch.common_time_ra[0].k            = 0;
    pdsch.common_time_ra[1].mapping_type = srsran_sch_mapping_type_A;
    pdsch.common_time_ra[1].sliv         = 57;
    pdsch.common_time_ra[1].k            = 0;
    pdsch.nof_common_time_ra             = 2;

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
    pusch.common_time_ra[0].mapping_type = srsran_sch_mapping_type_A;
    pusch.common_time_ra[0].sliv         = 27;
    pusch.common_time_ra[0].k            = 4;
    pusch.common_time_ra[1].mapping_type = srsran_sch_mapping_type_A;
    pusch.common_time_ra[1].sliv         = 27;
    pusch.common_time_ra[1].k            = 5;
    pusch.nof_common_time_ra             = 2;

    // pusch-Config: setup (1)
    //    setup
    //        dmrs-UplinkForPUSCH-MappingTypeA: setup (1)
    //            setup
    //                dmrs-AdditionalPosition: pos1 (1)
    //                transformPrecodingDisabled
    pusch.dmrs_typeA.additional_pos = srsran_dmrs_sch_add_pos_1;
    pusch.dmrs_typeA.present        = true;
    //        pusch-PowerControl
    //            msg3-Alpha: alpha1 (7)
    //            p0-NominalWithoutGrant: -90dBm
    //            p0-AlphaSets: 1 item
    //                Item 0
    //                    P0-PUSCH-AlphaSet
    //                        p0-PUSCH-AlphaSetId: 0
    //                        p0: 0dB
    //                        alpha: alpha1 (7)
    //            pathlossReferenceRSToAddModList: 1 item
    //                Item 0
    //                    PUSCH-PathlossReferenceRS
    //                        pusch-PathlossReferenceRS-Id: 0
    //                        referenceSignal: ssb-Index (0)
    //                            ssb-Index: 0
    //            sri-PUSCH-MappingToAddModList: 1 item
    //                Item 0
    //                    SRI-PUSCH-PowerControl
    //                        sri-PUSCH-PowerControlId: 0
    //                        sri-PUSCH-PathlossReferenceRS-Id: 0
    //                        sri-P0-PUSCH-AlphaSetId: 0
    //                        sri-PUSCH-ClosedLoopIndex: i0 (0)
    //        resourceAllocation: resourceAllocationType1 (1)
    //        uci-OnPUSCH: setup (1)
    //            setup
    //                betaOffsets: semiStatic (1)
    //                    semiStatic
    //                        betaOffsetACK-Index1: 9
    //                        betaOffsetACK-Index2: 9
    //                        betaOffsetACK-Index3: 9
    pusch.beta_offsets.ack_index1 = 9;
    pusch.beta_offsets.ack_index2 = 9;
    pusch.beta_offsets.ack_index3 = 9;

    //                        betaOffsetCSI-Part1-Index1: 6
    //                        betaOffsetCSI-Part1-Index2: 6
    pusch.beta_offsets.csi1_index1 = 6;
    pusch.beta_offsets.csi1_index2 = 6;

    //                        betaOffsetCSI-Part2-Index1: 6
    //                        betaOffsetCSI-Part2-Index2: 6
    pusch.beta_offsets.csi2_index1 = 6;
    pusch.beta_offsets.csi2_index2 = 6;

    //                scaling: f1 (3)
    pusch.scaling = 1;

    // pucch-Config: setup (1)
    //    setup
    //        resourceSetToAddModList: 2 items
    pucch.enabled = true;
    //            Item 0
    //                PUCCH-ResourceSet
    //                    pucch-ResourceSetId: 0
    //                    resourceList: 8 items
    //                        Item 0
    //                            PUCCH-ResourceId: 0
    //                        Item 1
    //                            PUCCH-ResourceId: 1
    //                        Item 2
    //                            PUCCH-ResourceId: 2
    //                        Item 3
    //                            PUCCH-ResourceId: 3
    //                        Item 4
    //                            PUCCH-ResourceId: 4
    //                        Item 5
    //                            PUCCH-ResourceId: 5
    //                        Item 6
    //                            PUCCH-ResourceId: 6
    //                        Item 7
    //                            PUCCH-ResourceId: 7
    pucch.sets[0].nof_resources = 8;

    //            Item 1
    //                PUCCH-ResourceSet
    //                    pucch-ResourceSetId: 1
    //                    resourceList: 8 items
    //                        Item 0
    //                            PUCCH-ResourceId: 8
    //                        Item 1
    //                            PUCCH-ResourceId: 9
    //                        Item 2
    //                            PUCCH-ResourceId: 10
    //                        Item 3
    //                            PUCCH-ResourceId: 11
    //                        Item 4
    //                            PUCCH-ResourceId: 12
    //                        Item 5
    //                            PUCCH-ResourceId: 13
    //                        Item 6
    //                            PUCCH-ResourceId: 14
    //                        Item 7
    //                            PUCCH-ResourceId: 15
    pucch.sets[1].nof_resources = 8;

    //        resourceToAddModList: 18 items
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
    pucch.sets[0].resources[0].format               = SRSRAN_PUCCH_NR_FORMAT_1;
    pucch.sets[0].resources[0].starting_prb         = 0;
    pucch.sets[0].resources[0].initial_cyclic_shift = 0;
    pucch.sets[0].resources[0].nof_symbols          = 14;
    pucch.sets[0].resources[0].start_symbol_idx     = 0;
    pucch.sets[0].resources[0].time_domain_occ      = 0;

    //            Item 1
    //                PUCCH-Resource
    //                    pucch-ResourceId: 1
    //                    startingPRB: 0
    //                    format: format1 (1)
    //                        format1
    //                            initialCyclicShift: 4
    //                            nrofSymbols: 14
    //                            startingSymbolIndex: 0
    //                            timeDomainOCC: 0
    pucch.sets[0].resources[1].format               = SRSRAN_PUCCH_NR_FORMAT_1;
    pucch.sets[0].resources[1].starting_prb         = 0;
    pucch.sets[0].resources[1].initial_cyclic_shift = 4;
    pucch.sets[0].resources[1].nof_symbols          = 14;
    pucch.sets[0].resources[1].start_symbol_idx     = 0;
    pucch.sets[0].resources[1].time_domain_occ      = 0;

    //            Item 2
    //                PUCCH-Resource
    //                    pucch-ResourceId: 2
    //                    startingPRB: 0
    //                    format: format1 (1)
    //                        format1
    //                            initialCyclicShift: 8
    //                            nrofSymbols: 14
    //                            startingSymbolIndex: 0
    //                            timeDomainOCC: 0
    pucch.sets[0].resources[2].format               = SRSRAN_PUCCH_NR_FORMAT_1;
    pucch.sets[0].resources[2].starting_prb         = 0;
    pucch.sets[0].resources[2].initial_cyclic_shift = 8;
    pucch.sets[0].resources[2].nof_symbols          = 14;
    pucch.sets[0].resources[2].start_symbol_idx     = 0;
    pucch.sets[0].resources[2].time_domain_occ      = 0;

    //            Item 3
    //                PUCCH-Resource
    //                    pucch-ResourceId: 3
    //                    startingPRB: 0
    //                    format: format1 (1)
    //                        format1
    //                            initialCyclicShift: 0
    //                            nrofSymbols: 14
    //                            startingSymbolIndex: 0
    //                            timeDomainOCC: 1
    pucch.sets[0].resources[3].format               = SRSRAN_PUCCH_NR_FORMAT_1;
    pucch.sets[0].resources[3].starting_prb         = 0;
    pucch.sets[0].resources[3].initial_cyclic_shift = 0;
    pucch.sets[0].resources[3].nof_symbols          = 14;
    pucch.sets[0].resources[3].start_symbol_idx     = 0;
    pucch.sets[0].resources[3].time_domain_occ      = 1;

    //            Item 4
    //                PUCCH-Resource
    //                    pucch-ResourceId: 4
    //                    startingPRB: 0
    //                    format: format1 (1)
    //                        format1
    //                            initialCyclicShift: 4
    //                            nrofSymbols: 14
    //                            startingSymbolIndex: 0
    //                            timeDomainOCC: 1
    pucch.sets[0].resources[4].format               = SRSRAN_PUCCH_NR_FORMAT_1;
    pucch.sets[0].resources[4].starting_prb         = 0;
    pucch.sets[0].resources[4].initial_cyclic_shift = 4;
    pucch.sets[0].resources[4].nof_symbols          = 14;
    pucch.sets[0].resources[4].start_symbol_idx     = 0;
    pucch.sets[0].resources[4].time_domain_occ      = 1;

    //            Item 5
    //                PUCCH-Resource
    //                    pucch-ResourceId: 5
    //                    startingPRB: 0
    //                    format: format1 (1)
    //                        format1
    //                            initialCyclicShift: 8
    //                            nrofSymbols: 14
    //                            startingSymbolIndex: 0
    //                            timeDomainOCC: 1
    pucch.sets[0].resources[5].format               = SRSRAN_PUCCH_NR_FORMAT_1;
    pucch.sets[0].resources[5].starting_prb         = 0;
    pucch.sets[0].resources[5].initial_cyclic_shift = 8;
    pucch.sets[0].resources[5].nof_symbols          = 14;
    pucch.sets[0].resources[5].start_symbol_idx     = 0;
    pucch.sets[0].resources[5].time_domain_occ      = 1;

    //            Item 6
    //                PUCCH-Resource
    //                    pucch-ResourceId: 6
    //                    startingPRB: 0
    //                    format: format1 (1)
    //                        format1
    //                            initialCyclicShift: 0
    //                            nrofSymbols: 14
    //                            startingSymbolIndex: 0
    //                            timeDomainOCC: 2
    pucch.sets[0].resources[6].format               = SRSRAN_PUCCH_NR_FORMAT_1;
    pucch.sets[0].resources[6].starting_prb         = 0;
    pucch.sets[0].resources[6].initial_cyclic_shift = 0;
    pucch.sets[0].resources[6].nof_symbols          = 14;
    pucch.sets[0].resources[6].start_symbol_idx     = 0;
    pucch.sets[0].resources[6].time_domain_occ      = 2;

    //            Item 7
    //                PUCCH-Resource
    //                    pucch-ResourceId: 7
    //                    startingPRB: 0
    //                    format: format1 (1)
    //                        format1
    //                            initialCyclicShift: 4
    //                            nrofSymbols: 14
    //                            startingSymbolIndex: 0
    //                            timeDomainOCC: 2
    pucch.sets[0].resources[7].format               = SRSRAN_PUCCH_NR_FORMAT_1;
    pucch.sets[0].resources[7].starting_prb         = 0;
    pucch.sets[0].resources[7].initial_cyclic_shift = 0;
    pucch.sets[0].resources[7].nof_symbols          = 14;
    pucch.sets[0].resources[7].start_symbol_idx     = 0;
    pucch.sets[0].resources[7].time_domain_occ      = 2;

    //            Item 8
    //                PUCCH-Resource
    //                    pucch-ResourceId: 8
    //                    startingPRB: 51
    //                    format: format2 (2)
    //                        format2
    //                            nrofPRBs: 1
    //                            nrofSymbols: 2
    //                            startingSymbolIndex: 0
    pucch.sets[1].resources[0].format           = SRSRAN_PUCCH_NR_FORMAT_2;
    pucch.sets[1].resources[0].starting_prb     = 51;
    pucch.sets[1].resources[0].nof_prb          = 1;
    pucch.sets[1].resources[0].nof_symbols      = 2;
    pucch.sets[1].resources[0].start_symbol_idx = 0;

    //            Item 9
    //                PUCCH-Resource
    //                    pucch-ResourceId: 9
    //                    startingPRB: 51
    //                    format: format2 (2)
    //                        format2
    //                            nrofPRBs: 1
    //                            nrofSymbols: 2
    //                            startingSymbolIndex: 2
    pucch.sets[1].resources[1].format           = SRSRAN_PUCCH_NR_FORMAT_2;
    pucch.sets[1].resources[1].starting_prb     = 51;
    pucch.sets[1].resources[1].nof_prb          = 1;
    pucch.sets[1].resources[1].nof_symbols      = 2;
    pucch.sets[1].resources[1].start_symbol_idx = 2;

    //            Item 10
    //                PUCCH-Resource
    //                    pucch-ResourceId: 10
    //                    startingPRB: 51
    //                    format: format2 (2)
    //                        format2
    //                            nrofPRBs: 1
    //                            nrofSymbols: 2
    //                            startingSymbolIndex: 4
    pucch.sets[1].resources[2].format           = SRSRAN_PUCCH_NR_FORMAT_2;
    pucch.sets[1].resources[2].starting_prb     = 51;
    pucch.sets[1].resources[2].nof_prb          = 1;
    pucch.sets[1].resources[2].nof_symbols      = 2;
    pucch.sets[1].resources[2].start_symbol_idx = 4;

    //            Item 11
    //                PUCCH-Resource
    //                    pucch-ResourceId: 11
    //                    startingPRB: 51
    //                    format: format2 (2)
    //                        format2
    //                            nrofPRBs: 1
    //                            nrofSymbols: 2
    //                            startingSymbolIndex: 6
    pucch.sets[1].resources[3].format           = SRSRAN_PUCCH_NR_FORMAT_2;
    pucch.sets[1].resources[3].starting_prb     = 51;
    pucch.sets[1].resources[3].nof_prb          = 1;
    pucch.sets[1].resources[3].nof_symbols      = 2;
    pucch.sets[1].resources[3].start_symbol_idx = 6;

    //            Item 12
    //                PUCCH-Resource
    //                    pucch-ResourceId: 12
    //                    startingPRB: 51
    //                    format: format2 (2)
    //                        format2
    //                            nrofPRBs: 1
    //                            nrofSymbols: 2
    //                            startingSymbolIndex: 8
    pucch.sets[1].resources[4].format           = SRSRAN_PUCCH_NR_FORMAT_2;
    pucch.sets[1].resources[4].starting_prb     = 51;
    pucch.sets[1].resources[4].nof_prb          = 1;
    pucch.sets[1].resources[4].nof_symbols      = 2;
    pucch.sets[1].resources[4].start_symbol_idx = 8;

    //            Item 13
    //                PUCCH-Resource
    //                    pucch-ResourceId: 13
    //                    startingPRB: 51
    //                    format: format2 (2)
    //                        format2
    //                            nrofPRBs: 1
    //                            nrofSymbols: 2
    //                            startingSymbolIndex: 10
    pucch.sets[1].resources[5].format           = SRSRAN_PUCCH_NR_FORMAT_2;
    pucch.sets[1].resources[5].starting_prb     = 51;
    pucch.sets[1].resources[5].nof_prb          = 1;
    pucch.sets[1].resources[5].nof_symbols      = 2;
    pucch.sets[1].resources[5].start_symbol_idx = 10;

    //            Item 14
    //                PUCCH-Resource
    //                    pucch-ResourceId: 14
    //                    startingPRB: 51
    //                    format: format2 (2)
    //                        format2
    //                            nrofPRBs: 1
    //                            nrofSymbols: 2
    //                            startingSymbolIndex: 12
    pucch.sets[1].resources[6].format           = SRSRAN_PUCCH_NR_FORMAT_2;
    pucch.sets[1].resources[6].starting_prb     = 51;
    pucch.sets[1].resources[6].nof_prb          = 1;
    pucch.sets[1].resources[6].nof_symbols      = 2;
    pucch.sets[1].resources[6].start_symbol_idx = 12;

    //            Item 15
    //                PUCCH-Resource
    //                    pucch-ResourceId: 15
    //                    startingPRB: 1
    //                    format: format2 (2)
    //                        format2
    //                            nrofPRBs: 1
    //                            nrofSymbols: 2
    //                            startingSymbolIndex: 0
    pucch.sets[1].resources[7].format           = SRSRAN_PUCCH_NR_FORMAT_2;
    pucch.sets[1].resources[7].starting_prb     = 51;
    pucch.sets[1].resources[7].nof_prb          = 1;
    pucch.sets[1].resources[7].nof_symbols      = 2;
    pucch.sets[1].resources[7].start_symbol_idx = 2;

    //            Item 16
    //                PUCCH-Resource
    //                    pucch-ResourceId: 16
    //                    startingPRB: 0
    //                    format: format1 (1)
    //                        format1
    //                            initialCyclicShift: 8
    //                            nrofSymbols: 14
    //                            startingSymbolIndex: 0
    //                            timeDomainOCC: 2
    pucch.sr_resources[1].resource.format               = SRSRAN_PUCCH_NR_FORMAT_1;
    pucch.sr_resources[1].resource.starting_prb         = 0;
    pucch.sr_resources[1].resource.initial_cyclic_shift = 8;
    pucch.sr_resources[1].resource.nof_symbols          = 14;
    pucch.sr_resources[1].resource.start_symbol_idx     = 0;
    pucch.sr_resources[1].resource.time_domain_occ      = 2;

    //            Item 17
    //                PUCCH-Resource
    //                    pucch-ResourceId: 17
    //                    startingPRB: 1
    //                    format: format2 (2)
    //                        format2
    //                            nrofPRBs: 1
    //                            nrofSymbols: 2
    //                            startingSymbolIndex: 2
    srsran_pucch_nr_resource_t pucch_res_17 = {};
    pucch_res_17.starting_prb               = 1;
    pucch_res_17.format                     = SRSRAN_PUCCH_NR_FORMAT_2;
    pucch_res_17.nof_prb                    = 1;
    pucch_res_17.nof_symbols                = 2;
    pucch_res_17.start_symbol_idx           = 2;

    //        format1: setup (1)
    //            setup
    //        format2: setup (1)
    //            setup
    //                maxCodeRate: zeroDot25 (2)
    for (uint32_t i = 0; i < SRSRAN_PUCCH_NR_MAX_NOF_SETS; i++) {
      srsran_pucch_nr_resource_set_t* set = &pucch.sets[i];
      for (uint32_t j = 0; j < set->nof_resources; j++) {
        if (set->resources[j].format == SRSRAN_PUCCH_NR_FORMAT_2) {
          set->resources[j].max_code_rate = 2; // 0.25
        }
      }
    }
    pucch_res_17.max_code_rate = 2;

    //        schedulingRequestResourceToAddModList: 1 item
    //            Item 0
    //                SchedulingRequestResourceConfig
    //                    schedulingRequestResourceId: 1
    //                    schedulingRequestID: 0
    //                    periodicityAndOffset: sl40 (10)
    //                        sl40: 8
    //                    resource: 16
    pucch.sr_resources[1].sr_id      = 0;
    pucch.sr_resources[1].period     = 40;
    pucch.sr_resources[1].offset     = 8;
    pucch.sr_resources[1].configured = true;

    //        dl-DataToUL-ACK: 7 items
    //            Item 0
    //                dl-DataToUL-ACK item: 8
    //            Item 1
    //                dl-DataToUL-ACK item: 7
    //            Item 2
    //                dl-DataToUL-ACK item: 6
    //            Item 3
    //                dl-DataToUL-ACK item: 5
    //            Item 4
    //                dl-DataToUL-ACK item: 4
    //            Item 5
    //                dl-DataToUL-ACK item: 12
    //            Item 6
    //                dl-DataToUL-ACK item: 11
    harq_ack.dl_data_to_ul_ack[0]  = 8;
    harq_ack.dl_data_to_ul_ack[1]  = 7;
    harq_ack.dl_data_to_ul_ack[2]  = 6;
    harq_ack.dl_data_to_ul_ack[3]  = 5;
    harq_ack.dl_data_to_ul_ack[4]  = 4;
    harq_ack.dl_data_to_ul_ack[5]  = 12;
    harq_ack.dl_data_to_ul_ack[6]  = 11;
    harq_ack.nof_dl_data_to_ul_ack = 7;

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
    srsran_csi_rs_nzp_resource_t nzp_resource_0                = {};
    nzp_resource_0.resource_mapping.row                        = srsran_csi_rs_resource_mapping_row_2;
    nzp_resource_0.resource_mapping.frequency_domain_alloc[0]  = true;
    nzp_resource_0.resource_mapping.frequency_domain_alloc[1]  = false;
    nzp_resource_0.resource_mapping.frequency_domain_alloc[2]  = false;
    nzp_resource_0.resource_mapping.frequency_domain_alloc[3]  = false;
    nzp_resource_0.resource_mapping.frequency_domain_alloc[4]  = false;
    nzp_resource_0.resource_mapping.frequency_domain_alloc[5]  = false;
    nzp_resource_0.resource_mapping.frequency_domain_alloc[6]  = false;
    nzp_resource_0.resource_mapping.frequency_domain_alloc[7]  = false;
    nzp_resource_0.resource_mapping.frequency_domain_alloc[8]  = false;
    nzp_resource_0.resource_mapping.frequency_domain_alloc[9]  = false;
    nzp_resource_0.resource_mapping.frequency_domain_alloc[10] = false;
    nzp_resource_0.resource_mapping.frequency_domain_alloc[11] = false;
    nzp_resource_0.resource_mapping.nof_ports                  = 1;
    nzp_resource_0.resource_mapping.first_symbol_idx           = 4;
    nzp_resource_0.resource_mapping.cdm                        = srsran_csi_rs_cdm_nocdm;
    nzp_resource_0.resource_mapping.density                    = srsran_csi_rs_resource_mapping_density_one;
    nzp_resource_0.resource_mapping.freq_band.start_rb         = 0;
    nzp_resource_0.resource_mapping.freq_band.nof_rb           = 52;
    nzp_resource_0.power_control_offset                        = 0;
    nzp_resource_0.power_control_offset_ss                     = 0;
    nzp_resource_0.scrambling_id                               = 0;
    nzp_resource_0.periodicity.period                          = 80;
    nzp_resource_0.periodicity.offset                          = 1;

    //    Item 1
    //        NZP-CSI-RS-Resource
    //            nzp-CSI-RS-ResourceId: 1
    //            resourceMapping
    //                frequencyDomainAllocation: row1 (0)
    //                    row1: 10 [bit length 4, 4 LSB pad bits, 0001 .... decimal value 1]
    //                nrofPorts: p1 (0)
    //                firstOFDMSymbolInTimeDomain: 4
    //                cdm-Type: noCDM (0)
    //                density: three (2)
    //                    three: NULL
    //                freqBand
    //                    startingRB: 0
    //                    nrofRBs: 52
    //            powerControlOffset: 0dB
    //            powerControlOffsetSS: db0 (1)
    //            scramblingID: 0
    //            periodicityAndOffset: slots40 (7)
    //                slots40: 11
    //            qcl-InfoPeriodicCSI-RS: 0
    srsran_csi_rs_nzp_resource_t nzp_resource_1               = {};
    nzp_resource_1.resource_mapping.row                       = srsran_csi_rs_resource_mapping_row_1;
    nzp_resource_1.resource_mapping.frequency_domain_alloc[0] = false;
    nzp_resource_1.resource_mapping.frequency_domain_alloc[1] = false;
    nzp_resource_1.resource_mapping.frequency_domain_alloc[2] = false;
    nzp_resource_1.resource_mapping.frequency_domain_alloc[3] = true;
    nzp_resource_1.resource_mapping.nof_ports                 = 1;
    nzp_resource_1.resource_mapping.first_symbol_idx          = 4;
    nzp_resource_1.resource_mapping.cdm                       = srsran_csi_rs_cdm_nocdm;
    nzp_resource_1.resource_mapping.density                   = srsran_csi_rs_resource_mapping_density_three;
    nzp_resource_1.resource_mapping.freq_band.start_rb        = 0;
    nzp_resource_1.resource_mapping.freq_band.nof_rb          = 52;
    nzp_resource_1.power_control_offset                       = 0;
    nzp_resource_1.power_control_offset_ss                    = 0;
    nzp_resource_1.scrambling_id                              = 0;
    nzp_resource_1.periodicity.period                         = 40;
    nzp_resource_1.periodicity.offset                         = 11;
    //    Item 2
    //        NZP-CSI-RS-Resource
    //            nzp-CSI-RS-ResourceId: 2
    //            resourceMapping
    //                frequencyDomainAllocation: row1 (0)
    //                    row1: 10 [bit length 4, 4 LSB pad bits, 0001 .... decimal value 1]
    //                nrofPorts: p1 (0)
    //                firstOFDMSymbolInTimeDomain: 8
    //                cdm-Type: noCDM (0)
    //                density: three (2)
    //                    three: NULL
    //                freqBand
    //                    startingRB: 0
    //                    nrofRBs: 52
    //            powerControlOffset: 0dB
    //            powerControlOffsetSS: db0 (1)
    //            scramblingID: 0
    //            periodicityAndOffset: slots40 (7)
    //                slots40: 11
    //            qcl-InfoPeriodicCSI-RS: 0
    srsran_csi_rs_nzp_resource_t nzp_resource_2               = {};
    nzp_resource_2.resource_mapping.row                       = srsran_csi_rs_resource_mapping_row_1;
    nzp_resource_2.resource_mapping.frequency_domain_alloc[0] = false;
    nzp_resource_2.resource_mapping.frequency_domain_alloc[1] = false;
    nzp_resource_2.resource_mapping.frequency_domain_alloc[2] = false;
    nzp_resource_2.resource_mapping.frequency_domain_alloc[3] = true;
    nzp_resource_2.resource_mapping.nof_ports                 = 1;
    nzp_resource_2.resource_mapping.first_symbol_idx          = 8;
    nzp_resource_2.resource_mapping.cdm                       = srsran_csi_rs_cdm_nocdm;
    nzp_resource_2.resource_mapping.density                   = srsran_csi_rs_resource_mapping_density_three;
    nzp_resource_2.resource_mapping.freq_band.start_rb        = 0;
    nzp_resource_2.resource_mapping.freq_band.nof_rb          = 52;
    nzp_resource_2.power_control_offset                       = 0;
    nzp_resource_2.power_control_offset_ss                    = 0;
    nzp_resource_2.scrambling_id                              = 0;
    nzp_resource_2.periodicity.period                         = 40;
    nzp_resource_2.periodicity.offset                         = 11;
    //    Item 3
    //        NZP-CSI-RS-Resource
    //            nzp-CSI-RS-ResourceId: 3
    //            resourceMapping
    //                frequencyDomainAllocation: row1 (0)
    //                    row1: 10 [bit length 4, 4 LSB pad bits, 0001 .... decimal value 1]
    //                nrofPorts: p1 (0)
    //                firstOFDMSymbolInTimeDomain: 4
    //                cdm-Type: noCDM (0)
    //                density: three (2)
    //                    three: NULL
    //                freqBand
    //                    startingRB: 0
    //                    nrofRBs: 52
    //            powerControlOffset: 0dB
    //            powerControlOffsetSS: db0 (1)
    //            scramblingID: 0
    //            periodicityAndOffset: slots40 (7)
    //                slots40: 12
    //            qcl-InfoPeriodicCSI-RS: 0
    srsran_csi_rs_nzp_resource_t nzp_resource_3               = {};
    nzp_resource_3.resource_mapping.row                       = srsran_csi_rs_resource_mapping_row_1;
    nzp_resource_3.resource_mapping.frequency_domain_alloc[0] = false;
    nzp_resource_3.resource_mapping.frequency_domain_alloc[1] = false;
    nzp_resource_3.resource_mapping.frequency_domain_alloc[2] = false;
    nzp_resource_3.resource_mapping.frequency_domain_alloc[3] = true;
    nzp_resource_3.resource_mapping.nof_ports                 = 1;
    nzp_resource_3.resource_mapping.first_symbol_idx          = 4;
    nzp_resource_3.resource_mapping.cdm                       = srsran_csi_rs_cdm_nocdm;
    nzp_resource_3.resource_mapping.density                   = srsran_csi_rs_resource_mapping_density_three;
    nzp_resource_3.resource_mapping.freq_band.start_rb        = 0;
    nzp_resource_3.resource_mapping.freq_band.nof_rb          = 52;
    nzp_resource_3.power_control_offset                       = 0;
    nzp_resource_3.power_control_offset_ss                    = 0;
    nzp_resource_3.scrambling_id                              = 0;
    nzp_resource_3.periodicity.period                         = 40;
    nzp_resource_3.periodicity.offset                         = 12;
    //    Item 4
    //        NZP-CSI-RS-Resource
    //            nzp-CSI-RS-ResourceId: 4
    //            resourceMapping
    //                frequencyDomainAllocation: row1 (0)
    //                    row1: 10 [bit length 4, 4 LSB pad bits, 0001 .... decimal value 1]
    //                nrofPorts: p1 (0)
    //                firstOFDMSymbolInTimeDomain: 8
    //                cdm-Type: noCDM (0)
    //                density: three (2)
    //                    three: NULL
    //                freqBand
    //                    startingRB: 0
    //                    nrofRBs: 52
    //            powerControlOffset: 0dB
    //            powerControlOffsetSS: db0 (1)
    //            scramblingID: 0
    //            periodicityAndOffset: slots40 (7)
    //                slots40: 12
    //            qcl-InfoPeriodicCSI-RS: 0
    srsran_csi_rs_nzp_resource_t nzp_resource_4               = {};
    nzp_resource_4.resource_mapping.row                       = srsran_csi_rs_resource_mapping_row_1;
    nzp_resource_4.resource_mapping.frequency_domain_alloc[0] = false;
    nzp_resource_4.resource_mapping.frequency_domain_alloc[1] = false;
    nzp_resource_4.resource_mapping.frequency_domain_alloc[2] = false;
    nzp_resource_4.resource_mapping.frequency_domain_alloc[3] = true;
    nzp_resource_4.resource_mapping.nof_ports                 = 1;
    nzp_resource_4.resource_mapping.first_symbol_idx          = 8;
    nzp_resource_4.resource_mapping.cdm                       = srsran_csi_rs_cdm_nocdm;
    nzp_resource_4.resource_mapping.density                   = srsran_csi_rs_resource_mapping_density_three;
    nzp_resource_4.resource_mapping.freq_band.start_rb        = 0;
    nzp_resource_4.resource_mapping.freq_band.nof_rb          = 52;
    nzp_resource_4.power_control_offset                       = 0;
    nzp_resource_4.power_control_offset_ss                    = 0;
    nzp_resource_4.scrambling_id                              = 0;
    nzp_resource_4.periodicity.period                         = 40;
    nzp_resource_4.periodicity.offset                         = 12;
    // zp-CSI-RS-ResourceSetToAddModList: 2 items
    //    Item 0
    //        NZP-CSI-RS-ResourceSet
    //            nzp-CSI-ResourceSetId: 0
    //            nzp-CSI-RS-Resources: 1 item
    //                Item 0
    //                    NZP-CSI-RS-ResourceId: 0
    pdsch.nzp_csi_rs_sets[0].data[0]  = nzp_resource_0;
    pdsch.nzp_csi_rs_sets[0].count    = 1;
    pdsch.nzp_csi_rs_sets[0].trs_info = false;
    //    Item 1
    //        NZP-CSI-RS-ResourceSet
    //            nzp-CSI-ResourceSetId: 1
    //            nzp-CSI-RS-Resources: 4 items
    //                Item 0
    //                    NZP-CSI-RS-ResourceId: 1
    //                Item 1
    //                    NZP-CSI-RS-ResourceId: 2
    //                Item 2
    //                    NZP-CSI-RS-ResourceId: 3
    //                Item 3
    //                    NZP-CSI-RS-ResourceId: 4
    //            trs-Info: true (0)
    pdsch.nzp_csi_rs_sets[1].data[0]  = nzp_resource_1;
    pdsch.nzp_csi_rs_sets[1].data[1]  = nzp_resource_2;
    pdsch.nzp_csi_rs_sets[1].data[2]  = nzp_resource_3;
    pdsch.nzp_csi_rs_sets[1].data[3]  = nzp_resource_4;
    pdsch.nzp_csi_rs_sets[1].count    = 4;
    pdsch.nzp_csi_rs_sets[1].trs_info = true;
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
    csi.reports[0].type                = SRSRAN_CSI_REPORT_TYPE_PERIODIC;
    csi.reports[0].channel_meas_id     = 0;
    csi.reports[0].interf_meas_present = true;
    csi.reports[0].interf_meas_id      = 1;
    csi.reports[0].periodic.period     = 80;
    csi.reports[0].periodic.offset     = 9;
    csi.reports[0].periodic.resource   = pucch_res_17;
    csi.reports[0].quantity            = SRSRAN_CSI_REPORT_QUANTITY_CRI_RI_PMI_CQI;
    csi.reports[0].freq_cfg            = SRSRAN_CSI_REPORT_FREQ_WIDEBAND;
    csi.reports[0].cqi_table           = SRSRAN_CSI_CQI_TABLE_2;
  }
};
} // namespace srsran

#endif // SRSRAN_RRC_NR_INTERFACE_TYPES_H
