/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <strings.h>
#include <stdlib.h>
#include <math.h>

#include "lte/utils/cexptab.h"
#include "lte/sync/cfo.h"
#include "lte/utils/vector.h"
#include "lte/utils/debug.h"

int cfo_init(cfo_t *h, int nsamples) {
  int ret = -1;
  bzero(h, sizeof(cfo_t));

  if (cexptab_init(&h->tab, CFO_CEXPTAB_SIZE)) {
    goto clean;
  }
  h->cur_cexp = malloc(sizeof(cf_t) * nsamples);
  if (!h->cur_cexp) {
    goto clean;
  }
  h->tol = CFO_TOLERANCE;
  h->last_freq = 0;
  h->nsamples = nsamples;
  cexptab_gen(&h->tab, h->cur_cexp, h->last_freq, h->nsamples);

  ret = 0;
clean:
  if (ret == -1) {
    cfo_free(h);
  }
  return ret;
}

void cfo_free(cfo_t *h) {
  cexptab_free(&h->tab);
  if (h->cur_cexp) {
    free(h->cur_cexp);
  }
  bzero(h, sizeof(cf_t));
}

void cfo_set_tol(cfo_t *h, float tol) {
  h->tol = tol;
}

void cfo_correct(cfo_t *h, cf_t *x, float freq) {
  if (fabs(h->last_freq - freq) > h->tol) {
    h->last_freq = freq;
    cexptab_gen(&h->tab, h->cur_cexp, h->last_freq, h->nsamples);
    INFO("CFO generating new table for frequency %.4f\n", freq);
  }
  vec_prod_ccc(h->cur_cexp, x, x, h->nsamples);
}
