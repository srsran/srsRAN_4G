/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
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

#ifndef SRSLTE_BINSOURCE_H
#define SRSLTE_BINSOURCE_H

#include "srslte/config.h"
#include <stdint.h>

/* Low-level API */
typedef struct SRSLTE_API {
  uint32_t  seed;
  uint32_t* seq_buff;
  int       seq_buff_nwords;
  int       seq_cache_nbits;
  int       seq_cache_rp;
} srslte_binsource_t;

SRSLTE_API void srslte_binsource_init(srslte_binsource_t* q);

SRSLTE_API void srslte_binsource_free(srslte_binsource_t* q);

SRSLTE_API void srslte_binsource_seed_set(srslte_binsource_t* q, uint32_t seed);

SRSLTE_API void srslte_binsource_seed_time(srslte_binsource_t* q);

SRSLTE_API int srslte_binsource_cache_gen(srslte_binsource_t* q, int nbits);

SRSLTE_API void srslte_binsource_cache_cpy(srslte_binsource_t* q, uint8_t* bits, int nbits);

SRSLTE_API int srslte_binsource_generate(srslte_binsource_t* q, uint8_t* bits, int nbits);

#endif // SRSLTE_BINSOURCE_H
