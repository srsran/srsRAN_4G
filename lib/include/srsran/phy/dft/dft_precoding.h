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

/**********************************************************************************************
 *  File:         dft_precoding.h
 *
 *  Description:  DFT-based transform precoding object.
 *                Used in generation of uplink SCFDMA signals.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 5.3.3
 *********************************************************************************************/

#ifndef SRSRAN_DFT_PRECODING_H
#define SRSRAN_DFT_PRECODING_H

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/dft/dft.h"

/* DFT-based Transform Precoding object */
typedef struct SRSRAN_API {

  uint32_t          max_prb;
  srsran_dft_plan_t dft_plan[SRSRAN_MAX_PRB + 1];

} srsran_dft_precoding_t;

SRSRAN_API int srsran_dft_precoding_init(srsran_dft_precoding_t* q, uint32_t max_prb, bool is_tx);

SRSRAN_API int srsran_dft_precoding_init_tx(srsran_dft_precoding_t* q, uint32_t max_prb);

SRSRAN_API int srsran_dft_precoding_init_rx(srsran_dft_precoding_t* q, uint32_t max_prb);

SRSRAN_API void srsran_dft_precoding_free(srsran_dft_precoding_t* q);

SRSRAN_API bool srsran_dft_precoding_valid_prb(uint32_t nof_prb);

SRSRAN_API uint32_t srsran_dft_precoding_get_valid_prb(uint32_t nof_prb);

SRSRAN_API int
srsran_dft_precoding(srsran_dft_precoding_t* q, cf_t* input, cf_t* output, uint32_t nof_prb, uint32_t nof_symbols);

#endif // SRSRAN_DFT_PRECODING_H
