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


#include <string.h>
#include <strings.h>
#include <stdlib.h>

#include "lte/utils/debug.h"

#include "lte/filter/filter2d.h"
#include "lte/utils/matrix.h"
#include "lte/utils/vector.h"
#include "lte/utils/debug.h"

/* Useful macros */
#define intceil(X, Y)  ((X-1)/Y+1)

#define idx(a, b) ((a)*(q->szfreq)+b)

int filter2d_init(filter2d_t* q, float **taps, int ntime, int nfreq, int sztime,
    int szfreq) {

  int ret = -1;
  bzero(q, sizeof(filter2d_t));

  if (matrix_init((void***)&q->taps, ntime, nfreq, sizeof(float))) {
    goto free_and_exit;
  }

  matrix_copy((void**) q->taps, (void**) taps, ntime, nfreq, sizeof(float));

  q->output = vec_malloc((ntime+sztime)*(szfreq)*sizeof(cf_t));
  if (!q->output) {
    goto free_and_exit;
  }

  bzero(q->output, (ntime+sztime)*(szfreq)*sizeof(cf_t));

  q->nfreq = nfreq;
  q->ntime = ntime;
  q->szfreq = szfreq;
  q->sztime = sztime;

  ret = 0;

  free_and_exit: if (ret == -1) {
    filter2d_free(q);
  }
  return ret;
}

void filter2d_free(filter2d_t *q) {

  matrix_free((void**) q->taps, q->ntime);
  if (q->output) {
    free(q->output);
  }
  bzero(q, sizeof(filter2d_t));
}

int filter2d_init_default(filter2d_t* q, int ntime, int nfreq, int sztime,
    int szfreq) {

  int i, j;
  int ret = -1;
  float **taps;

  if (matrix_init((void***) &taps, ntime, nfreq, sizeof(float))) {
    goto free_and_exit;
  }

  /* Compute the default 2-D interpolation mesh */
  for (i = 0; i < ntime; i++) {
    for (j = 0; j < nfreq; j++) {
      if (j < nfreq / 2)
        taps[i][j] = (j + 1.0) / (2.0 * intceil(nfreq, 2));

      else if (j == nfreq / 2)
        taps[i][j] = 0.5;

      else if (j > nfreq / 2)
        taps[i][j] = (nfreq - j) / (2.0 * intceil(nfreq, 2));
    }
  }

  INFO("Using default interpolation matrix of size %dx%d\n", ntime, nfreq);
  if (verbose >= VERBOSE_DEBUG) {
    matrix_fprintf_f(stdout, taps, ntime, nfreq);
  }

  if (filter2d_init(q, taps, ntime, nfreq, sztime, szfreq)) {
    goto free_and_exit;
  }

  ret = 0;
free_and_exit:
  matrix_free((void**) taps, ntime);
  return ret;
}

/* Moves the last ntime symbols to the start and clears the remaining of the output.
 * Should be called, for instance, before filtering each OFDM frame.
 */
void filter2d_reset(filter2d_t *q) {
  int i;

  for (i = 0; i < q->ntime; i++) {
    memcpy(&q->output[idx(i,0)], &q->output[idx(q->sztime + i,0)],
        sizeof(cf_t) * (q->szfreq));
  }
  for (; i < q->ntime + q->sztime; i++) {
    memset(&q->output[idx(i,0)], 0, sizeof(cf_t) * (q->szfreq));
  }
}

/** Adds samples x to the from the given time/freq indexes to the filter
 * and computes the output.
 */
void filter2d_add(filter2d_t *q, cf_t x, int time_idx, int freq_idx) {
  int i, j;

  int ntime = q->ntime;
  int nfreq = q->nfreq;

  for (i = 0; i < ntime; i++) {
    for (j = 0; j < nfreq; j++) {
      q->output[idx(i+time_idx, j+freq_idx - nfreq/2)] += x * (cf_t)(q->taps[i][j]);
    }
  }
}
