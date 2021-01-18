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

#ifndef SRSLTE_DMRS_PUCCH_H
#define SRSLTE_DMRS_PUCCH_H

#include "srslte/config.h"
#include "srslte/phy/ch_estimation/chest_ul.h"
#include "srslte/phy/phch/pucch_nr.h"

/**
 * @brief Puts NR-PUCCH format 1 DMRS in the provided resource grid
 * @param[in] q NR-PUCCH encoder/decoder object
 * @param[in] carrier Carrier configuration
 * @param[in] cfg PUCCH common configuration
 * @param[in] slot slot configuration
 * @param[in] resource PUCCH format 1 resource
 * @param[out] slot_symbols Resource grid of the given slot
 * @return SRSLTE_SUCCESS if successful, SRSLTE_ERROR code otherwise
 */
SRSLTE_API int srslte_dmrs_pucch_format1_put(const srslte_pucch_nr_t*                  q,
                                             const srslte_carrier_nr_t*                carrier,
                                             const srslte_pucch_nr_common_cfg_t*       cfg,
                                             const srslte_dl_slot_cfg_t*               slot,
                                             const srslte_pucch_nr_resource_format1_t* resource,
                                             cf_t*                                     slot_symbols);

/**
 * @brief Estimates NR-PUCCH format 1 resource elements from their DMRS in the provided resource grid
 * @param[in] q NR-PUCCH encoder/decoder object
 * @param[in] carrier Carrier configuration
 * @param[in] cfg PUCCH common configuration
 * @param[in] slot slot configuration
 * @param[in] resource PUCCH format 1 resource
 * @param[in] slot_symbols Resource grid of the given slot
 * @param[out] res UL Channel estimator result
 * @return SRSLTE_SUCCESS if successful, SRSLTE_ERROR code otherwise
 */
SRSLTE_API int srslte_dmrs_pucch_format1_estimate(const srslte_pucch_nr_t*                  q,
                                                  const srslte_carrier_nr_t*                carrier,
                                                  const srslte_pucch_nr_common_cfg_t*       cfg,
                                                  const srslte_dl_slot_cfg_t*               slot,
                                                  const srslte_pucch_nr_resource_format1_t* resource,
                                                  const cf_t*                               slot_symbols,
                                                  srslte_chest_ul_res_t*                    res);

#endif // SRSLTE_DMRS_PUCCH_H
