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
 *  File:         cfo.h
 *
 *  Description:  Carrier frequency offset correction using complex exponentials.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_CFO_H
#define SRSLTE_CFO_H

#include <complex.h>

#include "srslte/config.h"
#include "srslte/phy/utils/cexptab.h"
#include "srslte/phy/common/phy_common.h"

#define SRSLTE_CFO_CEXPTAB_SIZE 4096

typedef struct SRSLTE_API {
  float            last_freq;
  float            tol;
  int              nsamples;
  int              max_samples;
  srslte_cexptab_t tab;
  cf_t*            cur_cexp;
} srslte_cfo_t;

SRSLTE_API int srslte_cfo_init(srslte_cfo_t* h, uint32_t nsamples);

SRSLTE_API void srslte_cfo_free(srslte_cfo_t* h);

SRSLTE_API int srslte_cfo_resize(srslte_cfo_t* h, uint32_t samples);

SRSLTE_API void srslte_cfo_set_tol(srslte_cfo_t* h, float tol);

SRSLTE_API void srslte_cfo_correct(srslte_cfo_t* h, const cf_t* input, cf_t* output, float freq);

SRSLTE_API void
srslte_cfo_correct_offset(srslte_cfo_t* h, const cf_t* input, cf_t* output, float freq, int cexp_offset, int nsamples);

SRSLTE_API float srslte_cfo_est_corr_cp(cf_t* input_buffer, uint32_t nof_prb);

#endif // SRSLTE_CFO_H
