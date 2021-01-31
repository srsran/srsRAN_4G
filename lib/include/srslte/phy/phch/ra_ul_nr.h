/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSLTE_RA_UL_NR_H
#define SRSLTE_RA_UL_NR_H

#include "srslte/config.h"
#include "srslte/phy/phch/pucch_cfg_nr.h"
#include "uci_cfg_nr.h"

/**
 * @brief Calculates the minimum number of PRB required for transmitting NR-PUCCH Format 2, 3 or 4
 * @remark Based in TS 38.213 9.2.5.1 UE procedure for multiplexing HARQ-ACK or CSI and SR in a PUCCH
 * @return The number of PRB if the provided configuration is valid, SRSLTE_ERROR code otherwise
 */
SRSLTE_API int srslte_ra_ul_nr_pucch_format_2_3_min_prb(const srslte_pucch_nr_resource_t* resource,
                                                        const srslte_uci_cfg_nr_t*        uci_cfg);

#endif // SRSLTE_RA_UL_NR_H
