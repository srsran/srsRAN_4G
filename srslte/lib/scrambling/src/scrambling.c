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
#include <string.h>
#include <assert.h>
#include "srslte/utils/vector.h"
#include "srslte/scrambling/scrambling.h"

void srslte_scrambling_f(srslte_sequence_t *s, float *data) {
  srslte_scrambling_f_offset(s, data, 0, s->len);
}

void srslte_scrambling_f_offset(srslte_sequence_t *s, float *data, int offset, int len) {
  int i;  
  assert (len + offset <= s->len);

  for (i = 0; i < len; i++) {
    data[i] = data[i] * (1-2*s->c[i + offset]);
  }
}

void srslte_scrambling_c(srslte_sequence_t *s, cf_t *data) {
  srslte_scrambling_c_offset(s, data, 0, s->len);
}

void srslte_scrambling_c_offset(srslte_sequence_t *s, cf_t *data, int offset, int len) {
  int i;
  assert (len + offset <= s->len);

  for (i = 0; i < len; i++) {
    data[i] = data[i] * (1 - 2 * s->c[i + offset]);
  }
}

void srslte_scrambling_b(srslte_sequence_t *s, uint8_t *data) {
  srslte_scrambling_b_offset(s, data, 0, s->len);  
}

void srslte_scrambling_b_offset(srslte_sequence_t *s, uint8_t *data, int offset, int len) {
  int i;
  assert (len + offset <= s->len);
  // Do XOR on a word basis
  
  if (!(len%8)) {
    uint64_t *x = (uint64_t*) data; 
    uint64_t *y = (uint64_t*) &s->c[offset];
    for (int i=0;i<len/8;i++) {
      x[i] = (x[i] ^ y[i]);
    }
  } else if (!(len%4)) {
    uint32_t *x = (uint32_t*) data; 
    uint32_t *y = (uint32_t*) &s->c[offset];
    for (int i=0;i<len/4;i++) {
      x[i] = (x[i] ^ y[i]);
    }
  } else if (!(len%2)) {
    uint16_t *x = (uint16_t*) data; 
    uint16_t *y = (uint16_t*) &s->c[offset];
    for (int i=0;i<len/2;i++) {
      x[i] = (x[i] ^ y[i]);
    }
  } else {    
    for (i = 0; i < len; i++) {
      data[i] = (data[i] + s->c[i + offset]) % 2;
    }
  }
}

