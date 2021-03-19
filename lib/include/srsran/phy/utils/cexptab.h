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
