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

/******************************************************************************
 *  File:         pdsch_cfg_nr.h
 *
 *  Description:  Physical downlink shared channel configuration
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_PDSCH_CFG_NR_H
#define SRSLTE_PDSCH_CFG_NR_H

#include "srslte/phy/common/phy_common_nr.h"
#include "srslte/phy/fec/cbsegm.h"
#include "srslte/phy/fec/softbuffer.h"
#include "srslte/phy/phch/ra.h"

/**
 * @brief PDSCH DMRS type
 */
typedef enum {
  srslte_dmrs_pdsch_type_1 = 0, // 1 pilot every 2 sub-carriers (default)
  srslte_dmrs_pdsch_type_2      // 2 consecutive pilots every 6 sub-carriers
} srslte_dmrs_pdsch_type_t;

/**
 * @brief PDSCH DMRS length in symbols
 */
typedef enum {
  srslte_dmrs_pdsch_len_1 = 0, // single, 1 symbol long (default)
  srslte_dmrs_pdsch_len_2      // double, 2 symbol long
} srslte_dmrs_pdsch_len_t;

/**
 * @brief Determines whether the first pilot goes into symbol index 2 or 3
 */
typedef enum {
  srslte_dmrs_pdsch_typeA_pos_2 = 0, // Start in slot symbol index 2 (default)
  srslte_dmrs_pdsch_typeA_pos_3      // Start in slot symbol index 3
} srslte_dmrs_pdsch_typeA_pos_t;

/**
 * @brief Determines additional symbols if possible to be added
 */
typedef enum {
  srslte_dmrs_pdsch_add_pos_2 = 0,
  srslte_dmrs_pdsch_add_pos_0,
  srslte_dmrs_pdsch_add_pos_1,
  srslte_dmrs_pdsch_add_pos_3
} srslte_dmrs_pdsch_add_pos_t;

/**
 * @brief Provides PDSCH DMRS configuration from higher layers
 * @remark Parameters described in TS 38.331 V15.10.0
 */
typedef struct {
  /// Parameters provided by IE DMRS-DownlinkConfig
  srslte_dmrs_pdsch_type_t      type;
  srslte_dmrs_pdsch_add_pos_t   additional_pos;
  srslte_dmrs_pdsch_len_t       length;
  srslte_dmrs_pdsch_typeA_pos_t typeA_pos;
  bool                          scrambling_id0_present;
  uint32_t                      scrambling_id0;
  bool                          scrambling_id1_present;
  uint32_t                      scrambling_id1;

  /// Parameters provided by ServingCellConfig
  bool lte_CRS_to_match_around;

  /// Parameters provided by FeatureSetDownlink-v1540
  bool additional_DMRS_DL_Alt;

} srslte_pdsch_dmrs_cfg_t;

/**
 * @brief flatten PDSCH time domain allocation parameters
 * @remark Described in TS 38.331 V15.10.0 Section PDSCH-TimeDomainResourceAllocationList
 */
typedef struct SRSLTE_API {
  /// Slot offset between DCI and its scheduled PDSCH
  uint32_t k0;

  /// PDSCH mapping type
  srslte_pdsch_mapping_type_t mapping_type;

  /// An index giving valid combinations of start symbol and length (jointly encoded) as start and length indicator
  /// (SLIV). The network configures the field so that the allocation does not cross the slot boundary
  uint32_t sliv;

} srslte_pdsch_allocation_t;

typedef struct SRSLTE_API {
  /// UE identifier
  uint16_t rnti;

  /// Time domain resources
  uint32_t                    k0;
  uint32_t                    S;
  uint32_t                    L;
  srslte_pdsch_mapping_type_t mapping;

  /// Frequency domain resources
  bool prb_idx[SRSLTE_MAX_PRB_NR];

  /// DMRS Scrambling sequence initialization (false: 0 or true: 1)
  bool n_scid;

  /// Transport block
  /// ....
} srslte_pdsch_grant_nr_t;

typedef struct SRSLTE_API {
  srslte_pdsch_grant_nr_t grant;
  srslte_pdsch_dmrs_cfg_t dmrs_cfg;
} srslte_pdsch_cfg_nr_t;

#endif // SRSLTE_PDSCH_CFG_NR_H
