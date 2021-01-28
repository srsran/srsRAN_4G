/**
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

#include "srslte/phy/common/phy_common_nr.h"
#include "srslte/phy/phch/dci_nr.h"
#include "srslte/phy/resampling/interp.h"
#include "srslte/phy/resampling/resampler.h"

/**
 * @brief Puts in the resource grid the DeModulation Reference Signals for decoding PDCCH.
 *
 * @remark: Implemented as specified by TS 38.211 V15.8.0 Section 7.1.4.3
 *
 * @attention it expects sf_symbols to be size SRSLTE_SLOT_LEN_RE_NR(cfg->carrier.nof_prb)
 *
 * @param[in] carrier Provides carrier configuration
 * @param[in] coreset Provides higher layer CORSET configuration
 * @param[in] slot_cfg Provides DL slot configuration
 * @param[in] dci_location Provides DCI location
 * @param sf_symbols is the resource grid where the DMRS resource elements will be written
 * @return SRSLTE_SUCCESS if the configurations are valid, otherwise it returns an SRSLTE_ERROR code
 */
SRSLTE_API int srslte_dmrs_pdcch_put(const srslte_carrier_nr_t*   carrier,
                                     const srslte_coreset_t*      coreset,
                                     const srslte_dl_slot_cfg_t*  slot_cfg,
                                     const srslte_dci_location_t* dci_location,
                                     cf_t*                        sf_symbols);

/**
 * @brief PDCCH DMRS channel estimator object
 *
 * @see srslte_dmrs_pdcch_estimator_init
 * @see srslte_dmrs_pdcch_estimator_free
 * @see srslte_dmrs_pdcch_estimator_estimate
 * @see srslte_dmrs_pdcch_estimator_measure
 */
typedef struct SRSLTE_API {
  /// Current estimator carrier configuration
  srslte_carrier_nr_t carrier;

  /// Current estimator CORESET configuration
  srslte_coreset_t coreset;

  /// Stores the current CORESET bandwidth in PRB
  uint32_t coreset_bw;

  /// Stores the current CORESET size in RE
  uint32_t coreset_sz;

  /// Object for interpolating, it shall be initialised only once
  srslte_interp_lin_t interpolator;

  /// Pilots least square estimates, one vector for each possible symbol. Since there are one pilot every 4 sub-carriers
  /// , each vector is three times the CORESEt band-width
  cf_t* lse[SRSLTE_CORESET_DURATION_MAX];

  /// Channel estimates, size coreset_sz
  cf_t* ce;

  /// Frequency domain smoothing filter
  float*   filter;
  uint32_t filter_len;
} srslte_dmrs_pdcch_estimator_t;

/**
 * @brief Initialises the PDCCH DMRS channel estimator from a given carrier and CORESET configuration
 *
 * \attention The initialization function expects the object being previously zeroed on its first call
 * \attention Initialization is required every time the carrier and/or CORESET changes. No free is required between
 * initializations.
 *
 * @param[in,out] q provides PDCCH DMRS estimator object
 * @param[in] carrier Provides carrier configuration
 * @param[in] coreset Provides higher layer CORSET configuration
 * @return SRSLTE_SUCCESS if the configurations are valid, otherwise it returns an SRSLTE_ERROR code
 */
SRSLTE_API int srslte_dmrs_pdcch_estimator_init(srslte_dmrs_pdcch_estimator_t* q,
                                                const srslte_carrier_nr_t*     carrier,
                                                const srslte_coreset_t*        coreset);

/**
 * @brief Deallocate all memory allocated by a given PDCCH DMRS estimator object
 * @param q provides PDCCH DMRS estimator object
 */
SRSLTE_API void srslte_dmrs_pdcch_estimator_free(srslte_dmrs_pdcch_estimator_t* q);

/**
 * @brief Estimates the configured CORESET channel from the received PDCCH's DMRS.
 *
 * This function is designed to be called prior to the PDCCH blind decoding and shall be called only once for every
 * CORESET in every slot.
 *
 * The channel estimate measurements are performed at PDCCH candidate level through the function
 * srslte_dmrs_pdcch_estimator_measure.
 *
 * @param[in,out] cfg Configuration that includes Carrier, CORESET and the Search Space
 * @param[in] slot_cfg Slot index in the frame
 * @param[in] sf_symbols Received resource grid.
 * @return SRSLTE_SUCCESS if the configurations are valid, otherwise it returns an SRSLTE_ERROR code
 */
SRSLTE_API int srslte_dmrs_pdcch_estimate(srslte_dmrs_pdcch_estimator_t* q,
                                          const srslte_dl_slot_cfg_t*    slot_cfg,
                                          const cf_t*                    sf_symbols);

/**
 * @brief PDSCH DMRS measurement results
 *
 * @see srslte_dmrs_pdcch_estimator_measure
 */
typedef struct SRSLTE_API {
  /// Linear reference signal received power (RSRP). Measure correlation
  float rsrp;
  float rsrp_dBfs;

  /// Energy per resource element (EPRE)
  float epre;
  float epre_dBfs;

  /// Normalized Correlation (RSRP / EPRE)
  float norm_corr;

  /// CFO Measure in Hz (only available for CORESET durations 2 and 3)
  float cfo_hz;

  /// Measure synchronization error in micro-seconds
  float sync_error_us;
} srslte_dmrs_pdcch_measure_t;

/**
 * @brief PDSCH DMRS Channel estimates structure
 *
 * @see srslte_dmrs_pdcch_get_ce
 */
typedef struct SRSLTE_API {
  /// Channel estimates, subtract 1 DMRS for every 4 RE, a maximum of L=16 and 6 PRB per CCE
  cf_t     ce[SRSLTE_PDCCH_MAX_RE];
  uint32_t nof_re;
  float    noise_var;
} srslte_dmrs_pdcch_ce_t;

/**
 * @brief Performs PDCCH DMRS measurements of a given DCI location
 *
 * @note The measurement is useful for determining whether there is a PDCCH transmission in the given DCI location.
 *
 * @param[in] q provides PDCCH DMRS estimator object
 * @param[in] dci_location provides the search space
 * @param[in] location Provides the aggregation level and CCE resource
 * @param[out] measure Provides the structure for storing the channel estimate measurements
 * @return SRSLTE_SUCCESS if the configurations are valid, otherwise it returns an SRSLTE_ERROR code
 */
SRSLTE_API int srslte_dmrs_pdcch_get_measure(const srslte_dmrs_pdcch_estimator_t* q,
                                             const srslte_dci_location_t*         location,
                                             srslte_dmrs_pdcch_measure_t*         measure);

/**
 * @brief Extracts PDCCH DMRS channel estimates of a given PDCCH candidate for an aggregation level
 *
 * @attention The provided aggregation level and candidate need to be according to the search space.
 *
 * @param[in] q provides PDCCH DMRS estimator object
 * @param[in] location Provides the aggregation level and CCE resource
 * @param[out] ce Provides the structure for storing the channel estimates
 * @return SRSLTE_SUCCESS if the configurations are valid, otherwise it returns an SRSLTE_ERROR code
 */
SRSLTE_API int srslte_dmrs_pdcch_get_ce(const srslte_dmrs_pdcch_estimator_t* q,
                                        const srslte_dci_location_t*         location,
                                        srslte_dmrs_pdcch_ce_t*              ce);

#endif // SRSLTE_DMRS_PDCCH_H
