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

#ifndef SRSLTE_DMRS_PDCCH_H
#define SRSLTE_DMRS_PDCCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "srslte/phy/common/phy_common_nr.h"
#include "srslte/srslte.h"

SRSLTE_API int
srslte_dmrs_pdcch_put(const srslte_nr_pdcch_cfg_t* cfg, const srslte_dl_sf_cfg_t* dl_sf, cf_t* sf_symbols);

#ifdef __cplusplus
}
#endif

#endif // SRSLTE_DMRS_PDCCH_H
