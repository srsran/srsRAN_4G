/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSLTE_STATE_H
#define SRSLTE_STATE_H

#include "srslte/adt/circular_array.h"
#include "srslte/common/common.h"
#include "srslte/interfaces/ue_nr_interfaces.h"
#include "srslte/srslte.h"
#include <array>
#include <mutex>
#include <vector>

namespace srsue {
namespace nr {

typedef struct {
  uint32_t               nof_carriers;
  srslte_ue_dl_nr_args_t dl;
  srslte_ue_ul_nr_args_t ul;
} phy_nr_args_t;

typedef struct {
  srslte_sch_hl_cfg_nr_t         pdsch;
  srslte_sch_hl_cfg_nr_t         pusch;
  srslte_pucch_nr_hl_cfg_t       pucch;
  srslte_prach_cfg_t             prach;
  srslte_ue_dl_nr_pdcch_cfg_t    pdcch;
  srslte_ue_dl_nr_harq_ack_cfg_t harq_ack;
} phy_nr_cfg_t;

class state
{
private:
  struct pending_ul_grant_t {
    bool                enable;
    uint32_t            pid;
    srslte_sch_cfg_nr_t sch_cfg;
  };
  srslte::circular_array<pending_ul_grant_t, TTIMOD_SZ> pending_ul_grant = {};
  mutable std::mutex                                    pending_ul_grant_mutex;

  struct pending_dl_grant_t {
    bool                           enable;
    uint32_t                       pid;
    srslte_sch_cfg_nr_t            sch_cfg;
    srslte_pdsch_ack_resource_nr_t ack_resource;
  };
  srslte::circular_array<pending_dl_grant_t, TTIMOD_SZ> pending_dl_grant = {};
  mutable std::mutex                                    pending_dl_grant_mutex;

  srslte::circular_array<srslte_pdsch_ack_nr_t, TTIMOD_SZ> pending_ack = {};
  mutable std::mutex                                       pending_ack_mutex;

public:
  mac_interface_phy_nr* stack     = nullptr;
  srslte_carrier_nr_t   carrier   = {};
  phy_nr_args_t         args      = {};
  phy_nr_cfg_t          cfg       = {};
  int32_t               test_rnti = 0x1234; // Fix PDSCH RNTI for testing

  state()
  {
    carrier.id              = 500;
    carrier.nof_prb         = 100;
    carrier.max_mimo_layers = 1;

    // Default arguments
    args.nof_carriers              = 1;
    args.dl.nof_rx_antennas        = 1;
    args.dl.nof_max_prb            = 100;
    args.dl.pdsch.measure_evm      = true;
    args.dl.pdsch.measure_time     = true;
    args.dl.pdsch.sch.disable_simd = false;
    args.ul.nof_max_prb            = 100;
    args.ul.pusch.measure_time     = true;
    args.ul.pusch.sch.disable_simd = false;

    // Default PDSCH configuration
    cfg.pdsch.sch_cfg.mcs_table = srslte_mcs_table_256qam;

    // Default PRACH configuration
    cfg.prach.is_nr            = true;
    cfg.prach.config_idx       = 16;
    cfg.prach.root_seq_idx     = 1;
    cfg.prach.freq_offset      = 0;
    cfg.prach.zero_corr_zone   = 0;
    cfg.prach.num_ra_preambles = 64;
    cfg.prach.hs_flag          = false;

    // physicalCellGroupConfig
    //    pdsch-HARQ-ACK-Codebook: dynamic (1)
    cfg.harq_ack.pdsch_harq_ack_codebook = srslte_pdsch_harq_ack_codebook_dynamic;

    // commonControlResourceSet
    //    controlResourceSetId: 1
    //    frequencyDomainResources: ff0000000000
    //    duration: 1
    //    cce-REG-MappingType: nonInterleaved (1)
    //        nonInterleaved: NULL
    //    precoderGranularity: sameAsREG-bundle (0)
    cfg.pdcch.coreset[1].coreset_id           = 1;
    cfg.pdcch.coreset[1].precoder_granularity = srslte_coreset_precoder_granularity_reg_bundle;
    cfg.pdcch.coreset[1].duration             = 1;
    cfg.pdcch.coreset[1].mapping_type         = srslte_coreset_mapping_type_non_interleaved;
    for (uint32_t i = 0; i < SRSLTE_CORESET_FREQ_DOMAIN_RES_SIZE; i++) {
      cfg.pdcch.coreset[1].freq_resources[i] = (i < 8);
    }
    cfg.pdcch.coreset_present[1] = true;

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
    srslte_search_space_t search_space1 = {};
    search_space1.id                    = 1;
    search_space1.coreset_id            = 1;
    search_space1.nof_candidates[0]     = 0;
    search_space1.nof_candidates[1]     = 0;
    search_space1.nof_candidates[2]     = 1;
    search_space1.nof_candidates[3]     = 0;
    search_space1.nof_candidates[4]     = 0;
    search_space1.type                  = srslte_search_space_type_common_3;
    cfg.pdcch.search_space[1]           = search_space1;
    cfg.pdcch.search_space_present[1]   = true;

    // ra-SearchSpace: 1
    cfg.pdcch.ra_rnti                 = 0x16; //< Supposed to be deduced from PRACH configuration
    cfg.pdcch.ra_search_space         = search_space1;
    cfg.pdcch.ra_search_space.type    = srslte_search_space_type_common_1;
    cfg.pdcch.ra_search_space_present = true;

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
    cfg.pdsch.common_time_ra[0].mapping_type = srslte_sch_mapping_type_A;
    cfg.pdsch.common_time_ra[0].sliv         = 40;
    cfg.pdsch.common_time_ra[0].k            = 0;
    cfg.pdsch.common_time_ra[1].mapping_type = srslte_sch_mapping_type_A;
    cfg.pdsch.common_time_ra[1].sliv         = 57;
    cfg.pdsch.common_time_ra[1].k            = 0;
    cfg.pdsch.nof_common_time_ra             = 2;

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
    cfg.pusch.common_time_ra[0].mapping_type = srslte_sch_mapping_type_A;
    cfg.pusch.common_time_ra[0].sliv         = 27;
    cfg.pusch.common_time_ra[0].k            = 4;
    cfg.pusch.common_time_ra[1].mapping_type = srslte_sch_mapping_type_A;
    cfg.pusch.common_time_ra[1].sliv         = 27;
    cfg.pusch.common_time_ra[1].k            = 5;
    cfg.pusch.nof_common_time_ra             = 2;

    // pusch-Config: setup (1)
    //    setup
    //        dmrs-UplinkForPUSCH-MappingTypeA: setup (1)
    //            setup
    //                dmrs-AdditionalPosition: pos1 (1)
    //                transformPrecodingDisabled
    cfg.pusch.dmrs_typeA.additional_pos = srslte_dmrs_sch_add_pos_1;
    cfg.pusch.dmrs_typeA.present        = true;
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
    //                        betaOffsetCSI-Part1-Index1: 6
    //                        betaOffsetCSI-Part1-Index2: 6
    //                        betaOffsetCSI-Part2-Index1: 6
    //                        betaOffsetCSI-Part2-Index2: 6
    //                scaling: f1 (3)

    // pucch-Config: setup (1)
    //    setup
    //        resourceSetToAddModList: 2 items
    cfg.pucch.enabled = true;
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
    cfg.pucch.sets[0].nof_resources = 8;

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
    cfg.pucch.sets[1].nof_resources = 8;

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
    cfg.pucch.sets[0].resources[0].format               = SRSLTE_PUCCH_NR_FORMAT_1;
    cfg.pucch.sets[0].resources[0].starting_prb         = 0;
    cfg.pucch.sets[0].resources[0].initial_cyclic_shift = 0;
    cfg.pucch.sets[0].resources[0].nof_symbols          = 14;
    cfg.pucch.sets[0].resources[0].start_symbol_idx     = 0;
    cfg.pucch.sets[0].resources[0].time_domain_occ      = 0;

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
    cfg.pucch.sets[0].resources[1].format               = SRSLTE_PUCCH_NR_FORMAT_1;
    cfg.pucch.sets[0].resources[1].starting_prb         = 0;
    cfg.pucch.sets[0].resources[1].initial_cyclic_shift = 4;
    cfg.pucch.sets[0].resources[1].nof_symbols          = 14;
    cfg.pucch.sets[0].resources[1].start_symbol_idx     = 0;
    cfg.pucch.sets[0].resources[1].time_domain_occ      = 0;

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
    cfg.pucch.sets[0].resources[2].format               = SRSLTE_PUCCH_NR_FORMAT_1;
    cfg.pucch.sets[0].resources[2].starting_prb         = 0;
    cfg.pucch.sets[0].resources[2].initial_cyclic_shift = 8;
    cfg.pucch.sets[0].resources[2].nof_symbols          = 14;
    cfg.pucch.sets[0].resources[2].start_symbol_idx     = 0;
    cfg.pucch.sets[0].resources[2].time_domain_occ      = 0;

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
    cfg.pucch.sets[0].resources[3].format               = SRSLTE_PUCCH_NR_FORMAT_1;
    cfg.pucch.sets[0].resources[3].starting_prb         = 0;
    cfg.pucch.sets[0].resources[3].initial_cyclic_shift = 0;
    cfg.pucch.sets[0].resources[3].nof_symbols          = 14;
    cfg.pucch.sets[0].resources[3].start_symbol_idx     = 0;
    cfg.pucch.sets[0].resources[3].time_domain_occ      = 1;

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
    cfg.pucch.sets[0].resources[4].format               = SRSLTE_PUCCH_NR_FORMAT_1;
    cfg.pucch.sets[0].resources[4].starting_prb         = 0;
    cfg.pucch.sets[0].resources[4].initial_cyclic_shift = 4;
    cfg.pucch.sets[0].resources[4].nof_symbols          = 14;
    cfg.pucch.sets[0].resources[4].start_symbol_idx     = 0;
    cfg.pucch.sets[0].resources[4].time_domain_occ      = 1;

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
    cfg.pucch.sets[0].resources[5].format               = SRSLTE_PUCCH_NR_FORMAT_1;
    cfg.pucch.sets[0].resources[5].starting_prb         = 0;
    cfg.pucch.sets[0].resources[5].initial_cyclic_shift = 8;
    cfg.pucch.sets[0].resources[5].nof_symbols          = 14;
    cfg.pucch.sets[0].resources[5].start_symbol_idx     = 0;
    cfg.pucch.sets[0].resources[5].time_domain_occ      = 1;

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
    cfg.pucch.sets[0].resources[6].format               = SRSLTE_PUCCH_NR_FORMAT_1;
    cfg.pucch.sets[0].resources[6].starting_prb         = 0;
    cfg.pucch.sets[0].resources[6].initial_cyclic_shift = 0;
    cfg.pucch.sets[0].resources[6].nof_symbols          = 14;
    cfg.pucch.sets[0].resources[6].start_symbol_idx     = 0;
    cfg.pucch.sets[0].resources[6].time_domain_occ      = 2;

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
    cfg.pucch.sets[0].resources[7].format               = SRSLTE_PUCCH_NR_FORMAT_1;
    cfg.pucch.sets[0].resources[7].starting_prb         = 0;
    cfg.pucch.sets[0].resources[7].initial_cyclic_shift = 0;
    cfg.pucch.sets[0].resources[7].nof_symbols          = 14;
    cfg.pucch.sets[0].resources[7].start_symbol_idx     = 0;
    cfg.pucch.sets[0].resources[7].time_domain_occ      = 2;

    //            Item 8
    //                PUCCH-Resource
    //                    pucch-ResourceId: 8
    //                    startingPRB: 51
    //                    format: format2 (2)
    //                        format2
    //                            nrofPRBs: 1
    //                            nrofSymbols: 2
    //                            startingSymbolIndex: 0
    cfg.pucch.sets[1].resources[0].format           = SRSLTE_PUCCH_NR_FORMAT_2;
    cfg.pucch.sets[1].resources[0].starting_prb     = 51;
    cfg.pucch.sets[1].resources[0].nof_prb          = 1;
    cfg.pucch.sets[1].resources[0].nof_symbols      = 2;
    cfg.pucch.sets[1].resources[0].start_symbol_idx = 0;

    //            Item 9
    //                PUCCH-Resource
    //                    pucch-ResourceId: 9
    //                    startingPRB: 51
    //                    format: format2 (2)
    //                        format2
    //                            nrofPRBs: 1
    //                            nrofSymbols: 2
    //                            startingSymbolIndex: 2
    cfg.pucch.sets[1].resources[1].format           = SRSLTE_PUCCH_NR_FORMAT_2;
    cfg.pucch.sets[1].resources[1].starting_prb     = 51;
    cfg.pucch.sets[1].resources[1].nof_prb          = 1;
    cfg.pucch.sets[1].resources[1].nof_symbols      = 2;
    cfg.pucch.sets[1].resources[1].start_symbol_idx = 2;

    //            Item 10
    //                PUCCH-Resource
    //                    pucch-ResourceId: 10
    //                    startingPRB: 51
    //                    format: format2 (2)
    //                        format2
    //                            nrofPRBs: 1
    //                            nrofSymbols: 2
    //                            startingSymbolIndex: 4
    cfg.pucch.sets[1].resources[2].format           = SRSLTE_PUCCH_NR_FORMAT_2;
    cfg.pucch.sets[1].resources[2].starting_prb     = 51;
    cfg.pucch.sets[1].resources[2].nof_prb          = 1;
    cfg.pucch.sets[1].resources[2].nof_symbols      = 2;
    cfg.pucch.sets[1].resources[2].start_symbol_idx = 4;

    //            Item 11
    //                PUCCH-Resource
    //                    pucch-ResourceId: 11
    //                    startingPRB: 51
    //                    format: format2 (2)
    //                        format2
    //                            nrofPRBs: 1
    //                            nrofSymbols: 2
    //                            startingSymbolIndex: 6
    cfg.pucch.sets[1].resources[3].format           = SRSLTE_PUCCH_NR_FORMAT_2;
    cfg.pucch.sets[1].resources[3].starting_prb     = 51;
    cfg.pucch.sets[1].resources[3].nof_prb          = 1;
    cfg.pucch.sets[1].resources[3].nof_symbols      = 2;
    cfg.pucch.sets[1].resources[3].start_symbol_idx = 6;

    //            Item 12
    //                PUCCH-Resource
    //                    pucch-ResourceId: 12
    //                    startingPRB: 51
    //                    format: format2 (2)
    //                        format2
    //                            nrofPRBs: 1
    //                            nrofSymbols: 2
    //                            startingSymbolIndex: 8
    cfg.pucch.sets[1].resources[4].format           = SRSLTE_PUCCH_NR_FORMAT_2;
    cfg.pucch.sets[1].resources[4].starting_prb     = 51;
    cfg.pucch.sets[1].resources[4].nof_prb          = 1;
    cfg.pucch.sets[1].resources[4].nof_symbols      = 2;
    cfg.pucch.sets[1].resources[4].start_symbol_idx = 8;

    //            Item 13
    //                PUCCH-Resource
    //                    pucch-ResourceId: 13
    //                    startingPRB: 51
    //                    format: format2 (2)
    //                        format2
    //                            nrofPRBs: 1
    //                            nrofSymbols: 2
    //                            startingSymbolIndex: 10
    cfg.pucch.sets[1].resources[5].format           = SRSLTE_PUCCH_NR_FORMAT_2;
    cfg.pucch.sets[1].resources[5].starting_prb     = 51;
    cfg.pucch.sets[1].resources[5].nof_prb          = 1;
    cfg.pucch.sets[1].resources[5].nof_symbols      = 2;
    cfg.pucch.sets[1].resources[5].start_symbol_idx = 10;

    //            Item 14
    //                PUCCH-Resource
    //                    pucch-ResourceId: 14
    //                    startingPRB: 51
    //                    format: format2 (2)
    //                        format2
    //                            nrofPRBs: 1
    //                            nrofSymbols: 2
    //                            startingSymbolIndex: 12
    cfg.pucch.sets[1].resources[6].format           = SRSLTE_PUCCH_NR_FORMAT_2;
    cfg.pucch.sets[1].resources[6].starting_prb     = 51;
    cfg.pucch.sets[1].resources[6].nof_prb          = 1;
    cfg.pucch.sets[1].resources[6].nof_symbols      = 2;
    cfg.pucch.sets[1].resources[6].start_symbol_idx = 12;

    //            Item 15
    //                PUCCH-Resource
    //                    pucch-ResourceId: 15
    //                    startingPRB: 1
    //                    format: format2 (2)
    //                        format2
    //                            nrofPRBs: 1
    //                            nrofSymbols: 2
    //                            startingSymbolIndex: 0
    cfg.pucch.sets[1].resources[7].format           = SRSLTE_PUCCH_NR_FORMAT_2;
    cfg.pucch.sets[1].resources[7].starting_prb     = 51;
    cfg.pucch.sets[1].resources[7].nof_prb          = 1;
    cfg.pucch.sets[1].resources[7].nof_symbols      = 2;
    cfg.pucch.sets[1].resources[7].start_symbol_idx = 2;

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
    //            Item 17
    //                PUCCH-Resource
    //                    pucch-ResourceId: 17
    //                    startingPRB: 1
    //                    format: format2 (2)
    //                        format2
    //                            nrofPRBs: 1
    //                            nrofSymbols: 2
    //                            startingSymbolIndex: 2
    //        format1: setup (1)
    //            setup
    //        format2: setup (1)
    //            setup
    //                maxCodeRate: zeroDot25 (2)
    for (uint32_t i = 0; i < SRSLTE_PUCCH_NR_MAX_NOF_SETS; i++) {
      srslte_pucch_nr_resource_set_t* set = &cfg.pucch.sets[i];
      for (uint32_t j = 0; j < set->nof_resources; j++) {
        if (set->resources[j].format == SRSLTE_PUCCH_NR_FORMAT_2) {
          set->resources[j].max_code_rate = 2; // 0.25
        }
      }
    }

    //        schedulingRequestResourceToAddModList: 1 item
    //            Item 0
    //                SchedulingRequestResourceConfig
    //                    schedulingRequestResourceId: 1
    //                    schedulingRequestID: 0
    //                    periodicityAndOffset: sl40 (10)
    //                        sl40: 8
    //                    resource: 16

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
    cfg.harq_ack.dl_data_to_ul_ack[0]  = 8;
    cfg.harq_ack.dl_data_to_ul_ack[1]  = 7;
    cfg.harq_ack.dl_data_to_ul_ack[2]  = 6;
    cfg.harq_ack.dl_data_to_ul_ack[3]  = 5;
    cfg.harq_ack.dl_data_to_ul_ack[4]  = 4;
    cfg.harq_ack.dl_data_to_ul_ack[5]  = 12;
    cfg.harq_ack.dl_data_to_ul_ack[6]  = 11;
    cfg.harq_ack.nof_dl_data_to_ul_ack = 7;
  }

  /**
   * @brief Stores a received UL DCI into the pending UL grant list
   * @param tti_rx The TTI in which the grant was received
   * @param dci_ul The UL DCI message to store
   */
  void set_ul_pending_grant(uint32_t tti_rx, const srslte_dci_ul_nr_t& dci_ul)
  {
    // Convert UL DCI to grant
    srslte_sch_cfg_nr_t pusch_cfg = {};
    if (srslte_ra_ul_dci_to_grant_nr(&carrier, &cfg.pusch, &dci_ul, &pusch_cfg, &pusch_cfg.grant)) {
      ERROR("Computing UL grant");
      return;
    }

    // Calculate Transmit TTI
    uint32_t tti_tx = TTI_ADD(tti_rx, pusch_cfg.grant.k);

    // Scope mutex to protect read/write the list
    std::lock_guard<std::mutex> lock(pending_ul_grant_mutex);

    // Save entry
    pending_ul_grant_t& pending_grant = pending_ul_grant[tti_tx];
    pending_grant.sch_cfg             = pusch_cfg;
    pending_grant.pid                 = dci_ul.pid;
    pending_grant.enable              = true;
  }

  /**
   * @brief Checks the UL pending grant list if there is any grant to transmit for the given transmit TTI
   * @param tti_tx Current transmit TTI
   * @param sch_cfg Provides the Shared Channel configuration for the PUSCH transmission
   * @param pid Provides the HARQ process identifier
   * @return true if there is a pending grant for the given TX tti, false otherwise
   */
  bool get_ul_pending_grant(uint32_t tti_tx, srslte_sch_cfg_nr_t& pusch_cfg, uint32_t& pid)
  {
    // Scope mutex to protect read/write the list
    std::lock_guard<std::mutex> lock(pending_ul_grant_mutex);

    // Select entry
    pending_ul_grant_t& pending_grant = pending_ul_grant[tti_tx];

    // If the entry is not active, just return
    if (!pending_grant.enable) {
      return false;
    }

    // Load shared channel configuration
    pusch_cfg = pending_grant.sch_cfg;

    // Reset entry
    pending_grant.enable = false;

    return true;
  }

  /**
   * @brief Stores a received DL DCI into the pending DL grant list
   * @param tti_rx The TTI in which the grant was received
   * @param dci_dl The DL DCI message to store
   */
  void set_dl_pending_grant(uint32_t tti_rx, const srslte_dci_dl_nr_t& dci_dl)
  {
    // Convert DL DCI to grant
    srslte_sch_cfg_nr_t pdsch_cfg = {};
    if (srslte_ra_dl_dci_to_grant_nr(&carrier, &cfg.pdsch, &dci_dl, &pdsch_cfg, &pdsch_cfg.grant)) {
      ERROR("Computing UL grant");
      return;
    }

    // Calculate DL DCI to PDSCH ACK resource
    srslte_pdsch_ack_resource_nr_t ack_resource = {};
    if (srslte_ue_dl_nr_pdsch_ack_resource(&cfg.harq_ack, &dci_dl, &ack_resource) < SRSLTE_SUCCESS) {
      ERROR("Computing UL ACK resource");
      return;
    }

    // Calculate Receive TTI
    tti_rx = TTI_ADD(tti_rx, pdsch_cfg.grant.k);

    // Scope mutex to protect read/write the list
    std::lock_guard<std::mutex> lock(pending_dl_grant_mutex);

    // Save entry
    pending_dl_grant_t& pending_grant = pending_dl_grant[tti_rx];
    pending_grant.sch_cfg             = pdsch_cfg;
    pending_grant.ack_resource        = ack_resource;
    pending_grant.pid                 = dci_dl.pid;
    pending_grant.enable              = true;
  }

  /**
   * @brief Checks the DL pending grant list if there is any grant to receive for the given receive TTI
   * @param tti_rx Current receive TTI
   * @param sch_cfg Provides the Shared Channel configuration for the PDSCH transmission
   * @param ack_resource Provides the UL ACK resource
   * @param pid Provides the HARQ process identifier
   * @return true if there is a pending grant for the given TX tti, false otherwise
   */
  bool get_dl_pending_grant(uint32_t                        tti_rx,
                            srslte_sch_cfg_nr_t&            pdsch_cfg,
                            srslte_pdsch_ack_resource_nr_t& ack_resource,
                            uint32_t&                       pid)
  {
    // Scope mutex to protect read/write the list
    std::lock_guard<std::mutex> lock(pending_dl_grant_mutex);

    // Select entry
    pending_dl_grant_t& pending_grant = pending_dl_grant[tti_rx];

    // If the entry is not active, just return
    if (!pending_grant.enable) {
      return false;
    }

    // Load shared channel configuration and resource
    pdsch_cfg    = pending_grant.sch_cfg;
    ack_resource = pending_grant.ack_resource;
    pid          = pending_grant.pid;

    // Reset entry
    pending_grant.enable = false;

    return true;
  }

  /**
   * @brief Stores a pending PDSCH ACK into the pending ACK list
   * @param tti_rx The TTI in which the PDSCH transmission was received
   * @param dci_dl The DL DCI message to store
   */
  void set_pending_ack(const uint32_t& tti_rx, const srslte_pdsch_ack_resource_nr_t& ack_resource, const bool& crc_ok)
  {
    // Calculate Receive TTI
    uint32_t tti_tx = TTI_ADD(tti_rx, ack_resource.k1);

    // Scope mutex to protect read/write the list
    std::lock_guard<std::mutex> lock(pending_ack_mutex);

    // Select UL transmission time resource
    srslte_pdsch_ack_nr_t& ack = pending_ack[tti_tx];
    ack.nof_cc                 = 1;

    // Select serving cell
    srslte_pdsch_ack_cc_nr_t& ack_cc = ack.cc[ack_resource.scell_idx];
    srslte_pdsch_ack_m_nr_t&  ack_m  = ack_cc.m[ack_cc.M];
    ack_cc.M++;

    // Set PDSCH transmission information
    ack_m.resource = ack_resource;
    ack_m.value[0] = crc_ok ? 1 : 0;
    ack_m.present  = true;
  }

  bool get_pending_ack(const uint32_t& tti_tx, srslte_pdsch_ack_nr_t& pdsch_ack)
  {
    // Scope mutex to protect read/write the list
    std::lock_guard<std::mutex> lock(pending_ack_mutex);

    // Select UL transmission time resource
    srslte_pdsch_ack_nr_t& ack = pending_ack[tti_tx];

    // No pending grant was set
    if (ack.nof_cc == 0) {
      return false;
    }

    // Copy data
    pdsch_ack = ack;

    // Reset list entry
    ack = {};

    return true;
  }
};
} // namespace nr
} // namespace srsue

#endif // SRSLTE_STATE_H
