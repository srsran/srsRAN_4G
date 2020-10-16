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

/******************************************************************************
 *  File:         pdsch_cfg_nr.h
 *
 *  Description:  Physical downlink shared channel configuration
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_PDSCH_CFG_NR_H
#define SRSLTE_PDSCH_CFG_NR_H

#include "srslte/phy/ch_estimation/dmrs_pdsch.h"
#include "srslte/phy/common/phy_common_nr.h"
#include "srslte/phy/fec/cbsegm.h"
#include "srslte/phy/fec/softbuffer.h"
#include "srslte/phy/phch/ra.h"

/**
 * @brief flatten PDSCH time domain allocation parameters
 * @remark Described in TS 38.331 V15.10.0 Section PDSCH-TimeDomainResourceAllocationList
 */
typedef struct SRSLTE_API {
  /// Slot offset between DCI and its scheduled PDSCH
  uint32_t k0;

  /// PDSCH mapping type
  srslte_pdsch_mapping_type_t mapping_type;

  /// An index giving valid combinations of start symbol and length (jointly encoded) as start and length indicator
  /// (SLIV). The network configures the field so that the allocation does not cross the slot boundary
  uint32_t sliv;

} srslte_pdsch_allocation_t;

typedef struct SRSLTE_API {
  /// TBD
} srslte_pdsch_grant_nr_t;

typedef struct SRSLTE_API {
  srslte_pdsch_allocation_t allocation;

  /// PDSCH Start symbol index
  uint32_t S;

  /// PDSCH length in number of symbols
  uint32_t L;
} srslte_pdsch_cfg_nr_t;

#endif // SRSLTE_PDSCH_CFG_NR_H
