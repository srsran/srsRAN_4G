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
#include <strings.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#include "srslte/phy/utils/debug.h"

#include "srslte/phy/agc/agc.h"
#include "srslte/phy/utils/vector.h"
#include "srslte/phy/utils/debug.h"


int agc_init (agc_t *q) {
  bzero(q, sizeof(agc_t));
  agc_reset(q);
  return LIBLTE_SUCCESS;
}

void agc_free(agc_t *q) {
  bzero(q, sizeof(agc_t));
}

void agc_reset(agc_t *q) {
  q->bandwidth = AGC_DEFAULT_BW;
  q->lock = false; 
  q->gain = 1.0;
  q->y_out = 1.0; 
  q->isfirst = true; 
}

void agc_set_bandwidth(agc_t *q, float bandwidth) {
  q->bandwidth = bandwidth;
}

float agc_get_rssi(agc_t *q) {
  return 1.0/q->gain;
}

float agc_get_output_level(agc_t *q) {
  return q->y_out;
}

float agc_get_gain(agc_t *q) {
  return q->gain;
}


void agc_lock(agc_t *q, bool enable) {
  q->lock = enable;
}

void agc_process(agc_t *q, cf_t *input, cf_t *output, uint32_t len) {
  
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
