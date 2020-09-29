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

#ifndef SRSLTE_DMRS_PDCCH_H
#define SRSLTE_DMRS_PDCCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "srslte/phy/common/phy_common_nr.h"
#include "srslte/phy/resampling/resampler.h"
#include "srslte/srslte.h"

/**
 * @brief Puts in the resource grid the DeModulation Reference Signals for decoding PDCCH.
 *
 * \remark: Implemented as specified by TS 38.211 V15.8.0 Section 7.1.4.3

 * @param cfg Configuration that includes Carrier, CORESET, Search Space and PDCCH candidate
 * @param slot_idx Slot index in the frame
 * @param sf_symbols is the resource grid where the DMRS resource elements will be written
 * @return SRSLTE_SUCCESS if the configurations are valid, otherwise it returns an SRSLTE_ERROR code
 */
SRSLTE_API int srslte_dmrs_pdcch_put(const srslte_nr_pdcch_cfg_t* cfg, uint32_t slot_idx, cf_t* sf_symbols);

/**
 * @brief PDCCH DMRS channel estimator object
 *
 * @see srslte_dmrs_pdcch_estimator_init
 * @see srslte_dmrs_pdcch_estimator_free
 * @see srslte_dmrs_pdcch_estimator_estimate
 */
typedef struct SRSLTE_API {
  /// Current estimator carrier configuration
  srslte_nr_carrier_t carrier;

  /// Current estimator CORESET configuration
  srslte_coreset_t coreset;

  /// Stores the current CORESET bandwidth in PRB
  uint32_t coreset_bw;

  /// Stores the current CORESET size in RE
  uint32_t coreset_sz;

  /// Object for interpolating, it shall be initialised only once
  srslte_resampler_fft_t interpolator;

  /// Pilots least square estimates, one vector for each possible symbol. Since there are one pilot every 4 sub-carriers
  /// , each vector is three times the CORESEt band-width
  cf_t* lse[SRSLTE_CORESET_DURATION_MAX];

  /// Channel estimates, size coreset_sz
  cf_t* ce;

  /// Stores latest slot index in frame
  uint32_t slot_idx;
} srslte_dmrs_pdcch_estimator_t;

/**
 * @brief Initialises the PDCCH DMRS channel estimator from a given carrier and CORESET configuration
 *
 * \attention The initialization function expects the object being previously zeroed
 * \attention Initialization is required every time the carrier and/or CORESET changes. No free is required between
 * initializations.
 *
 * @param q provides PDCCH DMRS estimator object
 * @param carrier provides the required carrier configuration for some estimation
 * @param coreset provides the required configuration for initialising the object
 * @return SRSLTE_SUCCESS if the configurations are valid, otherwise it returns an SRSLTE_ERROR code
 */
SRSLTE_API int srslte_dmrs_pdcch_estimator_init(srslte_dmrs_pdcch_estimator_t* q,
                                                const srslte_nr_carrier_t*     carrier,
                                                const srslte_coreset_t*        coreset);

/**
 * @brief Deallocate all memory allocated by a given PDCCH DMRS estimator object
 * @param q provides PDCCH DMRS estimator object
 */
SRSLTE_API void srslte_dmrs_pdcch_estimator_free(srslte_dmrs_pdcch_estimator_t* q);

/**
 * Estimates the PDCCH channel from the received PDCCH's DMRS. The DMRS are only transmitted in the REG where the
 * PDCCH is allocated.
 *
 * The most efficient way is estimating the overall CORESET independently whether there is data or not. Said so, the
 * output grid `estimates` shall be at least as big as the CORESET.
 *
 * This function is designed to be called prior to the PDCCH blind decoding and shall be called only once for every
 * CORESET.
 *
 * @param cfg Configuration that includes Carrier, CORESET and the Search Space
 * @param slot_idx Slot index in the frame
 * @param sf_symbols Received resource grid.
 * @param estimates CORESET Resource grid with the estimated channel
 * @return SRSLTE_SUCCESS if the configurations are valid, otherwise it returns an SRSLTE_ERROR code
 */
SRSLTE_API int srslte_dmrs_pdcch_estimate(srslte_dmrs_pdcch_estimator_t* q, uint32_t slot_idx, const cf_t* sf_symbols);

/**
 * @brief PDSCH DMRS measurement results
 */
typedef struct SRSLTE_API {
  /// Linear reference signal received power (RSRP). Measure correlation
  float rsrp;

  /// Energy per resource element (EPRE)
  float epre;

  /// CFO Measure in Hz (only available for durations 2 and 3)
  float cfo_hz;

  /// Measure synchronization error in micro-seconds
  float sync_error_us;
} srslte_dmrs_pdcch_measure_t;

/**
 * @brief Performs PDCCH DMRS measurements of a given PDCCH candidate for an aggregation level
 *
 * @note The measurement is useful for determining whether there is a PDCCH transmission in the given candidate.
 *
 * @attention The provided aggregation level and candidate need to be according to the search space.
 *
 * @param q provides PDCCH DMRS estimator object
 * @param search_space provides the search space
 * @param slot_idx Slot index in the frame
 * @param aggregation_level Indicates the aggregation level of the candidate to examine
 * @param candidate Indicates the candidate index of the available
 * @param rnti Indicates the UE RNTI (only used for UE search space type)
 * @param measure Provides the structure for storing the channel estimate measurements
 * @return SRSLTE_SUCCESS if the configurations are valid, otherwise it returns an SRSLTE_ERROR code
 */
int srslte_dmrs_pdcch_get_measure(srslte_dmrs_pdcch_estimator_t* q,
                                  const srslte_search_space_t*   search_space,
                                  uint32_t                       slot_idx,
                                  uint32_t                       aggregation_level,
                                  uint32_t                       candidate,
                                  uint16_t                       rnti,
                                  srslte_dmrs_pdcch_measure_t*   measure);

#ifdef __cplusplus
}
#endif

#endif // SRSLTE_DMRS_PDCCH_H
