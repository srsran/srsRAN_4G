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

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <assert.h>

#include "srslte/common/sequence.h"

#define Nc 1600


/*
 * Pseudo Random Sequence generation.
 * It follows the 3GPP Release 8 (LTE) 36.211
 * Section 7.2
 */
void srslte_sequence_set_LTE_pr(srslte_sequence_t *q, uint32_t seed) {
  int n;
  uint32_t *x1, *x2;

  x1 = calloc(Nc + q->len + 31, sizeof(uint32_t));
  if (!x1) {
    perror("calloc");
    return;
  }
  x2 = calloc(Nc + q->len + 31, sizeof(uint32_t));
  if (!x2) {
    free(x1);
    perror("calloc");
    return;
  }

  for (n = 0; n < 31; n++) {
    x2[n] = (seed >> n) & 0x1;
  }
  x1[0] = 1;

  for (n = 0; n < Nc + q->len; n++) {
    x1[n + 31] = (x1[n + 3] + x1[n]) & 0x1;
    x2[n + 31] = (x2[n + 3] + x2[n + 2] + +x2[n+1] + x2[n]) & 0x1;
  }

  for (n = 0; n < q->len; n++) {
    q->c[n] = (x1[n + Nc] + x2[n + Nc]) & 0x1;
  }

  free(x1);
  free(x2);
}

int srslte_sequence_LTE_pr(srslte_sequence_t *q, uint32_t len, uint32_t seed) {
  if (srslte_sequence_init(q, len)) {
    return SRSLTE_ERROR;
  }
  q->len = len;
  srslte_sequence_set_LTE_pr(q, seed);
  return SRSLTE_SUCCESS;
}

int srslte_sequence_init(srslte_sequence_t *q, uint32_t len) {
  if (q->c && (q->len != len)) {
    free(q->c);
  }
  if (!q->c) {
    q->c = malloc(len * sizeof(uint8_t));
    if (!q->c) {
      return SRSLTE_ERROR;
    }
    q->len = len;
  }
  return SRSLTE_SUCCESS;
}

void srslte_sequence_free(srslte_sequence_t *q) {
  if (q->c) {
    free(q->c);
  }
  bzero(q, sizeof(srslte_sequence_t));
}


