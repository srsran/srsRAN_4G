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

#ifndef SRSRAN_DMRS_SCH_H
#define SRSRAN_DMRS_SCH_H

#include "srsran/phy/ch_estimation/chest_dl.h"
#include "srsran/phy/common/phy_common_nr.h"
#include "srsran/phy/phch/phch_cfg_nr.h"
#include <stdint.h>

#define SRSRAN_DMRS_SCH_MAX_SYMBOLS 4

/**
 * @brief Helper macro for counting the number of subcarriers taken by DMRS in a PRB.
 */
#define SRSRAN_DMRS_SCH_SC(CDM_GROUPS, DMRS_TYPE)                                                                      \
  (SRSRAN_MIN(SRSRAN_NRE, (CDM_GROUPS) * ((DMRS_TYPE) == srsran_dmrs_sch_type_1 ? 6 : 4)))

/**
 * @brief PDSCH DMRS estimator object
 *
 * @note The DMRS PDSCH object has to be initialised and the carrier parameters needs to be set.
 *
 * @see srsran_dmrs_sch_init
 * @see srsran_dmrs_sch_set_carrier
 * @see srsran_dmrs_sch_free
 * @see srsran_dmrs_sch_put_sf
 * @see srsran_dmrs_sch_estimate
 */
typedef struct {
  bool is_rx;

  srsran_carrier_nr_t carrier;

  srsran_interp_lin_t interpolator_type1; /// Type 1 DMRS: 1 pilot every 2 RE
  srsran_interp_lin_t interpolator_type2; /// Type 2 DMRS: 2 consecutive pilots every 6 RE

  uint32_t max_nof_prb;
  cf_t*    pilot_estimates; /// Pilots least squares estimates
  cf_t*    temp;            /// Temporal data vector of size SRSRAN_NRE * carrier.nof_prb

  float* filter; ///< Smoothing filter

  srsran_csi_trs_measurements_t csi; ///< Last estimated channel state information
} srsran_dmrs_sch_t;

/**
 * @brief Computes the symbol indexes carrying DMRS and stores them in symbols_idx
 * @param dmrs_cfg DMRS configuration
 * @param grant PDSCH information provided by a DCI
 * @param symbols_idx is the destination pointer where the symbols indexes are stored
 * @return It returns the number of symbols if inputs are valid, otherwise, it returns SRSRAN_ERROR code.
 */
SRSRAN_API int srsran_dmrs_sch_get_symbols_idx(const srsran_dmrs_sch_cfg_t* dmrs_cfg,
                                               const srsran_sch_grant_nr_t* grant,
                                               uint32_t                     symbols_idx[SRSRAN_DMRS_SCH_MAX_SYMBOLS]);

/**
 * @brief Computes the resource element pattern of resource elements reserved for DMRS
 * @param cfg PDSCH DMRS configuration provided by upper layers
 * @param[out] pattern Provides the RE pattern to fill
 * @return SRSRAN_SUCCESS if computation is successful, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_dmrs_sch_rvd_re_pattern(const srsran_dmrs_sch_cfg_t* cfg,
                                              const srsran_sch_grant_nr_t* grant,
                                              srsran_re_pattern_t*         pattern);

/**
 * @brief Calculates the number of resource elements taken by a PDSCH-DMRS for a given PDSCH transmission
 * @param dmrs_cfg PDSCH-DMRS configuration
 * @param grant PDSCH information provided by a DCI
 * @return it returns the number of resource elements if the configuration is valid, otherwise it returns SRSRAN_ERROR
 */
SRSRAN_API int srsran_dmrs_sch_get_N_prb(const srsran_dmrs_sch_cfg_t* dmrs_cfg, const srsran_sch_grant_nr_t* grant);

/**
 * @brief Stringifies the PDSCH DMRS configuration
 *
 * @param cfg PDSCH DMRS configuration
 * @param msg Pointer to the destination array
 * @param max_len Maximum number of characters to write
 *
 * @return It returns the number of characters written in the vector if no error occurs, otherwise it returns
 * SRSRAN_ERROR code
 */
SRSRAN_API int srsran_dmrs_sch_cfg_to_str(const srsran_dmrs_sch_cfg_t* cfg, char* msg, uint32_t max_len);

/**
 * @brief Initialises DMRS PDSCH object
 *
 * @param q DMRS PDSCH object
 * @param is_rx indicates whether the object is used as receiver (in this case, it shall initialise as an estimator)
 * @return it returns SRSRAN_ERROR code if an error occurs, otherwise it returns SRSRAN_SUCCESS
 */
SRSRAN_API int srsran_dmrs_sch_init(srsran_dmrs_sch_t* q, bool is_rx);

/**
 * @brief Frees DMRS PDSCH object
 *
 * @param q DMRS PDSCH object
 */
SRSRAN_API void srsran_dmrs_sch_free(srsran_dmrs_sch_t* q);

/**
 * @brief Sets the carrier configuration. if the PDSCH DMRS object is configured as UE, it will resize internal buffers
 * and objects.
 *
 * @param q DMRS PDSCH object
 * @param carrier Carrier configuration
 *
 * @return it returns SRSRAN_ERROR code if an error occurs, otherwise it returns SRSRAN_SUCCESS
 */
SRSRAN_API int srsran_dmrs_sch_set_carrier(srsran_dmrs_sch_t* q, const srsran_carrier_nr_t* carrier);

/**
 * @brief Puts PDSCH DMRS into a given resource grid
 *
 * @param q DMRS PDSCH object
 * @param slot_cfg Slot configuration
 * @param pdsch_cfg PDSCH configuration provided by upper layers
 * @param grant PDSCH information provided by a DCI
 * @param sf_symbols Resource grid
 *
 * @return it returns SRSRAN_ERROR code if an error occurs, otherwise it returns SRSRAN_SUCCESS
 */
SRSRAN_API int srsran_dmrs_sch_put_sf(srsran_dmrs_sch_t*           q,
                                      const srsran_slot_cfg_t*     slot_cfg,
                                      const srsran_sch_cfg_nr_t*   pdsch_cfg,
                                      const srsran_sch_grant_nr_t* grant,
                                      cf_t*                        sf_symbols);

/**
 * @brief Estimates the channel for PDSCH from the DMRS
 *
 * @attention Current implementation supports only type1 PDSCH DMRS (1 pilot every 2 RE)
 *
 * @param q DMRS-PDSCH object
 * @param slot Slot configuration
 * @param cfg PDSCH configuration provided by upper layers
 * @param grant PDSCH information provided by a DCI
 * @param sf_symbols Received resource grid
 * @param[out] ce Channel estimates
 *
 * @return it returns SRSRAN_ERROR code if an error occurs, otherwise it returns SRSRAN_SUCCESS
 */
SRSRAN_API int srsran_dmrs_sch_estimate(srsran_dmrs_sch_t*           q,
                                        const srsran_slot_cfg_t*     slot,
                                        const srsran_sch_cfg_nr_t*   cfg,
                                        const srsran_sch_grant_nr_t* grant,
                                        const cf_t*                  sf_symbols,
                                        srsran_chest_dl_res_t*       chest_res);

#endif // SRSRAN_DMRS_SCH_H
