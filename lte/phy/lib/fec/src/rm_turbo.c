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
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#include "liblte/phy/fec/rm_turbo.h"

#define NCOLS 32
#define NROWS_MAX NCOLS
#define RATE 3

unsigned char RM_PERM_TC[NCOLS] =
    { 0, 16, 8, 24, 4, 20, 12, 28, 2, 18, 10, 26, 6, 22, 14, 30, 1, 17, 9,
        25, 5, 21, 13, 29, 3, 19, 11, 27, 7, 23, 15, 31 };

int rm_turbo_init(rm_turbo_t *q, int buffer_len) {
  q->buffer_len = buffer_len;
  q->buffer = malloc(buffer_len * sizeof(float));
  if (!q->buffer) {
    perror("malloc");
    return -1;
  }
  q->d2_perm = malloc(buffer_len * sizeof(int) / 3 + 1);
  if (!q->d2_perm) {
    perror("malloc");
    return -1;
  }
  return 0;
}

void rm_turbo_free(rm_turbo_t *q) {
  if (q->buffer) {
    free(q->buffer);
  }
}

/* Turbo Code Rate Matching.
 * 3GPP TS 36.212 v10.1.0 section 5.1.4.1
 *
 * TODO: Soft buffer size limitation according to UE category
 */
int rm_turbo_tx(rm_turbo_t *q, char *input, int in_len, char *output, int out_len, int rv_idx) {

  char *tmp = (char*) q->buffer;
  int nrows, ndummy, K_p;

  int i, j, k, s, kidx, N_cb, k0;

  nrows = (int) (in_len / RATE - 1) / NCOLS + 1;
  K_p = nrows * NCOLS;
  if (3 * K_p > q->buffer_len) {
    fprintf(stderr,
        "Input too large. Max input length including dummy bits is %d\n",
        q->buffer_len);
    return -1;
  }

  ndummy = K_p - in_len / RATE;
  if (ndummy < 0) {
    ndummy = 0;
  }

  /* Sub-block interleaver (5.1.4.1.1) and bit collection */
  k = 0;
  for (s = 0; s < 2; s++) {
    for (j = 0; j < NCOLS; j++) {
      for (i = 0; i < nrows; i++) {
        if (s == 0) {
          kidx = k%K_p;
        } else {
          kidx = K_p + 2 * (k%K_p);
        }
        if (i * NCOLS + RM_PERM_TC[j] < ndummy) {
          tmp[kidx] = TX_NULL;
        } else {
          tmp[kidx] = input[(i * NCOLS + RM_PERM_TC[j] - ndummy) * 3 + s];
        }
        k++;
      }
    }
  }

  // d_k^(2) goes through special permutation
  for (k = 0; k < K_p; k++) {
    kidx = (RM_PERM_TC[k / nrows] + NCOLS * (k % nrows) + 1) % K_p;
    if ((kidx - ndummy) < 0) {
      tmp[K_p + 2 * k + 1] = TX_NULL;
    } else {
      tmp[K_p + 2 * k + 1] = input[3 * (kidx - ndummy) + 2];
    }
  }

  /* Bit selection and transmission 5.1.4.1.2 */
  N_cb = 3 * K_p;  // TODO: Soft buffer size limitation

  k0 = nrows * (2 * (int) ceilf((float) N_cb / (float) (8 * nrows))
              * rv_idx + 2);
  k = 0;
  j = 0;

  while (k < out_len) {
    if (tmp[(k0 + j) % N_cb] != TX_NULL) {
      output[k] = tmp[(k0 + j) % N_cb];
      k++;
    }
    j++;
  }
  return 0;
}

/* Undoes Turbo Code Rate Matching.
 * 3GPP TS 36.212 v10.1.0 section 5.1.4.1
 */
int rm_turbo_rx(rm_turbo_t *q, float *input, int in_len, float *output, int out_len, int rv_idx) {

  int nrows, ndummy, K_p, k0, N_cb, jp, kidx;
  int i, j, k;
  int d_i, d_j;
  bool isdummy;

  float *tmp = (float*) q->buffer;

  nrows = (int) (out_len / RATE - 1) / NCOLS + 1;
  K_p = nrows * NCOLS;
  if (3 * K_p > q->buffer_len) {
    fprintf(stderr,
        "Input too large. Max input length including dummy bits is %d\n",
        q->buffer_len);
    return -1;
  }

  ndummy = K_p - out_len / RATE;
  if (ndummy < 0) {
    ndummy = 0;
  }

  for (i = 0; i < RATE * K_p; i++) {
    tmp[i] = RX_NULL;
  }

  /* Undo bit collection. Account for dummy bits */
  N_cb = 3 * K_p;  // TODO: Soft buffer size limitation
  k0 = nrows * (2 * (int) ceilf((float) N_cb / (float) (8 * nrows))
              * rv_idx + 2);
  k = 0;
  j = 0;
  while (k < in_len) {
    jp = (k0 + j) % N_cb;

    if (jp == 32 || jp == 95 || jp == 0) {
      i=0;
    }

    if (jp < K_p || !(jp%2)) {
      if (jp >= K_p) {
        d_i = ((jp-K_p) / 2) / nrows;
        d_j = ((jp-K_p) / 2) % nrows;
      } else {
        d_i = jp / nrows;
        d_j = jp % nrows;
      }
      if (d_j * NCOLS + RM_PERM_TC[d_i] >= ndummy) {
        isdummy = false;
      } else {
        isdummy = true;
      }
    } else {
      int jpp = (jp-K_p-1)/2;
      kidx = (RM_PERM_TC[jpp / nrows] + NCOLS * (jpp % nrows) + 1) % K_p;
      q->d2_perm[kidx] = jpp; // save the permutation in a temporary buffer
      if ((kidx - ndummy) < 0) {
        isdummy = true;
      } else {
        isdummy = false;
      }
    }

    if (!isdummy) {
      if (tmp[jp] == RX_NULL) {
        tmp[jp] = input[k];
      } else if (input[k] != RX_NULL) {
        tmp[jp] += input[k]; /* soft combine LLRs */
      }
      k++;
    }
    j++;
  }

  /* interleaving and bit selection */
  for (i = 0; i < out_len / RATE; i++) {
    d_i = (i + ndummy) / NCOLS;
    d_j = (i + ndummy) % NCOLS;
    for (j = 0; j < RATE; j++) {
      if (j != 2) {
        kidx = K_p * j + (j+1)*(RM_PERM_TC[d_j] * nrows + d_i);
      } else {
        // use the saved permuatation function to avoid computing the inverse
        kidx = 2*q->d2_perm[(i+ndummy)%K_p]+K_p+1;
      }
      float o = tmp[kidx];
      if (o != RX_NULL) {
        output[i * RATE + j] = o;
      } else {
        output[i * RATE + j] = 0;
      }
    }
  }
  return 0;
}


/** High-level API */

int rm_turbo_initialize(rm_turbo_hl* h) {
  return rm_turbo_init(&h->q, 7000);
}

/** This function can be called in a subframe (1ms) basis */
int rm_turbo_work(rm_turbo_hl* hl) {
  if (hl->init.direction) {
    rm_turbo_tx(&hl->q, hl->input, hl->in_len, hl->output, hl->ctrl_in.E, hl->ctrl_in.rv_idx);
    hl->out_len = hl->ctrl_in.E;
  } else {
    rm_turbo_rx(&hl->q, hl->input, hl->in_len, hl->output, hl->ctrl_in.S, hl->ctrl_in.rv_idx);
    hl->out_len = hl->ctrl_in.S;
  }
  return 0;
}

int rm_turbo_stop(rm_turbo_hl* hl) {
  rm_turbo_free(&hl->q);
  return 0;
}

