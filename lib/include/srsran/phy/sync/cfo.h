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
 *  File:         cfo.h
 *
 *  Description:  Carrier frequency offset correction using complex exponentials.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSRAN_CFO_H
#define SRSRAN_CFO_H

#include <complex.h>

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/utils/cexptab.h"

#define SRSRAN_CFO_CEXPTAB_SIZE 4096

typedef struct SRSRAN_API {
  float            last_freq;
  float            tol;
  int              nsamples;
  int              max_samples;
  srsran_cexptab_t tab;
  cf_t*            cur_cexp;
} srsran_cfo_t;

SRSRAN_API int srsran_cfo_init(srsran_cfo_t* h, uint32_t nsamples);

SRSRAN_API void srsran_cfo_free(srsran_cfo_t* h);

SRSRAN_API int srsran_cfo_resize(srsran_cfo_t* h, uint32_t samples);

SRSRAN_API void srsran_cfo_set_tol(srsran_cfo_t* h, float tol);

SRSRAN_API void srsran_cfo_correct(srsran_cfo_t* h, const cf_t* input, cf_t* output, float freq);

SRSRAN_API void
srsran_cfo_correct_offset(srsran_cfo_t* h, const cf_t* input, cf_t* output, float freq, int cexp_offset, int nsamples);

SRSRAN_API float srsran_cfo_est_corr_cp(cf_t* input_buffer, uint32_t nof_prb);

#endif // SRSRAN_CFO_H
