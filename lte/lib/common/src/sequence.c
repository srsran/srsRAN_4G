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


#include "lte/common/sequence.h"

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <assert.h>

#define Nc 1600


/*
 * Pseudo Random Sequence generation.
 * It follows the 3GPP Release 8 (LTE) 36.211
 * Section 7.2
 */
void generate_prs_c(sequence_t *q, unsigned int seed) {
  int n;
  unsigned int *x1;
  unsigned int *x2;

  x1 = calloc(Nc + q->len + 31, sizeof(unsigned int));
  if (!x1) {
    perror("calloc");
    return;
  }
  x2 = calloc(Nc + q->len + 31, sizeof(unsigned int));
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

int sequence_LTEPRS(sequence_t *q, int len, int seed) {
  if (sequence_init(q, len)) {
    return -1;
  }
  q->len = len;
  generate_prs_c(q, seed);
  return 0;
}

int sequence_init(sequence_t *q, int len) {
  if (q->c && (q->len != len)) {
    free(q->c);
  }
  if (!q->c) {
    q->c = malloc(len * sizeof(char));
    if (!q->c) {
      return -1;
    }
  }
  return 0;
}

void sequence_free(sequence_t *q) {
  if (q->c) {
    free(q->c);
  }
  bzero(q, sizeof(sequence_t));
}


