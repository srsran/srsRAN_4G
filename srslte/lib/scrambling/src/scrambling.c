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
#include <assert.h>
#include "srslte/scrambling/scrambling.h"

void srslte_scrambling_f(srslte_sequence_t *s, float *data) {
  srslte_scrambling_f_offset(s, data, 0, s->len);
}

void srslte_scrambling_f_offset(srslte_sequence_t *s, float *data, int offset, int len) {
  int i;  
  assert (len + offset <= s->len);

  for (i = 0; i < len; i++) {
    data[i] = data[i] * (1 - 2 * s->c[i + offset]);
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
  int i;

  for (i = 0; i < s->len; i++) {
    data[i] = (data[i] + s->c[i]) % 2;
  }
}

void srslte_scrambling_b_offset(srslte_sequence_t *s, uint8_t *data, int offset, int len) {
  int i;
  assert (len + offset <= s->len);
  for (i = 0; i < len; i++) {
    data[i] = (data[i] + s->c[i + offset]) % 2;
  }
}

/* As defined in 36.211 5.3.1 */
void srslte_scrambling_b_offset_pusch(srslte_sequence_t *s, uint8_t *data, int offset, int len) {
  int i;
  assert (len + offset <= s->len);
  for (i = 0; i < len; i++) {
    if (data[i] == 3) {
      data[i] = 1; 
    } else if (data[i] == 2) {
      if (i > 1) {
        data[i] = data[i-1];        
      }
    } else {
      data[i] = (data[i] + s->c[i + offset]) % 2;      
    }
  }
}

