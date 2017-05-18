/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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

#include <strings.h>
#include <stdlib.h>
#include <math.h>

#include "srslte/utils/cexptab.h"
#include "srslte/sync/cfo.h"
#include "srslte/utils/vector.h"
#include "srslte/utils/debug.h"

int srslte_cfo_init(srslte_cfo_t *h, uint32_t nsamples) {
  int ret = SRSLTE_ERROR;
  bzero(h, sizeof(srslte_cfo_t));

  if (srslte_cexptab_init(&h->tab, SRSLTE_CFO_CEXPTAB_SIZE)) {
    goto clean;
  }
  h->cur_cexp = srslte_vec_malloc(sizeof(cf_t) * nsamples);
  if (!h->cur_cexp) {
    goto clean;
  }
  h->tol = SRSLTE_CFO_TOLERANCE;
  h->last_freq = 0;
  h->nsamples = nsamples;
  srslte_cexptab_gen(&h->tab, h->cur_cexp, h->last_freq, h->nsamples);

  ret = SRSLTE_SUCCESS;
clean:
  if (ret == SRSLTE_ERROR) {
    srslte_cfo_free(h);
  }
  return ret;
}

void srslte_cfo_free(srslte_cfo_t *h) {
  srslte_cexptab_free(&h->tab);
  if (h->cur_cexp) {
    free(h->cur_cexp);
  }
  bzero(h, sizeof(srslte_cfo_t));
}

void srslte_cfo_set_tol(srslte_cfo_t *h, float tol) {
  h->tol = tol;
}

int srslte_cfo_realloc(srslte_cfo_t *h, uint32_t samples) {
  h->cur_cexp = realloc(h->cur_cexp, sizeof(cf_t) * samples); 
  if (!h->cur_cexp) {
    perror("realloc");
    return SRSLTE_ERROR;
  }
  srslte_cexptab_gen(&h->tab, h->cur_cexp, h->last_freq, samples);
  h->nsamples = samples;
  
  return SRSLTE_SUCCESS;
}

void srslte_cfo_correct(srslte_cfo_t *h, cf_t *input, cf_t *output, float freq) {
  if (fabs(h->last_freq - freq) > h->tol) {
    h->last_freq = freq;
    srslte_cexptab_gen(&h->tab, h->cur_cexp, h->last_freq, h->nsamples);
    DEBUG("CFO generating new table for frequency %.4fe-6\n", freq*1e6);
  }
  srslte_vec_prod_ccc(h->cur_cexp, input, output, h->nsamples);
}
