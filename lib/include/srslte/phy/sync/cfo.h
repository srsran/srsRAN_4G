/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
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
