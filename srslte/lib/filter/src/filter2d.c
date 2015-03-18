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

#include "srslte/phy/utils/debug.h"

#include "srslte/phy/resampling/interp.h"
#include "srslte/phy/filter/filter2d.h"
#include "srslte/phy/utils/matrix.h"
#include "srslte/phy/utils/vector.h"
#include "srslte/phy/utils/debug.h"

/* Useful macros */
#define intceil(X, Y)  ((X-1)/Y+1)

#define idx(a, b) ((a)*(q->szfreq+q->nfreq)+b)

int filter2d_init(filter2d_t* q, float **taps, uint32_t ntime, uint32_t nfreq, uint32_t sztime,
    uint32_t szfreq) {

  int ret = -1;
  bzero(q, sizeof(filter2d_t));

  if (matrix_init((void***)&q->taps, ntime, nfreq, sizeof(float))) {
    goto free_and_exit;
  }

  matrix_copy((void**) q->taps, (void**) taps, ntime, nfreq, sizeof(float));

  q->output = vec_malloc((ntime+sztime)*(szfreq+nfreq)*sizeof(cf_t));
  if (!q->output) {
    goto free_and_exit;
  }

  bzero(q->output, (ntime+sztime)*(szfreq+nfreq)*sizeof(cf_t));

  q->nfreq = nfreq;
  q->ntime = ntime;
  q->szfreq = szfreq;
  q->sztime = sztime;
  
  q->norm = 0.0;
  for (int i = 0; i < ntime; i++) {
    for (int j = 0; j < nfreq; j++) {
      q->norm += q->taps[i][j];
    }
  }
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

int filter2d_init_ones(filter2d_t* q, uint32_t ntime, uint32_t nfreq, uint32_t sztime,
    uint32_t szfreq) 
{

  int i, j;
  float **taps;
  int ret = -1;

  if (matrix_init((void***) &taps, ntime, nfreq, sizeof(float))) {
    goto free_and_exit;
  }
  /* Compute the default 2-D interpolation mesh */
  for (i = 0; i < ntime; i++) {
    for (j = 0; j < nfreq; j++) {
      taps[i][j] = 1.0/(i+1);
    }
  }

  INFO("Using all-ones interpolation matrix of size %dx%d\n", ntime, nfreq);
  if (verbose >= VERBOSE_INFO) {
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
void filter2d_step(filter2d_t *q) {
  int i;

  for (i = 0; i < q->ntime; i++) {
    memcpy(&q->output[idx(i,0)], &q->output[idx(q->sztime + i,0)],
        sizeof(cf_t) * (q->szfreq+q->nfreq));
  }
  
  for (; i < q->ntime + q->sztime; i++) {
    memset(&q->output[idx(i,0)], 0, sizeof(cf_t) * (q->szfreq+q->nfreq));
  }
}

void filter2d_reset(filter2d_t *q) {
    bzero(q->output, (q->ntime+q->sztime)*(q->szfreq+q->nfreq)*sizeof(cf_t));
}

/** Adds samples x to the from the given time/freq indexes to the filter
 * and computes the output.
 */
void filter2d_add(filter2d_t *q, cf_t x, uint32_t time_idx, uint32_t freq_idx) {
  int i, j;

  uint32_t ntime = q->ntime;
  uint32_t nfreq = q->nfreq;
  
  if (freq_idx < q->szfreq && time_idx < q->sztime) {
    DEBUG("Adding %f+%fi to %d:%d\n",__real__ x,__imag__ x,time_idx,freq_idx);

    for (i = 0; i < ntime; i++) {
      for (j = 0; j < nfreq; j++) {
        q->output[idx(i+time_idx, j+freq_idx)] += x * (cf_t)(q->taps[i][j])/q->norm;
      }
    }    
  }
}

void filter2d_add_out(filter2d_t *q, cf_t x, int time_idx, int freq_idx) {
  int i, j;

  uint32_t ntime = q->ntime;
  uint32_t nfreq = q->nfreq;
  float norm=0; 
  for (i = 0; i < ntime; i++) {
    for (j = 0; j < nfreq; j++) {
      if (i+time_idx >= 0                 && j+freq_idx >= 0                    && 
          i+time_idx < q->ntime+q->sztime && j+freq_idx < q->nfreq + q->szfreq)
      {
        norm += q->taps[i][j];
      }
    }
  }  
  for (i = 0; i < ntime; i++) {
    for (j = 0; j < nfreq; j++) {
      if (i+time_idx >= 0                 && j+freq_idx >= 0                    && 
          i+time_idx < q->ntime+q->sztime && j+freq_idx < q->nfreq + q->szfreq)
      {
        q->output[idx(i+time_idx, j+freq_idx)] += x * (cf_t)(q->taps[i][j])/q->norm;        
      }
    }
  }    
}

void filter2d_get_symbol(filter2d_t *q, uint32_t nsymbol, cf_t *output) {
  memcpy(output, &q->output[idx(nsymbol,q->nfreq/2)], sizeof(cf_t) * (q->szfreq));
}
