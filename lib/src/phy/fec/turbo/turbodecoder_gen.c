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

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "srslte/phy/fec/turbodecoder_gen.h"
#include "srslte/phy/utils/vector.h"

#define NUMSTATES 8
#define NINPUTS 2
#define TAIL 3
#define TOTALTAIL 12

#define INF 10000

#define debug_enabled 0

#if debug_enabled
#define debug_state                                                                                                    \
  printf("k=%5d, in=%5d, pa=%3d, out=%5d, alpha=", k, x, parity[k - 1], out);                                          \
  srslte_vec_fprint_s(stdout, alpha, 8);                                                                               \
  printf(", beta=");                                                                                                   \
  srslte_vec_fprint_s(stdout, &beta[8 * (k)], 8);                                                                      \
  printf("\n");
#else
#define debug_state
#endif

/************************************************
 *
 *  MAP_GEN is the MAX-LOG-MAP generic implementation of the
 *  Decoder
 *
 ************************************************/
static void map_gen_beta(tdec_gen_t* s, int16_t* input, int16_t* app, int16_t* parity, uint32_t long_cb)
{
  int16_t  m_b[8], new[8], old[8];
  int16_t  x, y, xy;
  int      k;
  uint32_t end  = long_cb + SRSLTE_TCOD_RATE;
  int16_t* beta = s->beta;
  uint32_t i;

  for (i = 0; i < 8; i++) {
    old[i] = beta[8 * (end) + i];
  }

  for (k = end - 1; k >= 0; k--) {
    x = input[k];
    if (app && k < long_cb) {
      x += app[k];
    }
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
      old[i]          = new[i];
      beta[8 * k + i] = old[i];
    }

    if ((k % 4) == 0 && k < long_cb) {
      for (i = 1; i < 8; i++) {
        old[i] -= old[0];
      }
      old[0] = 0;
    }
  }
}

static void
map_gen_alpha(tdec_gen_t* s, int16_t* input, int16_t* app, int16_t* parity, int16_t* output, uint32_t long_cb)
{
  int16_t  m_b[8], new[8], old[8], max1[8], max0[8];
  int16_t  m1, m0;
  int16_t  x, y, xy;
  int16_t  out;
  uint32_t k;
  uint32_t end  = long_cb;
  int16_t* beta = s->beta;
  uint32_t i;

  old[0] = 0;
  for (i = 1; i < 8; i++) {
    old[i] = -INF;
  }

#if debug_enabled
  int16_t alpha[8];
#endif

  for (k = 1; k < end + 1; k++) {
    x = input[k - 1];
    if (app) {
      x += app[k - 1];
    }
    y = parity[k - 1];

    xy = x + y;

#if debug_enabled
    memcpy(alpha, old, sizeof(int16_t) * 8);
#endif

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

    if ((k % 4) == 0) {
      for (i = 1; i < 8; i++) {
        old[i] -= old[0];
      }
      old[0] = 0;
    }

    out           = m1 - m0;
    output[k - 1] = out;

    debug_state;
  }
}

int tdec_gen_init(void** hh, uint32_t max_long_cb)
{
  *hh = calloc(1, sizeof(tdec_gen_t));

  tdec_gen_t* h = (tdec_gen_t*)*hh;

  h->beta = srslte_vec_i16_malloc((max_long_cb + SRSLTE_TCOD_TOTALTAIL + 1) * NUMSTATES);
  if (!h->beta) {
    perror("srslte_vec_malloc");
    return -1;
  }
  h->max_long_cb = max_long_cb;
  return 1;
}

void tdec_gen_free(void* hh)
{
  tdec_gen_t* h = (tdec_gen_t*)hh;
  if (h) {
    if (h->beta) {
      free(h->beta);
    }
    free(h);
  }
}

void tdec_gen_dec(void* hh, int16_t* input, int16_t* app, int16_t* parity, int16_t* output, uint32_t long_cb)
{
  tdec_gen_t* h = (tdec_gen_t*)hh;

  h->beta[(long_cb + TAIL) * NUMSTATES] = 0;
  for (uint32_t k = 1; k < NUMSTATES; k++)
    h->beta[(long_cb + TAIL) * NUMSTATES + k] = -INF;

  map_gen_beta(h, input, app, parity, long_cb);
  map_gen_alpha(h, input, app, parity, output, long_cb);
}

void tdec_gen_extract_input(int16_t* input,
                            int16_t* syst,
                            int16_t* app2,
                            int16_t* parity0,
                            int16_t* parity1,
                            uint32_t long_cb)
{
  // Prepare systematic and parity bits for MAP DEC #1
  for (uint32_t i = 0; i < long_cb; i++) {
    syst[i]    = input[SRSLTE_TCOD_RATE * i];
    parity0[i] = input[SRSLTE_TCOD_RATE * i + 1];
    parity1[i] = input[SRSLTE_TCOD_RATE * i + 2];
  }
  for (uint32_t i = long_cb; i < long_cb + SRSLTE_TCOD_RATE; i++) {
    syst[i]    = input[SRSLTE_TCOD_RATE * long_cb + NINPUTS * (i - long_cb)];
    parity0[i] = input[SRSLTE_TCOD_RATE * long_cb + NINPUTS * (i - long_cb) + 1];

    app2[i]    = input[SRSLTE_TCOD_RATE * long_cb + NINPUTS * SRSLTE_TCOD_RATE + NINPUTS * (i - long_cb)];
    parity1[i] = input[SRSLTE_TCOD_RATE * long_cb + NINPUTS * SRSLTE_TCOD_RATE + NINPUTS * (i - long_cb) + 1];
  }
}

void tdec_gen_decision_byte(int16_t* app1, uint8_t* output, uint32_t long_cb)
{
  uint8_t mask[8] = {0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1};

  // long_cb is always byte aligned
  for (uint32_t i = 0; i < long_cb / 8; i++) {
    uint8_t out0 = app1[8 * i + 0] > 0 ? mask[0] : 0;
    uint8_t out1 = app1[8 * i + 1] > 0 ? mask[1] : 0;
    uint8_t out2 = app1[8 * i + 2] > 0 ? mask[2] : 0;
    uint8_t out3 = app1[8 * i + 3] > 0 ? mask[3] : 0;
    uint8_t out4 = app1[8 * i + 4] > 0 ? mask[4] : 0;
    uint8_t out5 = app1[8 * i + 5] > 0 ? mask[5] : 0;
    uint8_t out6 = app1[8 * i + 6] > 0 ? mask[6] : 0;
    uint8_t out7 = app1[8 * i + 7] > 0 ? mask[7] : 0;

    output[i] = out0 | out1 | out2 | out3 | out4 | out5 | out6 | out7;
  }
}
