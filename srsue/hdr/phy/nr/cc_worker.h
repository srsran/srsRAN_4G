/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSLTE_NR_CC_WORKER_H
#define SRSLTE_NR_CC_WORKER_H

#include "srslte/common/log.h"
#include "srslte/srslte.h"
#include "srsue/hdr/phy/phy_common.h"
#include <array>
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

class phy_nr_state
{
public:
  srslte_carrier_nr_t carrier   = {};
  phy_nr_args_t       args      = {};
  phy_nr_cfg_t        cfg       = {};
  int32_t             test_rnti = 0x1234; // Fix PDSCH RNTI for testing

  phy_nr_state()
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

class cc_worker
{
public:
  cc_worker(uint32_t cc_idx, srslog::basic_logger& log, phy_nr_state* phy_state_);
  ~cc_worker();

  bool set_carrier(const srslte_carrier_nr_t* carrier);
  void set_tti(uint32_t tti);

  cf_t*    get_rx_buffer(uint32_t antenna_idx);
  uint32_t get_buffer_len();

  bool work_dl();

  int read_pdsch_d(cf_t* pdsch_d);

private:
  srslte_dl_slot_cfg_t                dl_slot_cfg = {};
  uint32_t                            cc_idx      = 0;
  std::array<cf_t*, SRSLTE_MAX_PORTS> rx_buffer   = {};
  std::array<cf_t*, SRSLTE_MAX_PORTS> tx_buffer   = {};
  uint32_t                            buffer_sz   = 0;
  phy_nr_state*                       phy_state   = nullptr;
  srslte_ue_dl_nr_t                   ue_dl       = {};
  srslog::basic_logger&               logger;

  // Temporal attributes
  srslte_softbuffer_rx_t softbuffer_rx = {};
  std::vector<uint8_t>   data;
};

} // namespace nr
} // namespace srsue

#endif // SRSLTE_NR_CC_WORKER_H
