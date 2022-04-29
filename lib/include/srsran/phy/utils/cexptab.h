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

/******************************************************************************
 *  File:         cexptab.h
 *
 *  Description:  Utility module for generation of complex exponential tables.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSRAN_CEXPTAB_H
#define SRSRAN_CEXPTAB_H

#include "srsran/config.h"
#include <complex.h>
#include <stdint.h>

typedef struct SRSRAN_API {
  uint32_t size;
  cf_t*    tab;
} srsran_cexptab_t;

SRSRAN_API int srsran_cexptab_init(srsran_cexptab_t* nco, uint32_t size);

SRSRAN_API void srsran_cexptab_free(srsran_cexptab_t* nco);

SRSRAN_API void srsran_cexptab_gen(srsran_cexptab_t* nco, cf_t* x, float freq, uint32_t len);

SRSRAN_API void srsran_cexptab_gen_direct(cf_t* x, float freq, uint32_t len);

SRSRAN_API void srsran_cexptab_gen_sf(cf_t* x, float freq, uint32_t fft_size);

#endif // SRSRAN_CEXPTAB_H
