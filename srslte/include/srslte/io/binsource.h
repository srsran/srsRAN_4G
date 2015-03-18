/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#ifndef BINSOURCE_
#define BINSOURCE_


#include <stdint.h>
#include "srslte/config.h"

/* Low-level API */
typedef struct SRSLTE_API{
  uint32_t seed;
  uint32_t *seq_buff;
  int seq_buff_nwords;
  int seq_cache_nbits;
  int seq_cache_rp;
}binsource_t;

SRSLTE_API void binsource_init(binsource_t* q);
SRSLTE_API void binsource_free(binsource_t* q);
SRSLTE_API void binsource_seed_set(binsource_t* q, uint32_t seed);
SRSLTE_API void binsource_seed_time(binsource_t *q);
SRSLTE_API int binsource_cache_gen(binsource_t* q, int nbits);
SRSLTE_API void binsource_cache_cpy(binsource_t* q, uint8_t *bits, int nbits);
SRSLTE_API int binsource_generate(binsource_t* q, uint8_t *bits, int nbits);

/* High-level API */
typedef struct SRSLTE_API {
  binsource_t obj;
  struct binsource_init {
    int cache_seq_nbits;   // If non-zero, generates random bits on init
    uint32_t seed;    // If non-zero, uses as random seed, otherwise local time is used.
  } init;
  struct binsource_ctrl_in {
    int nbits;        // Number of bits to generate
  } ctrl_in;
  uint8_t* output;
  int out_len;
}binsource_hl;

SRSLTE_API int binsource_initialize(binsource_hl* h);
SRSLTE_API int binsource_work(  binsource_hl* hl);
SRSLTE_API int binsource_stop(binsource_hl* hl);

#endif // BINSOURCE_
