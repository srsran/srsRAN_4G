/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <math.h>

#include "srslte/fec/turbodecoder.h"
#include "srslte/utils/vector.h"

#define NUMSTATES       8
#define NINPUTS         2
#define TAIL            3
#define TOTALTAIL       12

#define INF 9e4
#define ZERO 9e-4

/************************************************
 *
 *  MAP_GEN is the MAX-LOG-MAP generic implementation of the
 *  Decoder
 *
 ************************************************/
void srslte_map_gen_beta(srslte_map_gen_t * s, srslte_llr_t * input, srslte_llr_t * parity,
                  uint32_t long_cb)
{
  srslte_llr_t m_b[8], new[8], old[8];
  srslte_llr_t x, y, xy;
  int k;
  uint32_t end = long_cb + SRSLTE_TCOD_RATE;
  srslte_llr_t *beta = s->beta;
  uint32_t i;

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

void srslte_map_gen_alpha(srslte_map_gen_t * s, srslte_llr_t * input, srslte_llr_t * parity, srslte_llr_t * output,
                   uint32_t long_cb)
{
  srslte_llr_t m_b[8], new[8], old[8], max1[8], max0[8];
  srslte_llr_t m1, m0;
  srslte_llr_t x, y, xy;
  srslte_llr_t out;
  uint32_t k;
  uint32_t end = long_cb;
  srslte_llr_t *beta = s->beta;
  uint32_t i;

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

int srslte_map_gen_init(srslte_map_gen_t * h, int max_long_cb)
{
  bzero(h, sizeof(srslte_map_gen_t));
  h->beta = srslte_vec_malloc(sizeof(srslte_llr_t) * (max_long_cb + SRSLTE_TCOD_TOTALTAIL + 1) * NUMSTATES);
  if (!h->beta) {
    perror("srslte_vec_malloc");
    return -1;
  }
  h->max_long_cb = max_long_cb;
  return 0;
}

void srslte_map_gen_free(srslte_map_gen_t * h)
{
  if (h->beta) {
    free(h->beta);
  }
  bzero(h, sizeof(srslte_map_gen_t));
}

void srslte_map_gen_dec(srslte_map_gen_t * h, srslte_llr_t * input, srslte_llr_t * parity, srslte_llr_t * output,
                 uint32_t long_cb)
{
  uint32_t k;

  h->beta[(long_cb + TAIL) * NUMSTATES] = 0;
  for (k = 1; k < NUMSTATES; k++)
    h->beta[(long_cb + TAIL) * NUMSTATES + k] = -INF;

  srslte_map_gen_beta(h, input, parity, long_cb);
  srslte_map_gen_alpha(h, input, parity, output, long_cb);
}

/************************************************
 *
 *  TURBO DECODER INTERFACE
 *
 ************************************************/
int srslte_tdec_init(srslte_tdec_t * h, uint32_t max_long_cb)
{
  int ret = -1;
  bzero(h, sizeof(srslte_tdec_t));
  uint32_t len = max_long_cb + SRSLTE_TCOD_TOTALTAIL;

  h->max_long_cb = max_long_cb;

  h->llr1 = srslte_vec_malloc(sizeof(srslte_llr_t) * len);
  if (!h->llr1) {
    perror("srslte_vec_malloc");
    goto clean_and_exit;
  }
  h->llr2 = srslte_vec_malloc(sizeof(srslte_llr_t) * len);
  if (!h->llr2) {
    perror("srslte_vec_malloc");
    goto clean_and_exit;
  }
  h->w = srslte_vec_malloc(sizeof(srslte_llr_t) * len);
  if (!h->w) {
    perror("srslte_vec_malloc");
    goto clean_and_exit;
  }
  h->syst = srslte_vec_malloc(sizeof(srslte_llr_t) * len);
  if (!h->syst) {
    perror("srslte_vec_malloc");
    goto clean_and_exit;
  }
  h->parity = srslte_vec_malloc(sizeof(srslte_llr_t) * len);
  if (!h->parity) {
    perror("srslte_vec_malloc");
    goto clean_and_exit;
  }

  if (srslte_map_gen_init(&h->dec, h->max_long_cb)) {
    goto clean_and_exit;
  }

  if (srslte_tc_interl_init(&h->interleaver, h->max_long_cb) < 0) {
    goto clean_and_exit;
  }

  ret = 0;
clean_and_exit:if (ret == -1) {
    srslte_tdec_free(h);
  }
  return ret;
}

void srslte_tdec_free(srslte_tdec_t * h)
{
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

  srslte_map_gen_free(&h->dec);

  srslte_tc_interl_free(&h->interleaver);

  bzero(h, sizeof(srslte_tdec_t));
}

void srslte_tdec_iteration(srslte_tdec_t * h, srslte_llr_t * input, uint32_t long_cb)
{
  uint32_t i;

  // Prepare systematic and parity bits for MAP DEC #1
  for (i = 0; i < long_cb; i++) {
    h->syst[i] = input[SRSLTE_TCOD_RATE * i] + h->w[i];
    h->parity[i] = input[SRSLTE_TCOD_RATE * i + 1];
  }
  for (i = long_cb; i < long_cb + SRSLTE_TCOD_RATE; i++) {
    h->syst[i] = input[SRSLTE_TCOD_RATE * long_cb + NINPUTS * (i - long_cb)];
    h->parity[i] = input[SRSLTE_TCOD_RATE * long_cb + NINPUTS * (i - long_cb) + 1];
  }

  // Run MAP DEC #1
  srslte_map_gen_dec(&h->dec, h->syst, h->parity, h->llr1, long_cb);

  // Prepare systematic and parity bits for MAP DEC #1
  for (i = 0; i < long_cb; i++) {
    h->syst[i] = h->llr1[h->interleaver.forward[i]]
      - h->w[h->interleaver.forward[i]];
    h->parity[i] = input[SRSLTE_TCOD_RATE * i + 2];
  }
  for (i = long_cb; i < long_cb + SRSLTE_TCOD_RATE; i++) {
    h->syst[i] =
      input[SRSLTE_TCOD_RATE * long_cb + NINPUTS * SRSLTE_TCOD_RATE + NINPUTS * (i - long_cb)];
    h->parity[i] = input[SRSLTE_TCOD_RATE * long_cb + NINPUTS * SRSLTE_TCOD_RATE
                         + NINPUTS * (i - long_cb) + 1];
  }

  // Run MAP DEC #1
  srslte_map_gen_dec(&h->dec, h->syst, h->parity, h->llr2, long_cb);
 
  // Update a-priori LLR from the last iteration
  for (i = 0; i < long_cb; i++) {
    h->w[i] += h->llr2[h->interleaver.reverse[i]] - h->llr1[i];
  }

}

int srslte_tdec_reset(srslte_tdec_t * h, uint32_t long_cb)
{
  if (long_cb > h->max_long_cb) {
    fprintf(stderr, "TDEC was initialized for max_long_cb=%d\n",
            h->max_long_cb);
    return -1;
  }
  memset(h->w, 0, sizeof(srslte_llr_t) * long_cb);
  return srslte_tc_interl_LTE_gen(&h->interleaver, long_cb);
}

void srslte_tdec_decision(srslte_tdec_t * h, uint8_t *output, uint32_t long_cb)
{
  uint32_t i;
  for (i = 0; i < long_cb; i++) {
    output[i] = (h->llr2[h->interleaver.reverse[i]] > 0) ? 1 : 0;    
  }
}

int srslte_tdec_run_all(srslte_tdec_t * h, srslte_llr_t * input, uint8_t *output,
                  uint32_t nof_iterations, uint32_t long_cb)
{
  uint32_t iter = 0;

  if (srslte_tdec_reset(h, long_cb)) {
    return SRSLTE_ERROR; 
  }

  do {
    srslte_tdec_iteration(h, input, long_cb);
    iter++;
  } while (iter < nof_iterations);

  srslte_tdec_decision(h, output, long_cb);
  
  return SRSLTE_SUCCESS;
}
