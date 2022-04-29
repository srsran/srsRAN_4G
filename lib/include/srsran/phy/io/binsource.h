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
 *  File:         binsource.h
 *
 *  Description:  Pseudo-random binary source.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSRAN_BINSOURCE_H
#define SRSRAN_BINSOURCE_H

#include "srsran/config.h"
#include <stdint.h>

/* Low-level API */
typedef struct SRSRAN_API {
  uint32_t  seed;
  uint32_t* seq_buff;
  int       seq_buff_nwords;
  int       seq_cache_nbits;
  int       seq_cache_rp;
} srsran_binsource_t;

SRSRAN_API void srsran_binsource_init(srsran_binsource_t* q);

SRSRAN_API void srsran_binsource_free(srsran_binsource_t* q);

SRSRAN_API void srsran_binsource_seed_set(srsran_binsource_t* q, uint32_t seed);

SRSRAN_API void srsran_binsource_seed_time(srsran_binsource_t* q);

SRSRAN_API int srsran_binsource_cache_gen(srsran_binsource_t* q, int nbits);

SRSRAN_API void srsran_binsource_cache_cpy(srsran_binsource_t* q, uint8_t* bits, int nbits);

SRSRAN_API int srsran_binsource_generate(srsran_binsource_t* q, uint8_t* bits, int nbits);

#endif // SRSRAN_BINSOURCE_H
