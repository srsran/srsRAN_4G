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

#ifndef SRSLTE_PHY_COMMON_NR_H
#define SRSLTE_PHY_COMMON_NR_H

#include "phy_common.h"
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defines the number of symbols per slot. Defined by TS 38.211 v15.8.0 Table 4.3.2-1.
 */
#define SRSLTE_NR_NSYMB_PER_SLOT 14

/**
 * @brief Defines the resource grid size in physical resource elements (frequency and time domain)
 */
#define SRSLTE_NR_SLOT_LEN_RE(nof_prb) (nof_prb * SRSLTE_NRE * SRSLTE_NR_NSYMB_PER_SLOT)

/**
 * @brief Defines the maximum numerology supported. Defined by TS 38.211 v15.8.0 Table 4.3.2-1.
 */
#define SRSLTE_NR_MAX_NUMEROLOGY 4

/**
 * @brief Defines the symbol duration, including cyclic prefix
 */
#define SRSLTE_SUBC_SPACING(NUM) (15000U << (NUM))

/**
 * @brief Defines the number of slots per SF. Defined by TS 38.211 v15.8.0 Table 4.3.2-1.
 */
#define SRSLTE_NR_NSLOTS_PER_SF(NUM) (1U << (NUM))

/**
 * @brief Defines the number of slots per frame. Defined by TS 38.211 v15.8.0 Table 4.3.2-1.
 */
#define SRSLTE_NR_NSLOTS_PER_FRAME(NUM) (SRSLTE_NR_NSLOTS_PER_SF(NUM) * SRSLTE_NOF_SF_X_FRAME)

/**
 * @brief Maximum Carrier identification value. Defined by TS 38.331 v15.10.0 as PhysCellId from 0 to 1007.
 */
#define SRSLTE_NR_MAX_ID 1007

/**
 * @brief Maximum number of physical resource blocks (PRB) that a 5G NR can have. This is defined by TS 38.331 v15.10.0
 * as maxNrofPhysicalResourceBlocks
 */
#define SRSLTE_NR_MAX_PRB 275

#define SRSLTE_NR_MAX_START 2199

/**
 * Common carrier parameters
 */
typedef struct {
  uint32_t id;
  uint32_t numerology;
  uint32_t nof_prb;
  uint32_t start;
} srslte_nr_carrier_t;

/**
 * CORESET related constants
 */
#define SRSLTE_CORESET_DURATION_MIN 1
#define SRSLTE_CORESET_DURATION_MAX 3
#define SRSLTE_CORESET_FREQ_DOMAIN_RES_SIZE 45
#define SRSLTE_CORESET_SHIFT_INDEX_MAX (SRSLTE_CORESET_NOF_PRB_MAX - 1)

typedef enum SRSLTE_API {
  srslte_coreset_mapping_type_interleaved = 0,
  srslte_coreset_mapping_type_non_interleaved,
} srslte_coreset_mapping_type_t;

typedef enum SRSLTE_API {
  srslte_coreset_bundle_size_n2 = 0,
  srslte_coreset_bundle_size_n3,
  srslte_coreset_bundle_size_n6,
} srslte_coreset_bundle_size_t;

typedef enum SRSLTE_API {
  srslte_coreset_precoder_granularity_contiguous = 0,
  srslte_coreset_precoder_granularity_reg_bundle
} srslte_coreset_precoder_granularity_t;

/**
 * CORESET structure
 *
 * Fields follow the same order than described in 3GPP 38.331 R15 - ControlResourceSet
 *
 */
typedef struct SRSLTE_API {
  srslte_coreset_mapping_type_t mapping_type;
  uint32_t                      id;
  uint32_t                      duration;
  bool                          freq_resources[SRSLTE_CORESET_FREQ_DOMAIN_RES_SIZE];
  srslte_coreset_bundle_size_t  interleaver_size;

  bool                                  dmrs_scrambling_id_present;
  uint32_t                              dmrs_scrambling_id;
  srslte_coreset_precoder_granularity_t precoder_granularity;
  srslte_coreset_bundle_size_t          reg_bundle_size;
  uint32_t                              shift_index;
  /** Missing TCI parameters */
} srslte_coreset_t;

typedef enum SRSLTE_API {
  srslte_search_space_type_common = 0,
  srslte_search_space_type_ue,
} srslte_search_space_type_t;

#define SRSLTE_SEARCH_SPACE_NOF_AGGREGATION_LEVELS 5

typedef struct SRSLTE_API {
  uint32_t                   start;    // start symbol within slot
  uint32_t                   duration; // in slots
  srslte_search_space_type_t type;
  uint32_t                   nof_candidates[SRSLTE_SEARCH_SPACE_NOF_AGGREGATION_LEVELS];
} srslte_search_space_t;

typedef struct SRSLTE_API {
  srslte_nr_carrier_t   carrier;
  uint16_t              rnti;
  srslte_coreset_t      coreset;
  srslte_search_space_t search_space;
  uint32_t              candidate;
  uint32_t              aggregation_level;
} srslte_nr_pdcch_cfg_t;

/**
 * @brief Calculates the bandwidth of a given CORESET in physical resource blocks (PRB) . This function uses the
 * frequency domain resources bit-map for counting the number of PRB.
 *
 * @attention Prior to this function call, the frequency domain resources bit-map shall be zeroed beyond the
 * carrier.nof_prb / 6 index, otherwise the CORESET bandwidth might be greater than the carrier.
 *
 * @param coreset provides the given CORESET configuration
 * @return The number of PRB that the CORESET takes in frequency domain
 */
SRSLTE_API uint32_t srslte_coreset_get_bw(const srslte_coreset_t* coreset);

/**
 * @brief Calculates the number of Physical Resource Elements (time and frequency domain) that a given CORESET uses.
 * This function uses the frequency domain resources bit-map and duration to compute the number of symbols.
 *
 * @attention Prior to this function call, the frequency domain resources bit-map shall be zeroed beyond the
 * carrier.nof_prb / 6 index, otherwise the CORESET bandwidth might be greater than the carrier.
 *
 * @param coreset provides the given CORESET configuration
 * @return The number of resource elements that compose the coreset
 */
SRSLTE_API uint32_t srslte_coreset_get_sz(const srslte_coreset_t* coreset);

#ifdef __cplusplus
}
#endif

#endif // SRSLTE_PHY_COMMON_NR_H
