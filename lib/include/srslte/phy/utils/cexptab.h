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
 *  File:         cexptab.h
 *
 *  Description:  Utility module for generation of complex exponential tables.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_CEXPTAB_H
#define SRSLTE_CEXPTAB_H

#include "srslte/config.h"
#include <complex.h>
#include <stdint.h>

typedef struct SRSLTE_API {
  uint32_t size;
  cf_t*    tab;
} srslte_cexptab_t;

SRSLTE_API int srslte_cexptab_init(srslte_cexptab_t* nco, uint32_t size);

SRSLTE_API void srslte_cexptab_free(srslte_cexptab_t* nco);

SRSLTE_API void srslte_cexptab_gen(srslte_cexptab_t* nco, cf_t* x, float freq, uint32_t len);

SRSLTE_API void srslte_cexptab_gen_direct(cf_t* x, float freq, uint32_t len);

SRSLTE_API void srslte_cexptab_gen_sf(cf_t* x, float freq, uint32_t fft_size);

#endif // SRSLTE_CEXPTAB_H
