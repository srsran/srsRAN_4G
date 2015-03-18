/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
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
#include <strings.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#include "srslte/utils/debug.h"

#include "srslte/agc/agc.h"
#include "srslte/utils/vector.h"
#include "srslte/utils/debug.h"


int srslte_agc_init (srslte_agc_t *q) {
  bzero(q, sizeof(srslte_agc_t));
  srslte_agc_reset(q);
  return SRSLTE_SUCCESS;
}

void srslte_agc_free(srslte_agc_t *q) {
  bzero(q, sizeof(srslte_agc_t));
}

void srslte_agc_reset(srslte_agc_t *q) {
  q->bandwidth = SRSLTE_AGC_DEFAULT_BW;
  q->lock = false; 
  q->gain = 1.0;
  q->y_out = 1.0; 
  q->isfirst = true; 
}

void srslte_agc_set_bandwidth(srslte_agc_t *q, float bandwidth) {
  q->bandwidth = bandwidth;
}

float srslte_agc_get_rssi(srslte_agc_t *q) {
  return 1.0/q->gain;
}

float srslte_agc_get_output_level(srslte_agc_t *q) {
  return q->y_out;
}

float srslte_agc_get_gain(srslte_agc_t *q) {
  return q->gain;
}


void srslte_agc_lock(srslte_agc_t *q, bool enable) {
  q->lock = enable;
}

void srslte_agc_process(srslte_agc_t *q, cf_t *input, cf_t *output, uint32_t len) {
  
  // Apply current gain to input signal 
  vec_sc_prod_cfc(input, q->gain, output, len);
  
  // compute output energy estimate
  float y = sqrtf(crealf(vec_dot_prod_conj_ccc(output, output, len))/len);
  
  if (q->isfirst) {
    q->y_out = y; 
    q->gain = 1/y;
    q->isfirst = false; 
  } else {
    q->y_out = (1-q->bandwidth) * q->y_out + q->bandwidth * y;
    if (!q->lock) {
      q->gain *= expf(-0.5*q->bandwidth*logf(q->y_out));
    }    
  }
  DEBUG("AGC gain: %.3f y_out=%.3f, y=%.3f\n", q->gain, q->y_out, y);
}
