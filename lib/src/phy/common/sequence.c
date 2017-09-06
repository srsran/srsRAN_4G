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
#include <stdio.h>
#include <strings.h>
#include <pthread.h>
#include <srslte/phy/common/sequence.h>

#include "srslte/phy/common/sequence.h"
#include "srslte/phy/utils/vector.h"
#include "srslte/phy/utils/bit.h"

#define Nc 1600

#define MAX_SEQ_LEN  (128*1024)

#define static_memory

/*
 * Pseudo Random Sequence generation.
 * It follows the 3GPP Release 8 (LTE) 36.211
 * Section 7.2
 */
#ifdef static_memory
static uint8_t x1[Nc+MAX_SEQ_LEN+31];
static uint8_t x2[Nc+MAX_SEQ_LEN+31];

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int srslte_sequence_set_LTE_pr(srslte_sequence_t *q, uint32_t len, uint32_t seed) {
  int n;

  if (len > q->max_len) {
    fprintf(stderr, "Error generating pseudo-random sequence: len %d exceeds maximum len %d\n",
            len, MAX_SEQ_LEN);
    return -1;
  }

  if (len > q->max_len) {
    fprintf(stderr, "Error generating pseudo-random sequence: len %d is greater than allocated len %d\n",
            len, q->max_len);
    return -1;
  }
  pthread_mutex_lock(&mutex);

  for (n = 0; n < 31; n++) {
    x2[n] = (seed >> n) & 0x1;
  }
  x1[0] = 1;

  for (n = 0; n < Nc + len; n++) {
    x1[n + 31] = (x1[n + 3] + x1[n]) & 0x1;
    x2[n + 31] = (x2[n + 3] + x2[n + 2] + x2[n+1] + x2[n]) & 0x1;
  }

  for (n = 0; n < len; n++) {
    q->c[n] = (x1[n + Nc] + x2[n + Nc]) & 0x1;
  }
  pthread_mutex_unlock(&mutex);

  return 0;
}

#else
int srslte_sequence_set_LTE_pr(srslte_sequence_t *q, uint32_t len, uint32_t seed) {
  int n;
  uint32_t *x1, *x2;

  if (len > q->max_len) {
    fprintf(stderr, "Error generating pseudo-random sequence: len %d is greater than allocated len %d\n",
            len, q->max_len);
    return -1;
  }

  x1 = calloc(Nc + len + 31, sizeof(uint32_t));
  if (!x1) {
    perror("calloc");
    return -1;
  }
  x2 = calloc(Nc + len + 31, sizeof(uint32_t));
  if (!x2) {
    free(x1);
    perror("calloc");
    return -1;
  }

  for (n = 0; n < 31; n++) {
    x2[n] = (seed >> n) & 0x1;
  }
  x1[0] = 1;

  for (n = 0; n < Nc + len; n++) {
    x1[n + 31] = (x1[n + 3] + x1[n]) & 0x1;
    x2[n + 31] = (x2[n + 3] + x2[n + 2] + +x2[n+1] + x2[n]) & 0x1;
  }

  for (n = 0; n < len; n++) {
    q->c[n] = (x1[n + Nc] + x2[n + Nc]) & 0x1;
  }

  free(x1);
  free(x2);

  return 0;
}

#endif

int srslte_sequence_LTE_pr(srslte_sequence_t *q, uint32_t len, uint32_t seed) {
  if (srslte_sequence_init(q, len)) {
    return SRSLTE_ERROR;
  }
  q->cur_len = len;
  srslte_sequence_set_LTE_pr(q, len, seed);
  srslte_bit_pack_vector(q->c, q->c_bytes, len);
  for (int i=0;i<len;i++) {
    q->c_float[i] = (1-2*q->c[i]);
    q->c_short[i] = (int16_t) q->c_float[i];
  }
  return SRSLTE_SUCCESS;
}

int srslte_sequence_init(srslte_sequence_t *q, uint32_t len) {
  if (q->c && len > q->max_len) {
    srslte_sequence_free(q);
  }
  if (!q->c) {
    q->c = srslte_vec_malloc(len * sizeof(uint8_t));
    if (!q->c) {
      return SRSLTE_ERROR;
    }
    q->c_bytes = srslte_vec_malloc(len * sizeof(uint8_t)/8+8);
    if (!q->c_bytes) {
      return SRSLTE_ERROR;
    }
    q->c_float = srslte_vec_malloc(len * sizeof(float));
    if (!q->c_float) {
      return SRSLTE_ERROR;
    }
    q->c_short = srslte_vec_malloc(len * sizeof(short));
    if (!q->c_short) {
      return SRSLTE_ERROR;
    }
    q->max_len = len;
  }
  return SRSLTE_SUCCESS;
}

void srslte_sequence_free(srslte_sequence_t *q) {
  if (q->c) {
    free(q->c);
  }
  if (q->c_bytes) {
    free(q->c_bytes);
  }
  if (q->c_float) {
    free(q->c_float);
  }
  if (q->c_short) {
    free(q->c_short);
  }
  bzero(q, sizeof(srslte_sequence_t));
}


