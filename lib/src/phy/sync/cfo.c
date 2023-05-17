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

#include "srsran/srsran.h"
#include <math.h>
#include <stdlib.h>
#include <strings.h>

#include "srsran/phy/sync/cfo.h"
#include "srsran/phy/utils/cexptab.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

/* Set next macro to 1 for using table generated CFO compensation */
#define SRSRAN_CFO_USE_EXP_TABLE 0

int srsran_cfo_init(srsran_cfo_t* h, uint32_t nsamples)
{
#if SRSRAN_CFO_USE_EXP_TABLE
  int ret = SRSRAN_ERROR;
  bzero(h, sizeof(srsran_cfo_t));

  if (srsran_cexptab_init(&h->tab, SRSRAN_CFO_CEXPTAB_SIZE)) {
    goto clean;
  }
  h->cur_cexp = srsran_vec_cf_malloc(nsamples);
  if (!h->cur_cexp) {
    goto clean;
  }
  h->tol         = 0;
  h->last_freq   = 0;
  h->nsamples    = nsamples;
  h->max_samples = nsamples;
  srsran_cexptab_gen(&h->tab, h->cur_cexp, h->last_freq, h->nsamples);

  ret = SRSRAN_SUCCESS;
clean:
  if (ret == SRSRAN_ERROR) {
    srsran_cfo_free(h);
  }
  return ret;
#else  /* SRSRAN_CFO_USE_EXP_TABLE */
  h->nsamples = nsamples;
  return SRSRAN_SUCCESS;
#endif /* SRSRAN_CFO_USE_EXP_TABLE */
}

void srsran_cfo_free(srsran_cfo_t* h)
{
#if SRSRAN_CFO_USE_EXP_TABLE
  srsran_cexptab_free(&h->tab);
  if (h->cur_cexp) {
    free(h->cur_cexp);
  }
#endif /* SRSRAN_CFO_USE_EXP_TABLE */
  bzero(h, sizeof(srsran_cfo_t));
}

void srsran_cfo_set_tol(srsran_cfo_t* h, float tol)
{
  h->tol = tol;
}

int srsran_cfo_resize(srsran_cfo_t* h, uint32_t samples)
{
#if SRSRAN_CFO_USE_EXP_TABLE
  if (samples <= h->max_samples) {
    srsran_cexptab_gen(&h->tab, h->cur_cexp, h->last_freq, samples);
    h->nsamples = samples;
  } else {
    ERROR("Error in cfo_resize(): nof_samples must be lower than initialized");
    return SRSRAN_ERROR;
  }
#endif /* SRSRAN_CFO_USE_EXP_TABLE */
  return SRSRAN_SUCCESS;
}

void srsran_cfo_correct(srsran_cfo_t* h, const cf_t* input, cf_t* output, float freq)
{
#if SRSRAN_CFO_USE_EXP_TABLE
  if (fabs(h->last_freq - freq) > h->tol) {
    h->last_freq = freq;
    srsran_cexptab_gen(&h->tab, h->cur_cexp, h->last_freq, h->nsamples);
    DEBUG("CFO generating new table for frequency %.4fe-6", freq * 1e6);
  }
  srsran_vec_prod_ccc(h->cur_cexp, input, output, h->nsamples);
#else  /* SRSRAN_CFO_USE_EXP_TABLE */
  srsran_vec_apply_cfo(input, freq, output, h->nsamples);
#endif /* SRSRAN_CFO_USE_EXP_TABLE */
}

/* CFO correction which allows to specify the offset within the correction
 * table to allow phase-continuity across multi-subframe transmissions (NB-IoT)
 * Note that when correction table needs to be regenerated, the regeneration
 * takes place for the maximum number of samples
 */
void srsran_cfo_correct_offset(srsran_cfo_t* h,
                               const cf_t*   input,
                               cf_t*         output,
                               float         freq,
                               int           cexp_offset,
                               int           nsamples)
{
  if (fabs(h->last_freq - freq) > h->tol) {
    h->last_freq = freq;
    srsran_cexptab_gen(&h->tab, h->cur_cexp, h->last_freq, h->nsamples);
    DEBUG("CFO generating new table for frequency %.4fe-6", freq * 1e6);
  }
  srsran_vec_prod_ccc(&h->cur_cexp[cexp_offset], input, output, nsamples);
}

float srsran_cfo_est_corr_cp(cf_t* input_buffer, uint32_t nof_prb)
{
  int   nFFT         = srsran_symbol_sz(nof_prb);
  int   sf_n_samples = nFFT * 15;
  float tFFT         = (float)(1 / 15000.0);
  int   cp_size      = SRSRAN_CP_LEN_NORM(1, nFFT);

  // Compensate for initial SC-FDMA half subcarrier shift
  srsran_vec_apply_cfo(input_buffer, (float)(1 / (nFFT * 15e3)) * (15e3 / 2.0), input_buffer, sf_n_samples);

  // Conjugate multiply the correlation inputs
  cf_t cfo_estimated = srsran_vec_dot_prod_conj_ccc(&input_buffer[nFFT + SRSRAN_CP_LEN_NORM(0, nFFT)],
                                                    &input_buffer[2 * nFFT + SRSRAN_CP_LEN_NORM(0, nFFT)],
                                                    cp_size);

  // CFO correction and compensation for initial SC-FDMA half subcarrier shift
  float cfo = (float)(-1 * carg(cfo_estimated) / (float)(2 * M_PI * tFFT));
  srsran_vec_apply_cfo(input_buffer, (float)(1 / (nFFT * 15e3)) * ((-15e3 / 2.0) - cfo), input_buffer, sf_n_samples);
  return cfo;
}
