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

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <stdint.h>

#include "srslte/fec/tc_interl.h"
#include "srslte/fec/turbocoder.h"

#define TURBO_SRSLTE_TCOD_RATE 	3

uint32_t mcd(uint32_t x, uint32_t y);

/************************************************
 *
 *  UMTS TURBO CODE INTERLEAVER
 *
 ************************************************/

#define MAX_ROWS 	20
#define MAX_COLS 	256

const unsigned short table_p[52] = { 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43,
    47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127,
    131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199,
    211, 223, 227, 229, 233, 239, 241, 251, 257 };
const uint8_t table_v[52] = { 3, 2, 2, 3, 2, 5, 2, 3, 2, 6, 3, 5, 2, 2, 2,
    2, 7, 5, 3, 2, 3, 5, 2, 5, 2, 6, 3, 3, 2, 3, 2, 2, 6, 5, 2, 5, 2, 2, 2, 19,
    5, 2, 3, 2, 3, 2, 6, 3, 7, 7, 6, 3 };

int srslte_tc_interl_init(srslte_tc_interl_t *h, uint32_t max_long_cb) {
  int ret = -1;
  h->forward = malloc(sizeof(uint32_t) * max_long_cb);
  if (!h->forward) {
    perror("malloc");
    goto clean_exit;
  }
  h->reverse = malloc(sizeof(uint32_t) * max_long_cb);
  if (!h->reverse) {
    perror("malloc");
    goto clean_exit;
  }
  h->max_long_cb = max_long_cb;
  ret = 0;
  clean_exit: if (ret == -1) {
    srslte_tc_interl_free(h);
  }
  return ret;
}

void srslte_tc_interl_free(srslte_tc_interl_t *h) {
  if (h->forward) {
    free(h->forward);
  }
  if (h->reverse) {
    free(h->reverse);
  }
  bzero(h, sizeof(srslte_tc_interl_t));
}

int srslte_tc_interl_UMTS_gen(srslte_tc_interl_t *h, uint32_t long_cb) {

  uint32_t i, j;
  uint32_t res, prim, aux;
  uint32_t kp, k;
  uint16_t *per, *desper;
  uint8_t v;
  uint16_t p;
  uint16_t s[MAX_COLS], q[MAX_ROWS], r[MAX_ROWS], T[MAX_ROWS];
  uint16_t U[MAX_COLS * MAX_ROWS];
  uint32_t M_Rows, M_Cols, M_long;

  M_long = long_cb;

  if (long_cb > h->max_long_cb) {
    fprintf(stderr, "Interleaver initiated for max_long_cb=%d\n",
        h->max_long_cb);
    return -1;
  }

  /* Find R*/
  if ((40 <= M_long) && (M_long <= 159))
    M_Rows = 5;
  else if (((160 <= M_long) && (M_long <= 200))
      || ((481 <= M_long) && (M_long <= 530)))
    M_Rows = 10;
  else
    M_Rows = 20;

  /* Find p i v*/
  if ((481 <= M_long) && (M_long <= 530)) {
    p = 53;
    v = 2;
    M_Cols = p;
  } else {
    i = 0;
    do {
      p = table_p[i];
      v = table_v[i];
      i++;
    } while (M_long > (M_Rows * (p + 1)));

  }

  /* Find C*/
  if ((M_long) <= (M_Rows) * ((p) - 1))
    M_Cols = (p) - 1;
  else if (((M_Rows) * (p - 1) < M_long) && (M_long <= (M_Rows) * (p)))
    M_Cols = p;
  else if ((M_Rows) * (p) < M_long)
    M_Cols = (p) + 1;

  q[0] = 1;
  prim = 6;

  for (i = 1; i < M_Rows; i++) {
    do {
      prim++;
      res = mcd(prim, p - 1);
    } while (res != 1);
    q[i] = prim;
  }

  s[0] = 1;
  for (i = 1; i < p - 1; i++) {
    s[i] = (v * s[i - 1]) % p;
  }

  if (M_long <= 159 && M_long >= 40) {
    T[0] = 4;
    T[1] = 3;
    T[2] = 2;
    T[3] = 1;
    T[4] = 0;
  } else if ((M_long <= 200 && M_long >= 160)
      || (M_long <= 530 && M_long >= 481)) {
    T[0] = 9;
    T[1] = 8;
    T[2] = 7;
    T[3] = 6;
    T[4] = 5;
    T[5] = 4;
    T[6] = 3;
    T[7] = 2;
    T[8] = 1;
    T[9] = 0;
  } else if ((M_long <= 2480 && M_long >= 2281)
      || (M_long <= 3210 && M_long >= 3161)) {
    T[0] = 19;
    T[1] = 9;
    T[2] = 14;
    T[3] = 4;
    T[4] = 0;
    T[5] = 2;
    T[6] = 5;
    T[7] = 7;
    T[8] = 12;
    T[9] = 18;
    T[10] = 16;
    T[11] = 13;
    T[12] = 17;
    T[13] = 15;
    T[14] = 3;
    T[15] = 1;
    T[16] = 6;
    T[17] = 11;
    T[18] = 8;
    T[19] = 10;
  } else {
    T[0] = 19;
    T[1] = 9;
    T[2] = 14;
    T[3] = 4;
    T[4] = 0;
    T[5] = 2;
    T[6] = 5;
    T[7] = 7;
    T[8] = 12;
    T[9] = 18;
    T[10] = 10;
    T[11] = 8;
    T[12] = 13;
    T[13] = 17;
    T[14] = 3;
    T[15] = 1;
    T[16] = 16;
    T[17] = 6;
    T[18] = 15;
    T[19] = 11;
  }

  for (i = 0; i < M_Rows; i++) {
    r[T[i]] = q[i];
  }

  for (i = 0; i < M_Rows; i++) {
    for (j = 0; j < p - 1; j++) {
      U[i * M_Cols + j] = s[(j * r[i]) % (p - 1)];
      if (M_Cols == (p - 1))
        U[i * M_Cols + j] -= 1;
    }
  }

  if (M_Cols == p) {
    for (i = 0; i < M_Rows; i++)
      U[i * M_Cols + p - 1] = 0;
  } else if (M_Cols == p + 1) {
    for (i = 0; i < M_Rows; i++) {
      U[i * M_Cols + p - 1] = 0;
      U[i * M_Cols + p] = p;
    }
    if (M_long == M_Cols * M_Rows) {
      aux = U[(M_Rows - 1) * M_Cols + p];
      U[(M_Rows - 1) * M_Cols + p] = U[(M_Rows - 1) * M_Cols + 0];
      U[(M_Rows - 1) * M_Cols + 0] = aux;
    }
  }

  per = h->forward;
  desper = h->reverse;

  k = 0;
  for (j = 0; j < M_Cols; j++) {
    for (i = 0; i < M_Rows; i++) {
      kp = T[i] * M_Cols + U[i * M_Cols + j];
      if (kp < M_long) {
        desper[kp] = k;
        per[k] = kp;
        k++;
      }
    }
  }

  return 0;

}

uint32_t mcd(uint32_t x, uint32_t y) {
  uint32_t r = 1;

  while (r) {
    r = x % y;
    x = y;
    y = r;
  }
  return x;
}
