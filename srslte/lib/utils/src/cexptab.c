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

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <complex.h>

#include "srslte/utils/cexptab.h"

int srslte_cexptab_init(srslte_cexptab_t *h, uint32_t size) {
  uint32_t i;

  h->size = size;
  h->tab = malloc(sizeof(cf_t) * size);
  if (h->tab) {
    for (i = 0; i < size; i++) {
      h->tab[i] = cexpf(_Complex_I * 2 * M_PI * (float) i / size);
    }
    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR;
  }
}

void srslte_cexptab_free(srslte_cexptab_t *h) {
  if (h->tab) {
    free(h->tab);
  }
  bzero(h, sizeof(srslte_cexptab_t));
}

void srslte_cexptab_gen(srslte_cexptab_t *h, cf_t *x, float freq, uint32_t len) {
  uint32_t i;
  uint32_t idx;
  float phase_inc = freq * h->size;
  float phase=0;

  for (i = 0; i < len; i++) {
    while (phase >= (float) h->size) {
      phase -= (float) h->size;
    }
    while (phase < 0) {
      phase += (float) h->size;
    }
    idx = (uint32_t) phase;
    x[i] = h->tab[idx];
    phase += phase_inc;

  }
}

void srslte_cexptab_gen_direct(cf_t *x, float freq, uint32_t len) {
  uint32_t i;
  for (i = 0; i < len; i++) {
    x[i] = cexpf(_Complex_I * 2 * M_PI * freq * i);
  }
}

