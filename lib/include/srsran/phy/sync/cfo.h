/**
 * Copyright 2013-2023 Software Radio Systems Limited
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
