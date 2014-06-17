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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "lte/fec/turbodecoder.h"

/************************************************
 *
 *  MAP_GEN is the MAX-LOG-MAP generic implementation of the
 *  Decoder
 *
 ************************************************/
void map_gen_beta(map_gen_t *s, llr_t *input, llr_t *parity, int long_cb) {
  llr_t m_b[8], new[8], old[8];
  llr_t x, y, xy;
  int k;
  int end = long_cb + RATE;
  llr_t *beta = s->beta;
  int i;

  for (i = 0; i < 8; i++) {
    old[i] = beta[8 * (end) + i];
  }

  for (k = end - 1; k >= 0; k--) {
    x = input[k];
    y = parity[k];

    xy = x + y;

    m_b[0] = old[4] + xy;
    m_b[1] = old[4];
    m_b[2] = old[5] + y;
    m_b[3] = old[5] + x;
    m_b[4] = old[6] + x;
    m_b[5] = old[6] + y;
    m_b[6] = old[7];
    m_b[7] = old[7] + xy;

    new[0] = old[0];
    new[1] = old[0] + xy;
    new[2] = old[1] + x;
    new[3] = old[1] + y;
    new[4] = old[2] + y;
    new[5] = old[2] + x;
    new[6] = old[3] + xy;
    new[7] = old[3];

    for (i = 0; i < 8; i++) {
      if (m_b[i] > new[i])
        new[i] = m_b[i];
      beta[8 * k + i] = new[i];
      old[i] = new[i];
    }
  }
}

void map_gen_alpha(map_gen_t *s, llr_t *input, llr_t *parity, llr_t *output,
    int long_cb) {
  llr_t m_b[8], new[8], old[8], max1[8], max0[8];
  llr_t m1, m0;
  llr_t x, y, xy;
  llr_t out;
  int k;
  int end = long_cb;
  llr_t *beta = s->beta;
  int i;

  old[0] = 0;
  for (i = 1; i < 8; i++) {
    old[i] = -INF;
  }

  for (k = 1; k < end + 1; k++) {
    x = input[k - 1];
    y = parity[k - 1];

    xy = x + y;

    m_b[0] = old[0];
    m_b[1] = old[3] + y;
    m_b[2] = old[4] + y;
    m_b[3] = old[7];
    m_b[4] = old[1];
    m_b[5] = old[2] + y;
    m_b[6] = old[5] + y;
    m_b[7] = old[6];

    new[0] = old[1] + xy;
    new[1] = old[2] + x;
    new[2] = old[5] + x;
    new[3] = old[6] + xy;
    new[4] = old[0] + xy;
    new[5] = old[3] + x;
    new[6] = old[4] + x;
    new[7] = old[7] + xy;

    for (i = 0; i < 8; i++) {
      max0[i] = m_b[i] + beta[8 * k + i];
      max1[i] = new[i] + beta[8 * k + i];
    }

    m1 = max1[0];
    m0 = max0[0];

    for (i = 1; i < 8; i++) {
      if (max1[i] > m1)
        m1 = max1[i];
      if (max0[i] > m0)
        m0 = max0[i];
    }

    for (i = 0; i < 8; i++) {
      if (m_b[i] > new[i])
        new[i] = m_b[i];
      old[i] = new[i];
    }

    out = m1 - m0;
    output[k - 1] = out;
  }
}

int map_gen_init(map_gen_t *h, int max_long_cb) {
  bzero(h, sizeof(map_gen_t));
  h->beta = malloc(sizeof(llr_t) * (max_long_cb + TOTALTAIL + 1) * NUMSTATES);
  if (!h->beta) {
    perror("malloc");
    return -1;
  }
  h->max_long_cb = max_long_cb;
  return 0;
}

void map_gen_free(map_gen_t *h) {
  if (h->beta) {
    free(h->beta);
  }
  bzero(h, sizeof(map_gen_t));
}

void map_gen_dec(map_gen_t *h, llr_t *input, llr_t *parity, llr_t *output,
    int long_cb) {
  int k;

  h->beta[(long_cb + TAIL) * NUMSTATES] = 0;
  for (k = 1; k < NUMSTATES; k++)
    h->beta[(long_cb + TAIL) * NUMSTATES + k] = -INF;

  map_gen_beta(h, input, parity, long_cb);
  map_gen_alpha(h, input, parity, output, long_cb);
}

/************************************************
 *
 *  TURBO DECODER INTERFACE
 *
 ************************************************/
int tdec_init(tdec_t *h, int max_long_cb) {
  int ret = -1;
  bzero(h, sizeof(tdec_t));
  int len = max_long_cb + TOTALTAIL;

  h->max_long_cb = max_long_cb;

  h->llr1 = malloc(sizeof(llr_t) * len);
  if (!h->llr1) {
    perror("malloc");
    goto clean_and_exit;
  }
  h->llr2 = malloc(sizeof(llr_t) * len);
  if (!h->llr2) {
    perror("malloc");
    goto clean_and_exit;
  }
  h->w = malloc(sizeof(llr_t) * len);
  if (!h->w) {
    perror("malloc");
    goto clean_and_exit;
  }
  h->syst = malloc(sizeof(llr_t) * len);
  if (!h->syst) {
    perror("malloc");
    goto clean_and_exit;
  }
  h->parity = malloc(sizeof(llr_t) * len);
  if (!h->parity) {
    perror("malloc");
    goto clean_and_exit;
  }

  if (map_gen_init(&h->dec, h->max_long_cb)) {
    goto clean_and_exit;
  }

  if (tc_interl_init(&h->interleaver, h->max_long_cb) < 0) {
    goto clean_and_exit;
  }

  ret = 0;
  clean_and_exit: if (ret == -1) {
    tdec_free(h);
  }
  return ret;
}

void tdec_free(tdec_t *h) {
  if (h->llr1) {
    free(h->llr1);
  }
  if (h->llr2) {
    free(h->llr2);
  }
  if (h->w) {
    free(h->w);
  }
  if (h->syst) {
    free(h->syst);
  }
  if (h->parity) {
    free(h->parity);
  }

  map_gen_free(&h->dec);

  tc_interl_free(&h->interleaver);

  bzero(h, sizeof(tdec_t));
}

void tdec_iteration(tdec_t *h, llr_t *input, int long_cb) {
  int i;

  // Prepare systematic and parity bits for MAP DEC #1
  for (i = 0; i < long_cb; i++) {
    h->syst[i] = input[RATE * i] + h->w[i];
    h->parity[i] = input[RATE * i + 1];
  }
  for (i = long_cb; i < long_cb + RATE; i++) {
    h->syst[i] = input[RATE * long_cb + NINPUTS * (i - long_cb)];
    h->parity[i] = input[RATE * long_cb + NINPUTS * (i - long_cb) + 1];
  }

  // Run MAP DEC #1
  map_gen_dec(&h->dec, h->syst, h->parity, h->llr1, long_cb);

  // Prepare systematic and parity bits for MAP DEC #1
  for (i = 0; i < long_cb; i++) {
    h->syst[i] = h->llr1[h->interleaver.forward[i]]
        - h->w[h->interleaver.forward[i]];
    h->parity[i] = input[RATE * i + 2];
  }
  for (i = long_cb; i < long_cb + RATE; i++) {
    h->syst[i] =
        input[RATE * long_cb + NINPUTS * RATE + NINPUTS * (i - long_cb)];
    h->parity[i] = input[RATE * long_cb + NINPUTS * RATE
        + NINPUTS * (i - long_cb) + 1];
  }

  // Run MAP DEC #1
  map_gen_dec(&h->dec, h->syst, h->parity, h->llr2, long_cb);

  // Update a-priori LLR from the last iteration
  for (i = 0; i < long_cb; i++) {
    h->w[i] += h->llr2[h->interleaver.reverse[i]] - h->llr1[i];
  }

}

int tdec_reset(tdec_t *h, int long_cb) {
  memset(h->w, 0, sizeof(llr_t) * long_cb);
  if (long_cb > h->max_long_cb) {
    fprintf(stderr, "TDEC was initialized for max_long_cb=%d\n",
        h->max_long_cb);
    return -1;
  }
  return tc_interl_LTE_gen(&h->interleaver, long_cb);
}

void tdec_decision(tdec_t *h, char *output, int long_cb) {
  int i;
  for (i = 0; i < long_cb; i++) {
    output[i] = (h->llr2[h->interleaver.reverse[i]] > 0) ? 1 : 0;
  }
}

void tdec_run_all(tdec_t *h, llr_t *input, char *output, int nof_iterations,
    int long_cb) {
  int iter = 0;

  tdec_reset(h, long_cb);

  do {
    tdec_iteration(h, input, long_cb);
    iter++;
  } while (iter < nof_iterations);

  tdec_decision(h, output, long_cb);
}

