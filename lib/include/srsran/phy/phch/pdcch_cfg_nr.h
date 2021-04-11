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

#ifndef SRSRAN_PDCCH_CFG_NR_H
#define SRSRAN_PDCCH_CFG_NR_H

#include "dci_nr.h"

/**
 * Maximum number of CORESET
 * @remark Defined in TS 38.331 by maxNrofControlResourceSets-1
 */
#define SRSRAN_UE_DL_NR_MAX_NOF_CORESET 12

/**
 * Maximum number of Search spaces
 * @remark Defined in TS 38.331 by maxNrofSearchSpaces-1
 */
#define SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE 40

/**
 * @brief PDCCH configuration provided by upper layers
 */
typedef struct SRSRAN_API {
  srsran_coreset_t coreset[SRSRAN_UE_DL_NR_MAX_NOF_CORESET]; ///< PDCCH Control resource sets (CORESET) collection
  bool             coreset_present[SRSRAN_UE_DL_NR_MAX_NOF_CORESET]; ///< CORESET present flags

  srsran_search_space_t search_space[SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE];
  bool                  search_space_present[SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE];

  srsran_search_space_t ra_search_space;
  bool                  ra_search_space_present;
} srsran_pdcch_cfg_nr_t;

#endif // SRSRAN_PDCCH_CFG_NR_H
