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
