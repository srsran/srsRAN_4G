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
#include "srslte/srslte.h"
#include <array>
#include <mutex>
#include <vector>

namespace srsue {
namespace nr {

typedef struct {
  uint32_t               nof_carriers;
  srslte_ue_dl_nr_args_t dl;
} phy_nr_args_t;

typedef struct {
  srslte_pdsch_cfg_nr_t pdsch;
  srslte_prach_cfg_t    prach;
  srslte_ue_dl_nr_cfg_t pdcch;
} phy_nr_cfg_t;

class state
{
public:
  srslte_carrier_nr_t carrier   = {};
  phy_nr_args_t       args      = {};
  phy_nr_cfg_t        cfg       = {};
  int32_t             test_rnti = 0x1234; // Fix PDSCH RNTI for testing

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
    cfg.pdsch.common_pdsch_time_ra[0].mapping_type = srslte_sch_mapping_type_A;
    cfg.pdsch.common_pdsch_time_ra[0].sliv         = 40;
    cfg.pdsch.common_pdsch_time_ra[1].mapping_type = srslte_sch_mapping_type_A;
    cfg.pdsch.common_pdsch_time_ra[1].sliv         = 57;
    cfg.pdsch.nof_common_pdsch_time_ra             = 2;
  }
};
} // namespace nr
} // namespace srsue

#endif // SRSLTE_STATE_H
