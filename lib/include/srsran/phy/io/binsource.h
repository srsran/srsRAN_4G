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
