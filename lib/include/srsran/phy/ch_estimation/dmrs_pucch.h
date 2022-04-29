/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#ifndef SRSRAN_DMRS_PUCCH_H
#define SRSRAN_DMRS_PUCCH_H

#include "srsran/config.h"
#include "srsran/phy/ch_estimation/chest_ul.h"
#include "srsran/phy/phch/pucch_nr.h"

#define SRSRAN_DMRS_PUCCH_FORMAT_3_4_MAX_NSYMB 4

/**
 * @brief Computes the symbol indexes carrying DMRS for NR-PUCCH formats 3 and 4
 * @remark Implements TS 38.211 Table 6.4.1.3.3.2-1: DM-RS positions for PUCCH format 3 and 4.
 * @param[in] resource Provides the format 3 or 4 resource
 * @param[out] idx Destination data for storing the symbol indexes
 * @return The number of DMRS symbols if the resource is valid, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_dmrs_pucch_format_3_4_get_symbol_idx(const srsran_pucch_nr_resource_t* resource,
                                                           uint32_t idx[SRSRAN_DMRS_PUCCH_FORMAT_3_4_MAX_NSYMB]);

/**
 * @brief Puts NR-PUCCH format 1 DMRS in the provided resource grid
 * @param[in] q NR-PUCCH encoder/decoder object
 * @param[in] carrier Carrier configuration
 * @param[in] cfg PUCCH common configuration
 * @param[in] slot slot configuration
 * @param[in] resource PUCCH format 1 resource
 * @param[out] slot_symbols Resource grid of the given slot
 * @return SRSRAN_SUCCESS if successful, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_dmrs_pucch_format1_put(const srsran_pucch_nr_t*            q,
                                             const srsran_carrier_nr_t*          carrier,
                                             const srsran_pucch_nr_common_cfg_t* cfg,
                                             const srsran_slot_cfg_t*            slot,
                                             const srsran_pucch_nr_resource_t*   resource,
                                             cf_t*                               slot_symbols);

/**
 * @brief Estimates NR-PUCCH format 1 resource elements from their DMRS in the provided resource grid
 * @param[in] q NR-PUCCH encoder/decoder object
 * @param[in] cfg PUCCH common configuration
 * @param[in] slot slot configuration
 * @param[in] resource PUCCH format 1 resource
 * @param[in] slot_symbols Resource grid of the given slot
 * @param[out] res UL Channel estimator result
 * @return SRSRAN_SUCCESS if successful, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_dmrs_pucch_format1_estimate(const srsran_pucch_nr_t*            q,
                                                  const srsran_pucch_nr_common_cfg_t* cfg,
                                                  const srsran_slot_cfg_t*            slot,
                                                  const srsran_pucch_nr_resource_t*   resource,
                                                  const cf_t*                         slot_symbols,
                                                  srsran_chest_ul_res_t*              res);

/**
 * @brief Puts NR-PUCCH format 2 DMRS in the provided resource grid
 * @param[in] q NR-PUCCH encoder/decoder object
 * @param[in] carrier Carrier configuration
 * @param[in] cfg PUCCH common configuration
 * @param[in] slot slot configuration
 * @param[in] resource PUCCH format 2 resource
 * @param[out] slot_symbols Resource grid of the given slot
 * @return SRSRAN_SUCCESS if successful, SRSRAN_ERROR code otherwise
 */
int srsran_dmrs_pucch_format2_put(const srsran_pucch_nr_t*            q,
                                  const srsran_carrier_nr_t*          carrier,
                                  const srsran_pucch_nr_common_cfg_t* cfg,
                                  const srsran_slot_cfg_t*            slot,
                                  const srsran_pucch_nr_resource_t*   resource,
                                  cf_t*                               slot_symbols);

/**
 * @brief Estimates NR-PUCCH format 2 resource elements from their DMRS in the provided resource grid
 * @param[in] q NR-PUCCH encoder/decoder object
 * @param[in] cfg PUCCH common configuration
 * @param[in] slot slot configuration
 * @param[in] resource PUCCH format 2 resource
 * @param[in] slot_symbols Resource grid of the given slot
 * @param[out] res UL Channel estimator result
 * @return SRSRAN_SUCCESS if successful, SRSRAN_ERROR code otherwise
 */
int srsran_dmrs_pucch_format2_estimate(const srsran_pucch_nr_t*            q,
                                       const srsran_pucch_nr_common_cfg_t* cfg,
                                       const srsran_slot_cfg_t*            slot,
                                       const srsran_pucch_nr_resource_t*   resource,
                                       const cf_t*                         slot_symbols,
                                       srsran_chest_ul_res_t*              res);

#endif // SRSRAN_DMRS_PUCCH_H
