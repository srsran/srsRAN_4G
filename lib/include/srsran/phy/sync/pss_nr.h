/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_PSS_NR_H
#define SRSRAN_PSS_NR_H

#include "srsran/config.h"
#include "srsran/phy/common/phy_common_nr.h"
#include <inttypes.h>

/**
 * @brief NR PSS sequence length in frequency domain
 */
#define SRSRAN_PSS_NR_LEN 127

/**
 * @brief NR PSS Symbol number
 */
#define SRSRAN_PSS_NR_SYMBOL_IDX 0

/**
 * @brief Put NR PSS sequence in the SSB grid
 * @remark Described in TS 38.211 section 7.4.2.2 Primary synchronization signal
 * @param ssb_grid SSB resource grid
 * @param N_id_2 Physical cell ID 2
 * @param beta PSS power allocation
 * @return SRSLTE_SUCCESS if the parameters are valid, SRSLTE_ERROR code otherwise
 */
SRSRAN_API int srsran_pss_nr_put(cf_t ssb_grid[SRSRAN_SSB_NOF_RE], uint32_t N_id_2, float beta);

/**
 * @brief Extracts the NR PSS Least Square Estimates (LSE) from the SSB grid
 * @param ssb_grid received SSB resource grid
 * @param N_id_2 Physical cell ID 2
 * @param lse Provides LSE pointer
 * @return SRSLTE_SUCCESS if the parameters are valid, SRSLTE_ERROR code otherwise
 */
SRSRAN_API int srsran_pss_nr_extract_lse(const cf_t* ssb_grid, uint32_t N_id_2, cf_t lse[SRSRAN_PSS_NR_LEN]);

#endif // SRSRAN_PSS_NR_H
