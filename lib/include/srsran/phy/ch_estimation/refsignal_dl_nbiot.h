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

#ifndef SRSRAN_REFSIGNAL_DL_NBIOT_H
#define SRSRAN_REFSIGNAL_DL_NBIOT_H

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"

// Number of references in a subframe: there are 2 symbols for port_id=0,1 x 2 slots x 2 refs per prb
#define SRSRAN_NBIOT_NUM_NRS_X_SYM_X_PORT 2

// Max number of symbols with RE in one PRB (4 ports LTE plus 2 port NB-IoT)
#define SRSRAN_NBIOT_MAX_NUM_RE_WITH_REFS 40

#define SRSRAN_NBIOT_REFSIGNAL_NUM_SF(nof_prb, port_id) (((port_id) < 2 ? 8 : 4) * (nof_prb))
#define SRSRAN_NBIOT_REFSIGNAL_PILOT_IDX(i, l, nof_prb) (2 * nof_prb * (l) + (i))

/** Brief: Narrowband Reference Signal (NRS)
 *
 * Object to manage downlink reference signals for channel estimation. (3GPP TS 36.211 version 13.3 Sec. 10.2.6)
 */
typedef struct SRSRAN_API {
  srsran_nbiot_cell_t cell;
  cf_t*               pilots[2][SRSRAN_NOF_SF_X_FRAME]; // Saves the reference signal per subframe for ports 0 and 1
} srsran_refsignal_dl_nbiot_t;

SRSRAN_API int srsran_refsignal_dl_nbiot_init(srsran_refsignal_dl_nbiot_t* q);

SRSRAN_API void srsran_refsignal_dl_nbiot_free(srsran_refsignal_dl_nbiot_t* q);

SRSRAN_API uint32_t srsran_refsignal_dl_nbiot_nof_symbols(uint32_t port_id);

SRSRAN_API int srsran_refsignal_dl_nbiot_set_cell(srsran_refsignal_dl_nbiot_t* q, srsran_nbiot_cell_t cell);

SRSRAN_API uint32_t srsran_refsignal_dl_nbiot_fidx(srsran_nbiot_cell_t cell, uint32_t l, uint32_t port_id, uint32_t m);

SRSRAN_API uint32_t srsran_refsignal_nrs_nsymbol(uint32_t l);

SRSRAN_API uint32_t srsran_refsignal_nbiot_cs_nof_re(srsran_nbiot_cell_t* cell, uint32_t port_id);

SRSRAN_API int srsran_refsignal_nrs_put_sf(srsran_nbiot_cell_t cell, uint32_t port_id, cf_t* pilots, cf_t* sf_symbols);

SRSRAN_API int srsran_refsignal_nrs_get_sf(srsran_nbiot_cell_t cell, uint32_t port_id, cf_t* sf_symbols, cf_t* pilots);

#endif // SRSRAN_REFSIGNAL_DL_NBIOT_H
