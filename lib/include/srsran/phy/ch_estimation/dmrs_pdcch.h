/**
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSRAN_DMRS_PDCCH_H
#define SRSRAN_DMRS_PDCCH_H

#include "srsran/phy/common/phy_common_nr.h"
#include "srsran/phy/phch/dci_nr.h"
#include "srsran/phy/resampling/interp.h"
#include "srsran/phy/resampling/resampler.h"

/**
 * @brief Puts in the resource grid the DeModulation Reference Signals for decoding PDCCH.
 *
 * @remark: Implemented as specified by TS 38.211 V15.8.0 Section 7.1.4.3
 *
 * @attention it expects sf_symbols to be size SRSRAN_SLOT_LEN_RE_NR(cfg->carrier.nof_prb)
 *
 * @param[in] carrier Provides carrier configuration
 * @param[in] coreset Provides higher layer CORSET configuration
 * @param[in] slot_cfg Provides DL slot configuration
 * @param[in] dci_location Provides DCI location
 * @param sf_symbols is the resource grid where the DMRS resource elements will be written
 * @return SRSRAN_SUCCESS if the configurations are valid, otherwise it returns an SRSRAN_ERROR code
 */
SRSRAN_API int srsran_dmrs_pdcch_put(const srsran_carrier_nr_t*   carrier,
                                     const srsran_coreset_t*      coreset,
                                     const srsran_slot_cfg_t*     slot_cfg,
                                     const srsran_dci_location_t* dci_location,
                                     cf_t*                        sf_symbols);

/**
 * @brief PDCCH DMRS channel estimator object
 *
 * @see srsran_dmrs_pdcch_estimator_init
 * @see srsran_dmrs_pdcch_estimator_free
 * @see srsran_dmrs_pdcch_estimator_estimate
 * @see srsran_dmrs_pdcch_estimator_measure
 */
typedef struct SRSRAN_API {
  /// Current estimator carrier configuration
  srsran_carrier_nr_t carrier;

  /// Current estimator CORESET configuration
  srsran_coreset_t coreset;

  /// Stores the current CORESET bandwidth in PRB
  uint32_t coreset_bw;

  /// Stores the current CORESET size in RE
  uint32_t coreset_sz;

  /// Object for interpolating, it shall be initialised only once
  srsran_interp_lin_t interpolator;

  /// Pilots least square estimates, one vector for each possible symbol. Since there are one pilot every 4 sub-carriers
  /// , each vector is three times the CORESEt band-width
  cf_t* lse[SRSRAN_CORESET_DURATION_MAX];

  /// Channel estimates, size coreset_sz
  cf_t* ce;

  /// Frequency domain smoothing filter
  float*   filter;
  uint32_t filter_len;
} srsran_dmrs_pdcch_estimator_t;

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
 * @return SRSRAN_SUCCESS if the configurations are valid, otherwise it returns an SRSRAN_ERROR code
 */
SRSRAN_API int srsran_dmrs_pdcch_estimator_init(srsran_dmrs_pdcch_estimator_t* q,
                                                const srsran_carrier_nr_t*     carrier,
                                                const srsran_coreset_t*        coreset);

/**
 * @brief Deallocate all memory allocated by a given PDCCH DMRS estimator object
 * @param q provides PDCCH DMRS estimator object
 */
SRSRAN_API void srsran_dmrs_pdcch_estimator_free(srsran_dmrs_pdcch_estimator_t* q);

/**
 * @brief Estimates the configured CORESET channel from the received PDCCH's DMRS.
 *
 * This function is designed to be called prior to the PDCCH blind decoding and shall be called only once for every
 * CORESET in every slot.
 *
 * The channel estimate measurements are performed at PDCCH candidate level through the function
 * srsran_dmrs_pdcch_estimator_measure.
 *
 * @param[in,out] cfg Configuration that includes Carrier, CORESET and the Search Space
 * @param[in] slot_cfg Slot index in the frame
 * @param[in] sf_symbols Received resource grid.
 * @return SRSRAN_SUCCESS if the configurations are valid, otherwise it returns an SRSRAN_ERROR code
 */
SRSRAN_API int
srsran_dmrs_pdcch_estimate(srsran_dmrs_pdcch_estimator_t* q, const srsran_slot_cfg_t* slot_cfg, const cf_t* sf_symbols);

/**
 * @brief PDSCH DMRS measurement results
 *
 * @see srsran_dmrs_pdcch_estimator_measure
 */
typedef struct SRSRAN_API {
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
} srsran_dmrs_pdcch_measure_t;

/**
 * @brief PDSCH DMRS Channel estimates structure
 *
 * @see srsran_dmrs_pdcch_get_ce
 */
typedef struct SRSRAN_API {
  /// Channel estimates, subtract 1 DMRS for every 4 RE, a maximum of L=16 and 6 PRB per CCE
  cf_t     ce[SRSRAN_PDCCH_MAX_RE];
  uint32_t nof_re;
  float    noise_var;
} srsran_dmrs_pdcch_ce_t;

/**
 * @brief Performs PDCCH DMRS measurements of a given DCI location
 *
 * @note The measurement is useful for determining whether there is a PDCCH transmission in the given DCI location.
 *
 * @param[in] q provides PDCCH DMRS estimator object
 * @param[in] dci_location provides the search space
 * @param[in] location Provides the aggregation level and CCE resource
 * @param[out] measure Provides the structure for storing the channel estimate measurements
 * @return SRSRAN_SUCCESS if the configurations are valid, otherwise it returns an SRSRAN_ERROR code
 */
SRSRAN_API int srsran_dmrs_pdcch_get_measure(const srsran_dmrs_pdcch_estimator_t* q,
                                             const srsran_dci_location_t*         location,
                                             srsran_dmrs_pdcch_measure_t*         measure);

/**
 * @brief Extracts PDCCH DMRS channel estimates of a given PDCCH candidate for an aggregation level
 *
 * @attention The provided aggregation level and candidate need to be according to the search space.
 *
 * @param[in] q provides PDCCH DMRS estimator object
 * @param[in] location Provides the aggregation level and CCE resource
 * @param[out] ce Provides the structure for storing the channel estimates
 * @return SRSRAN_SUCCESS if the configurations are valid, otherwise it returns an SRSRAN_ERROR code
 */
SRSRAN_API int srsran_dmrs_pdcch_get_ce(const srsran_dmrs_pdcch_estimator_t* q,
                                        const srsran_dci_location_t*         location,
                                        srsran_dmrs_pdcch_ce_t*              ce);

#endif // SRSRAN_DMRS_PDCCH_H
