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
  srslte_sch_hl_cfg_nr_t      pdsch;
  srslte_sch_hl_cfg_nr_t      pusch;
  srslte_prach_cfg_t          prach;
  srslte_ue_dl_nr_pdcch_cfg_t pdcch;
} phy_nr_cfg_t;

class state
{
private:
  struct pending_grant_t {
    bool                enable;
    uint32_t            pid;
    srslte_sch_cfg_nr_t sch_cfg;
  };
  srslte::circular_array<pending_grant_t, TTIMOD_SZ> pending_ul_grant = {};
  mutable std::mutex                                 pending_ul_grant_mutex;

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
    pending_grant_t& pending_grant = pending_ul_grant[tti_tx];
    pending_grant.sch_cfg          = pusch_cfg;
    pending_grant.pid              = dci_ul.pid;
    pending_grant.enable           = true;
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
    pending_grant_t& pending_grant = pending_ul_grant[tti_tx];

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
};
} // namespace nr
} // namespace srsue

#endif // SRSLTE_STATE_H
